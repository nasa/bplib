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

    // Instantiate mutex to protect counter memory space

    memset((void*) &BPLib_AS_NodeCountersPayload,        0, sizeof(BPLib_AS_NodeCountersPayload));
    memset((void*) &BPLib_AS_SourceCountersPayload,      0, sizeof(BPLib_AS_SourceCountersPayload));
    memset((void*) &BPLib_AS_ChannelContactStatsPayload, 0, sizeof(BPLib_AS_ChannelContactStatsPayload));

    BPLib_AS_ResetAllCounters();

    Status = BPLIB_SUCCESS;

    return Status;
}

bool BPLib_AS_EidIsValid(int16_t SourceEid)
{
    return (SourceEid >= 0 && SourceEid < BPLIB_MAX_NUM_SOURCE_EID);
}

BPLib_Status_t BPLib_AS_Get(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t* ReturnPtr)
{
    BPLib_Status_t Status;
    Status = BPLIB_SUCCESS;

    if (!BPLib_AS_EidIsValid(SourceEid))
    {
        Status = BPLIB_AS_INVALID_EID;
    }
    else
    {
        /* Node packet */
        switch (Counter)
        {
            case ADU_COUNT_DELIVERED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.AduCountDelivered;
                break;
            case ADU_COUNT_RECEIVED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.AduCountReceived;
                break;
            case BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount;
                break;
            case BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleAgentRejectedDirectiveCount;
                break;
            case BUNDLE_COUNT_ABANDONED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountAbandoned;
                break;
            case BUNDLE_COUNT_CUSTODY_RE_FORWARDED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountCustodyReForwarded;
                break;
            case BUNDLE_COUNT_CUSTODY_REJECTED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected;
                break;
            case BUNDLE_COUNT_CUSTODY_REQUEST:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountCustodyRequest;
                break;
            case BUNDLE_COUNT_CUSTODY_SIGNAL_RECEIVED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived;
                break;
            case BUNDLE_COUNT_CUSTODY_TRANSFERRED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountCustodyTransferred;
                break;
            case BUNDLE_COUNT_DELETED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeleted;
                break;
            case BUNDLE_COUNT_DELETED_BAD_EID:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedBadEid;
                break;
            case BUNDLE_COUNT_DELETED_CANCELLED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedCancelled;
                break;
            case BUNDLE_COUNT_DELETED_EXPIRED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedExpired;
                break;
            case BUNDLE_COUNT_DELETED_FORWARD_FAILED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedForwardFailed;
                break;
            case BUNDLE_COUNT_DELETED_HOP_EXCEEDED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedHopExceeded;
                break;
            case BUNDLE_COUNT_DELETED_INVALID_PAYLOAD:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedInvalidPayload;
                break;
            case BUNDLE_COUNT_DELETED_NO_STORAGE:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedNoStorage;
                break;
            case BUNDLE_COUNT_DELETED_TOO_LONG:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedTooLong;
                break;
            case BUNDLE_COUNT_DELETED_TRAFFIC_PARED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedTrafficPared;
                break;
            case BUNDLE_COUNT_DELETED_UNAUTHORIZED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedUnauthorized;
                break;
            case BUNDLE_COUNT_DELETED_UNINTELLIGIBLE:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedUnintelligible;
                break;
            case BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedUnsupportedBlock;
                break;
            case BUNDLE_COUNT_DELIVERED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDelivered;
                break;
            case BUNDLE_COUNT_DEPLETED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDepleted;
                break;
            case BUNDLE_COUNT_DISCARDED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDiscarded;
                break;
            case BUNDLE_COUNT_FORWARDED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountForwarded;
                break;
            case BUNDLE_COUNT_FORWARDED_FAILED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountForwardedFailed;
                break;
            case BUNDLE_COUNT_FRAGMENT_ERROR:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountFragmentError;
                break;
            case BUNDLE_COUNT_FRAGMENTED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountFragmented;
                break;
            case BUNDLE_COUNT_GENERATED_ACCEPTED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted;
                break;
            case BUNDLE_COUNT_GENERATED_ANONYMOUS:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedAnonymous;
                break;
            case BUNDLE_COUNT_GENERATED_BSR_ACCEPTED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrAccepted;
                break;
            case BUNDLE_COUNT_GENERATED_BSR_DELETED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDeleted;
                break;
            case BUNDLE_COUNT_GENERATED_BSR_DELIVERED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDelivered;
                break;
            case BUNDLE_COUNT_GENERATED_BSR_FORWARDED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrForwarded;
                break;
            case BUNDLE_COUNT_GENERATED_BSR_RECEIVED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrReceived;
                break;
            case BUNDLE_COUNT_GENERATED_CRS:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrs;
                break;
            case BUNDLE_COUNT_GENERATED_CRS_ACCEPTED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsAccepted;
                break;
            case BUNDLE_COUNT_GENERATED_CRS_DELETED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDeleted;
                break;
            case BUNDLE_COUNT_GENERATED_CRS_DELIVERED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDelivered;
                break;
            case BUNDLE_COUNT_GENERATED_CRS_FORWARDED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsForwarded;
                break;
            case BUNDLE_COUNT_GENERATED_CRS_RECEIVED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsReceived;
                break;
            case BUNDLE_COUNT_GENERATED_CUSTODY:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustody;
                break;
            case BUNDLE_COUNT_GENERATED_CUSTODY_SIGNAL:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustodySignal;
                break;
            case BUNDLE_COUNT_GENERATED_FRAGMENT:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedFragment;
                break;
            case BUNDLE_COUNT_GENERATED_REJECTED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedRejected;
                break;
            case BUNDLE_COUNT_IN_CUSTODY:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountInCustody;
                break;
            case BUNDLE_COUNT_INVALID_PRIMARY_BLOCK:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountInvalidPrimaryBlock;
                break;
            case BUNDLE_COUNT_MAX_BSR_RATE_EXCEEDED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountMaxBsrRateExceeded;
                break;
            case BUNDLE_COUNT_MAX_CRS_RATE_EXCEEDED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountMaxCrsRateExceeded;
                break;
            case BUNDLE_COUNT_NO_CONTACT:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountNoContact;
                break;
            case BUNDLE_COUNT_NO_FURTHER_INFO:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountNoFurtherInfo;
                break;
            case BUNDLE_COUNT_NO_ROUTE:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountNoRoute;
                break;
            case BUNDLE_COUNT_REASSEMBLED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReassembled;
                break;
            case BUNDLE_COUNT_RECEIVED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceived;
                break;
            case BUNDLE_COUNT_RECEIVED_ADMIN_RECORD:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedAdminRecord;
                break;
            case BUNDLE_COUNT_RECEIVED_BSR_ACCEPTED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrAccepted;
                break;
            case BUNDLE_COUNT_RECEIVED_BSR_DELETED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDeleted;
                break;
            case BUNDLE_COUNT_RECEIVED_BSR_DELIVERED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDelivered;
                break;
            case BUNDLE_COUNT_RECEIVED_BSR_FORWARDED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrForwarded;
                break;
            case BUNDLE_COUNT_RECEIVED_BSR_RECEIVED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrReceived;
                break;
            case BUNDLE_COUNT_RECEIVED_CRS:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedCrs;
                break;
            case BUNDLE_COUNT_RECEIVED_CRS_ACCEPTED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsAccepted;
                break;
            case BUNDLE_COUNT_RECEIVED_CRS_DELETED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDeleted;
                break;
            case BUNDLE_COUNT_RECEIVED_CRS_DELIVERED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDelivered;
                break;
            case BUNDLE_COUNT_RECEIVED_CRS_FORWARDED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsForwarded;
                break;
            case BUNDLE_COUNT_RECEIVED_CRS_RECEIVED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsReceived;
                break;
            case BUNDLE_COUNT_RECEIVED_CUSTODY_SIGNAL:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedCustodySignal;
                break;
            case BUNDLE_COUNT_RECEIVED_FRAGMENT:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedFragment;
                break;
            case BUNDLE_COUNT_REDUNDANT:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountRedundant;
                break;
            case BUNDLE_COUNT_REJECTED_CUSTODY:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountRejectedCustody;
                break;
            case BUNDLE_COUNT_RETURNED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReturned;
                break;
            case BUNDLE_COUNT_STORAGE_AVAILABLE:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountStorageAvailable;
                break;
            case BUNDLE_COUNT_STORED:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountStored;
                break;
            case BUNDLE_COUNT_UNINTELLIGIBLE_BLOCK:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleBlock;
                break;
            case BUNDLE_COUNT_UNINTELLIGIBLE_EID:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleEid;
                break;
            case BUNDLE_COUNT_UNPROCESSED_BLOCKS:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountUnprocessedBlocks;
                break;
            case NODE_STARTUP_COUNTER:
                *ReturnPtr = BPLib_AS_NodeCountersPayload.NodeStartupCounter;
                break;
            default:
                Status = BPLIB_AS_UNKNOWN_NODE_CNTR;
                break;
        }

        /*Source packet
        switch (Counter)
        {
            case BUNDLE_CNT_GEN_ACCPT:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted;
                break;
            case BUNDLE_CNT_GEN_REJ:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedRejected;
                break;
            case BUNDLE_CNT_GEN_FRAG:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedFragment;
                break;
            case BUNDLE_CNT_RECV:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceived;
                break;
            case BUNDLE_CNT_RECV_FRAG:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedFragment;
                break;
            case BUNDLE_CNT_UNPROC_BLKS:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnprocessedBlocks;
                break;
            case BUNDLE_CNT_FORW:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwarded;
                break;
            case BUNDLE_CNT_RET:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReturned;
                break;
            case BUNDLE_CNT_FRAG:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmented;
                break;
            case BUNDLE_CNT_REASSEMBLED:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReassembled;
                break;
            case BUNDLE_CNT_FRAG_ERR:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmentError;
                break;
            case BUNDLE_CNT_DELVR:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDelivered;
                break;
            case BUNDLE_CNT_ABAND:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountAbandoned;
                break;
            case ADU_CNT_DELVR:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountDelivered;
                break;
            case BUNDLE_CNT_DEL:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeleted;
                break;
            case BUNDLE_CNT_DEL_EXP:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedExpired;
                break;
            case BUNDLE_CNT_DEL_HOP_EXCD:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedHopExceeded;
                break;
            case BUNDLE_CNT_DEL_INVAL_PAY:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedInvalidPayload;
                break;
            case BUNDLE_CNT_DEL_FORW_FAILED:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedForwardFailed;
                break;
            case BUNDLE_CNT_DEL_TRAF_PARED:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTrafficPared;
                break;
            case BUNDLE_CNT_DEL_UNINTEL:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnintelligible;
                break;
            case BUNDLE_CNT_DEL_UNSUPPORT_BLK:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnsupportedBlock;
                break;
            case BUNDLE_CNT_DEL_CANCELLED:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedCancelled;
                break;
            case BUNDLE_CNT_DISCARDED:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDiscarded;
                break;
            case BUNDLE_CNT_DEL_NO_STOR:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedNoStorage;
                break;
            case BUNDLE_CNT_DEL_BAD_EID:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedBadEid;
                break;
            case BUNDLE_CNT_DEL_TOO_LONG:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTooLong;
                break;
            case BUNDLE_CNT_CUSTODY_TRANS:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyTransferred;
                break;
            case BUNDLE_CNT_CUSTODY_REJ:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected;
                break;
            case BUNDLE_CNT_CUSTODY_REQ:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRequest;
                break;
            case BUNDLE_CNT_CUSTODY_RE_FORW:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyReForwarded;
                break;
            case BUNDLE_CNT_NO_FURTHER_INFO:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoFurtherInfo;
                break;
            case BUNDLE_CNT_RED:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRedundant;
                break;
            case BUNDLE_CNT_DEPLETED:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDepleted;
                break;
            case BUNDLE_CNT_UNINTEL_EID:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleEid;
                break;
            case BUNDLE_CNT_NO_ROUTE:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoRoute;
                break;
            case BUNDLE_CNT_NO_CONTACT:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoContact;
                break;
            case BUNDLE_CNT_UNINTEL_BLK:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleBlock;
                break;
            case BUNDLE_CNT_RECV_CS:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCustodySignal;
                break;
            case BUNDLE_CNT_GEN_CS:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedCustodySignal;
                break;
            case BUNDLE_CNT_REJ_CUSTODY:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRejectedCustody;
                break;
            case ADU_CNT_RECV:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountReceived;
                break;
            case BUNDLE_CNT_FORW_FAILED:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwardedFailed;
                break;
            case BUNDLE_CNT_RECV_BSR_RECV:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrReceived;
                break;
            case BUNDLE_CNT_RECV_BSR_ACCPT:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrAccepted;
                break;
            case BUNDLE_CNT_RECV_BSR_FORW:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrForwarded;
                break;
            case BUNDLE_CNT_RECV_BSR_DELVR:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDelivered;
                break;
            case BUNDLE_CNT_RECV_BSR_DEL:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDeleted;
                break;
            case BUNDLE_CNT_RECV_CRS_RECV:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsReceived;
                break;
            case BUNDLE_CNT_RECV_CRS_ACCPT:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsAccepted;
                break;
            case BUNDLE_CNT_RECV_CRS_FORW:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsForwarded;
                break;
            case BUNDLE_CNT_RECV_CRS_DELVR:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDelivered;
                break;
            case BUNDLE_CNT_RECV_CRS_DEL:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDeleted;
                break;
            case BUNDLE_CNT_RECV_ADMIN_REC:
                *ReturnPtr = BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedAdminRecord;
                break;
            default:
                Status = BPLIB_AS_UNKNOWN_SRC_CNTR;
                break;
        }
        */
    }

    return Status;
}

