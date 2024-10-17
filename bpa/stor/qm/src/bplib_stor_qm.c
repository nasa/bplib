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

// #include "bplib_os.h"

#include "bplib_mem.h"

#include "bplib_stor_qm.h"

/**
 * @brief Minimum time between interface poll cycles
 *
 * Most timers in DTN are on the longer side (i.e. minutes/hours, not milliseconds)
 * so it is generally not necessary to be aggressive with polling.
 */
#define BPLIB_INTF_MIN_POLL_INTERVAL 500

#define BPLIB_INTF_AVAILABLE_FLAGS (BPLIB_INTF_STATE_OPER_UP | BPLIB_INTF_STATE_ADMIN_UP)

BPLib_STOR_CACHE_Ref_t BPLib_STOR_QM_GetIntfControlblock(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id)
{
    #ifdef QM
    BPLib_STOR_CACHE_Block_t *blk;

    blk = BPLib_STOR_CACHE_BlockFromExternalId(tbl->pool, intf_id);
    return BPLib_STOR_CACHE_RefCreate(blk);
    #else // QM
    return NULL;
    #endif
}

void BPLib_STOR_QM_ReleaseIntfControlblock(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_STOR_CACHE_Ref_t refptr)
{
    #ifdef QM
    BPLib_STOR_CACHE_RefRelease(refptr);
    #endif // QM
}

BPLib_STOR_QM_Duct_t *bplip_queue_lookup_intf(const BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id)
{
    #ifdef QM
    BPLib_STOR_CACHE_Block_t *blk;

    blk = BPLib_STOR_CACHE_BlockFromExternalId(tbl->pool, intf_id);
    return BPLib_STOR_QM_DuctCast(blk);
    #else // QM
    return NULL;
    #endif // QM
}

static inline const BPLib_STOR_QM_Duct_t *bplip_queue_lookup_intf_const(const BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id)
{
    #ifdef QM
    /* note that bplip_queue_lookup_intf does not modify its argument itself */
    return bplip_queue_lookup_intf(tbl, intf_id);
    #else // QM
    return NULL;
    #endif // QM
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int BPLib_STOR_QM_QueueIngressToParent(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
{
    #ifdef QM
    BPLib_STOR_QM_Duct_t *curr_duct;
    BPLib_STOR_QM_Duct_t *next_duct;
    uint32_t            queue_depth;

    /* This implements a simple ingress for sub-interfaces (of any kind) where the ingress
     * of the sub-interface nodes is simply funneled into the parent */

    curr_duct = BPLib_STOR_QM_DuctCast(BPLib_STOR_CACHE_GetBlockFromLink(subq_src));
    if (curr_duct == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "Failed to cast duct block\n");
        return -1;
    }

    next_duct = BPLib_STOR_QM_DuctCast(BPLib_STOR_CACHE_Dereference(curr_duct->parent));
    if (next_duct == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "Failed to cast duct parent block\n");
        return -1;
    }

    /* Entire contents moved as a batch. */
    queue_depth = BPLib_STOR_QM_DuctTryMoveAll(&next_duct->ingress, &curr_duct->ingress, BPLIB_MONOTIME_ZERO);

    return queue_depth;
    #else // QM
    return 0;
    #endif // QM
}

