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
#ifndef BPLIB_STOR_SQL_H
#define BPLIB_STOR_SQL_H

#include "bplib_api_types.h"
#include "bplib_mem.h"
#include "bplib_eid.h"
#include "bplib_stor_loadbatch.h"

#define BPLIB_SQL_MAX_STRLEN 4096

/* This is a temporary define denoting milliseconds between POSIX and DTN time
** It is used until a more mature boot-era solution is implemented in BPLIB_TIME
*/
#define BPLIB_STOR_EPOCHOFFSET 946684800000

BPLib_Status_t BPLib_SQL_Init(BPLib_Instance_t* Inst, const char* DbName);

BPLib_Status_t BPLib_SQL_Store(BPLib_Instance_t* Inst, size_t *TotalBytesStored);

BPLib_Status_t BPLib_SQL_DiscardExpired(BPLib_Instance_t* Inst, size_t* NumDiscarded);

BPLib_Status_t BPLib_SQL_DiscardEgressed(BPLib_Instance_t* Inst, size_t* NumDiscarded);

BPLib_Status_t BPLib_SQL_FindForEIDs(BPLib_Instance_t* Inst, BPLib_STOR_LoadBatch_t* Batch,
    BPLib_EID_Pattern_t *DestEIDs, size_t NumEIDs);

BPLib_Status_t BPLib_SQL_MarkBatchEgressed(BPLib_Instance_t* Inst, BPLib_STOR_LoadBatch_t* Batch);

BPLib_Status_t BPLib_SQL_LoadBundle(BPLib_Instance_t* Inst, int64_t BundleID, BPLib_Bundle_t** Bundle);

BPLib_Status_t BPLib_SQL_GetDbSize(BPLib_Instance_t *Inst, size_t *DbSize);

#endif /* BPLIB_STOR_SQL_H */
