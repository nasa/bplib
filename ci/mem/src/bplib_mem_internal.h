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

#ifndef BPLIB_MEM_INTERNAL_H
#define BPLIB_MEM_INTERNAL_H

#include <string.h>

#include "bplib.h"
#include "bplib_mem.h"
// #include "bplib_os.h" // TODO Unstub bplib_os functions.
#include "bplib_mem_rbtree.h"

typedef struct BPLib_MEM_Lock
{
    bp_handle_t lock_id;
} BPLib_MEM_Lock_t;

typedef struct BPLib_MEM_BlockHeader
{
    BPLib_MEM_Block_t base_link; /* must be first - this is the pointer used in the application */

    uint32_t content_type_signature; /* a "signature" (sanity check) value for identifying the data */
    uint16_t user_content_length;    /* actual length of user content (does not include fixed fields) */
    uint16_t refcount;               /* number of active references to the object */

} BPLib_MEM_BlockHeader_t;

/*
 * This union is just to make sure that the "content_start" field
 * in a generic data block is suitably aligned to hold any data type, mainly
 * the max-sized integers, max-sized floating point values, or a pointer.
 */
typedef union BPLib_MEM_AlignedData
{
    uint8_t     first_octet;
    uintmax_t   align_int;
    void       *align_ptr;
    long double align_float;
} BPLib_MEM_AlignedData_t;


/**
 * BPLib_MEM_ApiContent is a specialized variant of BPLib_STOR_CACHE_ApiContent.
 * 
 * BPLib_MEM_RBT is the only user in MEM.
 */
typedef struct BPLib_MEM_ApiContent
{
    BPLib_MEM_RBT_Link_t     rbt_link;
    BPLib_MEM_BlocktypeApi_t api;
    size_t                   user_content_size;
    BPLib_MEM_AlignedData_t  user_data_start;
} BPLib_MEM_ApiContent_t;

typedef struct BPLib_MEM_GenericDataContent
{
    BPLib_MEM_AlignedData_t user_data_start;
} BPLib_MEM_GenericDataContent_t;

#ifdef BPLIB_MEM_ABSTRACT_TYPES
struct BPLib_MEM
{
    BPLib_MEM_BlockContent_t admin_block; /**< Start of first real block (see num_bufs_total) */
};
#endif // BPLIB_MEM_ABSTRACT_TYPES

/**
 * @brief Gets the administrative block for the given pool
 *
 * This is always the first block in the pool.
 *
 * @param pool
 * @return BPLib_MEM_BlockAdminContent_t*
 */
static inline BPLib_MEM_BlockAdminContent_t *BPLib_MEM_GetAdmin(BPLib_MEM_Pool_t *pool)
{
    /* this just confirms that the passed-in pointer looks OK */
    return &pool->admin_block.u.admin;
}

/**
 * @brief Acquires a given lock
 *
 * The lock should be identified via BPLib_MEM_LockPrepare() or this
 * The lock should be identified via BPLib_MEM_LockPrepare() or this
 * can re-acquire the same lock again after releasing it with
 * BPLib_MEM_LockRelease().
 *
 * @param lock
 */
static inline void BPLib_MEM_LockAcquire(BPLib_MEM_Lock_t *lock)
{
    // bplib_os_lock(lock->lock_id); // TODO Unstub.
}

/**
 * @brief Release a given lock (simple)
 *
 * This simply unlocks a resource.
 *
 * This API will NOT automatically wake any task that might be waiting for a
 * state change of the underlying resource
 *
 * @param lock
 */
static inline void BPLib_MEM_LockRelease(BPLib_MEM_Lock_t *lock)
{
    // bplib_os_unlock(lock->lock_id); // TODO Unstub.
}

/**
 * @brief Indicate state change of the resource held by the lock
 *
 * Signals to other threads that state of the resource has changed
 *
 * Other threads waiting on the same lock are unblocked and should all re-check the condition
 * they are waiting on.
 *
 * @param lock
 */
static inline void BPLib_MEM_LockBroadcastSignal(BPLib_MEM_Lock_t *lock)
{
    // bplib_os_broadcast_signal(lock->lock_id); // TODO Unstub.
}

/**
 * @brief Prepares for resource-based locking
 *
 * Locates the correct lock to use for the given resource, but does not acquire the lock
 *
 * @note  it is imperative that all calls use the same referece address (such as the head
 * of the list) when referring to the same resource for locking to work correctly.
 *
 * @param resource_addr
 * @return BPLib_MEM_Lock_t*
 */
BPLib_MEM_Lock_t *BPLib_MEM_LockPrepare(void *resource_addr);

/**
 * @brief Lock a given resource
 *
 * Locates the correct lock to use for a given resource address, and also acquires it
 *
 * @note  it is imperative that all calls use the same referece address (such as the head
 * of the list) when referring to the same resource for locking to work correctly.
 *
 * @param resource_addr
 * @return BPLib_MEM_Lock_t*
 */
