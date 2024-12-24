#include "waitqueue.h"

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <signal.h>
#include <unistd.h>

typedef struct thread_run_state
{
    bool* should_run;
    BPLib_WaitQueue_t* q;
} thread_run_state_t;

volatile sig_atomic_t run = 1;

void handle_sigint(int sig)
{
    run = 0;
}

void* producer_func(void* arg)
{
    thread_run_state_t* state = (thread_run_state_t*)(arg);
    BPLib_WaitQueue_t* q  = (state->q);

    int val = 0;

    while (*state->should_run)
    {
        if (BPLib_CI_WaitQueueTryPush(q, &val, 100))
        {
            printf("Thread-%lu pushed %d\n", pthread_self(), val);
            val++;
            usleep(250e3);
        }
    }

    return NULL;
}

void* consumer_func(void* arg)
{
    thread_run_state_t* state = (thread_run_state_t*)(arg);
    BPLib_WaitQueue_t* q  = (state->q);

    int ret_val;

    while (*state->should_run)
    {
        if (BPLib_CI_WaitQueueTryPull(q, &ret_val, 100))
        {
            printf("Thread-%lu got %d\n", pthread_self(), ret_val);
        }
    }

    return NULL;
}

int main(int argc, char** argv)
{
    int storage[5];
    BPLib_WaitQueue_t q;
    bool should_run;
    thread_run_state_t state;
    pthread_t consumers[10];
    pthread_t producers[10];
    int num_consumers;
    int num_producers;

    signal(SIGINT, handle_sigint);

    if (!BPLib_CI_WaitQueueInit(&q, (void*)(storage), sizeof(int), 5))
    {
        printf("Queue didn't init\n");
        return -1;
    }

    /* Create threads */
    state.q = &q;
    state.should_run = &should_run;
    num_consumers = 2;
    num_producers = 1;
    should_run = true;
    for (int i = 0; i < num_consumers; i++)
    {
        pthread_create(&consumers[i], NULL, consumer_func, (void*)(&state));
    }
    for (int i = 0; i < num_producers; i++)
    {
        pthread_create(&producers[i], NULL, producer_func, (void*)(&state));
    }

    /* Main task goes here */
    while (run)
    {
        usleep(250e3);
    }

    /* Join threads */
    should_run = false;
    for (int i = 0; i < num_consumers; i++)
    {
        pthread_join(consumers[i], NULL);
    }
    for (int i = 0; i < num_producers; i++)
    {
        pthread_join(producers[i], NULL);
    }

    BPLib_CI_WaitQueueDestroy(&q);

    return 0;

}