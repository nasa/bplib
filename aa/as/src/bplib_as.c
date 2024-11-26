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

/* ======= */
/* Globals */
/* ======= */

BPLib_NodeMibCountersHkTlm_Payload_t    BPLib_AS_NodeCountersPayload;
BPLib_SourceMibCountersHkTlm_Payload_t  BPLib_AS_SourceCountersPayload;
BPLib_ChannelContactStatHkTlm_Payload_t BPLib_AS_ChannelContactStatsPayload;

/* ==================== */
/* Function Definitions */
/* ==================== */

BPLib_Status_t BPLib_AS_Init(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    // Instantiate mutex to protect counter memory space

    /* Instantiate all payloads under the stewardship of AS */
    BPLib_AS_ResetAllCounters();
    memset((void*) &BPLib_AS_ChannelContactStatsPayload, 0, sizeof(BPLib_AS_ChannelContactStatsPayload));

    return Status;
}

void BPLib_AS_Increment(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Amount)
{
    BPLib_AS_NodeCountersPayload.NodeCounters[Counter] += Amount;

    /*
    if (Counter < BPLIB_AS_NUM_SOURCE_CNTRS)
    {
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[Counter] += Amount;
    }
    */
}

void BPLib_AS_Decrement(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Amount)
{
    BPLib_AS_NodeCountersPayload.NodeCounters[Counter] -= Amount;

    /*
    if (Counter < BPLIB_AS_NUM_SOURCE_CNTRS)
    {
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[Counter] -= Amount;
    }
    */
}

BPLib_Status_t BPLib_AS_ResetSourceCounters(int16_t SourceEid)
{
    BPLib_Status_t Status;

    /* Default to BPLIB_SUCCESS */
    Status = BPLIB_SUCCESS;

    if (!BPLib_AS_EidIsValid(SourceEid))
    { /* Invalid source EID */
        BPLib_EM_SendEvent(BPLIB_AS_RESET_SRC_INVAL_EID_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not reset source counters due to an invalid source EID (%d)",
                            SourceEid);

        Status = BPLIB_AS_INVALID_EID;
    }
    else
    { /* Valid source EID */

        memset((void*) &BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters, 
                0, sizeof(BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters));
    }

    return Status;
}

