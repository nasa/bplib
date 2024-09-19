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

#ifndef BPLIB_CACHE_H
#define BPLIB_CACHE_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"

#include "bplib_api_types.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/
typedef enum BPLib_STOR_CACHE_Module_module_type
{
    BPLib_STOR_CACHE_Module_module_type_none,
    BPLib_STOR_CACHE_Module_module_type_internal,
    BPLib_STOR_CACHE_Module_module_type_offload
} BPLib_STOR_CACHE_Module_module_type_t;

typedef enum BPLib_STOR_CACHE_Module_module_valtype
{
    BPLib_STOR_CACHE_Module_module_valtype_none,
    BPLib_STOR_CACHE_Module_module_valtype_integer,
    BPLib_STOR_CACHE_Module_module_valtype_string
} BPLib_STOR_CACHE_Module_module_valtype_t;

typedef enum BPLib_STOR_CACHE_Module_confkey
{
    BPLib_STOR_CACHE_Module_confkey_none,
    BPLib_STOR_CACHE_Module_confkey_offload_base_dir,
} BPLib_STOR_CACHE_Module_confkey_t;

struct BPLib_STOR_CACHE_Module_module_api
{
    BPLib_STOR_CACHE_Module_module_type_t module_type;
    BPLib_STOR_CACHE_Block_t *(*instantiate)(BPLib_STOR_CACHE_Module_ref_t parent_ref, void *init_arg);
    int (*configure)(BPLib_STOR_CACHE_Block_t *svc, int key, BPLib_STOR_CACHE_Module_module_valtype_t vt, const void *val);
    int (*query)(BPLib_STOR_CACHE_Block_t *svc, int key, BPLib_STOR_CACHE_Module_module_valtype_t vt, const void **val);
    int (*start)(BPLib_STOR_CACHE_Block_t *svc);
    int (*stop)(BPLib_STOR_CACHE_Block_t *svc);
};

typedef struct BPLib_STOR_CACHE_Module_offload_api
{
    BPLib_STOR_CACHE_Module_module_api_t std;
    int (*offload)(BPLib_STOR_CACHE_Block_t *svc, bp_sid_t *sid, BPLib_STOR_CACHE_Block_t *pblk);
    int (*restore)(BPLib_STOR_CACHE_Block_t *svc, bp_sid_t sid, BPLib_STOR_CACHE_Block_t **pblk);
    int (*release)(BPLib_STOR_CACHE_Block_t *svc, bp_sid_t sid);
} BPLib_STOR_CACHE_Module_offload_api_t;

struct BPLib_STOR_CACHE_Module_block
{
    /* note that if it becomes necessary to recover bits here,
     * both the type and offset could be reduced in size */
    BPLib_STOR_CACHE_Module_blocktype_t   type;
    uint32_t                  parent_offset;
    struct BPLib_STOR_CACHE_Block *next;
    struct BPLib_STOR_CACHE_Block *prev;
};

/*
 * The basic types of blocks which are cacheable in the mem
 */
typedef struct BPLib_STOR_CACHE_Module_bblock_primary   BPLib_STOR_CACHE_Module_bblock_primary_t;
typedef struct BPLib_STOR_CACHE_Module_bblock_canonical BPLib_STOR_CACHE_Module_bblock_canonical_t;

typedef struct BPLib_STOR_CACHE_Module_subq_base BPLib_STOR_CACHE_Module_subq_base_t;
typedef struct BPLib_STOR_CACHE_Module_duct      BPLib_STOR_CACHE_Module_duct_t;

typedef enum BPLib_STOR_CACHE_Module_blocktype
{
    BPLib_STOR_CACHE_Module_blocktype_undefined = 0,

    /*
     * Note, the enum value here does matter -- these block types
     * are all refcount-capable, and thus are grouped together so this
     * can be implemented as a range check.  Do not change the
     * order of enum values without also updating the checks.
     */
    BPLib_STOR_CACHE_Module_blocktype_api       = 1,
    BPLib_STOR_CACHE_Module_blocktype_generic   = 2,
    BPLib_STOR_CACHE_Module_blocktype_primary   = 4,
    BPLib_STOR_CACHE_Module_blocktype_canonical = 5,
    BPLib_STOR_CACHE_Module_blocktype_flow      = 6,
    BPLib_STOR_CACHE_Module_blocktype_ref       = 7,
    BPLib_STOR_CACHE_Module_blocktype_max       = 8, /* placeholder for the max "regular" block type */

    /*
     * All of these block types are _not_ at the beginning of the structure,
     * these are members within the structures.  Their position within the parent
     * is indicated in the parent_offset field so the parent block pointer can be
     * reconstituted from one of these secondary indices.
     */

    BPLib_STOR_CACHE_Module_blocktype_secondary_generic = 100,
    BPLib_STOR_CACHE_Module_blocktype_list_head         = 101,
    BPLib_STOR_CACHE_Module_blocktype_job               = 102, /* a job or pending work item to do */
    BPLib_STOR_CACHE_Module_blocktype_secondary_max     = 103,

    /* The administrative block will be marked with 0xFF, this still permits the
     * type to be stored as a uint8_t if needed to save bits */
    BPLib_STOR_CACHE_Module_blocktype_admin = 255

} BPLib_STOR_CACHE_Module_blocktype_t;

