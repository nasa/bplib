/*
** Implementation of Ben Kenwright's "Fast Efficient Fixed-Size Memory Pool"
** https://arxiv.org/pdf/2210.16471
** 
*/
#include "bplib_ben_allocator.h"

#include <stdio.h>
#include <string.h>

static const void* addr_from_index(BPLib_MEM_PoolImpl_t* pool, uint32_t ind)
{
    return (uint8_t*)pool->mem_start + (ind * pool->block_size);
}

static uint32_t index_from_addr(BPLib_MEM_PoolImpl_t* pool, const void* p)
{
    return  (((uint32_t)((uint8_t*)p - (uint8_t*)pool->mem_start)) / pool->block_size);
}

bool BPLib_MEM_PoolImplInit(BPLib_MEM_PoolImpl_t* pool, const void* init_mem,
    size_t mem_len, uint32_t block_size)
{
    if (pool == NULL)
    {
        return false;
    }
    if (init_mem == NULL)
    {
        return false;
    }
    if (block_size < sizeof(uint32_t))
    {
        return false;
    }
    if (mem_len == 0)
    {
        return false;
    }

    memset(pool, 0, sizeof(BPLib_MEM_PoolImpl_t));
    pool->mem_start = (uint8_t*)(init_mem);
    pool->block_size = block_size;
    pool->num_blocks = mem_len / block_size;
    pool->mem_next = pool->mem_start;
    pool->num_free = pool->num_blocks;

    return true;
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
    uint32_t* p;
    void* ret;

    if (pool == NULL)
    {
        return NULL;
    }

    if (pool->num_init < pool->num_blocks)
    {
        p = (uint32_t *)(addr_from_index(pool, pool->num_init));
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
            pool->mem_next = (void*) addr_from_index(pool, *((uint32_t*)pool->mem_next));
        }
        else
        {
            pool->mem_next = NULL;
        }
    }

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
        (*(uint32_t*)to_free) = index_from_addr(pool, pool->mem_next);
        pool->mem_next = (void*)(to_free);
    }
    else
    {
        (*(uint32_t*)to_free) = pool->num_blocks;
        pool->mem_next = (void*)(to_free);
    }
    pool->num_free++;
}
