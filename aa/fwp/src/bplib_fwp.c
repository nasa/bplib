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

/*
** Include
*/

#include "bplib_fwp.h"


/*
** Global Data
*/

BPLib_FWP_ProxyCallbacks_t BPLib_FWP_ProxyCallbacks;
BPLib_FWP_ConfigPtrs_t BPLib_FWP_ConfigPtrs;

/*
** Function Definitions
*/

/* Initialize proxy callbacks */
BPLib_Status_t BPLib_FWP_Init(BPLib_FWP_ProxyCallbacks_t Callbacks)
{
    if (Callbacks.BPA_TIMEP_GetMonotonicTime            == NULL ||
        Callbacks.BPA_TIMEP_GetHostEpoch                == NULL ||
        Callbacks.BPA_TIMEP_GetHostClockState           == NULL ||
        Callbacks.BPA_TIMEP_GetHostTime                 == NULL ||
        Callbacks.BPA_TABLEP_SingleTableUpdate          == NULL ||
        Callbacks.BPA_EVP_Init                          == NULL ||
        Callbacks.BPA_EVP_SendEvent                     == NULL ||
        Callbacks.BPA_PERFLOGP_Entry                    == NULL ||
        Callbacks.BPA_PERFLOGP_Exit                     == NULL ||
        Callbacks.BPA_ADUP_AddApplication               == NULL ||
        Callbacks.BPA_ADUP_StartApplication             == NULL ||
        Callbacks.BPA_ADUP_StopApplication              == NULL ||
        Callbacks.BPA_ADUP_RemoveApplication            == NULL ||
        Callbacks.BPA_TLMP_SendNodeMibConfigPkt         == NULL ||
        Callbacks.BPA_TLMP_SendPerSourceMibConfigPkt    == NULL ||
        Callbacks.BPA_TLMP_SendNodeMibCounterPkt        == NULL ||
        Callbacks.BPA_TLMP_SendPerSourceMibCounterPkt   == NULL ||
        Callbacks.BPA_TLMP_SendChannelContactPkt        == NULL ||
        Callbacks.BPA_TLMP_SendStoragePkt               == NULL)
    {
        return BPLIB_FWP_CALLBACK_INIT_ERROR;
    }
    else
    {
        /* Initialize Time Proxy callbacks */
        BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime  = Callbacks.BPA_TIMEP_GetMonotonicTime;
        BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostEpoch      = Callbacks.BPA_TIMEP_GetHostEpoch;
        BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostClockState = Callbacks.BPA_TIMEP_GetHostClockState;
        BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostTime       = Callbacks.BPA_TIMEP_GetHostTime;

        /* Initialize PerfLog Proxy callbacks */
        BPLib_FWP_ProxyCallbacks.BPA_PERFLOGP_Entry = Callbacks.BPA_PERFLOGP_Entry;
        BPLib_FWP_ProxyCallbacks.BPA_PERFLOGP_Exit  = Callbacks.BPA_PERFLOGP_Exit;

        /* Initialize Table Proxy callbacks */
        BPLib_FWP_ProxyCallbacks.BPA_TABLEP_SingleTableUpdate = Callbacks.BPA_TABLEP_SingleTableUpdate;

        /* Initialize Event Proxy callbacks */
        BPLib_FWP_ProxyCallbacks.BPA_EVP_Init      = Callbacks.BPA_EVP_Init;
        BPLib_FWP_ProxyCallbacks.BPA_EVP_SendEvent = Callbacks.BPA_EVP_SendEvent;

        /* Initialize ADU Proxy callbacks */
        BPLib_FWP_ProxyCallbacks.BPA_ADUP_AddApplication    = Callbacks.BPA_ADUP_AddApplication;
        BPLib_FWP_ProxyCallbacks.BPA_ADUP_StartApplication  = Callbacks.BPA_ADUP_StartApplication;
        BPLib_FWP_ProxyCallbacks.BPA_ADUP_StopApplication   = Callbacks.BPA_ADUP_StopApplication;
        BPLib_FWP_ProxyCallbacks.BPA_ADUP_RemoveApplication = Callbacks.BPA_ADUP_RemoveApplication;

        /* Initialize Telemetry Proxy callbacks*/
        BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibConfigPkt       = Callbacks.BPA_TLMP_SendNodeMibConfigPkt;
        BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibConfigPkt  = Callbacks.BPA_TLMP_SendPerSourceMibConfigPkt;
        BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibCounterPkt      = Callbacks.BPA_TLMP_SendNodeMibCounterPkt;
        BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibCounterPkt = Callbacks.BPA_TLMP_SendPerSourceMibCounterPkt;
        BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendChannelContactPkt      = Callbacks.BPA_TLMP_SendChannelContactPkt;
        BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendStoragePkt             = Callbacks.BPA_TLMP_SendStoragePkt;

        /* Initialize other proxies' callbacks TODO */
    }

    /* Capture configuration pointers in the global configuration struct */
    /* TODO: Call BPLib_NC_TableUpdate() for each table */

    return BPLIB_SUCCESS;
}
