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
