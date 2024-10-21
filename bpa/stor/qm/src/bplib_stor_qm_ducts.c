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

#include <assert.h>
#include "bplib_api_types.h"

#include "bplib_mem.h"

#include "bplib_stor_cache_block.h"

#include "bplib_stor_qm_ducts.h"

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_SubqInit
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_QM_SubqInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_SubqBase_t *qblk)
{
    /* init the link structs */
    BPLib_STOR_QM_InitListHead(base_block, &qblk->block_list);
    qblk->pull_count = 0;
    qblk->push_count = 0;
}

void BPLib_STOR_QM_SubqWorkitemInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_QM_SubqWorkitem_t *wblk)
{
    BPLib_STOR_QM_JobInit(base_block, &wblk->job_header);
    BPLib_STOR_QM_SubqInit(base_block, &wblk->base_subq);
    wblk->current_depth_limit = 0;
}

static BPLib_Status_t BPLib_STOR_QM_DuctEventHandler(void *arg, BPLib_STOR_CACHE_Block_t *jblk)
{
    BPLib_STOR_CACHE_Block_t             *fblk;
    BPLib_STOR_QM_Duct_t              *duct;
    uint32_t                         changed_flags;
    BPLib_STOR_QM_DuctGenericEvent_t event;
    bool                             was_running;
    bool                             is_running;

    fblk = BPLib_STOR_QM_GetBlockFromLink(jblk);
    duct = BPLib_STOR_QM_DuctCast(fblk);
    if (duct == NULL)
    {
        return BPLIB_ERROR;
    }

    was_running   = BPLib_STOR_QM_DuctIsUp(duct);
    changed_flags = duct->pending_state_flags ^ duct->current_state_flags;
    duct->current_state_flags ^= changed_flags;
    is_running = BPLib_STOR_QM_DuctIsUp(duct);

    /* detect changes from up->down or vice versa */
    /* this is the combination of several flags, so its not simply checking changed_flags */
    if (was_running != is_running)
    {
        if (is_running)
        {
            event.intf_state.event_type = BPLib_STOR_QM_DuctEventUp;
        }
        else
        {
            event.intf_state.event_type = BPLib_STOR_QM_DuctEventDown;
        }

        event.intf_state.intf_id = BPLib_STOR_CACHE_GetExternalId(fblk);
        duct->statechange_job.event_handler(&event, fblk);
    }

    if (changed_flags & BPLIB_CACHE_STATE_FLAG_POLL)
    {
        event.event_type = BPLib_STOR_QM_DuctEventPoll;
        duct->statechange_job.event_handler(&event, fblk);
    }

    return BPLIB_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_DuctInit
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_QM_DuctInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_QM_Duct_t *fblk)
{
    /* now init the link structs */
    BPLib_STOR_QM_JobInit(base_block, &fblk->statechange_job.base_job);
    fblk->statechange_job.base_job.handler = BPLib_STOR_QM_DuctEventHandler;

    BPLib_STOR_QM_SubqWorkitemInit(base_block, &fblk->ingress);
    BPLib_STOR_QM_SubqWorkitemInit(base_block, &fblk->egress);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_SubqPushSingle
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_QM_SubqPushSingle(BPLib_STOR_CACHE_SubqBase_t *subq, BPLib_STOR_CACHE_Block_t *cpb)
{
    BPLib_STOR_QM_InsertBefore(&subq->block_list, cpb);
    ++subq->push_count;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_ListCountBlocks
 *
 *-----------------------------------------------------------------*/
uint32_t BPLib_STOR_QM_ListCountBlocks(BPLib_STOR_CACHE_Block_t *list)
{
    BPLib_STOR_CACHE_Block_t *node;
    uint32_t             count;

    node  = list->next;
    count = 0;
    while (node != list)
    {
        ++count;
        node = node->next;
    }

    return count;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_SubqMergeList
 *
 *-----------------------------------------------------------------*/
uint32_t BPLib_STOR_QM_SubqMergeList(BPLib_STOR_CACHE_SubqBase_t *subq_dst, BPLib_STOR_CACHE_Block_t *list)
{
    uint32_t block_count;

    /* for record-keeping, must count the blocks to know how many are being moved here */
    block_count = BPLib_STOR_QM_ListCountBlocks(list);

    BPLib_STOR_QM_MergeList(&subq_dst->block_list, list);
    BPLib_STOR_QM_ExtractNode(list);
    subq_dst->push_count += block_count;

    return block_count;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_SubqMoveAll
 *
 *-----------------------------------------------------------------*/
uint32_t BPLib_STOR_QM_SubqMoveAll(BPLib_STOR_CACHE_SubqBase_t *subq_dst, BPLib_STOR_CACHE_SubqBase_t *subq_src)
{
    uint32_t queue_depth;

    /* appends the entire subq to the destination */
    queue_depth = BPLib_STOR_QM_SubqGetDepth(subq_src);
    if (queue_depth > 0)
    {
        BPLib_STOR_QM_MergeList(&subq_dst->block_list, &subq_src->block_list);
        BPLib_STOR_QM_ExtractNode(&subq_src->block_list);
        subq_src->pull_count += queue_depth;
        subq_dst->push_count += queue_depth;
    }
    return queue_depth;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_SubqDropAll
 *
 *-----------------------------------------------------------------*/
uint32_t BPLib_STOR_QM_SubqDropAll(BPLib_STOR_CACHE_Pool_t *pool, BPLib_STOR_CACHE_SubqBase_t *subq)
{
    uint32_t queue_depth;

    /* appends the entire subq to the destination */
    queue_depth = BPLib_STOR_QM_SubqGetDepth(subq);
    if (queue_depth > 0)
    {
        BPLib_STOR_QM_RecycleAllBlocksInList(pool, &subq->block_list);
        subq->pull_count += queue_depth;
    }
    return queue_depth;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_SubqPullSingle
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_t *BPLib_STOR_QM_SubqPullSingle(BPLib_STOR_CACHE_SubqBase_t *subq)
{
    BPLib_STOR_CACHE_Block_t *node;

    /* if the head is reached here, then the list is empty */
    node = subq->block_list.next;
    if (BPLib_STOR_QM_IsListHead(node))
    {
        return NULL;
    }

    BPLib_STOR_QM_ExtractNode(node);
    ++subq->pull_count;

    return node;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_DuctCast
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_QM_Duct_t *BPLib_STOR_QM_DuctCast(BPLib_STOR_CACHE_Block_t *cb)
{
    BPLib_STOR_CACHE_BlockContent_t *content;

    content = BPLib_STOR_QM_BlockDereferenceContent(cb);
    if (content != NULL && content->header.base_link.type == BPLib_STOR_CACHE_BlocktypeDuct)
    {
        return &content->u.duct.dblock;
    }

    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_SubqWorkitemWaitForSpace
 *
 * Internal function, lock must be held when invoked
 *
 *-----------------------------------------------------------------*/
bool BPLib_STOR_QM_SubqWorkitemWaitForSpace(BPLib_MEM_Lock_t *lock, BPLib_STOR_QM_SubqWorkitem_t *subq,
                                              uint32_t quantity, uint64_t abs_timeout)
{
    uint32_t next_depth;
    bool     within_timeout;

    /* future depth after adding given quantity */
    next_depth     = BPLib_STOR_QM_SubqGetDepth(&subq->base_subq) + quantity;
    within_timeout = (abs_timeout != 0);
    while (next_depth > subq->current_depth_limit && within_timeout)
    {
        /* adding given quantity would overfill, wait for something else to pull */
        within_timeout = BPLib_STOR_QM_LockWait(lock, abs_timeout);
        next_depth     = BPLib_STOR_QM_SubqGetDepth(&subq->base_subq) + quantity;
    }

    return (next_depth <= subq->current_depth_limit);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_SubqWorkitemWaitForFill
 *
 * Internal function, lock must be held when invoked
 *
 *-----------------------------------------------------------------*/
bool BPLib_STOR_QM_SubqWorkitemWaitForFill(BPLib_MEM_Lock_t *lock, BPLib_STOR_QM_SubqWorkitem_t *subq,
                                             uint32_t quantity, uint64_t abs_timeout)
{
    uint32_t curr_depth;
    bool     within_timeout;

    curr_depth     = BPLib_STOR_QM_SubqGetDepth(&subq->base_subq);
    within_timeout = (abs_timeout != 0);
    while (curr_depth < quantity && within_timeout)
    {
        within_timeout = BPLib_STOR_QM_LockWait(lock, abs_timeout);
        curr_depth     = BPLib_STOR_QM_SubqGetDepth(&subq->base_subq);
    }

    return (curr_depth >= quantity);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_DuctTryPush
 *
 *-----------------------------------------------------------------*/
bool BPLib_STOR_QM_DuctTryPush(BPLib_STOR_QM_SubqWorkitem_t *subq_dst, BPLib_STOR_CACHE_Block_t *qblk, uint64_t abs_timeout)
{
    BPLib_MEM_Lock_t                     *lock;
    bool                                  got_space;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;
    BPLib_STOR_CACHE_Pool_t              *pool;

    pool  = BPLib_STOR_QM_GetParentPoolFromLink(&subq_dst->job_header.link);
    admin = BPLib_STOR_QM_GetAdmin(pool);
    lock  = BPLib_STOR_QM_LockResource(pool);

    got_space = BPLib_STOR_QM_SubqWorkitemWaitForSpace(lock, subq_dst, 1, abs_timeout);
    if (got_space)
    {
        /* this does not fail, but must be done under lock to keep things consistent */
        BPLib_STOR_QM_SubqPushSingle(&subq_dst->base_subq, qblk);

        /* mark the duct as "active" - done directly here while the lock is still held */
        BPLib_STOR_QM_JobMarkActiveInternal(&admin->active_list, &subq_dst->job_header);

        /* in case any threads were waiting on a non-empty queue */
        BPLib_STOR_QM_LockBroadcastSignal(lock);
    }

    BPLib_STOR_QM_LockRelease(lock);

    return got_space;

    return true;
}

BPLib_STOR_CACHE_Block_t *BPLib_STOR_QM_DuctTryPull(BPLib_STOR_QM_SubqWorkitem_t *subq_src, uint64_t abs_timeout)
{
    BPLib_MEM_Lock_t  *lock;
    BPLib_STOR_CACHE_Block_t *qblk;
    bool                 got_space;
    BPLib_STOR_CACHE_Pool_t       *pool;

    qblk = NULL;
    pool = BPLib_STOR_QM_GetParentPoolFromLink(&subq_src->job_header.link);
    lock = BPLib_STOR_QM_LockResource(pool);

    got_space = BPLib_STOR_QM_SubqWorkitemWaitForFill(lock, subq_src, 1, abs_timeout);
    if (got_space)
    {
        qblk = BPLib_STOR_QM_SubqPullSingle(&subq_src->base_subq);

        /* in case any threads were waiting on a non-full queue */
        BPLib_STOR_QM_LockBroadcastSignal(lock);
    }

    BPLib_STOR_QM_LockRelease(lock);

    return qblk;
}

uint32_t BPLib_STOR_QM_DuctTryMoveAll(BPLib_STOR_QM_SubqWorkitem_t *subq_dst, BPLib_STOR_QM_SubqWorkitem_t *subq_src,
                                       uint64_t abs_timeout)
{
    BPLib_MEM_Lock_t                     *lock;
    uint32_t                              prev_quantity;
    uint32_t                              quantity;
    bool                                  got_space;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;
    BPLib_STOR_CACHE_Pool_t              *pool;

    got_space = false;
    pool      = BPLib_STOR_QM_GetParentPoolFromLink(&subq_dst->job_header.link);
    admin     = BPLib_STOR_QM_GetAdmin(pool);
    lock      = BPLib_STOR_QM_LockResource(pool);

    /* note, there is a possibility that while waiting, another task puts more entries
     * into the source queue.  This loop will catch that and wait again.  However it
     * will not catch the case of another thread taking out of the source queue, as
     * it will still wait for the original amount. */
    quantity = BPLib_STOR_QM_SubqGetDepth(&subq_src->base_subq);
    do
    {
        prev_quantity = quantity;
        got_space     = BPLib_STOR_QM_SubqWorkitemWaitForSpace(lock, subq_dst, quantity, abs_timeout);
        quantity      = BPLib_STOR_QM_SubqGetDepth(&subq_src->base_subq);
    }
    while (got_space && quantity > prev_quantity);

    if (got_space)
    {
        /* this does not fail, but must be done under lock to keep things consistent */
        quantity = BPLib_STOR_QM_SubqMoveAll(&subq_dst->base_subq, &subq_src->base_subq);

        /* mark the duct as "active" - done directly here while the lock is still held */
        BPLib_STOR_QM_JobMarkActiveInternal(&admin->active_list, &subq_dst->job_header);

        /* in case any threads were waiting on a non-empty queue */
        BPLib_STOR_QM_LockBroadcastSignal(lock);
    }
    else
    {
        quantity = 0;
    }

    BPLib_STOR_QM_LockRelease(lock);

    return quantity;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_DuctDisable
 *
 *-----------------------------------------------------------------*/
uint32_t BPLib_STOR_QM_DuctDisable(BPLib_STOR_QM_SubqWorkitem_t *subq)
{
    BPLib_STOR_CACHE_Pool_t      *pool;
    BPLib_MEM_Lock_t *lock;
    uint32_t            quantity_dropped;

    pool = BPLib_STOR_QM_GetParentPoolFromLink(&subq->job_header.link);
    lock = BPLib_STOR_QM_LockResource(pool);

    /* prevents any additional entries in duct queues */
    subq->current_depth_limit = 0;
    quantity_dropped          = BPLib_STOR_QM_SubqDropAll(pool, &subq->base_subq);

    BPLib_STOR_QM_JobCancelInternal(&subq->job_header);

    BPLib_STOR_QM_LockRelease(lock);

    return quantity_dropped;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_DuctEnable
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_QM_DuctEnable(BPLib_STOR_QM_SubqWorkitem_t *subq, uint32_t depth_limit)
{
    BPLib_STOR_CACHE_Pool_t      *pool;
    BPLib_MEM_Lock_t *lock;

    pool = BPLib_STOR_QM_GetParentPoolFromLink(&subq->job_header.link);
    lock = BPLib_STOR_QM_LockResource(pool);

    /* prevents any additional entries in duct queues */
    subq->current_depth_limit = depth_limit;

    BPLib_STOR_QM_LockRelease(lock);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_DuctAlloc
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_t *BPLib_STOR_QM_DuctAlloc(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number, void *init_arg)
{
    BPLib_STOR_CACHE_BlockContent_t *result;
    BPLib_MEM_Lock_t          *lock;

    lock   = BPLib_STOR_QM_LockResource(pool);
    result = BPLib_STOR_QM_AllocBlockInternal(pool, BPLib_STOR_CACHE_BlocktypeDuct, magic_number, init_arg, BPLIB_MEM_ALLOC_PRI_LO);
    BPLib_STOR_QM_LockRelease(lock);

    return (BPLib_STOR_CACHE_Block_t *)result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_DuctModifyFlags
 *
 *-----------------------------------------------------------------*/
bool BPLib_STOR_QM_DuctModifyFlags(BPLib_STOR_CACHE_Block_t *cb, uint32_t set_bits, uint32_t clear_bits)
{
    BPLib_MEM_Lock_t                *lock;
    BPLib_STOR_CACHE_Pool_t                     *pool;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;
    BPLib_STOR_QM_Duct_t                *duct;
    uint32_t                           next_flags;
    bool                               flags_changed;

    pool  = BPLib_STOR_QM_GetParentPoolFromLink(cb);
    admin = BPLib_STOR_QM_GetAdmin(pool);
    lock  = BPLib_STOR_QM_LockResource(pool);
    duct  = BPLib_STOR_QM_DuctCast(cb);
    if (duct != NULL)
    {
        next_flags = duct->pending_state_flags;
        next_flags |= set_bits;
        next_flags &= ~clear_bits;
        flags_changed = (duct->pending_state_flags != next_flags);

        if (flags_changed)
        {
            duct->pending_state_flags = next_flags;
            BPLib_STOR_QM_JobMarkActiveInternal(&admin->active_list, &duct->statechange_job.base_job);
            BPLib_STOR_QM_LockBroadcastSignal(lock);
        }
    }
    else
    {
        flags_changed = false;
    }
    BPLib_STOR_QM_LockRelease(lock);

    return flags_changed;
}
