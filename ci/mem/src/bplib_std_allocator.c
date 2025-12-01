/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */

#include "bplib_std_allocator.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

BPLib_Status_t BPLib_MEM_PoolImplInit(BPLib_MEM_PoolImpl_t* pool, const void* init_mem,
    size_t mem_len, uint32_t block_size)
{
    if (pool == NULL)
    {
        return BPLIB_ERROR;
    }
    if (init_mem != NULL)
    {
        printf("WARNING: STDALLOC doesn't actually use the init_mem param: It just calls malloc()\n");
    }

    memset(pool, 0, sizeof(BPLib_MEM_PoolImpl_t));
    pool->mem = (void*)init_mem;
    pool->size = 0;
    pool->block_size = block_size;
    return BPLIB_SUCCESS;
}

void BPLib_MEM_PoolImplDestroy(BPLib_MEM_PoolImpl_t* pool)
{
    if (pool == NULL)
    {
        return;
    }
    memset(pool, 0, sizeof(BPLib_MEM_PoolImpl_t));
}

void* BPLib_MEM_PoolImplAlloc(BPLib_MEM_PoolImpl_t* pool)
{
    return malloc(pool->block_size);
}

void BPLib_MEM_PoolImplFree(BPLib_MEM_PoolImpl_t* pool, void* to_free)
{
    free(to_free);
}
