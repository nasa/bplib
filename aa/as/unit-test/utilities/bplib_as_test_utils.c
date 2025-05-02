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

#include "bplib_as_test_utils.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

void Test_BPLib_AS_SetNodeCounterValues(uint32 SetValue)
{
    uint8_t NodeCounterLoop;

    for(NodeCounterLoop = 0; NodeCounterLoop < BPLIB_AS_NUM_NODE_CNTRS; NodeCounterLoop++)
    {
        BPLib_AS_NodeCountersPayload.NodeCounters[NodeCounterLoop] = SetValue;
    }
}

void Test_BPLib_AS_NodeCountersValueTest(uint32 ActualValue)
{
    uint8_t NodeCounterLoop;

    for(NodeCounterLoop = 0; NodeCounterLoop < BPLIB_AS_NUM_NODE_CNTRS; NodeCounterLoop++)
    {
        UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[NodeCounterLoop]);
    }
}

void Test_BPLib_AS_SetSourceCounterValues(int32_t SourceEid, uint32_t SetValue)
{
    uint8_t SourceCounterLoop;

    for(SourceCounterLoop = 0; SourceCounterLoop < BPLIB_AS_NUM_SOURCE_CNTRS; SourceCounterLoop++)
    {
        BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[SourceCounterLoop] = SetValue;
    }
}

void Test_BPLib_AS_SourceCountersValueTest(int32_t SourceEid, uint32_t ActualValue)
{
    uint8_t SourceCounterLoop;

    for(SourceCounterLoop = 0; SourceCounterLoop < BPLIB_AS_NUM_SOURCE_CNTRS; SourceCounterLoop++)
    {
        UtAssert_EQ(uint32_t, ActualValue,
                    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[SourceCounterLoop]);
    }
}

void Test_BPLib_AS_SetBundleCounterValues(uint32_t SetValue)
{
    /* Set the bundle counters that are node + source */
    BPLib_AS_NodeCountersPayload.NodeCounters[ADU_COUNT_DELIVERED]                    = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[ADU_COUNT_RECEIVED]                     = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_ABANDONED]                 = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_RE_FORWARDED]      = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REJECTED]          = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REQUEST]           = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]       = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]                   = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_EXPIRED]           = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_HOP_EXCEEDED]      = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_INVALID_PAYLOAD]   = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_NO_STORAGE]        = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_TOO_LONG]          = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_TRAFFIC_PARED]     = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNAUTHORIZED]      = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNINTELLIGIBLE]    = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK] = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELIVERED]                 = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DEPLETED]                  = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DISCARDED]                 = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FORWARDED]                 = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FORWARDED_FAILED]          = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENT_ERROR]            = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENTED]                = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_ACCEPTED]        = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CUSTODY_SIGNAL]  = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_FRAGMENT]        = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_REJECTED]        = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_MAX_BSR_RATE_EXCEEDED]     = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_NO_FURTHER_INFO]           = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REASSEMBLED]               = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED]                  = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_ACCEPTED]     = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELETED]      = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELIVERED]    = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_FORWARDED]    = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_RECEIVED]     = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_ACCEPTED]     = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_DELETED]      = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_DELIVERED]    = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_FORWARDED]    = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_RECEIVED]     = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CUSTODY_SIGNAL]   = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_FRAGMENT]         = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REDUNDANT]                 = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REJECTED_CUSTODY]          = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNINTELLIGIBLE_BLOCK]      = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNINTELLIGIBLE_EID]        = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNPROCESSED_BLOCKS]        = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_SIGNAL_RECEIVED]   = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_ACCEPTED]    = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_DELETED]     = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_DELIVERED]   = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_FORWARDED]   = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_RECEIVED]    = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS]             = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_ACCEPTED]    = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_DELETED]     = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_DELIVERED]   = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_FORWARDED]   = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_RECEIVED]    = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CUSTODY]         = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_INVALID_PRIMARY_BLOCK]     = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_MAX_BSR_RATE_EXCEEDED]     = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_MAX_CRS_RATE_EXCEEDED]     = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_NO_FURTHER_INFO]           = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REASSEMBLED]               = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED]                  = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_ACCEPTED]     = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELETED]      = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELIVERED]    = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_FORWARDED]    = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_RECEIVED]     = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS]              = SetValue;
}

void Test_BPLib_AS_BundleCountersValueTest(uint32_t ActualValue)
{
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[ADU_COUNT_DELIVERED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[ADU_COUNT_RECEIVED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_ABANDONED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_RE_FORWARDED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REJECTED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REQUEST]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_EXPIRED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_HOP_EXCEEDED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_INVALID_PAYLOAD]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_NO_STORAGE]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_TOO_LONG]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_TRAFFIC_PARED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNAUTHORIZED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNINTELLIGIBLE]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELIVERED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DEPLETED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DISCARDED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FORWARDED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FORWARDED_FAILED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENT_ERROR]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENTED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_ACCEPTED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CUSTODY_SIGNAL]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_FRAGMENT]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_REJECTED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_MAX_BSR_RATE_EXCEEDED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_NO_FURTHER_INFO]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REASSEMBLED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_ACCEPTED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELETED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELIVERED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_FORWARDED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_RECEIVED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_ACCEPTED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_DELETED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_DELIVERED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_FORWARDED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_RECEIVED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CUSTODY_SIGNAL]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_FRAGMENT]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REDUNDANT]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REJECTED_CUSTODY]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNINTELLIGIBLE_BLOCK]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNINTELLIGIBLE_EID]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNPROCESSED_BLOCKS]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_SIGNAL_RECEIVED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_ACCEPTED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_DELETED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_DELIVERED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_FORWARDED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_RECEIVED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_ACCEPTED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_DELETED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_DELIVERED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_FORWARDED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_RECEIVED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CUSTODY]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_INVALID_PRIMARY_BLOCK]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_MAX_BSR_RATE_EXCEEDED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_MAX_CRS_RATE_EXCEEDED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_NO_FURTHER_INFO]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REASSEMBLED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_ACCEPTED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELETED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELIVERED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_FORWARDED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_RECEIVED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS]);
}

