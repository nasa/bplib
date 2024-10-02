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

#ifndef BPLIB_MEM_H
#define BPLIB_MEM_H

/*
** Include
*/

// TODO stdint.h, stddef.h, and stdbool.h belong in bplib_api_types.h.
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "bplib_api_types.h"

#include "bplib_mem_rbtree.h"

// TODO BPLIB_MEM_TIMEOUT should be BP_TIMEOUT in the BPLIB_SUCCESS list.
#define BPLIB_MEM_TIMEOUT -5 // TODO Based on OS_ERROR_TIMEOUT returned

// TODO BPLIB_FLAG_DIAGNOSTIC (from BP_FLAG_DIAGNOSTIC) should b in bplib.h
#define BPLIB_MEM_FLAG_DIAGNOSTIC              0x00000000

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
#define BPLIB_MEM_ALLOC_PRI_LO  0
#define BPLIB_MEM_ALLOC_PRI_MLO 63
#define BPLIB_MEM_ALLOC_PRI_MED 127
#define BPLIB_MEM_ALLOC_PRI_MHI 191
#define BPLIB_MEM_ALLOC_PRI_HI  255


#define BELONGS_IN_BPLIB_API_TYPES  // TODO This whole block of declarations belongs in bplib_api_types.h
#ifdef BELONGS_IN_BPLIB_API_TYPES

typedef struct bp_handle
{
    uint32_t hdl;
} bp_handle_t;

#define BP_INVALID_HANDLE \
    (const bp_handle_t)   \
    {                     \
        0                 \
    } /* used for integers (os locks, storage services) */

/**
 * Checks for validity of given handle
 *
 * @param h the handle value
 * @retval false if the handle is not possibly valid
 * @retval true if the handle may be valid
 */
static inline bool bp_handle_is_valid(bp_handle_t h)
{
    return (h.hdl != 0);
}

/**
 * Gets the "printable" value of a handle
 *
 * This may be used in conjunction with the "%d" format
 * specifier on printf()-style calls to display the logical
 * value of a handle.
 *
 * @param h the handle value
 * @returns handle value as an "int"
 */
static inline int bp_handle_printable(bp_handle_t h)
{
    return (int)h.hdl;
}

/**
 * Checks if two handles are equal
 *
 * Since handles are not integers, one cannot use the == operator
 * to check for equality.  This inline function may be used in its place.
 *
 * @param h1 the first handle value
 * @param h2 the second handle value
 * @retval false if the handles are different
 * @retval true if the handles are equal
 */
static inline bool bp_handle_equal(bp_handle_t h1, bp_handle_t h2)
{
    return (h1.hdl == h2.hdl);
}

/**
 * Converts the given handle to a serial number
 *
 * This determines the 0-based serial number corresponding to a handle,
 * which can in turn be used to index into a table/array.
 *
 * @sa bp_handle_from_serial
 *
 * @param h the handle value
 * @param base the object base handle (indicates the type/class of handle)
 *
 * @returns handle as a serial number
 */
static inline int bp_handle_to_serial(bp_handle_t h, bp_handle_t base)
{
    return (h.hdl - base.hdl);
}

/**
 * Converts the given serial number given to a handle
 *
 * This determines the handle value corresponding to a 0-based serial number.
 *
 * @sa bp_handle_to_serial
 *
 * @param hv   the handle value
 * @param base the object base handle (indicates the type/class of handle)
 *
 * @returns handle as a serial number
 */
static inline bp_handle_t bp_handle_from_serial(int hv, bp_handle_t base)
{
    return (bp_handle_t) {.hdl = (uint32_t)hv + base.hdl};
}

#define BPLIB_HANDLE_MAX_SERIAL 0xffffff

#define BPLIB_HANDLE_RAM_STORE_BASE \
    (bp_handle_t)                   \
    {                               \
        0x1000000                   \
    }
#define BPLIB_HANDLE_FLASH_STORE_BASE \
    (bp_handle_t)                     \
    {                                 \
        0x2000000                     \
    }
#define BPLIB_HANDLE_FILE_STORE_BASE \
    (bp_handle_t)                    \
    {                                \
        0x3000000                    \
    }
#define BPLIB_HANDLE_OS_BASE \
    (bp_handle_t)            \
    {                        \
        0x4000000            \
    }

#define BPLIB_HANDLE_INTF_BASE \
    (bp_handle_t)              \
    {                          \
        0x5000000              \
    }

// BPLIB_HANDLE_MPOOL_BASE is from heritage bplib_api_types.h, hence the "MPOOL".
#define BPLIB_HANDLE_MPOOL_BASE \
    (bp_handle_t)               \
    {                           \
        0x6000000               \
    }
#endif // BELONGS_IN_BPLIB_API_TYPES

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
 * BPLib STOR CACHE defines many blocktypes.
 * MEM only recognizes a few blocktypes, as listed here.
 * The administrative block will be marked with 0xFF, this still permits the
 * type to be stored as a uint8_t if needed to save bits.
 */

#define BPLIB_MEM_BLOCKTYPE_ADMIN 255

