#include "bplib_qm_waitqueue.h"

#include <string.h>
#include <time.h>

#include <errno.h>
#include <stdio.h>

static void ms_to_abstimeout(uint32_t ms, struct timespec *ts)
{
    if (ts == NULL)
    {
        return;
    }

    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec += ms / 1000;
    ts->tv_nsec += (ms % 1000 * 1000000);
    ts->tv_sec += ts->tv_nsec / 1000000000;
    ts->tv_nsec %= 1000000000;
}

/*******************************************************************************
* Exported Functions
*/
bool BPLib_CI_WaitQueueInit(BPLib_WaitQueue_t* q, void* storage, size_t el_size, size_t capacity)
{
    if ((q == NULL) || (storage == NULL) || (el_size == 0) || (capacity < 2))
    {
        return false;
    }

    q->storage = storage;
    q->el_size = el_size;
    q->capacity = capacity;
    q->front = 0;
    q->rear = capacity - 1;
    q->size = 0;

    // TODO: move to bplib_OS module
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->cv_push, NULL);
    pthread_cond_init(&q->cv_pull, NULL);
    return true;
}

void BPLib_CI_WaitQueueDestroy(BPLib_WaitQueue_t* q)
{
    if (q == NULL)
    {
        return;
    }

    q->storage = NULL;
    q->el_size = 0;
    q->capacity = 0;
    q->front = 0;
    q->rear = 0;
    q->size = 0;

    // TODO: move to bplib_OS module
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->cv_push);
    pthread_cond_destroy(&q->cv_pull);
}

bool BPLib_CI_WaitQueueTryPush(BPLib_WaitQueue_t* q, void* item, int timeout_ms)
{
    struct timespec deadline;
    int rc;

    if ((q == NULL) || (item == NULL))
    {
        return false;
    }

    ms_to_abstimeout((uint32_t)(timeout_ms), &deadline);
    pthread_mutex_lock(&q->lock);
    /**** Critical Section Begin ****/

    /* Wait for queue to be non-full */
    while (q->size == q->capacity)
    {
        rc = pthread_cond_timedwait(&q->cv_push, &q->lock, &deadline);
        if (rc != 0)
        {
            if (rc != ETIMEDOUT)
            {
                printf(" BPLib_CI_WaitQueueTryPush NON-TIMEOUT ERROR: %s\n", strerror(rc));
            }
            pthread_mutex_unlock(&q->lock);
            return false;
        }
    }

    /* Push an item */
    q->rear = (q->rear  + 1) % q->capacity;
    memcpy((void*)(((char *)q->storage) + (q->rear*q->el_size)), item, q->el_size);
    q->size++;

    /* Notify other pulling threads that an item can be pulled. */
    pthread_cond_signal(&q->cv_pull);

    /**** Critical Section End ****/
    pthread_mutex_unlock(&q->lock);

    return true;
}

bool BPLib_CI_WaitQueueTryPull(BPLib_WaitQueue_t* q, void* ret_item, int timeout_ms)
{
    struct timespec deadline;
    int rc;

    if ((q == NULL) || (ret_item == NULL))
    {
        return false;
    }

    ms_to_abstimeout((uint32_t)(timeout_ms), &deadline);
    pthread_mutex_lock(&q->lock);
    /**** Critical Section Begin ****/
 
    /* Wait for queue to be non-empty */
    while (q->size == 0)
    {
        rc = pthread_cond_timedwait(&q->cv_pull, &q->lock, &deadline);
        if (rc != 0)
        {
            if (rc != ETIMEDOUT)
            {
                printf(" BPLib_CI_WaitQueueTryPull NON-TIMEOUT ERROR: %s\n", strerror(rc));
            }
            pthread_mutex_unlock(&q->lock);
            return false;
        }
    }

    /* Pull an item */
    memcpy(ret_item, (void*)(((char *)q->storage) + (q->front*q->el_size)), q->el_size);
    q->size--;
    q->front = (q->front + 1) % (q->capacity); 

    /* Notify other pushing threads that an item can be pushed */
    pthread_cond_signal(&q->cv_push);

    /**** Critical Section End ****/
    pthread_mutex_unlock(&q->lock);

    return true;
}
