#ifndef __GSFC_BPLIB_MEM_POOL_H__
#define __GSFC_BPLIB_MEM_POOL_H__

#include "benalloc.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include <pthread.h>

#define BPLIB_MEM_CHUNKSIZE 512U

typedef struct BPLib_MEM_Block
{
    uint8_t chunk[BPLIB_MEM_CHUNKSIZE];
    size_t chunk_len;
    struct BPLib_MEM_Block* next;
} BPLib_MEM_Block_t;

// TODO: Make implementation private if desired
typedef struct BPLib_MEM_Pool
{
    BPLib_MEM_PoolImpl_t impl;
    pthread_mutex_t lock;
} BPLib_MEM_Pool_t;

bool BPLib_MEM_PoolInit(BPLib_MEM_Pool_t* pool, void* init_mem, size_t init_size);

void BPLib_MEM_PoolDestroy(BPLib_MEM_Pool_t* pool);

BPLib_MEM_Block_t* BPLib_MEM_BlockAlloc(BPLib_MEM_Pool_t* pool);

void BPLib_MEM_BlockFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* block);

// Consider returning size_t with number of blocks and returning first block through parameter
BPLib_MEM_Block_t* BPLib_MEM_BlockListAlloc(BPLib_MEM_Pool_t* pool, size_t byte_len);

void BPLib_MEM_BlockListFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* head);

#endif /* __GSFC_BPLIB_MEM_POOL_H__ */
