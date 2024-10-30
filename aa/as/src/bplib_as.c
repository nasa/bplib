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

BPLib_NodeMibCountersHkTlm_Payload_t BPLib_AS_NodeCountersPayload;


/* ==================== */
/* Function Definitions */
/* ==================== */

BPLib_Status_t BPLib_AS_Init(void)
{
    BPLib_Status_t Status;

    // Instantiate mutex to protect counter memory space

    memset((void*) &BPLib_AS_NodeCountersPayload, 0, sizeof(BPLib_AS_NodeCountersPayload));

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_AS_Get(uint8_t CounterType, BPLib_AS_Counter_t Counter, void* ReturnPtr)
{
    BPLib_Status_t Status;

    /* if (CounterType == 1)
    {
        // Node packet
    */
    switch (Counter)
    {
        case BUNDLE_AGT_ACCPT_CNT:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount;
            break;
        case BUNDLE_AGT_REJ_CNT:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleAgentRejectedDirectiveCount;
            break;
        case BUNDLE_CNT_GEN_ACCPT:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted;
            break;
        case BUNDLE_CNT_GEN_REJ:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedRejected;
            break;
        case BUNDLE_CNT_GEN_CUSTODY:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustody;
            break;
        case BUNDLE_CNT_GEN_BSR_RECV:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrReceived;
            break;
        case BUNDLE_CNT_GEN_BSR_ACCPT:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrAccepted;
            break;
        case BUNDLE_CNT_GEN_BSR_FORW:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrForwarded;
            break;
        case BUNDLE_CNT_GEN_BSR_DELVR:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDelivered;
            break;
        case BUNDLE_CNT_GEN_BSR_DEL:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDeleted;
            break;
        case BUNDLE_CNT_MAX_BSR_RATE_EXCD:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountMaxBsrRateExceeded;
            break;
        case BUNDLE_CNT_GEN_ANON:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedAnonymous;
            break;
        case BUNDLE_CNT_GEN_FRAG:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedFragment;
            break;
        case BUNDLE_CNT_RECV:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceived;
            break;
        case BUNDLE_CNT_RECV_FRAG:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceivedFragment;
            break;
        case BUNDLE_CNT_UNPROC_BLKS:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountUnprocessedBlocks;
            break;
        case BUNDLE_CNT_INVAL_PRI_BLK:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountInvalidPrimaryBlock;
            break;
        case BUNDLE_CNT_CS_RECV:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived;
            break;
        case BUNDLE_CNT_FORW:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountForwarded;
            break;
        case BUNDLE_CNT_RET:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReturned;
            break;
        case BUNDLE_CNT_FRAG:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountFragmented;
            break;
        case BUNDLE_CNT_REASSEMBLED:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReassembled;
            break;
        case BUNDLE_CNT_FRAG_ERR:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountFragmentError;
            break;
        case BUNDLE_CNT_DELVR:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDelivered;
            break;
        case BUNDLE_CNT_ABAND:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountAbandoned;
            break;
        case ADU_CNT_DELVR:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.AduCountDelivered;
            break;
        case BUNDLE_CNT_DEL:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDeleted;
            break;
        case BUNDLE_CNT_DEL_EXP:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDeletedExpired;
            break;
        case BUNDLE_CNT_DEL_HOP_EXCD:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDeletedHopExceeded;
            break;
        case BUNDLE_CNT_DEL_INVAL_PAY:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDeletedInvalidPayload;
            break;
        case BUNDLE_CNT_DEL_FORW_FAILED:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDeletedForwardFailed;
            break;
        case BUNDLE_CNT_DEL_TRAF_PARED:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDeletedTrafficPared;
            break;
        case BUNDLE_CNT_DEL_UNINTEL:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDeletedUnintelligible;
            break;
        case BUNDLE_CNT_DEL_UNSUPPORT_BLK:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDeletedUnsupportedBlock;
            break;
        case BUNDLE_CNT_DEL_CANCELLED:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDeletedCancelled;
            break;
        case BUNDLE_CNT_DISCARDED:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDiscarded;
            break;
        case BUNDLE_CNT_DEL_NO_STOR:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDeletedNoStorage;
            break;
        case BUNDLE_CNT_DEL_BAD_EID:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDeletedBadEid;
            break;
        case BUNDLE_CNT_DEL_UNAUTH:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDeletedUnauthorized;
            break;
        case BUNDLE_CNT_DEL_TOO_LONG:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDeletedtooLong;
            break;
        case BUNDLE_CNT_CUSTODY_TRANS:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountCustodyTransferred;
            break;
        case BUNDLE_CNT_CUSTODY_REJ:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected;
            break;
        case BUNDLE_CNT_CUSTODY_REQ:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountCustodyRequest;
            break;
        case BUNDLE_CNT_CUSTODY_RE_FORW:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountCustodyReForwarded;
            break;
        case BUNDLE_CNT_NO_FURTHER_INFO:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountNoFurtherInfo;
            break;
        case BUNDLE_CNT_RED:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountRedundant;
            break;
        case BUNDLE_CNT_DEPLETED:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDepleted;
            break;
        case BUNDLE_CNT_UNINTEL_EID:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleEid;
            break;
        case BUNDLE_CNT_NO_ROUTE:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountNoRoute;
            break;
        case BUNDLE_CNT_NO_CONTACT:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountNoContact;
            break;
        case BUNDLE_CNT_UNINTEL_BLK:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleBlock;
            break;
        case BUNDLE_CNT_RECV_CS:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceivedCustodySignal;
            break;
        case BUNDLE_CNT_GEN_CS:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustodySignal;
            break;
        case BUNDLE_CNT_REJ_CUSTODY:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountRejectedCustody;
            break;
        case BUNDLE_CNT_GEN_CRS_RECV:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsReceived;
            break;
        case BUNDLE_CNT_GEN_CRS_ACCPT:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsAccepted;
            break;
        case BUNDLE_CNT_GEN_CRS_FORW:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsForwarded;
            break;
        case BUNDLE_CNT_GEN_CRS_DELVR:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDelivered;
            break;
        case BUNDLE_CNT_GEN_CRS_DEL:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDeleted;
            break;
        case BUNDLE_CNT_MAX_CRS_RATE_EXCD:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountMaxCrsRateExceeded;
            break;
        case ADU_CNT_RECV:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.AduCountReceived;
            break;
        case NODE_STARTUP_CNTR:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.NodeStartupCounter;
            break;
        case BUNDLE_CNT_GEN_CRS:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrs;
            break;
        case BUNDLE_CNT_FORW_FAILED:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountForwardedFailed;
            break;
        case BUNDLE_CNT_RECV_BSR_RECV:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrReceived;
            break;
        case BUNDLE_CNT_RECV_BSR_ACCPT:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrAccepted;
            break;
        case BUNDLE_CNT_RECV_BSR_FORW:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrForwarded;
            break;
        case BUNDLE_CNT_RECV_BSR_DELVR:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDelivered;
            break;
        case BUNDLE_CNT_RECV_BSR_DEL:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDeleted;
            break;
        case BUNDLE_CNT_RECV_CRS:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceivedCrs;
            break;
        case BUNDLE_CNT_RECV_CRS_RECV:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsReceived;
            break;
        case BUNDLE_CNT_RECV_CRS_ACCPT:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsAccepted;
            break;
        case BUNDLE_CNT_RECV_CRS_FORW:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsForwarded;
            break;
        case BUNDLE_CNT_RECV_CRS_DELVR:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDelivered;
            break;
        case BUNDLE_CNT_RECV_CRS_DEL:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDeleted;
            break;
        case BUNDLE_CNT_RECV_ADMIN_REC:
            ReturnPtr = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceivedAdminRecord;
            break;
        default:
            Status = BPLIB_AS_INVALID_CNTR;
            break;
    }
    /*
    }
    else if (CounterType == 2)
    {
        switch (Counter)
        {
            // Source packet
        }
    }
    else
    {
        Status = SOME_ERROR;
    }
    */

    return Status;
}

BPLib_Status_t BPLib_AS_Set(BPLib_AS_Counter_t Counter, void* DesiredValuePtr)
{
    BPLib_Status_t Status;
    Status = BPLIB_SUCCESS;

    switch (Counter)
    {
        case BUNDLE_AGT_ACCPT_CNT:
            BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_AGT_REJ_CNT:
            BPLib_AS_NodeCountersPayload.BundleAgentRejectedDirectiveCount = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_ACCPT:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_REJ:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedRejected = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_CUSTODY:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustody = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_BSR_RECV:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrReceived = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_BSR_ACCPT:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrAccepted = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_BSR_FORW:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrForwarded = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_BSR_DELVR:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDelivered = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_BSR_DEL:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDeleted = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_MAX_BSR_RATE_EXCD:
            BPLib_AS_NodeCountersPayload.BundleCountMaxBsrRateExceeded = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_ANON:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedAnonymous = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_FRAG:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedFragment = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RECV:
            BPLib_AS_NodeCountersPayload.BundleCountReceived = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RECV_FRAG:
            BPLib_AS_NodeCountersPayload.BundleCountReceivedFragment = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_UNPROC_BLKS:
            BPLib_AS_NodeCountersPayload.BundleCountUnprocessedBlocks = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_INVAL_PRI_BLK:
            BPLib_AS_NodeCountersPayload.BundleCountInvalidPrimaryBlock = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_CS_RECV:
            BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_FORW:
            BPLib_AS_NodeCountersPayload.BundleCountForwarded = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RET:
            BPLib_AS_NodeCountersPayload.BundleCountReturned = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_FRAG:
            BPLib_AS_NodeCountersPayload.BundleCountFragmented = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_REASSEMBLED:
            BPLib_AS_NodeCountersPayload.BundleCountReassembled = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_FRAG_ERR:
            BPLib_AS_NodeCountersPayload.BundleCountFragmentError = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DELVR:
            BPLib_AS_NodeCountersPayload.BundleCountDelivered = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_ABAND:
            BPLib_AS_NodeCountersPayload.BundleCountAbandoned = *(uint32_t*) DesiredValuePtr;
            break;
        case ADU_CNT_DELVR:
            BPLib_AS_NodeCountersPayload.AduCountDelivered = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DEL:
            BPLib_AS_NodeCountersPayload.BundleCountDeleted = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DEL_EXP:
            BPLib_AS_NodeCountersPayload.BundleCountDeletedExpired = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DEL_HOP_EXCD:
            BPLib_AS_NodeCountersPayload.BundleCountDeletedHopExceeded = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DEL_INVAL_PAY:
            BPLib_AS_NodeCountersPayload.BundleCountDeletedInvalidPayload = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DEL_FORW_FAILED:
            BPLib_AS_NodeCountersPayload.BundleCountDeletedForwardFailed = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DEL_TRAF_PARED:
            BPLib_AS_NodeCountersPayload.BundleCountDeletedTrafficPared = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DEL_UNINTEL:
            BPLib_AS_NodeCountersPayload.BundleCountDeletedUnintelligible = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DEL_UNSUPPORT_BLK:
            BPLib_AS_NodeCountersPayload.BundleCountDeletedUnsupportedBlock = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DEL_CANCELLED:
            BPLib_AS_NodeCountersPayload.BundleCountDeletedCancelled = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DISCARDED:
            BPLib_AS_NodeCountersPayload.BundleCountDiscarded = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DEL_NO_STOR:
            BPLib_AS_NodeCountersPayload.BundleCountDeletedNoStorage = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DEL_BAD_EID:
            BPLib_AS_NodeCountersPayload.BundleCountDeletedBadEid = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DEL_UNAUTH:
            BPLib_AS_NodeCountersPayload.BundleCountDeletedUnauthorized = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DEL_TOO_LONG:
            BPLib_AS_NodeCountersPayload.BundleCountDeletedtooLong = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_CUSTODY_TRANS:
            BPLib_AS_NodeCountersPayload.BundleCountCustodyTransferred = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_CUSTODY_REJ:
            BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_CUSTODY_REQ:
            BPLib_AS_NodeCountersPayload.BundleCountCustodyRequest = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_CUSTODY_RE_FORW:
            BPLib_AS_NodeCountersPayload.BundleCountCustodyReForwarded = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_NO_FURTHER_INFO:
            BPLib_AS_NodeCountersPayload.BundleCountNoFurtherInfo = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RED:
            BPLib_AS_NodeCountersPayload.BundleCountRedundant = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_DEPLETED:
            BPLib_AS_NodeCountersPayload.BundleCountDepleted = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_UNINTEL_EID:
            BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleEid = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_NO_ROUTE:
            BPLib_AS_NodeCountersPayload.BundleCountNoRoute = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_NO_CONTACT:
            BPLib_AS_NodeCountersPayload.BundleCountNoContact = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_UNINTEL_BLK:
            BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleBlock = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RECV_CS:
            BPLib_AS_NodeCountersPayload.BundleCountReceivedCustodySignal = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_CS:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustodySignal = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_REJ_CUSTODY:
            BPLib_AS_NodeCountersPayload.BundleCountRejectedCustody = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_CRS_RECV:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsReceived = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_CRS_ACCPT:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsAccepted = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_CRS_FORW:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsForwarded = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_CRS_DELVR:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDelivered = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_CRS_DEL:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDeleted = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_MAX_CRS_RATE_EXCD:
            BPLib_AS_NodeCountersPayload.BundleCountMaxCrsRateExceeded = *(uint32_t*) DesiredValuePtr;
            break;
        case ADU_CNT_RECV:
            BPLib_AS_NodeCountersPayload.AduCountReceived = *(uint32_t*) DesiredValuePtr;
            break;
        case NODE_STARTUP_CNTR:
            BPLib_AS_NodeCountersPayload.NodeStartupCounter = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_GEN_CRS:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrs = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_FORW_FAILED:
            BPLib_AS_NodeCountersPayload.BundleCountForwardedFailed = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RECV_BSR_RECV:
            BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrReceived = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RECV_BSR_ACCPT:
            BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrAccepted = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RECV_BSR_FORW:
            BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrForwarded = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RECV_BSR_DELVR:
            BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDelivered = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RECV_BSR_DEL:
            BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDeleted = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RECV_CRS:
            BPLib_AS_NodeCountersPayload.BundleCountReceivedCrs = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RECV_CRS_RECV:
            BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsReceived = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RECV_CRS_ACCPT:
            BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsAccepted = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RECV_CRS_FORW:
            BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsForwarded = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RECV_CRS_DELVR:
            BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDelivered = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RECV_CRS_DEL:
            BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDeleted = *(uint32_t*) DesiredValuePtr;
            break;
        case BUNDLE_CNT_RECV_ADMIN_REC:
            BPLib_AS_NodeCountersPayload.BundleCountReceivedAdminRecord = *(uint32_t*) DesiredValuePtr;
            break;
        default:
            Status = BPLIB_AS_INVALID_CNTR;
            break;
    }

    return Status;
}

BPLib_Status_t BPLib_AS_Increment(BPLib_AS_Counter_t Counter)
{
    BPLib_Status_t Status;
    uint32_t* SetVal;

    memset((void*) SetVal, 0, sizeof(SetVal));

    /* Obtain the value of the node counter */
    Status = BPLib_AS_Get(1, Counter, (void*) SetVal);

    if (Status == BPLIB_SUCCESS)
    {
        /* Set the incremented node counter and associated source counter(s) */
        Status = BPLib_AS_Set(Counter, (void*) *SetVal++);
    }

    return Status;
}

BPLib_Status_t BPLib_AS_Decrement(BPLib_AS_Counter_t Counter)
{
    BPLib_Status_t Status;
    uint32_t* SetVal;

    memset((void*) SetVal, 0, sizeof(SetVal));

    /* Obtain the value of the node counter */
    Status = BPLib_AS_Get(1, Counter, (void*) SetVal);

    if (Status == BPLIB_SUCCESS)
    {
        /* Set the decremented node counter and associated source counter(s) */
        Status = BPLib_AS_Set(Counter, (void*) *SetVal--);
    }

    return Status;
}

BPLib_Status_t BPLib_AS_ResetAllCounters()
{
    BPLib_Status_t Status;
    uint16_t       CounterLoopCtrl;
    uint8_t        Zero;

    Zero = 0;

    for (CounterLoopCtrl = 0; CounterLoopCtrl < BPLIB_AS_NUM_COUNTERS; CounterLoopCtrl++)
    {
        Status = BPLib_AS_Set(CounterLoopCtrl, (void*) &Zero);

        if (Status != BPLIB_SUCCESS)
        {
            break;
        }
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

/*
BPLib_Status_t BPLib_AS_RequestInterpreter()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    * Receives requests to increment counters or set statistics
    * determines whether request is valid
    * Generates and sends status messages with current telemetry to TLMP
    * Maintains set of source EID patterns

    return Status;
}

BPLib_Status_t BPLib_AS_StatisticsDatabase()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    * Maintains current values of statistics
    * Maintains map of MIB elements accessed by source EID pattern to counter values

    return Status;
}
*/
