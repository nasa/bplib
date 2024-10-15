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

#ifndef BPLIB_STOR_CACHE_BLOCK_H
#define BPLIB_STOR_CACHE_BLOCK_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>

#include "bplib_api_types.h"

#include "bplib_mem.h"
#include "bplib_mem_rbtree.h"

#include "bplib_stor_cache_types.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

// Defs for abstract types
struct BPLib_STOR_CACHE_BblockCanonicalContent
{
    BPLib_STOR_CACHE_BblockCanonical_t  cblock;
    BPLib_MEM_AlignedData_t             user_data_start;
};

struct BPLib_STOR_QM_DuctContent
{
    BPLib_STOR_QM_Duct_t             dblock;
    BPLib_MEM_AlignedData_t             user_data_start;
};

struct BPLib_STOR_CACHE_BlockRefContent
{
    BPLib_STOR_CACHE_Ref_t              pref_target;
    BPLib_MEM_AlignedData_t             user_data_start;
};

typedef union BPLib_STOR_CACHE_BlockBuffer
{
    BPLib_STOR_CACHE_GenericDataContent_t     generic_data;
    BPLib_STOR_CACHE_ModuleApiContent_t       api;
    BPLib_STOR_CACHE_BBlockPrimaryContent_t   primary;
    BPLib_STOR_CACHE_BblockCanonicalContent_t canonical;
    BPLib_STOR_QM_DuctContent_t               duct;
    BPLib_STOR_CACHE_BlockRefContent_t        ref;
    BPLib_STOR_CACHE_BlockAdminContent_t      admin;

    /* guarantees a minimum size of the generic data blocks, also determines the amount
     * of extra space available for user objects in other types of blocks. */
    uint8_t content_bytes[BPLIB_MEM_MIN_USER_BLOCK_SIZE];
} BPLib_STOR_CACHE_BlockBuffer_t;

struct BPLib_STOR_CACHE_BlockContent
{
    BPLib_STOR_CACHE_BlockHeader_t header; /* must be first */
    BPLib_STOR_CACHE_BlockBuffer_t u;
};

struct BPLib_STOR_CACHE_Pool
{
    BPLib_STOR_CACHE_BlockContent_t admin_block; /**< Start of first real block (see num_bufs_total) */
};

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BlockFromExternalId(BPLib_STOR_CACHE_Pool_t *pool, bp_handle_t handle);

/**
 * @brief Gets the logical information associated with a primary block
 *
 * @param cpb
 * @return BPLib_STOR_CACHE_BblockPrimary_t*
 */
static inline BPLib_STOR_CACHE_PrimaryBlock_t *BPLib_STOR_CACHE_BblockPrimaryGetLogical(BPLib_STOR_CACHE_BblockPrimary_t *cpb)
{
    return &cpb->data.logical;
}

/**
 * @brief Gets the list of encoded chunks associated with a primary block
 *
 * @param cpb
 * @return BPLib_STOR_CACHE_Block_t*
 */
static inline BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BblockPrimaryGetEncodedChunks(BPLib_STOR_CACHE_BblockPrimary_t *cpb)
{
    return &cpb->chunk_list;
}

/**
 * @brief Gets the list of canonical blocks associated with a primary block
 *
 * @param cpb
 * @return BPLib_STOR_CACHE_Block_t*
 */
static inline BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BblockPrimaryGetCanonicalList(BPLib_STOR_CACHE_BblockPrimary_t *cpb)
{
    return &cpb->cblock_list;
}

/**
 * @brief Gets the logical data associated with a canonical block
 *
 * @param ccb
 * @return bp_canonical_block_buffer_t*
 */
static inline bp_canonical_block_buffer_t *BPLib_STOR_CACHE_BblockCanonicalGetLogical(BPLib_STOR_CACHE_BblockCanonical_t *ccb)
{
    return &ccb->canonical_logical_data;
}

/**
 * @brief Gets the list of encoded chunks associated with a canonical block
 *
 * @param ccb
 * @return BPLib_STOR_CACHE_Block_t*
 */
