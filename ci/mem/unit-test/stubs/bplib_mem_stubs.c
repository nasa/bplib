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

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in bplib_mem header
 */

#include "bplib_mem.h"
#include "utgenstub.h"


/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_CopyOutFromOffset()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_MEM_CopyOutFromOffset(BPLib_Bundle_t* Bundle,
                                            uint64_t Offset,
                                            uint64_t NumBytesToCopy,
                                            void* OutputBuffer,
                                            size_t OutputBufferSize)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_CopyOutFromOffset, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_MEM_CopyOutFromOffset, BPLib_Bundle_t *, Bundle);
    UT_GenStub_AddParam(BPLib_MEM_CopyOutFromOffset, uint64_t, Offset);
    UT_GenStub_AddParam(BPLib_MEM_CopyOutFromOffset, uint64_t, NumBytesToCopy);
    UT_GenStub_AddParam(BPLib_MEM_CopyOutFromOffset, void *, OutputBuffer);
    UT_GenStub_AddParam(BPLib_MEM_CopyOutFromOffset, size_t, OutputBufferSize);

    UT_GenStub_Execute(BPLib_MEM_CopyOutFromOffset, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_CopyOutFromOffset, BPLib_Status_t);
}


/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BlobCopyOut()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_MEM_BlobCopyOut(BPLib_Bundle_t *bundle, void *out_buffer, size_t max_len, size_t *out_size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_BlobCopyOut, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_MEM_BlobCopyOut, BPLib_Bundle_t *, bundle);
    UT_GenStub_AddParam(BPLib_MEM_BlobCopyOut, void *, out_buffer);
    UT_GenStub_AddParam(BPLib_MEM_BlobCopyOut, size_t, max_len);
    UT_GenStub_AddParam(BPLib_MEM_BlobCopyOut, size_t *, out_size);

    UT_GenStub_Execute(BPLib_MEM_BlobCopyOut, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_BlobCopyOut, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BlockAlloc()
 * ----------------------------------------------------
 */
BPLib_MEM_Block_t *BPLib_MEM_BlockAlloc(BPLib_MEM_Pool_t *pool)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_BlockAlloc, BPLib_MEM_Block_t *);

    UT_GenStub_AddParam(BPLib_MEM_BlockAlloc, BPLib_MEM_Pool_t *, pool);

    UT_GenStub_Execute(BPLib_MEM_BlockAlloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_BlockAlloc, BPLib_MEM_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BlockFree()
 * ----------------------------------------------------
 */
void BPLib_MEM_BlockFree(BPLib_MEM_Pool_t *pool, BPLib_MEM_Block_t *block)
{
    UT_GenStub_AddParam(BPLib_MEM_BlockFree, BPLib_MEM_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_MEM_BlockFree, BPLib_MEM_Block_t *, block);

    UT_GenStub_Execute(BPLib_MEM_BlockFree, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BlockListAlloc()
 * ----------------------------------------------------
 */
BPLib_MEM_Block_t *BPLib_MEM_BlockListAlloc(BPLib_MEM_Pool_t *pool, size_t byte_len)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_BlockListAlloc, BPLib_MEM_Block_t *);

    UT_GenStub_AddParam(BPLib_MEM_BlockListAlloc, BPLib_MEM_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_MEM_BlockListAlloc, size_t, byte_len);

    UT_GenStub_Execute(BPLib_MEM_BlockListAlloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_BlockListAlloc, BPLib_MEM_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BlockListFree()
 * ----------------------------------------------------
 */
void BPLib_MEM_BlockListFree(BPLib_MEM_Pool_t *pool, BPLib_MEM_Block_t *head)
{
    UT_GenStub_AddParam(BPLib_MEM_BlockListFree, BPLib_MEM_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_MEM_BlockListFree, BPLib_MEM_Block_t *, head);

    UT_GenStub_Execute(BPLib_MEM_BlockListFree, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BundleAlloc()
 * ----------------------------------------------------
 */
BPLib_Bundle_t *BPLib_MEM_BundleAlloc(BPLib_MEM_Pool_t *pool, const void *blob_data, size_t data_len)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_BundleAlloc, BPLib_Bundle_t *);

    UT_GenStub_AddParam(BPLib_MEM_BundleAlloc, BPLib_MEM_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_MEM_BundleAlloc, const void *, blob_data);
    UT_GenStub_AddParam(BPLib_MEM_BundleAlloc, size_t, data_len);

    UT_GenStub_Execute(BPLib_MEM_BundleAlloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_BundleAlloc, BPLib_Bundle_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BundleFree()
 * ----------------------------------------------------
 */
void BPLib_MEM_BundleFree(BPLib_MEM_Pool_t *pool, BPLib_Bundle_t *bundle)
{
    UT_GenStub_AddParam(BPLib_MEM_BundleFree, BPLib_MEM_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_MEM_BundleFree, BPLib_Bundle_t *, bundle);

    UT_GenStub_Execute(BPLib_MEM_BundleFree, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_PoolDestroy()
 * ----------------------------------------------------
 */
void BPLib_MEM_PoolDestroy(BPLib_MEM_Pool_t *pool)
{
    UT_GenStub_AddParam(BPLib_MEM_PoolDestroy, BPLib_MEM_Pool_t *, pool);

    UT_GenStub_Execute(BPLib_MEM_PoolDestroy, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_PoolInit()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_MEM_PoolInit(BPLib_MEM_Pool_t *pool, void *init_mem, size_t init_size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_PoolInit, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_MEM_PoolInit, BPLib_MEM_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_MEM_PoolInit, void *, init_mem);
    UT_GenStub_AddParam(BPLib_MEM_PoolInit, size_t, init_size);

    UT_GenStub_Execute(BPLib_MEM_PoolInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_PoolInit, BPLib_Status_t);
}