#ifdef QM
void BPLib_STOR_QM_IngressQueueSingleBundle(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_STOR_CACHE_Block_t *pblk)
{
    BPLib_STOR_CACHE_BblockPrimary_t *pri_block;
    BPLib_STOR_CACHE_PrimaryBlock_t  *pri;
    bp_ipn_addr_t                     dest_addr;
    bp_handle_t                       next_hop;
    uint32_t                          req_flags;
    uint32_t                          flag_mask;

    /* is this routable right now? */
    pri_block = BPLib_STOR_CACHE_BblockPrimaryCast(pblk);
    if (pri_block != NULL)
    {
        pri = BPLib_STOR_CACHE_BblockPrimaryGetLogical(pri_block);

        BPLib_STOR_QM_GetEid(&dest_addr, &pri->destinationEID);

        /* the next hop must be "up" (both administratively and operationally) to be valid */
        /* Also, if this bundle has not yet been stored, and the delivery policy wants some form of acknowledgement,
         * then it should be directed to an interface that is storage-capable, even if the outgoing CLA is up */
        req_flags = BPLIB_INTF_STATE_ADMIN_UP | BPLIB_INTF_STATE_OPER_UP;
        flag_mask = req_flags;

        if (!bp_handle_is_valid(pri_block->data.delivery.storage_intf_id) &&
            pri_block->data.delivery.delivery_policy !=BPLib_STOR_CACHE_PolicyDeliveryNone)
        {
            /* not yet stored and needs to be, so next hop must be a storage */
            flag_mask |= BPLIB_CACHE_STATE_FLAG_STORAGE;
            req_flags |= BPLIB_CACHE_STATE_FLAG_STORAGE;
        }
        next_hop = BPLib_STOR_QM_GetNextIntfWithFlags(tbl, dest_addr.node_number, req_flags, flag_mask);
        if (bp_handle_is_valid(next_hop) && BPLib_STOR_QM_PushEgressBundle(tbl, next_hop, pblk) == 0)
        {
            /* successfully queued */
            ++tbl->queueing_success_count;
            pblk = NULL;
        }
    }

    /* if qblk is still set to non-null at this point, it means the block was not routable */
    if (pblk != NULL)
    {
        /* this should never happen, must discard the block because there is nowhere to put it */
        BPLib_STOR_CACHE_RecycleBlock(pblk);
        ++tbl->queueing_error_count;
    }
}

int BPLib_STOR_QM_IngressBaseintfForwarder(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
{
    BPLib_STOR_CACHE_Block_t *qblk;
    BPLib_STOR_QM_Duct_t  *duct;
    int                  forward_count;

    duct = BPLib_STOR_QM_DuctCast(BPLib_STOR_CACHE_GetBlockFromLink(subq_src));
    if (duct == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "Failed to cast duct block\n");
        return -1;
    }

    forward_count = 0;
    while (true)
    {
        qblk = BPLib_STOR_QM_DuctTryPull(&duct->ingress, BPLIB_MONOTIME_ZERO);
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
        BPLib_STOR_QM_IngressQueueSingleBundle(arg, qblk);
    }

    /* This should return 0 if it did no work and no errors.
     * Should return >0 if some work was done */
    return forward_count;
}

BPLib_STOR_CACHE_Pool_t *BPLib_STOR_QM_GetQtblPool(const BPLib_STOR_QM_QueueTbl_t *tbl)
{
    return tbl->pool;
}

BPLib_STOR_QM_QueueTbl_t *BPLib_STOR_QM_AllocTable(uint32_t max_queues, size_t cache_mem_size)
{
    size_t                    complete_size;
    size_t                    align;
    size_t                    queue_offset;
    size_t                    BPLib_STOR_CACHE_Offset;
    uint8_t                  *mem_ptr;
    BPLib_STOR_QM_QueueTbl_t *tbl_ptr;
    struct queueentry_align
    {
        /* This byte only exists to check the offset of the following member */
        /* cppcheck-suppress unusedStructMember */
        uint8_t            byte;
        BPLib_STOR_QM_QueueEntry_t queue_tbl_offset;
    };

    if (max_queues == 0)
    {
        return NULL;
    }

    complete_size = sizeof(BPLib_STOR_QM_QueueTbl_t);

    align         = offsetof(struct queueentry_align, queue_tbl_offset) - 1;
    complete_size = (complete_size + align) & ~align;
    queue_offset  = complete_size;
    complete_size += sizeof(BPLib_STOR_QM_QueueEntry_t) * max_queues;

    align = sizeof(void *) - 1;
    align |= sizeof(uintmax_t) - 1;
    complete_size      = (complete_size + align) & ~align;
    BPLib_STOR_CACHE_Offset = complete_size;
    complete_size += cache_mem_size;

    complete_size = (complete_size + align) & ~align;

    tbl_ptr = (BPLib_STOR_QM_QueueTbl_t *)bplib_os_calloc(complete_size);
    mem_ptr = (uint8_t *)tbl_ptr;

    if (tbl_ptr != NULL)
    {
        tbl_ptr->pool = BPLib_STOR_CACHE_Create(mem_ptr + BPLib_STOR_CACHE_Offset, complete_size - BPLib_STOR_CACHE_Offset);
        if (tbl_ptr->pool == NULL)
        {
            bplib_os_free(tbl_ptr);
            tbl_ptr = NULL;
        }
    }

    if (tbl_ptr != NULL)
    {
        tbl_ptr->activity_lock = BPLib_MEM_OS_CreateLock();
        tbl_ptr->last_intf_poll = BPLib_STOR_CACHE_GetMonotonicTime();
        BPLib_STOR_CACHE_InitListHead(NULL, &tbl_ptr->duct_list);

        tbl_ptr->max_queues = max_queues;
        tbl_ptr->queue_tbl  = (void *)(mem_ptr + queue_offset);
    }

    return tbl_ptr;
}

