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

/* ======== */
/* Includes */
/* ======== */
#include "bplib_em.h"
#include "bplib_fwp.h"

/* ================== */
/* Exported functions */
/* ================== */
BPL_Status_t BPLib_EM_Init(void)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    return Status;
}

// char const * BPL_EM_EventTypeToString(BPL_EM_EventType_t Type)
// {
//     /* BPL_EM_EventTypeStrings should always match BPL_EM_EventType_t. */
//     static char const * BPL_EM_EventTypeStrings[] = {
//         "UNKNOWN",
//         "DEBUG",
//         "INFO",
//         "WARNING",
//         "ERROR",
//         "CRITICAL"
//     };

//     switch (Type)
//     {
//         case BPL_EM_EventType_DEBUG:
//             return BPL_EM_EventTypeStrings[1];
//         case BPL_EM_EventType_INFO:
//             return BPL_EM_EventTypeStrings[2];
//         case BPL_EM_EventType_WARNING:
//             return BPL_EM_EventTypeStrings[3];
//         case BPL_EM_EventType_ERROR:
//             return BPL_EM_EventTypeStrings[4];
//         case BPL_EM_EventType_CRITICAL:
//             return BPL_EM_EventTypeStrings[5];
//         default:
//             /* This default case also captures the BPL_EM_EventType_UNKNOWN case. */
//             return BPL_EM_EventTypeStrings[0];
//     }
// }

BPL_Status_t BPA_EVP_SendEvent(uint16_t EventID, BPL_EM_EventType_t EventType,
    char const * EventText, ...)
{
    /* Check arguments */

    BPLib_FWP_ProxyCallbacks.BPA_EVP_SendEvent(/* insert args */);
    
    return ReturnStatus;
}

void BPL_EM_Deinitialize(void)
{
    /* Clear proxy function pointers */
    BPL_EM_ProxyCallbacks.Initialize_Impl = NULL;
    BPL_EM_ProxyCallbacks.SendEvent_Impl = NULL;

    return;
}
