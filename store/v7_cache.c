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
#include "v7_cache.h"
#include "v7.h"
#include "v7_rbtree.h"

/*
 * Randomly-chosen 32-bit static values that can be put into
 * data structures to help positively identify those structs later.
 *
 * Not strictly necessary, but improves debug capabilities, helping
 * to catch any typecasting errors.  This should be employed whenever
 * using the "generic data" facility in the mempool implementation,
 * because the data type is always void*, it needs another way
 * to confirm the actual content is the intended type.
 */
#define BPLIB_STORE_SIGNATURE_STATE 0x683359a7
#define BPLIB_STORE_SIGNATURE_ENTRY 0xf223fff9
#define BPLIB_STORE_SIGNATURE_QUEUE 0x30241224

#define BP_BUNDLE_FLAG_WITHIN_LIFETIME   0x01
#define BP_BUNDLE_FLAG_AWAITING_CUSTODY  0x02

typedef struct bplib_store_cache_state
{
    uint32_t signature;
    bplib_mpool_t *parent_pool;
    bplib_mpool_flow_t *parent_flow;

    /*
     * pending_list holds bundle refs that are currently actionable in some way,
     * such as one of its timers getting reached, or the state flags changed.
     *
     * This may simply be re-classifying it into one of the other lists or indices.
     */
    bplib_mpool_block_t pending_list;

    /*
     * expired_list holds bundle refs that have reached expiration time and
     * may be recycled, pending its removal from the other secondary indices.
     */
    bplib_mpool_block_t expired_list;

    /*
     * idle_list holds the items which do not fit into the other two lists.
     * They are not currently actionable, and placed here for holding.
     *
     * NOTE: this is just a sequential flat list for simplicity.  It can be
     * iterated, but that does not scale well.  One of the secondary indices
     * may be used for more efficient lookup.
     */
    bplib_mpool_block_t idle_list;

    bplib_rbt_root_t dest_eid_index;
    bplib_rbt_root_t time_index;

} bplib_store_cache_state_t;

/*
* There are generally two ways for a bundle to be pulled from storage:
* 1. The outgoing CLA comes up, and the bundle is to be sent through it
*     -> this is based on destination EID, and matching routes in the route tbl.
*        it is triggered based on a state change event from a CLA interface
* 2. The bundle has been sitting for some amount of time
*     -> this is triggered based on a periodic polling/maintenance call
*        may be based on the expiration time or retransmit time
*
* Both of these tasks need reasonably efficient lookups - cannot be sequential
* searches through a list.  A dedicated index is created for each, using the
* R-B tree facility.
*
* Note that the R-B tree mechanism does not allow for duplicate entries, but
* there absolutely can be multiple entries in this use-case, so we need to
* use an intermediate queue here.
*/
typedef struct bplib_store_cache_queue
{
    bplib_rbt_link_t rbt_link;          /* must be first */
    bplib_mpool_block_t bundle_list;

} bplib_store_cache_queue_t;

typedef struct bplib_store_cache_entry
{
    uint32_t flags;
    bplib_mpool_refptr_t *refptr;
    uint64_t expire_time;
    uint64_t retx_time;
    bplib_mpool_block_t time_link;
    bplib_mpool_block_t destination_link;
} bplib_store_cache_entry_t;

typedef enum
{
    bplib_store_cache_construct_state_init,
    bplib_store_cache_construct_state_alloc,
    bplib_store_cache_construct_state_lock_intf,
    bplib_store_cache_construct_state_setup_cb,
    bplib_store_cache_construct_state_register_egress,
    bplib_store_cache_construct_state_register_event,
    bplib_store_cache_construct_state_attach_cb,
    bplib_store_cache_construct_state_unlock_intf,
    bplib_store_cache_construct_state_done
} bplib_store_cache_construct_state_t;

typedef struct bplib_store_cache_construct_transaction
{
    bplib_store_cache_construct_state_t construct_state;
    bplib_mpool_block_t *storage_block;
    bplib_mpool_block_t *parent_intf_block;
    bplib_store_cache_state_t *state_ptr;
} bplib_store_cache_construct_transaction_t;


/* Allows reconsitition of the entry struct from an RBT link pointer */
static inline bplib_store_cache_queue_t *bplib_store_cache_queue_from_rbt_link(const bplib_rbt_link_t *link)
{
    /* this relies on "rbt_link" being the first entry in the struct */
    return (bplib_store_cache_queue_t *)((void*)link);
}

/* Allows reconsitition of the entry struct from an time tree link pointer */
static inline bplib_store_cache_queue_t *bplib_store_cache_queue_from_bundle_list(const bplib_mpool_block_t *list)
{
    return (bplib_store_cache_queue_t *)((uint8_t*)list - offsetof(bplib_store_cache_queue_t, bundle_list));
}

