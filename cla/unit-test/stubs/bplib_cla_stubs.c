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
 * Auto-Generated stub implementations for functions defined in bplib_cla header
 */

#include "bplib_cla.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CLA_ContactSetup()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CLA_ContactSetup(uint32_t ContactId)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CLA_ContactSetup, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CLA_ContactSetup, uint32_t, ContactId);

    UT_GenStub_Execute(BPLib_CLA_ContactSetup, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CLA_ContactSetup, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CLA_ContactStart()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CLA_ContactStart(uint32_t ContactId)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CLA_ContactStart, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CLA_ContactStart, uint32_t, ContactId);

    UT_GenStub_Execute(BPLib_CLA_ContactStart, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CLA_ContactStart, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CLA_ContactStop()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CLA_ContactStop(uint32_t ContactId)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CLA_ContactStop, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CLA_ContactStop, uint32_t, ContactId);

    UT_GenStub_Execute(BPLib_CLA_ContactStop, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CLA_ContactStop, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CLA_ContactTeardown()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CLA_ContactTeardown(BPLib_Instance_t *Inst, uint32_t ContactId)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CLA_ContactTeardown, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CLA_ContactTeardown, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_CLA_ContactTeardown, uint32_t, ContactId);

    UT_GenStub_Execute(BPLib_CLA_ContactTeardown, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CLA_ContactTeardown, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CLA_ContactsTblValidateFunc()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CLA_ContactsTblValidateFunc(void *TblData)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CLA_ContactsTblValidateFunc, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CLA_ContactsTblValidateFunc, void *, TblData);

    UT_GenStub_Execute(BPLib_CLA_ContactsTblValidateFunc, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CLA_ContactsTblValidateFunc, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CLA_Egress()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CLA_Egress(BPLib_Instance_t *Inst, uint32_t ContId, void *BundleOut, size_t *Size, size_t BufLen,
                                uint32_t Timeout)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CLA_Egress, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CLA_Egress, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_CLA_Egress, uint32_t, ContId);
    UT_GenStub_AddParam(BPLib_CLA_Egress, void *, BundleOut);
    UT_GenStub_AddParam(BPLib_CLA_Egress, size_t *, Size);
    UT_GenStub_AddParam(BPLib_CLA_Egress, size_t, BufLen);
    UT_GenStub_AddParam(BPLib_CLA_Egress, uint32_t, Timeout);

    UT_GenStub_Execute(BPLib_CLA_Egress, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CLA_Egress, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CLA_GetAutoEgress()
 * ----------------------------------------------------
 */
bool BPLib_CLA_GetAutoEgress(uint32_t ContactId)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CLA_GetAutoEgress, bool);

    UT_GenStub_AddParam(BPLib_CLA_GetAutoEgress, uint32_t, ContactId);

    UT_GenStub_Execute(BPLib_CLA_GetAutoEgress, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CLA_GetAutoEgress, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CLA_GetContactRunState()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CLA_GetContactRunState(uint32_t ContactId, BPLib_CLA_ContactRunState_t *ReturnState)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CLA_GetContactRunState, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CLA_GetContactRunState, uint32_t, ContactId);
    UT_GenStub_AddParam(BPLib_CLA_GetContactRunState, BPLib_CLA_ContactRunState_t *, ReturnState);

    UT_GenStub_Execute(BPLib_CLA_GetContactRunState, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CLA_GetContactRunState, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CLA_Ingress()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CLA_Ingress(BPLib_Instance_t *Inst, uint32_t ContId, const void *Bundle, size_t Size,
                                 uint32_t Timeout)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CLA_Ingress, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CLA_Ingress, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_CLA_Ingress, uint32_t, ContId);
    UT_GenStub_AddParam(BPLib_CLA_Ingress, const void *, Bundle);
    UT_GenStub_AddParam(BPLib_CLA_Ingress, size_t, Size);
    UT_GenStub_AddParam(BPLib_CLA_Ingress, uint32_t, Timeout);

    UT_GenStub_Execute(BPLib_CLA_Ingress, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CLA_Ingress, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CLA_SetAutoEgress()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CLA_SetAutoEgress(uint32_t ContactId, bool AutoEgressEnabled)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CLA_SetAutoEgress, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CLA_SetAutoEgress, uint32_t, ContactId);
    UT_GenStub_AddParam(BPLib_CLA_SetAutoEgress, bool, AutoEgressEnabled);

    UT_GenStub_Execute(BPLib_CLA_SetAutoEgress, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CLA_SetAutoEgress, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CLA_SetContactExited()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CLA_SetContactExited(uint32_t ContactId)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CLA_SetContactExited, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CLA_SetContactExited, uint32_t, ContactId);

    UT_GenStub_Execute(BPLib_CLA_SetContactExited, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CLA_SetContactExited, BPLib_Status_t);
}
