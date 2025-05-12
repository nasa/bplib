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

/* Note: This is a large number of bundles. This value may need to be tweaked for a different target platform.
** From studying performance implications, it's been discovered that large but
** infreuqent searches for bundles is far less resource intensive than frequent small
** searches.  BundleIDs (which are only integers and not the actual bundle data, get loaded
** into this batch.  Then, as QM can accept the actual bundles, the binary data is loaded one-by-one from storage
** as memory becomes available.  The binary itself doesn't needed to be loaded in batch because
** SQL has an API where the lookup is constant time for BLOBs.
*/
#define BPLIB_STOR_LOADBATCHSIZE 2000

typedef struct BPLib_STOR_LoadBatch
{
    int64_t BundleIDs[BPLIB_STOR_LOADBATCHSIZE];
    size_t Size;
    size_t Head;
    size_t Tail;
} BPLib_STOR_LoadBatch_t;

BPLib_Status_t BPLib_STOR_LoadBatch_Init(BPLib_STOR_LoadBatch_t* Batch);

bool BPLib_STOR_LoadBatch_IsEmpty(BPLib_STOR_LoadBatch_t* Batch);

BPLib_Status_t BPLib_STOR_LoadBatch_GetNext(BPLib_STOR_LoadBatch_t* Batch, int64_t *BundleID);

BPLib_Status_t BPLib_STOR_LoadBatch_AddBundleID(BPLib_STOR_LoadBatch_t* Batch, int64_t BundleID);

#endif /* BPLIB_STOR_LOADBATCH_H */
