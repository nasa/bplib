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

#ifndef BPLIB_FWP_H
#define BPLIB_FWP_H

/*
** Include
*/

#include "bplib_api_types.h"
#include "bplib_stor.h"
#include "bplib_nc.h"

/* ====== */
/* Macros */
/* ====== */

#define BPLIB_NUM_TBLS 11

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
    BPLib_Status_t (*BPA_TABLEP_TableUpdate)(uint8_t TableType, void** TblPtr);

    /* Event Proxy function callbacks */
    BPLib_Status_t (*BPA_EVP_Init)(void);
    BPLib_Status_t (*BPA_EVP_SendEvent)(uint16_t EventID, BPLib_EM_EventType_t EventType, char const* EventText);

    /* Telemetry Proxy function callbacks */
    BPLib_Status_t (*BPA_TLMP_SendNodeMibConfigPkt)(BPLib_NodeMibConfigHkTlm_Payload_t* NodeMIBConfigTlmPayload);
    BPLib_Status_t (*BPA_TLMP_SendPerSourceMibConfigPkt)(BPLib_SourceMibConfigHkTlm_Payload_t* SrcMIBConfigTlmPayload);
    BPLib_Status_t (*BPA_TLMP_SendNodeMibCounterPkt)(BPLib_NodeMibCountersHkTlm_Payload_t* NodeMIBCounterTlmPayload);
    BPLib_Status_t (*BPA_TLMP_SendPerSourceMibCounterPkt)(BPLib_SourceMibCountersHkTlm_Payload_t* SrcMIBCounterTlmPayload);
    BPLib_Status_t (*BPA_TLMP_SendNodeMibReportsPkt)(BPLib_NodeMibReportsHkTlm_Payload_t* NodeMibReportsPayload);
    BPLib_Status_t (*BPA_TLMP_SendChannelContactPkt)(BPLib_ChannelContactStatHkTlm_Payload_t* ChannelContactTlmPayload);
    BPLib_Status_t (*BPA_TLMP_SendStoragePkt)(BPLib_StorageHkTlm_Payload_t* StorTlmPayload);

    /* ADU Proxy function callbacks */
    BPLib_Status_t (*BPA_ADUP_AddApplication)(uint32_t ChanId);
    BPLib_Status_t (*BPA_ADUP_StartApplication)(uint32_t ChanId);
    BPLib_Status_t (*BPA_ADUP_StopApplication)(uint32_t ChanId);
    BPLib_Status_t (*BPA_ADUP_RemoveApplication)(uint32_t ChanId);

    /* CLA Proxy function callbacks */
    BPLib_Status_t (*BPA_CLAP_ContactSetup)(uint32_t ContactId, BPLib_CLA_ContactsSet_t ContactInfo);
    BPLib_Status_t (*BPA_CLAP_ContactStart)(uint32_t ContactId);
    BPLib_Status_t (*BPA_CLAP_ContactStop)(uint32_t ContactId);
    void           (*BPA_CLAP_ContactTeardown)(uint32_t ContactId);

    /* Add other proxies' function callbacks here: TODO */

} BPLib_FWP_ProxyCallbacks_t;

/* =========== */
/* Global Data */
/* =========== */

extern BPLib_FWP_ProxyCallbacks_t BPLib_FWP_ProxyCallbacks;

/* =================== */
/* Function Prototypes */
/* =================== */

/**
  * \brief     Framework Proxy initialization
  * \param[in] Callbacks (BPLib_FWP_ProxyCallbacks_t*) Pointer to callback functions for BPLib populated by BPNode
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Initialization was successful
  * \retval    BPLIB_FWP_CALLBACK_INIT_ERROR: At least on passed in callback function is NULL
  */
BPLib_Status_t BPLib_FWP_Init(BPLib_FWP_ProxyCallbacks_t* Callbacks);

#endif /* BPLIB_FWP_H */