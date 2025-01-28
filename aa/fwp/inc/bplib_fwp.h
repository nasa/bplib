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

#ifndef BPLIB_FWP_H
#define BPLIB_FWP_H

/*
** Include
*/

#include "bplib_api_types.h"
#include "bplib_as.h"
#include "bplib_nc_payloads.h"
#include "bplib_stor.h"
#include "bplib_time.h"
#include "bplib_em.h"

/*
** Type Definitions
*/

typedef struct 
{
    /* Time Proxy function callbacks */
    int64_t (*BPA_TIMEP_GetMonotonicTime)(void);
    void (*BPA_TIMEP_GetHostEpoch)(BPLib_TIME_Epoch_t *Epoch);
    BPLib_TIME_ClockState_t (*BPA_TIMEP_GetHostClockState)(void);
    int64_t (*BPA_TIMEP_GetHostTime)(void);
    
    /* PerfLog Proxy function callbacks */
    void (*BPA_PERFLOGP_Entry)(uint32_t PerfLogID);
    void (*BPA_PERFLOGP_Exit)(uint32_t PerfLogID);

    /* Table Proxy function callbacks */
    BPLib_Status_t (*BPA_TABLEP_SingleTableUpdate)(int16_t TblHandle);

    /* Event Proxy function callbacks */
    BPLib_Status_t (*BPA_EVP_Init)(void);
    BPLib_Status_t (*BPA_EVP_SendEvent)(uint16_t EventID, BPLib_EM_EventType_t EventType, char const* EventText);
    
    /* Telemetry Proxy function callbacks */
    BPLib_Status_t (*BPA_TLMP_SendNodeMibConfigPkt)(BPLib_NodeMibConfigHkTlm_Payload_t* NodeMIBConfigTlmPayload);
    BPLib_Status_t (*BPA_TLMP_SendPerSourceMibConfigPkt)(BPLib_SourceMibConfigHkTlm_Payload_t* SrcMIBConfigTlmPayload);
    BPLib_Status_t (*BPA_TLMP_SendNodeMibCounterPkt)(BPLib_NodeMibCountersHkTlm_Payload_t* NodeMIBCounterTlmPayload);
    BPLib_Status_t (*BPA_TLMP_SendPerSourceMibCounterPkt)(BPLib_SourceMibCountersHkTlm_Payload_t* SrcMIBCounterTlmPayload);
    BPLib_Status_t (*BPA_TLMP_SendChannelContactPkt)(BPLib_ChannelContactStatHkTlm_Payload_t* ChannelContactTlmPayload);
    BPLib_Status_t (*BPA_TLMP_SendStoragePkt)(BPLib_StorageHkTlm_Payload_t* StorTlmPayload);

    /* ADU Proxy function callbacks */
    BPLib_Status_t (*BPA_ADUP_AddApplication)(uint8_t ChanId);
    BPLib_Status_t (*BPA_ADUP_StartApplication)(uint8_t ChanId);
    BPLib_Status_t (*BPA_ADUP_StopApplication)(uint8_t ChanId);
    BPLib_Status_t (*BPA_ADUP_RemoveApplication)(uint8_t ChanId);

    /* Add other proxies' function callbacks here: TODO */

} BPLib_FWP_ProxyCallbacks_t;

/*
** Global Data
*/

extern BPLib_FWP_ProxyCallbacks_t BPLib_FWP_ProxyCallbacks;

/*
** Exported Functions
*/

/**
 * \brief Framework Proxy initialization
 *
 *  \par Description
 *       FWP initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
BPLib_Status_t BPLib_FWP_Init(BPLib_FWP_ProxyCallbacks_t Callbacks);

#endif /* BPLIB_FWP_H */