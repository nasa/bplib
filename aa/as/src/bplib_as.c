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
    BPLib_Status_t Status;
    uint32_t       CounterValue;

    /* Obtain the value of the counter */
    Status = BPLib_AS_Get(SourceEid, Counter, &CounterValue);

    if (Status == BPLIB_SUCCESS)
    {
        /* Increment the counter */
        CounterValue += Amount;

        /*
        ** The errors that could occur in BPLib_AS_Set() are the exact same as errors that are checked for in
        ** BPLib_AS_Get(). Since BPLib_AS_Get() is a prerequisite to getting to this call, the return status for
        ** this BPLib_AS_Set() call is ignored.
        */
        BPLib_AS_Set(SourceEid, Counter, CounterValue);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_AS_INCREMENT_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not get counter %d with source EID %d to increment by %d, RC = %d",
                            Counter,
                            SourceEid,
                            Amount,
                            Status);
    }
}

void BPLib_AS_Decrement(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Amount)
{
    BPLib_Status_t Status;
    uint32_t       CounterValue;

    /* Obtain the value of the node counter */
    Status = BPLib_AS_Get(SourceEid, Counter, &CounterValue);

    if (Status == BPLIB_SUCCESS)
    {
        /* Decrement the counter */
        CounterValue -= Amount;

        /*
        ** The errors that could occur in BPLib_AS_Set() are the exact same as errors that are checked for in
        ** BPLib_AS_Get(). Since BPLib_AS_Get() is a prerequisite to getting to this call, the return status for
        ** this BPLib_AS_Set() call is ignored.
        */
        BPLib_AS_Set(SourceEid, Counter, CounterValue);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_AS_DECREMENT_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not get counter %d with source EID %d to decrement by %d, RC = %d",
                            Counter,
                            SourceEid,
                            Amount,
                            Status);
    }
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
        /* Reset all node counters that overlap with source counters */
        BPLib_AS_NodeCountersPayload.AduCountDelivered                  = 0;
        BPLib_AS_NodeCountersPayload.AduCountReceived                   = 0;
        BPLib_AS_NodeCountersPayload.BundleCountAbandoned               = 0;
        BPLib_AS_NodeCountersPayload.BundleCountCustodyReForwarded      = 0;
        BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected         = 0;
        BPLib_AS_NodeCountersPayload.BundleCountCustodyRequest          = 0;
        BPLib_AS_NodeCountersPayload.BundleCountCustodyTransferred      = 0;
        BPLib_AS_NodeCountersPayload.BundleCountDeleted                 = 0;
        BPLib_AS_NodeCountersPayload.BundleCountDeletedBadEid           = 0;
        BPLib_AS_NodeCountersPayload.BundleCountDeletedExpired          = 0;
        BPLib_AS_NodeCountersPayload.BundleCountDeletedHopExceeded      = 0;
        BPLib_AS_NodeCountersPayload.BundleCountDeletedInvalidPayload   = 0;
        BPLib_AS_NodeCountersPayload.BundleCountDeletedNoStorage        = 0;
        BPLib_AS_NodeCountersPayload.BundleCountDeletedTrafficPared     = 0;
        BPLib_AS_NodeCountersPayload.BundleCountDeletedUnintelligible   = 0;
        BPLib_AS_NodeCountersPayload.BundleCountDeletedUnsupportedBlock = 0;
        BPLib_AS_NodeCountersPayload.BundleCountDeletedTooLong          = 0;
        BPLib_AS_NodeCountersPayload.BundleCountDelivered               = 0;
        BPLib_AS_NodeCountersPayload.BundleCountDepleted                = 0;
        BPLib_AS_NodeCountersPayload.BundleCountDiscarded               = 0;
        BPLib_AS_NodeCountersPayload.BundleCountForwarded               = 0;
        BPLib_AS_NodeCountersPayload.BundleCountForwardedFailed         = 0;
        BPLib_AS_NodeCountersPayload.BundleCountFragmentError           = 0;
        BPLib_AS_NodeCountersPayload.BundleCountFragmented              = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted       = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustodySignal  = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedFragment       = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedRejected       = 0;
        BPLib_AS_NodeCountersPayload.BundleCountMaxBsrRateExceeded      = 0;
        BPLib_AS_NodeCountersPayload.BundleCountNoFurtherInfo           = 0;
        BPLib_AS_NodeCountersPayload.BundleCountReassembled             = 0;
        BPLib_AS_NodeCountersPayload.BundleCountReceived                = 0;
        BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrAccepted     = 0;
        BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDeleted      = 0;
        BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDelivered    = 0;
        BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrForwarded    = 0;
        BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrReceived     = 0;
        BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsAccepted     = 0;
        BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDeleted      = 0;
        BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDelivered    = 0;
        BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsForwarded    = 0;
        BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsReceived     = 0;
        BPLib_AS_NodeCountersPayload.BundleCountReceivedCustodySignal   = 0;
        BPLib_AS_NodeCountersPayload.BundleCountReceivedFragment        = 0;
        BPLib_AS_NodeCountersPayload.BundleCountRedundant               = 0;
        BPLib_AS_NodeCountersPayload.BundleCountRejectedCustody         = 0;
        BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleBlock     = 0;
        BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleEid       = 0;
        BPLib_AS_NodeCountersPayload.BundleCountUnprocessedBlocks       = 0;

        /* Set all source counters to 0 */
        memset((void*) &BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid], 0,
                sizeof(BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid]));
    }

    return Status;
}

