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
#include "bplib_mem_rbtree.h"

#include "osapi.h"

/*
 * Randomly-chosen 32-bit static values that can be put into
 * data structures to help positively identify those structs later.
 */
#define BPLIB_MEM_SIGNATURE_STATE    0x6833597a
#define BPLIB_MEM_SIGNATURE_ENTRY    0xf223ff9f
#define BPLIB_MEM_SIGNATURE_BLOCKREF 0x77e961b1

/**
 * Initialize the MEM module's internal calls to OSAL.
 * Most of the internal MEM OS code came from the heritage bplib/os library.
 * It's only a subset of that library.
 */
void BPLib_MEM_OS_Init(void);

/* The OSAL "os" submodule does not provide bplib_os_log at this time, so its here.  This should be moved  */
int BPLIB_MEM_OS_Log(const char *file, unsigned int line, uint32_t *flags, uint32_t event, const char *fmt, ...);

/**
 * Lacking bplib_os.h and bplib/os, improvised MEM-local BPLib_MEM_OS_* calls.
 */
#define bplog(flags, evt, ...) BPLIB_MEM_OS_Log(__FILE__, __LINE__, flags, evt, __VA_ARGS__)

typedef struct BPLib_MEM_Lock
{
    bp_handle_t lock_id;
} BPLib_MEM_Lock_t;

typedef struct BPLib_MEM_Global
{
    BPLib_MEM_Lock_t BPLib_MEM_PoolLock;
} BPLib_MEM_Global_T;

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_OS_CreateLock -
 *-------------------------------------------------------------------------------------*/
bp_handle_t BPLib_MEM_OS_CreateLock(void);

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_OS_NextSerial -
 *-------------------------------------------------------------------------------------*/
unsigned int BPLib_MEM_OS_NextSerial(void);

#define BPLIB_MEM_DTNTIME_INFINITE UINT64_MAX // TODO Should be in bplib.h as BP_DTNTIME_INFINITE (or BPLIB_)

int BPLib_MEM_OS_WaitUntilMs(bp_handle_t h, uint64_t abs_dtntime_ms);

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

// TODO Add briefs for BPLib_MEM_OS_Lock and Unlock.
void BPLib_MEM_OS_Lock(bp_handle_t h);
void BPLib_MEM_OS_Unlock(bp_handle_t h);


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
    BPLib_MEM_OS_Lock(lock->lock_id);
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
    BPLib_MEM_OS_Unlock(lock->lock_id);
}

/**
 * @brief Prepares for resource-based locking
 *
 * Locates the correct lock to use for the given resource, but does not acquire the lock
 *
 * @note  it is imperative that all calls use the same reference address (such as the head
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

// TODO Add brief for SubqInit and BPLib_MEM_InsertBefore.
void BPLib_MEM_SubqInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_SubqBase_t *qblk);

void BPLib_MEM_InsertBefore(BPLib_MEM_Block_t *list, BPLib_MEM_Block_t *node);

uint64_t BPLib_MEM_OS_GetDtnTimeMs(void);

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
static inline uint32_t BPLib_MEM_SubqGetDepth(const BPLib_MEM_SubqBase_t *subq)
{
    return (subq->push_count - subq->pull_count);
}

/**
 * @brief Gets the offset of the block user content section
 *
 * Some block types have an area that may be used for general purpose data storage.
 * The size and offset of this area depends on the block type.  This gets the offset
 * of the start of the area for the given a block type.
 *
 * @param bt Block type
 * @return size_t
 */
size_t BPLib_MEM_GetUserDataOffsetByBlocktype(BPLib_MEM_Blocktype_t bt);

/**
 * @brief Garbage collection routine
 *
 * Processes the list of recycled blocks and performing any additional cleanup work
 *
 * @param pool The mpool object
 * @param limit The maximum number of entries to process
 *
 * @returns The number of blocks collected
 */
uint32_t BPLib_MEM_CollectBlocks(BPLib_MEM_Pool_t *pool, uint32_t limit);

// TODO add briefs for BPLib_MEM_GetGenericDataCapacity and BPLib_MEM_Maintain
size_t BPLib_MEM_GetGenericDataCapacity(const BPLib_MEM_Block_t *cb);

void BPLib_MEM_Maintain(BPLib_MEM_Pool_t *pool);

/**
 * @brief Inserts a node after the given position or list head
 *
 * Note when inserting at a list head, it is essentially a mirror image (outside looking in)
 * Therefore this call will place a node at the _beginning_ of the list (prepend)
 *
 * @param list
 * @param node
 */
void BPLib_MEM_InsertAfter(BPLib_MEM_Block_t *list, BPLib_MEM_Block_t *node);

/**
 * Subq-as-list iterator functions for operating on subqs with list-type operations.
 * CACHE uses subqs more extensively, but they are used in MEM for subqs
 * like the free_blocks and recycle_blocks subqs.
 */
