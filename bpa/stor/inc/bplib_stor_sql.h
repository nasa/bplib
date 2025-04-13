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

/* This is a temporary define denoating milliseconds between POSIX and DTN time
** It is used until a more mature boot-era solution is implemented in BPLIB_TIME
*/
#define BPLIB_STOR_EPOCHOFFSET 946684800000

BPLib_Status_t BPLib_SQL_Init(BPLib_Instance_t* Inst, const char* DbName);

BPLib_Status_t BPLib_SQL_Store(BPLib_Instance_t* Inst);

BPLib_Status_t BPLib_SQL_EgressForDestEID(BPLib_Instance_t* Inst, BPLib_EID_Pattern_t* DestEID,
    size_t MaxBundles);

BPLib_Status_t BPLib_SQL_GarbageCollect(BPLib_Instance_t* Inst, size_t* NumDiscarded);

#endif /* BPLIB_STOR_SQL_H */
