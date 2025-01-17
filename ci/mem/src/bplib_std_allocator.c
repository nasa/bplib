#include "bplib_std_allocator.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

bool BPLib_MEM_PoolImplInit(BPLib_MEM_PoolImpl_t* pool, const void* init_mem,
    size_t mem_len, uint32_t block_size)
{
    if (pool == NULL)
    {
        return false;
    }
    if (init_mem != NULL)
    {
        printf("WARNING: STDALLOC doesn't actually use the init_mem param: It just calls malloc()\n");
    }

    memset(pool, 0, sizeof(BPLib_MEM_PoolImpl_t));
    pool->mem = (void*)init_mem;
    pool->size = 0;
    pool->block_size = block_size;
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
    return malloc(pool->block_size);
}

void BPLib_MEM_PoolImplFree(BPLib_MEM_PoolImpl_t* pool, void* to_free)
{
    free(to_free);
}
