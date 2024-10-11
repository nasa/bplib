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

#ifndef BPLIB_STOR_QM_DUCTS_H
#define BPLIB_STOR_QM_DUCTS_H

#include <string.h>

#include "bplib_mem.h"

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"

/* JPHFIX: These are duplicate of the flow flags, should be consolidated */
#define BPLIB_INTF_STATE_ADMIN_UP 0x01
#define BPLIB_INTF_STATE_OPER_UP  0x02

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

#define BPLIB_CACHE_STATE_FLAG_ADMIN_UP 0x01
#define BPLIB_CACHE_STATE_FLAG_OPER_UP  0x02
#define BPLIB_CACHE_STATE_FLAG_STORAGE  0x04
#define BPLIB_CACHE_STATE_FLAG_ENDPOINT 0x08
#define BPLIB_CACHE_STATE_FLAG_POLL     0x10

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
#define BPLIB_MAX_SUBQ_DEPTH   0x10000000
#define BP_MPOOL_SHORT_SUBQ_DEPTH 0x10

/*
 * Enumeration that defines the various possible routing table events.  This enum
 * must always appear first in the structure that is the argument to the event handler,
 * and indicates the actual event that has occurred
 */
typedef enum
{
    BPLib_STOR_QM_DuctEventUndefined,
    BPLib_STOR_QM_DuctEventPoll,
    BPLib_STOR_QM_DuctEventUp,
    BPLib_STOR_QM_DuctEventDown,
    BPLib_STOR_QM_DuctEventMax

} BPLib_STOR_QM_DuctEvent_t;

typedef struct BPLib_STOR_QM_StateEvent
{
    BPLib_STOR_QM_DuctEvent_t event_type; /* must be first */
    bp_ipn_t                 dest;
    bp_ipn_t                 mask;
} BPLib_STOR_QM_StateEvent_t;

typedef struct BPLib_STOR_QM_DuctStatechangeEvent
{
    BPLib_STOR_QM_DuctEvent_t event_type; /* must be first */
    bp_handle_t              intf_id;
} BPLib_STOR_QM_DuctStatechangeEvent_t;

typedef union BPLib_STOR_QM_DuctGenericEvent
{
    BPLib_STOR_QM_DuctEvent_t             event_type;
    BPLib_STOR_QM_DuctStatechangeEvent_t intf_state;
} BPLib_STOR_QM_DuctGenericEvent_t;

/**
 * @brief Allocate a duct block
 *
 * @param pool
 * @param magic_number
 * @param init_arg Opaque pointer passed to initializer (may be NULL)
 * @return BPLib_STOR_CACHE_Block_t*
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_QM_DuctAlloc(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number, void *init_arg);

/**
 * @brief Drops the entire contents of a subq
 *
 * This purges the contents of a queue, such as when the interface is set to a "down" state,
 * this removes any pending items that were in it.
 *
 * @param subq
 * @return uint32_t
 */
uint32_t BPLib_STOR_QM_DuctDisable(BPLib_STOR_CACHE_SubqWorkitem_t *subq);
void     BPLib_STOR_QM_DuctEnable(BPLib_STOR_CACHE_SubqWorkitem_t *subq, uint32_t depth_limit);

bool BPLib_STOR_QM_DuctTryPush(BPLib_STOR_CACHE_SubqWorkitem_t *subq_dst, BPLib_STOR_CACHE_Block_t *qblk,
                               BPLib_TIME_MonotonicTime_t abs_timeout);
uint32_t BPLib_STOR_QM_DuctTryMoveAll(BPLib_STOR_CACHE_SubqWorkitem_t *subq_dst, BPLib_STOR_CACHE_SubqWorkitem_t *subq_src,
                                       BPLib_TIME_MonotonicTime_t abs_timeout);

BPLib_STOR_CACHE_Block_t *BPLib_STOR_QM_DuctTryPull(BPLib_STOR_CACHE_SubqWorkitem_t *subq_src, BPLib_TIME_MonotonicTime_t abs_timeout);

bool BPLib_STOR_QM_DuctModifyFlags(BPLib_STOR_CACHE_Block_t *cb, uint32_t set_bits, uint32_t clear_bits);

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

static inline bool BPLib_STOR_QM_DuctIsUp(const BPLib_STOR_QM_Duct_t *duct)
{
    return (duct->current_state_flags & (BPLIB_CACHE_STATE_FLAG_ADMIN_UP | BPLIB_CACHE_STATE_FLAG_OPER_UP)) == 0;
}

#endif /* BPLIB_STOR_QM_DUCTS_H */
