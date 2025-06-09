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
#include "bpcat_fwp.h"
#include "bplib.h"

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>

static int64_t BPA_TIMEP_GetMonotonicTime(void)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
    {
        perror("clock_gettime MONOTONIC");
        return 0;
    }
    return (int64_t)(ts.tv_sec) * 1000 + (int64_t)(ts.tv_nsec) / 1000000;
}

static int64_t BPA_TIMEP_GetHostTime(void)
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) == -1) {
        perror("gettimeofday");
        return 0;
    }

    return (int64_t)(tv.tv_sec) * 1000 + (int64_t)(tv.tv_usec) / 1000;
}

static void BPA_TIMEP_GetHostEpoch(BPLib_TIME_Epoch_t *Epoch)
{
    if (Epoch != NULL)
    {
        Epoch->Year   = 1970;
        Epoch->Day    = 1;
        Epoch->Hour   = 1;
        Epoch->Minute = 0;
        Epoch->Second = 0;
        Epoch->Msec   = 0;
    }
}

static BPLib_TIME_ClockState_t BPA_TIMEP_GetHostClockState(void)
{
    return BPLIB_TIME_CLOCK_VALID;
}

static void BPA_PERFLOGP_Entry(uint32_t PerfLogID)
{

}

static void BPA_PERFLOGP_Exit(uint32_t PerfLogID)
{

}

static BPLib_Status_t BPA_TABLEP_TableUpdate(uint8_t TableType, void** TblPtr)
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_EVP_Init()
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_EVP_SendEvent(uint16_t EventID, BPLib_EM_EventType_t EventType, char const* Spec)
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendNodeMibConfigPkt(BPLib_NodeMibConfigHkTlm_Payload_t* NodeMIBConfigTlmPayload)
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendPerSourceMibConfigPkt(BPLib_SourceMibConfigHkTlm_Payload_t* SrcMIBConfigTlmPayload)
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendNodeMibCounterPkt(BPLib_NodeMibCountersHkTlm_Payload_t* NodeMIBCounterTlmPayload)
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendPerSourceMibCounterPkt(BPLib_SourceMibCountersHkTlm_Payload_t* SrcMIBCounterTlmPayload)
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendChannelContactPkt(BPLib_ChannelContactStatHkTlm_Payload_t* ChannelContactTlmPayload)
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendStoragePkt(BPLib_StorageHkTlm_Payload_t* StorTlmPayload)
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_ADUP_AddApplication(uint32_t ChanId)
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_ADUP_StartApplication(uint32_t ChanId)
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_ADUP_StopApplication(uint32_t ChanId)
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_ADUP_RemoveApplication(uint32_t ChanId)
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_CLAP_ContactSetup(uint32_t ContactId, BPLib_CLA_ContactsSet_t ContactInfo)
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_CLAP_ContactStart(uint32_t ContactId)
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_CLAP_ContactStop(uint32_t ContactId)
{
    return BPLIB_SUCCESS;
}

static void BPA_CLAP_ContactTeardown(uint32_t ContactId)
{
    return;
}

static BPLib_FWP_ProxyCallbacks_t Callbacks = {
    /* Time Proxy */
    .BPA_TIMEP_GetMonotonicTime = BPA_TIMEP_GetMonotonicTime,
    .BPA_TIMEP_GetHostEpoch = BPA_TIMEP_GetHostEpoch,
    .BPA_TIMEP_GetHostClockState = BPA_TIMEP_GetHostClockState,
    .BPA_TIMEP_GetHostTime = BPA_TIMEP_GetHostTime,

    /* Perf Log Proxy */
    .BPA_PERFLOGP_Entry = BPA_PERFLOGP_Entry,
    .BPA_PERFLOGP_Exit = BPA_PERFLOGP_Exit,

    /* Table Proxy */
    .BPA_TABLEP_TableUpdate = BPA_TABLEP_TableUpdate,

    /* Event Proxy */
    .BPA_EVP_Init = BPA_EVP_Init,
    .BPA_EVP_SendEvent = BPA_EVP_SendEvent,

    /* ADU Proxy */
    .BPA_ADUP_AddApplication = BPA_ADUP_AddApplication,
    .BPA_ADUP_StartApplication = BPA_ADUP_StartApplication,
    .BPA_ADUP_StopApplication = BPA_ADUP_StopApplication,
    .BPA_ADUP_RemoveApplication = BPA_ADUP_RemoveApplication,

    /* Telemetry Proxy */
    .BPA_TLMP_SendNodeMibConfigPkt = BPA_TLMP_SendNodeMibConfigPkt,
    .BPA_TLMP_SendPerSourceMibConfigPkt = BPA_TLMP_SendPerSourceMibConfigPkt,
    .BPA_TLMP_SendNodeMibCounterPkt = BPA_TLMP_SendNodeMibCounterPkt,
    .BPA_TLMP_SendPerSourceMibCounterPkt = BPA_TLMP_SendPerSourceMibCounterPkt,
    .BPA_TLMP_SendChannelContactPkt = BPA_TLMP_SendChannelContactPkt,
    .BPA_TLMP_SendStoragePkt = BPA_TLMP_SendStoragePkt,

    /* CLA Proxy */
    .BPA_CLAP_ContactSetup = BPA_CLAP_ContactSetup,
    .BPA_CLAP_ContactStart = BPA_CLAP_ContactStart,
    .BPA_CLAP_ContactStop = BPA_CLAP_ContactStop,
    .BPA_CLAP_ContactTeardown = BPA_CLAP_ContactTeardown,
};

BPCat_Status_t BPCat_FWP_Init()
{
    BPLib_Status_t Status;
    Status = BPLib_FWP_Init(&Callbacks);
    if (Status != BPCAT_SUCCESS)
    {
        return BPCAT_FWP_ERR;
    }

    return BPCAT_SUCCESS;
}
