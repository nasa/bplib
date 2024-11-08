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

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_AS_Get(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t* ReturnPtr)
{
    BPLib_Status_t Status;
    Status = BPLIB_SUCCESS;

    if (SourceEid <= -2 || SourceEid >= BPLIB_MAX_NUM_SOURCE_EID)
    {
        Status = BPLIB_AS_INVALID_EID;
    }
    else
    {
        if (SourceEid == BPLIB_AS_NODE_EID)
        { /* Node packet */
            switch (Counter)
            {
                case BUNDLE_AGT_ACCPT_CNT:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount;
                    break;
                case BUNDLE_AGT_REJ_CNT:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleAgentRejectedDirectiveCount;
                    break;
                case BUNDLE_CNT_GEN_ACCPT:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted;
                    break;
                case BUNDLE_CNT_GEN_REJ:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedRejected;
                    break;
                case BUNDLE_CNT_GEN_CUSTODY:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustody;
                    break;
                case BUNDLE_CNT_GEN_BSR_RECV:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrReceived;
                    break;
                case BUNDLE_CNT_GEN_BSR_ACCPT:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrAccepted;
                    break;
                case BUNDLE_CNT_GEN_BSR_FORW:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrForwarded;
                    break;
                case BUNDLE_CNT_GEN_BSR_DELVR:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDelivered;
                    break;
                case BUNDLE_CNT_GEN_BSR_DEL:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDeleted;
                    break;
                case BUNDLE_CNT_MAX_BSR_RATE_EXCD:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountMaxBsrRateExceeded;
                    break;
                case BUNDLE_CNT_GEN_ANON:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedAnonymous;
                    break;
                case BUNDLE_CNT_GEN_FRAG:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedFragment;
                    break;
                case BUNDLE_CNT_RECV:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceived;
                    break;
                case BUNDLE_CNT_RECV_FRAG:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedFragment;
                    break;
                case BUNDLE_CNT_UNPROC_BLKS:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountUnprocessedBlocks;
                    break;
                case BUNDLE_CNT_INVAL_PRI_BLK:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountInvalidPrimaryBlock;
                    break;
                case BUNDLE_CNT_CS_RECV:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived;
                    break;
                case BUNDLE_CNT_FORW:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountForwarded;
                    break;
                case BUNDLE_CNT_RET:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReturned;
                    break;
                case BUNDLE_CNT_FRAG:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountFragmented;
                    break;
                case BUNDLE_CNT_REASSEMBLED:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReassembled;
                    break;
                case BUNDLE_CNT_FRAG_ERR:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountFragmentError;
                    break;
                case BUNDLE_CNT_DELVR:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDelivered;
                    break;
                case BUNDLE_CNT_ABAND:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountAbandoned;
                    break;
                case ADU_CNT_DELVR:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.AduCountDelivered;
                    break;
                case BUNDLE_CNT_DEL:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeleted;
                    break;
                case BUNDLE_CNT_DEL_EXP:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedExpired;
                    break;
                case BUNDLE_CNT_DEL_HOP_EXCD:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedHopExceeded;
                    break;
                case BUNDLE_CNT_DEL_INVAL_PAY:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedInvalidPayload;
                    break;
                case BUNDLE_CNT_DEL_FORW_FAILED:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedForwardFailed;
                    break;
                case BUNDLE_CNT_DEL_TRAF_PARED:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedTrafficPared;
                    break;
                case BUNDLE_CNT_DEL_UNINTEL:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedUnintelligible;
                    break;
                case BUNDLE_CNT_DEL_UNSUPPORT_BLK:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedUnsupportedBlock;
                    break;
                case BUNDLE_CNT_DEL_CANCELLED:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedCancelled;
                    break;
                case BUNDLE_CNT_DISCARDED:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDiscarded;
                    break;
                case BUNDLE_CNT_DEL_NO_STOR:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedNoStorage;
                    break;
                case BUNDLE_CNT_DEL_BAD_EID:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedBadEid;
                    break;
                case BUNDLE_CNT_DEL_UNAUTH:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedUnauthorized;
                    break;
                case BUNDLE_CNT_DEL_TOO_LONG:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDeletedTooLong;
                    break;
                case BUNDLE_CNT_CUSTODY_TRANS:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountCustodyTransferred;
                    break;
                case BUNDLE_CNT_CUSTODY_REJ:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected;
                    break;
                case BUNDLE_CNT_CUSTODY_REQ:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountCustodyRequest;
                    break;
                case BUNDLE_CNT_CUSTODY_RE_FORW:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountCustodyReForwarded;
                    break;
                case BUNDLE_CNT_NO_FURTHER_INFO:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountNoFurtherInfo;
                    break;
                case BUNDLE_CNT_RED:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountRedundant;
                    break;
                case BUNDLE_CNT_DEPLETED:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountDepleted;
                    break;
                case BUNDLE_CNT_UNINTEL_EID:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleEid;
                    break;
                case BUNDLE_CNT_NO_ROUTE:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountNoRoute;
                    break;
                case BUNDLE_CNT_NO_CONTACT:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountNoContact;
                    break;
                case BUNDLE_CNT_UNINTEL_BLK:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleBlock;
                    break;
                case BUNDLE_CNT_RECV_CS:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedCustodySignal;
                    break;
                case BUNDLE_CNT_GEN_CS:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustodySignal;
                    break;
                case BUNDLE_CNT_REJ_CUSTODY:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountRejectedCustody;
                    break;
                case BUNDLE_CNT_GEN_CRS_RECV:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsReceived;
                    break;
                case BUNDLE_CNT_GEN_CRS_ACCPT:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsAccepted;
                    break;
                case BUNDLE_CNT_GEN_CRS_FORW:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsForwarded;
                    break;
                case BUNDLE_CNT_GEN_CRS_DELVR:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDelivered;
                    break;
                case BUNDLE_CNT_GEN_CRS_DEL:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDeleted;
                    break;
                case BUNDLE_CNT_MAX_CRS_RATE_EXCD:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountMaxCrsRateExceeded;
                    break;
                case ADU_CNT_RECV:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.AduCountReceived;
                    break;
                case NODE_STARTUP_CNTR:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.NodeStartupCounter;
                    break;
                case BUNDLE_CNT_GEN_CRS:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrs;
                    break;
                case BUNDLE_CNT_FORW_FAILED:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountForwardedFailed;
                    break;
                case BUNDLE_CNT_RECV_BSR_RECV:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrReceived;
                    break;
                case BUNDLE_CNT_RECV_BSR_ACCPT:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrAccepted;
                    break;
                case BUNDLE_CNT_RECV_BSR_FORW:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrForwarded;
                    break;
                case BUNDLE_CNT_RECV_BSR_DELVR:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDelivered;
                    break;
                case BUNDLE_CNT_RECV_BSR_DEL:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDeleted;
                    break;
                case BUNDLE_CNT_RECV_CRS:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedCrs;
                    break;
                case BUNDLE_CNT_RECV_CRS_RECV:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsReceived;
                    break;
                case BUNDLE_CNT_RECV_CRS_ACCPT:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsAccepted;
                    break;
                case BUNDLE_CNT_RECV_CRS_FORW:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsForwarded;
                    break;
                case BUNDLE_CNT_RECV_CRS_DELVR:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDelivered;
                    break;
                case BUNDLE_CNT_RECV_CRS_DEL:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDeleted;
                    break;
                case BUNDLE_CNT_RECV_ADMIN_REC:
                    *ReturnPtr = BPLib_AS_NodeCountersPayload.BundleCountReceivedAdminRecord;
                    break;
                default:
                    Status = BPLIB_AS_UNKNOWN_NODE_CNTR;
                    break;
            }
        }
        else
        { /* Source packet */
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
        }
    }

    return Status;
}