/* Allows reconsitition of the entry struct from an time tree link pointer */
static inline bplib_store_cache_entry_t *bplib_store_cache_entry_from_time_link(const bplib_mpool_block_t *link)
{
    return (bplib_store_cache_entry_t *)((uint8_t*)link - offsetof(bplib_store_cache_entry_t, time_link));
}

/* Allows reconsitition of the entry struct from an destination tree link pointer */
static inline bplib_store_cache_entry_t *bplib_store_cache_entry_from_destination_link(const bplib_mpool_block_t *link)
{
    return (bplib_store_cache_entry_t *)((uint8_t*)link - offsetof(bplib_store_cache_entry_t, destination_link));
}

static bplib_store_cache_state_t *bplib_store_cache_get_state(bplib_mpool_block_t *intf_block)
{
    bplib_mpool_baseintf_t *baseintf;
    bplib_mpool_flow_t *flow;
    bplib_mpool_generic_data_t *gd;
    bplib_store_cache_state_t *state;

    baseintf = bplib_mpool_cast_baseintf(intf_block);
    flow = bplib_mpool_cast_flow(intf_block);
    if (baseintf == NULL || flow == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bad intf_block\n", __func__);
        return NULL;
    }

    gd = bplib_mpool_cast_generic_data(baseintf->service_specific_block, BPLIB_STORE_SIGNATURE_STATE);
    if (gd == NULL || sizeof(bplib_store_cache_state_t) != bplib_mpool_get_generic_data_size(gd))
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): storage_block incorrect for bplib_store_cache_state_t\n", __func__);
        return NULL;
    }

    state = bplib_mpool_get_generic_data_ptr(gd);
    if (state->parent_flow != flow)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): embedded ptr mismatch\n", __func__);
        return NULL;
    }

    return state;
}

int bplib_store_cache_handle_ref_recycle(bplib_mpool_block_t *rblk, void *arg)
{
    bplib_mpool_block_t *sblk;
    bplib_store_cache_state_t *state;
    bplib_mpool_primary_block_t *pri_block;
    bplib_store_cache_entry_t *store_entry;
    bplib_mpool_generic_data_t *store_block;

    state = arg;

    pri_block = bplib_mpool_cast_primary(rblk);
    if (pri_block != NULL)
    {
        assert(pri_block->delivery_data.committed_storage_id != 0);

        /* The storage ID is the pointer to the storage entry block saved as an integer */
        sblk = (bplib_mpool_block_t *)pri_block->delivery_data.committed_storage_id;
        store_block = bplib_mpool_cast_generic_data(sblk, BPLIB_STORE_SIGNATURE_ENTRY);
        if (store_block != NULL)
        {
            store_entry = bplib_mpool_get_generic_data_ptr(store_block);

            if (bp_handle_is_valid(pri_block->delivery_data.egress_intf_id))
            {
                /* bundle was successfully forwarded to a CLA; can forget about it until retransmit time */
                /* If not doing full custody tracking, then the egress CLA is now considered the implicit custodian */
                if (pri_block->delivery_data.delivery_policy != bplib_policy_delivery_custody_tracking)
                {
                    store_entry->flags &= ~BP_BUNDLE_FLAG_AWAITING_CUSTODY;
                }
            }
        }

        /*
         * always put back into pending_list, this will re-evalute its current
         * state and reclassify it as appropriate
         */
        bplib_mpool_extract_node(sblk);
        bplib_mpool_insert_before(&state->pending_list, sblk);
    }

    return BP_SUCCESS;
}

void bplib_store_cache_remove_from_subindex(bplib_mpool_t *pool, bplib_rbt_root_t *index_root, bplib_mpool_block_t *index_link)
{
    bplib_store_cache_queue_t *store_queue;
    bplib_mpool_block_t *list_ptr;
    bplib_mpool_block_t *qblk;

    /* grab the list ptr before removal (in case it becomes empty by this) */
    list_ptr = bplib_mpool_get_next_block(index_link);
    bplib_mpool_extract_node(index_link);

    /* If this arrived back at an empty head node, that means this was the last entry in that subq,
     * which then needs to be removed from its parent index tree */
    if (list_ptr != index_link && bplib_mpool_is_empty_list_head(list_ptr))
    {
        store_queue = bplib_store_cache_queue_from_bundle_list(list_ptr);

        if (bplib_rbt_get_key_value(&store_queue->rbt_link) != 0)
        {
            bplib_rbt_extract_node(index_root, &store_queue->rbt_link);
        }

        qblk = bplib_mpool_get_generic_block_from_pointer(store_queue, BPLIB_STORE_SIGNATURE_QUEUE);
        bplib_mpool_recycle_block(pool, qblk);
    }
}