typedef struct BPLib_MEM_ListIter
{
    BPLib_MEM_Block_t *position;
    BPLib_MEM_Block_t *pending_entry;
} BPLib_MEM_ListIter_t;

/* basic list iterators (forward or reverse) */

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Position an iterator at the first value in the list
 *
 * @param iter          The iterator object to position
 * @returns integer status code
 * @retval BPLIB_SUCCESS if iterator is valid
 */
int BPLib_MEM_ListIterGotoFirst(const BPLib_MEM_Block_t *list, BPLib_MEM_ListIter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Position an iterator at the last value in the list
 *
 * @param iter          The iterator object to position
 * @returns integer status code
 * @retval BPLIB_SUCCESS if iterator is valid
 */
int BPLib_MEM_ListIterGotoLast(const BPLib_MEM_Block_t *list, BPLib_MEM_ListIter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Move an iterator forward by one step in the tree
 *
 * Sets the iterator to the immediate successor of the current node.
 * This allows the caller to perform a ascending-order tree traversal.
 *
 * @param iter          The iterator object to move
 * @retval BPLIB_SUCCESS if iterator is valid
 */
int BPLib_MEM_ListIterForward(BPLib_MEM_ListIter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Move an iterator backward by one step in the tree
 *
 * Sets the iterator to the immediate predecessor of the current node.
 * This allows the caller to perform a descending-order tree traversal.
 *
 * @param iter          The iterator object to move
 * @retval BPLIB_SUCCESS if iterator is valid
 */
int BPLib_MEM_ListIterReverse(BPLib_MEM_ListIter_t *iter);

/**
 * @brief Process every item in the list in sequential order
 *
 * The callback function will be invoked for every item in the list, except for the head node.
 *
 * If "always_remove" is true, the item will be removed from the list prior to invoking the call.
 * In this case, the callback function must guarantee to place the block onto another list (or
 * some other tracking facility) to not leak blocks.
 *
 * @param list
 * @param always_remove
 * @param callback_fn
 * @param callback_arg
 * @return int Number of items that were in the list
 */
int BPLib_MEM_ForeachItemInList(BPLib_MEM_Block_t *list, bool always_remove,
                                BPLib_MEM_CallbackFunc_t callback_fn, void *callback_arg);

/**
 * @brief Search a list in sequential order
 *
 * The match function will be invoked for every entry in the list, and the supplied argument
 * will be passed to it. If the function returns 0, then the search stops and the node is returned.
 *
 * @param list The list to search
 * @param match_fn A function that should return 0 if a match is found, nonzero otherwise
 * @param match_arg An opaque argument passed to the match_fn, typically the match reference object
 * @return bplib_mpool_block_t* The matching list entry
 * @retval NULL if no match was found
 */
BPLib_MEM_Block_t *BPLib_MEM_SearchList(const BPLib_MEM_Block_t *list,
                                        BPLib_MEM_CallbackFunc_t match_fn,
                                        void *match_arg);

/**
 * @brief Append a single bundle to the given sub-queue
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq
 * @param cpb
 */
void BPLib_MEM_SubqPushSingle(BPLib_MEM_SubqBase_t *subq, BPLib_MEM_Block_t *cpb);

/**
 * @brief Get the next bundle from the given sub-queue
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq
 * @return BPLib_MEM_Block_t*
 */
BPLib_MEM_Block_t *BPLib_MEM_SubqPullSingle(BPLib_MEM_SubqBase_t *subq);

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

/* gets to the underlying block content (which may be a ref block) */
BPLib_MEM_BlockContent_t       *BPLib_MEM_GetBlockContent(BPLib_MEM_Block_t *cb);
const BPLib_MEM_BlockContent_t *BPLib_MEM_GetBlockContentConst(const BPLib_MEM_Block_t *cb);

/* similar to BPLib_MEM_GetBlockContent() but also dereferences any ref blocks */
BPLib_MEM_BlockContent_t *BPLib_MEM_BlockDereferenceContent(BPLib_MEM_Block_t *cb);

BPLib_MEM_BlockContent_t *BPLib_MEM_AllocBlockInternal(BPLib_MEM_Pool_t *pool, BPLib_MEM_Blocktype_t blocktype,
                                                       uint32_t content_type_signature, void *init_arg,
                                                       uint8_t priority);

// Mockups for CACHE entries.
typedef struct BPLib_MEM_EntryState
{
    BPLib_MEM_Block_t idle_list;
} BPLib_MEM_EntryState_t;

typedef struct BPLib_MEM_Entry BPLib_MEM_Entry_t;

struct BPLib_MEM_Entry
{
    BPLib_MEM_Entry_t      *parent;
    BPLib_MEM_EntryState_t state;
    uint32_t               flags;
};

#endif /* BPLIB_MEM_INTERNAL_H */
