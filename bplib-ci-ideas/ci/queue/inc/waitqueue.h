#ifndef __GSFC_BPLIB_WAITQUEUE_H__
#define __GSFC_BPLIB_WAITQUEUE_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// Use bplib_os
#include <pthread.h>

#define WAITQUEUE_NONBLOCK 0
#define WAITQUEUE_BLOCK_FOREVER -1

/*
** The idea here is that the user passes in the underlying queue storage.
**
** NOTE: This ends up with the effect that each queue is sized at either compile time (stack alloc) 
** or init time (one-time malloc) and will always be explicity bound to that size. 
** I find this preferable to the queue being implicitly bound to how much free memory 
** remains in MEM, but others may not agree. A side-effect is that a "generic" void*
** is used as the parameter to objects being pushed/pulled.  If that gives heartburn, 
** we could write "derived" queues that wrap these calls with their specific data-types.
*/
typedef struct BPLib_WaitQueue
{
    pthread_mutex_t lock;
    void* storage;
    size_t el_size;
    size_t capacity;
    // queue_state
    int front;
    int rear;
    size_t size;
} BPLib_WaitQueue_t;

bool BPLib_CI_WaitQueueInit(BPLib_WaitQueue_t* q, void* storage, size_t el_size, size_t capacity);

void BPLib_CI_WaitQueueDestroy(BPLib_WaitQueue_t* q);

bool BPLib_CI_WaitQueueTryPush(BPLib_WaitQueue_t* q, void* item, int timeout);

bool BPLib_CI_WaitQueueTryPull(BPLib_WaitQueue_t* q, void* ret_item, int timeout);

#endif /* __GSFC_BPLIB_WAITQUEUE_H__ */
