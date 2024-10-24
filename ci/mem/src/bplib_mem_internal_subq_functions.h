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

#ifndef BPLIB_MEM_INTERNAL_SUBQ_FUNCTIONS_H
#define BPLIB_MEM_INTERNAL_SUBQ_FUNCTIONS_H

/**
 * @brief Gets the next block in a list of blocks
 *
 * @param cb
 * @return BPLib_MEM_Block_t*
 */
static inline BPLib_MEM_Block_t *BPLib_MEM_GetNextBlock(BPLib_MEM_Block_t *cb)
{
    return cb->next;
}

/**
 * @brief Gets the previous block in a list of blocks
 *
 * @param cb
 * @return BPLib_MEM_Block_t*
 */
static inline BPLib_MEM_Block_t *BPLib_MEM_GetPrevBlock(BPLib_MEM_Block_t *cb)
{
    return cb->prev;
}

/**
 * @brief Checks if this block is part of a list
 *
 * @param list
 * @return true If this is in a list
 * @return false If the block is a singleton
 */
static inline bool BPLib_MEM_IsLinkAttached(const BPLib_MEM_Block_t *list)
{
    return (list->next != list);
}

/**
 * @brief Checks if this block is a singleton
 *
 * @param list
 * @return true If the block is a singleton
 * @return false If the block is part of a list
 */
static inline bool BPLib_MEM_IsLinkUnattached(const BPLib_MEM_Block_t *list)
{
    return (list->next == list);
}

/**
 * @brief Checks if this block is the head of a list
 *
 * This is both a start condition and an end condition when iterating a list
 *
 * @param list
 * @return true If the block is a head node
 * @return false If the block is not a head node
 */
static inline bool BPLib_MEM_IsListHead(const BPLib_MEM_Block_t *list)
{
    return (list->type == BPLib_MEM_BlocktypeListHead);
}

/**
 * @brief Checks if this block is an empty list
 *
 * @param list
 * @return true If the list is empty
 * @return false If the list is not empty, or not a list head node
 */
static inline bool BPLib_MEM_IsEmptyListHead(const BPLib_MEM_Block_t *list)
{
    return (BPLib_MEM_IsListHead(list) && BPLib_MEM_IsLinkUnattached(list));
}

/**
 * @brief Checks if this block is a non-empty list
 *
 * @param list
 * @return true If the list is not empty
 * @return false If the list is empty, or not a list head node
 */
static inline bool BPLib_MEM_IsNonEmptyListHead(const BPLib_MEM_Block_t *list)
{
    return (BPLib_MEM_IsListHead(list) && BPLib_MEM_IsLinkAttached(list));
}

/**
 * @brief Initialize a memory block sub-queue (subq)
 *
 * Memory block subqs are internal block lists such as the free blocks,
 * recycle blocks, and active blocks lists.
 *
 * @param base_block a pointer to a block intended to be the administrative block
 * @param qblk a pointer to the first block of the subq.
 * @returns void
 */
void BPLib_MEM_SubqInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_SubqBase_t *qblk);

/**
 * @brief Insert a memory block befor a block in a list of blocks
 *
 * Assuming all next and prev pointers are not NULL, assigns
 * the appropriate pointer values to insert the node before the list
 * element.
 *
 * @param list a pointer to a block intended to follow the inserted node
 * @param node a pointer to the block to be inserted.
 * @returns void
 */
void BPLib_MEM_InsertBefore(BPLib_MEM_Block_t *list, BPLib_MEM_Block_t *node);

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

/* basic list ops */

/**
 * @brief Sets the node to be an empty list head node
 *
 * Initialize a new list head object.
 *
 * Any existing content will be discarded, so this should typically only
 * be used on new blocks (such as a temporary list created on the stack)
 * where it is guaranteed to NOT have any valid content, and the object
 * is in an unknown/undefined state.
 *
 * To clear a list that has already been initialized once, use
 * BPLib_STOR_CACHE_RecycleAllBlocksInList()
 *
 * @param base_block The parent/container of the list
 * @param list_head  The list to initialize
 */
