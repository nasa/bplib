/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in bplib_cla_internal header
 */

#include "bplib_cla_internal.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CLA_IsAControlMsg()
 * ----------------------------------------------------
 */
bool BPLib_CLA_IsAControlMsg(const void *MsgPtr, size_t Size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CLA_IsAControlMsg, bool);

    UT_GenStub_AddParam(BPLib_CLA_IsAControlMsg, const void *, MsgPtr);
    UT_GenStub_AddParam(BPLib_CLA_IsAControlMsg, size_t, Size);

    UT_GenStub_Execute(BPLib_CLA_IsAControlMsg, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CLA_IsAControlMsg, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CLA_ProcessControlMessage()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CLA_ProcessControlMessage(BPLib_CLA_CtrlMsg_t *CtrlMsgPtr)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CLA_ProcessControlMessage, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CLA_ProcessControlMessage, BPLib_CLA_CtrlMsg_t *, CtrlMsgPtr);

    UT_GenStub_Execute(BPLib_CLA_ProcessControlMessage, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CLA_ProcessControlMessage, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CLA_SetContactRunState()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CLA_SetContactRunState(uint32_t ContactId, BPLib_CLA_ContactRunState_t RunState)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CLA_SetContactRunState, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CLA_SetContactRunState, uint32_t, ContactId);
    UT_GenStub_AddParam(BPLib_CLA_SetContactRunState, BPLib_CLA_ContactRunState_t, RunState);

    UT_GenStub_Execute(BPLib_CLA_SetContactRunState, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CLA_SetContactRunState, BPLib_Status_t);
}
