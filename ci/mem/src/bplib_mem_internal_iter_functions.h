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

#ifndef BPLIB_MEM_INTERNAL_ITER_FUNCTIONS_H
#define BPLIB_MEM_INTERNAL_ITER_FUNCTIONS_H

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
BPLib_Status_t BPLib_MEM_ListIterGotoFirst(const BPLib_MEM_Block_t *list, BPLib_MEM_ListIter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Position an iterator at the last value in the list
 *
 * @param iter          The iterator object to position
 * @returns integer status code
 * @retval BPLIB_SUCCESS if iterator is valid
 */
BPLib_Status_t BPLib_MEM_ListIterGotoLast(const BPLib_MEM_Block_t *list, BPLib_MEM_ListIter_t *iter);

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
BPLib_Status_t BPLib_MEM_ListIterForward(BPLib_MEM_ListIter_t *iter);

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
BPLib_Status_t BPLib_MEM_ListIterReverse(BPLib_MEM_ListIter_t *iter);

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

BPLib_MEM_BlockContent_t *BPLib_MEM_AllocBlockInternal(BPLib_MEM_Pool_t *pool,
    BPLib_MEM_Blocktype_t blocktype, uint32_t content_type_signature, void *init_arg, uint8_t priority);

void BPLib_MEM_InitBaseObject(BPLib_MEM_BlockHeader_t *block_hdr, uint16_t user_content_length,
                                  uint32_t content_type_signature);

void BPLib_MEM_BblockPrimaryInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_BblockPrimary_t *pblk);

void BPLib_MEM_BblockCanonicalInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_BblockCanonical_t *cblk);

BPLib_MEM_Block_t *BPLib_MEM_BblockPrimaryAlloc(BPLib_MEM_Pool_t *pool, uint32_t magic_number,
                                                void *init_arg, uint8_t priority, BPLib_TIME_MonotonicTime_t timeout);

BPLib_MEM_Block_t *BPLib_MEM_BblockCanonicalAlloc(BPLib_MEM_Pool_t *pool, uint32_t magic_number, void *init_arg);

void BPLib_MEM_DuctInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_Duct_t *fblk);

void BPLib_MEM_JobInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_Job_t *jblk);

static inline bool BPLib_MEM_DuctIsUp(const BPLib_MEM_Duct_t *duct)
{
    return (duct->current_state_flags & (BPLIB_CACHE_STATE_FLAG_ADMIN_UP | BPLIB_CACHE_STATE_FLAG_OPER_UP)) == 0;
}

void BPLib_MEM_InitSecondaryLink(BPLib_MEM_Block_t *base_block, BPLib_MEM_Block_t *secondary_link,
                                 BPLib_MEM_Blocktype_t block_type);

#endif /* BPLIB_MEM_INTERNAL_ITER_FUNCTIONS_H */