typedef enum BPLib_MEM_Blocktype
{
    /**
     * The complete list is in BPLib_STOR_CACHE_Blocktype_t.
     * The values match, mainly for ease of understanding and debugging.
     * Declarations in CACHE that are not used in MEM are not in this list, to
     * cause a build-time error if they are used.
     */

    BPLib_MEM_BlocktypeUndefined = 0,
    BPLib_MEM_BlocktypeApi       = 1,    // In MEM, used only for RBTree
    BPLib_MEM_BlocktypeGeneric   = 2,
    BPLib_MEM_BlocktypeMax       = 8,    // From BPLib_STOR_CACHE_BlocktypeMax

    BPLib_MEM_BlocktypeSecondaryGeneric = 100,
    BPLib_MEM_BlocktypeListHead         = 101,  // For Available and Free lists (subqs)
    BPLib_MEM_BlocktypeSecondaryMax     = 103,
    BPLib_MEM_BlocktypeAdmin            = 255
} BPLib_MEM_Blocktype_t;

typedef struct BPLib_MEM_Block BPLib_MEM_Block_t;

// TODO Return BPlib_MEM_Block_t to an abstract type. The abstract type belongs in bplib_api_types.h
struct BPLib_MEM_Block
{
    /* note that if it becomes necessary to recover bits here,
     * the offset could be reduced in size
     */
    BPLib_MEM_Blocktype_t     type;
    uint32_t                  parent_offset;
    BPLib_MEM_Block_t *next;
    BPLib_MEM_Block_t *prev;
};

typedef struct BPLib_MEM_BlockHeader
{
    BPLib_MEM_Block_t base_link; /* must be first - this is the pointer used in the application */

    uint32_t content_type_signature; /* a "signature" (sanity check) value for identifying the data */
    uint16_t user_content_length;    /* actual length of user content (does not include fixed fields) */
    uint16_t refcount;               /* number of active references to the object */

} BPLib_MEM_BlockHeader_t;

void BPLib_MEM_InitBaseObject(BPLib_MEM_BlockHeader_t *block_hdr, uint16_t user_content_length,
                                  uint32_t content_type_signature);

/**
 * @brief Callback function for various memory pool block actions
 *
 * This is a generic API for a function to handle various events/conditions
 * that occur at the block level.  The generic argument supplies the context
 * information.
 *
 * The return value depends on the context and may or may not be used, it
 * should should return 0 unless otherwise specified.
 */
typedef int (*BPLib_MEM_CallbackFunc_t)(void *, BPLib_MEM_Block_t *);

/**
 * @brief Blocktype API
 *
 * Specifies functions for module-specific block operations,
 * including construction and destruction
 */
typedef struct BPLib_MEM_BlocktypeApi
{
    BPLib_MEM_CallbackFunc_t construct; /**< Initialize a newly-created block */
    BPLib_MEM_CallbackFunc_t destruct;  /**< De-initialize a recycled block */

} BPLib_MEM_BlocktypeApi_t;

typedef struct BPLib_MEM_SubqBase
{
    BPLib_MEM_Block_t block_list;

    /* note - "unsigned int" is chosen here as it is likely to be
     * a single-cycle read in most CPUs.  The range is not as critical
     * because what matters is the difference between these values.
     * The "volatile" qualification helps ensure the values are read as they
     * appear in code and are not rearranged by the compiler, as they could
     * be changed by other threads.  */
    volatile unsigned int push_count;
    volatile unsigned int pull_count;
} BPLib_MEM_SubqBase_t;

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
 * BPLib_MEM_ApiContent is a specialized variant of BPLib_STOR_CACHE_ApiContent.
 * Blocks must have a non-NULL pointer to API Content to be allocated.
 */
typedef struct BPLib_MEM_ApiContent
{
    BPLib_MEM_RBT_Link_t     rbt_link;
    BPLib_MEM_BlocktypeApi_t api;
    size_t                   user_content_size;
    BPLib_MEM_AlignedData_t  user_data_start;
} BPLib_MEM_ApiContent_t;

typedef struct BPLib_MEM_BlockAdminContent
{
    size_t   buffer_size;
    uint32_t num_bufs_total;
    uint32_t bblock_alloc_threshold;   /**< threshold at which new bundles will no longer be allocatable */
    uint32_t internal_alloc_threshold; /**< threshold at which internal blocks will no longer be allocatable */
    uint32_t max_alloc_watermark;

    BPLib_MEM_RBT_Root_t       blocktype_registry; /**< registry of block signature values */
    BPLib_MEM_ApiContent_t blocktype_basic;    /**< a fixed entity in the registry for type 0 */
    BPLib_MEM_ApiContent_t blocktype_cbor;     /**< a fixed entity in the registry for CBOR blocks */

    BPLib_MEM_SubqBase_t free_blocks;    /**< blocks which are available for use */
    BPLib_MEM_SubqBase_t recycle_blocks; /**< blocks which can be garbage-collected */

    /* note that the active_list and managed_block_list are not FIFO in nature, as blocks
     * can be removed from the middle of the list or otherwise rearranged. Therefore a subq
     * is not used for these, because the push_count and pull_count would not remain accurate. */

    BPLib_MEM_Block_t active_list; /**< a list of flows/queues that need processing */

} BPLib_MEM_BlockAdminContent_t;

