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

#ifndef BPLIB_NC_H
#define BPLIB_NC_H

/* ======== */
/* Includes */
/* ======== */

#include "bplib_api_types.h"
#include "bplib_nc_payloads.h"
#include "bplib_nc_directives.h"
#include "bplib_as.h"

/* ======== */
/* Typedefs */
/* ======== */

/** 
  * \brief Channel application state
  */
typedef enum 
{
    BPLIB_NC_APP_STATE_STOPPED = 0,
    BPLIB_NC_APP_STATE_ADDED   = 1,
    BPLIB_NC_APP_STATE_STARTED = 2,
    BPLIB_NC_APP_STATE_REMOVED = 3,
} BPLib_NC_ApplicationState_t;

/* =================== */
/* Function Prototypes */
/* =================== */

/**
  * \brief     Initialize NC
  * \details   Node Configuration initialization
  * \note      As of right now, this function always returns BPLIB_SUCCESS
  * \param[in] void No arguments accepted
  * \return    Return status
  * \retval    BPLIB_SUCCESS: Command was successful
  */
BPLib_Status_t BPLib_NC_Init(void);

/**
 * \brief Validate MIB Per Node Configuration Table configurations
 *
 *  \par Description
 *       Validate configuration table parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management. 
 *         Every time a new MIB Configuration Per Node table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 *  \retval    BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE: table parameters are out of range
*/
BPLib_Status_t BPLib_NC_MIBConfigPNTblValidateFunc(void *TblData);


/**
 * \brief Validate MIB Per Source Table configurations
 *
 *  \par Description
 *       Validate configuration table parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management. 
 *         Every time a new MIB Configuration Per Source table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 * \retval    BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE: table parameters are out of range
 */
BPLib_Status_t BPLib_NC_MIBConfigPSTblValidateFunc(void *TblData);

void BPLib_NC_SetAppState(uint8_t ChanId, BPLib_NC_ApplicationState_t State);

BPLib_NC_ApplicationState_t BPLib_NC_GetAppState(uint8_t ChanId);

#endif // BPLIB_NC_H