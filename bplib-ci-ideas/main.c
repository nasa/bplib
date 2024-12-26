#include "bundle.h"
#include "qm.h"

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#define NUM_WORKERS 1
#define MAX_JOBS 1024
#define QUEUE_TIMEOUT_MS 100

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
    BPLib_Bundle_t* bundle;

    signal(SIGINT, handle_sigint);

    /* There should be a BPLib_Init() here. */

    /* Setup the queue table. */
    if (!BPLib_QM_QueueTableInit(&tbl, MAX_JOBS))
    {
        fprintf(stderr, "BPLib_QM_QueueTableInit()\n");
        return -1;
    }

    /* Create Generic Workers */
    for (int i = 0; i < NUM_WORKERS; i++)
    {
        pthread_create(&generic_workers[i], NULL, generic_worker, (void*)(&tbl));
    }

    /* Create a single event to kick off the system, this would be done from the network */
    bundle = NULL; // TODO use a MEM alloc
    BPLib_QM_PostEvent(&tbl, bundle, STATE_CLA_TO_BI, QM_PRI_NORMAL, QM_WAIT_FOREVER);

    /* Run the event loop until someone presses CTRL-C */
    BPLib_QM_EventLoop(&tbl, (bool*)(&run));

    /* Join threads */
    for (int i = 0; i < NUM_WORKERS; i++)
    {
        pthread_join(generic_workers[i], NULL);
    }

    /* Cleanup */
    BPLib_QM_QueueTableDestroy(&tbl);

    return 0;

}