/**
 * @brief Gets the size of the user buffer associated with a data block
 *
 * @param cb pointer to block
 * @return size_t
 */
size_t BPLib_MEM_GetUserContentSize(const BPLib_MEM_Block_t *cb);

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
size_t BPLib_MEM_ReadRefCount(const BPLib_MEM_Block_t *cb);

/*
 * Minimum size of a generic data block
 */
#define BPLIB_MEM_MIN_USER_BLOCK_SIZE 480 // bytes

#define BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE 0x6b243e33

typedef struct BPLib_MEM_GenericDataContent
{
    BPLib_MEM_AlignedData_t user_data_start;
} BPLib_MEM_GenericDataContent_t;

// TODO flat_memory_buffer
typedef union BPLib_MEM_BlockBuffer
{
    BPLib_MEM_GenericDataContent_t     generic_data;
    BPLib_MEM_ApiContent_t              api;
    BPLib_MEM_BlockAdminContent_t      admin;

    /* guarantees a minimum size of the generic data blocks, also determines the amount
     * of extra space available for user objects in other types of blocks. */
    uint8_t content_bytes[BPLIB_MEM_MIN_USER_BLOCK_SIZE];
} BPLib_MEM_BlockBuffer_t;

// TODO Abstract eventually - MEM_BlockContent
typedef struct BPLib_MEM_BlockContent
{
    BPLib_MEM_BlockHeader_t header; /* must be first */
    BPLib_MEM_BlockBuffer_t u;
} BPLib_MEM_BlockContent_t;

// TODO Abstract eventually - MEM_Pool
typedef struct BPLib_MEM_Pool
{
    BPLib_MEM_BlockContent_t admin_block; /**< Start of first real block (see num_bufs_total) */
} BPLib_MEM_Pool_t;

#define BPLIB_MEM_GET_BUFFER_USER_START_OFFSET(m) (offsetof(BPLib_MEM_BlockBuffer_t, m.user_data_start))

#define BPLIB_MEM_GET_BLOCK_USER_CAPACITY(m) (sizeof(BPLib_MEM_BlockBuffer_t) - MPOOL_GET_BUFFER_USER_START_OFFSET(m))

/**
 * @brief Checks if the block is indirect (a reference)
 *
 * @param cb
 * @return true
 * @return false
 */
static inline bool BPLib_MEM_IsIndirectBlock(const BPLib_MEM_Block_t *cb)
{
    // return (cb->type == BPLib_MEM_BlocktypeRef);

    // There are no Refs in MEM.
    return false;
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
static inline bool BPLib_MEM_IsAnyContentNode(const BPLib_MEM_Block_t *cb)
{
    return (cb->type > BPLib_MEM_BlocktypeUndefined && cb->type < BPLib_MEM_BlocktypeMax);
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
static inline bool BPLib_MEM_IsSecondaryIndexNode(const BPLib_MEM_Block_t *cb)
{
    return (cb->type >= BPLib_MEM_BlocktypeSecondaryGeneric && cb->type <=  BPLib_MEM_BlocktypeSecondaryMax);
}

BPLib_MEM_Block_t *BPLib_MEM_BlockFromExternalId(BPLib_MEM_Pool_t *pool, bp_handle_t handle);

static inline bp_handle_t BPLib_MEM_GetExternalId(const BPLib_MEM_Block_t *cb)
{
    return bp_handle_from_serial(cb->parent_offset, BPLIB_HANDLE_MPOOL_BASE);
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
 * bplib_mpool_recycle_all_blocks_in_list()
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
 * @return bplib_mpool_t*
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
 * @retval BPLIB_SUCCESS if registration successful
 * @retval BP_DUPLICATE if the block type is already registered.
 */
int BPLib_MEM_RegisterBlocktype (BPLib_MEM_Pool_t *pool, uint32_t magic_number, const BPLib_MEM_BlocktypeApi_t *api,
                                   size_t user_content_size);

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_QueryMemCurrentUse
 *
 *-----------------------------------------------------------------*/
size_t BPLib_MEM_QueryMemCurrentUse(BPLib_MEM_Pool_t *pool);

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_QueryMemMaxUse
 *
 *-----------------------------------------------------------------*/
size_t BPLib_MEM_QueryMemMaxUse(BPLib_MEM_Pool_t *pool);

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_GetParentPoolFromLink
 *
 *-----------------------------------------------------------------*/
BPLib_MEM_Pool_t *BPLib_MEM_GetParentPoolFromLink(BPLib_MEM_Block_t *cb);

// TODO Move or remove the header for "Exported Functions"
/**
 *  Exported Functions
 */

/**
 * \brief Memory Allocator initialization
 *
 *  \par Description
 *       MEM initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
int BPLib_MEM_Init(void);

#endif /* BPLIB_MEM_H */
