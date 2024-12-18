#include "mem.h"

#include <stdib.h>

typedef struct BPLib_MEM_PoolState
{
    size_t alloc_size;
    size_t bytes_used;
} BPLib_MEM_PoolState_t;

static bool BPLib_MEM_PoolIsValid(BPLib_MEM_Pool_t* pool)
{
    if (pool == NULL)
    {
        return false;
    }
    if (pool->mem == NULL)
    {
        return false;
    }
    if (pool->len == 0)
    {
        return false;
    }

    return true;
}

void BPLib_MEM_PoolInit(void* init_mem, size_t init_mem_len, BPLib_MEM_Pool_t* pool)
{
    if (pool == NULL)
    {
        return;
    }

    memset(pool, 0, sizeof(BPLib_MEM_Pool_t));
    pool->mem = init_mem;
    pool->len = init_mem_len;
    pool->state.bytes_used = 0;
    pool->state.alloc_size = sizeof(BPLib_MEM_Block_t);
}

void BPLib_MEM_PoolDestroy(BPLib_MEM_Pool_t* pool)
{
    if (pool == NULL)
    {
        return NULL;
    }

    memset(pool, 0, sizeof(BPLib_MEM_Pool_t));
    pool->mem = NULL;
    pool->mem_len = 0;
}

size_t BPLib_MEM_GetBlockSize(BPLib_MEM_Pool_t* pool)
{
    if (!BPLib_MEM_PoolIsValid(pool))
    {
        return 0;
    }

    return (size_t)(BPLIB_MEM_CHUNKSIZE);
}

BPLib_MEM_Block_t* BPLib_MEM_BlockAlloc(BPLib_MEM_Pool_t* pool)
{
    size_t alloc_len;
    void* block_mem;
    BPLib_MEM_Block_t* new_block;

    if (!BPLib_MEM_PoolIsValid(pool))
    {
        return 0;
    }

    /* Ensure there's enough space for a full new block */
    if ((pool->len - pool->bytes_used) < pool->state.alloc_size)
    {
        return NULL;
    }

    /* Implementation-specifc memory allocation goes here */
    block_mem = calloc(1, pool->state.alloc_size);
    if (block_mem == NULL)
    {
        return NULL;
    }
    new_block = (BPLib_MEM_Block_t*)(block_mem);
    new_block->chunk = block_mem;
    new_block->chunk_len = (size_t)(BPLIB_MEM_CHUNKSIZE);
    new_block->next = NULL;

    /* Update pool state to reflect new allocation */
    pool->bytes_used += pool->state.alloc_size;
}

void BPLib_MEM_BlockFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* block)
{
    if (!BPLib_MEM_PoolIsValid(pool))
    {
        return;
    }

    /* Implementation-specific memory deallocation goes here */
    free(block);
    pool->bytes_used -= pool->state.alloc_size;
}

BPLib_MEM_Block_t* BPLib_MEM_BlockListAlloc(BPLib_MEM_Pool_t* pool, size_t byte_len)
{
    size_t bytes_alloc;
    BPLib_MEM_Block_t* head;
    BPLib_MEM_Block_t* curr_tail;
    BPLib_MEM_Block_t* new_block;

    if (!BPLib_MEM_PoolIsValid(pool))
    {
        return NULL;
    }
    if (byte_len == 0)
    {
        return NULL;
    }

    head = NULL;
    bytes_alloc = 0;
    do
    {
        new_block = BPLib_MEM_BlockAlloc(pool);
        if (new_block == NULL)
        {
            BPLib_MEM_BlockListFree(pool, head);
            return NULL;
        }
        bytes_alloc += pool->state.alloc_size;

        if (head == NULL)
        {
            head = new_block;
            curr_tail = head;
        }
        else
        {
            curr_tail->next = new_block;
            curr_tail = new_block;
        }
    } (while (bytes_alloc < byte_len));

    return head;
}

void BPLib_MEM_BlockListFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* block)
{
    BPLib_MEM_Block_t* to_free;
    BPLib_MEM_Block_t* curr;

    if (!BPLib_MEM_PoolIsValid(pool))
    {
        return NULL;
    }

    curr = block;
    while (curr != NULL)
    {
        to_free = curr;
        curr = curr->next;
        BPLib_MEM_BlockFree(pool, to_free);
    }
}