static inline BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BblockCanonicalGetEncodedChunks(BPLib_STOR_CACHE_BblockCanonical_t *ccb)
{
    return &ccb->chunk_list;
}

/**
 * @brief Sets the offset and length of the content part of an encoded canonical block
 *
 * This refers to the position within the CBOR data of the actual content
 *
 * @param ccb
 * @param offset
 * @param length
 */
static inline void BPLib_STOR_CACHE_BblockCanonicalSetContentPosition(BPLib_STOR_CACHE_BblockCanonical_t *ccb, size_t offset,
                                                                     size_t length)
{
    ccb->encoded_content_offset = offset;
    ccb->encoded_content_length = length;
}

/**
 * @brief Gets the length of the content part of the encoded canonical block
 *
 * @param ccb
 * @return size_t
 */
static inline size_t BPLib_STOR_CACHE_BblockCanonicalGetContentLength(const BPLib_STOR_CACHE_BblockCanonical_t *ccb)
{
    return ccb->encoded_content_length;
}

/**
 * @brief Gets the offset of the content part of the encoded canonical block
 *
 * @param ccb
 * @return size_t
 */
static inline size_t BPLib_STOR_CACHE_BblockCanonicalGetContentOffset(const BPLib_STOR_CACHE_BblockCanonical_t *ccb)
{
    return ccb->encoded_content_offset;
}

/**
 * @brief Cast a block to a primary type
 *
 * Confirms that the block is actually a primary block (of any flavor) and returns
 * a pointer to that block, or returns NULL if not convertible to a primary block type.
 *
 * @param cb
 * @return BPLib_STOR_CACHE_BblockPrimary_t*
 */
BPLib_STOR_CACHE_BblockPrimary_t *BPLib_STOR_CACHE_BblockPrimaryCast(BPLib_STOR_CACHE_Block_t *cb);

/**
 * @brief Cast a block to a canonical type
 *
 * Confirms that the block is actually a canonical block (of any flavor) and returns
 * a pointer to that block, or returns NULL if not convertible to a canonical block type.
 *
 * @param cb
 * @return BPLib_STOR_CACHE_BblockCanonical_t*
 */
BPLib_STOR_CACHE_BblockCanonical_t *BPLib_STOR_CACHE_BblockCanonicalCast(BPLib_STOR_CACHE_Block_t *cb);

/**
 * @brief Cast a block to a CBOR data type
 *
 * Confirms that the block is a CBOR data block, and returns a pointer to that block
 *
 * @param cb
 * @return BPLib_STOR_CACHE_GenericData_t*
 */
void *BPLib_STOR_CACHE_BblockCborCast(BPLib_STOR_CACHE_Block_t *cb);

void BPLib_STOR_CACHE_BblockCborSetSize(BPLib_STOR_CACHE_Block_t *cb, size_t user_content_size);

/**
 * @brief Allocate a new primary block
 *
 * @param pool
 * @return BPLib_STOR_CACHE_Block_t*
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BblockPrimaryAlloc(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number,
                                                              void *init_arg, uint8_t priority, BPLib_TIME_MonotonicTime_t timeout);

/**
 * @brief Allocate a new canonical block
 *
 * @param pool
 * @return BPLib_STOR_CACHE_Block_t*
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BblockCanonicalAlloc(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number, void *init_arg);

/**
 * @brief Allocate a new CBOR data block
 *
 * @param pool
 * @return BPLib_STOR_CACHE_Block_t*
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BblockCborAlloc(BPLib_STOR_CACHE_Pool_t *pool);

/**
 * @brief Append CBOR data to the given list
 *
 * @param head
 * @param blk
 */
void BPLib_STOR_CACHE_BblockCborAppend(BPLib_STOR_CACHE_Block_t *head, BPLib_STOR_CACHE_Block_t *blk);

/**
 * @brief Append a canonical block to the bundle
 *
 * @param cpb
 * @param blk
 */
void BPLib_STOR_CACHE_BblockPrimaryAppend(BPLib_STOR_CACHE_BblockPrimary_t *cpb, BPLib_STOR_CACHE_Block_t *blk);

