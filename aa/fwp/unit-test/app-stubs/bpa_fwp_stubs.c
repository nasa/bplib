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
 * Auto-Generated stub implementations for functions defined in bpa_fwp_stubs header
 */

#include "bpa_fwp_stubs.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_ADUP_AddApplication()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_ADUP_AddApplication(uint32_t ChanId)
{
    UT_GenStub_SetupReturnBuffer(BPA_ADUP_AddApplication, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_ADUP_AddApplication, uint8_t, ChanId);

    UT_GenStub_Execute(BPA_ADUP_AddApplication, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_ADUP_AddApplication, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_ADUP_RemoveApplication()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_ADUP_RemoveApplication(uint32_t ChanId)
{
    UT_GenStub_SetupReturnBuffer(BPA_ADUP_RemoveApplication, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_ADUP_RemoveApplication, uint8_t, ChanId);

    UT_GenStub_Execute(BPA_ADUP_RemoveApplication, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_ADUP_RemoveApplication, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_ADUP_StartApplication()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_ADUP_StartApplication(uint32_t ChanId)
{
    UT_GenStub_SetupReturnBuffer(BPA_ADUP_StartApplication, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_ADUP_StartApplication, uint8_t, ChanId);

    UT_GenStub_Execute(BPA_ADUP_StartApplication, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_ADUP_StartApplication, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_ADUP_StopApplication()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_ADUP_StopApplication(uint32_t ChanId)
{
    UT_GenStub_SetupReturnBuffer(BPA_ADUP_StopApplication, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_ADUP_StopApplication, uint8_t, ChanId);

    UT_GenStub_Execute(BPA_ADUP_StopApplication, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_ADUP_StopApplication, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_CLAP_ContactSetup()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_CLAP_ContactSetup(uint32_t ContactId, BPLib_CLA_ContactsSet_t ContactInfo)
{
    UT_GenStub_SetupReturnBuffer(BPA_CLAP_ContactSetup, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_CLAP_ContactSetup, uint32_t, ContactId);
    UT_GenStub_AddParam(BPA_CLAP_ContactSetup, BPLib_CLA_ContactsSet_t, ContactInfo);

    UT_GenStub_Execute(BPA_CLAP_ContactSetup, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_CLAP_ContactSetup, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_CLAP_ContactStart()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_CLAP_ContactStart(uint32_t ContactId)
{
    UT_GenStub_SetupReturnBuffer(BPA_CLAP_ContactStart, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_CLAP_ContactStart, uint32_t, ContactId);

    UT_GenStub_Execute(BPA_CLAP_ContactStart, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_CLAP_ContactStart, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_CLAP_ContactStop()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_CLAP_ContactStop(uint32_t ContactId)
{
    UT_GenStub_SetupReturnBuffer(BPA_CLAP_ContactStop, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_CLAP_ContactStop, uint32_t, ContactId);

    UT_GenStub_Execute(BPA_CLAP_ContactStop, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_CLAP_ContactStop, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_CLAP_ContactTeardown()
 * ----------------------------------------------------
 */
void BPA_CLAP_ContactTeardown(uint32_t ContactId)
{
    UT_GenStub_AddParam(BPA_CLAP_ContactTeardown, uint32_t, ContactId);

    UT_GenStub_Execute(BPA_CLAP_ContactTeardown, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_EVP_Init()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_EVP_Init(void)
{
    UT_GenStub_SetupReturnBuffer(BPA_EVP_Init, BPLib_Status_t);

    UT_GenStub_Execute(BPA_EVP_Init, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_EVP_Init, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_EVP_SendEvent()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_EVP_SendEvent(uint16_t EventID, BPLib_EM_EventType_t EventType, char const *EventText)
{
    UT_GenStub_SetupReturnBuffer(BPA_EVP_SendEvent, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_EVP_SendEvent, uint16_t, EventID);
    UT_GenStub_AddParam(BPA_EVP_SendEvent, BPLib_EM_EventType_t, EventType);
    UT_GenStub_AddParam(BPA_EVP_SendEvent, char const *, EventText);

    UT_GenStub_Execute(BPA_EVP_SendEvent, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_EVP_SendEvent, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_PERFLOGP_Entry()
 * ----------------------------------------------------
 */
void BPA_PERFLOGP_Entry(uint32_t PerfLogID)
{
    UT_GenStub_AddParam(BPA_PERFLOGP_Entry, uint32_t, PerfLogID);

    UT_GenStub_Execute(BPA_PERFLOGP_Entry, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_PERFLOGP_Exit()
 * ----------------------------------------------------
 */
void BPA_PERFLOGP_Exit(uint32_t PerfLogID)
{
    UT_GenStub_AddParam(BPA_PERFLOGP_Exit, uint32_t, PerfLogID);

    UT_GenStub_Execute(BPA_PERFLOGP_Exit, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_TABLEP_TableUpdate()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_TABLEP_TableUpdate(uint8_t TableType, void **TblPtr)
{
    UT_GenStub_SetupReturnBuffer(BPA_TABLEP_TableUpdate, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_TABLEP_TableUpdate, uint8_t, TableType);
    UT_GenStub_AddParam(BPA_TABLEP_TableUpdate, void **, TblPtr);

    UT_GenStub_Execute(BPA_TABLEP_TableUpdate, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_TABLEP_TableUpdate, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_TIMEP_GetHostClockState()
 * ----------------------------------------------------
 */
BPLib_TIME_ClockState_t BPA_TIMEP_GetHostClockState(void)
{
    UT_GenStub_SetupReturnBuffer(BPA_TIMEP_GetHostClockState, BPLib_TIME_ClockState_t);

    UT_GenStub_Execute(BPA_TIMEP_GetHostClockState, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_TIMEP_GetHostClockState, BPLib_TIME_ClockState_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_TIMEP_GetHostEpoch()
 * ----------------------------------------------------
 */
void BPA_TIMEP_GetHostEpoch(BPLib_TIME_Epoch_t *Epoch)
{
    UT_GenStub_AddParam(BPA_TIMEP_GetHostEpoch, BPLib_TIME_Epoch_t *, Epoch);

    UT_GenStub_Execute(BPA_TIMEP_GetHostEpoch, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_TIMEP_GetHostTime()
 * ----------------------------------------------------
 */
int64_t BPA_TIMEP_GetHostTime(void)
{
    UT_GenStub_SetupReturnBuffer(BPA_TIMEP_GetHostTime, int64_t);

    UT_GenStub_Execute(BPA_TIMEP_GetHostTime, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_TIMEP_GetHostTime, int64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_TIMEP_GetMonotonicTime()
 * ----------------------------------------------------
 */
int64_t BPA_TIMEP_GetMonotonicTime(void)
{
    UT_GenStub_SetupReturnBuffer(BPA_TIMEP_GetMonotonicTime, int64_t);

    UT_GenStub_Execute(BPA_TIMEP_GetMonotonicTime, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_TIMEP_GetMonotonicTime, int64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_TLMP_SendChannelContactPkt()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_TLMP_SendChannelContactPkt(BPLib_ChannelContactStatHkTlm_Payload_t *ChannelContactTlmPayload)
{
    UT_GenStub_SetupReturnBuffer(BPA_TLMP_SendChannelContactPkt, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_TLMP_SendChannelContactPkt, BPLib_ChannelContactStatHkTlm_Payload_t *,
                        ChannelContactTlmPayload);

    UT_GenStub_Execute(BPA_TLMP_SendChannelContactPkt, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_TLMP_SendChannelContactPkt, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_TLMP_SendNodeMibConfigPkt()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_TLMP_SendNodeMibConfigPkt(BPLib_NodeMibConfigHkTlm_Payload_t *NodeMIBConfigTlmPayload)
{
    UT_GenStub_SetupReturnBuffer(BPA_TLMP_SendNodeMibConfigPkt, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_TLMP_SendNodeMibConfigPkt, BPLib_NodeMibConfigHkTlm_Payload_t *, NodeMIBConfigTlmPayload);

    UT_GenStub_Execute(BPA_TLMP_SendNodeMibConfigPkt, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_TLMP_SendNodeMibConfigPkt, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_TLMP_SendNodeMibCounterPkt()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_TLMP_SendNodeMibCounterPkt(BPLib_NodeMibCountersHkTlm_Payload_t *NodeMIBCounterTlmPayload)
{
    UT_GenStub_SetupReturnBuffer(BPA_TLMP_SendNodeMibCounterPkt, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_TLMP_SendNodeMibCounterPkt, BPLib_NodeMibCountersHkTlm_Payload_t *,
                        NodeMIBCounterTlmPayload);

    UT_GenStub_Execute(BPA_TLMP_SendNodeMibCounterPkt, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_TLMP_SendNodeMibCounterPkt, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_TLMP_SendNodeMibReportsPkt()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_TLMP_SendNodeMibReportsPkt(BPLib_NodeMibReportsHkTlm_Payload_t *NodeMibReportsHkTlm_Payload)
{
    UT_GenStub_SetupReturnBuffer(BPA_TLMP_SendNodeMibReportsPkt, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_TLMP_SendNodeMibReportsPkt, BPLib_NodeMibReportsHkTlm_Payload_t *,
                        NodeMibReportsHkTlm_Payload);

    UT_GenStub_Execute(BPA_TLMP_SendNodeMibReportsPkt, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_TLMP_SendNodeMibReportsPkt, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_TLMP_SendPerSourceMibConfigPkt()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_TLMP_SendPerSourceMibConfigPkt(BPLib_SourceMibConfigHkTlm_Payload_t *SrcMIBConfigTlmPayload)
{
    UT_GenStub_SetupReturnBuffer(BPA_TLMP_SendPerSourceMibConfigPkt, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_TLMP_SendPerSourceMibConfigPkt, BPLib_SourceMibConfigHkTlm_Payload_t *,
                        SrcMIBConfigTlmPayload);

    UT_GenStub_Execute(BPA_TLMP_SendPerSourceMibConfigPkt, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_TLMP_SendPerSourceMibConfigPkt, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_TLMP_SendPerSourceMibCounterPkt()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_TLMP_SendPerSourceMibCounterPkt(BPLib_SourceMibCountersHkTlm_Payload_t *SrcMIBCounterTlmPayload)
{
    UT_GenStub_SetupReturnBuffer(BPA_TLMP_SendPerSourceMibCounterPkt, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_TLMP_SendPerSourceMibCounterPkt, BPLib_SourceMibCountersHkTlm_Payload_t *,
                        SrcMIBCounterTlmPayload);

    UT_GenStub_Execute(BPA_TLMP_SendPerSourceMibCounterPkt, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_TLMP_SendPerSourceMibCounterPkt, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPA_TLMP_SendStoragePkt()
 * ----------------------------------------------------
 */
BPLib_Status_t BPA_TLMP_SendStoragePkt(BPLib_StorageHkTlm_Payload_t *StorTlmPayload)
{
    UT_GenStub_SetupReturnBuffer(BPA_TLMP_SendStoragePkt, BPLib_Status_t);

    UT_GenStub_AddParam(BPA_TLMP_SendStoragePkt, BPLib_StorageHkTlm_Payload_t *, StorTlmPayload);

    UT_GenStub_Execute(BPA_TLMP_SendStoragePkt, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPA_TLMP_SendStoragePkt, BPLib_Status_t);
}
