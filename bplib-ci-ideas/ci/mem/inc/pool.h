#ifndef __GSFC_BPLIB_MEM_H__
#define __GSFC_BPLIB_MEM_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define BPLIB_MEM_CHUNKSIZE 512U

typedef struct BPLib_MEM_Block
{
    struct BPLib_MEM_Block* next;
    uint8_t chunk[BPLIB_MEM_CHUNKSIZE];
    size_t chunk_len;
} BPLib_MEM_Block_t;

typedef struct BPLib_MEM_PoolState
{
    size_t alloc_size;
    size_t bytes_used;
} BPLib_MEM_PoolState_t;

typedef struct BPLib_MEM_Pool
{
    void* mem; // Presently unused in impl.
    size_t size;
    BPLib_MEM_PoolState_t state;
} BPLib_MEM_Pool_t;

void BPLib_MEM_PoolInit(BPLib_MEM_Pool_t* pool, void* init_mem, size_t init_size);

void BPLib_MEM_PoolDestroy(BPLib_MEM_Pool_t* pool);

size_t BPLib_MEM_GetBlockSize(BPLib_MEM_Pool_t* pool);

BPLib_MEM_Block_t* BPLib_MEM_BlockAlloc(BPLib_MEM_Pool_t* pool);

void BPLib_MEM_BlockFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* block);

BPLib_MEM_Block_t* BPLib_MEM_BlockListAlloc(BPLib_MEM_Pool_t* pool, size_t byte_len);

void BPLib_MEM_BlockListFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* head);

#endif /* __GSFC_BPLIB_MEM_H__ */
