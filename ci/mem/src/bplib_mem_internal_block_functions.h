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

#ifndef BPLIB_MEM_INTERNAL_BLOCK_FUNCTIONS_H
#define BPLIB_MEM_INTERNAL_BLOCK_FUNCTIONS_H

// TODO MEM-local Time helpers, also in the cache code separately. Should be in bplib_api_types.h or TIME.
// TODO Reconcile heritage time functions with bplib_time.h TIME module.
uint64_t BPLib_MEM_OS_GetDtnTimeMs(void);

static inline uint64_t BPLib_MEM_GetDtnTime(void)
{
    BPLib_TIME_MonotonicTime_t monotime;
    BPLib_TIME_GetMonotonicTime(&monotime);
    return BPLib_TIME_GetDtnTime(monotime);
}

static inline BPLib_TIME_MonotonicTime_t BPLib_MEM_GetMonotonicTime(void)
{
    BPLib_TIME_MonotonicTime_t monotime;
    BPLib_TIME_GetMonotonicTime(&monotime);
    return monotime;
}

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

BPLib_MEM_Block_t *BPLib_MEM_BlockFromExternalId(BPLib_MEM_Pool_t *pool, BPLib_Handle_t handle);

static inline BPLib_Handle_t BPLib_MEM_GetExternalId(const BPLib_MEM_Block_t *cb)
{
    return BPLib_HandleFromSerial(cb->parent_offset, BPLIB_HANDLE_MPOOL_BASE);
}

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
 * @retval BPLIB_DUPLICATE if the block type is already registered.
 */
int BPLib_MEM_RegisterBlocktype (BPLib_MEM_Pool_t *pool, uint32_t magic_number, const BPLib_MEM_BlocktypeApi_t *api,
                                   size_t user_content_size);

/**
 * Initialize the MEM module's internal calls to OSAL.
 * Most of the internal MEM OS code came from the heritage bplib/os library.
 * It's only a subset of that library.
 */
BPLib_Status_t BPLib_MEM_OS_Init(void);

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_OS_NextSerial -
 *-------------------------------------------------------------------------------------*/
unsigned int BPLib_MEM_OS_NextSerial(void);

#define BPLIB_MEM_DTNTIME_INFINITE UINT64_MAX // TODO Should be in bplib.h as BPLIB_DTNTIME_INFINITE.

int BPLib_MEM_OS_WaitUntilMs(BPLib_Handle_t h, uint64_t abs_dtntime_ms);

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

#endif /* BPLIB_MEM_INTERNAL_BLOCK_FUNCTIONS_H */
