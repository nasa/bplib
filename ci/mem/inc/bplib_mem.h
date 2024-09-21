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

#define BPLIB_HANDLE_MPOOL_BASE \
    (bp_handle_t)               \
    {                           \
        0x6000000               \
    }
#endif // BELONGS_IN_BPLIB_API_TYPES

// TODO Return BPlib_MEM_Block_t to an abstract type.
typedef struct BPLib_MEM_Block
{
    /* note that if it becomes necessary to recover bits here,
     * both the type and offset could be reduced in size */
    uint32_t                  parent_offset;
    struct BPLib_MEM_Block_t  *next;
    struct BPLib_MEM_Block_t  *prev;
} BPLib_MEM_Block_t;

//struct BPLib_MEM_Block_abstract // TODO
//{
    /* note that if it becomes necessary to recover bits here,
     * the offset could be reduced in size */
//    uint32_t                  parent_offset;
//    struct BPLib_MEM_Block_t *next;
//    struct BPLib_MEM_Block_t *prev;
//};

/**
 * BPLib STOR CACHE defines many blocktypes.
 * MEM only recognizes a few blocktypes, as listed here.
 * The administrative block will be marked with 0xFF, this still permits the
 * type to be stored as a uint8_t if needed to save bits.
 */

#define BPLIB_MEM_BLOCKTYPE_ADMIN 255

typedef enum BPLib_MEM_Blocktype
{
    BPLib_MEM_BlocktypeUndefined = 0,
    BPLib_MEM_BlocktypeGeneric   = 2,
    BPLib_MEM_BlocktypeMax       = 8,  // From BPLib_STOR_CACHE_BlocktypeMax
    BPLib_MEM_BlocktypeAdmin     = 255
} BPLib_MEM_Blocktype_t;

/**
 * @brief Callback frunction for various mpool block actions
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
    BPLib_MEM_Block_t *block_list;

    /* note - "unsigned int" is chosen here as it is likely to be
     * a single-cycle read in most CPUs.  The range is not as critical
     * because what matters is the difference between these values.
     * The "volatile" qualification helps ensure the values are read as they
     * appear in code and are not rearranged by the compiler, as they could
     * be changed by other threads.  */
    volatile unsigned int push_count;
    volatile unsigned int pull_count;
} BPLib_MEM_SubqBase_t;

typedef struct BPLib_MEM_BlockAdminContent
{
    size_t   buffer_size;
    uint32_t num_bufs_total;
    uint32_t bblock_alloc_threshold;   /**< threshold at which new bundles will no longer be allocatable */
    uint32_t internal_alloc_threshold; /**< threshold at which internal blocks will no longer be allocatable */
    uint32_t max_alloc_watermark;

    BPLib_MEM_SubqBase_t free_blocks;    /**< blocks which are available for use */
    BPLib_MEM_SubqBase_t recycle_blocks; /**< blocks which can be garbage-collected */

    /* note that the active_list and managed_block_list are not FIFO in nature, as blocks
     * can be removed from the middle of the list or otherwise rearranged. Therefore a subq
     * is not used for these, because the push_count and pull_count would not remain accurate. */

    BPLib_MEM_Block_t active_list; /**< a list of flows/queues that need processing */

} BPLib_MEM_BlockAdminContent_t;

/*
 * Minimum size of a generic data block
 */
#define BPLIB_MEM_MIN_USER_BLOCK_SIZE 480 // bytes

#define BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE 0x6b243e33


typedef union BPLib_MEM_BlockBuffer
{
    BPLib_MEM_GenericDataContent_t     generic_data;
    BPLib_MEM_ApiContent_t              api;
    BPLib_MEM_BlockAdminContent_t      admin;

    /* guarantees a minimum size of the generic data blocks, also determines the amount
     * of extra space available for user objects in other types of blocks. */
    uint8_t content_bytes[BPLIB_MEM_MIN_USER_BLOCK_SIZE];
} BPLib_MEM_BlockBuffer_t;

typedef struct BPLib_MEM_Block_Content
{
    BPLib_MEM_BlockHeader_t header; /* must be first */
    BPLib_MEM_BlockBuffer_t u;
} BPLib_MEM_BlockContent_t;

typedef struct BPLib_MEM_Pool
{
    BPLib_MEM_BlockContent_t admin_block; /**< Start of first real block (see num_bufs_total) */
} BPLib_MEM_Pool_t;

#define BPLIB_MEM_GET_BUFFER_USER_START_OFFSET(m) (offsetof(bplib_mpool_block_buffer_t, m.user_data_start))

#define BPLIB_MEM_GET_BLOCK_USER_CAPACITY(m) (sizeof(bplib_mpool_block_buffer_t) - MPOOL_GET_BUFFER_USER_START_OFFSET(m))

BPLib_MEM_Block_t *BPLib_MEM_BlockFromExternalId(BPLib_MEM_Pool_t *pool, bp_handle_t handle);

static inline bp_handle_t BPLib_MEM_GetExternalId(const BPLib_MEM_Block_t *cb)
{
    return bp_handle_from_serial(cb->parent_offset, BPLIB_HANDLE_MPOOL_BASE);
}

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
