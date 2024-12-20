#include "pool.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    BPLib_MEM_Pool_t pool;
    BPLib_MEM_Block_t* block_head;
    BPLib_MEM_Block_t* curr;
    void* pool_mem;

    /* Create a memory pool */
    pool_mem = calloc(1, 16384);
    BPLib_MEM_PoolInit(&pool, pool_mem, 16384);

    /* Create a list of blocks */
    block_head = BPLib_MEM_BlockListAlloc(&pool, 2048);
    curr = block_head;
    while (curr)
    {
        curr = curr->next;
    }

    /* Destroy the block list */
    BPLib_MEM_BlockListFree(&pool, block_head);

    /* Cleanup */
    BPLib_MEM_PoolDestroy(&pool);
    free(pool_mem);
    return 0;
}