void bplib_store_cache_add_to_subindex(bplib_mpool_t *pool, bplib_rbt_root_t *index_root, bplib_mpool_block_t *index_link, bp_val_t index_val)
{
    bplib_store_cache_queue_t *store_queue;
    bplib_mpool_block_t *tblk;
    bplib_rbt_link_t *tlink;
    bplib_mpool_generic_data_t *gd_block;

    tlink = bplib_rbt_search(index_val, index_root);
    if (tlink != NULL)
    {
        /* not the first time this was seen, add to the existing subq */
        store_queue = bplib_store_cache_queue_from_rbt_link(tlink);
    }
    else
    {
        /* first occurrance of this particular index, need to create a subq block */
        tblk = bplib_mpool_alloc_generic_block(pool, BPLIB_STORE_SIGNATURE_QUEUE, sizeof(bplib_store_cache_queue_t));
        gd_block = bplib_mpool_cast_generic_data(tblk, BPLIB_STORE_SIGNATURE_QUEUE);
        if (gd_block != NULL)
        {
            store_queue = bplib_mpool_get_generic_data_ptr(gd_block);

            bplib_mpool_link_reset(&store_queue->bundle_list, bplib_mpool_blocktype_head);

            /* This should always work, because it was already known _not_ to be a duplicate */
            bplib_rbt_insert_value(index_val, index_root, &store_queue->rbt_link);
        }
        else
        {
            /* must be out of memory */
            store_queue = NULL;
        }
    }

    if (store_queue != NULL)
    {
        bplib_mpool_insert_before(&store_queue->bundle_list, index_link);
    }
}


void bplib_store_cache_do_transmit(bplib_store_cache_state_t *state, bplib_store_cache_entry_t *store_entry, bplib_mpool_primary_block_t *pri_block)
{
    bplib_mpool_block_t *rblk;
    uint64_t action_time;

    /* Set egress intf to invalid - which can be used as a marker for items which are
     * blocked from egress (it will be set valid again when it actually goes out, so if
     * it stays invalid, that means there is no egress intf available)
     */
    pri_block->delivery_data.egress_intf_id = BP_INVALID_HANDLE;

    /* calculate the time for the _next_ retransmit event, or expire time (whichever comes first) */
    action_time = pri_block->delivery_data.local_retx_interval + bplib_os_get_dtntime_ms();
    if (action_time > store_entry->expire_time)
    {
        action_time = store_entry->expire_time;
    }

    /*
     * since every entry in the tree may consume an extra block (required for duplicate handling),
     * try to batch together a group of times.  There shouldn't be a real need for millisecond-
     * level precision here, and this should hopefully reduce the block usage by consolidating
     * all entries that are on the same second.
     */
    action_time = (action_time + 999) / 1000;

    /* If this was in the time index, the old entry needs to be removed first, then it needs to be added in the new spot */
    bplib_store_cache_remove_from_subindex(state->parent_pool, &state->time_index, &store_entry->time_link);
    bplib_store_cache_add_to_subindex(state->parent_pool, &state->time_index, &store_entry->time_link, action_time);

    /* In the unlikely event that this fails, the refcount will remain 1, so it will be found again next poll */
    rblk = bplib_mpool_create_block_reference(state->parent_pool, bplib_mpool_get_reference_target(store_entry->refptr), bplib_store_cache_handle_ref_recycle, state);
    if (rblk != NULL)
    {
        /* put it into the queue for egress (does not fail at this point) */
        bplib_mpool_append_subq_bundle(&state->parent_flow->input, rblk);
        bplib_mpool_mark_flow_active(state->parent_pool, state->parent_flow);
    }
}

