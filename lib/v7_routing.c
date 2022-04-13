/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"
#include "bplib_routing.h"
#include "v7_mpool.h"
#include "v7_mpool_bblocks.h"
#include "v7_mpool_flows.h"
#include "v7_mpool_ref.h"
#include "v7.h"

#define BPLIB_ROUTING_SIGNATURE_BASE_INTF 0xe1ce32cf

#define BPLIB_INTF_AVAILABLE_FLAGS (BPLIB_INTF_STATE_OPER_UP | BPLIB_INTF_STATE_ADMIN_UP)

typedef struct bplib_routeentry
{
    bp_ipn_t    dest;
    bp_ipn_t    mask;
    bp_handle_t intf_id;
} bplib_routeentry_t;

typedef struct bplib_intf
{
    bp_handle_t               handle;
    uint32_t                  state_flags;
    bplib_mpool_ref_t         blk_refptr;
    bplib_route_action_func_t ingress_forwarder;
    bplib_route_action_func_t egress_forwarder;
    bplib_route_action_func_t event_handler;
    void                     *egress_arg;
    void                     *ingress_arg;
} bplib_intf_t;

struct bplib_routetbl
{
    uint32_t            max_routes;
    uint32_t            max_intfs;
    uint32_t            registered_routes;
    uint32_t            last_intf_serial;
    uintmax_t           routing_success_count;
    uintmax_t           routing_error_count;
    bplib_mpool_t      *pool;
    bplib_routeentry_t *route_tbl;
    bplib_intf_t       *intf_tbl;
};

static inline bplib_intf_t *bplip_route_lookup_intf(bplib_routetbl_t *tbl, bp_handle_t intf_id)
{
    uint32_t serial = bp_handle_to_serial(intf_id, BPLIB_HANDLE_INTF_BASE);
    if (serial > BPLIB_HANDLE_MAX_SERIAL)
    {
        return NULL;
    }
    return &tbl->intf_tbl[serial % tbl->max_intfs];
}

static inline const bplib_intf_t *bplip_route_lookup_intf_const(const bplib_routetbl_t *tbl, bp_handle_t intf_id)
{
    uint32_t serial = bp_handle_to_serial(intf_id, BPLIB_HANDLE_INTF_BASE);
    if (serial > BPLIB_HANDLE_MAX_SERIAL)
    {
        return NULL;
    }
    return &tbl->intf_tbl[serial % tbl->max_intfs];
}

