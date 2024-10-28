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

BPLib_Status_t BPLib_AS_Set(BPLib_AS_NodeCounter_t CounterIndex, int64_t DesiredValue)
{
    BPLib_Status_t Status;
    Status = BPLIB_SUCCESS;

    switch (CounterIndex)
    {
        case BUNDLE_CNT_GEN_ANONYMOUS:
            break;
        case SYSTEM_NODE_UP_TIME:
            break;
        case ACCEPTED_DIRECTIVE_CNT:
            break;
        case REJECTED_DIRECTIVE_CNT:
            break;
        case BUNDLE_CNT_GEN_CUSTODY:
            break;
        case BUNDLE_CNT_GEN_BSR_RECV:
            break;
        case BUNDLE_CNT_GEN_BSR_ACCPT:
            break;
        case BUNDLE_CNT_GEN_BSR_FORW:
            break;
        case BUNDLE_CNT_GEN_BSR_DELVR:
            break;
        case BUNDLE_CNT_GEN_BSR_DEL:
            break;
        case BUNDLE_CNT_INVAL_PRI_BLK:
            break;
        case BUNDLE_CNT_CS_RECV:
            break;
        case BUNDLE_CNT_DEL_UNAUTH_SRC:
            break;
        case BUNDLE_CNT_GEN_CRS_RECV:
            break;
        case BUNDLE_CNT_GEN_CRS_ACCPT:
            break;
        case BUNDLE_CNT_GEN_CRS_FORW:
            break;
        case BUNDLE_CNT_GEN_CRS_DELVR:
            break;
        case BUNDLE_CNT_GEN_CRS_DEL:
            break;
        case BUNDLE_CNT_MAX_CRS_RATE_EXC:
            break;
        case NODE_STARTUP_COUNTER:
            break;
        case BUNDLE_CNT_GEN_CRS:
            break;
        case BUNDLE_CNT_RECV_CRS:
            break;
        case ADU_COUNT_DELIVERED:
            break;
        case ADU_COUNT_RECEIVED:
            break;
        case TIME_BOOT_ERA:
            break;
        case TIME_MONOTONIC_CNT:
            break;
        case TIME_CF:
            break;
        default:
            break;
    }

    return Status;
}

BPLib_Status_t BPLib_AS_Increment(int32_t index)
{
    // BPLib_AS_Set(val, val + 1)
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_AS_SetZero()
{
    // BPLib_AS_Set(val, 0)
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

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
