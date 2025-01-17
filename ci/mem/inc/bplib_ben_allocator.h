#ifndef __GSFC_BPLIB_MEM_BENALLOC_H__
#define __GSFC_BPLIB_MEM_BENALLOC_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct BPLib_MEM_PoolImpl
{
    void* mem_start;
    void* mem_next;
    uint32_t num_blocks;
    uint32_t block_size;
    uint32_t num_free;
    uint32_t num_init;
} BPLib_MEM_PoolImpl_t;

bool BPLib_MEM_PoolImplInit(BPLib_MEM_PoolImpl_t* pool, const void* init_mem,
    size_t mem_len, uint32_t block_size);

void BPLib_MEM_PoolImplDestroy(BPLib_MEM_PoolImpl_t* pool);

void* BPLib_MEM_PoolImplAlloc(BPLib_MEM_PoolImpl_t* pool);

void BPLib_MEM_PoolImplFree(BPLib_MEM_PoolImpl_t* pool, void* to_free);

#endif /* __GSFC_BPLIB_MEM_BENALLOC_H__ */
