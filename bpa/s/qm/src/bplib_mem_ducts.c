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

/**
 * @file
 *
 * @brief Contains the memory pool implementation for blocks representing
 * flows of bundles (ingress and egress queue).
 *
 */

#include <string.h>
#include <assert.h>

#include "bplib.h"
#include "bplib_mem.h"
#include "bplib_mem_internal.h"
// STOR #include "bplib_s_qm_ducts.h"

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_subq_init
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_subq_init(bplib_mpool_block_t *base_block, bplib_mpool_subq_base_t *qblk)
{
    /* init the link structs */
    bplib_mpool_init_list_head(base_block, &qblk->block_list);
    qblk->pull_count = 0;
    qblk->push_count = 0;
}

void bplib_mpool_subq_workitem_init(bplib_mpool_block_t *base_block, bplib_mpool_subq_workitem_t *wblk)
{
    #ifdef STOR
    bplib_mpool_job_init(base_block, &wblk->job_header);
    #endif // STOR
    bplib_mpool_subq_init(base_block, &wblk->base_subq);
    wblk->current_depth_limit = 0;
}

#ifdef STOR
static int bplib_mpool_flow_event_handler(void *arg, bplib_mpool_block_t *jblk)
{
    bplib_mpool_block_t             *fblk;
    bplib_mpool_flow_t              *flow;
    uint32_t                         changed_flags;
    bplib_mpool_flow_generic_event_t event;
    bool                             was_running;
    bool                             is_running;

    fblk = bplib_mpool_get_block_from_link(jblk);
    flow = bplib_mpool_flow_cast(fblk);
    if (flow == NULL)
    {
        return -1;
    }

    was_running   = bplib_mpool_flow_is_up(flow);
    changed_flags = flow->pending_state_flags ^ flow->current_state_flags;
    flow->current_state_flags ^= changed_flags;
    is_running = bplib_mpool_flow_is_up(flow);

    /* detect changes from up->down or vice versa */
    /* this is the combination of several flags, so its not simply checking changed_flags */
    if (was_running != is_running)
    {
        if (is_running)
        {
            event.intf_state.event_type = bplib_mpool_flow_event_up;
        }
        else
        {
            event.intf_state.event_type = bplib_mpool_flow_event_down;
        }

        event.intf_state.intf_id = bplib_mpool_get_external_id(fblk);
        #ifdef STOR
        flow->statechange_job.event_handler(&event, fblk);
        #endif // STOR
    }

    if (changed_flags & BPLIB_MPOOL_FLOW_FLAGS_POLL)
    {
        event.event_type = bplib_mpool_flow_event_poll;
        #ifdef STOR
        flow->statechange_job.event_handler(&event, fblk);
        #endif // STOR
    }

    return 0;
}
#endif // STOR

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_flow_init
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_flow_init(bplib_mpool_block_t *base_block, bplib_mpool_flow_t *fblk)
{
    /* now init the link structs */
    #ifdef STOR
    bplib_mpool_job_init(base_block, &fblk->statechange_job.base_job);
    fblk->statechange_job.base_job.handler = bplib_mpool_flow_event_handler;
    #endif //STOR

    bplib_mpool_subq_workitem_init(base_block, &fblk->ingress);
    bplib_mpool_subq_workitem_init(base_block, &fblk->egress);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_subq_push_single
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_subq_push_single(bplib_mpool_subq_base_t *subq, bplib_mpool_block_t *cpb)
{
    bplib_mpool_insert_before(&subq->block_list, cpb);
    ++subq->push_count;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_list_count_blocks
 *
 *-----------------------------------------------------------------*/
uint32_t bplib_mpool_list_count_blocks(bplib_mpool_block_t *list)
{
    bplib_mpool_block_t *node;
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
 * Function: bplib_mpool_subq_merge_list
 *
 *-----------------------------------------------------------------*/
uint32_t bplib_mpool_subq_merge_list(bplib_mpool_subq_base_t *subq_dst, bplib_mpool_block_t *list)
{
    uint32_t block_count;

    /* for record-keeping, must count the blocks to know how many are being moved here */
    block_count = bplib_mpool_list_count_blocks(list);

    bplib_mpool_merge_list(&subq_dst->block_list, list);
    bplib_mpool_extract_node(list);
    subq_dst->push_count += block_count;

    return block_count;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_subq_move_all
 *
 *-----------------------------------------------------------------*/
uint32_t bplib_mpool_subq_move_all(bplib_mpool_subq_base_t *subq_dst, bplib_mpool_subq_base_t *subq_src)
{
    uint32_t queue_depth;

    /* appends the entire subq to the destination */
    queue_depth = bplib_mpool_subq_get_depth(subq_src);
    if (queue_depth > 0)
    {
        bplib_mpool_merge_list(&subq_dst->block_list, &subq_src->block_list);
        bplib_mpool_extract_node(&subq_src->block_list);
        subq_src->pull_count += queue_depth;
        subq_dst->push_count += queue_depth;
    }
    return queue_depth;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_subq_drop_all
 *
 *-----------------------------------------------------------------*/
uint32_t bplib_mpool_subq_drop_all(bplib_mpool_t *pool, bplib_mpool_subq_base_t *subq)
{
    uint32_t queue_depth;

    /* appends the entire subq to the destination */
    queue_depth = bplib_mpool_subq_get_depth(subq);
    if (queue_depth > 0)
    {
        bplib_mpool_recycle_all_blocks_in_list(pool, &subq->block_list);
        subq->pull_count += queue_depth;
    }
    return queue_depth;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_subq_pull_single
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_t *bplib_mpool_subq_pull_single(bplib_mpool_subq_base_t *subq)
{
    bplib_mpool_block_t *node;

    /* if the head is reached here, then the list is empty */
    node = subq->block_list.next;
    if (bplib_mpool_is_list_head(node))
    {
        return NULL;
    }

    bplib_mpool_extract_node(node);
    ++subq->pull_count;

    return node;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_flow_cast
 *
 *-----------------------------------------------------------------*/
bplib_mpool_flow_t *bplib_mpool_flow_cast(bplib_mpool_block_t *cb)
{
    #ifdef STOR

    bplib_mpool_block_content_t *content;

    content = bplib_mpool_block_dereference_content(cb);
    if (content != NULL && content->header.base_link.type == bplib_mpool_blocktype_flow)
    {
        return &content->u.flow.fblock;
    }

    #endif // STOR

    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_subq_workitem_wait_for_space
 *
 * Internal function, lock must be held when invoked
 *
 *-----------------------------------------------------------------*/
bool bplib_mpool_subq_workitem_wait_for_space(bplib_mpool_lock_t *lock, bplib_mpool_subq_workitem_t *subq,
                                              uint32_t quantity, uint64_t abs_timeout)
{
    uint32_t next_depth;
    bool     within_timeout;

    /* future depth after adding given quantity */
    next_depth     = bplib_mpool_subq_get_depth(&subq->base_subq) + quantity;
    within_timeout = (abs_timeout != 0);
    while (next_depth > subq->current_depth_limit && within_timeout)
    {
        /* adding given quantity would overfill, wait for something else to pull */
        within_timeout = bplib_mpool_lock_wait(lock, abs_timeout);
        next_depth     = bplib_mpool_subq_get_depth(&subq->base_subq) + quantity;
    }

    return (next_depth <= subq->current_depth_limit);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_subq_workitem_wait_for_fill
 *
 * Internal function, lock must be held when invoked
 *
 *-----------------------------------------------------------------*/
bool bplib_mpool_subq_workitem_wait_for_fill(bplib_mpool_lock_t *lock, bplib_mpool_subq_workitem_t *subq,
                                             uint32_t quantity, uint64_t abs_timeout)
{
    uint32_t curr_depth;
    bool     within_timeout;

    curr_depth     = bplib_mpool_subq_get_depth(&subq->base_subq);
    within_timeout = (abs_timeout != 0);
    while (curr_depth < quantity && within_timeout)
    {
        within_timeout = bplib_mpool_lock_wait(lock, abs_timeout);
        curr_depth     = bplib_mpool_subq_get_depth(&subq->base_subq);
    }

    return (curr_depth >= quantity);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_flow_try_push
 *
 *-----------------------------------------------------------------*/
bool bplib_mpool_flow_try_push(bplib_mpool_subq_workitem_t *subq_dst, bplib_mpool_block_t *qblk, uint64_t abs_timeout)
{
    #ifdef STOR

    bplib_mpool_lock_t                *lock;
    bool                               got_space;
    bplib_mpool_block_admin_content_t *admin;
    bplib_mpool_t                     *pool;

    pool  = bplib_mpool_get_parent_pool_from_link(&subq_dst->job_header.link);
    admin = bplib_mpool_get_admin(pool);
    lock  = bplib_mpool_lock_resource(pool);

    got_space = bplib_mpool_subq_workitem_wait_for_space(lock, subq_dst, 1, abs_timeout);
    if (got_space)
    {
        /* this does not fail, but must be done under lock to keep things consistent */
        bplib_mpool_subq_push_single(&subq_dst->base_subq, qblk);

        /* mark the flow as "active" - done directly here while the lock is still held */
        bplib_mpool_job_mark_active_internal(&admin->active_list, &subq_dst->job_header);

        /* in case any threads were waiting on a non-empty queue */
        bplib_mpool_lock_broadcast_signal(lock);
    }

    bplib_mpool_lock_release(lock);

    return got_space;

    #endif // STOR

    return true;
}

bplib_mpool_block_t *bplib_mpool_flow_try_pull(bplib_mpool_subq_workitem_t *subq_src, uint64_t abs_timeout)
{
    #ifdef STOR

    bplib_mpool_lock_t  *lock;
    bplib_mpool_block_t *qblk;
    bool                 got_space;
    bplib_mpool_t       *pool;

    qblk = NULL;
    pool = bplib_mpool_get_parent_pool_from_link(&subq_src->job_header.link);
    lock = bplib_mpool_lock_resource(pool);

    got_space = bplib_mpool_subq_workitem_wait_for_fill(lock, subq_src, 1, abs_timeout);
    if (got_space)
    {
        qblk = bplib_mpool_subq_pull_single(&subq_src->base_subq);

        /* in case any threads were waiting on a non-full queue */
        bplib_mpool_lock_broadcast_signal(lock);
    }

    bplib_mpool_lock_release(lock);

    return qblk;

    #endif // STOR

    return NULL;
}

uint32_t bplib_mpool_flow_try_move_all(bplib_mpool_subq_workitem_t *subq_dst, bplib_mpool_subq_workitem_t *subq_src,
                                       uint64_t abs_timeout)
{
    #ifdef STOR

    bplib_mpool_lock_t                *lock;
    uint32_t                           prev_quantity;
    uint32_t                           quantity;
    bool                               got_space;
    bplib_mpool_block_admin_content_t *admin;
    bplib_mpool_t                     *pool;

    got_space = false;
    pool      = bplib_mpool_get_parent_pool_from_link(&subq_dst->job_header.link);
    admin     = bplib_mpool_get_admin(pool);
    lock      = bplib_mpool_lock_resource(pool);

    /* note, there is a possibility that while waiting, another task puts more entries
     * into the source queue.  This loop will catch that and wait again.  However it
     * will not catch the case of another thread taking out of the source queue, as
     * it will still wait for the original amount. */
    quantity = bplib_mpool_subq_get_depth(&subq_src->base_subq);
    do
    {
        prev_quantity = quantity;
        got_space     = bplib_mpool_subq_workitem_wait_for_space(lock, subq_dst, quantity, abs_timeout);
        quantity      = bplib_mpool_subq_get_depth(&subq_src->base_subq);
    }
    while (got_space && quantity > prev_quantity);

    if (got_space)
    {
        /* this does not fail, but must be done under lock to keep things consistent */
        quantity = bplib_mpool_subq_move_all(&subq_dst->base_subq, &subq_src->base_subq);

        /* mark the flow as "active" - done directly here while the lock is still held */
        bplib_mpool_job_mark_active_internal(&admin->active_list, &subq_dst->job_header);

        /* in case any threads were waiting on a non-empty queue */
        bplib_mpool_lock_broadcast_signal(lock);
    }
    else
    {
        quantity = 0;
    }

    bplib_mpool_lock_release(lock);

    return quantity;

    #endif // STOR

    return 0;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_flow_disable
 *
 *-----------------------------------------------------------------*/
uint32_t bplib_mpool_flow_disable(bplib_mpool_subq_workitem_t *subq)
{
    #ifdef STOR

    bplib_mpool_t      *pool;
    bplib_mpool_lock_t *lock;
    uint32_t            quantity_dropped;

    pool = bplib_mpool_get_parent_pool_from_link(&subq->job_header.link);
    lock = bplib_mpool_lock_resource(pool);

    /* prevents any additional entries in flow queues */
    subq->current_depth_limit = 0;
    quantity_dropped          = bplib_mpool_subq_drop_all(pool, &subq->base_subq);

    bplib_mpool_job_cancel_internal(&subq->job_header);

    bplib_mpool_lock_release(lock);

    return quantity_dropped;

    #endif // STOR

    return 0;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_flow_enable
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_flow_enable(bplib_mpool_subq_workitem_t *subq, uint32_t depth_limit)
{
    #ifdef STOR

    bplib_mpool_t      *pool;
    bplib_mpool_lock_t *lock;

    pool = bplib_mpool_get_parent_pool_from_link(&subq->job_header.link);
    lock = bplib_mpool_lock_resource(pool);

    /* prevents any additional entries in flow queues */
    subq->current_depth_limit = depth_limit;

    bplib_mpool_lock_release(lock);

    #endif // STOR
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_flow_alloc
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_t *bplib_mpool_flow_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg)
{
    #ifdef STOR

    bplib_mpool_block_content_t *result;
    bplib_mpool_lock_t          *lock;

    lock   = bplib_mpool_lock_resource(pool);
    result = bplib_mpool_alloc_block_internal(pool, bplib_mpool_blocktype_flow, magic_number, init_arg, BPLIB_MPOOL_ALLOC_PRI_LO);
    bplib_mpool_lock_release(lock);

    return (bplib_mpool_block_t *)result;

    #endif // STOR

    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_flow_modify_flags
 *
 *-----------------------------------------------------------------*/
bool bplib_mpool_flow_modify_flags(bplib_mpool_block_t *cb, uint32_t set_bits, uint32_t clear_bits)
{
    #ifdef STOL

    bplib_mpool_lock_t                *lock;
    bplib_mpool_t                     *pool;
    bplib_mpool_block_admin_content_t *admin;
    bplib_mpool_flow_t                *flow;
    uint32_t                           next_flags;
    bool                               flags_changed;

    pool  = bplib_mpool_get_parent_pool_from_link(cb);
    admin = bplib_mpool_get_admin(pool);
    lock  = bplib_mpool_lock_resource(pool);
    flow  = bplib_mpool_flow_cast(cb);
    if (flow != NULL)
    {
        next_flags = flow->pending_state_flags;
        next_flags |= set_bits;
        next_flags &= ~clear_bits;
        flags_changed = (flow->pending_state_flags != next_flags);

        if (flags_changed)
        {
            flow->pending_state_flags = next_flags;
            bplib_mpool_job_mark_active_internal(&admin->active_list, &flow->statechange_job.base_job);
            bplib_mpool_lock_broadcast_signal(lock);
        }
    }
    else
    {
        flags_changed = false;
    }
    bplib_mpool_lock_release(lock);

    return flags_changed;

    #endif // STOR

    return true;
}