BPLib_Status_t BPLib_AS_ResetBundleCounters(int16_t SourceEid)
{
    BPLib_Status_t Status;

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
        /* Reset the bundle counters associated with nodes only */
        BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrAccepted  = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDeleted   = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDelivered = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrForwarded = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrReceived  = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrs          = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsAccepted  = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDeleted   = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDelivered = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsForwarded = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsReceived  = 0;
        BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustody      = 0;
        BPLib_AS_NodeCountersPayload.BundleCountInvalidPrimaryBlock   = 0;
        BPLib_AS_NodeCountersPayload.BundleCountMaxCrsRateExceeded    = 0;
        BPLib_AS_NodeCountersPayload.BundleCountReceivedCrs           = 0;

        /* Reset bundle counters associated with nodes AND sources */
        BPLib_AS_NodeCountersPayload.AduCountDelivered                                              = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountDelivered                  = 0;

        BPLib_AS_NodeCountersPayload.AduCountReceived                                               = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountReceived                   = 0;

        BPLib_AS_NodeCountersPayload.BundleCountAbandoned                                           = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountAbandoned               = 0;

        BPLib_AS_NodeCountersPayload.BundleCountCustodyReForwarded                                  = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyReForwarded      = 0;

        BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected                                     = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected         = 0;

        BPLib_AS_NodeCountersPayload.BundleCountCustodyRequest                                      = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRequest          = 0;

        BPLib_AS_NodeCountersPayload.BundleCountCustodyTransferred                                  = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyTransferred      = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeleted                                             = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeleted                 = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeletedExpired                                      = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedExpired          = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeletedHopExceeded                                  = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedHopExceeded      = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeletedInvalidPayload                               = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedInvalidPayload   = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeletedNoStorage                                    = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedNoStorage        = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeletedTooLong                                      = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTooLong          = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeletedTrafficPared                                 = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTrafficPared     = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeletedUnintelligible                               = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnintelligible   = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeletedUnsupportedBlock                             = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnsupportedBlock = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDelivered                                           = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDelivered               = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDepleted                                            = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDepleted                = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDiscarded                                           = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDiscarded               = 0;

        BPLib_AS_NodeCountersPayload.BundleCountForwarded                                           = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwarded               = 0;

        BPLib_AS_NodeCountersPayload.BundleCountForwardedFailed                                     = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwardedFailed         = 0;

        BPLib_AS_NodeCountersPayload.BundleCountFragmentError                                       = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmentError           = 0;

        BPLib_AS_NodeCountersPayload.BundleCountFragmented                                          = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmented              = 0;

        BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted                                   = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted       = 0;

        BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustodySignal                              = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedCustodySignal  = 0;

        BPLib_AS_NodeCountersPayload.BundleCountGeneratedFragment                                   = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedFragment       = 0;

        BPLib_AS_NodeCountersPayload.BundleCountGeneratedRejected                                   = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedRejected       = 0;

        BPLib_AS_NodeCountersPayload.BundleCountMaxBsrRateExceeded                                  = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountMaxBsrRateExceeded      = 0;

        BPLib_AS_NodeCountersPayload.BundleCountNoFurtherInfo                                       = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoFurtherInfo           = 0;

        BPLib_AS_NodeCountersPayload.BundleCountReassembled                                         = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReassembled             = 0;

        BPLib_AS_NodeCountersPayload.BundleCountReceived                                            = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceived                = 0;

        BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrAccepted                                 = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrAccepted     = 0;

        BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDeleted                                  = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDeleted      = 0;

        BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDelivered                                = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDelivered    = 0;

        BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrForwarded                                = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrForwarded    = 0;

        BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrReceived                                 = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrReceived     = 0;

        BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsAccepted                                 = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsAccepted     = 0;

        BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDeleted                                  = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDeleted      = 0;

        BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDelivered                                = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDelivered    = 0;

        BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsForwarded                                = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsForwarded    = 0;

        BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsReceived                                 = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsReceived     = 0;

        BPLib_AS_NodeCountersPayload.BundleCountReceivedCustodySignal                               = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCustodySignal   = 0;

        BPLib_AS_NodeCountersPayload.BundleCountReceivedFragment                                    = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedFragment        = 0;

        BPLib_AS_NodeCountersPayload.BundleCountRedundant                                           = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRedundant               = 0;

        BPLib_AS_NodeCountersPayload.BundleCountRejectedCustody                                     = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRejectedCustody         = 0;

        BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleBlock                                 = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleBlock     = 0;

        BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleEid                                   = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleEid       = 0;

        BPLib_AS_NodeCountersPayload.BundleCountUnprocessedBlocks                                   = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnprocessedBlocks       = 0;
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
        /* Reset node-only error counters */
        BPLib_AS_NodeCountersPayload.BundleAgentRejectedDirectiveCount = 0;
        BPLib_AS_NodeCountersPayload.BundleCountInvalidPrimaryBlock    = 0;

        /* Reset all node error counters with corresponding source error counters */
        BPLib_AS_NodeCountersPayload.BundleCountAbandoned                                           = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountAbandoned               = 0;

        BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected                                     = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected         = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeletedBadEid                                       = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedBadEid           = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeletedInvalidPayload                               = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedInvalidPayload   = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeletedNoStorage                                    = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedNoStorage        = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeletedTooLong                                      = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTooLong          = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeletedUnauthorized                                 = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnauthorized     = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeletedUnintelligible                               = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnintelligible   = 0;

        BPLib_AS_NodeCountersPayload.BundleCountDeletedUnsupportedBlock                             = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnsupportedBlock = 0;

        BPLib_AS_NodeCountersPayload.BundleCountFragmentError                                       = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmentError           = 0;

        BPLib_AS_NodeCountersPayload.BundleCountGeneratedRejected                                   = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedRejected       = 0;

        BPLib_AS_NodeCountersPayload.BundleCountRejectedCustody                                     = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRejectedCustody         = 0;

        BPLib_AS_NodeCountersPayload.BundleCountUnprocessedBlocks                                   = 0;
        BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnprocessedBlocks       = 0;
    }

    return Status;
}

void BPLib_AS_ResetAllCounters(void)
{
    int16_t SourceCtrl;

    memset((void*) &BPLib_AS_NodeCountersPayload, 0, sizeof(BPLib_AS_NodeCountersPayload));

    for (SourceCtrl = 0; SourceCtrl < BPLIB_MAX_NUM_SOURCE_EID; SourceCtrl++)
    {
        memset((void*) &BPLib_AS_SourceCountersPayload.SourceCounters[SourceCtrl], 0,
                sizeof(BPLib_AS_SourceCountersPayload.SourceCounters[SourceCtrl]));
    }
}