BPLib_Status_t BPLib_AS_Set(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Value)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (!BPLib_AS_EidIsValid(SourceEid))
    { /* Provided source EID does not match an expected pattern */
        Status = BPLIB_AS_INVALID_EID;
    }
    else
    {
        /* Node counter */
        switch(Counter)
        {
            case ADU_COUNT_DELIVERED:
                BPLib_AS_NodeCountersPayload.AduCountDelivered = Value;
                break;
            case ADU_COUNT_RECEIVED:
                BPLib_AS_NodeCountersPayload.AduCountReceived = Value;
                break;
            case BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT:
                BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount = Value;
                break;
            case BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT:
                BPLib_AS_NodeCountersPayload.BundleAgentRejectedDirectiveCount = Value;
                break;
            case BUNDLE_COUNT_ABANDONED:
                BPLib_AS_NodeCountersPayload.BundleCountAbandoned = Value;
                break;
            case BUNDLE_COUNT_CUSTODY_RE_FORWARDED:
                BPLib_AS_NodeCountersPayload.BundleCountCustodyReForwarded = Value;
                break;
            case BUNDLE_COUNT_CUSTODY_REJECTED:
                BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected = Value;
                break;
            case BUNDLE_COUNT_CUSTODY_REQUEST:
                BPLib_AS_NodeCountersPayload.BundleCountCustodyRequest = Value;
                break;
            case BUNDLE_COUNT_CUSTODY_SIGNAL_RECEIVED:
                BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived = Value;
                break;
            case BUNDLE_COUNT_CUSTODY_TRANSFERRED:
                BPLib_AS_NodeCountersPayload.BundleCountCustodyTransferred = Value;
                break;
            case BUNDLE_COUNT_DELETED:
                BPLib_AS_NodeCountersPayload.BundleCountDeleted = Value;
                break;
            case BUNDLE_COUNT_DELETED_BAD_EID:
                BPLib_AS_NodeCountersPayload.BundleCountDeletedBadEid = Value;
                break;
            case BUNDLE_COUNT_DELETED_CANCELLED:
                BPLib_AS_NodeCountersPayload.BundleCountDeletedCancelled = Value;
                break;
            case BUNDLE_COUNT_DELETED_EXPIRED:
                BPLib_AS_NodeCountersPayload.BundleCountDeletedExpired = Value;
                break;
            case BUNDLE_COUNT_DELETED_FORWARD_FAILED:
                BPLib_AS_NodeCountersPayload.BundleCountDeletedForwardFailed = Value;
                break;
            case BUNDLE_COUNT_DELETED_HOP_EXCEEDED:
                BPLib_AS_NodeCountersPayload.BundleCountDeletedHopExceeded = Value;
                break;
            case BUNDLE_COUNT_DELETED_INVALID_PAYLOAD:
                BPLib_AS_NodeCountersPayload.BundleCountDeletedInvalidPayload = Value;
                break;
            case BUNDLE_COUNT_DELETED_NO_STORAGE:
                BPLib_AS_NodeCountersPayload.BundleCountDeletedNoStorage = Value;
                break;
            case BUNDLE_COUNT_DELETED_TOO_LONG:
                BPLib_AS_NodeCountersPayload.BundleCountDeletedTooLong = Value;
                break;
            case BUNDLE_COUNT_DELETED_TRAFFIC_PARED:
                BPLib_AS_NodeCountersPayload.BundleCountDeletedTrafficPared = Value;
                break;
            case BUNDLE_COUNT_DELETED_UNAUTHORIZED:
                BPLib_AS_NodeCountersPayload.BundleCountDeletedUnauthorized = Value;
                break;
            case BUNDLE_COUNT_DELETED_UNINTELLIGIBLE:
                BPLib_AS_NodeCountersPayload.BundleCountDeletedUnintelligible = Value;
                break;
            case BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK:
                BPLib_AS_NodeCountersPayload.BundleCountDeletedUnsupportedBlock = Value;
                break;
            case BUNDLE_COUNT_DELIVERED:
                BPLib_AS_NodeCountersPayload.BundleCountDelivered = Value;
                break;
            case BUNDLE_COUNT_DEPLETED:
                BPLib_AS_NodeCountersPayload.BundleCountDepleted = Value;
                break;
            case BUNDLE_COUNT_DISCARDED:
                BPLib_AS_NodeCountersPayload.BundleCountDiscarded = Value;
                break;
            case BUNDLE_COUNT_FORWARDED:
                BPLib_AS_NodeCountersPayload.BundleCountForwarded = Value;
                break;
            case BUNDLE_COUNT_FORWARDED_FAILED:
                BPLib_AS_NodeCountersPayload.BundleCountForwardedFailed = Value;
                break;
            case BUNDLE_COUNT_FRAGMENT_ERROR:
                BPLib_AS_NodeCountersPayload.BundleCountFragmentError = Value;
                break;
            case BUNDLE_COUNT_FRAGMENTED:
                BPLib_AS_NodeCountersPayload.BundleCountFragmented = Value;
                break;
            case BUNDLE_COUNT_GENERATED_ACCEPTED:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted = Value;
                break;
            case BUNDLE_COUNT_GENERATED_ANONYMOUS:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedAnonymous = Value;
                break;
            case BUNDLE_COUNT_GENERATED_BSR_ACCEPTED:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrAccepted = Value;
                break;
            case BUNDLE_COUNT_GENERATED_BSR_DELETED:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDeleted = Value;
                break;
            case BUNDLE_COUNT_GENERATED_BSR_DELIVERED:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDelivered = Value;
                break;
            case BUNDLE_COUNT_GENERATED_BSR_FORWARDED:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrForwarded = Value;
                break;
            case BUNDLE_COUNT_GENERATED_BSR_RECEIVED:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrReceived = Value;
                break;
            case BUNDLE_COUNT_GENERATED_CRS:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrs = Value;
                break;
            case BUNDLE_COUNT_GENERATED_CRS_ACCEPTED:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsAccepted = Value;
                break;
            case BUNDLE_COUNT_GENERATED_CRS_DELETED:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDeleted = Value;
                break;
            case BUNDLE_COUNT_GENERATED_CRS_DELIVERED:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDelivered = Value;
                break;
            case BUNDLE_COUNT_GENERATED_CRS_FORWARDED:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsForwarded = Value;
                break;
            case BUNDLE_COUNT_GENERATED_CRS_RECEIVED:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsReceived = Value;
                break;
            case BUNDLE_COUNT_GENERATED_CUSTODY:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustody = Value;
                break;
            case BUNDLE_COUNT_GENERATED_CUSTODY_SIGNAL:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustodySignal = Value;
                break;
            case BUNDLE_COUNT_GENERATED_FRAGMENT:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedFragment = Value;
                break;
            case BUNDLE_COUNT_GENERATED_REJECTED:
                BPLib_AS_NodeCountersPayload.BundleCountGeneratedRejected = Value;
                break;
            case BUNDLE_COUNT_IN_CUSTODY:
                BPLib_AS_NodeCountersPayload.BundleCountInCustody = Value;
                break;
            case BUNDLE_COUNT_INVALID_PRIMARY_BLOCK:
                BPLib_AS_NodeCountersPayload.BundleCountInvalidPrimaryBlock = Value;
                break;
            case BUNDLE_COUNT_MAX_BSR_RATE_EXCEEDED:
                BPLib_AS_NodeCountersPayload.BundleCountMaxBsrRateExceeded = Value;
                break;
            case BUNDLE_COUNT_MAX_CRS_RATE_EXCEEDED:
                BPLib_AS_NodeCountersPayload.BundleCountMaxCrsRateExceeded = Value;
                break;
            case BUNDLE_COUNT_NO_CONTACT:
                BPLib_AS_NodeCountersPayload.BundleCountNoContact = Value;
                break;
            case BUNDLE_COUNT_NO_FURTHER_INFO:
                BPLib_AS_NodeCountersPayload.BundleCountNoFurtherInfo = Value;
                break;
            case BUNDLE_COUNT_NO_ROUTE:
                BPLib_AS_NodeCountersPayload.BundleCountNoRoute = Value;
                break;
            case BUNDLE_COUNT_REASSEMBLED:
                BPLib_AS_NodeCountersPayload.BundleCountReassembled = Value;
                break;
            case BUNDLE_COUNT_RECEIVED:
                BPLib_AS_NodeCountersPayload.BundleCountReceived = Value;
                break;
            case BUNDLE_COUNT_RECEIVED_ADMIN_RECORD:
                BPLib_AS_NodeCountersPayload.BundleCountReceivedAdminRecord = Value;
                break;
            case BUNDLE_COUNT_RECEIVED_BSR_ACCEPTED:
                BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrAccepted = Value;
                break;
            case BUNDLE_COUNT_RECEIVED_BSR_DELETED:
                BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDeleted = Value;
                break;
            case BUNDLE_COUNT_RECEIVED_BSR_DELIVERED:
                BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDelivered = Value;
                break;
            case BUNDLE_COUNT_RECEIVED_BSR_FORWARDED:
                BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrForwarded = Value;
                break;
            case BUNDLE_COUNT_RECEIVED_BSR_RECEIVED:
                BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrReceived = Value;
                break;
            case BUNDLE_COUNT_RECEIVED_CRS:
                BPLib_AS_NodeCountersPayload.BundleCountReceivedCrs = Value;
                break;
            case BUNDLE_COUNT_RECEIVED_CRS_ACCEPTED:
                BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsAccepted = Value;
                break;
            case BUNDLE_COUNT_RECEIVED_CRS_DELETED:
                BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDeleted = Value;
                break;
            case BUNDLE_COUNT_RECEIVED_CRS_DELIVERED:
                BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDelivered = Value;
                break;
            case BUNDLE_COUNT_RECEIVED_CRS_FORWARDED:
                BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsForwarded = Value;
                break;
            case BUNDLE_COUNT_RECEIVED_CRS_RECEIVED:
                BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsReceived = Value;
                break;
            case BUNDLE_COUNT_RECEIVED_CUSTODY_SIGNAL:
                BPLib_AS_NodeCountersPayload.BundleCountReceivedCustodySignal = Value;
                break;
            case BUNDLE_COUNT_RECEIVED_FRAGMENT:
                BPLib_AS_NodeCountersPayload.BundleCountReceivedFragment = Value;
                break;
            case BUNDLE_COUNT_REDUNDANT:
                BPLib_AS_NodeCountersPayload.BundleCountRedundant = Value;
                break;
            case BUNDLE_COUNT_REJECTED_CUSTODY:
                BPLib_AS_NodeCountersPayload.BundleCountRejectedCustody = Value;
                break;
            case BUNDLE_COUNT_RETURNED:
                BPLib_AS_NodeCountersPayload.BundleCountReturned = Value;
                break;
            case BUNDLE_COUNT_STORAGE_AVAILABLE:
                BPLib_AS_NodeCountersPayload.BundleCountStorageAvailable = Value;
                break;
            case BUNDLE_COUNT_STORED:
                BPLib_AS_NodeCountersPayload.BundleCountStored = Value;
                break;
            case BUNDLE_COUNT_UNINTELLIGIBLE_BLOCK:
                BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleBlock = Value;
                break;
            case BUNDLE_COUNT_UNINTELLIGIBLE_EID:
                BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleEid = Value;
                break;
            case BUNDLE_COUNT_UNPROCESSED_BLOCKS:
                BPLib_AS_NodeCountersPayload.BundleCountUnprocessedBlocks = Value;
                break;
            case NODE_STARTUP_COUNTER:
                BPLib_AS_NodeCountersPayload.NodeStartupCounter = Value;
                break;
            default:
                Status = BPLIB_AS_UNKNOWN_NODE_CNTR;
                break;
        }

        /* Source counter
        switch (Counter)
        {
            case BUNDLE_CNT_GEN_ACCPT:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted = Value;
                break;
            case BUNDLE_CNT_GEN_REJ:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedRejected = Value;
                break;
            case BUNDLE_CNT_GEN_FRAG:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedFragment = Value;
                break;
            case BUNDLE_CNT_RECV:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceived = Value;
                break;
            case BUNDLE_CNT_RECV_FRAG:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedFragment = Value;
                break;
            case BUNDLE_CNT_UNPROC_BLKS:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnprocessedBlocks = Value;
                break;
            case BUNDLE_CNT_FORW:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwarded = Value;
                break;
            case BUNDLE_CNT_RET:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReturned = Value;
                break;
            case BUNDLE_CNT_FRAG:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmented = Value;
                break;
            case BUNDLE_CNT_REASSEMBLED:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReassembled = Value;
                break;
            case BUNDLE_CNT_FRAG_ERR:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmentError = Value;
                break;
            case BUNDLE_CNT_DELVR:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDelivered = Value;
                break;
            case BUNDLE_CNT_ABAND:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountAbandoned = Value;
                break;
            case ADU_CNT_DELVR:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountDelivered = Value;
                break;
            case BUNDLE_CNT_DEL:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeleted = Value;
                break;
            case BUNDLE_CNT_DEL_EXP:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedExpired = Value;
                break;
            case BUNDLE_CNT_DEL_HOP_EXCD:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedHopExceeded = Value;
                break;
            case BUNDLE_CNT_DEL_INVAL_PAY:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedInvalidPayload = Value;
                break;
            case BUNDLE_CNT_DEL_FORW_FAILED:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedForwardFailed = Value;
                break;
            case BUNDLE_CNT_DEL_TRAF_PARED:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTrafficPared = Value;
                break;
            case BUNDLE_CNT_DEL_UNINTEL:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnintelligible = Value;
                break;
            case BUNDLE_CNT_DEL_UNSUPPORT_BLK:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnsupportedBlock = Value;
                break;
            case BUNDLE_CNT_DEL_CANCELLED:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedCancelled = Value;
                break;
            case BUNDLE_CNT_DISCARDED:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDiscarded = Value;
                break;
            case BUNDLE_CNT_DEL_NO_STOR:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedNoStorage = Value;
                break;
            case BUNDLE_CNT_DEL_BAD_EID:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedBadEid = Value;
                break;
            case BUNDLE_CNT_DEL_TOO_LONG:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTooLong = Value;
                break;
            case BUNDLE_CNT_CUSTODY_TRANS:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyTransferred = Value;
                break;
            case BUNDLE_CNT_CUSTODY_REJ:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected = Value;
                break;
            case BUNDLE_CNT_CUSTODY_REQ:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRequest = Value;
                break;
            case BUNDLE_CNT_CUSTODY_RE_FORW:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyReForwarded = Value;
                break;
            case BUNDLE_CNT_NO_FURTHER_INFO:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoFurtherInfo = Value;
                break;
            case BUNDLE_CNT_RED:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRedundant = Value;
                break;
            case BUNDLE_CNT_DEPLETED:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDepleted = Value;
                break;
            case BUNDLE_CNT_UNINTEL_EID:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleEid = Value;
                break;
            case BUNDLE_CNT_NO_ROUTE:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoRoute = Value;
                break;
            case BUNDLE_CNT_NO_CONTACT:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoContact = Value;
                break;
            case BUNDLE_CNT_UNINTEL_BLK:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleBlock = Value;
                break;
            case BUNDLE_CNT_RECV_CS:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCustodySignal = Value;
                break;
            case BUNDLE_CNT_GEN_CS:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedCustodySignal = Value;
                break;
            case BUNDLE_CNT_REJ_CUSTODY:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRejectedCustody = Value;
                break;
            case ADU_CNT_RECV:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountReceived = Value;
                break;
            case BUNDLE_CNT_FORW_FAILED:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwardedFailed = Value;
                break;
            case BUNDLE_CNT_RECV_BSR_RECV:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrReceived = Value;
                break;
            case BUNDLE_CNT_RECV_BSR_ACCPT:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrAccepted = Value;
                break;
            case BUNDLE_CNT_RECV_BSR_FORW:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrForwarded = Value;
                break;
            case BUNDLE_CNT_RECV_BSR_DELVR:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDelivered = Value;
                break;
            case BUNDLE_CNT_RECV_BSR_DEL:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDeleted = Value;
                break;
            case BUNDLE_CNT_RECV_CRS_RECV:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsReceived = Value;
                break;
            case BUNDLE_CNT_RECV_CRS_ACCPT:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsAccepted = Value;
                break;
            case BUNDLE_CNT_RECV_CRS_FORW:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsForwarded = Value;
                break;
            case BUNDLE_CNT_RECV_CRS_DELVR:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDelivered = Value;
                break;
            case BUNDLE_CNT_RECV_CRS_DEL:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDeleted = Value;
                break;
            case BUNDLE_CNT_RECV_ADMIN_REC:
                BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedAdminRecord = Value;
                break;
            default:
                Status = BPLIB_AS_UNKNOWN_SRC_CNTR;
                break;
        }
        */
    }

    return Status;
}