/**
 * @brief Find a canonical block within the bundle
 *
 * @param cpb
 * @param block_type
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BblockPrimaryLocateCanonical(BPLib_STOR_CACHE_BblockPrimary_t *cpb,
                                                                 bp_blocktype_t                block_type);

/**
 * @brief Drop all encode data (CBOR) from a primary block
 *
 * This would be used if the logical data has been changed, necessitating re-encoding of the data.
 * The old data blocks are returned to the pool, and the updated contents can replace it.
 *
 * @param cpb
 */
void BPLib_STOR_CACHE_BblockPrimaryDropEncode(BPLib_STOR_CACHE_BblockPrimary_t *cpb);

/**
 * @brief Drop all encode data (CBOR) from a canonical block
 *
 * This would be used if the logical data has been changed, necessitating re-encoding of the data.
 * The old data blocks are returned to the pool, and the updated contents can replace it.
 *
 * @param ccb
 */
void BPLib_STOR_CACHE_BblockCanonicalDropEncode(BPLib_STOR_CACHE_BblockCanonical_t *ccb);

/**
 * @brief Copy an entire chain of encoded blocks to a target buffer
 *
 * @param list
 * @param out_ptr
 * @param max_out_size
 * @param seek_start
 * @param max_count
 * @return size_t
 */
size_t BPLib_STOR_CACHE_BblockCborExport(BPLib_STOR_CACHE_Block_t *list, void *out_ptr, size_t max_out_size, size_t seek_start,
                                      size_t max_count);

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* STOR CACHE API */
/**
 * @brief Creates a memory pool object using a preallocated memory block
 *
 * @param pool_mem  Pointer to pool memory
 * @param pool_size Size of pool memory
 * @return BPLib_STOR_CACHE_Pool_t*
 */
BPLib_STOR_CACHE_Pool_t *BPLib_STOR_CACHE_Create(void *pool_mem, size_t pool_size);

/**
 * @brief Obtain current usage of a memory pool
 *
 * @param pool Pool object
 * @return memory currently in use
 */
size_t BPLib_STOR_CACHE_QueryMemCurrentUse(BPLib_STOR_CACHE_Pool_t *pool);

/**
 * @brief Obtain maximum usage of a memory pool
 *
 * @param pool Pool object
 * @return maximum memory used
 */
size_t BPLib_STOR_CACHE_QueryMemMaxUse(BPLib_STOR_CACHE_Pool_t *pool);

/**
 * @brief Gets the next block in a list of blocks
 *
 * @param cb
 * @return BPLib_STOR_CACHE_Block_t*
 */
static inline BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_GetNextBlock(BPLib_STOR_CACHE_Block_t *cb)
{
    return cb->next;
}

/**
 * @brief Gets the previous block in a list of blocks
 *
 * @param cb
 * @return BPLib_STOR_CACHE_Block_t*
 */
static inline BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_GetPrevBlock(BPLib_STOR_CACHE_Block_t *cb)
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
static inline bool BPLib_STOR_CACHE_IsLinkAttached(const BPLib_STOR_CACHE_Block_t *list)
{
    return (list->next != list);
}

/**
 * @brief Checks if this block is a singleon
 *
 * @param list
 * @return true If the block is a singleton
 * @return false If the block is part of a list
 */
static inline bool BPLib_STOR_CACHE_IsLinkUnattached(const BPLib_STOR_CACHE_Block_t *list)
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
static inline bool BPLib_STOR_CACHE_IsListHead(const BPLib_STOR_CACHE_Block_t *list)
{
    return (list->type == BPLib_STOR_CACHE_BlocktypeListHead);
}

/**
 * @brief Checks if this block is an empty list
 *
 * @param list
 * @return true If the list is empty
 * @return false If the list is not empty, or not a list head node
 */
static inline bool BPLib_STOR_CACHE_IsEmptyListHead(const BPLib_STOR_CACHE_Block_t *list)
{
    return (BPLib_STOR_CACHE_IsListHead(list) && BPLib_STOR_CACHE_IsLinkUnattached(list));
}

/**
 * @brief Checks if this block is a non-empty list
 *
 * @param list
 * @return true If the list is not empty
 * @return false If the list is empty, or not a list head node
 */
