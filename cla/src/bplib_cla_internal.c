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
 * Internal function definitions for CLA
 */

/* ======== */
/* Includes */
/* ======== */

#include <string.h>
#include "bplib_cla.h"
#include "bplib_cla_internal.h"
#include "bplib_bi.h"

/* =========== */
/* Global Data */
/* =========== */

BPLib_CLA_ContactRunState_t BPLib_CLA_ContactRunStates[BPLIB_MAX_NUM_CONTACTS];

/* ==================== */
/* Function Definitions */
/* ==================== */

bool BPLib_CLA_IsAControlMsg(const void *MsgPtr)
{
    BPLib_CLA_CtrlMsg_t* InCtrlMsgPtr = (BPLib_CLA_CtrlMsg_t*) MsgPtr;
    char TagStr[] = "BPNMSG";
    if (strncmp(InCtrlMsgPtr->CtrlMsgTag, TagStr, strlen(TagStr)) == 0)
        return true;
    else
        return false;
}


/* BPLib_CLA_ProcessControlMessage*/
BPLib_Status_t BPLib_CLA_ProcessControlMessage(BPLib_CLA_CtrlMsg_t* CtrlMsgPtr)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    /* There are 4 types of control messages*/
    
    switch (CtrlMsgPtr->MsgTypes)
    {
        case SentIt:
            /* LTP Only, Add session ID and and Bundle ID to Sent It Map */
            break;
        case SessionComplete:
            BPLib_BI_RecvCtrlMsg(CtrlMsgPtr);
            break;
        case SessionCancelled:
        case SessionStarted:
        default:
            /* Do Nothing*/
            break;
    }
        
    return Status;
}

BPLib_Status_t BPLib_CLA_SetContactRunState(uint32_t ContactId, BPLib_CLA_ContactRunState_t RunState)
{
    BPLib_Status_t Status;

    if (ContactId < BPLIB_MAX_NUM_CONTACTS)
    {
        BPLib_CLA_ContactRunStates[ContactId] = RunState;
        Status = BPLIB_SUCCESS;
    }
    else
    {
        Status = BPLIB_CLA_INVALID_CONTACT_ID;
    }

    return Status;
}