BPLib_Status_t BPLib_AS_Increment(int16_t SourceEid, BPLib_AS_Counter_t Counter)
{
    BPLib_Status_t Status;
    uint32_t       CounterValue;

    /* Obtain the value of the counter */
    Status = BPLib_AS_Get(SourceEid, Counter, &CounterValue);

    if (Status != BPLIB_SUCCESS)
    { /* An error occured in BPLib_AS_Get() */
        switch (Status)
        {
            case BPLIB_AS_INVALID_EID:
                BPLib_EM_SendEvent(BPLIB_AS_INCREMENT_INVAL_EID_ERR_EID,
                                    BPLib_EM_EventType_ERROR,
                                    "Could not get counter %d to increment due to a source EID (%d) with unexpected pattern",
                                    Counter,
                                    SourceEid);

                break;
            case BPLIB_AS_UNKNOWN_NODE_CNTR:
                BPLib_EM_SendEvent(BPLIB_AS_INCREMENT_UNKNOWN_NODE_CNTR_ERR_EID,
                                    BPLib_EM_EventType_ERROR,
                                    "Could not get unrecognized node counter, %d, to increment",
                                    Counter);

                break;
            case BPLIB_AS_UNKNOWN_SRC_CNTR:
                BPLib_EM_SendEvent(BPLIB_AS_INCREMENT_UNKNOWN_SRC_CNTR_ERR_EID,
                                    BPLib_EM_EventType_ERROR,
                                    "Could not get unrecognized source counter, %d, to increment",
                                    Counter);

                break;
        }
    }
    else
    { /* BPLib_AS_Get() was successful */
        /* Increment the counter */
        CounterValue++;

        /* Set the incremented node counter and associated source counter(s) */
        Status = BPLib_AS_Set(SourceEid, Counter, CounterValue);

        switch (Status)
        {
            case BPLIB_AS_INVALID_EID:
                BPLib_EM_SendEvent(BPLIB_AS_INCREMENT_INVAL_EID_ERR_EID,
                                    BPLib_EM_EventType_ERROR,
                                    "Could not increment counter %d due to a source EID (%d) with unexpected pattern",
                                    Counter,
                                    SourceEid);

                break;
            case BPLIB_AS_UNKNOWN_NODE_CNTR:
                BPLib_EM_SendEvent(BPLIB_AS_INCREMENT_UNKNOWN_NODE_CNTR_ERR_EID,
                                    BPLib_EM_EventType_ERROR,
                                    "Could not increment unrecognized node counter %d",
                                    Counter);

                break;
            case BPLIB_AS_UNKNOWN_SRC_CNTR:
                BPLib_EM_SendEvent(BPLIB_AS_INCREMENT_UNKNOWN_SRC_CNTR_ERR_EID,
                                    BPLib_EM_EventType_ERROR,
                                    "Could not increment unrecognized source counter %d",
                                    Counter);
        }
    }

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_AS_INCREMENT_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Successfully incremented counter %d, for source with EID %d to %d",
                            Counter,
                            SourceEid,
                            CounterValue);
    }

    return Status;
}