bp_handle_t BPLib_STOR_QM_RegisterGenericIntf(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t parent_intf_id,
                                              BPLib_STOR_CACHE_Block_t *duct_block)
{
    BPLib_STOR_QM_Duct_t *duct;
    BPLib_STOR_CACHE_Ref_t   fref;
    bp_handle_t         result;

    result = BP_INVALID_HANDLE;
    duct   = BPLib_STOR_QM_DuctCast(duct_block);
    if (duct == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_OUT_OF_MEMORY, "Failed to cast duct block\n");
        return result;
    }

    fref = BPLib_STOR_QM_GetIntfControlblock(tbl, parent_intf_id);

    if (fref == NULL && bp_handle_is_valid(parent_intf_id))
    {
        /* this is an error */
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "Parent intf not valid\n");
    }
    else
    {
        duct->parent = fref;
        result       = BPLib_STOR_CACHE_GetExternalId(duct_block);

        /* keep track of all ducts here in the duct_list - this counts as a ref so it
         * will not be auto-collected as long as its here in this list.  The ref is
         * intentionally _not_ released here, see BPLib_STOR_QM_DelIntf */
        fref = BPLib_STOR_CACHE_RefCreate(duct_block);
        if (fref != NULL)
        {
            BPLib_MEM_OS_Lock(tbl->activity_lock);
            BPLib_STOR_CACHE_InsertBefore(&tbl->duct_list, duct_block);
            BPLib_MEM_OS_Unlock(tbl->activity_lock);
        }
    }

    return result;
}

int BPLib_STOR_QM_RegisterHandlerImpl(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id, size_t func_position,
                                      BPLib_STOR_CACHE_CallbackFunc_t new_func)
{
    BPLib_STOR_QM_Duct_t          *ifp;
    uint8_t                     *base_ptr;
    BPLib_STOR_CACHE_SubqWorkitem_t *subq;

    ifp = bplip_queue_lookup_intf(tbl, intf_id);
    if (ifp == NULL)
    {
        /* Not a valid handle */
        return -1;
    }

    base_ptr = (uint8_t *)ifp;

    subq = (BPLib_STOR_CACHE_SubqWorkitem_t *)(void *)(base_ptr + func_position);

    if (subq->job_header.handler == new_func)
    {
        /* already registered */
        return 0;
    }

    /* One or the other must be NULL or else this is an invalid registration */
    if (subq->job_header.handler != NULL && new_func != NULL)
    {
        return -1;
    }

    /* OK, now set it to the new value */
    subq->job_header.handler = new_func;
    return 0;
}

int BPLib_STOR_QM_RegisterForwardIngressHandler(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id,
                                                 BPLib_STOR_CACHE_CallbackFunc_t ingress)
{
    return BPLib_STOR_QM_RegisterHandlerImpl(tbl, intf_id, offsetof(BPLib_STOR_QM_Duct_t, ingress), ingress);
}

int BPLib_STOR_QM_RegisterForwardEgressHandler(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id,
                                                BPLib_STOR_CACHE_CallbackFunc_t egress)
{
    return BPLib_STOR_QM_RegisterHandlerImpl(tbl, intf_id, offsetof(BPLib_STOR_QM_Duct_t, egress), egress);
}

int BPLib_STOR_QM_RegisterEventHandler(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id, BPLib_STOR_CACHE_CallbackFunc_t event)
{
    BPLib_STOR_QM_Duct_t *ifp;

    ifp = bplip_queue_lookup_intf(tbl, intf_id);
    if (ifp == NULL)
    {
        /* Not a valid handle */
        return -1;
    }

    if (ifp->statechange_job.event_handler == event)
    {
        /* already registered */
        return 0;
    }

    /* One or the other must be NULL or else this is an invalid registration */
    if (ifp->statechange_job.event_handler != NULL && event != NULL)
    {
        return -1;
    }

    /* OK, now set it to the new value */
    ifp->statechange_job.event_handler = event;
    return 0;
}

