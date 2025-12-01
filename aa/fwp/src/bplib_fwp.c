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

/* ======== */
/* Includes */
/* ======== */

#include "bplib_fwp.h"

/* =========== */
/* Global Data */
/* =========== */

BPLib_FWP_ProxyCallbacks_t BPLib_FWP_ProxyCallbacks;

/* ==================== */
/* Function Definitions */
/* ==================== */

/* Initialize proxy callbacks */
BPLib_Status_t BPLib_FWP_Init(BPLib_FWP_ProxyCallbacks_t* Callbacks)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;

    if (Callbacks->BPA_TIMEP_GetMonotonicTime            == NULL ||
        Callbacks->BPA_TIMEP_GetHostEpoch                == NULL ||
        Callbacks->BPA_TIMEP_GetHostClockState           == NULL ||
        Callbacks->BPA_TIMEP_GetHostTime                 == NULL ||
        Callbacks->BPA_EVP_Init                          == NULL ||
        Callbacks->BPA_EVP_SendEvent                     == NULL ||
        Callbacks->BPA_PERFLOGP_Entry                    == NULL ||
        Callbacks->BPA_PERFLOGP_Exit                     == NULL ||
        Callbacks->BPA_ADUP_AddApplication               == NULL ||
        Callbacks->BPA_ADUP_StartApplication             == NULL ||
        Callbacks->BPA_ADUP_StopApplication              == NULL ||
        Callbacks->BPA_ADUP_RemoveApplication            == NULL ||
        Callbacks->BPA_TLMP_SendNodeMibConfigPkt         == NULL ||
        Callbacks->BPA_TLMP_SendPerSourceMibConfigPkt    == NULL ||
        Callbacks->BPA_TLMP_SendNodeMibCounterPkt        == NULL ||
        Callbacks->BPA_TLMP_SendNodeMibReportsPkt        == NULL ||
        Callbacks->BPA_TLMP_SendPerSourceMibCounterPkt   == NULL ||
        Callbacks->BPA_TLMP_SendChannelContactPkt        == NULL ||
        Callbacks->BPA_TLMP_SendStoragePkt               == NULL ||
        Callbacks->BPA_TABLEP_TableUpdate                == NULL ||
        Callbacks->BPA_CLAP_ContactSetup                 == NULL ||
        Callbacks->BPA_CLAP_ContactStart                 == NULL ||
        Callbacks->BPA_CLAP_ContactStop                  == NULL ||
        Callbacks->BPA_CLAP_ContactTeardown              == NULL)
    {
        Status = BPLIB_FWP_CALLBACK_INIT_ERROR;
    }
    else
    {
        memcpy(&BPLib_FWP_ProxyCallbacks, Callbacks, sizeof(BPLib_FWP_ProxyCallbacks_t));
    }

    return Status;
}