/*
 * Enumeration that defines the various possible routing table events.  This enum
 * must always appear first in the structure that is the argument to the event handler,
 * and indicates the actual event that has occurred
 */
typedef enum
{
    BPLib_STOR_CACHE_Module_eventid_undefined,
    BPLib_STOR_CACHE_Module_eventid_recycle,
    BPLib_STOR_CACHE_Module_eventid_max

} BPLib_STOR_CACHE_Module_eventid_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Service API */
bp_handle_t BPLib_STOR_CACHE_Module_attach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *service_addr);
int         BPLib_STOR_CACHE_Module_detach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *service_addr);

bp_handle_t BPLib_STOR_CACHE_Module_register_module_service(bplib_routetbl_t *tbl, bp_handle_t cache_intf_id,
                                                const BPLib_STOR_CACHE_Module_module_api_t *api, void *init_arg);
int BPLib_STOR_CACHE_Module_configure(bplib_routetbl_t *tbl, bp_handle_t module_intf_id, int key, BPLib_STOR_CACHE_Module_module_valtype_t vt,
                          const void *val);
int BPLib_STOR_CACHE_Module_query(bplib_routetbl_t *tbl, bp_handle_t module_intf_id, int key, BPLib_STOR_CACHE_Module_module_valtype_t vt,
                      const void **val);
int BPLib_STOR_CACHE_Module_start(bplib_routetbl_t *tbl, bp_handle_t module_intf_id);
int BPLib_STOR_CACHE_Module_stop(bplib_routetbl_t *tbl, bp_handle_t module_intf_id);

void BPLib_STOR_CACHE_Module_debug_scan(bplib_routetbl_t *tbl, bp_handle_t intf_id);

/**
 * @brief Blocktype API
 *
 * Specifies functions for module-specific block operations,
 * including construction and destruction
 */
typedef struct BPLib_STOR_CACHE_Module_list_iter
{
    BPLib_STOR_CACHE_Block_t *position;
    BPLib_STOR_CACHE_Block_t *pending_entry;
} BPLib_STOR_CACHE_Module_list_iter_t;

/**
 * @brief Blocktype API
 *
 * Specifies functions for module-specific block operations,
 * including construction and destruction
 */
typedef struct BPLib_STOR_CACHE_Module_blocktype_api
{
    BPLib_STOR_CACHE_Module_callback_func_t construct; /**< Initialize a newly-created block */
    BPLib_STOR_CACHE_Module_callback_func_t destruct;  /**< De-initialize a recycled block */

} BPLib_STOR_CACHE_Module_blocktype_api_t;

/**
 * @brief Gets the next block in a list of blocks
 *
 * @param cb
 * @return BPLib_STOR_CACHE_Block_t*
 */
static inline BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_Module_get_next_block(BPLib_STOR_CACHE_Block_t *cb)
{
    return cb->next;
}

/**
 * @brief Gets the previous block in a list of blocks
 *
 * @param cb
 * @return BPLib_STOR_CACHE_Block_t*
 */
static inline BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_Module_get_prev_block(BPLib_STOR_CACHE_Block_t *cb)
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
static inline bool BPLib_STOR_CACHE_Module_is_link_attached(const BPLib_STOR_CACHE_Block_t *list)
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
static inline bool BPLib_STOR_CACHE_Module_is_link_unattached(const BPLib_STOR_CACHE_Block_t *list)
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
static inline bool BPLib_STOR_CACHE_Module_is_list_head(const BPLib_STOR_CACHE_Block_t *list)
{
    return (list->type == BPLib_STOR_CACHE_Module_blocktype_list_head);
}

/**
 * @brief Checks if this block is an empty list
 *
 * @param list
 * @return true If the list is empty
 * @return false If the list is not empty, or not a list head node
 */
static inline bool BPLib_STOR_CACHE_Module_is_empty_list_head(const BPLib_STOR_CACHE_Block_t *list)
{
    return (BPLib_STOR_CACHE_Module_is_list_head(list) && BPLib_STOR_CACHE_Module_is_link_unattached(list));
}

