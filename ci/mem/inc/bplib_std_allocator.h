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

#ifndef __GSFC_BPLIB_MEM_STDALLOC_H__
#define __GSFC_BPLIB_MEM_STDALLOC_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct BPLib_MEM_PoolImpl
{
    void* mem;
    size_t size;
    uint32_t block_size;
} BPLib_MEM_PoolImpl_t;

bool BPLib_MEM_PoolImplInit(BPLib_MEM_PoolImpl_t* pool, const void* init_mem,
    size_t mem_len, uint32_t block_size);

void BPLib_MEM_PoolImplDestroy(BPLib_MEM_PoolImpl_t* pool);

void* BPLib_MEM_PoolImplAlloc(BPLib_MEM_PoolImpl_t* pool);

void BPLib_MEM_PoolImplFree(BPLib_MEM_PoolImpl_t* pool, void* to_free);

#endif /* __GSFC_BPLIB_MEM_STDALLOC_H__ */