int bplib_store_cache_egress_impl(bplib_mpool_block_t *intf_block, void *egress_arg)
{
    bplib_mpool_flow_t *flow;
    bplib_mpool_block_t *qblk;
    bplib_mpool_block_t *sblk;
    bplib_store_cache_state_t *state;
    bplib_mpool_primary_block_t *pri_block;
    bplib_mpool_generic_data_t *store_block;
    bplib_store_cache_entry_t *store_entry;
    bp_primary_block_t *pri;
    bp_ipn_addr_t dest_addr;
    int forward_count;

    state = bplib_store_cache_get_state(intf_block);
    if (state == NULL)
    {
        return -1;
    }

    flow = bplib_mpool_cast_flow(intf_block);
    if (flow == NULL)
    {
        return -1;
    }

    forward_count = 0;
    while(true)
    {
        qblk = bplib_mpool_shift_subq_bundle(&flow->output);
        if (qblk == NULL)
        {
            /* no more bundles */
            break;
        }

        ++forward_count;

        /* Create the storage-specific data block for keeping local refs  */
        sblk = bplib_mpool_alloc_generic_block(state->parent_pool, BPLIB_STORE_SIGNATURE_ENTRY, sizeof(bplib_store_cache_entry_t));
        if (sblk != NULL)
        {
            /* These 2 are dynamic casts, could return NULL, but should not unless there are bugs */
            pri_block = bplib_mpool_cast_primary(qblk);
            store_block = bplib_mpool_cast_generic_data(sblk, BPLIB_STORE_SIGNATURE_ENTRY);

            if (pri_block != NULL && store_block != NULL)
            {
                /* these 2 are static casts, they cannot return NULL */
                pri = bplib_mpool_get_pri_block_logical(pri_block);
                store_entry = bplib_mpool_get_generic_data_ptr(store_block);

                store_entry->refptr = bplib_mpool_create_light_reference(state->parent_pool, qblk);

                bplib_mpool_link_reset(&store_entry->time_link, bplib_mpool_blocktype_secondary_link);
                bplib_mpool_link_reset(&store_entry->destination_link, bplib_mpool_blocktype_secondary_link);

                v7_get_eid(&dest_addr, &pri->destinationEID);
                bplib_store_cache_add_to_subindex(state->parent_pool, &state->dest_eid_index, &store_entry->destination_link, dest_addr.node_number);

                if (pri_block->delivery_data.delivery_policy != bplib_policy_delivery_none)
                {
                    store_entry->flags |= BP_BUNDLE_FLAG_AWAITING_CUSTODY;
                }

                /*
                 * Determine expire_time - this should reflect the absolute DTN time at which
                 * this bundle is no longer useful.  This serves as the index into the time tree.
                 */
                /* for now, assume its within lifetime, first timed poll will find out if its not */
                /* TBD: may not want to even store it if this is not true */
                store_entry->expire_time = pri->creationTimeStamp.time + pri->lifetime;
                store_entry->flags |= BP_BUNDLE_FLAG_WITHIN_LIFETIME;

                pri_block->delivery_data.storage_intf_id = flow->self_intf_id;
                pri_block->delivery_data.committed_storage_id = (bp_sid_t)sblk;

                bplib_store_cache_do_transmit(state, store_entry, pri_block);

                /* NOTE:
                * This may also be the right place to generate a custody signal, if the bundle
                * has the full custody tracking/transfer service level.
                */

                /* done with the bundle and the storage entry block (do not recycle it) */
                sblk = NULL;
            }
        }

        /*
         * If anything is left pointing to a block, those blocks should be recycled.
         * (or else this would be a leak).
         *
         * The original/input ref to the bundle can be removed without issue, a copy
         * was stored, so this should not trigger a zero refcount unless the storage
         * failed.
         */
        if (qblk)
        {
            bplib_mpool_recycle_block(state->parent_pool, qblk);
        }
        if (sblk)
        {
            bplib_mpool_recycle_block(state->parent_pool, sblk);
        }
    }

    return forward_count;
}

void bplib_store_cache_cleanup_bundle(void *arg, bplib_mpool_block_t *sblk)
{
    bplib_store_cache_state_t *state;
    bplib_mpool_generic_data_t *store_block;
    bplib_store_cache_entry_t *store_entry;

    state = arg;

    /* This cast should always work, unless there is a bug */
    store_block = bplib_mpool_cast_generic_data(sblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_block != NULL)
    {
        store_entry = bplib_mpool_get_generic_data_ptr(store_block);

        /* ensure this has been removed from the various indices */
        bplib_store_cache_remove_from_subindex(state->parent_pool, &state->time_index, &store_entry->time_link);
        bplib_store_cache_remove_from_subindex(state->parent_pool, &state->dest_eid_index, &store_entry->destination_link);

        /* release the refptr */
        bplib_mpool_release_light_reference(state->parent_pool, store_entry->refptr);
        store_entry->refptr = NULL;
    }
}

