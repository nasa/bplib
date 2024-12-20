#include "pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>


/*******************************************************************************
* Static Functions
*/
static void BPLib_MEM_PoolLock()
{
    // Either CV or Mutex
    return;
}

static void BPLib_MEM_PoolUnlock()
{
    // Either CV or Mutex
    return;
}

/*******************************************************************************
* Exported Functions
*/
bool BPLib_MEM_PoolInit(BPLib_MEM_Pool_t* pool, void* init_mem, size_t init_size)
{
    if (pool == NULL)
    {
        return false;
    }

    memset(pool, 0, sizeof(BPLib_MEM_Pool_t));
    return BPLib_MEM_PoolImplInit(&pool->impl, init_mem, init_size,
        sizeof(BPLib_MEM_Block_t));

}

void BPLib_MEM_PoolDestroy(BPLib_MEM_Pool_t* pool)
{
    if (pool == NULL)
    {
        return;
    }

    // MEM LOCK
    memset(pool, 0, sizeof(BPLib_MEM_Pool_t));
    BPLib_MEM_PoolImplDestroy(&pool->impl);
    // MEM UNLOCK
}

BPLib_MEM_Block_t* BPLib_MEM_BlockAlloc(BPLib_MEM_Pool_t* pool)
{
    size_t alloc_len;
    void* block_mem;
    BPLib_MEM_Block_t* block;

    if (pool == NULL)
    {
        return NULL;
    }

    // MEM LOCK
    block = (BPLib_MEM_Block_t*)(BPLib_MEM_PoolImplAlloc(&pool->impl));
    // MEM UNLOCK
    block->chunk_len = BPLIB_MEM_CHUNKSIZE;
    block->next = NULL;

    return block;
}

void BPLib_MEM_BlockFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* block)
{
    if (pool == NULL || block == NULL)
    {
        return;
    }

    // MEM LOCK
    BPLib_MEM_PoolImplFree(&pool->impl, (void*)block->chunk);
    // MEM UNLOCK
}

BPLib_MEM_Block_t* BPLib_MEM_BlockListAlloc(BPLib_MEM_Pool_t* pool, size_t byte_len)
{
    size_t bytes_alloc;
    BPLib_MEM_Block_t* head;
    BPLib_MEM_Block_t* curr_tail;
    BPLib_MEM_Block_t* new_block;

    if (pool == NULL || byte_len == 0)
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
        bytes_alloc += new_block->chunk_len;

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
    } while (bytes_alloc < byte_len);

    return head;
}

void BPLib_MEM_BlockListFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* head)
{
    BPLib_MEM_Block_t* to_free;
    BPLib_MEM_Block_t* curr;

    if (pool == NULL || head == NULL)
    {
        return;
    }

    curr = head;
    while (curr != NULL)
    {
        to_free = curr;
        curr = curr->next;
        BPLib_MEM_BlockFree(pool, to_free);
    }
}
