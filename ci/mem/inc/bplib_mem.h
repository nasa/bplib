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

#include "bplib_api_types.h"

#include "bplib_time.h"

#include "bplib_rbt.h"

/*
 * Minimum size of a generic data block
 */
#define BPLIB_MEM_MIN_USER_BLOCK_SIZE 480 // bytes

#define BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE 0x6b243e33

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

typedef struct BPLib_MEM_Pool BPLib_MEM_Pool_t;

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

typedef struct BPLib_MEM_Lock
{
    BPLib_Handle_t lock_id;
} BPLib_MEM_Lock_t;

typedef struct BPLib_MEM_Global
{
    BPLib_MEM_Lock_t BPLib_MEM_PoolLock;
} BPLib_MEM_Global_T;

/**
 * The BPLib_MEM_Blocktype is a subset of the BPLib_STOR_CACHE_Blocktype.
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
     * cause a build-time error if they are used in MEM.
     */

    BPLib_MEM_BlocktypeUndefined = 0,
    BPLib_MEM_BlocktypeApi       = 1,
    BPLib_MEM_BlocktypeGeneric   = 2,
    BPLib_MEM_BlocktypePrimary   = 4,
    BPLib_MEM_BlocktypeCanonical = 5,
    BPLib_MEM_BlocktypeDuct      = 6,
    BPLib_MEM_BlocktypeRef       = 7,
    BPLib_MEM_BlocktypeMax       = 8, /* placeholder for the max "regular" block type */

    BPLib_MEM_BlocktypeSecondaryGeneric = 100,
    BPLib_MEM_BlocktypeListHead         = 101,  // For Available and Free lists (subqs)
    BPLib_MEM_BlocktypeJob              = 102,  // A job or pending work item to do
    BPLib_MEM_BlocktypeSecondaryMax     = 103,
    BPLib_MEM_BlocktypeAdmin            = 255
} BPLib_MEM_Blocktype_t;

typedef struct BPLib_MEM_Block BPLib_MEM_Block_t;

/**
 * @brief Create a condition variable for an underlying mutex.
 *
 * Creates a condition variable for use with BPLib_MEM_OS_Lock and
 * BPLib_MEM_OS_Unlock
 *
 * @return BPLib_Handle_t The handle of the condition variable.
 */
BPLib_Handle_t BPLib_MEM_OS_CreateLock(void);

/**
 * @brief Locks/Acquires the underlying mutex associated with a condition variable
 *
 * The mutex should always be locked by a task before reading or modifying the
 * data object associated with a condition variable.
 *
 * @param h The handle of the condition variable.
 */
void BPLib_MEM_OS_Lock(BPLib_Handle_t h);

/**
 * @brief Release the underlying mutex associated with a condition variable
 *
 * @param h The handle of the condition variable.
 */
void BPLib_MEM_OS_Unlock(BPLib_Handle_t h);

/**
 * @brief Acquires a given lock
 *
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
    if (lock != NULL)
    {
        BPLib_MEM_OS_Unlock(lock->lock_id);
    }
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
 * @param BPLIB_TIME_TO_INT(until_time)
 * @return true
 * @return false
 */
bool BPLib_MEM_LockWait(BPLib_MEM_Lock_t *lock, BPLib_TIME_MonotonicTime_t until_time);

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

/**
 * @brief Allocate a new user data block
 *
 * Note that the maximum block capacity is set at compile time.  This limits the size
 * of user data objects that can be stored by this mechanism.
 *
 * @param pool
 * @param magic_number
 * @param init_arg Opaque pointer passed to initializer (may be NULL)
 * @return BPLib_MEM_Block_t*
 */
BPLib_MEM_Block_t *BPLib_MEM_GenericDataAlloc(BPLib_MEM_Pool_t *pool, uint32_t magic_number, void *init_arg);

/**
 * @brief Cast a block to generic user data type
 *
 * User blocks require a match on on the "magic number", which may be a random 32-bit integer.
 * This is intended as a data integrity check to confirm the block is indeed the correct flavor.
 *
 * @param cb
 * @param required_magic
 * @return BPLib_MEM_GenericData_t*
 */
void *BPLib_MEM_GenericDataCast(BPLib_MEM_Block_t *cb, uint32_t required_magic);

/**
 * @brief Cast a generic user data segment to its parent block
 *
 * This is the inverse of BPLib_MEM_GenericDataCast() and allows obtaining the pool
 * block from a generic data pointer.  The pointer passed in must be from a prior call
 * to BPLib_MEM_GenericDataCast() or else the result is undefined.
 *
 * @param blk pointer from previous call to BPLib_MEM_GenericDataCast
 * @param required_magic
 * @return BPLib_MEM_GenericData_t*
 */
BPLib_MEM_Block_t *BPLib_MEM_GenericDataUncast(void *blk, BPLib_MEM_Blocktype_t parent_bt,
                                               uint32_t required_magic);

/**
 * @brief Get the parent memory block from a link to a memory block that may be a child
 *
 * Gets the pointer to a memory block that is the top-level parent block.
 * Traverses to the parent block via the parent offset, if the block is a not
 * the top-level parent.
 *
 * @param lblk pointer to BPLib_MEM_Block_t
 * @return pointer to BPLib_MEM_Block_t Top-level parent block
 */
BPLib_MEM_Block_t *BPLib_MEM_GetBlockFromLink(BPLib_MEM_Block_t *lblk);

/**
 * @brief Get the generic data capacity for a memory block
 *
 * Gets the size of the user content space for a block, based on the
 * size of BPLib_MEM_BlockBuffer_t less the data offset for the block's
 * blocktype.
 *
 * @param cb pointer to BPLib_MEM_Block_t
 * @return size_t
 */
size_t BPLib_MEM_GetGenericDataCapacity(const BPLib_MEM_Block_t *cb);

/**
 * @brief Perform garbage collection for a given pool of memory blocks
 *
 * @param pool pointer to BPLib_MEM_Pool_t
 */
void BPLib_MEM_Maintain(BPLib_MEM_Pool_t *pool);

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
BPLib_Status_t BPLib_MEM_Init(void);

#endif /* BPLIB_MEM_H */