static inline bool bplib_route_intf_match(const bplib_intf_t *ifp, bp_handle_t intf_id)
{
    return (ifp != NULL && bp_handle_equal(ifp->handle, intf_id));
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int bplib_route_ingress_to_parent(bplib_routetbl_t *tbl, bplib_mpool_block_t *flow_block, void *ingress_arg)
{
    bplib_mpool_flow_t *curr_flow;
    bplib_mpool_flow_t *next_flow;
    uint32_t            queue_depth;

    /* This implements a simple ingress for sub-interfaces (of any kind) where the ingress
     * of the sub-interface nodes is simply funneled into the parent */
    curr_flow = bplib_mpool_flow_cast(flow_block);
    if (curr_flow == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to cast flow block\n");
        return -1;
    }

    next_flow = bplib_mpool_flow_cast(bplib_mpool_dereference(curr_flow->parent));
    if (next_flow == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to cast flow parent block\n");
        return -1;
    }

    if (bplib_mpool_subq_may_push(&next_flow->ingress))
    {
        /*
         * Entire contents moved as a batch.  NOTE- this may actually overfill the destination queue,
         * because bplib_mpool_subq_may_push() returns true if one entry can be added, but
         * this may add more than one.  Currently this shouldn't be a big problem, at least not
         * big enough to warrant moving the entries one-by-one.
         */
        queue_depth = bplib_mpool_subq_move_all(&next_flow->ingress, &curr_flow->ingress);
        bplib_mpool_flow_mark_active(tbl->pool, curr_flow->parent);
    }
    else
    {
        queue_depth = 0;
    }

    return queue_depth;
}

void bplib_route_ingress_route_single_bundle(bplib_routetbl_t *tbl, bplib_mpool_block_t *pblk)
{
    bplib_mpool_bblock_primary_t *pri_block;
    bp_primary_block_t           *pri;
    bp_ipn_addr_t                 dest_addr;
    bp_handle_t                   next_hop;
    uint32_t                      req_flags;
    uint32_t                      flag_mask;

    /* is this routable right now? */
    pri_block = bplib_mpool_bblock_primary_cast(pblk);
    if (pri_block != NULL)
    {
        pri = bplib_mpool_bblock_primary_get_logical(pri_block);

        v7_get_eid(&dest_addr, &pri->destinationEID);

        /* the next hop must be "up" (both administratively and operationally) to be valid */
        /* Also, if this bundle has not yet been stored, and the delivery policy wants some form of acknowledgement,
         * then it should be directed to an interface that is storage-capable, even if the outgoing CLA is up */
        req_flags = BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP;
        flag_mask = req_flags;

        if (bp_handle_is_valid(pri_block->delivery_data.storage_intf_id))
        {
            /* the bundle is already stored, so next hop should not be another storage */
            flag_mask |= BPLIB_INTF_STATE_STORAGE;
        }
        else if (pri_block->delivery_data.delivery_policy != bplib_policy_delivery_none)
        {
            /* not yet stored and needs to be, so next hop must be a storage */
            flag_mask |= BPLIB_INTF_STATE_STORAGE;
            req_flags |= BPLIB_INTF_STATE_STORAGE;
        }
        next_hop = bplib_route_get_next_intf_with_flags(tbl, dest_addr.node_number, req_flags, flag_mask);
        if (bp_handle_is_valid(next_hop) && bplib_route_push_egress_bundle(tbl, next_hop, pblk) == 0)
        {
            /* successfully routed */
            ++tbl->routing_success_count;
            pblk = NULL;
        }
    }

    /* if qblk is still set to non-null at this point, it means the block was not routable */
    if (pblk != NULL)
    {
        /* this should never happen, must discard the block because there is nowhere to put it */
        bplib_mpool_recycle_block(tbl->pool, pblk);
        ++tbl->routing_error_count;
    }
}

int bplib_route_ingress_baseintf_forwarder(bplib_routetbl_t *tbl, bplib_mpool_block_t *flow_block, void *forward_arg)
{
    bplib_mpool_block_t *qblk;
    bplib_mpool_flow_t  *flow;
    int                  forward_count;

    flow = bplib_mpool_flow_cast(flow_block);
    if (flow == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to cast flow block\n");
        return -1;
    }

    forward_count = 0;
    while (true)
    {
        qblk = bplib_mpool_subq_pull_single(&flow->ingress);
        if (qblk == NULL)
        {
            /* no more bundles */
            break;
        }

        /* Increment the counter based off items shifted from the input queue -
         * even if it gets dropped after this (hopefully not) it still counts
         * as something moved/changed by this action */
        ++forward_count;

        /*
         * This call always puts the block somewhere -
         * if its unroutable, the block will be put into the recycle bin.
         */
        bplib_route_ingress_route_single_bundle(tbl, qblk);
    }

    /* This should return 0 if it did no work and no errors.
     * Should return >0 if some work was done */
    return forward_count;
}

bplib_mpool_t *bplib_route_get_mpool(const bplib_routetbl_t *tbl)
{
    return tbl->pool;
}

bplib_routetbl_t *bplib_route_alloc_table(uint32_t max_routes, uint32_t max_intfs, size_t cache_mem_size)
{
    size_t            complete_size;
    size_t            align;
    size_t            route_offset;
    size_t            intf_offset;
    size_t            bplib_mpool_offset;
    uint8_t          *mem_ptr;
    bplib_routetbl_t *tbl_ptr;
    struct routeentry_align
    {
        uint8_t            byte;
        bplib_routeentry_t route_tbl_offset;
    };
    struct intf_align
    {
        uint8_t      byte;
        bplib_intf_t intf_tbl_offset;
    };

    if (max_routes == 0 || max_intfs == 0)
    {
        return NULL;
    }

    complete_size = sizeof(bplib_routetbl_t);

    align         = offsetof(struct routeentry_align, route_tbl_offset) - 1;
    complete_size = (complete_size + align) & ~align;
    route_offset  = complete_size;
    complete_size += sizeof(bplib_routeentry_t) * max_routes;

    align         = offsetof(struct intf_align, intf_tbl_offset) - 1;
    complete_size = (complete_size + align) & ~align;
    intf_offset   = complete_size;
    complete_size += sizeof(bplib_intf_t) * max_intfs;

    align = sizeof(void *) - 1;
    align |= sizeof(uintmax_t) - 1;
    complete_size      = (complete_size + align) & ~align;
    bplib_mpool_offset = complete_size;
    complete_size += cache_mem_size;

    complete_size = (complete_size + align) & ~align;

    tbl_ptr = (bplib_routetbl_t *)bplib_os_calloc(complete_size);
    mem_ptr = (uint8_t *)tbl_ptr;

    if (tbl_ptr != NULL)
    {
        tbl_ptr->pool = bplib_mpool_create(mem_ptr + bplib_mpool_offset, complete_size - bplib_mpool_offset);
        if (tbl_ptr->pool == NULL)
        {
            bplib_os_free(tbl_ptr);
            tbl_ptr = NULL;
        }
    }

    if (tbl_ptr != NULL)
    {
        tbl_ptr->max_routes = max_routes;
        tbl_ptr->max_intfs  = max_intfs;
        tbl_ptr->route_tbl  = (void *)(mem_ptr + route_offset);
        tbl_ptr->intf_tbl   = (void *)(mem_ptr + intf_offset);
        --tbl_ptr->last_intf_serial;
    }

    return tbl_ptr;
}

bp_handle_t bplib_route_register_generic_intf(bplib_routetbl_t *tbl, bp_handle_t parent_intf_id,
                                              bplib_mpool_ref_t blkref)
{
    uint32_t            count;
    uint32_t            next_intf_serial;
    uint32_t            max_serial;
    bplib_mpool_flow_t *flow;
    bplib_intf_t       *parent_ifp;
    bplib_intf_t       *ifp;
    bp_handle_t         result;

    result = BP_INVALID_HANDLE;
    flow   = bplib_mpool_flow_cast(bplib_mpool_dereference(blkref));
    if (flow == NULL)
    {
        bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed to cast flow block\n");
        return result;
    }

    if (!bp_handle_is_valid(parent_intf_id))
    {
        /* this will be registered as a base intf */
        parent_ifp = NULL;
    }
    else
    {
        parent_ifp = bplip_route_lookup_intf(tbl, parent_intf_id);
        if (!bplib_route_intf_match(parent_ifp, parent_intf_id))
        {
            /* this is an error */
            bplog(NULL, BP_FLAG_DIAGNOSTIC, "Parent intf not valid\n");
            return result;
        }
    }

    next_intf_serial = tbl->last_intf_serial;
    max_serial       = 0xFFFFFF / tbl->max_intfs;
    max_serial *= tbl->max_intfs;
    count = tbl->max_intfs;
    ifp   = NULL;
    while (true)
    {
        if (count == 0)
        {
            ifp = NULL;
            break;
        }
        ++next_intf_serial;
        if (next_intf_serial >= max_serial)
        {
            next_intf_serial = 0;
        }
        --count;

        ifp = &tbl->intf_tbl[next_intf_serial % tbl->max_intfs];
        if (!bp_handle_is_valid(ifp->handle))
        {
            tbl->last_intf_serial = next_intf_serial;

            ifp->blk_refptr = bplib_mpool_ref_duplicate(blkref);
            result          = bp_handle_from_serial(next_intf_serial, BPLIB_HANDLE_INTF_BASE);
            ifp->handle     = result;

            flow->external_id = result;
            if (parent_ifp != NULL)
            {
                flow->parent = bplib_mpool_ref_duplicate(parent_ifp->blk_refptr);
            }
            else
            {
                flow->parent = NULL;
            }

            break;
        }
    }

    return result;
}

int bplib_route_register_handler_impl(bplib_routetbl_t *tbl, bp_handle_t intf_id, size_t func_position,
                                      bplib_route_action_func_t new_func, size_t arg_position, void *arg)
{
    bplib_intf_t              *ifp;
    bplib_route_action_func_t *func_ptr;
    void                     **arg_ptr;

    ifp = bplip_route_lookup_intf(tbl, intf_id);
    if (!bplib_route_intf_match(ifp, intf_id))
    {
        /* Not a valid handle */
        return -1;
    }

    func_ptr = (bplib_route_action_func_t *)((uint8_t *)ifp + func_position);

    if (*func_ptr == new_func)
    {
        /* already registered */
        return 0;
    }

    /* One or the other must be NULL or else this is an invalid registration */
    if (*func_ptr != NULL && new_func != NULL)
    {
        return -1;
    }

    /* OK, now set it to the new value */
    *func_ptr = new_func;

    if (arg_position != 0)
    {
        arg_ptr  = (void **)((uint8_t *)ifp + arg_position);
        *arg_ptr = arg;
    }

    return 0;
}

int bplib_route_register_forward_ingress_handler(bplib_routetbl_t *tbl, bp_handle_t intf_id,
                                                 bplib_route_action_func_t ingress, void *ingress_arg)
{
    return bplib_route_register_handler_impl(tbl, intf_id, offsetof(bplib_intf_t, ingress_forwarder), ingress,
                                             offsetof(bplib_intf_t, ingress_arg), ingress_arg);
}

int bplib_route_register_forward_egress_handler(bplib_routetbl_t *tbl, bp_handle_t intf_id,
                                                bplib_route_action_func_t egress, void *egress_arg)
{
    return bplib_route_register_handler_impl(tbl, intf_id, offsetof(bplib_intf_t, egress_forwarder), egress,
                                             offsetof(bplib_intf_t, egress_arg), egress_arg);
}

int bplib_route_register_event_handler(bplib_routetbl_t *tbl, bp_handle_t intf_id, bplib_route_action_func_t event)
{
    return bplib_route_register_handler_impl(tbl, intf_id, offsetof(bplib_intf_t, event_handler), event, 0, NULL);
}

bplib_mpool_ref_t bplib_route_get_intf_controlblock(bplib_routetbl_t *tbl, bp_handle_t intf_id)
{
    bplib_intf_t *ifp;

    ifp = bplip_route_lookup_intf(tbl, intf_id);
    if (!bplib_route_intf_match(ifp, intf_id))
    {
        /* Not a valid handle */
        return NULL;
    }

    return bplib_mpool_ref_duplicate(ifp->blk_refptr);
}

void bplib_route_release_intf_controlblock(bplib_routetbl_t *tbl, bplib_mpool_ref_t refptr)
{
    bplib_mpool_ref_release(tbl->pool, refptr);
}

int bplib_route_del_intf(bplib_routetbl_t *tbl, bp_handle_t intf_id)
{
    uint32_t            pos;
    bplib_routeentry_t *rp;
    bplib_intf_t       *ifp;

    ifp = bplip_route_lookup_intf(tbl, intf_id);
    if (!bplib_route_intf_match(ifp, intf_id))
    {
        /* Not a valid handle */
        return -1;
    }

    /* before it can be deleted, should ensure it is not referenced */
    for (pos = 0; pos < tbl->registered_routes; ++pos)
    {
        rp = &tbl->route_tbl[pos];
        if (bp_handle_equal(rp->intf_id, intf_id))
        {
            --tbl->registered_routes;

            /* close the gap, if this left one */
            if (pos < tbl->registered_routes)
            {
                memmove(&rp[0], &rp[1], sizeof(*rp) * (tbl->registered_routes - pos));
            }
            break;
        }
    }

    if (ifp->blk_refptr)
    {
        bplib_mpool_ref_release(tbl->pool, ifp->blk_refptr);
    }

    /* just wipe it */
    memset(ifp, 0, sizeof(*ifp));
    return 0;
}

bp_handle_t bplib_route_get_next_intf_with_flags(const bplib_routetbl_t *tbl, bp_ipn_t dest, uint32_t req_flags,
                                                 uint32_t flag_mask)
{
    uint32_t            pos;
    bplib_routeentry_t *rp;
    bp_handle_t         intf;
    const bplib_intf_t *ifp;
    uint32_t            intf_flags;

    intf = BP_INVALID_HANDLE;
    for (pos = 0; pos < tbl->registered_routes; ++pos)
    {
        rp = &tbl->route_tbl[pos];
        if (((rp->dest ^ dest) & rp->mask) == 0)
        {
            intf_flags = ~req_flags;
            if (flag_mask != 0)
            {
                ifp = bplip_route_lookup_intf_const(tbl, rp->intf_id);
                if (bplib_route_intf_match(ifp, rp->intf_id))
                {
                    intf_flags = ifp->state_flags;
                }
            }
            if ((intf_flags & flag_mask) == req_flags)
            {
                intf = rp->intf_id;
                break;
            }
        }
    }

    return intf;
}

bp_handle_t bplib_route_get_next_avail_intf(const bplib_routetbl_t *tbl, bp_ipn_t dest)
{
    return bplib_route_get_next_intf_with_flags(tbl, dest, BPLIB_INTF_AVAILABLE_FLAGS, BPLIB_INTF_AVAILABLE_FLAGS);
}

int bplib_route_push_ingress_bundle(const bplib_routetbl_t *tbl, bp_handle_t intf_id, bplib_mpool_block_t *cb)
{
    const bplib_intf_t *ifp;
    bplib_mpool_flow_t *flow;
    int                 status;

    status = -1;
    ifp    = bplip_route_lookup_intf_const(tbl, intf_id);
    if (bplib_route_intf_match(ifp, intf_id))
    {
        flow = bplib_mpool_flow_cast(bplib_mpool_dereference(ifp->blk_refptr));
        if (flow != NULL && bplib_mpool_subq_may_push(&flow->ingress))
        {
            bplib_mpool_subq_push_single(&flow->ingress, cb);
            bplib_mpool_flow_mark_active(tbl->pool, ifp->blk_refptr);
            status = 0;
        }
    }

    return status;
}

int bplib_route_push_egress_bundle(const bplib_routetbl_t *tbl, bp_handle_t intf_id, bplib_mpool_block_t *cb)
{
    const bplib_intf_t *ifp;
    bplib_mpool_flow_t *flow;
    int                 status;

    status = -1;
    ifp    = bplip_route_lookup_intf_const(tbl, intf_id);
    if (bplib_route_intf_match(ifp, intf_id))
    {
        flow = bplib_mpool_flow_cast(bplib_mpool_dereference(ifp->blk_refptr));
        if (flow != NULL && bplib_mpool_subq_may_push(&flow->egress))
        {
            bplib_mpool_subq_push_single(&flow->egress, cb);
            bplib_mpool_flow_mark_active(tbl->pool, ifp->blk_refptr);
            status = 0;
        }
    }

    return status;
}

int bplib_route_add(bplib_routetbl_t *tbl, bp_ipn_t dest, bp_ipn_t mask, bp_handle_t intf_id)
{
    uint32_t            pos;
    uint32_t            insert_pos;
    bplib_routeentry_t *rp;

    if (tbl->registered_routes >= tbl->max_routes)
    {
        return -1;
    }

    /* Mask check: should have MSB's set, no gaps */
    if (((~mask + 1) & (~mask)) != 0)
    {
        return -1;
    }

    /* Find the position, the sequence should go from most specific to least specific mask */
    insert_pos = 0;
    for (pos = 0; pos < tbl->registered_routes; ++pos)
    {
        rp = &tbl->route_tbl[pos];
        if (rp->mask == mask && rp->dest == dest && bp_handle_equal(rp->intf_id, intf_id))
        {
            break;
        }
        if ((rp->mask & mask) == mask)
        {
            ++insert_pos; /* must come (at least) after this route */
        }
    }

    if (pos < tbl->registered_routes)
    {
        /* duplicate route */
        return -1;
    }

    /* If necessary, shift entries back to make a gap.
     * This is somewhat expensive, but route add/remove probably does not
     * happen that often */
    rp = &tbl->route_tbl[insert_pos];
    if (insert_pos < tbl->registered_routes)
    {
        memmove(&rp[1], &rp[0], sizeof(*rp) * (tbl->registered_routes - insert_pos));
    }

    rp->dest    = dest;
    rp->mask    = mask;
    rp->intf_id = intf_id;

    ++tbl->registered_routes;

    return 0;
}

int bplib_route_del(bplib_routetbl_t *tbl, bp_ipn_t dest, bp_ipn_t mask, bp_handle_t intf_id)
{
    uint32_t            pos;
    bplib_routeentry_t *rp;

    if (tbl->registered_routes == 0)
    {
        return -1;
    }

    /* Find the position, the sequence should go from most specific to least specific */
    for (pos = 0; pos < tbl->registered_routes; ++pos)
    {
        rp = &tbl->route_tbl[pos];
        if (rp->mask == mask && rp->dest == dest && bp_handle_equal(rp->intf_id, intf_id))
        {
            break;
        }
    }

    if (pos >= tbl->registered_routes)
    {
        /* route not found */
        return -1;
    }

    --tbl->registered_routes;

    /* If this was in the middle, close the gap */
    rp = &tbl->route_tbl[pos];
    if (pos < tbl->registered_routes)
    {
        memmove(&rp[0], &rp[1], sizeof(*rp) * (tbl->registered_routes - pos));
    }

    return 0;
}

void bplib_route_handle_intf_statechange_event(bplib_routetbl_t *tbl, bp_handle_t intf_id, bool is_up)
{
    bplib_generic_event_t event;
    bplib_routeentry_t   *rp;
    bplib_intf_t         *other_ifp;
    uint32_t              rpos;
    uint32_t              ipos;

    if (is_up)
    {
        event.intf_state.event_type = bplib_event_type_interface_up;
    }
    else
    {
        event.intf_state.event_type = bplib_event_type_interface_down;
    }
    event.intf_state.intf_id = intf_id;

    /* foreach intf - find intfs that are changing availablity state */
    for (ipos = 0; ipos < tbl->max_intfs; ++ipos)
    {
        other_ifp = &tbl->intf_tbl[ipos];
        if (bp_handle_is_valid(other_ifp->handle) && other_ifp->event_handler != NULL)
        {
            other_ifp->event_handler(tbl, bplib_mpool_dereference(other_ifp->blk_refptr), &event);
        }
    }

    if (is_up)
    {
        event.route_state.event_type = bplib_event_type_route_up;
    }
    else
    {
        event.route_state.event_type = bplib_event_type_route_down;
    }

    /* foreach route - find routes that are changing usability state */
    for (rpos = 0; rpos < tbl->registered_routes; ++rpos)
    {
        rp = &tbl->route_tbl[rpos];
        if (bp_handle_equal(rp->intf_id, intf_id))
        {
            event.route_state.dest = rp->dest;
            event.route_state.mask = rp->mask;

            for (ipos = 0; ipos < tbl->max_intfs; ++ipos)
            {
                other_ifp = &tbl->intf_tbl[ipos];
                if (bp_handle_is_valid(other_ifp->handle) && other_ifp->event_handler != NULL)
                {
                    other_ifp->event_handler(tbl, bplib_mpool_dereference(other_ifp->blk_refptr), &event);
                }
            }
        }
    }
}

int bplib_route_intf_set_flags(bplib_routetbl_t *tbl, bp_handle_t intf_id, uint32_t flags)
{
    bplib_intf_t *ifp;
    uint32_t      was_avail_flags;

    ifp = bplip_route_lookup_intf(tbl, intf_id);
    if (!bplib_route_intf_match(ifp, intf_id))
    {
        return -1;
    }

    was_avail_flags = ~ifp->state_flags & BPLIB_INTF_AVAILABLE_FLAGS;
    ifp->state_flags |= flags;

    /* detect transition from down (unavailable) -> up (available) */
    /* storage service(s) need to be informed of this change */
    if (was_avail_flags != 0 && (~ifp->state_flags & BPLIB_INTF_AVAILABLE_FLAGS) == 0)
    {
        bplib_route_handle_intf_statechange_event(tbl, intf_id, true);
    }

    return 0;
}

int bplib_route_intf_unset_flags(bplib_routetbl_t *tbl, bp_handle_t intf_id, uint32_t flags)
{
    bplib_intf_t *ifp;
    uint32_t      was_avail_flags;

    ifp = bplip_route_lookup_intf(tbl, intf_id);
    if (!bplib_route_intf_match(ifp, intf_id))
    {
        return -1;
    }

    was_avail_flags = ~ifp->state_flags & BPLIB_INTF_AVAILABLE_FLAGS;
    ifp->state_flags &= ~flags;

    /* detect transition from up (available) -> down (unavailable) */
    /* storage service(s) need to be informed of this change */
    if (was_avail_flags == 0 && (~ifp->state_flags & BPLIB_INTF_AVAILABLE_FLAGS) != 0)
    {
        bplib_route_handle_intf_statechange_event(tbl, intf_id, false);
    }

    return 0;
}

void bplib_route_forward_intf(void *arg, bplib_mpool_block_t *flow_block)
{
    bplib_routetbl_t   *tbl;
    bplib_intf_t       *ifp;
    bplib_mpool_flow_t *flow;

    tbl = arg;

    flow = bplib_mpool_flow_cast(flow_block);
    if (flow == NULL)
    {
        return;
    }

    ifp = bplip_route_lookup_intf(tbl, flow->external_id);
    if (!bplib_route_intf_match(ifp, flow->external_id))
    {
        return;
    }

    /* forward all egress */
    if (ifp->egress_forwarder != NULL && bplib_mpool_subq_may_pull(&flow->egress))
    {
        ifp->egress_forwarder(tbl, flow_block, ifp->egress_arg);
    }

    /* forward all ingress */
    if (ifp->ingress_forwarder != NULL && bplib_mpool_subq_may_pull(&flow->ingress))
    {
        ifp->ingress_forwarder(tbl, flow_block, ifp->ingress_arg);
    }
}

void bplib_route_do_maintenance(bplib_routetbl_t *tbl)
{
    uint32_t              pos;
    bplib_intf_t         *ifp;
    bplib_generic_event_t poll_event;

    /* poll each interface, may move items to other queues based on time */
    poll_event.event_type = bplib_event_type_poll_interval;
    for (pos = 0; pos < tbl->max_intfs; ++pos)
    {
        ifp = &tbl->intf_tbl[pos];
        if (bp_handle_is_valid(ifp->handle) && ifp->event_handler != NULL)
        {
            /* poll intf  */
            ifp->event_handler(tbl, bplib_mpool_dereference(ifp->blk_refptr), &poll_event);
        }
    }

    /* now forward any bundles between interfaces, based on active flows */
    while (true)
    {
        /* keep calling bplib_mpool_flow_process_active() until it returns 0 meaning no work was done */
        if (bplib_mpool_flow_process_active(tbl->pool, bplib_route_forward_intf, tbl) <= 0)
        {
            break;
        }
    }

    /* now complete the recycle process for any blocks released by above */
    bplib_mpool_maintain(tbl->pool);
}