void bplib_store_cache_handle_bundle_pending_action(void *arg, bplib_mpool_block_t *sblk)
{
    bplib_store_cache_state_t *state;
    bplib_mpool_primary_block_t *pri_block;
    bplib_mpool_block_t *pblk;
    bplib_mpool_block_t *next_list;
    bplib_mpool_generic_data_t *store_block;
    bplib_store_cache_entry_t *store_entry;
    uint64_t now_time;

    state = arg;

    /* This cast should always work, unless there is a bug */
    store_block = bplib_mpool_cast_generic_data(sblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_block != NULL)
    {
        store_entry = bplib_mpool_get_generic_data_ptr(store_block);
        pblk = bplib_mpool_get_reference_target(store_entry->refptr);
        pri_block = bplib_mpool_cast_primary(pblk);
        now_time = bplib_os_get_dtntime_ms();
        next_list = NULL;

        if (pri_block != NULL)
        {
            /*
            * Check various conditions that may indicate action is needed on this bundle
            */
            if ((store_entry->flags & BP_BUNDLE_FLAG_WITHIN_LIFETIME) == BP_BUNDLE_FLAG_WITHIN_LIFETIME &&
                    now_time >= store_entry->expire_time)
            {
                store_entry->flags &= ~BP_BUNDLE_FLAG_WITHIN_LIFETIME;
            }

            /*
            * If all flags have been UN-set then this is a candidate for removal
            * Note that this just removes the local ref, the memory for the bundle itself will not be actually
            * recycled until the refcount also reaches 0.  Removing this ref will permit it to reach 0.
            */
            if ((~store_entry->flags & (BP_BUNDLE_FLAG_WITHIN_LIFETIME | BP_BUNDLE_FLAG_AWAITING_CUSTODY)) != 0)
            {
                next_list = &state->expired_list;
            }
            else
            {
                next_list = &state->idle_list;

                /* If refcount > 1 here, that means there is already a copy in a queue
                 * somewhere in the system.  Do not re-queue until that ref disappears */
                if (bplib_mpool_get_read_refcount(pblk) <= 1 &&
                    (!bp_handle_is_valid(pri_block->delivery_data.egress_intf_id) || now_time >= store_entry->retx_time))
                {
                    bplib_store_cache_do_transmit(state, store_entry, pri_block);
                }
            }

            /* note that this was already removed from pending_list */
            bplib_mpool_insert_before(next_list, sblk);
        }
    }
}

void bplib_store_cache_flush_pending(bplib_store_cache_state_t *state)
{
    /* Attempt to re-route all bundles in the pending list */
    /* In some cases the bundle can get re-added to the pending list, so this is done in a loop */
    do
    {
        bplib_mpool_foreach_item_in_list(&state->pending_list, true, bplib_store_cache_handle_bundle_pending_action, state);
        bplib_mpool_foreach_item_in_list(&state->expired_list, false, bplib_store_cache_cleanup_bundle, state);
        bplib_mpool_recycle_all_blocks_in_list(state->parent_pool, &state->expired_list);
    }
    while (!bplib_mpool_is_empty_list_head(&state->pending_list));
}

void bplib_store_cache_bundle_make_pending_from_time_index(void *arg, bplib_mpool_block_t *qblk)
{
    bplib_store_cache_state_t *state;
    bplib_store_cache_entry_t *store_entry;
    bplib_mpool_block_t *sblk;

    state = arg;

    store_entry = bplib_store_cache_entry_from_time_link(qblk);
    sblk = bplib_mpool_get_generic_block_from_pointer(store_entry, BPLIB_STORE_SIGNATURE_ENTRY);
    if (sblk != NULL)
    {
        bplib_mpool_extract_node(sblk);
        bplib_mpool_insert_before(&state->pending_list, sblk);
    }
}

void bplib_store_cache_bundle_make_pending_from_destination_index(void *arg, bplib_mpool_block_t *qblk)
{
    bplib_store_cache_state_t *state;
    bplib_store_cache_entry_t *store_entry;
    bplib_mpool_block_t *sblk;

    state = arg;

    store_entry = bplib_store_cache_entry_from_destination_link(qblk);
    sblk = bplib_mpool_get_generic_block_from_pointer(store_entry, BPLIB_STORE_SIGNATURE_ENTRY);
    if (sblk != NULL)
    {
        bplib_mpool_extract_node(sblk);
        bplib_mpool_insert_before(&state->pending_list, sblk);
    }
}

int bplib_store_cache_do_poll(bplib_store_cache_state_t *state)
{
    bplib_rbt_iter_t it;
    bplib_mpool_block_t *sblk;
    bplib_store_cache_queue_t *store_queue;
    int iter_status;

    iter_status = bplib_rbt_iter_goto_max(bplib_os_get_dtntime_ms(), &state->time_index, &it);
    while (iter_status == BP_SUCCESS)
    {
        store_queue = bplib_store_cache_queue_from_rbt_link(it.position);

        /* preemptively move the iterator - the current entry will be removed,
         * and if that was done first, it would invalidate the iterator */
        iter_status = bplib_rbt_iter_prev(&it);

        /* move the entire set of nodes on this tree entry to the pending_list */
        bplib_mpool_foreach_item_in_list(&store_queue->bundle_list, true, bplib_store_cache_bundle_make_pending_from_time_index, state);

        /* done with this entry in the time index (will be re-added when pending_list is processed) */
        bplib_rbt_extract_node(&state->time_index, &store_queue->rbt_link);

        /* this should always work */
        sblk = bplib_mpool_get_generic_block_from_pointer(store_queue, BPLIB_STORE_SIGNATURE_QUEUE);
        if (sblk != NULL)
        {
            bplib_mpool_recycle_block(state->parent_pool, sblk);
        }
    }

    return BP_SUCCESS;
}

