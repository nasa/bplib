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

#include "bplib_as.h"
#include "bplib_as_internal.h"
#include "bplib_fwp.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

BPLib_Status_t BPLib_AS_Init(void)
{
    BPLib_Status_t Status;
    uint32 OS_Status;

    MutexId = 0;
    strcpy(MutexName, "AS_CounterMutex");

    /* Instantiate a mutex for AS counters */
    OS_Status = OS_MutSemCreate(MutexId, MutexName, 0);

    /* Translate mutex status into BPLib_Status_t */
    if (OS_Status == OS_SUCCESS)
    {
        Status = BPLIB_SUCCESS;
    }
    else
    {
        Status = BPLIB_AS_INIT_MUTEX_ERR;
    }

    /* Instantiate all payloads under the stewardship of AS */
    BPLib_AS_ResetAllCounters();
    memset((void*) &BPLib_AS_ChannelContactStatsPayload, 0, sizeof(BPLib_AS_ChannelContactStatsPayload));

    return Status;
}

void BPLib_AS_Increment(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Amount)
{
    uint32 OS_Status;
    BPLib_Status_t Status;

    /* Lock access to the counters before using a counter as a reference to increment said counter */
    OS_Status = OS_MutSemTake(MutexId);
    if (OS_Status != OS_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_AS_TAKE_MUTEX_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Failed to take from the counter mutex while incrementing counter %d, RC = %d",
                            Counter,
                            OS_Status);
    }
    else
    {
        /* Use BPLib_AS_SetCounter to evaluate EID and counter ranges */
        Status = BPLib_AS_SetCounter(SourceEid, Counter, BPLib_AS_NodeCountersPayload.NodeCounters[Counter] + Amount);
        if (Status != BPLIB_SUCCESS)
        {
            BPLib_EM_SendEvent(BPLIB_AS_SET_CTR_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Could not set counter %d with source EID %d to %d, RC = %d",
                                Counter,
                                SourceEid,
                                BPLib_AS_NodeCountersPayload.NodeCounters[Counter] + Amount,
                                Status);
        }

        /* Unlock the counters */
        OS_Status = OS_MutSemGive(MutexId);
        if (OS_Status != OS_SUCCESS)
        {
            BPLib_EM_SendEvent(BPLIB_AS_GIVE_MUTEX_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to give to the counter mutex while incrementing counter %d, RC = %d",
                                Counter,
                                OS_Status);
        }
    }
}

void BPLib_AS_Decrement(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Amount)
{
    uint32 OS_Status;
    BPLib_Status_t Status;

    /* Default to success so Status only needs to be changed when an error occurs */
    Status = BPLIB_SUCCESS;

    /* Lock access to the counters before using a counter as a reference to increment said counter */
    OS_Status = OS_MutSemTake(MutexId);
    if (OS_Status != OS_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_AS_TAKE_MUTEX_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Failed to take from the counter mutex while decrementing counter %d, RC = %d",
                            Counter,
                            OS_Status);
    }
    else
    {
        /* Use BPLib_AS_SetCounter to evaluate EID and counter ranges */
        Status = BPLib_AS_SetCounter(SourceEid, Counter, BPLib_AS_NodeCountersPayload.NodeCounters[Counter] - Amount);
        if (Status != BPLIB_SUCCESS)
        {
            BPLib_EM_SendEvent(BPLIB_AS_SET_CTR_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Could not set counter %d with source EID %d to %d, RC = %d",
                                Counter,
                                SourceEid,
                                BPLib_AS_NodeCountersPayload.NodeCounters[Counter] - Amount,
                                Status);
        }

        OS_Status = OS_MutSemGive(MutexId);
        if (OS_Status != OS_SUCCESS)
        {
            BPLib_EM_SendEvent(BPLIB_AS_GIVE_MUTEX_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to give to the counter mutex while decrementing counter %d, RC = %d",
                                Counter,
                                OS_Status);
        }
    }
}

BPLib_Status_t BPLib_AS_ResetCounter(int16_t SourceEid, BPLib_AS_Counter_t Counter)
{
    BPLib_Status_t Status;

    Status = BPLib_AS_SetCounter(SourceEid, Counter, 0);

    return Status;
}

BPLib_Status_t BPLib_AS_ResetSourceCounters(int16_t SourceEid)
{
    BPLib_Status_t Status;

    /* Default to BPLIB_SUCCESS */
    Status = BPLIB_SUCCESS;

    if (!BPLib_AS_EidIsValid(SourceEid))
    { /* Invalid source EID */
        Status = BPLIB_AS_INVALID_EID;
    }
    else
    { /* Valid source EID */

        memset((void*) &BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters, 
                0, sizeof(BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters));
    }

    return Status;
}

