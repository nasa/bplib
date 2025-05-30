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

#ifndef BPLIB_AS_INTERNAL_H
#define BPLIB_AS_INTERNAL_H

/* ======== */
/* Includes */
/* ======== */

#include "bplib_api_types.h"
#include "bplib_as.h"
#include "bplib_eventids.h"
#include "osapi.h"
#include "bplib_eid.h"

/* ======= */
/* Externs */
/* ======= */

extern BPLib_NodeMibCountersHkTlm_Payload_t    BPLib_AS_NodeCountersPayload;        /** \brief Global node MIB counter payload */
extern BPLib_SourceMibCountersHkTlm_Payload_t  BPLib_AS_SourceCountersPayload;      /** \brief Global source MID counter payload */
extern BPLib_NodeMibReportsHkTlm_Payload_t     BPLib_AS_NodeReportsPayload;          /** \brief Global node MID reports payload */

/* ====== */
/* Macros */
/* ====== */
#define BPLIB_AS_MAX_MUTEX_NAME_SIZE (20u) /** \brief Max allowed length for AS counter mutex name */

/* =============== */
/* Mutex Variables */
/* =============== */
extern osal_id_t MutexId;
extern char      MutexName[BPLIB_AS_MAX_MUTEX_NAME_SIZE];

/* =================== */
/* Function Prototypes */
/* =================== */

/**
 * \brief     Returns counter value for the provided EID
 * \note      Source counters are unimplemented and return 0 by default
 * \param[in] EID Endpoint identifier
 * \param[in] Counter Enumeration of counter to return
 * \return    Counter value
 * \retval    A counter value of 0 could be the real value or indicate a silent validation 
 *            error
 */
uint32_t BPLib_AS_GetCounterImpl(BPLib_EID_t *EID, BPLib_AS_Counter_t Counter);

/**
 * \brief     Set given counter associated with given EID to given value
 * \details   Reference the BPLib_AS_Counter_t struct to see what each counter represents
 * \note      Directly sets counter in counter payloads to given value
 * \param[in] EID     (BPLib_EID_t) EID of node whose counter should be set
 * \param[in] Counter (BPLib_AS_Counter_t) Counter to reset
 * \param[in] Value   (uint32_t) Numerical value to assign to the counter
 * \return    Execution status
 * \retval    BPLIB_INVALID_EID: Source EID did not pass criteria in BPLib_EID_IsValid()
 * \retval    BPLIB_AS_UNKNOWN_NODE_CNTR: Counter is outside node counters' range
 * \retval    BPLIB_AS_UNKNOWN_SRC_CNTR: Counter is outside source counters' range
 * \retval    BPLIB_SUCCESS: Successful execution
 * \ref       BPLib_AS_Counter_t
 * \ref       BPLib_SourceMibCountersHkTlm_Payload_t
 * \ref       BPLib_NodeMibCountersHkTlm_Payload_t
 * \ref       BPLib_EID_IsValid [BPLib_EID_IsValid()]
 */
BPLib_Status_t BPLib_AS_SetCounter(BPLib_EID_t EID, BPLib_AS_Counter_t Counter, uint32_t Value);

/**
 * \brief     Initialize the mutex that guards the node and source MIB counters
 * \details   Uses OS_MutSemCreate() with the internal MutexId and MutexName
 * \note      This function does not handle errors, it only returns the error status
 * \return    Execution status
 * \retval    BPLIB_AS_INIT_MUTEX_ERR: An error ocurred while creating the mutex
 * \retval    BPLIB_SUCCESS: Mutex successfully created
 * \anchor    BPLib_AS_InitMutex [BPLib_AS_InitMutex()]
 */
BPLib_Status_t BPLib_AS_InitMutex(void);

/**
 * \brief     Take from the mutex guarding the node and source MIB counters
 * \details   Uses OS_MutSemTake() with the internal MutexId and handles errors
 * \note      Specific errors aren't handled, just a general non-success case
 * \return    void
 * \anchor    BPLib_AS_LockCounters [BPLib_AS_LockCounters()]
 */
void BPLib_AS_LockCounters(void);

/**
 * \brief     Give to the mutex guarding the node and source MIB counters
 * \details   Uses OS_MutSemGive() with the internal MutexId and handles errors
 * \note      Specific errors aren't handled, just a general non-success case
 * \return    void
 * \anchor    BPLib_AS_UnlockCounters [BPLib_AS_UnlockCounters()]
 */
void BPLib_AS_UnlockCounters(void);

/**
 * \brief     Set the initial node MIB reports telemetry
 * \return    void
 */
void BPLib_AS_InitializeReportsHkTlm(void);

#endif // BPLIB_AS_INTERNAL_H