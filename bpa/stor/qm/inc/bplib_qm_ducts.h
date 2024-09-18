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

#ifndef BPLIB_MEM_DUCTS_H
#define BPLIB_MEM_DUCTS_H

#include <string.h>

#include "bplib.h"
#include "bplib_api_types.h"
#include "bplib_mem.h"
#include "bplib_mem_bblocks.h"

#ifdef STOR
#include "v7_mpool_job.h"
#endif // STOR
#define BPLIB_MEM_FLOW_FLAGS_ADMIN_UP 0x01
#define BPLIB_MEM_FLOW_FLAGS_OPER_UP  0x02
#define BPLIB_MEM_FLOW_FLAGS_STORAGE  0x04
#define BPLIB_MEM_FLOW_FLAGS_ENDPOINT 0x08
#define BPLIB_MEM_FLOW_FLAGS_POLL     0x10

#ifdef STOR
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
#define BPLIB_MEM_MAX_SUBQ_DEPTH   0x10000000
#define BPLIB_MEM_SHORT_SUBQ_DEPTH 0x10
#endif // STOR
/*
 * Enumeration that defines the various possible routing table events.  This enum
 * must always appear first in the structure that is the argument to the event handler,
 * and indicates the actual event that has occurred
 */
typedef enum
{
    BPLib_STOR_CACHE_FlowEventUndefined,
    BPLib_STOR_CACHE_FlowEventPoll,
    BPLib_STOR_CACHE_FlowEventUp,
    BPLib_STOR_CACHE_FlowEventDown,
    BPLib_STOR_CACHE_FlowEventMax

} BPLib_STOR_CACHE_FlowEvent_t;

typedef struct bplib_route_state_event
{
    BPLib_STOR_CACHE_FlowEvent_t event_type; /* must be first */
    bp_ipn_t                 dest;
    bp_ipn_t                 mask;
} bplib_route_state_event_t;

typedef struct BPLib_STOR_CACHE_FlowStatechangeEvent
{
    BPLib_STOR_CACHE_FlowEvent_t event_type; /* must be first */
    bp_handle_t              intf_id;
} BPLib_STOR_CACHE_FlowStatechangeEvent_t;

typedef union BPLib_STOR_CACHE_FlowGenericEvent
{
    BPLib_STOR_CACHE_FlowEvent_t             event_type;
    BPLib_STOR_CACHE_FlowStatechangeEvent_t intf_state;
} BPLib_STOR_CACHE_FlowGenericEvent_t;

struct BPLib_MEM_flow
{
    uint32_t pending_state_flags;
    uint32_t current_state_flags;

#ifdef STOR
    BPLib_STOR_CACHE_JobStatechange_t statechange_job;
#endif //STOR

    BPLib_MEM_ref_t             parent;

    BPLib_STOR_CACHE_SubqWorkitem_t ingress;
    BPLib_STOR_CACHE_SubqWorkitem_t egress;
};

/**
 * @brief Cast a block to a flow type
 *
 * @param cb
 * @return BPLib_MEM_flow_t*
 */
BPLib_MEM_flow_t *BPLib_STOR_CACHE_FlowCast(BPLib_MEM_block_t *cb);

/**
 * @brief Allocate a flow block
 *
 * @param pool
 * @param magic_number
 * @param init_arg Opaque pointer passed to initializer (may be NULL)
 * @return BPLib_MEM_block_t*
 */
BPLib_MEM_block_t *BPLib_STOR_CACHE_FlowAlloc(BPLib_MEM_t *pool, uint32_t magic_number, void *init_arg);

#ifdef STOR
/**
 * @brief Drops the entire contents of a subq
 *
 * This purges the contents of a queue, such as when the interface is set to a "down" state,
 * this removes any pending items that were in it.
 *
 * @param subq
 * @return uint32_t
 */
uint32_t BPLib_STOR_CACHE_FlowDisable(BPLib_STOR_CACHE_SubqWorkitem_t *subq);
void     BPLib_STOR_CACHE_FlowEnable(BPLib_STOR_CACHE_SubqWorkitem_t *subq, uint32_t depth_limit);

bool BPLib_STOR_CACHE_FlowTryPush(BPLib_STOR_CACHE_SubqWorkitem_t *subq_dst, BPLib_MEM_block_t *qblk, uint64_t abs_timeout);
uint32_t BPLib_STOR_CACHE_FlowTryMoveAll(BPLib_STOR_CACHE_SubqWorkitem_t *subq_dst, BPLib_STOR_CACHE_SubqWorkitem_t *subq_src,
                                       uint64_t abs_timeout);

BPLib_MEM_block_t *BPLib_STOR_CACHE_FlowTryPull(BPLib_STOR_CACHE_SubqWorkitem_t *subq_src, uint64_t abs_timeout);

bool BPLib_STOR_CACHE_FlowModifyFlags(BPLib_MEM_block_t *cb, uint32_t set_bits, uint32_t clear_bits);

#endif // STOR

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
static inline uint32_t BPLib_STOR_CACHE_SubqGetDepth(const BPLib_STOR_CACHE_SubqBase_t *subq)
{
    return (subq->push_count - subq->pull_count);
}

#ifdef STOR
/**
 * @brief Check if given subq has any content
 *
 * @note This check is lockless, therefore the state of the subq may change at any time
 * if other threads/tasks are pulling from the same subq.  Therefore even if this function
 * returns true, it does not guarantee that a subsequent call to
 * BPLib_STOR_CACHE_SubqWorkitemTryPull() will return a valid block.
 *
 * @param subq
 * @retval true if subq has at least one entry
 */
static inline bool BPLib_STOR_CACHE_SubqWorkitemMayPull(const BPLib_STOR_CACHE_SubqWorkitem_t *subq)
{
    return (BPLib_STOR_CACHE_SubqGetDepth(&subq->base_subq) != 0);
}

/**
 * @brief Check if given subq is less than half full
 *
 * The check is for the "half full" point at which the application should generally stop
 * adding new entries to the queue, and shift focus toward draining the queue.  At the
 * half-full point there is still room in the queue so BPLib_STOR_CACHE_SubqWorkitemTryPush()
 * may still succeed even if this test returns false.
 *
 * @note This check is lockless, therefore the state of the subq may change at any time
 * if other threads/tasks are pushing from the same subq.  Therefore even if this function
 * returns true, it does not guarantee that a subsequent call to
 * BPLib_STOR_CACHE_SubqWorkitemTryPush() will succeed.
 *
 * @param subq
 * @retval true if subq is less than half full.
 */
static inline bool BPLib_STOR_CACHE_SubqWorkitemMayPush(const BPLib_STOR_CACHE_SubqWorkitem_t *subq)
{
    return (BPLib_STOR_CACHE_SubqGetDepth(&subq->base_subq) < (subq->current_depth_limit / 2));
}

#endif // STOR

static inline bool BPLib_STOR_CACHE_FlowIsUp(const BPLib_MEM_flow_t *flow)
{
    #ifdef STOR
    return (~flow->current_state_flags & (BPLIB_MEM_FLOW_FLAGS_ADMIN_UP | BPLIB_MEM_FLOW_FLAGS_OPER_UP)) == 0;
    #endif // STOR
    return (true);
}

#endif /* BPLIB_MEM_DUCTS_H */