static inline bool BPLib_STOR_CACHE_IsNonemptyListHead(const BPLib_STOR_CACHE_Block_t *list)
{
    return (BPLib_STOR_CACHE_IsListHead(list) && BPLib_STOR_CACHE_IsLinkAttached(list));
}

/**
 * @brief Checks if the block is some form of binary data block
 *
 * @param cb
 * @return true
 * @return false
 */
static inline bool BPLib_STOR_CACHE_IsGenericDataBlock(const BPLib_STOR_CACHE_Block_t *cb)
{
    return (cb->type == BPLib_STOR_CACHE_BlocktypeGeneric);
}

/**
 * @brief Checks if the block is indirect (a reference)
 *
 * @param cb
 * @return true
 * @return false
 */
static inline bool BPLib_STOR_CACHE_IsIndirectBlock(const BPLib_STOR_CACHE_Block_t *cb)
{
    return (cb->type == BPLib_STOR_CACHE_BlocktypeRef);
}

/**
 * @brief Checks if the block is any valid content type
 *
 * This indicates blocks that have actual content
 * This is any block type other than a list head, free block, or
 * secondary index.
 *
 * @param cb
 * @return true
 * @return false
 */
static inline bool BPLib_STOR_CACHE_IsAnyContentNode(const BPLib_STOR_CACHE_Block_t *cb)
{
    return (cb->type > BPLib_STOR_CACHE_BlocktypeUndefined && cb->type < BPLib_STOR_CACHE_BlocktypeMax);
}

/**
 * @brief Checks if the block is any secondary index type
 *
 * These blocks are members of a larger block
 *
 * @param cbj
 * @return true
 * @return false
 */
static inline bool BPLib_STOR_CACHE_IsSecondaryIndexNode(const BPLib_STOR_CACHE_Block_t *cb)
{
    return (cb->type >= BPLib_STOR_CACHE_BlocktypeSecondaryGeneric && cb->type <= BPLib_STOR_CACHE_BlocktypeSecondaryMax);
}

static inline bp_handle_t BPLib_STOR_CACHE_GetExternalId(const BPLib_STOR_CACHE_Block_t *cb)
{
    return bp_handle_from_serial(cb->parent_offset, BPLIB_HANDLE_MPOOL_BASE);
}

/* basic list iterators (forward or reverse) */

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Position an iterator at the first value in the list
 *
 * @param iter          The iterator object to position
 * @returns integer status code
 * @retval BP_SUCCESS if iterator is valid
 */