void Test_BPLib_AS_SetErrorCounterValues(int32_t SourceEid, uint32_t SetValue)
{
    /* Set the error counters associated with nodes and sources */
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_ABANDONED]                                         = SetValue;
    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_ABANDONED]                 = SetValue;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REJECTED]                                  = SetValue;
    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_REJECTED]          = SetValue;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_BAD_EID]                                   = SetValue;
    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_BAD_EID]           = SetValue;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_INVALID_PAYLOAD]                           = SetValue;
    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_INVALID_PAYLOAD]   = SetValue;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_NO_STORAGE]                                = SetValue;
    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_NO_STORAGE]        = SetValue;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_TOO_LONG]                                  = SetValue;
    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_TOO_LONG]          = SetValue;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNAUTHORIZED]                              = SetValue;
    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_UNAUTHORIZED]      = SetValue;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNINTELLIGIBLE]                            = SetValue;
    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_UNINTELLIGIBLE]    = SetValue;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK]                         = SetValue;
    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK] = SetValue;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENT_ERROR]                                    = SetValue;
    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_FRAGMENT_ERROR]            = SetValue;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_REJECTED]                                = SetValue;
    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_GENERATED_REJECTED]        = SetValue;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REJECTED_CUSTODY]                                  = SetValue;
    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_REJECTED_CUSTODY]          = SetValue;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNPROCESSED_BLOCKS]                                = SetValue;
    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_UNPROCESSED_BLOCKS]        = SetValue;

    /* Set the error counters that are node-only */
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT]                          = SetValue;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_INVALID_PRIMARY_BLOCK]                             = SetValue;
}

void Test_BPLib_AS_ErrorCountersValueTest(int32_t SourceEid, uint32_t ActualValue)
{
    /* Evaluate the error counters associated with nodes and sources */
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_ABANDONED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_ABANDONED]);

    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REJECTED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_REJECTED]);

    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_BAD_EID]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_BAD_EID]);

    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_INVALID_PAYLOAD]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_INVALID_PAYLOAD]);

    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_NO_STORAGE]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_NO_STORAGE]);

    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_TOO_LONG]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_TOO_LONG]);

    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNAUTHORIZED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_UNAUTHORIZED]);

    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNINTELLIGIBLE]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_UNINTELLIGIBLE]);

    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK]);

    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENT_ERROR]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_FRAGMENT_ERROR]);

    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_REJECTED]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_GENERATED_REJECTED]);

    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REJECTED_CUSTODY]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_REJECTED_CUSTODY]);

    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNPROCESSED_BLOCKS]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_UNPROCESSED_BLOCKS]);

    /* Evaluate the error counters that are node-only */
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT]);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_INVALID_PRIMARY_BLOCK]);
}

void BPLib_AS_Test_Verify_Event(uint16_t EventNum, int32_t EventID, const char* EventText)
{
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, EventNum + 1);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[EventNum].EventID, EventID);
    UtAssert_STRINGBUF_EQ(EventText, BPLIB_EM_EXPANDED_EVENT_SIZE,
                            context_BPLib_EM_SendEvent[EventNum].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
}

void BPLib_AS_Test_Setup(void)
{
    /* Initialize the node counter payload to 0s */
    memset((void*) &BPLib_AS_NodeCountersPayload, 0, sizeof(BPLib_AS_NodeCountersPayload));

    /* Initialize the source counter payload to 0s */
    memset((void*) &BPLib_AS_SourceCountersPayload, 0, sizeof(BPLib_AS_SourceCountersPayload));

    /* Clear out event handler context */
    memset((void*) &context_BPLib_EM_SendEvent, 0, sizeof(BPLib_EM_SendEvent_context_t));

    /* Initialize test environment to default state for every test */
    UT_ResetState(0);

    UT_SetHandlerFunction(UT_KEY(BPLib_EM_SendEvent), UT_Handler_BPLib_EM_SendEvent, NULL);

    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibCounterPkt      = BPA_TLMP_SendNodeMibCounterPkt;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibCounterPkt = BPA_TLMP_SendPerSourceMibCounterPkt;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibReportsPkt      = BPA_TLMP_SendNodeMibReportsPkt;
}

void BPLib_AS_Test_Teardown(void)
{
    /* Clean up test environment */
}

void UtTest_Setup(void)
{
    TestBplibAs_Register();
    TestBplibAsInternal_Register();
}
