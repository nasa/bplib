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

#ifndef BPA_FWP_STUBS_H
#define BPA_FWP_STUBS_H

/*
** Include
*/

#include "bplib_time.h"
#include "bplib_em.h"
#include "bplib_nc_payloads.h"
#include "bplib_as.h"
#include "bplib_stor.h"


/*
** Function Definitions
*/

int64_t BPA_TIMEP_GetMonotonicTime(void);

void BPA_TIMEP_GetHostEpoch(BPLib_TIME_Epoch_t *Epoch);

BPLib_TIME_ClockState_t BPA_TIMEP_GetHostClockState(void);

int64_t BPA_TIMEP_GetHostTime(void);

BPLib_Status_t BPA_EVP_Init(void);

BPLib_Status_t BPA_EVP_SendEvent(uint16_t EventID, BPLib_EM_EventType_t EventType, char const* EventText);

int32_t BPA_TABLEP_SingleTableUpdate(int16_t TblHandle);

void BPA_PERFLOGP_Entry(uint32_t PerfLogID);

void BPA_PERFLOGP_Exit(uint32_t PerfLogID);

BPLib_Status_t BPA_ADUP_AddApplication(uint8_t ChanId);

BPLib_Status_t BPA_ADUP_StartApplication(uint8_t ChanId);

BPLib_Status_t BPA_ADUP_StopApplication(uint8_t ChanId);

BPLib_Status_t BPA_TLMP_SendNodeMibConfigPkt(BPLib_NodeMibConfigHkTlm_Payload_t* NodeMIBConfigTlmPayload);

BPLib_Status_t BPA_TLMP_SendPerSourceMibConfigPkt(BPLib_SourceMibConfigHkTlm_Payload_t* SrcMIBConfigTlmPayload);

BPLib_Status_t BPA_TLMP_SendNodeMibCounterPkt(BPLib_NodeMibCountersHkTlm_Payload_t* NodeMIBCounterTlmPayload);

BPLib_Status_t BPA_TLMP_SendPerSourceMibCounterPkt(BPLib_SourceMibCountersHkTlm_Payload_t* SrcMIBCounterTlmPayload);

BPLib_Status_t BPA_TLMP_SendChannelContactPkt(BPLib_ChannelContactStatHkTlm_Payload_t* ChannelContactTlmPayload);

BPLib_Status_t BPA_TLMP_SendStoragePkt(BPLib_StorageHkTlm_Payload_t* StorTlmPayload);

#endif /* BPA_FWP_STUBS_H */