BPLib_Status_t BPLib_AS_ResetBundleCounters(int16_t SourceEid)
{
    BPLib_Status_t Status;
    int16_t        CounterLoop;

    /* Default to BPLIB_SUCCESS */
    Status = BPLIB_SUCCESS;

    if (!BPLib_AS_EidIsValid(SourceEid))
    { /* Invalid source EID*/
        BPLib_EM_SendEvent(BPLIB_AS_RESET_BNDL_INVAL_EID_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not reset bundle counters due to invalid source EID (%d)",
                            SourceEid);
        
        Status = BPLIB_AS_INVALID_EID;
    }
    else
    { /* Valid source EID */
        /* Reset the bundle counters associated with nodes and sources */
        BPLib_AS_NodeCountersPayload.NodeCounters[ADU_COUNT_DELIVERED]                                            = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[ADU_COUNT_DELIVERED]                    = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[ADU_COUNT_RECEIVED]                                             = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[ADU_COUNT_RECEIVED]                     = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_ABANDONED]                                         = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_ABANDONED]                 = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_RE_FORWARDED]                              = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_RE_FORWARDED]      = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REJECTED]                                  = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_REJECTED]          = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REQUEST]                                   = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_REQUEST]           = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]                               = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]       = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]                                           = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED]                   = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_EXPIRED]                                   = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_EXPIRED]           = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_HOP_EXCEEDED]                              = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_HOP_EXCEEDED]      = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_INVALID_PAYLOAD]                           = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_INVALID_PAYLOAD]   = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_NO_STORAGE]                                = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_NO_STORAGE]        = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_TOO_LONG]                                  = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_TOO_LONG]          = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_TRAFFIC_PARED]                             = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_TRAFFIC_PARED]     = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNAUTHORIZED]                              = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_UNAUTHORIZED]      = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNINTELLIGIBLE]                            = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_UNINTELLIGIBLE]    = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK]                         = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK] = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELIVERED]                                         = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELIVERED]                 = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DEPLETED]                                          = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DEPLETED]                  = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DISCARDED]                                         = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DISCARDED]                 = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FORWARDED]                                         = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_FORWARDED]                 = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FORWARDED_FAILED]                                  = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_FORWARDED_FAILED]          = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENT_ERROR]                                    = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_FRAGMENT_ERROR]            = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENTED]                                        = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_FRAGMENTED]                = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_ACCEPTED]                                = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_GENERATED_ACCEPTED]        = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CUSTODY_SIGNAL]                          = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_GENERATED_CUSTODY_SIGNAL]  = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_FRAGMENT]                                = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_GENERATED_FRAGMENT]        = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_REJECTED]                                = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_GENERATED_REJECTED]        = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_MAX_BSR_RATE_EXCEEDED]                             = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_MAX_BSR_RATE_EXCEEDED]     = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_NO_FURTHER_INFO]                                   = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_NO_FURTHER_INFO]           = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REASSEMBLED]                                       = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_REASSEMBLED]               = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED]                                          = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_RECEIVED]                  = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_ACCEPTED]                             = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_RECEIVED_BSR_ACCEPTED]     = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELETED]                              = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_RECEIVED_BSR_DELETED]      = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELIVERED]                            = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_RECEIVED_BSR_DELIVERED]    = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_FORWARDED]                            = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_RECEIVED_BSR_FORWARDED]    = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_RECEIVED]                             = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_RECEIVED_BSR_RECEIVED]     = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_ACCEPTED]                             = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_RECEIVED_CRS_ACCEPTED]     = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_DELETED]                              = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_RECEIVED_CRS_DELETED]      = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_DELIVERED]                            = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_RECEIVED_CRS_DELIVERED]    = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_FORWARDED]                            = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_RECEIVED_CRS_FORWARDED]    = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_RECEIVED]                             = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_RECEIVED_CRS_RECEIVED]     = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CUSTODY_SIGNAL]                           = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_RECEIVED_CUSTODY_SIGNAL]   = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_FRAGMENT]                                 = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_RECEIVED_FRAGMENT]         = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REDUNDANT]                                         = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_REDUNDANT]                 = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REJECTED_CUSTODY]                                  = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_REJECTED_CUSTODY]          = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNINTELLIGIBLE_BLOCK]                              = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_UNINTELLIGIBLE_BLOCK]      = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNINTELLIGIBLE_EID]                                = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_UNINTELLIGIBLE_EID]        = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNPROCESSED_BLOCKS]                                = 0;
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_UNPROCESSED_BLOCKS]        = 0;

        /* Reset the bundle counters that are node-only */
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_SIGNAL_RECEIVED]                           = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_ACCEPTED]                            = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_DELETED]                             = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_DELIVERED]                           = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_FORWARDED]                           = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_RECEIVED]                            = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS]                                     = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_ACCEPTED]                            = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_DELETED]                             = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_DELIVERED]                           = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_FORWARDED]                           = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_RECEIVED]                            = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CUSTODY]                                 = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_INVALID_PRIMARY_BLOCK]                             = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_MAX_BSR_RATE_EXCEEDED]                             = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_MAX_CRS_RATE_EXCEEDED]                             = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_NO_FURTHER_INFO]                                   = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REASSEMBLED]                                       = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED]                                          = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_ACCEPTED]                             = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELETED]                              = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELIVERED]                            = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_FORWARDED]                            = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_RECEIVED]                             = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS]                                      = 0;
    }

    return Status;
}

BPLib_Status_t BPLib_AS_ResetErrorCounters(int16_t SourceEid)
{
    BPLib_Status_t Status;

    /* Default to BPLIB_SUCCESS */
    Status = BPLIB_SUCCESS;

    if (!BPLib_AS_EidIsValid(SourceEid))
    { /* Invalid source EID */
        BPLib_EM_SendEvent(BPLIB_AS_RESET_ERR_INVAL_EID_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not reset error counters due to invalid source EID (%d)",
                            SourceEid);

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