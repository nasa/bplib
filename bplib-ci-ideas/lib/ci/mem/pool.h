#ifndef __GSFC_BPLIB_MEM_H__
#define __GSFC_BPLIB_MEM_H__

#include <stdint.h>

#define BPLIB_MEM_CHUNKSIZE 512U

typedef struct BPLib_MEM_Block
{
    uint8_t chunk[BPLIB_MEM_CHUNKSIZE];
    size_t chunk_len;
    BPLib_MEM_Block_t* next;
} BPLib_MEM_Block_t;

typedef struct BPLib_MEM_PoolState_t;

typedef struct BPLib_MEM_Pool
{
    BPLib_MEM_PoolState_t state;
    void* mem; // Presently unused in impl.
    size_t mem_len;
} BPLib_MEM_Pool_t;

void BPLib_MEM_PoolInit(void* init_mem, size_t init_mem_len, BPLib_MEM_Pool_t* pool);

void BPLib_MEM_PoolDestroy(BPLib_MEM_Pool_t* pool);

size_t BPLib_MEM_GetBlockSize(BPLib_MEM_Pool_t* pool);

BPLib_MEM_Block_t* BPLib_MEM_BlockAlloc(BPLib_MEM_Pool_t* pool);

void BPLib_MEM_BlockFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* block);

BPLib_MEM_Block_t* BPLib_MEM_BlockListAlloc(BPLib_MEM_Pool_t* pool, size_t byte_len);

void BPLib_MEM_BlockListFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* block);

#endif /* __GSFC_BPLIB_MEM_H__ */
