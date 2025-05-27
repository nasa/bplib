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
#define BPLIB_STOR_LOADBATCH_H

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in bplib_stor_loadbatch header
 */

#include "bplib_stor_loadbatch.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_LoadBatch_AddID()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_LoadBatch_AddID(BPLib_STOR_LoadBatch_t *Batch, int64_t BundleID)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_LoadBatch_AddID, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_LoadBatch_AddID, BPLib_STOR_LoadBatch_t *, Batch);
    UT_GenStub_AddParam(BPLib_STOR_LoadBatch_AddID, int64_t, BundleID);

    UT_GenStub_Execute(BPLib_STOR_LoadBatch_AddID, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_LoadBatch_AddID, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_LoadBatch_AdvanceReader()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_LoadBatch_AdvanceReader(BPLib_STOR_LoadBatch_t *Batch)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_LoadBatch_AdvanceReader, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_LoadBatch_AdvanceReader, BPLib_STOR_LoadBatch_t *, Batch);

    UT_GenStub_Execute(BPLib_STOR_LoadBatch_AdvanceReader, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_LoadBatch_AdvanceReader, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_LoadBatch_Init()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_LoadBatch_Init(BPLib_STOR_LoadBatch_t *Batch)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_LoadBatch_Init, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_LoadBatch_Init, BPLib_STOR_LoadBatch_t *, Batch);

    UT_GenStub_Execute(BPLib_STOR_LoadBatch_Init, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_LoadBatch_Init, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_LoadBatch_IsConsumed()
 * ----------------------------------------------------
 */
bool BPLib_STOR_LoadBatch_IsConsumed(BPLib_STOR_LoadBatch_t *Batch)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_LoadBatch_IsConsumed, bool);

    UT_GenStub_AddParam(BPLib_STOR_LoadBatch_IsConsumed, BPLib_STOR_LoadBatch_t *, Batch);

    UT_GenStub_Execute(BPLib_STOR_LoadBatch_IsConsumed, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_LoadBatch_IsConsumed, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_LoadBatch_IsEmpty()
 * ----------------------------------------------------
 */
bool BPLib_STOR_LoadBatch_IsEmpty(BPLib_STOR_LoadBatch_t *Batch)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_LoadBatch_IsEmpty, bool);

    UT_GenStub_AddParam(BPLib_STOR_LoadBatch_IsEmpty, BPLib_STOR_LoadBatch_t *, Batch);

    UT_GenStub_Execute(BPLib_STOR_LoadBatch_IsEmpty, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_LoadBatch_IsEmpty, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_LoadBatch_PeekNextID()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_LoadBatch_PeekNextID(BPLib_STOR_LoadBatch_t *Batch, int64_t *BundleID)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_LoadBatch_PeekNextID, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_LoadBatch_PeekNextID, BPLib_STOR_LoadBatch_t *, Batch);
    UT_GenStub_AddParam(BPLib_STOR_LoadBatch_PeekNextID, int64_t *, BundleID);

    UT_GenStub_Execute(BPLib_STOR_LoadBatch_PeekNextID, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_LoadBatch_PeekNextID, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_LoadBatch_Reset()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_LoadBatch_Reset(BPLib_STOR_LoadBatch_t *Batch)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_LoadBatch_Reset, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_LoadBatch_Reset, BPLib_STOR_LoadBatch_t *, Batch);

    UT_GenStub_Execute(BPLib_STOR_LoadBatch_Reset, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_LoadBatch_Reset, BPLib_Status_t);
}