int BPLib_STOR_QM_DelIntf(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id)
{
    uint32_t            pos;
    BPLib_STOR_QM_QueueEntry_t *rp;
    BPLib_STOR_CACHE_Ref_t   ref;
    BPLib_STOR_QM_Duct_t *ifp;

    ref = BPLib_STOR_QM_GetIntfControlblock(tbl, intf_id);
    if (ref == NULL)
    {
        /* Not a valid handle */
        return -1;
    }

    ifp = BPLib_STOR_QM_DuctCast(BPLib_STOR_CACHE_Dereference(ref));
    if (ifp != NULL)
    {
        /* before it can be deleted, should ensure it is not referenced */
        for (pos = 0; pos < tbl->registered_queues; ++pos)
        {
            rp = &tbl->queue_tbl[pos];
            if (bp_handle_equal(rp->intf_id, intf_id))
            {
                --tbl->registered_queues;

                /* close the gap, if this left one */
                if (pos < tbl->registered_queues)
                {
                    memmove(&rp[0], &rp[1], sizeof(*rp) * (tbl->registered_queues - pos));
                }
                break;
            }
        }
    }

    /* remove the duct from the duct_list.  This releases the reference
     * that was created during BPLib_STOR_QM_RegisterGenericIntf()  */
    BPLib_MEM_OS_Lock(tbl->activity_lock);
    if (BPLib_STOR_CACHE_IsLinkAttached(BPLib_STOR_CACHE_Dereference(ref)))
    {
        BPLib_STOR_CACHE_ExtractNode(BPLib_STOR_CACHE_Dereference(ref));
    }
    BPLib_MEM_OS_Unlock(tbl->activity_lock);

    /* release the local ref, this should make the refcount 0 again */
    BPLib_STOR_CACHE_RefRelease(ref);

    /* just wipe it */
    memset(ifp, 0, sizeof(*ifp));
    return 0;
}

