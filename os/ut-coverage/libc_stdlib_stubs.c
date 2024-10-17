/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/* OSAL coverage stub replacement for stdlib.h */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utstubs.h"
#include "utassert.h"

#include "bplib_cs_stdlib.h"

void *BPLIB_CS_calloc(size_t nmemb, size_t sz)
{
    int32  Status;
    void  *PoolPtr;
    size_t PoolSize;
    size_t BlockSize;

    BlockSize = nmemb * sz;
    UT_GetDataBuffer(UT_KEY(BPLIB_CS_calloc), &PoolPtr, &PoolSize, NULL);

    Status = UT_DEFAULT_IMPL(BPLIB_CS_calloc);

    if (Status != 0)
    {
        return NULL;
    }

    if (BlockSize > PoolSize)
    {
        /*
         * This indicates that the application is trying to allocate
         * a block larger than the pool.  It typically means that the
         * emulated heap size is too small, so it is prudent to generate
         * a message.
         */
        UtAssert_Failed("BPLIB_CS_calloc() heap has been exhausted");
        return NULL;
    }

    return (void *)PoolPtr;
}

void BPLIB_CS_free(void *ptr)
{
    void  *PoolPtr;
    size_t PoolSize;
    int32  Status;

    /*
     * If there is a data buffer associated with free() then this
     * will sanity-check that the block being freed came from that heap.
     */
    UT_GetDataBuffer(UT_KEY(BPLIB_CS_free), &PoolPtr, &PoolSize, NULL);

    Status = UT_DEFAULT_IMPL(BPLIB_CS_free);
    if (Status == 0 && PoolPtr != NULL && PoolPtr != ptr)
    {
        UtAssert_Failed("BPLIB_CS_free(): Heap corruption -- pointer not correct");
    }
}
