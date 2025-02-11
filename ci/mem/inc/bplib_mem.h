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

#include "bplib_ben_allocator.h"
#include "bplib_bblocks.h"

#include <pthread.h>

/**
 ** \brief Defines the size of a memory block user data if used as a bytearray
*/
#define BPLIB_MEM_CHUNKSIZE (512U)

typedef struct BPLib_MEM_Block BPLib_MEM_Block_t;

/**
 * @struct BPLib_Bundle_t
 * @brief Represents the entire bundle, including its blocks and an additional blob for other data.
 */
typedef struct BPLib_Bundle
{
    BPLib_BBlocks_t blocks;
    struct BPLib_MEM_Block* blob;
} BPLib_Bundle_t;

/**
 * @brief This union represents data types anticipated for allocation by this memory pool.
 * @union BPLib_MEM_UserData_t
 */
typedef union BPLib_MEM_UserData
{
    uint8_t raw_bytes[BPLIB_MEM_CHUNKSIZE]; /**< A raw byte array, useful for storing arbitrary binary data */
    struct BPLib_Bundle bundle; /**< A bundle type, intended to store CBOR-decoded Bundle metadata */
} BPLib_MEM_UserData_t;

/**
 * @struct BPLib_MEM_Block_t
 * @brief Represents a block of memory in the pool.
 * 
 * This structure holds a chunk of usable memory (`user_data`). 
 * It also has a pointer (`next`) to link to other blocks in a linked list.
 */
struct BPLib_MEM_Block
{
    BPLib_MEM_UserData_t user_data; /**< User data stored in the block */
    size_t used_len; /**< Byte-length of user data currently within the chunk. This is initialized to 0. */
    struct BPLib_MEM_Block* next; /**< Pointer to the next block in the list */
};

/**
 * @struct BPLib_MEM_Pool_t
 * @brief Represents a memory pool that manages memory blocks.
 * 
 * This structure holds the implementation of the pool (`impl`), and a mutex lock (`lock`)
 * for thread safety when accessing the memory pool.
 */
typedef struct BPLib_MEM_Pool
{
    BPLib_MEM_PoolImpl_t impl; /**< The pool implementation (details hidden) */
    pthread_mutex_t lock; /**< Mutex for synchronizing access to the pool */
} BPLib_MEM_Pool_t;

/**
 * @brief Initializes a memory pool.
 * 
 * This function initializes a memory pool with the provided initial memory and size.
 * 
 * @param[out] pool Pointer to the memory pool to initialize.
 * @param[in] init_mem Pointer to the initial memory for the pool.
 * @param[in] init_size Size of the initial memory.
 * 
 * @return Status of the operation.
 */
BPLib_Status_t BPLib_MEM_PoolInit(BPLib_MEM_Pool_t* pool, void* init_mem, size_t init_size);

/**
 * @brief Destroys a memory pool.
 * 
 * This function destroys the memory pool and frees any allocated resources.
 * 
 * @param[in] pool Pointer to the memory pool to destroy.
 */
void BPLib_MEM_PoolDestroy(BPLib_MEM_Pool_t* pool);

/**
 * @brief Allocates a new memory block from the pool.
 * 
 * This function allocates a single memory block from the memory pool.
 * 
 * @param[in] pool Pointer to the memory pool from which to allocate a block.
 * 
 * @return Pointer to the allocated memory block.
 */
BPLib_MEM_Block_t* BPLib_MEM_BlockAlloc(BPLib_MEM_Pool_t* pool);

/**
 * @brief Frees a memory block back to the pool.
 * 
 * This function frees a memory block that was previously allocated from the pool.
 * 
 * @param[in] pool Pointer to the memory pool to which to return the block.
 * @param[in] block Pointer to the memory block to free.
 */
void BPLib_MEM_BlockFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* block);

/**
 * @brief Allocates a list of memory blocks from the pool.
 * 
 * This function allocates a list of memory blocks that together have the requested byte length.
 * 
 * @param[in] pool Pointer to the memory pool from which to allocate blocks.
 * @param[in] byte_len The total byte length of the blocks to allocate.
 * 
 * @return Pointer to the head of the allocated block list.
 */
BPLib_MEM_Block_t* BPLib_MEM_BlockListAlloc(BPLib_MEM_Pool_t* pool, size_t byte_len);

/**
 * @brief Frees a list of memory blocks back to the pool.
 * 
 * This function frees a list of memory blocks that were previously allocated from the pool.
 * 
 * @param[in] pool Pointer to the memory pool to which to return the blocks.
 * @param[in] head Pointer to the head of the block list to free.
 */
void BPLib_MEM_BlockListFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* head);

/**
 * @brief Allocates a bundle from the memory pool.
 * 
 * This function allocates a new `BPLib_Bundle_t` from the memory pool, 
 * including a blob of data specified by the `blob_data` and `data_len` parameters.
 * 
 * @param[in] pool Pointer to the memory pool from which to allocate the bundle.
 * @param[in] blob_data A pointer to the data to store in the bundle.
 * @param[in] data_len The length of the data to store in the bundle.
 * 
 * @return A pointer to the allocated bundle.
 */
BPLib_Bundle_t* BPLib_MEM_BundleAlloc(BPLib_MEM_Pool_t* pool, const void* blob_data, size_t data_len);

/**
 * @brief Frees a bundle from the memory pool.
 * 
 * This function frees the memory associated with a `BPLib_Bundle_t`.
 * The bundle pointer provided have been allocated by using BPLib_MEM_BundleAlloc.
 * 
 * @param[in] pool Pointer to the memory pool to which the bundle will be freed.
 * @param[in] bundle Pointer to the bundle to free.
 */
void BPLib_MEM_BundleFree(BPLib_MEM_Pool_t* pool, BPLib_Bundle_t* bundle);

/**
 * @brief Copies the blob data out of a bundle.
 * 
 * This function copies the blob data from the specified bundle into the provided buffer.
 * If the bundle has more blob data than max_len, this function returns BPLIB_BUF_LEN_ERROR
 * 
 * @param[in] bundle Pointer to the bundle from which to copy the data.
 * @param[out] out_buffer A buffer to store the copied data.
 * @param[in] max_len The maximum number of bytes to copy.
 * @param[out] out_size The actual number of bytes copied.
 * 
 * @return Status of the operation.
 */
BPLib_Status_t BPLib_MEM_BlobCopyOut(BPLib_Bundle_t* bundle, void* out_buffer, size_t max_len, size_t* out_size);

#endif /* BPLIB_MEM_H */
