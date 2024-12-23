#include "waitqueue.h"

#include <string.h>

bool BPLib_CI_WaitQueueInit(BPLib_WaitQueue_t* q, void* storage, size_t el_size, size_t capacity)
{
    if ((q == NULL) || (storage == NULL) || (el_size == 0) || (capacity < 2))
    {
        return false;
    }

    pthread_mutex_init(&q->lock, NULL);
    q->storage = storage;
    q->el_size = el_size;
    q->capacity = capacity;
    q->front = 0;
    q->rear = capacity - 1;
    q->size = 0;

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
}

bool BPLib_CI_WaitQueueTryPush(BPLib_WaitQueue_t* q, void* item, int timeout)
{
    if ((q == NULL) || (item == NULL))
    {
        return false;
    }

    if (q->size == q->capacity)
    {
        return false; // becomes a wait
    }

    // LOCK
    q->rear = (q->rear  + 1) % q->capacity;
    memcpy((void*)(((char *)q->storage) + q->rear), item, q->el_size);
    q->size++;
    // UNLOCK

    return true;
}

bool BPLib_CI_WaitQueueTryPull(BPLib_WaitQueue_t* q, void* ret_item, int timeout)
{
    if ((q == NULL) || (ret_item == NULL))
    {
        return false;
    }

    if (q->size == 0)
    {
        return false; // becomes a wait
    }

    // LOCK
    memcpy(ret_item, (void*)(((char *)q->storage) + q->front), q->el_size);
    q->size--;
    q->front = (q->front + 1) % (q->capacity); 
    // UNLOCK

    return true;
}