void BPLib_AS_ResetBundleCounters()
{
    /* Only reset bundle-related node counters */
    BPLib_AS_NodeCountersPayload.NodeCounters[ADU_COUNT_DELIVERED]                    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[ADU_COUNT_RECEIVED]                     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_ABANDONED]                 = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_RE_FORWARDED]      = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REJECTED]          = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REQUEST]           = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]       = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]                   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_EXPIRED]           = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_HOP_EXCEEDED]      = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_INVALID_PAYLOAD]   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_NO_STORAGE]        = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_TOO_LONG]          = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_TRAFFIC_PARED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNAUTHORIZED]      = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNINTELLIGIBLE]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK] = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELIVERED]                 = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DEPLETED]                  = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DISCARDED]                 = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FORWARDED]                 = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FORWARDED_FAILED]          = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENT_ERROR]            = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENTED]                = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_ACCEPTED]        = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CUSTODY_SIGNAL]  = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_FRAGMENT]        = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_REJECTED]        = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_MAX_BSR_RATE_EXCEEDED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_NO_FURTHER_INFO]           = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REASSEMBLED]               = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED]                  = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_ACCEPTED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELETED]      = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELIVERED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_FORWARDED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_RECEIVED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_ACCEPTED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_DELETED]      = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_DELIVERED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_FORWARDED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_RECEIVED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CUSTODY_SIGNAL]   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_FRAGMENT]         = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REDUNDANT]                 = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REJECTED_CUSTODY]          = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNINTELLIGIBLE_BLOCK]      = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNINTELLIGIBLE_EID]        = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNPROCESSED_BLOCKS]        = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_SIGNAL_RECEIVED]   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_ACCEPTED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_DELETED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_DELIVERED]   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_FORWARDED]   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_RECEIVED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS]             = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_ACCEPTED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_DELETED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_DELIVERED]   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_FORWARDED]   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_RECEIVED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CUSTODY]         = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_INVALID_PRIMARY_BLOCK]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_MAX_BSR_RATE_EXCEEDED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_MAX_CRS_RATE_EXCEEDED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_NO_FURTHER_INFO]           = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REASSEMBLED]               = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED]                  = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_ACCEPTED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELETED]      = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELIVERED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_FORWARDED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_RECEIVED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS]              = 0;
}

BPLib_Status_t BPLib_AS_ResetErrorCounters(int16_t SourceEid)
{
    BPLib_Status_t Status;

    /* Default to BPLIB_SUCCESS */
    Status = BPLIB_SUCCESS;

    if (!BPLib_AS_EidIsValid(SourceEid))
    { /* Invalid source EID */
        Status = BPLIB_AS_INVALID_EID;
    }
    else
    { /* Valid source EID */
        /* Reset the error counters associated with nodes and sources */
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_ABANDONED]                                         = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_ABANDONED]                 = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REJECTED]                                  = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_REJECTED]          = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_BAD_EID]                                   = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_BAD_EID]           = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_INVALID_PAYLOAD]                           = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_INVALID_PAYLOAD]   = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_NO_STORAGE]                                = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_NO_STORAGE]        = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_TOO_LONG]                                  = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_TOO_LONG]          = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNAUTHORIZED]                              = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_UNAUTHORIZED]      = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNINTELLIGIBLE]                            = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_UNINTELLIGIBLE]    = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK]                         = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK] = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENT_ERROR]                                    = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_FRAGMENT_ERROR]            = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_REJECTED]                                = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_GENERATED_REJECTED]        = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REJECTED_CUSTODY]                                  = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_REJECTED_CUSTODY]          = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNPROCESSED_BLOCKS]                                = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_UNPROCESSED_BLOCKS]        = 0;

        /* Reset the error counters that are node-only */
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT]                          = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_INVALID_PRIMARY_BLOCK]                             = 0;
    }

    return Status;
}

void BPLib_AS_ResetAllCounters(void)
{
    int16_t SourceCtrl;

    memset((void*) &BPLib_AS_NodeCountersPayload, 0, sizeof(BPLib_AS_NodeCountersPayload));

    for (SourceCtrl = 0; SourceCtrl < BPLIB_MAX_NUM_SOURCE_EID; SourceCtrl++)
    {
        memset((void*) &BPLib_AS_SourceCountersPayload.MibArray[SourceCtrl], 0,
                sizeof(BPLib_AS_SourceCountersPayload.MibArray[SourceCtrl]));
    }
}

BPLib_Status_t BPLib_AS_SendNodeMibCountersHk()
{
    BPLib_Status_t Status;
    uint32 OS_Status;

    OS_Status = OS_MutSemTake(MutexId);
    if (OS_Status != OS_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_AS_TAKE_MUTEX_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Failed to take from the counter mutex while sending node MIB counter HK, RC = %d",
                            OS_Status);
    }
    else
    {
        Status = BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibCounterPkt(&BPLib_AS_NodeCountersPayload);

        OS_Status = OS_MutSemGive(MutexId);
        if (OS_Status != OS_SUCCESS)
        {
            BPLib_EM_SendEvent(BPLIB_AS_GIVE_MUTEX_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to give to the counter mutex while sending node MIB counter HK, RC = %d",
                                OS_Status);
        }
    }

    return Status;
}

BPLib_Status_t BPLib_AS_SendSourceMibCountersHk()
{
    BPLib_Status_t Status;
    uint32 OS_Status;

    OS_Status = OS_MutSemTake(MutexId);
    if (OS_Status != OS_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_AS_TAKE_MUTEX_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Failed to take from the counter mutex while sending source MIB counter HK, RC = %d",
                            OS_Status);
    }
    else
    {
        Status = BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibCounterPkt(&BPLib_AS_SourceCountersPayload);

        OS_Status = OS_MutSemGive(MutexId);
        if (OS_Status != OS_SUCCESS)
        {
            BPLib_EM_SendEvent(BPLIB_AS_GIVE_MUTEX_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to give to the counter mutex while sending node MIB counter HK, RC = %d",
                                OS_Status);
        }
    }

    return Status;
}

BPLib_Status_t BPLib_AS_SendChannelContactStatHk()
{
    BPLib_Status_t Status;

    // Lock data

    Status = BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendChannelContactPkt(&BPLib_AS_ChannelContactStatsPayload);

    // Unlock data

    return Status;
}

void BPLib_AS_SetAppState(uint8_t ChanId, BPLib_AS_ApplicationState_t State)
{
    BPLib_AS_ChannelContactStatsPayload.ChannelStatus[ChanId].State = State;
}

BPLib_AS_ApplicationState_t BPLib_AS_GetAppState(uint8_t ChanId)
{
    return BPLib_AS_ChannelContactStatsPayload.ChannelStatus[ChanId].State;
}