bp_handle_t BPLib_STOR_QM_GetNextIntfWithFlags(const BPLib_STOR_QM_QueueTbl_t *tbl, bp_ipn_t dest, uint32_t req_flags,
                                                 uint32_t flag_mask)
{
    uint32_t                  pos;
    BPLib_STOR_QM_QueueEntry_t       *rp;
    bp_handle_t               intf;
    const BPLib_STOR_QM_Duct_t *ifp;
    uint32_t                  intf_flags;

    intf = BP_INVALID_HANDLE;
    for (pos = 0; pos < tbl->registered_queues; ++pos)
    {
        rp = &tbl->queue_tbl[pos];
        if (((rp->dest ^ dest) & rp->mask) == 0)
        {
            intf_flags = ~req_flags;
            if (flag_mask != 0)
            {
                ifp = bplip_queue_lookup_intf_const(tbl, rp->intf_id);
                if (ifp != NULL)
                {
                    intf_flags = ifp->current_state_flags;
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

bp_handle_t BPLib_STOR_QM_GetNextAvailIntf(const BPLib_STOR_QM_QueueTbl_t *tbl, bp_ipn_t dest)
{
    return BPLib_STOR_QM_GetNextIntfWithFlags(tbl, dest, BPLIB_INTF_AVAILABLE_FLAGS, BPLIB_INTF_AVAILABLE_FLAGS);
}

int BPLib_STOR_QM_PushIngressBundle(const BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id, BPLib_STOR_CACHE_Block_t *cb)
{
    BPLib_STOR_QM_Duct_t *duct;
    int                 status;

    status = -1;
    duct   = bplip_queue_lookup_intf(tbl, intf_id);
    if (duct != NULL && BPLib_STOR_QM_DuctTryPush(&duct->ingress, cb, BPLIB_MONOTIME_ZERO))
    {
        status = 0;
    }

    return status;
}

int BPLib_STOR_QM_PushEgressBundle(const BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id, BPLib_STOR_CACHE_Block_t *cb)
{
    BPLib_STOR_QM_Duct_t *duct;
    int                 status;

    status = -1;
    duct   = bplip_queue_lookup_intf(tbl, intf_id);
    if (duct != NULL && BPLib_STOR_QM_DuctTryPush(&duct->egress, cb, BPLIB_MONOTIME_ZERO))
    {
        status = 0;
    }

    return status;
}

int BPLib_STOR_QM_Add(BPLib_STOR_QM_QueueTbl_t *tbl, bp_ipn_t dest, bp_ipn_t mask, bp_handle_t intf_id)
{
    uint32_t            pos;
    uint32_t            insert_pos;
    BPLib_STOR_QM_QueueEntry_t *rp;

    if (tbl->registered_queues >= tbl->max_queues)
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
    for (pos = 0; pos < tbl->registered_queues; ++pos)
    {
        rp = &tbl->queue_tbl[pos];
        if (rp->mask == mask && rp->dest == dest && bp_handle_equal(rp->intf_id, intf_id))
        {
            break;
        }
        if ((rp->mask & mask) == mask)
        {
            ++insert_pos; /* must come (at least) after this queue */
        }
    }

    if (pos < tbl->registered_queues)
    {
        /* duplicate queue */
        return -1;
    }

    /* If necessary, shift entries back to make a gap.
     * This is somewhat expensive, but queue add/remove probably does not
     * happen that often */
    rp = &tbl->queue_tbl[insert_pos];
    if (insert_pos < tbl->registered_queues)
    {
        memmove(&rp[1], &rp[0], sizeof(*rp) * (tbl->registered_queues - insert_pos));
    }

    rp->dest    = dest;
    rp->mask    = mask;
    rp->intf_id = intf_id;

    ++tbl->registered_queues;

    return 0;
}

int BPLib_STOR_QM_Del(BPLib_STOR_QM_QueueTbl_t *tbl, bp_ipn_t dest, bp_ipn_t mask, bp_handle_t intf_id)
{
    uint32_t            pos;
    BPLib_STOR_QM_QueueEntry_t *rp;

    if (tbl->registered_queues == 0)
    {
        return -1;
    }

    /* Find the position, the sequence should go from most specific to least specific */
    for (pos = 0; pos < tbl->registered_queues; ++pos)
    {
        rp = &tbl->queue_tbl[pos];
        if (rp->mask == mask && rp->dest == dest && bp_handle_equal(rp->intf_id, intf_id))
        {
            break;
        }
    }

    if (pos >= tbl->registered_queues)
    {
        /* queue not found */
        return -1;
    }

    --tbl->registered_queues;

    /* If this was in the middle, close the gap */
    rp = &tbl->queue_tbl[pos];
    if (pos < tbl->registered_queues)
    {
        memmove(&rp[0], &rp[1], sizeof(*rp) * (tbl->registered_queues - pos));
    }

    return 0;
}

int BPLib_STOR_QM_IntfSetFlags(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id, uint32_t flags)
{
    BPLib_STOR_CACHE_Ref_t duct_ref;

    duct_ref = BPLib_STOR_QM_GetIntfControlblock(tbl, intf_id);
    if (duct_ref != NULL)
    {
        if (BPLib_STOR_QM_DuctModifyFlags(BPLib_STOR_CACHE_Dereference(duct_ref), flags, 0))
        {
            BPLib_STOR_QM_SetMaintenanceRequest(tbl);
        }
    }
    BPLib_STOR_QM_ReleaseIntfControlblock(tbl, duct_ref);

    return 0;
}

int BPLib_STOR_QM_IntfUnsetFlags(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id, uint32_t flags)
{
    BPLib_STOR_CACHE_Ref_t duct_ref;

    duct_ref = BPLib_STOR_QM_GetIntfControlblock(tbl, intf_id);
    if (duct_ref != NULL)
    {
        if (BPLib_STOR_QM_DuctModifyFlags(BPLib_STOR_CACHE_Dereference(duct_ref), 0, flags))
        {
            BPLib_STOR_QM_SetMaintenanceRequest(tbl);
        }
    }
    BPLib_STOR_QM_ReleaseIntfControlblock(tbl, duct_ref);

    return 0;
}

void BPLib_STOR_QM_DoTimedPoll(BPLib_STOR_QM_QueueTbl_t *tbl)
{
    uint64_t                current_time;
    uint64_t                poll_time;
    uint32_t                set_flags;
    uint32_t                clear_flags;
    bool                    poll_cycle;
    BPLib_STOR_CACHE_ListIter_t iter;
    int                     status;

    current_time = BPLib_STOR_CACHE_GetDtnTime();
    set_flags    = 0;
    clear_flags  = 0;

    /* because the time is a 64-bit value and may not be atomic, it should
     * be sampled and updated inside of a lock section to ensure the value
     * is consistent */
    BPLib_MEM_OS_Lock(tbl->activity_lock);
    poll_time = tbl->last_intf_poll.Time + BPLIB_INTF_MIN_POLL_INTERVAL;
    if (current_time >= poll_time)
    {
        tbl->last_intf_poll.Time = poll_time;
        ++tbl->poll_count;
        poll_cycle = (tbl->poll_count & 1);

        if (poll_cycle)
        {
            set_flags = BPLIB_CACHE_STATE_FLAG_POLL;
        }
        else
        {
            clear_flags = BPLIB_CACHE_STATE_FLAG_POLL;
        }

        status = BPLib_STOR_CACHE_ListIterGotoFirst(&tbl->duct_list, &iter);
        while (status == BPLIB_SUCCESS)
        {
            /* NOTE: this will end up taking the pool lock as well, when it schedules the
             * state change for processing.  This means this task will have two locks at
             * once (the tbl activity lock and the pool resource lock).  As long as the locks
             * are always taken in that order (and not the other way around) this should be OK
             * for now, but it needs to be ensured that locks are never taken in the opposite
             * order. */
            BPLib_STOR_QM_DuctModifyFlags(iter.position, set_flags, clear_flags);
            status = BPLib_STOR_CACHE_ListIterForward(&iter);
        }
    }
    BPLib_MEM_OS_Unlock(tbl->activity_lock);
}

void BPLib_STOR_QM_SetMaintenanceRequest(BPLib_STOR_QM_QueueTbl_t *tbl)
{
    tbl->maint_request_flag = true;
    // TODO OSAL bplib_os_broadcast_signal(tbl->activity_lock);
}

void BPLib_STOR_QM_MaintenanceRequestWait(BPLib_STOR_QM_QueueTbl_t *tbl)
{
    #ifdef WAIT_UNTIL_MS  // TODO 1 Fix bplib_os_wait_until_ms.
    uint64_t poll_time;
    #endif // WAIT_UNTIL_MS

    BPLib_MEM_OS_Lock(tbl->activity_lock);

    /* because the time is a 64-bit value and may not be atomic, it should
     * be sampled and updated inside of a lock section to ensure the value
     * is consistent */
    #ifdef WAIT_UNTIL_MS  // TODO 1 Fix bplib_os_wait_until_ms.
    poll_time = tbl->last_intf_poll.Time + BPLIB_INTF_MIN_POLL_INTERVAL;

    while (!tbl->maint_request_flag && BPLib_STOR_CACHE_OsGetDtntimeMs() < poll_time)
    {
         BPLib_STOR_CACHE_OsWaitUntilMs(tbl->activity_lock, poll_time);
    }
    #endif // WAIT_UNTIL_MS

    tbl->maint_request_flag = false;
    tbl->maint_active_flag  = true;

    BPLib_MEM_OS_Unlock(tbl->activity_lock);
}

void BPLib_STOR_QM_MaintenanceCompleteWait(BPLib_STOR_QM_QueueTbl_t *tbl)
{
    BPLib_MEM_OS_Lock(tbl->activity_lock);

    #ifdef WAIT_UNTIL_MS  // TODO 1 Fix bplib_os_wait_until_ms.
    while (tbl->maint_request_flag || tbl->maint_active_flag)
    {
        BPLib_STOR_CACHE_OsWaitUntilMs(tbl->activity_lock, BPLIB_DTNTIME_INFINITE);
    }
    #endif // WAIT_UNTIL_MS

    BPLib_MEM_OS_Unlock(tbl->activity_lock);
}

void BPLib_STOR_QM_ProcessActiveDucts(BPLib_STOR_QM_QueueTbl_t *tbl)
{
    #ifdef JOB
    BPLib_STOR_QM_JobRunAll(tbl->pool, tbl);
    #endif // JOB
}

void BPLib_STOR_QM_PeriodicMaintenance(BPLib_STOR_QM_QueueTbl_t *tbl)
{
    /* execute time-based interface polling for intfs that require it */
    BPLib_STOR_QM_DoTimedPoll(tbl);

    /* now forward any bundles between interfaces, based on active ducts */
    BPLib_STOR_QM_ProcessActiveDucts(tbl);

    /* do general pool garbage collection to make sure it was done at least once */
    BPLib_STOR_CACHE_Maintain(tbl->pool);

    BPLib_MEM_OS_Lock(tbl->activity_lock);
    tbl->maint_active_flag = false;
    // TODO OSAL BPLib_STOR_CACHE_OsBroadcastSignalAndUnlock(tbl->activity_lock);
}

#endif // QM