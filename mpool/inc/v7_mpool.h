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

#ifndef V7_MPOOL_H
#define V7_MPOOL_H

#include <string.h>

#include "bplib_api_types.h"

/*
 * Priority levels for pool buffers -
 * The intent here is to give some preference to things which may decrease pool
 * memory usage (such as receipt of a DACS, which might allow dozens of
 * bundles to be deleted) over things that may increase pool memory usage
 * (such as an app sending in new data bundles).
 *
 * This only comes into play once the pool is mostly used.  As long as memory
 * use is low, everything gets allocated without issue.  Once memory becomes
 * constrained, things need to start becoming more choosy as to who gets the buffer.
 */
#define BPLIB_MPOOL_ALLOC_PRI_LO  0
#define BPLIB_MPOOL_ALLOC_PRI_MLO 63
#define BPLIB_MPOOL_ALLOC_PRI_MED 127
#define BPLIB_MPOOL_ALLOC_PRI_MHI 191
#define BPLIB_MPOOL_ALLOC_PRI_HI  255

/*
 * The basic types of blocks which are cacheable in the mpool
 */
typedef struct bplib_mpool_bblock_primary   bplib_mpool_bblock_primary_t;
typedef struct bplib_mpool_bblock_canonical bplib_mpool_bblock_canonical_t;

typedef struct bplib_mpool_subq_base bplib_mpool_subq_base_t;
typedef struct bplib_mpool_flow      bplib_mpool_flow_t;

typedef enum bplib_mpool_blocktype
{
    bplib_mpool_blocktype_undefined = 0,

    /*
     * Note, the enum value here does matter -- these block types
     * are all refcount-capable, and thus are grouped together so this
     * can be implemented as a range check.  Do not change the
     * order of enum values without also updating the checks.
     */
    bplib_mpool_blocktype_api       = 1,
    bplib_mpool_blocktype_generic   = 2,
    bplib_mpool_blocktype_primary   = 4,
    bplib_mpool_blocktype_canonical = 5,
    bplib_mpool_blocktype_flow      = 6,
    bplib_mpool_blocktype_ref       = 7,
    bplib_mpool_blocktype_max       = 8, /* placeholder for the max "regular" block type */

    /*
     * All of these block types are _not_ at the beginning of the structure,
     * these are members within the structures.  Their position within the parent
     * is indicated in the parent_offset field so the parent block pointer can be
     * reconstituted from one of these secondary indices.
     */

    bplib_mpool_blocktype_secondary_generic = 100,
    bplib_mpool_blocktype_list_head         = 101,
    bplib_mpool_blocktype_job               = 102, /* a job or pending work item to do */
    bplib_mpool_blocktype_secondary_max     = 103,

    /* The administrative block will be marked with 0xFF, this still permits the
     * type to be stored as a uint8_t if needed to save bits */
    bplib_mpool_blocktype_admin = 255

} bplib_mpool_blocktype_t;

struct bplib_mpool_block
{
    /* note that if it becomes necessary to recover bits here,
     * both the type and offset could be reduced in size */
    bplib_mpool_blocktype_t   type;
    uint32_t                  parent_offset;
    struct bplib_mpool_block *next;
    struct bplib_mpool_block *prev;
};

/*
 * Enumeration that defines the various possible routing table events.  This enum
 * must always appear first in the structure that is the argument to the event handler,
 * and indicates the actual event that has occurred
 */
typedef enum
{
    bplib_mpool_eventid_undefined,
    bplib_mpool_eventid_recycle,
    bplib_mpool_eventid_max

} bplib_mpool_eventid_t;

/**
 * @brief Blocktype API
 *
 * Specifies functions for module-specific block operations,
 * including construction and destruction
 */
typedef struct bplib_mpool_list_iter
{
    bplib_mpool_block_t *position;
    bplib_mpool_block_t *pending_entry;
} bplib_mpool_list_iter_t;

/**
 * @brief Blocktype API
 *
 * Specifies functions for module-specific block operations,
 * including construction and destruction
 */
typedef struct bplib_mpool_blocktype_api
{
    bplib_mpool_callback_func_t construct; /**< Initialize a newly-created block */
    bplib_mpool_callback_func_t destruct;  /**< De-initialize a recycled block */

} bplib_mpool_blocktype_api_t;

/**
 * @brief Gets the next block in a list of blocks
 *
 * @param cb
 * @return bplib_mpool_block_t*
 */
static inline bplib_mpool_block_t *bplib_mpool_get_next_block(bplib_mpool_block_t *cb)
{
    return cb->next;
}

/**
 * @brief Gets the previous block in a list of blocks
 *
 * @param cb
 * @return bplib_mpool_block_t*
 */
static inline bplib_mpool_block_t *bplib_mpool_get_prev_block(bplib_mpool_block_t *cb)
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
static inline bool bplib_mpool_is_link_attached(const bplib_mpool_block_t *list)
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
static inline bool bplib_mpool_is_link_unattached(const bplib_mpool_block_t *list)
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
static inline bool bplib_mpool_is_list_head(const bplib_mpool_block_t *list)
{
    return (list->type == bplib_mpool_blocktype_list_head);
}

