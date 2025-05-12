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

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in bplib_stor header
 */

#include "bplib_stor.h"
#include "utgenstub.h"

BPLib_StorageHkTlm_Payload_t BPLib_STOR_StoragePayload;

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_Destroy()
 * ----------------------------------------------------
 */
void BPLib_STOR_Destroy(BPLib_Instance_t *Inst)
{
    UT_GenStub_AddParam(BPLib_STOR_Destroy, BPLib_Instance_t *, Inst);

    UT_GenStub_Execute(BPLib_STOR_Destroy, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_EgressForID()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_EgressForID(BPLib_Instance_t *Inst, uint16_t EgressID, bool LocalDelivery,
                                      size_t *NumEgressed)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_EgressForID, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_EgressForID, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_STOR_EgressForID, uint16_t, EgressID);
    UT_GenStub_AddParam(BPLib_STOR_EgressForID, bool, LocalDelivery);
    UT_GenStub_AddParam(BPLib_STOR_EgressForID, size_t *, NumEgressed);

    UT_GenStub_Execute(BPLib_STOR_EgressForID, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_EgressForID, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_FlushPending()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_FlushPending(BPLib_Instance_t *Inst)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_FlushPending, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_FlushPending, BPLib_Instance_t *, Inst);

    UT_GenStub_Execute(BPLib_STOR_FlushPending, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_FlushPending, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_GarbageCollect()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_GarbageCollect(BPLib_Instance_t *Inst, size_t *NumDiscarded)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_GarbageCollect, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_GarbageCollect, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_STOR_GarbageCollect, size_t *, NumDiscarded);

    UT_GenStub_Execute(BPLib_STOR_GarbageCollect, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_GarbageCollect, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_Init()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_Init(BPLib_Instance_t *Inst)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_Init, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_Init, BPLib_Instance_t *, Inst);

    UT_GenStub_Execute(BPLib_STOR_Init, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_Init, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_ScanCache()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_ScanCache(BPLib_Instance_t *Inst)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_ScanCache, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_ScanCache, BPLib_Instance_t *, Inst);

    UT_GenStub_Execute(BPLib_STOR_ScanCache, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_ScanCache, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_StorageTblValidateFunc()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_StorageTblValidateFunc(void *TblData)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_StorageTblValidateFunc, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_StorageTblValidateFunc, void *, TblData);

    UT_GenStub_Execute(BPLib_STOR_StorageTblValidateFunc, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_StorageTblValidateFunc, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_StoreBundle()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t *Inst, BPLib_Bundle_t *Bundle)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_StoreBundle, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_STOR_StoreBundle, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_STOR_StoreBundle, BPLib_Bundle_t *, Bundle);

    UT_GenStub_Execute(BPLib_STOR_StoreBundle, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_StoreBundle, BPLib_Status_t);
}
