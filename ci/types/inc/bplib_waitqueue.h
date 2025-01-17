#ifndef __GSFC_BPLIB_CI_WAITQUEUE_H__
#define __GSFC_BPLIB_CI_WAITQUEUE_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// TODO: Use bplib_os
#include <pthread.h>

#define WAITQUEUE_NO_WAIT      0L
#define WAITQUEUE_WAIT_FOREVER -1L

typedef struct BPLib_WaitQueue
{
    // init state
    void* storage;
    size_t el_size;
    size_t capacity;
    // internal state
    int front;
    int rear;
    size_t size;
    // Locks/CV
    pthread_mutex_t lock;
    pthread_cond_t cv_pull;
    pthread_cond_t cv_push;
} BPLib_WaitQueue_t;

bool BPLib_CI_WaitQueueInit(BPLib_WaitQueue_t* q, void* storage, size_t el_size, size_t capacity);

void BPLib_CI_WaitQueueDestroy(BPLib_WaitQueue_t* q);

bool BPLib_CI_WaitQueueTryPush(BPLib_WaitQueue_t* q, void* item, int timeout_ms);

bool BPLib_CI_WaitQueueTryPull(BPLib_WaitQueue_t* q, void* ret_item, int timeout_ms);

#endif /* __GSFC_BPLIB_CI_WAITQUEUE_H__ */