int bplib_store_cache_do_route_up(bplib_store_cache_state_t *state, bp_ipn_t dest, bp_ipn_t mask)
{
    bplib_rbt_iter_t it;
    bplib_store_cache_queue_t *store_queue;
    bp_ipn_t curr_ipn;
    int iter_status;

    iter_status = bplib_rbt_iter_goto_min(dest, &state->dest_eid_index, &it);
    while (iter_status == BP_SUCCESS)
    {
        curr_ipn = bplib_rbt_get_key_value(it.position);
        if ((curr_ipn & mask) != dest)
        {
            /* no longer a route match, all done */
            break;
        }

        store_queue = bplib_store_cache_queue_from_rbt_link(it.position);

        /* move the entire set of nodes on this tree entry to the pending_list */
        bplib_mpool_foreach_item_in_list(&store_queue->bundle_list, false, bplib_store_cache_bundle_make_pending_from_destination_index, state);

        /* preemptively move the iterator - the current entry will be removed,
         * and if that was done first, it would invalidate the iterator */
        iter_status = bplib_rbt_iter_next(&it);
    }

    return BP_SUCCESS;
}

int bplib_store_cache_event_impl(bplib_mpool_block_t *intf_block, void *event_arg)
{
    bplib_mpool_flow_t *flow;
    bplib_store_cache_state_t *state;
    bplib_generic_event_t *event;

    event = event_arg;
    state = bplib_store_cache_get_state(intf_block);
    if (state == NULL)
    {
        return -1;
    }

    flow = bplib_mpool_cast_flow(intf_block);
    if (flow == NULL)
    {
        return -1;
    }

    if (event->event_type == bplib_event_type_poll_interval)
    {
        bplib_store_cache_do_poll(state);
    }
    else if (event->event_type == bplib_event_type_route_up)
    {
        bplib_store_cache_do_route_up(state, event->route_state.dest, event->route_state.mask);
    }

    /* any sort of action may have put bundles in the pending queue, so flush it now */
    bplib_store_cache_flush_pending(state);


    return 0;
}

void bplib_store_cache_do_construct(bplib_routetbl_t *tbl, bp_handle_t intf_id, bplib_store_cache_construct_transaction_t *xactn)
{
    bplib_mpool_t *pool;
    int status;

    pool = bplib_route_get_mpool(tbl);
    status = 0;

    while (xactn->construct_state < bplib_store_cache_construct_state_done)
    {
        switch(xactn->construct_state)
        {
            case bplib_store_cache_construct_state_alloc:
            {
                xactn->storage_block = bplib_mpool_alloc_generic_block(pool, BPLIB_STORE_SIGNATURE_STATE, sizeof(bplib_store_cache_state_t));
                if (xactn->storage_block == NULL)
                {
                    bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "%s(): Insufficient memory to create file storage\n", __func__);
                    status = -1;
                }
                break;
            }
            case bplib_store_cache_construct_state_lock_intf:
            {
                xactn->parent_intf_block = bplib_route_get_intf_controlblock(tbl, intf_id);
                if (xactn->parent_intf_block == NULL)
                {
                    bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): Parent intf invalid\n", __func__);
                    status = -1;
                }
                break;
            }
            case bplib_store_cache_construct_state_setup_cb:
            {
                bplib_mpool_generic_data_t *gd;

                gd = bplib_mpool_cast_generic_data(xactn->storage_block, BPLIB_STORE_SIGNATURE_STATE);
                if (gd == NULL)
                {
                    bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): bplib_mpool_cast_generic_data failed\n", __func__);
                    status = -1;
                }
                else
                {
                    xactn->state_ptr = bplib_mpool_get_generic_data_ptr(gd);

                    xactn->state_ptr->parent_pool = pool;

                    bplib_mpool_link_reset(&xactn->state_ptr->pending_list, bplib_mpool_blocktype_head);
                    bplib_mpool_link_reset(&xactn->state_ptr->expired_list, bplib_mpool_blocktype_head);
                    bplib_mpool_link_reset(&xactn->state_ptr->idle_list, bplib_mpool_blocktype_head);
                }
                break;
            }
            case bplib_store_cache_construct_state_register_egress:
            {
                status = bplib_route_register_forward_egress_handler(tbl, intf_id, bplib_store_cache_egress_impl, NULL);
                break;
            }
            case bplib_store_cache_construct_state_register_event:
            {
                status = bplib_route_register_event_handler(tbl, intf_id, bplib_store_cache_event_impl);
                break;
            }
            case bplib_store_cache_construct_state_attach_cb:
            {
                bplib_mpool_baseintf_t *parent_intf;

                parent_intf = bplib_mpool_cast_baseintf(xactn->parent_intf_block);
                if (parent_intf == NULL)
                {
                    bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): parent is not a baseintf!\n", __func__);
                    status = -1;
                }
                else if (parent_intf->service_specific_block != NULL)
                {
                    bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): already bound!\n", __func__);
                    status = -1;
                }
                else
                {
                    xactn->state_ptr->parent_flow = bplib_mpool_cast_flow(xactn->parent_intf_block);
                    parent_intf->service_specific_block = xactn->storage_block;
                    xactn->storage_block = NULL;
                }
                break;
            }
            case bplib_store_cache_construct_state_unlock_intf:
            {
                bplib_route_release_intf_controlblock(xactn->parent_intf_block);
                xactn->parent_intf_block = NULL;
                break;
            }
            default:
                /* no-op, placeholder */
                break;
        }

        if (status < 0)
        {
            /* something went wrong */
            break;
        }

        ++xactn->construct_state;
    }
}

