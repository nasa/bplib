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
#include "bplib_stor_loadbatch.h"

BPLib_Status_t BPLib_STOR_LoadBatch_Init(BPLib_STOR_LoadBatch_t* Batch)
{
    /* For now Init and Reset are identical. A separate function is used
    ** because many other BPLib utilities have an Init(). This is stylistic more
    ** than functional. If this is ever made thread-safe, Init also needs to
    ** do some extra one-time setup for mutexes
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
    if (Batch == NULL)
    {
        return true;
    }
    return (Batch->Size == 0);
}

bool BPLib_STOR_LoadBatch_IsConsumed(BPLib_STOR_LoadBatch_t* Batch)
{
    if (Batch == NULL)
    {
        return true;
    }
    return (Batch->ReadIndex >= Batch->Size);
}

BPLib_Status_t BPLib_STOR_LoadBatch_AddID(BPLib_STOR_LoadBatch_t* Batch, int64_t BundleID)
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

BPLib_Status_t BPLib_STOR_LoadBatch_PeekNextID(BPLib_STOR_LoadBatch_t* Batch, int64_t *BundleID)
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

    *BundleID = Batch->BundleIDs[Batch->ReadIndex];
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_STOR_LoadBatch_AdvanceReader(BPLib_STOR_LoadBatch_t* Batch)
{
    if (Batch == NULL)
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

    Batch->ReadIndex++;
    return BPLIB_SUCCESS;
}
