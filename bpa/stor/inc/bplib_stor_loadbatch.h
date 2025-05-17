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
#ifndef BPLIB_STOR_LOADBATCH_H
#define BPLIB_STOR_LOADBATCH_H

#include "bplib_api_types.h"

/* Important Note:
** Load batch loads integers into memory. It does not load bundle blobs. You can
** set this number north of 250k and everything works fine. The tradeoff is how much
** system memory the array of integers takes up. For performance analysis, I reccommend setting
** this to a higher value using a CMake variable.  In general, nothing can keep up with how
** fast we egress (good problem to have right now). To slow this down, a separate ticket
** needs to be written to "artificially" rate limiting in BPNode CLAs. Limiting can't be
** easily done in BPLib
** in BPNode, not bplib.
*/
#ifndef BPLIB_STOR_LOADBATCH_SIZE
#define BPLIB_STOR_LOADBATCHSIZE 1000
#endif

typedef struct BPLib_STOR_LoadBatch
{
    int64_t BundleIDs[BPLIB_STOR_LOADBATCHSIZE];
    size_t Size;
    size_t ReadIndex;
} BPLib_STOR_LoadBatch_t;

BPLib_Status_t BPLib_STOR_LoadBatch_Init(BPLib_STOR_LoadBatch_t* Batch);

BPLib_Status_t BPLib_STOR_LoadBatch_Reset(BPLib_STOR_LoadBatch_t* Batch);

bool BPLib_STOR_LoadBatch_IsEmpty(BPLib_STOR_LoadBatch_t* Batch);

bool BPLib_STOR_LoadBatch_IsConsumed(BPLib_STOR_LoadBatch_t* Batch);

BPLib_Status_t BPLib_STOR_LoadBatch_AddID(BPLib_STOR_LoadBatch_t* Batch, int64_t BundleID);

BPLib_Status_t BPLib_STOR_LoadBatch_PeekNextID(BPLib_STOR_LoadBatch_t* Batch, int64_t *BundleID);

BPLib_Status_t BPLib_STOR_LoadBatch_AdvanceReader(BPLib_STOR_LoadBatch_t* Batch);

#endif /* BPLIB_STOR_LOADBATCH_H */