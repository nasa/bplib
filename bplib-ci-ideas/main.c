#include "bundle.h"
#include "pool.h"
#include "qm.h"

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>

#define NUM_WORKERS       10U
#define MAX_JOBS          256U
#define QUEUE_TIMEOUT_MS  100
#define POOL_SIZE         16384U

volatile sig_atomic_t run = 1;

void handle_sigint(int sig)
{
    if ((sig == SIGINT))
    {
        run = 0;
    }
}

void* generic_worker(void* arg)
{
    BPLib_QM_QueueTable_t* tbl;

    tbl = (BPLib_QM_QueueTable_t*)(arg);
    while (run)
    {
        BPLib_QM_RunJob(tbl, QUEUE_TIMEOUT_MS);
    }

    return NULL;
}

int main(int argc, char** argv)
{
    pthread_t generic_workers[NUM_WORKERS];
    BPLib_QM_QueueTable_t tbl;

    BPLib_MEM_Pool_t pool;
    BPLib_MEM_Block_t* block_list;
    void* pool_mem;
    BPLib_Bundle_t* bundle;

    int i;

    signal(SIGINT, handle_sigint);

    /* Init memory allocator - Maybe this belongs in QM or somewhere else... 
    ** In this demo, nothing else creates bundles so I put it here.
    */
    pool_mem = calloc(POOL_SIZE, 1);
    if (!BPLib_MEM_PoolInit(&pool, pool_mem, POOL_SIZE))
    {
        fprintf(stderr, "BPLib_MEM_PoolInit()\n");
        return -1;
    }

    /* Setup the queue table. */
    if (!BPLib_QM_QueueTableInit(&tbl, MAX_JOBS))
    {
        fprintf(stderr, "BPLib_QM_QueueTableInit()\n");
        return -1;
    }

    /* Create Generic Workers */
    for (i = 0; i < NUM_WORKERS; i++)
    {
        pthread_create(&generic_workers[i], NULL, generic_worker, (void*)(&tbl));
    }

    /* Make one faux-bundle per worker */
    for (i = 0; i < NUM_WORKERS; i++)
    {
        /* Allocate a "bundle" with a unique node number to simulate CLA traffic */
        bundle = (BPLib_Bundle_t*)(BPLib_MEM_BlockAlloc(&pool));
        bundle->blocks.pri_blk.src_eid.node_number = i + 1;
        BPLib_QM_PostEvent(&tbl, bundle, STATE_CLA_TO_BI, QM_PRI_NORMAL, QM_WAIT_FOREVER);
    }

    /* Run the event loop until someone presses CTRL-C */
    BPLib_QM_EventLoop(&tbl, (bool*)(&run));

    /* Join threads */
    for (i = 0; i < NUM_WORKERS; i++)
    {
        pthread_join(generic_workers[i], NULL);
    }

    /* Cleanup */
    BPLib_QM_QueueTableDestroy(&tbl);
    BPLib_MEM_PoolDestroy(&pool);
    free(pool_mem);

    return 0;
}