BPLib_Status_t BPLib_AS_Decrement(int16_t SourceEid, BPLib_AS_Counter_t Counter)
{
    BPLib_Status_t Status;
    uint32_t       CounterValue;

    /* Obtain the value of the node counter */
    Status = BPLib_AS_Get(SourceEid, Counter, &CounterValue);

    if (Status != BPLIB_SUCCESS)
    { /* An error occured in BPLib_AS_Get() */
        switch (Status)
        {
            case BPLIB_AS_INVALID_EID:
                BPLib_EM_SendEvent(BPLIB_AS_DECREMENT_INVAL_EID_ERR_EID,
                                    BPLib_EM_EventType_ERROR,
                                    "Could not get counter %d to decrement due to a source EID (%d) with unexpected pattern",
                                    Counter,
                                    SourceEid);

                break;
            case BPLIB_AS_UNKNOWN_NODE_CNTR:
                BPLib_EM_SendEvent(BPLIB_AS_DECREMENT_UNKNOWN_NODE_CNTR_ERR_EID,
                                    BPLib_EM_EventType_ERROR,
                                    "Could not get unrecognized node counter, %d, to decrement",
                                    Counter);

                break;
            case BPLIB_AS_UNKNOWN_SRC_CNTR:
                BPLib_EM_SendEvent(BPLIB_AS_DECREMENT_UNKNOWN_SRC_CNTR_ERR_EID,
                                    BPLib_EM_EventType_ERROR,
                                    "Could not get unrecognized source counter, %d, to decrement",
                                    Counter);

                break;
        }
    }
    else
    {
        /* Decrement the node counter */
        CounterValue--;

        /* Set the decremented node counter and associated source counter(s) */
        Status = BPLib_AS_Set(SourceEid, Counter, CounterValue);

        switch (Status)
        {
            case BPLIB_AS_INVALID_EID:
                BPLib_EM_SendEvent(BPLIB_AS_DECREMENT_INVAL_EID_ERR_EID,
                                    BPLib_EM_EventType_ERROR,
                                    "Could not get counter %d to decrement due to a source EID (%d) with unexpected pattern",
                                    Counter,
                                    SourceEid);

                break;
            case BPLIB_AS_UNKNOWN_NODE_CNTR:
                BPLib_EM_SendEvent(BPLIB_AS_DECREMENT_UNKNOWN_NODE_CNTR_ERR_EID,
                                    BPLib_EM_EventType_ERROR,
                                    "Could not get unrecognized node counter, %d, to decrement",
                                    Counter);

                break;
            case BPLIB_AS_UNKNOWN_SRC_CNTR:
                BPLib_EM_SendEvent(BPLIB_AS_DECREMENT_UNKNOWN_SRC_CNTR_ERR_EID,
                                    BPLib_EM_EventType_ERROR,
                                    "Could not get unrecognized source counter, %d, to decrement",
                                    Counter);

                break;
        }
    }

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_AS_DECREMENT_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Successfully decremented counter %d, for source with EID %d to %d",
                            Counter,
                            SourceEid,
                            CounterValue);
    }

    return Status;
}

