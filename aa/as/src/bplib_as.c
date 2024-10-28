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

void* BPLib_AS_Get(BPLib_AS_CounterPacket_t CounterType, BPLib_AS_NodeCounter_t CounterIndex)
{
    BPLib_Status_t Status;
    Status = BPLIB_SUCCESS;

    void* ReturnVal;

    /* if (CounterType == NODE_COUNTER)
    {
        // Node packet counter access
    */
    switch (CounterIndex)
    {
        case BUNDLE_CNT_GEN_ANONYMOUS:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedAnonymous;
            break;
        case SYSTEM_NODE_UP_TIME:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.SystemNodeUpTime;
            break;
        case ACCEPTED_DIRECTIVE_CNT:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.AcceptedDirectiveCount;
            break;
        case REJECTED_DIRECTIVE_CNT:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.RejectedDirectiveCount;
            break;
        case BUNDLE_CNT_GEN_CUSTODY:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustody;
            break;
        case BUNDLE_CNT_GEN_BSR_RECV:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrReceived;
            break;
        case BUNDLE_CNT_GEN_BSR_ACCPT:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrAccepted;
            break;
        case BUNDLE_CNT_GEN_BSR_FORW:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrForwarded;
            break;
        case BUNDLE_CNT_GEN_BSR_DELVR:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDelivered;
            break;
        case BUNDLE_CNT_GEN_BSR_DEL:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDeleted;
            break;
        case BUNDLE_CNT_INVAL_PRI_BLK:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountInvalidPrimaryBlock;
            break;
        case BUNDLE_CNT_CS_RECV:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived;
            break;
        case BUNDLE_CNT_DEL_UNAUTH_SRC:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountDeletedUnauthorizedSrc;
            break;
        case BUNDLE_CNT_GEN_CRS_RECV:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsReceived;
            break;
        case BUNDLE_CNT_GEN_CRS_ACCPT:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsAccepted;
            break;
        case BUNDLE_CNT_GEN_CRS_FORW:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsForwarded;
            break;
        case BUNDLE_CNT_GEN_CRS_DELVR:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDelivered;
            break;
        case BUNDLE_CNT_GEN_CRS_DEL:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDeleted;
            break;
        case BUNDLE_CNT_MAX_CRS_RATE_EXC:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountMaxCrsRateExceeded;
            break;
        case NODE_STARTUP_COUNTER:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.NodeStartupCounter;
            break;
        case BUNDLE_CNT_GEN_CRS:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrs;
            break;
        case BUNDLE_CNT_RECV_CRS:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.BundleCountReceivedCrs;
            break;
        case ADU_COUNT_DELIVERED:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.AduCountDelivered;
            break;
        case ADU_COUNT_RECEIVED:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.AduCountReceived;
            break;
        case TIME_BOOT_ERA:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.TimeBootEra;
            break;
        case TIME_MONOTONIC_CNT:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.MonotonicTime;
            break;
        case TIME_CF:
            ReturnVal = (void*) &BPLib_AS_NodeCountersPayload.CorrelationFactor;
            break;
        default:
            break;
    }
    /* else
    {
        switch (CounterIndex)
        {
            // Source packet counter access
        }
    }
    */

    return ReturnVal;
}

