#include "benalloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char** argv)
{
    BPLib_MEM_PoolImpl_t pool;
    void* pool_mem;

    /* Create a memory pool */
    pool_mem = calloc(1, 256);
    BPLib_MEM_PoolImpl_Init(&pool, pool_mem, 256, 64);

    /* Fill up the pool and make sure it NULLs out */
    void* a = BPLib_MEM_PoolImpl_Alloc(&pool);
    assert(a != NULL);
    void* b = BPLib_MEM_PoolImpl_Alloc(&pool);
    assert(b != NULL);
    void* c = BPLib_MEM_PoolImpl_Alloc(&pool);
    assert(c != NULL);
    void* d = BPLib_MEM_PoolImpl_Alloc(&pool);
    assert(d != NULL);

    /* At this point we should be out of space. */
    assert(BPLib_MEM_PoolImpl_Alloc(&pool) == NULL);

    /***************************************************************************/
    /* Free and make sure it 'de-frags' */
    BPLib_MEM_PoolImpl_Destroy(&pool);
    BPLib_MEM_PoolImpl_Init(&pool, pool_mem, 256, 64);

    a = BPLib_MEM_PoolImpl_Alloc(&pool);
    assert(a != NULL);
    b = BPLib_MEM_PoolImpl_Alloc(&pool);
    assert(b != NULL);
    c = BPLib_MEM_PoolImpl_Alloc(&pool);
    assert(c != NULL);

    BPLib_MEM_PoolImpl_Free(&pool, a);

    /* We should be able to allocate 2 blocks, implying space is re-used */
    a = BPLib_MEM_PoolImpl_Alloc(&pool);
    assert(a != NULL);
    d  = BPLib_MEM_PoolImpl_Alloc(&pool);
    assert(d != NULL);
    assert(BPLib_MEM_PoolImpl_Alloc(&pool) == NULL);

    return 0;
}
