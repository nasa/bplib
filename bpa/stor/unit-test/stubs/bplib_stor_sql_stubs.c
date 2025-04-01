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
#define BPLIB_STOR_SQL_H

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in bplib_stor_sql header
 */

#include "bplib_stor_sql.h"
#include "utgenstub.h"
#include "bplib_api_types.h"
#include "bplib_eid.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_SQL_EgressForDestEID()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_SQL_EgressForDestEID(BPLib_Instance_t *Inst, BPLib_EID_Pattern_t *DestEID, size_t MaxBundles)
{
    UT_GenStub_SetupReturnBuffer(BPLib_SQL_EgressForDestEID, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_SQL_EgressForDestEID, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_SQL_EgressForDestEID, BPLib_EID_Pattern_t *, DestEID);
    UT_GenStub_AddParam(BPLib_SQL_EgressForDestEID, size_t, MaxBundles);

    UT_GenStub_Execute(BPLib_SQL_EgressForDestEID, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_SQL_EgressForDestEID, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_SQL_GarbageCollect()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_SQL_GarbageCollect(BPLib_Instance_t *Inst, size_t *NumDiscarded)
{
    UT_GenStub_SetupReturnBuffer(BPLib_SQL_GarbageCollect, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_SQL_GarbageCollect, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_SQL_GarbageCollect, size_t *, NumDiscarded);

    UT_GenStub_Execute(BPLib_SQL_GarbageCollect, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_SQL_GarbageCollect, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_SQL_Init()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_SQL_Init(BPLib_Instance_t *Inst, const char *DbName)
{
    UT_GenStub_SetupReturnBuffer(BPLib_SQL_Init, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_SQL_Init, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_SQL_Init, const char *, DbName);

    UT_GenStub_Execute(BPLib_SQL_Init, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_SQL_Init, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_SQL_Store()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_SQL_Store(BPLib_Instance_t *Inst)
{
    UT_GenStub_SetupReturnBuffer(BPLib_SQL_Store, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_SQL_Store, BPLib_Instance_t *, Inst);

    UT_GenStub_Execute(BPLib_SQL_Store, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_SQL_Store, BPLib_Status_t);
}