void BPLib_MEM_InitListHead(BPLib_MEM_Block_t *base_block, BPLib_MEM_Block_t *list_head);

/**
 * @brief Creates a memory pool object using a preallocated memory block
 *
 * @param pool_mem  Pointer to pool memory
 * @param pool_size Size of pool memory
 * @return BPLib_MEM_Pool_t*
 */
BPLib_MEM_Pool_t *BPLib_MEM_PoolCreate(void *pool_mem, size_t pool_size);

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_RecycleBlockInternal
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_RecycleBlockInternal(BPLib_MEM_Pool_t *pool, BPLib_MEM_Block_t *blk);

/**
 * @brief Recycle an entire list of blocks which are no longer needed
 *
 * The blocks will be returned to the pool.  All blocks in the list must have
 * been allocated from the same pool.
 *
 * The pool pointer may also be passed as NULL if the list object is known to be a
 * member of a block that was also allocted from the pool.  In this case, the pool will
 * be inferred from the list pointer.  However, if the list is a temporary object
 * and not connected to the originating pool, then the actual pool pointer must be
 * passed in.
 *
 * @param pool Originating pool, or NULL to infer/determine from list
 * @param list List of objects to recycle
 */
void BPLib_MEM_RecycleAllBlocksInList(BPLib_MEM_Pool_t *pool, BPLib_MEM_Block_t *list);

/**
 * @brief Recycle a single block which is no longer needed
 *
 * The block will be returned to the pool it originated from.
 *
 * @param blk
 */
void BPLib_MEM_RecycleBlock(BPLib_MEM_Block_t *blk);

/**
 * @brief Cast a block to a duct type
 *
 * @param cb
 * @return BPLib_MEM_Duct_t*
 */
BPLib_MEM_Duct_t *BPLib_MEM_DuctCast(BPLib_MEM_Block_t *cb);

typedef struct BPLib_MEM_State
{
    int placeholder;

    BPLib_IpnAddr_t self_addr;

    /*
     * pending_list holds bundle refs that are currently actionable in some way,
     * such as one of its timers getting reached, or the state flags changed.
     *
     * This may simply be re-classifying it into one of the other lists or indices.
     */
    BPLib_MEM_Block_t pending_list;

    BPLib_TIME_MonotonicTime_t action_time; /**< Monotonic time when the pending_list was last checked */

    /*
     * idle_list holds the items which do not fit into the other two lists.
     * They are not currently actionable, and placed here for holding.
     *
     * NOTE: this is just a sequential flat list for simplicity.  It can be
     * iterated, but that does not scale well.  One of the secondary indices
     * may be used for more efficient lookup.
     */
    BPLib_MEM_Block_t idle_list;

    BPLib_RBT_Root_t bundle_index;
    BPLib_RBT_Root_t dacs_index;
    BPLib_RBT_Root_t dest_eid_index;
    BPLib_RBT_Root_t time_index;

    // TODO const BPLib_STOR_PS_OffloadApi_t *offload_api;
    BPLib_MEM_Block_t         *offload_blk;


    uint32_t generated_dacs_seq;

    uint32_t fsm_state_enter_count[BPLib_MEM_EntryStateMax];
    uint32_t fsm_state_exit_count[BPLib_MEM_EntryStateMax];
    uint32_t discard_count;

} BPLib_MEM_State_t;

/* Allows reconstitution of the base block from a cache state pointer */
static inline BPLib_MEM_Block_t *BPLib_MEM_StateSeldblock(BPLib_MEM_State_t *state)
{
    /* any of the sub-lists can be used here, they should all trace back to the same parent */
    return BPLib_MEM_GetBlockFromLink(&state->pending_list);
}

/* Allows reconstitution of the duct object from a cache state pointer */
static inline BPLib_MEM_Duct_t *BPLib_MEM_GetDuct(BPLib_MEM_State_t *state)
{
    return BPLib_MEM_DuctCast(BPLib_MEM_StateSeldblock(state));
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

#endif /* BPLIB_MEM_INTERNAL_SUBQ_FUNCTIONS_H */
