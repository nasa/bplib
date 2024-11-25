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

/* =================== */
/* Function Prototypes */
/* =================== */

/**
 * \brief     Checks if the source EID matches an expected, valid pattern
 * \details   Eventually some more advanced checks will occur with something like regex.
 * \note      The source EID is not expected to be int16_t typed data but it is for now, as a placeholder
 * \param[in] SourceEid (int16_t) Indicator of which source in the BPLib_SourceMibCountersHkTlm_Payload_t::MibArray
 *                      array should be modified
 * \return    Source EID validity
 * \retval    true: The source EID matches the criteria required to be valid <enter criteria here>
 * \retval    false: The source EID is invalid in some way
 * \secreflist
 * \refitem   BPLib_SourceMibCountersHkTlm_Payload_t
 * \endsecreflist
 * \anchor    BPLib_AS_EidIsValid
 */
bool BPLib_AS_EidIsValid(int16_t SourceEid);

/**
  * \brief      Allows the caller to get access to certain values in counter packets specified by the parameters.
  *             This is necessary to simplify the code and avoid walls of switch-case statements
  * \details    Accessor function for counters used by Admin Statistics
  * \note       This function assumes all modifiable counter types are uint32_t
  * \note       This function does not handle error events since it's entirely internal
  * \param[in]  SourceEid (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::MibArray
  *                       array.
  * \param[in]  Counter   (BPLib_AS_Counter_t) Counter to access
  * \param[out] ReturnPtr (uint32_t*) What the counter value requested is stored in
  * \return     Execution status
  * \retval     BPLIB_AS_INVALID_EID: Source EID did not pass criteria in BPLib_AS_EidIsValid()
  * \retval     BPLIB_AS_UNKNOWN_NODE_CNTR: The node-specific counter did not match a recognized value
  * \retval     BPLIB_AS_UNKNOWN_SRC_CNTR: The source-specific counter did not match a recognized value
  * \retval     BPLIB_SUCCESS: Successful execution
  * \secreflist
  * \refitem   BPLib_SourceMibCountersHkTlm_Payload_t
  * \refitem   BPLib_AS_Counter_t
  * \refitem   BPLib_AS_EidIsValid [BPLib_AS_EidIsValid()]
  * \endsecreflist
  * \anchor     BPLib_AS_Get
  */
BPLib_Status_t BPLib_AS_Get(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t* ReturnPtr);

/**
 * \brief     Modify the counter specified by the source EID and counter to the desired value
 * \details   Mutator function for counters used by Admin Statistics
 * \note      This function assumes all modifiable counters are of type uint32_t
 * \note      This function does not handle error events since it's internal
 * \param[in] SourceEid (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::MibArray array.
 * \param[in] Counter   (BPLib_AS_Counter_t) Counter to set
 * \param[in] Value     (uint32_t) Value that the caller wishes to set the counter represented by Counter and SourceEid
 * \return    Execution status
 * \retval    BPLIB_AS_INVALID_EID: Source EID did not pass criteria in BPLib_AS_EidIsValid()
 * \retval    BPLIB_AS_UNKNOWN_NODE_CNTR: The node-specific counter did not match a recognized value
 * \retval    BPLIB_AS_UNKNOWN_SRC_CNTR: The source-specific counter did not match a recognized value
 * \retval    BPLIB_SUCCESS: Successful execution
 * \secreflist
 * \refitem   BPLib_SourceMibCountersHkTlm_Payload_t
 * \refitem   BPLib_AS_Counter_t
 * \refitem   BPLib_AS_EidIsValid [BPLib_AS_EidIsValid()]
 * \endsecreflist
 * \anchor    BPLib_AS_Set
 */
BPLib_Status_t BPLib_AS_Set(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Value);

#endif // BPLIB_AS_INTERNAL_H