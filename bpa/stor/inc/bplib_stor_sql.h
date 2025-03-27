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

#include "bplib_mem.h"
#include "bplib_eid.h"

#include <sqlite3.h>

BPLib_Status_t BPLib_SQL_Init(sqlite3** db, const char* DbName);

BPLib_Status_t BPLib_SQL_StoreBatch(sqlite3* db, BPLib_Bundle_t** BatchArr, size_t BatchSize);

BPLib_Status_t BPLib_SQL_EgressForDestEID(sqlite3* db, BPLib_EID_t* DestEID,
    size_t MaxBundles, size_t* NumEgressed);

BPLib_Status_t BPLib_SQL_DiscardExpired(sqlite3* db, size_t* NumDiscarded);

#endif /* BPLIB_STOR_SQL_H */