void bplib_store_cache_do_destruct(bplib_routetbl_t *tbl, bp_handle_t intf_id, bplib_store_cache_construct_transaction_t *xactn)
{
    bplib_mpool_t *pool;
    int status;

    pool = bplib_route_get_mpool(tbl);

    while (xactn->construct_state > bplib_store_cache_construct_state_init)
    {
        --xactn->construct_state;
        switch(xactn->construct_state)
        {
            case bplib_store_cache_construct_state_alloc:
            {
                if (xactn->storage_block)
                {
                    bplib_mpool_recycle_block(pool, xactn->storage_block);
                    xactn->storage_block = NULL;
                }
                break;
            }
            case bplib_store_cache_construct_state_lock_intf:
            {
                if (xactn->parent_intf_block)
                {
                    bplib_route_release_intf_controlblock(xactn->parent_intf_block);
                    xactn->parent_intf_block = NULL;
                }
                break;
            }
            case bplib_store_cache_construct_state_setup_cb:
            {
                bplib_rbt_iter_t it;
                bplib_store_cache_queue_t *store_queue;
                bplib_mpool_block_t *sblk;

                xactn->state_ptr->parent_pool = NULL;

                /* make sure all lists and indices are empty, blocks will be leaked if not */
                /* first tackle the time index */
                status = bplib_rbt_iter_goto_min(0, &xactn->state_ptr->time_index, &it);
                while (status == BP_SUCCESS)
                {
                    store_queue = bplib_store_cache_queue_from_rbt_link(it.position);
                    status = bplib_rbt_iter_next(&it);

                    /* move the entire set of nodes on this tree entry to the pending_list */
                    bplib_mpool_foreach_item_in_list(&store_queue->bundle_list, true, bplib_store_cache_bundle_make_pending_from_time_index, xactn->state_ptr);

                    /* done with this entry in the time index (will be re-added when pending_list is processed) */
                    bplib_rbt_extract_node(&xactn->state_ptr->time_index, &store_queue->rbt_link);

                    /* this should always work */
                    sblk = bplib_mpool_get_generic_block_from_pointer(store_queue, BPLIB_STORE_SIGNATURE_QUEUE);
                    if (sblk != NULL)
                    {
                        bplib_mpool_recycle_block(pool, sblk);
                    }
                }
                /* first tackle the destination EID index */
                status = bplib_rbt_iter_goto_min(0, &xactn->state_ptr->dest_eid_index, &it);
                while (status == BP_SUCCESS)
                {
                    store_queue = bplib_store_cache_queue_from_rbt_link(it.position);
                    status = bplib_rbt_iter_next(&it);

                    /* move the entire set of nodes on this tree entry to the pending_list */
                    bplib_mpool_foreach_item_in_list(&store_queue->bundle_list, true, bplib_store_cache_bundle_make_pending_from_destination_index, xactn->state_ptr);

                    /* done with this entry in the time index (will be re-added when pending_list is processed) */
                    bplib_rbt_extract_node(&xactn->state_ptr->dest_eid_index, &store_queue->rbt_link);

                    /* this should always work */
                    sblk = bplib_mpool_get_generic_block_from_pointer(store_queue, BPLIB_STORE_SIGNATURE_QUEUE);
                    if (sblk != NULL)
                    {
                        bplib_mpool_recycle_block(pool, sblk);
                    }
                }
                /* iterator status should not stop remainder of desconstruction */
                status = BP_SUCCESS;
                bplib_mpool_recycle_all_blocks_in_list(pool, &xactn->state_ptr->idle_list);
                bplib_mpool_recycle_all_blocks_in_list(pool, &xactn->state_ptr->pending_list);
                bplib_mpool_recycle_all_blocks_in_list(pool, &xactn->state_ptr->expired_list);
                break;
            }
            case bplib_store_cache_construct_state_register_egress:
            {
                if (xactn->storage_block)
                {
                    bplib_route_register_forward_egress_handler(tbl, intf_id, NULL, NULL);
                }
                break;
            }
            case bplib_store_cache_construct_state_register_event:
            {
                if (xactn->storage_block)
                {
                    bplib_route_register_event_handler(tbl, intf_id, NULL);
                }
                break;
            }
            case bplib_store_cache_construct_state_attach_cb:
            {
                bplib_mpool_baseintf_t *parent_intf;
                bplib_mpool_generic_data_t *gd;

                parent_intf = bplib_mpool_cast_baseintf(xactn->parent_intf_block);
                if (parent_intf == NULL)
                {
                    bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): parent is not a baseintf!\n", __func__);
                    status = -1; /* this should stop deconstruction */
                }
                else
                {
                    gd = bplib_mpool_cast_generic_data(parent_intf->service_specific_block, BPLIB_STORE_SIGNATURE_STATE);
                    if (gd == NULL)
                    {
                        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): parent does not have cache service attached!\n", __func__);
                        status = -1; /* this should stop deconstruction */
                    }
                    else
                    {
                        xactn->state_ptr = bplib_mpool_get_generic_data_ptr(gd);
                        xactn->storage_block = parent_intf->service_specific_block;
                        parent_intf->service_specific_block = NULL;
                        xactn->state_ptr->parent_flow = NULL;
                    }
                }
                break;
            }
            case bplib_store_cache_construct_state_unlock_intf:
            {
                xactn->parent_intf_block = bplib_route_get_intf_controlblock(tbl, intf_id);
                if (xactn->parent_intf_block == NULL)
                {
                    bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): Parent intf invalid\n", __func__);
                    status = -1; /* this should stop deconstruction */
                }
                break;
            }
            default:
                /* no-op, placeholder */
                break;
        }

        if (status < 0)
        {
            break;
        }
    }
}