BPLib_Status_t BPLib_AS_Set(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t DesiredValue)
{
    BPLib_Status_t Status;
    Status = BPLIB_SUCCESS;

    if (SourceEid <= -2 || SourceEid >= BPLIB_MAX_NUM_SOURCE_EID)
    { /* Provided source EID does not match an expected pattern */
        Status = BPLIB_AS_INVALID_EID;
    }
    else
    {
        if (SourceEid == BPLIB_AS_NODE_EID)
        { /* Node counter */
            switch(Counter)
            {
                case BUNDLE_AGT_ACCPT_CNT:
                    BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount = DesiredValue;
                    break;
                case BUNDLE_AGT_REJ_CNT:
                    BPLib_AS_NodeCountersPayload.BundleAgentRejectedDirectiveCount = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_ACCPT:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_REJ:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedRejected = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_CUSTODY:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustody = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_BSR_RECV:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrReceived = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_BSR_ACCPT:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrAccepted = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_BSR_FORW:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrForwarded = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_BSR_DELVR:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDelivered = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_BSR_DEL:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDeleted = DesiredValue;
                    break;
                case BUNDLE_CNT_MAX_BSR_RATE_EXCD:
                    BPLib_AS_NodeCountersPayload.BundleCountMaxBsrRateExceeded = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_ANON:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedAnonymous = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_FRAG:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedFragment = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV:
                    BPLib_AS_NodeCountersPayload.BundleCountReceived = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_FRAG:
                    BPLib_AS_NodeCountersPayload.BundleCountReceivedFragment = DesiredValue;
                    break;
                case BUNDLE_CNT_UNPROC_BLKS:
                    BPLib_AS_NodeCountersPayload.BundleCountUnprocessedBlocks = DesiredValue;
                    break;
                case BUNDLE_CNT_INVAL_PRI_BLK:
                    BPLib_AS_NodeCountersPayload.BundleCountInvalidPrimaryBlock = DesiredValue;
                    break;
                case BUNDLE_CNT_CS_RECV:
                    BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived = DesiredValue;
                    break;
                case BUNDLE_CNT_FORW:
                    BPLib_AS_NodeCountersPayload.BundleCountForwarded = DesiredValue;
                    break;
                case BUNDLE_CNT_RET:
                    BPLib_AS_NodeCountersPayload.BundleCountReturned = DesiredValue;
                    break;
                case BUNDLE_CNT_FRAG:
                    BPLib_AS_NodeCountersPayload.BundleCountFragmented = DesiredValue;
                    break;
                case BUNDLE_CNT_REASSEMBLED:
                    BPLib_AS_NodeCountersPayload.BundleCountReassembled = DesiredValue;
                    break;
                case BUNDLE_CNT_FRAG_ERR:
                    BPLib_AS_NodeCountersPayload.BundleCountFragmentError = DesiredValue;
                    break;
                case BUNDLE_CNT_DELVR:
                    BPLib_AS_NodeCountersPayload.BundleCountDelivered = DesiredValue;
                    break;
                case BUNDLE_CNT_ABAND:
                    BPLib_AS_NodeCountersPayload.BundleCountAbandoned = DesiredValue;
                    break;
                case ADU_CNT_DELVR:
                    BPLib_AS_NodeCountersPayload.AduCountDelivered = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL:
                    BPLib_AS_NodeCountersPayload.BundleCountDeleted = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_EXP:
                    BPLib_AS_NodeCountersPayload.BundleCountDeletedExpired = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_HOP_EXCD:
                    BPLib_AS_NodeCountersPayload.BundleCountDeletedHopExceeded = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_INVAL_PAY:
                    BPLib_AS_NodeCountersPayload.BundleCountDeletedInvalidPayload = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_FORW_FAILED:
                    BPLib_AS_NodeCountersPayload.BundleCountDeletedForwardFailed = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_TRAF_PARED:
                    BPLib_AS_NodeCountersPayload.BundleCountDeletedTrafficPared = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_UNINTEL:
                    BPLib_AS_NodeCountersPayload.BundleCountDeletedUnintelligible = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_UNSUPPORT_BLK:
                    BPLib_AS_NodeCountersPayload.BundleCountDeletedUnsupportedBlock = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_CANCELLED:
                    BPLib_AS_NodeCountersPayload.BundleCountDeletedCancelled = DesiredValue;
                    break;
                case BUNDLE_CNT_DISCARDED:
                    BPLib_AS_NodeCountersPayload.BundleCountDiscarded = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_NO_STOR:
                    BPLib_AS_NodeCountersPayload.BundleCountDeletedNoStorage = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_BAD_EID:
                    BPLib_AS_NodeCountersPayload.BundleCountDeletedBadEid = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_UNAUTH:
                    BPLib_AS_NodeCountersPayload.BundleCountDeletedUnauthorized = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_TOO_LONG:
                    BPLib_AS_NodeCountersPayload.BundleCountDeletedTooLong = DesiredValue;
                    break;
                case BUNDLE_CNT_CUSTODY_TRANS:
                    BPLib_AS_NodeCountersPayload.BundleCountCustodyTransferred = DesiredValue;
                    break;
                case BUNDLE_CNT_CUSTODY_REJ:
                    BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected = DesiredValue;
                    break;
                case BUNDLE_CNT_CUSTODY_REQ:
                    BPLib_AS_NodeCountersPayload.BundleCountCustodyRequest = DesiredValue;
                    break;
                case BUNDLE_CNT_CUSTODY_RE_FORW:
                    BPLib_AS_NodeCountersPayload.BundleCountCustodyReForwarded = DesiredValue;
                    break;
                case BUNDLE_CNT_NO_FURTHER_INFO:
                    BPLib_AS_NodeCountersPayload.BundleCountNoFurtherInfo = DesiredValue;
                    break;
                case BUNDLE_CNT_RED:
                    BPLib_AS_NodeCountersPayload.BundleCountRedundant = DesiredValue;
                    break;
                case BUNDLE_CNT_DEPLETED:
                    BPLib_AS_NodeCountersPayload.BundleCountDepleted = DesiredValue;
                    break;
                case BUNDLE_CNT_UNINTEL_EID:
                    BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleEid = DesiredValue;
                    break;
                case BUNDLE_CNT_NO_ROUTE:
                    BPLib_AS_NodeCountersPayload.BundleCountNoRoute = DesiredValue;
                    break;
                case BUNDLE_CNT_NO_CONTACT:
                    BPLib_AS_NodeCountersPayload.BundleCountNoContact = DesiredValue;
                    break;
                case BUNDLE_CNT_UNINTEL_BLK:
                    BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleBlock = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_CS:
                    BPLib_AS_NodeCountersPayload.BundleCountReceivedCustodySignal = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_CS:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustodySignal = DesiredValue;
                    break;
                case BUNDLE_CNT_REJ_CUSTODY:
                    BPLib_AS_NodeCountersPayload.BundleCountRejectedCustody = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_CRS_RECV:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsReceived = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_CRS_ACCPT:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsAccepted = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_CRS_FORW:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsForwarded = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_CRS_DELVR:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDelivered = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_CRS_DEL:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDeleted = DesiredValue;
                    break;
                case BUNDLE_CNT_MAX_CRS_RATE_EXCD:
                    BPLib_AS_NodeCountersPayload.BundleCountMaxCrsRateExceeded = DesiredValue;
                    break;
                case ADU_CNT_RECV:
                    BPLib_AS_NodeCountersPayload.AduCountReceived = DesiredValue;
                    break;
                case NODE_STARTUP_CNTR:
                    BPLib_AS_NodeCountersPayload.NodeStartupCounter = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_CRS:
                    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrs = DesiredValue;
                    break;
                case BUNDLE_CNT_FORW_FAILED:
                    BPLib_AS_NodeCountersPayload.BundleCountForwardedFailed = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_BSR_RECV:
                    BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrReceived = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_BSR_ACCPT:
                    BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrAccepted = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_BSR_FORW:
                    BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrForwarded = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_BSR_DELVR:
                    BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDelivered = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_BSR_DEL:
                    BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDeleted = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_CRS:
                    BPLib_AS_NodeCountersPayload.BundleCountReceivedCrs = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_CRS_RECV:
                    BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsReceived = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_CRS_ACCPT:
                    BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsAccepted = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_CRS_FORW:
                    BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsForwarded = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_CRS_DELVR:
                    BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDelivered = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_CRS_DEL:
                    BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDeleted = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_ADMIN_REC:
                    BPLib_AS_NodeCountersPayload.BundleCountReceivedAdminRecord = DesiredValue;
                    break;
                default:
                    Status = BPLIB_AS_UNKNOWN_NODE_CNTR;
                    break;
            }
        }
        else
        { /* Source counter */
            switch (Counter)
            {
                case BUNDLE_CNT_GEN_ACCPT:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_REJ:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedRejected = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_FRAG:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedFragment = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceived = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_FRAG:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedFragment = DesiredValue;
                    break;
                case BUNDLE_CNT_UNPROC_BLKS:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnprocessedBlocks = DesiredValue;
                    break;
                case BUNDLE_CNT_FORW:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwarded = DesiredValue;
                    break;
                case BUNDLE_CNT_RET:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReturned = DesiredValue;
                    break;
                case BUNDLE_CNT_FRAG:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmented = DesiredValue;
                    break;
                case BUNDLE_CNT_REASSEMBLED:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReassembled = DesiredValue;
                    break;
                case BUNDLE_CNT_FRAG_ERR:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmentError = DesiredValue;
                    break;
                case BUNDLE_CNT_DELVR:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDelivered = DesiredValue;
                    break;
                case BUNDLE_CNT_ABAND:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountAbandoned = DesiredValue;
                    break;
                case ADU_CNT_DELVR:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountDelivered = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeleted = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_EXP:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedExpired = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_HOP_EXCD:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedHopExceeded = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_INVAL_PAY:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedInvalidPayload = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_FORW_FAILED:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedForwardFailed = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_TRAF_PARED:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTrafficPared = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_UNINTEL:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnintelligible = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_UNSUPPORT_BLK:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnsupportedBlock = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_CANCELLED:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedCancelled = DesiredValue;
                    break;
                case BUNDLE_CNT_DISCARDED:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDiscarded = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_NO_STOR:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedNoStorage = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_BAD_EID:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedBadEid = DesiredValue;
                    break;
                case BUNDLE_CNT_DEL_TOO_LONG:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTooLong = DesiredValue;
                    break;
                case BUNDLE_CNT_CUSTODY_TRANS:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyTransferred = DesiredValue;
                    break;
                case BUNDLE_CNT_CUSTODY_REJ:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected = DesiredValue;
                    break;
                case BUNDLE_CNT_CUSTODY_REQ:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRequest = DesiredValue;
                    break;
                case BUNDLE_CNT_CUSTODY_RE_FORW:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyReForwarded = DesiredValue;
                    break;
                case BUNDLE_CNT_NO_FURTHER_INFO:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoFurtherInfo = DesiredValue;
                    break;
                case BUNDLE_CNT_RED:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRedundant = DesiredValue;
                    break;
                case BUNDLE_CNT_DEPLETED:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDepleted = DesiredValue;
                    break;
                case BUNDLE_CNT_UNINTEL_EID:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleEid = DesiredValue;
                    break;
                case BUNDLE_CNT_NO_ROUTE:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoRoute = DesiredValue;
                    break;
                case BUNDLE_CNT_NO_CONTACT:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoContact = DesiredValue;
                    break;
                case BUNDLE_CNT_UNINTEL_BLK:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleBlock = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_CS:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCustodySignal = DesiredValue;
                    break;
                case BUNDLE_CNT_GEN_CS:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedCustodySignal = DesiredValue;
                    break;
                case BUNDLE_CNT_REJ_CUSTODY:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRejectedCustody = DesiredValue;
                    break;
                case ADU_CNT_RECV:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountReceived = DesiredValue;
                    break;
                case BUNDLE_CNT_FORW_FAILED:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwardedFailed = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_BSR_RECV:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrReceived = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_BSR_ACCPT:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrAccepted = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_BSR_FORW:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrForwarded = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_BSR_DELVR:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDelivered = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_BSR_DEL:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDeleted = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_CRS_RECV:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsReceived = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_CRS_ACCPT:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsAccepted = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_CRS_FORW:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsForwarded = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_CRS_DELVR:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDelivered = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_CRS_DEL:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDeleted = DesiredValue;
                    break;
                case BUNDLE_CNT_RECV_ADMIN_REC:
                    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedAdminRecord = DesiredValue;
                    break;
                default:
                    Status = BPLIB_AS_UNKNOWN_SRC_CNTR;
                    break;
            }
        }
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

BPLib_Status_t BPLib_AS_ResetAllCounters(void)
{
    BPLib_Status_t Status;
    BPLib_Status_t SetStatus;
    uint16_t       CounterCtrl;
    int16_t        SourceCtrl;

    Status = BPLIB_SUCCESS;

    for (CounterCtrl = 0; CounterCtrl < BPLIB_AS_NUM_NODE_CNTRS; CounterCtrl++)
    {
        SetStatus = BPLib_AS_Set(BPLIB_AS_NODE_EID, ResettableNodeCounters[CounterCtrl], 0);

        if (SetStatus == BPLIB_AS_UNKNOWN_NODE_CNTR)
        {
            Status = BPLIB_AS_UNKNOWN_NODE_CNTR;

            BPLib_EM_SendEvent(BPLIB_AS_RESET_ALL_UNKNOWN_NODE_CNTR_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Could not set unrecognized node counter, %d, to zero",
                                CounterCtrl);
        }
    }

    for (SourceCtrl = 0; SourceCtrl < BPLIB_MAX_NUM_SOURCE_EID; SourceCtrl++)
    {
        for (CounterCtrl = 0; CounterCtrl < BPLIB_AS_NUM_SRC_CNTRS; CounterCtrl++)
        {
            SetStatus = BPLib_AS_Set(SourceCtrl, ResettableSourceCounters[CounterCtrl], 0);

            switch (SetStatus)
            {
                case BPLIB_AS_INVALID_EID:
                    Status = BPLIB_AS_INVALID_EID;

                    BPLib_EM_SendEvent(BPLIB_AS_RESET_ALL_INVAL_EID_ERR_EID,
                                        BPLib_EM_EventType_ERROR,
                                        "Could not set counter %d to zero due to a source EID (%d) with unexpected pattern",
                                        CounterCtrl,
                                        SourceCtrl);

                    break;
                case BPLIB_AS_UNKNOWN_SRC_CNTR:
                    Status = BPLIB_AS_UNKNOWN_SRC_CNTR;

                    BPLib_EM_SendEvent(BPLIB_AS_RESET_ALL_UNKNOWN_SRC_CNTR_ERR_EID,
                                        BPLib_EM_EventType_ERROR,
                                        "Could not set unrecognized source counter, %d, to zero",
                                        CounterCtrl);

                    break;
            }
        }
    }

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_AS_RESET_ALL_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Successfully set the counters for every node and every source to 0");
    }

    return Status;
}

BPLib_Status_t BPLib_AS_ResetSourceCounters(int16_t SourceEid)
{
    BPLib_Status_t Status;
    BPLib_Status_t SetStatus;
    int16_t CounterCtrl;

    Status = BPLIB_SUCCESS;

    if (SourceEid == BPLIB_AS_NODE_EID)
    { /* Given source EID is for a node */
        Status = BPLIB_AS_INVALID_EID;

        BPLib_EM_SendEvent(BPLIB_AS_RESET_SRC_INVAL_EID_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not reset source counter due to a source EID indicator for a node counter (%d)",
                            SourceEid);
    }
    else
    { /* Given source EID is for a source */
        for(CounterCtrl = 0; CounterCtrl < BPLIB_AS_NUM_SRC_CNTRS; CounterCtrl++)
        {
            SetStatus = BPLib_AS_Set(SourceEid, ResettableSourceCounters[CounterCtrl], 0);

            switch (SetStatus)
            {
                case BPLIB_AS_INVALID_EID:
                    Status = BPLIB_AS_INVALID_EID;

                    BPLib_EM_SendEvent(BPLIB_AS_RESET_SRC_INVAL_EID_ERR_EID,
                                        BPLib_EM_EventType_ERROR,
                                        "Could not set source counter %d to zero due to a source EID (%d) with unexpected pattern",
                                        CounterCtrl,
                                        SourceEid);

                    break;
                case BPLIB_AS_UNKNOWN_SRC_CNTR:
                    Status = BPLIB_AS_UNKNOWN_SRC_CNTR;

                    BPLib_EM_SendEvent(BPLIB_AS_RESET_SRC_UNKNOWN_SRC_CNTR_ERR_EID,
                                        BPLib_EM_EventType_ERROR,
                                        "Could not set unrecognized source counter, %d, to zero",
                                        CounterCtrl);

                    break;
            }

            /* 
            ** This loop will continue even if an error occurs just so that 
            ** all possible counters will be reset to 0
            */
        }
    }
    
    return Status;
}

BPLib_Status_t BPLib_AS_ResetBundleCounters(int16_t SourceEid)
{
    BPLib_Status_t Status;
    BPLib_Status_t SetStatus;
    int16_t CounterCtrl;

    Status = BPLIB_SUCCESS;

    for(CounterCtrl = 0; CounterCtrl < BPLIB_AS_NUM_BNDL_CNTRS; CounterCtrl++)
    {
        SetStatus = BPLib_AS_Set(SourceEid, BundleCounters[CounterCtrl], 0);

        switch (SetStatus)
        {
            case BPLIB_AS_INVALID_EID:
                Status = BPLIB_AS_INVALID_EID;

                BPLib_EM_SendEvent(BPLIB_AS_RESET_BNDL_INVAL_EID_ERR_EID,
                                    BPLib_EM_EventType_ERROR,
                                    "Could not set bundle counter %d to zero due to a source EID (%d) with unexpected pattern",
                                    CounterCtrl,
                                    SourceEid);

                break;
            case BPLIB_AS_UNKNOWN_NODE_CNTR:
                Status = BPLIB_AS_UNKNOWN_NODE_CNTR;

                BPLib_EM_SendEvent(BPLIB_AS_RESET_BNDL_UNKNOWN_NODE_CNTR_ERR_EID,
                                    BPLib_EM_EventType_ERROR,
                                    "Could not set unrecognized bundle node counter, %d, to zero",
                                    CounterCtrl);

                break;
            case BPLIB_AS_UNKNOWN_SRC_CNTR:
                Status = BPLIB_AS_UNKNOWN_SRC_CNTR;

                BPLib_EM_SendEvent(BPLIB_AS_RESET_BNDL_UNKNOWN_SRC_CNTR_ERR_EID,
                                    BPLib_EM_EventType_ERROR,
                                    "Could not set unrecognized bundle source counter, %d, to zero",
                                    CounterCtrl);

                break;
        }

        /* 
        ** This loop will continue even if an error occurs just so that 
        ** all possible counters will be reset to 0
        */
    }
    
    return Status;
}

BPLib_Status_t BPLib_AS_ResetErrorCounters(void)
{
    BPLib_Status_t Status;
    BPLib_Status_t SetStatus;
    int16_t CounterCtrl;

    Status = BPLIB_SUCCESS;

    for(CounterCtrl = 0; CounterCtrl < BPLIB_AS_NUM_ERR_CNTRS; CounterCtrl++)
    {
        SetStatus = BPLib_AS_Set(BPLIB_AS_NODE_EID, ErrorCounters[CounterCtrl], 0);

        if (SetStatus == BPLIB_AS_UNKNOWN_NODE_CNTR)
        {
            Status = BPLIB_AS_UNKNOWN_NODE_CNTR;

            BPLib_EM_SendEvent(BPLIB_AS_RESET_ERR_UNKNOWN_NODE_CNTR_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Could not set unrecognized bundle node counter, %d, to zero",
                                CounterCtrl);
        }

        /* 
        ** This loop will continue even if an error occurs just so that 
        ** all possible counters will be reset to 0
        */
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