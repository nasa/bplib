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
#include "bplib_stor_loadbatch.h"

BPLib_Status_t BPLib_STOR_LoadBatch_Init(BPLib_STOR_LoadBatch_t* Batch)
{
    /* For now Init and Reset are identical. A separate function is used
    ** because many other BPLib utilities have an Init(). This is stylistic more
    ** than functional. If this is ever made thread-safe, Init also needs to
    ** do some extra one-time setup.
    */
    return BPLib_STOR_LoadBatch_Reset(Batch);
}

BPLib_Status_t BPLib_STOR_LoadBatch_Reset(BPLib_STOR_LoadBatch_t* Batch)
{
    if (Batch == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    Batch->Size = 0;
    Batch->ReadIndex = 0;
    return BPLIB_SUCCESS;
}

bool BPLib_STOR_LoadBatch_IsEmpty(BPLib_STOR_LoadBatch_t* Batch)
{
    return (Batch == NULL || Batch->Size == 0);
}

BPLib_Status_t BPLib_STOR_LoadBatch_AddBundleID(BPLib_STOR_LoadBatch_t* Batch, int64_t BundleID)
{
    if (Batch == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    if (Batch->Size >= BPLIB_STOR_LOADBATCHSIZE)
    {
        return BPLIB_STOR_BATCH_FULL;
    }

    Batch->BundleIDs[Batch->Size++] = BundleID;
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_STOR_LoadBatch_GetNext(BPLib_STOR_LoadBatch_t* Batch, int64_t *BundleID)
{
    if ((Batch == NULL) || (BundleID == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    if (BPLib_STOR_LoadBatch_IsEmpty(Batch))
    {
        return BPLIB_STOR_BATCH_EMPTY;
    }

    if (Batch->ReadIndex >= Batch->Size)
    {
        return BPLIB_STOR_BATCH_CONSUMED;
    }

    *BundleID = Batch->BundleIDs[Batch->ReadIndex++];
    return BPLIB_SUCCESS;
}