int bplib_store_cache_attach(bplib_routetbl_t *tbl, bp_handle_t intf_id)
{
    bplib_store_cache_construct_transaction_t xactn;
    int status;

    memset(&xactn, 0, sizeof(xactn));

    bplib_store_cache_do_construct(tbl, intf_id, &xactn);
    if (xactn.construct_state < bplib_store_cache_construct_state_done)
    {
        bplib_store_cache_do_destruct(tbl, intf_id, &xactn);
        status = BP_ERROR;
    }
    else
    {
        status = BP_SUCCESS;
    }

    return status;
}

int bplib_store_cache_detach(bplib_routetbl_t *tbl, bp_handle_t intf_id)
{
    bplib_store_cache_construct_transaction_t xactn;
    int status;

    memset(&xactn, 0, sizeof(xactn));

    xactn.construct_state = bplib_store_cache_construct_state_done;

    bplib_store_cache_do_destruct(tbl, intf_id, &xactn);
    if (xactn.construct_state > bplib_store_cache_construct_state_init)
    {
        status = BP_ERROR;
    }
    else
    {
        status = BP_SUCCESS;
    }

    return status;
}

void bplib_store_cache_debug_scan(bplib_routetbl_t *tbl, bp_handle_t intf_id)
{
    bplib_mpool_block_t *intf_block;
    bplib_store_cache_state_t *state;

    intf_block = bplib_route_get_intf_controlblock(tbl, intf_id);
    if (intf_block == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): Parent intf invalid\n", __func__);
        return;
    }

    state = bplib_store_cache_get_state(intf_block);
    if (state == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): Parent intf is not a storage cache\n", __func__);
        return;
    }

    printf("DEBUG: %s() intf_id=%d\n", __func__, bp_handle_printable(intf_id));

    bplib_mpool_debug_print_queue_stats(&state->pending_list, "pending_list");
    bplib_mpool_debug_print_queue_stats(&state->expired_list, "expired_list");
    bplib_mpool_debug_print_queue_stats(&state->idle_list, "idle_list");
}
