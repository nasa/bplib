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

#ifndef V7_MPOOL_FLOWS_H
#define V7_MPOOL_FLOWS_H

#include <string.h>

#include "v7_mpool.h"

/**
 * @brief Upper limit to how deep a single queue may ever be
 *
 * This value is large enough such that a queue can essentially be considered unlimited.
 * Note that it is counting bundles, not bytes.
 *
 * Note that in some circumstances a queue can become "overfilled" - such as if two queues
 * that were individually less than this limit are merged together.  Even in that event,
 * there will still never be any ambiguity or chance of overrunning the counters used
 * to track queue depth (which are currently 32 bit values).
 */
#define BP_MPOOL_MAX_SUBQ_DEPTH 0x10000000

struct bplib_mpool_subq
{
    bplib_mpool_block_t block_listx;
    bplib_q_stats_t     stats;
    uint32_t            current_depth_limit;
};

struct bplib_mpool_flow
{
    bp_handle_t        external_id;
    bplib_mpool_subq_t ingress;
    bplib_mpool_subq_t egress;
    bplib_mpool_ref_t  parent;
};

/**
 * @brief Cast a block to a flow type
 *
 * @param cb
 * @return bplib_mpool_flow_t*
 */
bplib_mpool_flow_t *bplib_mpool_flow_cast(bplib_mpool_block_t *cb);

bplib_mpool_block_t *bplib_mpool_flow_alloc(bplib_mpool_t *pool, uint32_t magic_number, size_t req_capacity);

/**
 * @brief Append a single bundle to the given queue (flow)
 *
 * @param subq
 * @param cpb
 */
void bplib_mpool_subq_push_single(bplib_mpool_subq_t *subq, bplib_mpool_block_t *cpb);

/**
 * @brief Get the next bundle from the given queue (flow)
 *
 * @param subq
 * @return bplib_mpool_block_t*
 */
bplib_mpool_block_t *bplib_mpool_subq_pull_single(bplib_mpool_subq_t *subq);

/**
 * @brief Merges the entire contents of the source subq into the destination
 *
 * @param subq_dst
 * @param subq_src
 * @return The number of queue entries moved
 */
uint32_t bplib_mpool_subq_move_all(bplib_mpool_subq_t *subq_dst, bplib_mpool_subq_t *subq_src);

/**
 * @brief Drops the entire contents of a subq
 *
 * This purges the contents of a queue, such as when the interface is set to a "down" state,
 * this removes any pending items that were in it.
 *
 * @param pool
 * @param subq
 * @return uint32_t
 */
uint32_t bplib_mpool_subq_drop_all(bplib_mpool_t *pool, bplib_mpool_subq_t *subq);

/**
 * @brief Get the current depth of a given subq
 *
 * @param subq
 * @returns Current depth of queue
 */
static inline uint32_t bplib_mpool_subq_get_depth(bplib_mpool_subq_t *subq)
{
    return (subq->stats.push_count - subq->stats.pull_count);
}

/**
 * @brief Check if given subq can accomodate additional entries
 *
 * @param subq
 * @retval true if subq can accomodate at least one more entry
 */
static inline bool bplib_mpool_subq_may_push(bplib_mpool_subq_t *subq)
{
    return (bplib_mpool_subq_get_depth(subq) < subq->current_depth_limit);
}

/**
 * @brief Check if given subq has any content
 *
 * @param subq
 * @retval true if subq has at least one entry
 */
static inline bool bplib_mpool_subq_may_pull(bplib_mpool_subq_t *subq)
{
    return (bplib_mpool_subq_get_depth(subq) > 0);
}

/**
 * @brief Mark a given flow as active
 *
 * This marks it so it will be processed during the next call to forward data
 *
 * @param pool
 * @param flow
 */
void bplib_mpool_flow_mark_active(bplib_mpool_t *pool, bplib_mpool_ref_t flow_ref);

/**
 * @brief Process (forward) all active flows in the system
 *
 * The given callback function will be invoked for all flows which are
 * currently marked as active
 *
 * @param pool
 * @param callback_fn
 * @param callback_arg
 * @return int
 */
int bplib_mpool_flow_process_active(bplib_mpool_t *pool, bplib_mpool_callback_func_t callback_fn, void *callback_arg);

#endif /* V7_MPOOL_FLOWS_H */
