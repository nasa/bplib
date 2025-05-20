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
 * Auto-Generated stub implementations for functions defined in bplib_pi header
 */

#include "bplib_pi.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_PI_AddApplication()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_PI_AddApplication(uint32_t ChanId)
{
    UT_GenStub_SetupReturnBuffer(BPLib_PI_AddApplication, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_PI_AddApplication, uint32_t, ChanId);

    UT_GenStub_Execute(BPLib_PI_AddApplication, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_PI_AddApplication, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_PI_Egress()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_PI_Egress(BPLib_Instance_t *Inst, uint8_t ChanId, void *AduPtr, size_t *AduSize, size_t BufLen,
                               uint32_t Timeout)
{
    UT_GenStub_SetupReturnBuffer(BPLib_PI_Egress, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_PI_Egress, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_PI_Egress, uint8_t, ChanId);
    UT_GenStub_AddParam(BPLib_PI_Egress, void *, AduPtr);
    UT_GenStub_AddParam(BPLib_PI_Egress, size_t *, AduSize);
    UT_GenStub_AddParam(BPLib_PI_Egress, size_t, BufLen);
    UT_GenStub_AddParam(BPLib_PI_Egress, uint32_t, Timeout);

    UT_GenStub_Execute(BPLib_PI_Egress, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_PI_Egress, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_PI_Ingress()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_PI_Ingress(BPLib_Instance_t *Inst, uint8_t ChanId, void *AduPtr, size_t AduSize)
{
    UT_GenStub_SetupReturnBuffer(BPLib_PI_Ingress, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_PI_Ingress, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_PI_Ingress, uint8_t, ChanId);
    UT_GenStub_AddParam(BPLib_PI_Ingress, void *, AduPtr);
    UT_GenStub_AddParam(BPLib_PI_Ingress, size_t, AduSize);

    UT_GenStub_Execute(BPLib_PI_Ingress, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_PI_Ingress, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_PI_RemoveApplication()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_PI_RemoveApplication(BPLib_Instance_t *Inst, uint32_t ChanId)
{
    UT_GenStub_SetupReturnBuffer(BPLib_PI_RemoveApplication, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_PI_RemoveApplication, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_PI_RemoveApplication, uint32_t, ChanId);

    UT_GenStub_Execute(BPLib_PI_RemoveApplication, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_PI_RemoveApplication, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_PI_StartApplication()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_PI_StartApplication(uint32_t ChanId)
{
    UT_GenStub_SetupReturnBuffer(BPLib_PI_StartApplication, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_PI_StartApplication, uint32_t, ChanId);

    UT_GenStub_Execute(BPLib_PI_StartApplication, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_PI_StartApplication, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_PI_StopApplication()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_PI_StopApplication(uint32_t ChanId)
{
    UT_GenStub_SetupReturnBuffer(BPLib_PI_StopApplication, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_PI_StopApplication, uint32_t, ChanId);

    UT_GenStub_Execute(BPLib_PI_StopApplication, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_PI_StopApplication, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_PI_ValidateConfigs()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_PI_ValidateConfigs(void *TblData)
{
    UT_GenStub_SetupReturnBuffer(BPLib_PI_ValidateConfigs, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_PI_ValidateConfigs, void *, TblData);

    UT_GenStub_Execute(BPLib_PI_ValidateConfigs, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_PI_ValidateConfigs, BPLib_Status_t);
}
