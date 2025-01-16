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

#include "bplib_as_internal.h"
#include "bplib_as.h"

/* ================= */
/* Telemetry Packets */
/* ================= */

BPLib_NodeMibCountersHkTlm_Payload_t   BPLib_AS_NodeCountersPayload;   /** \brief Global node MIB counter payload */
BPLib_SourceMibCountersHkTlm_Payload_t BPLib_AS_SourceCountersPayload; /** \brief Global source MID counter payload */

/* =============== */
/* Mutex Variables */
/* =============== */
osal_id_t MutexId;

/* ==================== */
/* Function Definitions */
/* ==================== */

bool BPLib_AS_EidIsValid(int16_t SourceEid)
{
    return (SourceEid >= 0 && SourceEid < BPLIB_MAX_NUM_SOURCE_EID);
}

BPLib_Status_t BPLib_AS_SetCounter(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Value)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (BPLib_AS_EidIsValid(SourceEid))
    {
        if ((int8_t) Counter > -1 && (int8_t) Counter < BPLIB_AS_NUM_NODE_CNTRS)
        { // Counter is within range
            BPLib_AS_NodeCountersPayload.NodeCounters[Counter] = Value;

            /*
            if (Counter > -1 && Counter < BPLIB_AS_NUM_SOURCE_CNTRS)
            { // Counter is within range
                BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[Counter] = Value;
            }
            else
            { // Counter is out of valid range
                Status = BPLIB_AS_UNKNOWN_SRC_CNTR;
            }
            */
        }
        else
        { // Counter is out of valid range
            Status = BPLIB_AS_UNKNOWN_NODE_CNTR;
        }
    }
    else
    {
        Status = BPLIB_AS_INVALID_EID;
    }

    return Status;
}

BPLib_Status_t BPLib_AS_InitMutex(void)
{
    uint32 OS_Status;
    BPLib_Status_t Status;
    char MutexName[BPLIB_AS_MAX_MUTEX_NAME_SIZE];

    MutexId = OS_OBJECT_ID_UNDEFINED;
    strncpy(MutexName, "BPLib_AS_CounterMut", BPLIB_AS_MAX_MUTEX_NAME_SIZE);

    /* Instantiate a mutex for AS counters */
    OS_Status = OS_MutSemCreate(&MutexId, MutexName, 0);

    /* Translate mutex status into BPLib_Status_t */
    if (OS_Status == OS_SUCCESS)
    {
        Status = BPLIB_SUCCESS;
    }
    else
    {
        Status = BPLIB_AS_INIT_MUTEX_ERR;
    }

    return Status;
}

void BPLib_AS_LockCounters(void)
{
    uint32 OS_Status;

    OS_Status = OS_MutSemTake(MutexId);
    if (OS_Status != OS_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_AS_TAKE_MUTEX_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Failed to take from the counter mutex, RC = %d",
                            OS_Status);
    }
}

void BPLib_AS_UnlockCounters(void)
{
    uint32 OS_Status;

    OS_Status = OS_MutSemGive(MutexId);
    if (OS_Status != OS_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_AS_GIVE_MUTEX_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Failed to give to the counter mutex, RC = %d",
                            OS_Status);
    }
}