BPLib_MEM_Lock_t *BPLib_MEM_LockResource(void *resource_addr);

/**
 * @brief Waits for a state change related to the given lock
 *
 * @note The resource must be locked when called.
 *
 * @param lock
 * @param until_dtntime
 * @return true
 * @return false
 */
bool BPLib_MEM_LockWait(BPLib_MEM_Lock_t *lock, uint64_t until_dtntime);

#ifdef STOR // blocktype or subq
void BPLib_MEM_BblockPrimaryInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_BblockPrimary_t *pblk);
void BPLib_MEM_BblockCanonicalInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_BblockCanonical_t *cblk);
void BPLib_MEM_SubqInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_SubqBase_t *qblk);
void BPLib_MEM_FlowInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_Flow_t *fblk);

/**
 * @brief Append a single bundle to the given queue (flow)
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq
 * @param cpb
 */
void BPLib_MEM_SubqPushSingle(BPLib_MEM_SubqBase_t *subq, BPLib_MEM_Block_t *cpb);

/**
 * @brief Get the next bundle from the given queue (flow)
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq
 * @return BPLib_MEM_Block_t*
 */
BPLib_MEM_Block_t *BPLib_MEM_SubqPullSingle(BPLib_MEM_SubqBase_t *subq);

#endif // STOR blocktype or subq

/**
 * @brief Counts the number of blocks in a list
 *
 * For record-keeping or stats-reporting purposes, this counts the number of blocks in a list
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param list the subject list head
 * @return uint32_t number of blocks
 */
uint32_t BPLib_MEM_ListCountBlocks(BPLib_MEM_Block_t *list);

#ifdef STOR // subq
/**
 * @brief Pushes an entire list of blocks into a subq
 *
 * Similar to BPLib_MEM_SubqMoveAll() but the source is just a simple list rather
 * than a subq FIFO object.
 *
 * Because simple lists do not track the number of entries, this counts the blocks in the
 * list prior to moving the items, so the dest queue push stats will remain correct.
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq_dst the destination queue
 * @param list  the source list
 * @return uint32_t number of blocks moved
 */
uint32_t BPLib_MEM_SubqMergeList(BPLib_MEM_SubqBase_t *subq_dst, BPLib_MEM_Block_t *list);

/**
 * @brief Merges the entire contents of the source subq into the destination
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq_dst
 * @param subq_src
 * @return The number of queue entries moved
 */
uint32_t BPLib_MEM_SubqMoveAll(BPLib_MEM_SubqBase_t *subq_dst, BPLib_MEM_SubqBase_t *subq_src);
uint32_t BPLib_MEM_SubqMoveAll(BPLib_MEM_SubqBase_t *subq_dst, BPLib_MEM_SubqBase_t *subq_src);

/**
 * @brief Drops the entire contents of a subq
 *
 * This purges the contents of a queue, such as when the interface is set to a "down" state,
 * this removes any pending items that were in it.
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param pool
 * @param subq
 * @return uint32_t The number of queue entries dropped
 */
uint32_t BPLib_MEM_SubqDropAll(BPLib_MEM_Pool_t *pool, BPLib_MEM_SubqBase_t *subq);

#endif // STOR subq

#ifdef STOR // job
void BPLib_MEM_JobCancelInternal(BPLib_MEM_Job_t *job);
void BPLib_MEM_JobMarkActiveInternal(BPLib_MEM_Block_t *active_list, BPLib_MEM_Job_t *job);
#endif //STOR job

/* gets to the underlying block content (which may be a ref block) */
BPLib_MEM_BlockContent_t       *BPLib_MEM_GetBlockContent(BPLib_MEM_Block_t *cb);
const BPLib_MEM_BlockContent_t *BPLib_MEM_GetBlockContentConst(const BPLib_MEM_Block_t *cb);

/* similar to BPLib_MEM_GetBlockContent() but also dereferences any ref blocks */
BPLib_MEM_BlockContent_t *BPLib_MEM_BlockDereferenceContent(BPLib_MEM_Block_t *cb);

void BPLib_MEM_InitBaseObject(BPLib_MEM_BlockHeader_t *block_hdr, uint16_t user_content_length,
                                  uint32_t content_type_signature);

#ifdef STOR // blocktype
BPLib_MEM_BlockContent_t *BPLib_MEM_AllocBlockInternal(BPLib_MEM_Pool_t *pool, BPLib_MEM_Blocktype_t blocktype,
                                                              uint32_t content_type_signature, void *init_arg,
                                                              uint8_t priority);
#endif // STOR blocktype

#endif /* BPLIB_MEM_INTERNAL_H */
