#ifndef __GSFC_BPLIB_WAITQUEUE_H__
#define __GSFC_BPLIB_WAITQUEUE_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define WAITQUEUE_NONBLOCK 0
#define WAITQUEUE_BLOCK_FOREVER -1

typedef struct BPLib_WaitQueueItem
{
    // Just a linked list node?
} BPLib_WaitQueueItem_t;

typedef struct BPLib_WaitQueue
{
} BPLib_WaitQueue_t;

void BPLib_CI_WaitQueueInit(BPLib_WaitQueue_t* q);

void BPLib_CI_WaitQueueDestroy(BPLib_WaitQueue_t* q);

void BPLib_CI_WaitQueueTryPush(BPLib_WaitQueue_t* q, BPLib_QueueItem_t *item, int timeout);

BPLib_QueueItem_t BPLib_CI_WaitQueueTryPull(BPLib_Queue_t* q, int timeout);

#endif /* __GSFC_BPLIB_WAITQUEUE_H__ */
