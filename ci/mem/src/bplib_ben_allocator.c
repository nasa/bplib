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
 * C implementation of Ben Kenwright's "Fast Efficient Fixed-Size Memory Pool"
 * https://arxiv.org/pdf/2210.16471
 *
 */
#include "bplib_ben_allocator.h"

#include <stdio.h>
#include <string.h>

/*******************************************************************************
 * Defines and Types
 */

/* It is expected that the memory returned from this allocator will be cast
** to structs with valid allignment. For these casts to work, the memory
** that backs this pool needs to start on a boundary of the largest primitive
** type. It is assumed that this allignment will be sizeof(uint64_t) because
** many RFC 9171 fields require this type to be supported natively.
*/
#define LARGEST_ALLIGNMENT  (8u)

typedef uint64_t MemIndex_t;

/*******************************************************************************
 * Static Functions
 */
static const void* AddrFromIndex(BPLib_MEM_PoolImpl_t* pool, MemIndex_t ind)
{
    return (void *)((uintptr_t)pool->mem_start + (ind * pool->block_size));
}

static MemIndex_t IndexFromAddr(BPLib_MEM_PoolImpl_t* pool, const void* p)
{
    return (MemIndex_t)(((uintptr_t)p - (uintptr_t)pool->mem_start) / pool->block_size);
}

/*******************************************************************************
 * Exported Functions
 */
BPLib_Status_t BPLib_MEM_PoolImplInit(BPLib_MEM_PoolImpl_t* pool, void* init_mem,
    size_t mem_len, size_t block_size)
{
    size_t unaligned_bytes;
    void* alligned_init_mem;

    /* NULL Checks */
    if (pool == NULL)
    {
        return BPLIB_ERROR;
    }
    if (init_mem == NULL)
    {
        return BPLIB_ERROR;
    }
    if (mem_len == 0)
    {
        return BPLIB_ERROR;
    }

    /* Size safety checks */
    if ((block_size < sizeof(MemIndex_t)) || ((block_size % LARGEST_ALLIGNMENT) != 0))
    {
        /* Minimum allocation size must be at least the size of the MemIndex_t
        ** that is used to maintain the index of the next free block. It also must
        ** be a an exact multiple of the system's strictest/largest allignment.
        */
        return BPLIB_ERROR;
    }

    /* Before allowing this initialization to succeed, we need to be sure init_mem
    ** begins on an alignment boundary. If the user called malloc() to obtain init_mem,
    ** this should be guaranteed. Because this allocator doesn't know where init_mem
    ** comes from, we need to forcibly ensure that we allocate the first block on the
    ** strictest possible allignment boundary. Each subsequent block will also be on
    ** that alignment boundary because we've ensured block_size is a multiple of the alignment.
    */
    alligned_init_mem = (void*)(((uintptr_t)(init_mem) + ((uintptr_t)LARGEST_ALLIGNMENT - 1))
        & ~((uintptr_t)LARGEST_ALLIGNMENT - 1));
    unaligned_bytes = (size_t)((uintptr_t)alligned_init_mem - (uintptr_t)init_mem);

    /* Now that the start of useable memory is known, we need to make sure we
    ** have the space to allocate at least one block.
    */
    if (mem_len < (unaligned_bytes + block_size))
    {
        return BPLIB_ERROR;
    }
    mem_len -= unaligned_bytes;

    memset(pool, 0, sizeof(BPLib_MEM_PoolImpl_t));
    pool->mem_start = alligned_init_mem;
    pool->block_size = block_size;
    pool->num_blocks = mem_len / block_size;
    pool->mem_next = pool->mem_start;
    pool->num_free = pool->num_blocks;
    pool->num_init = 0;
    printf("MEM: %lu blocks at init\n", pool->num_blocks);

    return BPLIB_SUCCESS;
}

void BPLib_MEM_PoolImplDestroy(BPLib_MEM_PoolImpl_t* pool)
{
    if (pool == NULL)
    {
        return;
    }
    memset(pool, 0, sizeof(BPLib_MEM_PoolImpl_t));
}

void* BPLib_MEM_PoolImplAlloc(BPLib_MEM_PoolImpl_t* pool)
{
    MemIndex_t* p;
    void* ret;

    if (pool == NULL)
    {
        return NULL;
    }

    if (pool->num_init < pool->num_blocks)
    {
        p = (MemIndex_t *)(AddrFromIndex(pool, pool->num_init));
        *p = pool->num_init + 1;
        pool->num_init++; 
    }

    ret = NULL;
    if (pool->num_free > 0)
    {
        ret = (void *)pool->mem_next;
        pool->num_free--;
        if (pool->num_free != 0)
        {
            pool->mem_next = (void*) AddrFromIndex(pool, *((MemIndex_t*)pool->mem_next));
        }
        else
        {
            pool->mem_next = NULL;
        }
    }

    printf("MEMAlloc: Blocks Free %lu\n", pool->num_free);
    return ret;
}

void BPLib_MEM_PoolImplFree(BPLib_MEM_PoolImpl_t* pool, void* to_free)
{
    if (pool == NULL)
    {
        return;
    }

    if (pool->mem_next != NULL)
    {
        (*(MemIndex_t*)to_free) = IndexFromAddr(pool, pool->mem_next);
        pool->mem_next = (void*)(to_free);
    }
    else
    {
        (*(MemIndex_t*)to_free) = pool->num_blocks;
        pool->mem_next = (void*)(to_free);
    }
    pool->num_free++;
    printf("MEMFree: Blocks Free %lu\n", pool->num_free);
}