/**
 * @brief Checks if this block is a non-empty list
 *
 * @param list
 * @return true If the list is not empty
 * @return false If the list is empty, or not a list head node
 */
static inline bool BPLib_STOR_CACHE_Module_is_nonempty_list_head(const BPLib_STOR_CACHE_Block_t *list)
{
    return (BPLib_STOR_CACHE_Module_is_list_head(list) && BPLib_STOR_CACHE_Module_is_link_attached(list));
}

/**
 * @brief Checks if the block is some form of binary data block
 *
 * @param cb
 * @return true
 * @return false
 */
static inline bool BPLib_STOR_CACHE_Module_is_generic_data_block(const BPLib_STOR_CACHE_Block_t *cb)
{
    return (cb->type == BPLib_STOR_CACHE_Module_blocktype_generic);
}

/**
 * @brief Checks if the block is indirect (a reference)
 *
 * @param cb
 * @return true
 * @return false
 */
static inline bool BPLib_STOR_CACHE_Module_is_indirect_block(const BPLib_STOR_CACHE_Block_t *cb)
{
    return (cb->type == BPLib_STOR_CACHE_Module_blocktype_ref);
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
static inline bool BPLib_STOR_CACHE_Module_is_any_content_node(const BPLib_STOR_CACHE_Block_t *cb)
{
    return (cb->type > BPLib_STOR_CACHE_Module_blocktype_undefined && cb->type < BPLib_STOR_CACHE_Module_blocktype_max);
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
static inline bool BPLib_STOR_CACHE_Module_is_secondary_index_node(const BPLib_STOR_CACHE_Block_t *cb)
{
    return (cb->type >= BPLib_STOR_CACHE_Module_blocktype_secondary_generic && cb->type <= BPLib_STOR_CACHE_Module_blocktype_secondary_max);
}

static inline bp_handle_t BPLib_STOR_CACHE_Module_get_external_id(const BPLib_STOR_CACHE_Block_t *cb)
{
    return bp_handle_from_serial(cb->parent_offset, BPLIB_HANDLE_MPOOL_BASE);
}

BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_Block_from_external_id(BPLib_STOR_CACHE_Module_t *pool, bp_handle_t handle);

/* basic list iterators (forward or reverse) */

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Position an iterator at the first value in the list
 *
 * @param iter          The iterator object to position
 * @returns integer status code
 * @retval BP_SUCCESS if iterator is valid
 */
int BPLib_STOR_CACHE_Module_list_iter_goto_first(const BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Module_list_iter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Position an iterator at the last value in the list
 *
 * @param iter          The iterator object to position
 * @returns integer status code
 * @retval BP_SUCCESS if iterator is valid
 */
int BPLib_STOR_CACHE_Module_list_iter_goto_last(const BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Module_list_iter_t *iter);

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
int BPLib_STOR_CACHE_Module_list_iter_forward(BPLib_STOR_CACHE_Module_list_iter_t *iter);

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
int BPLib_STOR_CACHE_Module_list_iter_reverse(BPLib_STOR_CACHE_Module_list_iter_t *iter);

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
void BPLib_STOR_CACHE_Module_init_secondary_link(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Block_t *secondary_link,
                                     BPLib_STOR_CACHE_Module_blocktype_t block_type);

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
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_Module_get_block_from_link(BPLib_STOR_CACHE_Block_t *lblk);

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
size_t BPLib_STOR_CACHE_Module_get_user_data_offset_by_blocktype(BPLib_STOR_CACHE_Module_blocktype_t bt);

/**
 * @brief Gets the capacity (maximum size) of the generic data block
 *
 * This value is currently fixed at compile time, but the actual object definition
 * is not available outside the mem module, so this call can be used in place of sizeof()
 *
 * @param cb
 * @return size_t
 */
size_t BPLib_STOR_CACHE_Module_get_generic_data_capacity(const BPLib_STOR_CACHE_Block_t *cb);

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
 * BPLib_STOR_CACHE_Module_recycle_all_blocks_in_list()
 *
 * @param base_block The parent/container of the list
 * @param list_head  The list to initialize
 */
void BPLib_STOR_CACHE_Module_init_list_head(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Block_t *list_head);

/**
 * @brief Inserts a node after the given position or list head
 *
 * Note when inserting at a list head, it is essentially a mirror image (outside looking in)
 * Therefore this call will place a node at the _beginning_ of the list (prepend)
 *
 * @param list
 * @param node
 */
void BPLib_STOR_CACHE_Module_insert_after(BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Block_t *node);

/**
 * @brief Inserts a node before the given position or list head
 *
 * Note when inserting at a list head, it is essentially a mirror image (outside looking in)
 * Therefore this call will place a node at the _end_ of the list (append)
 *
 * @param list
 * @param node
 */
void BPLib_STOR_CACHE_Module_insert_before(BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Block_t *node);

/**
 * @brief Removes a node from its list
 *
 * The node becomes a singleton.
 * It is OK to invoke this on a node which is already a singleton, it will have no effect.
 *
 * @param node
 */
void BPLib_STOR_CACHE_Module_extract_node(BPLib_STOR_CACHE_Block_t *node);

/**
 * @brief Merge two lists together
 *
 * Note that the entire content is merged, including the head node.  After using this,
 * the BPLib_STOR_CACHE_Module_extract_node() should be used to remove one of the head nodes,
 * depending on which one should keep the contents.
 *
 * @param dest
 * @param src
 */
void BPLib_STOR_CACHE_Module_merge_list(BPLib_STOR_CACHE_Block_t *dest, BPLib_STOR_CACHE_Block_t *src);

/**
 * @brief Finds the parent mem container from any link pointer that was obtained from the pool
 *
 * @param cb
 * @return BPLib_STOR_CACHE_Module_t*
 */
BPLib_STOR_CACHE_Module_t *BPLib_STOR_CACHE_Module_get_parent_pool_from_link(BPLib_STOR_CACHE_Block_t *cb);

/**
 * @brief Cast a block to generic user data type
 *
 * User blocks require a match on on the "magic number", which may be a random 32-bit integer.
 * This is intended as a data integrity check to confirm the block is indeed the correct flavor.
 *
 * @param cb
 * @param required_magic
 * @return BPLib_STOR_CACHE_Module_generic_data_t*
 */
void *BPLib_STOR_CACHE_Module_generic_data_cast(BPLib_STOR_CACHE_Block_t *cb, uint32_t required_magic);

/**
 * @brief Cast a generic user data segment to its parent block
 *
 * This is the inverse of BPLib_STOR_CACHE_Module_generic_data_cast() and allows obtaining the pool
 * block from a generic data pointer.  The pointer passed in must be from a prior call
 * to BPLib_STOR_CACHE_Module_generic_data_cast() or else the result is undefined.
 *
 * @param blk pointer from previous call to BPLib_STOR_CACHE_Module_generic_data_cast
 * @param required_magic
 * @return BPLib_STOR_CACHE_Module_generic_data_t*
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_Module_generic_data_uncast(void *blk, BPLib_STOR_CACHE_Module_blocktype_t parent_bt,
                                                     uint32_t required_magic);

/**
 * @brief Gets the size of the user buffer associated with a data block
 *
 * For a CBOR block this is the length of the CBOR data within this block.  For a user
 * data block, this refers to the size of the actual user object stored here.
 *
 * @param cb pointer to block
 * @return size_t
 */
size_t BPLib_STOR_CACHE_Module_get_user_content_size(const BPLib_STOR_CACHE_Block_t *cb);

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
size_t BPLib_STOR_CACHE_Module_read_refcount(const BPLib_STOR_CACHE_Block_t *cb);

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
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_Module_generic_data_alloc(BPLib_STOR_CACHE_Module_t *pool, uint32_t magic_number, void *init_arg);

/**
 * @brief Recycle a single block which is no longer needed
 *
 * The block will be returned to the pool it originated from.
 *
 * @param blk
 */
void BPLib_STOR_CACHE_Module_recycle_block(BPLib_STOR_CACHE_Block_t *blk);

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
void BPLib_STOR_CACHE_Module_recycle_all_blocks_in_list(BPLib_STOR_CACHE_Module_t *pool, BPLib_STOR_CACHE_Block_t *list);

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
int BPLib_STOR_CACHE_Module_foreach_item_in_list(BPLib_STOR_CACHE_Block_t *list, bool always_remove,
                                     BPLib_STOR_CACHE_Module_callback_func_t callback_fn, void *callback_arg);

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
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_Module_search_list(const BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Module_callback_func_t match_fn,
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
uint32_t BPLib_STOR_CACHE_Module_collect_blocks(BPLib_STOR_CACHE_Module_t *pool, uint32_t limit);

/**
 * @brief Run basic maintenance on the memory pool
 *
 * Mainly, this performs any garbage-collection tasks on recycled memory blocks, returning the memory
 * back to the free pool so it can be used again.
 *
 * @param pool
 */
void BPLib_STOR_CACHE_Module_maintain(BPLib_STOR_CACHE_Module_t *pool);

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
int BPLib_STOR_CACHE_Module_register_blocktype(BPLib_STOR_CACHE_Module_t *pool, uint32_t magic_number, const BPLib_STOR_CACHE_Module_blocktype_api_t *api,
                                   size_t user_content_size);

#endif /* BPLIB_CACHE_H */