int BPLib_STOR_CACHE_ListIterGotoFirst(const BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_ListIter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Position an iterator at the last value in the list
 *
 * @param iter          The iterator object to position
 * @returns integer status code
 * @retval BP_SUCCESS if iterator is valid
 */
int BPLib_STOR_CACHE_ListIterGotoLast(const BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_ListIter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Move an iterator forward by one step in the tree
 *
 * Sets the iterator to the immediate successor of the current node.
 * This allows the caller to perform a ascending-order tree traversal.
 *
 * @param iter          The iterator object to move
 * @retval BP_SUCCESS if iterator is valid
 */
int BPLib_STOR_CACHE_ListIterForward(BPLib_STOR_CACHE_ListIter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Move an iterator backward by one step in the tree
 *
 * Sets the iterator to the immediate predecessor of the current node.
 * This allows the caller to perform a descending-order tree traversal.
 *
 * @param iter          The iterator object to move
 * @retval BP_SUCCESS if iterator is valid
 */
int BPLib_STOR_CACHE_ListIterReverse(BPLib_STOR_CACHE_ListIter_t *iter);

/**
 * @brief Initialize a secondary link for block indexing purposes
 *
 * A secondary link may be required for cases where there needs to be more than one way of
 * indexing data blocks, for example in a case where blocks may need to be located by
 * DTN time or node number.
 *
 * The secondary index may be passed in place of the primary index to any "cast" function,
 * and it will automatically be converted back to a pointer to the same block.
 *
 * @param base_block
 * @param secondary_link
 */
void BPLib_STOR_CACHE_InitSecondaryLink(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Block_t *secondary_link,
                                     BPLib_STOR_CACHE_Blocktype_t block_type);

/**
 * @brief Obtain the pointer to the parent/containing block from a link
 *
 * If the block points to a secondary link structure, convert it back to the
 * parent block structure.
 *
 * If the passed in pointer already directly refers to a memory block then
 * that pointer is passed through.
 *
 * In all cases, a pointer to the actual content block is returned.
 *
 * @param[in] lblk link structure, such as from a list traversal.  May be a secondary link.
 * @return BPLib_STOR_CACHE_Block_t* pointer to container block
 * @retval NULL if the lblk pointer is not convertible to a content block
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_GetBlockFromLink(BPLib_STOR_CACHE_Block_t *lblk);

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
size_t BPLib_STOR_CACHE_GetUserDataOffsetByBlocktype(BPLib_STOR_CACHE_Blocktype_t bt);

/**
 * @brief Gets the capacity (maximum size) of the generic data block
 *
 * This value is currently fixed at compile time, but the actual object definition
 * is not available outside the mem module, so this call can be used in place of sizeof()
 *
 * @param cb
 * @return size_t
 */
size_t BPLib_STOR_CACHE_GetGenericDataCapacity(const BPLib_STOR_CACHE_Block_t *cb);

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
void BPLib_STOR_CACHE_InitListHead(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Block_t *list_head);

/**
 * @brief Inserts a node after the given position or list head
 *
 * Note when inserting at a list head, it is essentially a mirror image (outside looking in)
 * Therefore this call will place a node at the _beginning_ of the list (prepend)
 *
 * @param list
 * @param node
 */
void BPLib_STOR_CACHE_InsertAfter(BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Block_t *node);

/**
 * @brief Inserts a node before the given position or list head
 *
 * Note when inserting at a list head, it is essentially a mirror image (outside looking in)
 * Therefore this call will place a node at the _end_ of the list (append)
 *
 * @param list
 * @param node
 */
void BPLib_STOR_CACHE_InsertBefore(BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Block_t *node);

/**
 * @brief Removes a node from its list
 *
 * The node becomes a singleton.
 * It is OK to invoke this on a node which is already a singleton, it will have no effect.
 *
 * @param node
 */
void BPLib_STOR_CACHE_ExtractNode(BPLib_STOR_CACHE_Block_t *node);

/**
 * @brief Merge two lists together
 *
 * Note that the entire content is merged, including the head node.  After using this,
 * the BPLib_STOR_CACHE_ExtractNode() should be used to remove one of the head nodes,
 * depending on which one should keep the contents.
 *
 * @param dest
 * @param src
 */
void BPLib_STOR_CACHE_MergeList(BPLib_STOR_CACHE_Block_t *dest, BPLib_STOR_CACHE_Block_t *src);

/**
 * @brief Finds the parent mem container from any link pointer that was obtained from the pool
 *
 * @param cb
 * @return BPLib_STOR_CACHE_Pool_t*
 */
BPLib_STOR_CACHE_Pool_t *BPLib_STOR_CACHE_GetParentPoolFromLink(BPLib_STOR_CACHE_Block_t *cb);

/**
 * @brief Gets the size of the user buffer associated with a data block
 *
 * For a CBOR block this is the length of the CBOR data within this block.  For a user
 * data block, this refers to the size of the actual user object stored here.
 *
 * @param cb pointer to block
 * @return size_t
 */
size_t BPLib_STOR_CACHE_GetUserContentSize(const BPLib_STOR_CACHE_Block_t *cb);

/**
 * @brief Reads the reference count of the object
 *
 * Primary and canonical blocks have a reference count, allowing them to be quickly
 * duplicated (such as to keep one copy in a storage, while another sent to a CLA) without
 * actually copying the data itself.  The content blocks will be kept in the pool until
 * the refcount reaches zero, and then the memory blocks will be recycled.
 *
 * If this returns "1" it means that the given block pointer is currently the only reference to
 * this particular block (that is, it is not also present in an interface queue somewhere else)
 *
 * @param cb
 * @return size_t
 */
size_t BPLib_STOR_CACHE_ReadRefcount(const BPLib_STOR_CACHE_Block_t *cb);

/**
 * @brief Allocate a new user data block
 *
 * Note that the maximum block capacity is set at compile time.  This limits the size
 * of user data objects that can be stored by this mechanism.
 *
 * @param pool
 * @param magic_number
 * @param init_arg Opaque pointer passed to initializer (may be NULL)
 * @return BPLib_STOR_CACHE_Block_t*
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_GenericDataAlloc(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number, void *init_arg);

/**
 * @brief Recycle a single block which is no longer needed
 *
 * The block will be returned to the pool it originated from.
 *
 * @param blk
 */
void BPLib_STOR_CACHE_RecycleBlock(BPLib_STOR_CACHE_Block_t *blk);

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
void BPLib_STOR_CACHE_RecycleAllBlocksInList(BPLib_STOR_CACHE_Pool_t *pool, BPLib_STOR_CACHE_Block_t *list);

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
int BPLib_STOR_CACHE_ForeachItemInList(BPLib_STOR_CACHE_Block_t *list, bool always_remove,
BPLib_STOR_CACHE_CallbackFunc_t callback_fn, void *callback_arg);

/**
 * @brief Search a list in sequential order
 *
 * The match function will be invoked for every entry in the list, and the supplied argument
 * will be passed to it. If the function returns 0, then the search stops and the node is returned.
 *
 * @param list The list to search
 * @param match_fn A function that should return 0 if a match is found, nonzero otherwise
 * @param match_arg An opaque argument passed to the match_fn, typically the match reference object
 * @return BPLib_STOR_CACHE_Block_t* The matching list entry
 * @retval NULL if no match was found
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_SearchList(const BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_CallbackFunc_t match_fn,
                                               void *match_arg);

/**
 * @brief Garbage collection routine
 *
 * Processes the list of recycled blocks and performing any additional cleanup work
 *
 * @param pool The mem object
 * @param limit The maximum number of entries to process
 *
 * @returns The number of blocks collected
 */
uint32_t BPLib_STOR_CACHE_CollectBlocks(BPLib_STOR_CACHE_Pool_t *pool, uint32_t limit);

/**
 * @brief Run basic maintenance on the memory pool
 *
 * Mainly, this performs any garbage-collection tasks on recycled memory blocks, returning the memory
 * back to the free pool so it can be used again.
 *
 * @param pool
 */
void BPLib_STOR_CACHE_Maintain(BPLib_STOR_CACHE_Pool_t *pool);

/**
 * @brief Registers a given block type signature
 *
 * The api contains a constructor and destructor function, which will be invoked on newly allocated
 * and recycled blocks, respectively.  This should be invoked during startup/initialization for all
 * the services types being used.
 *
 * @note At the current time there is no defined method to unregister a block type, as types of services
 * in use are not expected to change dynamically at runtime.  It is also somewhat difficult to ensure that
 * there are no instances of the block type in existence in the pool.  If reconfiguration is required, the
 * entire pool should be reinitialized.
 *
 * @param pool
 * @param magic_number 32-bit Block identifier/signature
 * @param api Structure containing op callbacks
 * @param user_content_size Maximum size of user content associated with blocktype
 * @returns status code
 * @retval BP_SUCCESS if registration successful
 * @retval BP_DUPLICATE if the block type is already registered.
 */
int BPLib_STOR_CACHE_RegisterBlocktype(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number, const BPLib_STOR_CACHE_BlocktypeApi_t *api,
                                   size_t user_content_size);

// TODO Add briefs for copy full bundle in/out and compute full bundle size.
size_t BPLib_STOR_CACHE_CopyFullBundleOut(BPLib_STOR_CACHE_BblockPrimary_t *cpb, void *buffer, size_t buf_sz);

size_t BPLib_STOR_CACHE_CopyFullBundleIn(BPLib_STOR_CACHE_BblockPrimary_t *cpb, const void *buffer, size_t buf_sz);

size_t BPLib_STOR_CACHE_ComputeFullBundleSize(BPLib_STOR_CACHE_BblockPrimary_t *cpb);

#endif /* BPLIB_STOR_CACHE_BLOCK_H */
