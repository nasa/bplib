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

#ifndef BPLIB_MEM_BEN_ALLOCATOR_H
#define BPLIB_MEM_BEN_ALLOCATOR_H

#include "bplib_api_types.h"

/**
 * @struct BPLib_MEM_PoolImpl_t
 * @brief Represents the internal structure of a memory pool implementation.
 * 
 * This structure is used to manage memory allocation and deallocation within the pool.
 */
typedef struct BPLib_MEM_PoolImpl
{
    void* mem_start; /**< Pointer to the start of the memory region backing the pool */
    void* mem_next; /**< Pointer to the next available memory block location within the pool */
    uint32_t num_blocks; /**< The maximum number of blocks that can be allocated by this pool. */
    uint32_t block_size; /**< Size of each block in the pool, in bytes. */
    uint32_t num_free; /**< Number of free blocks available */
    uint32_t num_init; /**< Number of initialized blocks. */
} BPLib_MEM_PoolImpl_t;

/**
 * @brief Initializes the memory pool implementation.
 * 
 * This function initializes the memory pool, setting up the memory and block size.
 * 
 * @param[out] pool Pointer to the memory pool implementation to initialize.
 * @param[in] init_mem Pointer to the initial memory to use for the pool.
 * @param[in] mem_len The length of the memory to use.
 * @param[in] block_size The size of each block in the pool.
 * 
 * @return Status of the operation.
 */
BPLib_Status_t BPLib_MEM_PoolImplInit(BPLib_MEM_PoolImpl_t* pool, const void* init_mem,
    size_t mem_len, uint32_t block_size);

/**
 * @brief Destroys the memory pool implementation.
 * 
 * This function frees any resources associated with the memory pool implementation.
 * 
 * @param[in] pool Pointer to the memory pool implementation to destroy.
 */
void BPLib_MEM_PoolImplDestroy(BPLib_MEM_PoolImpl_t* pool);

/**
 * @brief Allocates a block of memory from the pool implementation.
 * 
 * This function allocates a single block of memory from the pool.
 * 
 * @param[in] pool Pointer to the memory pool implementation from which to allocate memory.
 * 
 * @return Pointer to the allocated memory block, or NULL if allocation fails.
 */
void* BPLib_MEM_PoolImplAlloc(BPLib_MEM_PoolImpl_t* pool);

/**
 * @brief Frees a block of memory back to the pool implementation.
 * 
 * This function frees a previously allocated memory block and returns it to the pool.
 * 
 * @param[in] pool Pointer to the memory pool implementation to return the block to.
 * @param[in] to_free Pointer to the memory block to free.
 */
void BPLib_MEM_PoolImplFree(BPLib_MEM_PoolImpl_t* pool, void* to_free);

#endif /* BPLIB_MEM_BEN_ALLOCATOR_H */
