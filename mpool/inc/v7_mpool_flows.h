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

struct bplib_mpool_subq_base
{
    bplib_mpool_block_t block_list;

    /* note - "unsigned int" is chosen here as it is likely to be
     * a single-cycle read in most CPUs.  The range is not as critical
     * because what matters is the difference between these values.
     * The "volatile" qualification helps ensure the values are read as they
     * appear in code and are not rearranged by the compiler, as they could
     * be changed by other threads.  */
    volatile unsigned int push_count;
    volatile unsigned int pull_count;
};

typedef struct bplib_mpool_subq_depthlimit
{
    bplib_mpool_subq_base_t base_subq;
    unsigned int            current_depth_limit;
} bplib_mpool_subq_depthlimit_t;

struct bplib_mpool_flow
{
    bp_handle_t       external_id;
    bplib_mpool_ref_t parent;

    bplib_mpool_subq_depthlimit_t ingress;
    bplib_mpool_subq_depthlimit_t egress;
};

/**
 * @brief Cast a block to a flow type
 *
 * @param cb
 * @return bplib_mpool_flow_t*
 */
bplib_mpool_flow_t *bplib_mpool_flow_cast(bplib_mpool_block_t *cb);

/**
 * @brief Allocate a flow block
 *
 * @param pool
 * @param magic_number
 * @param init_arg Opaque pointer passed to initializer (may be NULL)
 * @return bplib_mpool_block_t*
 */
bplib_mpool_block_t *bplib_mpool_flow_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg);

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
uint32_t bplib_mpool_subq_depthlimit_disable(bplib_mpool_t *pool, bplib_mpool_subq_depthlimit_t *subq);

bool     bplib_mpool_subq_depthlimit_try_push(bplib_mpool_t *pool, bplib_mpool_ref_t flow_dst_ref,
                                              bplib_mpool_subq_depthlimit_t *subq_dst, bplib_mpool_block_t *qblk,
                                              uint64_t abs_timeout);
uint32_t bplib_mpool_subq_depthlimit_try_move_all(bplib_mpool_t *pool, bplib_mpool_ref_t flow_dst_ref,
                                                  bplib_mpool_subq_depthlimit_t *subq_dst,
                                                  bplib_mpool_subq_depthlimit_t *subq_src, uint64_t abs_timeout);

bplib_mpool_block_t *bplib_mpool_subq_depthlimit_try_pull(bplib_mpool_t *pool, bplib_mpool_ref_t flow_src_ref,
                                                          bplib_mpool_subq_depthlimit_t *subq_src,
                                                          uint64_t                       abs_timeout);

/**
 * @brief Get the current depth of a given subq
 *
 * @note If this is called outside of a lock, the results may be indeterminate because
 * the state may change by the time the result is returned.  However, since 32-bit reads
 * are generally atomic on most CPUs, it should be sufficiently safe to call while unlocked
 * if the caller ensures that the calling task is the only thread currently during
 * pushes or pulls - thereby ensuring that at least one of the values will be stable
 * between the check and the actual push/pull.  In the event that the value changes
 * between the time of check and time of use, it will only be "better" (that is, if the
 * caller is pulling, the depth can only go up if another thread pushes, and if the
 * caller is pushing, the depth can only go down if another thread pulls).
 *
 * @param subq
 * @returns Current depth of queue
 */
static inline uint32_t bplib_mpool_subq_get_depth(const bplib_mpool_subq_base_t *subq)
{
    return (subq->push_count - subq->pull_count);
}

/**
 * @brief Check if given subq has any content
 *
 * @note This check is lockless, therefore the state of the subq may change at any time
 * if other threads/tasks are pulling from the same subq.  Therefore even if this function
 * returns true, it does not guarantee that a subsequent call to
 * bplib_mpool_subq_depthlimit_try_pull() will return a valid block.
 *
 * @param subq
 * @retval true if subq has at least one entry
 */
static inline bool bplib_mpool_subq_depthlimit_may_pull(const bplib_mpool_subq_depthlimit_t *subq)
{
    return (bplib_mpool_subq_get_depth(&subq->base_subq) != 0);
}

/**
 * @brief Mark a given flow as active
 *
 * This marks it so it will be processed during the next call to forward data
 *
 * @note This is handled automatically by functions which append to a flow subq, such
 * as bplib_mpool_subq_depthlimit_try_push() and bplib_mpool_subq_depthlimit_try_move_all().
 * Applictions only need to explicitly call this API to mark it as active if there is
 * some other factor that requires it to be processed again.
 *
 * @param pool
 * @param flow_ref
 */
void bplib_mpool_flow_mark_active(bplib_mpool_t *pool, bplib_mpool_ref_t flow_ref);

/**
 * @brief Get the next active flow in the pool
 *
 * The given callback function will be invoked for all flows which are
 * currently marked as active
 *
 * @param pool
 * @return bplib_mpool_ref_t
 */
bplib_mpool_ref_t bplib_mpool_flow_get_next_active(bplib_mpool_t *pool);

#endif /* V7_MPOOL_FLOWS_H */