BPLib_Status_t BPLib_AS_Set(BPLib_AS_CounterPacket_t CounterType, BPLib_AS_NodeCounter_t CounterIndex, void* DesiredValuePtr)
{
    BPLib_Status_t Status;
    Status = BPLIB_SUCCESS;

    /* if (CounterType == NODE_COUNTER)
    {
        // Node packet counter modification
    */
    switch (CounterIndex)
    {
        case BUNDLE_CNT_GEN_ANONYMOUS:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedAnonymous = *((uint32_t*) DesiredValuePtr);
            break;
        case SYSTEM_NODE_UP_TIME:
            BPLib_AS_NodeCountersPayload.SystemNodeUpTime = *((uint32_t*) DesiredValuePtr);
            break;
        case ACCEPTED_DIRECTIVE_CNT:
            BPLib_AS_NodeCountersPayload.AcceptedDirectiveCount = *((uint32_t*) DesiredValuePtr);
            break;
        case REJECTED_DIRECTIVE_CNT:
            BPLib_AS_NodeCountersPayload.RejectedDirectiveCount = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_GEN_CUSTODY:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustody = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_GEN_BSR_RECV:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrReceived = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_GEN_BSR_ACCPT:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrAccepted = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_GEN_BSR_FORW:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrForwarded = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_GEN_BSR_DELVR:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDelivered = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_GEN_BSR_DEL:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDeleted = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_INVAL_PRI_BLK:
            BPLib_AS_NodeCountersPayload.BundleCountInvalidPrimaryBlock = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_CS_RECV:
            BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_DEL_UNAUTH_SRC:
            BPLib_AS_NodeCountersPayload.BundleCountDeletedUnauthorizedSrc = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_GEN_CRS_RECV:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsReceived = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_GEN_CRS_ACCPT:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsAccepted = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_GEN_CRS_FORW:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsForwarded = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_GEN_CRS_DELVR:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDelivered = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_GEN_CRS_DEL:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDeleted = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_MAX_CRS_RATE_EXC:
            BPLib_AS_NodeCountersPayload.BundleCountMaxCrsRateExceeded = *((uint32_t*) DesiredValuePtr);
            break;
        case NODE_STARTUP_COUNTER:
            BPLib_AS_NodeCountersPayload.NodeStartupCounter = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_GEN_CRS:
            BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrs = *((uint32_t*) DesiredValuePtr);
            break;
        case BUNDLE_CNT_RECV_CRS:
            BPLib_AS_NodeCountersPayload.BundleCountReceivedCrs = *((uint32_t*) DesiredValuePtr);
            break;
        case ADU_COUNT_DELIVERED:
            BPLib_AS_NodeCountersPayload.AduCountDelivered = *((uint32_t*) DesiredValuePtr);
            break;
        case ADU_COUNT_RECEIVED:
            BPLib_AS_NodeCountersPayload.AduCountReceived = *((uint32_t*) DesiredValuePtr);
            break;
        case TIME_BOOT_ERA:
            BPLib_AS_NodeCountersPayload.TimeBootEra = *((uint32_t*) DesiredValuePtr);
            break;
        case TIME_MONOTONIC_CNT:
            BPLib_AS_NodeCountersPayload.MonotonicTime = *((int64_t*) DesiredValuePtr);
            break;
        case TIME_CF:
            BPLib_AS_NodeCountersPayload.CorrelationFactor = *((int64_t*) DesiredValuePtr);
            break;
        default:
            Status = BPLIB_INVALID_NODE_CNTR_INDEX;
            break;
    }
    /* else
    {
        switch (CounterIndex)
        {
            // Source packet counter modification
        }
    }
    */

    return Status;
}

BPLib_Status_t BPLib_AS_Increment(BPLib_AS_CounterPacket_t CounterType, BPLib_AS_NodeCounter_t CounterIndex)
{
    BPLib_Status_t Status;

    if (CounterIndex == TIME_MONOTONIC_CNT || CounterIndex == TIME_CF)
    {
        uint32_t* SetVal  = (uint32_t*) BPLib_AS_Get(CounterType, CounterIndex);
        uint32_t  IncremVal = *SetVal++;

        Status = BPLib_AS_Set(CounterType, CounterIndex, (void*) IncremVal);
    }
    else
    {
        int64_t* SetVal    = (int64_t*) BPLib_AS_Get(CounterType, CounterIndex);
        int64_t  IncremVal = *SetVal++;

        Status = BPLib_AS_Set(CounterType, CounterIndex, (void*) IncremVal);
    }

    return Status;
}

BPLib_Status_t BPLib_AS_SetAllZero()
{
    // Call BPLib_AS_Set() on all vals in packet
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_AS_Write()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

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