BPLib_Status_t BPLib_AS_ResetSourceCounters(int16_t SourceEid)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (!BPLib_AS_EidIsValid(SourceEid))
    {
        BPLib_EM_SendEvent(BPLIB_AS_RESET_SRC_INVAL_EID_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not reset source counters due to an invalid source EID (%d)",
                            SourceEid);

        Status = BPLIB_AS_INVALID_EID;
    }
    else
    {
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

        memset((void*) &BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid], 0,
                sizeof(BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid]));
    }

    return Status;
}

BPLib_Status_t BPLib_AS_ResetBundleCounters(int16_t SourceEid)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (!BPLib_AS_EidIsValid(SourceEid))
    {
        BPLib_EM_SendEvent(BPLIB_AS_RESET_BNDL_INVAL_EID_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not reset bundle counter due to invalid source EID (%d)",
                            SourceEid);
        
        Status = BPLIB_AS_INVALID_EID;
    }
    else
    {
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

    Status = BPLIB_SUCCESS;

    if (!BPLib_AS_EidIsValid(SourceEid))
    {
        BPLib_EM_SendEvent(BPLIB_AS_RESET_ERR_INVAL_EID_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not reset bundle counter due to invalid source EID (%d)",
                            SourceEid);

        Status = BPLIB_AS_INVALID_EID;
    }
    else
    {
        BPLib_AS_NodeCountersPayload.BundleAgentRejectedDirectiveCount = 0;
        BPLib_AS_NodeCountersPayload.BundleCountInvalidPrimaryBlock    = 0;

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

BPLib_Status_t BPLib_AS_ResetAllCounters(void)
{
    BPLib_Status_t Status;
    int16_t        SourceCtrl;

    Status = BPLIB_SUCCESS;

    memset((void*) &BPLib_AS_NodeCountersPayload, 0, sizeof(BPLib_AS_NodeCountersPayload));

    for (SourceCtrl = 0; SourceCtrl < BPLIB_MAX_NUM_SOURCE_EID; SourceCtrl++)
    {
        memset((void*) &BPLib_AS_SourceCountersPayload.SourceCounters[SourceCtrl], 0,
                sizeof(BPLib_AS_SourceCountersPayload.SourceCounters[SourceCtrl]));
    }

    return Status;
}

BPLib_Status_t BPLib_AS_Write()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /* Status = BPLib_STOR_Write(); */

    return Status;
}

BPLib_Status_t BPLib_AS_Restore()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}