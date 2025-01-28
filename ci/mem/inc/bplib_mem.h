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

#ifndef BPLIB_MEM_H
#define BPLIB_MEM_H

#include "bplib_ben_allocator.h"

#include <pthread.h>

#define BPLIB_MEM_CHUNKSIZE (512U)

typedef struct BPLib_MEM_Block
{
    uint8_t chunk[BPLIB_MEM_CHUNKSIZE];
    size_t chunk_len;
    struct BPLib_MEM_Block* next;
} BPLib_MEM_Block_t;

// TODO: Make implementation private if desired
typedef struct BPLib_MEM_Pool
{
    BPLib_MEM_PoolImpl_t impl;
    pthread_mutex_t lock;
} BPLib_MEM_Pool_t;

BPLib_Status_t BPLib_MEM_PoolInit(BPLib_MEM_Pool_t* pool, void* init_mem, size_t init_size);

void BPLib_MEM_PoolDestroy(BPLib_MEM_Pool_t* pool);

BPLib_MEM_Block_t* BPLib_MEM_BlockAlloc(BPLib_MEM_Pool_t* pool);

void BPLib_MEM_BlockFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* block);

BPLib_MEM_Block_t* BPLib_MEM_BlockListAlloc(BPLib_MEM_Pool_t* pool, size_t byte_len);

void BPLib_MEM_BlockListFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* head);

#endif /* BPLIB_MEM_H */
