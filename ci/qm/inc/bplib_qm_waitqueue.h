/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef BPLIB_QM_WAITQUEUE_H
#define BPLIB_QM_WAITQUEUE_H

#include "bplib_mem.h"
#include "bplib_qm_job.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// TODO: Use bplib_os
#include <pthread.h>

/**
 * @struct BPLib_QM_WaitQueue
 * @brief Represents a queue that supports wait and signal operations in a thead-safe manner.
 * 
 * This structure represents a generic queue with wait and notify semantics. It supports 
 * thread synchronization using mutexes and condition variables for pulling and pushing items.
 */
typedef struct BPLib_QM_WaitQueue
{
    void* storage; /**< Pointer to the storage array used by the queue */
    size_t el_size; /**< Size of each element in the queue */
    size_t capacity; /**< Maximum capacity of the queue */
    
    int front; /**< Index of the front of the queue */
    int rear; /**< Index of the rear of the queue */
    size_t size; /**< Current number of elements in the queue */
    
    pthread_mutex_t lock; /**< Mutex for thread synchronization */
    pthread_cond_t cv_pull; /**< Condition variable for waiting on pulls */
    pthread_cond_t cv_push; /**< Condition variable for waiting on pushes */
} BPLib_QM_WaitQueue_t;

/**
 * @brief Initializes a wait queue.
 * 
 * This function initializes the given wait queue with the provided storage and parameters.
 * It sets up the necessary internal state and synchronization primitives (mutex and condition variables).
 * 
 * @param[out] q The queue to be initialized.
 * @param[in] el_size The size of each element in the queue.
 * @param[in] capacity The maximum capacity of the queue.
 * 
 * @return `true` if the initialization was successful, `false` otherwise.
 */
bool BPLib_QM_WaitQueueInit(BPLib_QM_WaitQueue_t* q, size_t el_size, size_t capacity);

/**
 * @brief Destroys a wait queue.
 * 
 * This function destroys the wait queue, releasing any resources it has allocated (e.g., mutexes, condition variables).
 * 
 * @param[in] q The queue to be destroyed.
 */
void BPLib_QM_WaitQueueDestroy(BPLib_QM_WaitQueue_t* q);

/**
 * @brief Attempts to push an item into the wait queue.
 * 
 * This function attempts to add an item to the queue, with an optional timeout.
 * It blocks until the item is successfully pushed or the timeout is reached.
 * 
 * @param[in] q The queue to push the item into.
 * @param[in] item The item to be added to the queue.
 * @param[in] timeout_ms The timeout in milliseconds. If the queue is full, it waits until this timeout expires.
 * 
 * @return `true` if the item was successfully pushed, `false` if the operation timed out.
 */
bool BPLib_QM_WaitQueueTryPush(BPLib_QM_WaitQueue_t* q, const void* item, int timeout_ms);

/**
 * @brief Attempts to pull an item from the wait queue.
 * 
 * This function attempts to retrieve an item from the queue, with an optional timeout.
 * It blocks until an item is available or the timeout is reached.
 * 
 * @param[in] q The queue to pull the item from.
 * @param[out] ret_item The item retrieved from the queue.
 * @param[in] timeout_ms The timeout in milliseconds. If the queue is empty, it waits until this timeout expires.
 * 
 * @return `true` if an item was successfully pulled, `false` if the operation timed out.
 */
bool BPLib_QM_WaitQueueTryPull(BPLib_QM_WaitQueue_t* q, void* ret_item, int timeout_ms);

/* Note: The documentation above applies for each of these queue types.
** These wrappers intend to add a level of typesafety because the generic queue implementation.
** Please be extremely cautious using WaitQueue_t directly, instead preferring one of these wrappers.
*/
/*******************************************************************************
** Bundle Queue Wrapper 
*/
typedef struct BPLib_QM_BundleQueue
{
    BPLib_QM_WaitQueue_t BaseQueue;
} BPLib_QM_BundleQueue_t;

static inline bool BPLib_QM_BundleQueueInit(BPLib_QM_BundleQueue_t* q, size_t Capacity)
{
    return BPLib_QM_WaitQueueInit(&q->BaseQueue, sizeof(BPLib_Bundle_t*), Capacity);
}

static inline void BPLib_QM_BundleQueueDestroy(BPLib_QM_BundleQueue_t* q)
{
    BPLib_QM_WaitQueueDestroy(&q->BaseQueue);
}

static inline bool BPLib_QM_BundleQueueTryPush(BPLib_QM_BundleQueue_t* q, const BPLib_Bundle_t* Bundle, int TimeoutMs)
{
    return BPLib_QM_WaitQueueTryPush(&q->BaseQueue, (const void*)&Bundle, TimeoutMs);
}

static inline bool BPLib_QM_BundleQueueTryPull(BPLib_QM_BundleQueue_t* q, BPLib_Bundle_t* Bundle, int TimeoutMs)
{
    return BPLib_QM_WaitQueueTryPull(&q->BaseQueue, (void*)(&Bundle), TimeoutMs);
}

/*******************************************************************************
** Job Queue Wrapper 
*/
typedef struct BPLib_QM_JobQueue
{
    BPLib_QM_WaitQueue_t BaseQueue;
} BPLib_QM_JobQueue_t;

static inline bool BPLib_QM_JobQueueInit(BPLib_QM_JobQueue_t* q, size_t Capacity)
{
    return BPLib_QM_WaitQueueInit(&q->BaseQueue, sizeof(BPLib_QM_Job_t), Capacity);
}

static inline void BPLib_QM_JobQueueDestroy(BPLib_QM_JobQueue_t* q)
{
    BPLib_QM_WaitQueueDestroy(&q->BaseQueue);
}

static inline bool BPLib_QM_JobQueueTryPush(BPLib_QM_JobQueue_t* q, const BPLib_QM_Job_t* Job, int TimeoutMs)
{
    return BPLib_QM_WaitQueueTryPush(&q->BaseQueue, (const void*)Job, TimeoutMs);
}

static inline bool BPLib_QM_JobQueueTryPull(BPLib_QM_JobQueue_t* q, BPLib_QM_Job_t* Job, int TimeoutMs)
{
    return BPLib_QM_WaitQueueTryPull(&q->BaseQueue, (void*)&Job, TimeoutMs);
}

#endif /* BPLIB_QM_WAITQUEUE_H */
