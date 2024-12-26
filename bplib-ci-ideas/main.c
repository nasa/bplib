#include "qm.h"

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#define NUM_WORKERS 4
#define MAX_JOBS 1024

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
    while (run)
    {
        usleep(100e3);
    }

    return NULL;
}

int main(int argc, char** argv)
{
    pthread_t generic_workers[NUM_WORKERS];
    BPLib_QM_QueueTable_t tbl;

    signal(SIGINT, handle_sigint);

    if (!BPLib_QM_QueueTableInit(&tbl, MAX_JOBS))
    {
        printf("QM Failed to init\n");
        return -1;
    }

    /* Create Generic Workers */
    for (int i = 0; i < NUM_WORKERS; i++)
    {
        pthread_create(&generic_workers[i], NULL, generic_worker, (void*)(&tbl));
    }

    /* Main task goes here */
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