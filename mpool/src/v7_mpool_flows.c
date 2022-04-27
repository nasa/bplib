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
#include "bplib_os.h"
#include "v7_mpool_internal.h"

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_subq_init
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_subq_init(bplib_mpool_subq_t *qblk)
{
    /* init the link structs */
    bplib_mpool_init_list_head(&qblk->block_listx);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_subq_push_single
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_subq_push_single(bplib_mpool_subq_t *subq, bplib_mpool_block_t *cpb)
{
    /* checks that it is some form of primary bundle */
    assert(bplib_mpool_bblock_primary_cast(cpb) != NULL);
    bplib_mpool_insert_before(&subq->block_listx, cpb);
    ++subq->stats.push_count;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_subq_move_all
 *
 *-----------------------------------------------------------------*/
uint32_t bplib_mpool_subq_move_all(bplib_mpool_subq_t *subq_dst, bplib_mpool_subq_t *subq_src)
{
    uint32_t queue_depth;

    /* appends the entire subq to the destination */
    queue_depth = bplib_mpool_subq_get_depth(subq_src);
    if (queue_depth > 0)
    {
        bplib_mpool_merge_list(&subq_dst->block_listx, &subq_src->block_listx);
        bplib_mpool_extract_node(&subq_src->block_listx);
        subq_src->stats.pull_count += queue_depth;
        subq_dst->stats.push_count += queue_depth;
    }
    return queue_depth;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_subq_drop_all
 *
 *-----------------------------------------------------------------*/
uint32_t bplib_mpool_subq_drop_all(bplib_mpool_t *pool, bplib_mpool_subq_t *subq)
{
    uint32_t queue_depth;

    /* appends the entire subq to the destination */
    queue_depth = bplib_mpool_subq_get_depth(subq);
    if (queue_depth > 0)
    {
        bplib_mpool_recycle_all_blocks_in_list(pool, &subq->block_listx);
        subq->stats.pull_count += queue_depth;
    }
    return queue_depth;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_subq_pull_single
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_t *bplib_mpool_subq_pull_single(bplib_mpool_subq_t *subq)
{
    bplib_mpool_block_t *node = subq->block_listx.next;

    /* if the head is reached here, then the list is empty */
    if (bplib_mpool_is_list_head(node))
    {
        return NULL;
    }

    bplib_mpool_extract_node(node);
    ++subq->stats.pull_count;

    return node;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_flow_cast
 *
 *-----------------------------------------------------------------*/
bplib_mpool_flow_t *bplib_mpool_flow_cast(bplib_mpool_block_t *cb)
{
    bplib_mpool_block_content_t *content;

    content = (bplib_mpool_block_content_t *)bplib_mpool_obtain_base_block(cb);
    if (content != NULL && content->header.base_link.type == bplib_mpool_blocktype_flow)
    {
        return &content->u.flow.fblock;
    }

    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_flow_mark_active
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_flow_mark_active(bplib_mpool_t *pool, bplib_mpool_ref_t flow_ref)
{
    bplib_mpool_block_content_t *content;

    content = (bplib_mpool_block_content_t *)bplib_mpool_obtain_base_block(bplib_mpool_dereference(flow_ref));
    if (content != NULL && content->header.base_link.type == bplib_mpool_blocktype_flow)
    {
        bplib_mpool_extract_node(&content->u.flow.processable_link);
        bplib_mpool_insert_before(&pool->active_flow_list, &content->u.flow.processable_link);
    }
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_flow_init
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_flow_init(bplib_mpool_flow_t *fblk)
{
    /* now init the link structs */
    bplib_mpool_subq_init(&fblk->ingress);
    bplib_mpool_subq_init(&fblk->egress);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_flow_alloc
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_t *bplib_mpool_flow_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg)
{
    return (bplib_mpool_block_t *)bplib_mpool_alloc_block_internal(pool, bplib_mpool_blocktype_flow, magic_number,
                                                                   init_arg);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_flow_process_active
 *
 *-----------------------------------------------------------------*/
int bplib_mpool_flow_process_active(bplib_mpool_t *pool, bplib_mpool_callback_func_t callback_fn, void *callback_arg)
{
    bplib_mpool_block_t temp_list;
    int                 count;

    /*
     * To address the possibility that the callback puts the item into the same
     * list it was originally in, first make a separate temp list and move all
     * the items into that.  This should ensure that this only processes
     * the contents once, even if the callback puts it back into the orginal list.
     * While that still might create an infinite loop at the next level up,
     * it won't create an infinite loop here at least.
     */
    bplib_mpool_init_list_head(&temp_list);
    bplib_mpool_merge_list(&temp_list, &pool->active_flow_list);
    bplib_mpool_extract_node(&pool->active_flow_list);

    count = bplib_mpool_foreach_item_in_list(&temp_list, true, callback_fn, callback_arg);

    /* this should have moved everything - if non-empty then nodes will be leaked! */
    assert(bplib_mpool_is_empty_list_head(&temp_list));

    return count;
}