/**
 * @brief Checks if this block is an empty list
 *
 * @param list
 * @return true If the list is empty
 * @return false If the list is not empty, or not a list head node
 */
static inline bool bplib_mpool_is_empty_list_head(const bplib_mpool_block_t *list)
{
    return (bplib_mpool_is_list_head(list) && bplib_mpool_is_link_unattached(list));
}

/**
 * @brief Checks if this block is a non-empty list
 *
 * @param list
 * @return true If the list is not empty
 * @return false If the list is empty, or not a list head node
 */
static inline bool bplib_mpool_is_nonempty_list_head(const bplib_mpool_block_t *list)
{
    return (bplib_mpool_is_list_head(list) && bplib_mpool_is_link_attached(list));
}

/**
 * @brief Checks if the block is some form of binary data block
 *
 * @param cb
 * @return true
 * @return false
 */
static inline bool bplib_mpool_is_generic_data_block(const bplib_mpool_block_t *cb)
{
    return (cb->type == bplib_mpool_blocktype_generic);
}

/**
 * @brief Checks if the block is indirect (a reference)
 *
 * @param cb
 * @return true
 * @return false
 */
static inline bool bplib_mpool_is_indirect_block(const bplib_mpool_block_t *cb)
{
    return (cb->type == bplib_mpool_blocktype_ref);
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
static inline bool bplib_mpool_is_any_content_node(const bplib_mpool_block_t *cb)
{
    return (cb->type >= bplib_mpool_blocktype_generic && cb->type < bplib_mpool_blocktype_max);
}

/**
 * @brief Checks if the block is any secondary index type
 *
 * These blocks are members of a larger block
 *
 * @param cb
 * @return true
 * @return false
 */
static inline bool bplib_mpool_is_secondary_index_node(const bplib_mpool_block_t *cb)
{
    return (cb->type >= bplib_mpool_blocktype_secondary_generic && cb->type <= bplib_mpool_blocktype_secondary_max);
}

static inline bp_handle_t bplib_mpool_get_external_id(const bplib_mpool_block_t *cb)
{
    return bp_handle_from_serial(cb->parent_offset, BPLIB_HANDLE_MPOOL_BASE);
}

bplib_mpool_block_t *bplib_mpool_block_from_external_id(bplib_mpool_t *pool, bp_handle_t handle);

/* basic list iterators (forward or reverse) */

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Position an iterator at the first value in the list
 *
 * @param iter          The iterator object to position
 * @returns integer status code
 * @retval BP_SUCCESS if iterator is valid
 */
int bplib_mpool_list_iter_goto_first(const bplib_mpool_block_t *list, bplib_mpool_list_iter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Position an iterator at the last value in the list
 *
 * @param iter          The iterator object to position
 * @returns integer status code
 * @retval BP_SUCCESS if iterator is valid
 */
int bplib_mpool_list_iter_goto_last(const bplib_mpool_block_t *list, bplib_mpool_list_iter_t *iter);

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
int bplib_mpool_list_iter_forward(bplib_mpool_list_iter_t *iter);

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
int bplib_mpool_list_iter_reverse(bplib_mpool_list_iter_t *iter);

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
void bplib_mpool_init_secondary_link(bplib_mpool_block_t *base_block, bplib_mpool_block_t *secondary_link,
                                     bplib_mpool_blocktype_t block_type);

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
 * @return bplib_mpool_block_t* pointer to container block
 * @retval NULL if the lblk pointer is not convertible to a content block
 */
bplib_mpool_block_t *bplib_mpool_get_block_from_link(bplib_mpool_block_t *lblk);

/**
 * @brief Gets the capacity (maximum size) of the generic data block
 *
 * This value is currently fixed at compile time, but the actual object definition
 * is not available outside the mpool module, so this call can be used in place of sizeof()
 *
 * @param cb
 * @return size_t
 */
size_t bplib_mpool_get_generic_data_capacity(const bplib_mpool_block_t *cb);

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
 * bplib_mpool_recycle_all_blocks_in_list()
 *
 * @param base_block The parent/container of the list
 * @param list_head  The list to initialize
 */
void bplib_mpool_init_list_head(bplib_mpool_block_t *base_block, bplib_mpool_block_t *list_head);

/**
 * @brief Inserts a node after the given position or list head
 *
 * Note when inserting at a list head, it is essentially a mirror image (outside looking in)
 * Therefore this call will place a node at the _beginning_ of the list (prepend)
 *
 * @param list
 * @param node
 */
void bplib_mpool_insert_after(bplib_mpool_block_t *list, bplib_mpool_block_t *node);

/**
 * @brief Inserts a node before the given position or list head
 *
 * Note when inserting at a list head, it is essentially a mirror image (outside looking in)
 * Therefore this call will place a node at the _end_ of the list (append)
 *
 * @param list
 * @param node
 */
void bplib_mpool_insert_before(bplib_mpool_block_t *list, bplib_mpool_block_t *node);

/**
 * @brief Removes a node from its list
 *
 * The node becomes a singleton.
 * It is OK to invoke this on a node which is already a singleton, it will have no effect.
 *
 * @param node
 */
void bplib_mpool_extract_node(bplib_mpool_block_t *node);

/**
 * @brief Merge two lists together
 *
 * Note that the entire content is merged, including the head node.  After using this,
 * the bplib_mpool_extract_node() should be used to remove one of the head nodes,
 * depending on which one should keep the contents.
 *
 * @param dest
 * @param src
 */
void bplib_mpool_merge_list(bplib_mpool_block_t *dest, bplib_mpool_block_t *src);

/**
 * @brief Finds the parent mpool container from any link pointer that was obtained from the pool
 *
 * @param cb
 * @return bplib_mpool_t*
 */
bplib_mpool_t *bplib_mpool_get_parent_pool_from_link(bplib_mpool_block_t *cb);

/**
 * @brief Cast a block to generic user data type
 *
 * User blocks require a match on on the "magic number", which may be a random 32-bit integer.
 * This is intended as a data integrity check to confirm the block is indeed the correct flavor.
 *
 * @param cb
 * @param required_magic
 * @return bplib_mpool_generic_data_t*
 */
void *bplib_mpool_generic_data_cast(bplib_mpool_block_t *cb, uint32_t required_magic);

/**
 * @brief Cast a generic user data segment to its parent block
 *
 * This is the inverse of bplib_mpool_generic_data_cast() and allows obtaining the pool
 * block from a generic data pointer.  The pointer passed in must be from a prior call
 * to bplib_mpool_generic_data_cast() or else the result is undefined.
 *
 * @param blk pointer from previous call to bplib_mpool_generic_data_cast
 * @param required_magic
 * @return bplib_mpool_generic_data_t*
 */
bplib_mpool_block_t *bplib_mpool_generic_data_uncast(void *blk, bplib_mpool_blocktype_t parent_bt,
                                                     uint32_t required_magic);

/**
 * @brief Gets the size of the user buffer associated with a data block
 *
 * For a CBOR block this is the length of the CBOR data within this block.  For a user
 * data block, this refers to the size of the actual user object stored here.
 *
 * @param ceb
 * @return size_t
 */
size_t bplib_mpool_get_user_content_size(const bplib_mpool_block_t *ceb);

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
size_t bplib_mpool_read_refcount(const bplib_mpool_block_t *cb);

/**
 * @brief Allocate a new user data block
 *
 * Note that the maximum block capacity is set at compile time.  This limits the size
 * of user data objects that can be stored by this mechanism.
 *
 * @param pool
 * @param magic_number
 * @param init_arg Opaque pointer passed to initializer (may be NULL)
 * @return bplib_mpool_block_t*
 */
bplib_mpool_block_t *bplib_mpool_generic_data_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg);

/**
 * @brief Recycle a single block which is no longer needed
 *
 * The block will be returned to the pool it originated from.
 *
 * @param blk
 */
void bplib_mpool_recycle_block(bplib_mpool_block_t *blk);

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
void bplib_mpool_recycle_all_blocks_in_list(bplib_mpool_t *pool, bplib_mpool_block_t *list);

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
int bplib_mpool_foreach_item_in_list(bplib_mpool_block_t *list, bool always_remove,
                                     bplib_mpool_callback_func_t callback_fn, void *callback_arg);

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
bplib_mpool_block_t *bplib_mpool_search_list(const bplib_mpool_block_t *list, bplib_mpool_callback_func_t match_fn,
                                             void *match_arg);

/**
 * @brief Run basic maintenance on the memory pool
 *
 * Mainly, this performs any garbage-collection tasks on recycled memory blocks, returning the memory
 * back to the free pool so it can be used again.
 *
 * @param pool
 */
void bplib_mpool_maintain(bplib_mpool_t *pool);

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
int bplib_mpool_register_blocktype(bplib_mpool_t *pool, uint32_t magic_number, const bplib_mpool_blocktype_api_t *api,
                                   size_t user_content_size);

/**
 * @brief Creates a memory pool object using a preallocated memory block
 *
 * @param pool_mem  Pointer to pool memory
 * @param pool_size Size of pool memory
 * @return bplib_mpool_t*
 */
bplib_mpool_t *bplib_mpool_create(void *pool_mem, size_t pool_size);

/**
 * @brief Obtain current usage of a memory pool
 *
 * @param pool Pool object
 * @return memory currently in use
 */
size_t bplib_mpool_query_mem_current_use(bplib_mpool_t *pool);

/**
 * @brief Obtain maximum usage of a memory pool
 *
 * @param pool Pool object
 * @return maximum memory used
 */
size_t bplib_mpool_query_mem_max_use(bplib_mpool_t *pool);

/* DEBUG/TEST verification routines */

void bplib_mpool_debug_scan(bplib_mpool_t *pool);
void bplib_mpool_debug_print_list_stats(bplib_mpool_block_t *list, const char *label);

#endif /* V7_MPOOL_H */
