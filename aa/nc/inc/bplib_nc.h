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
    BPLIB_NC_APP_STATE_REMOVED = 0,
    BPLIB_NC_APP_STATE_STOPPED = 1,
    BPLIB_NC_APP_STATE_ADDED   = 2,
    BPLIB_NC_APP_STATE_STARTED = 3,
} BPLib_NC_ApplicationState_t;

/**
  * \brief Indicator for type of table
  */
typedef enum
{
    CHANNEL_CONFIG            =  0, /* Channel Configuration table */
    CONTACTS                  =  1, /* Contacts table */
    COMPRESSED_REPORTING      =  2, /* Compressed Reporting table */
    CUSTODIAN_AUTH_POLICY     =  3, /* Custodian Authorization Policy table */
    CUSTODY_AUTH_POLICY       =  4, /* Custody Authorization Policy table */
    MIB_CONFIG_PER_NODE       =  5, /* MIB Configuration per Node table */
    MIB_CONFIG_PER_SRC        =  6, /* MIB Configuration per Source table */
    REPORT_TO_EID_AUTH_POLICY =  7, /* Report-to-EID Authorization Policy table */
    SRC_AUTH_POLICY           =  8, /* Source Authorization Policy table */
    SRC_LATENCY_POLICY        =  9, /* Source Latency Policy table */
    STORAGE                   = 10, /* Storage table */
    ADU_PROXY_CONFIG          = 11, /* FWP's ADU Proxy Configuration table; confined to BPNode */
} BPLib_NC_TableType_t;

/* =================== */
/* Function Prototypes */
/* =================== */

/**
  * \brief     Initialize NC
  * \details   Node Configuration initialization
  * \param[in] ConfigPtrs (BPLib_FWP_ConfigPtrs_t*) Pointer to configurations for BPLib populated by BPNode
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Initialization was successful
  * \retval    BPLIB_FWP_CONFIG_PTRS_INIT_ERROR: At least one passed in configuration/table is NULL
  */
BPLib_Status_t BPLib_NC_Init(BPLib_FWP_ConfigPtrs_t* ConfigPtrs);

/**
 * \brief Validate MIB Per Node Configuration Table configurations
 *
 * \par Description
 *       Validate configuration table parameters
 *
 * \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management.
 *         Every time a new MIB Configuration Per Node table is loaded, this function should be called to
 *         validate its parameters.
 *
 * \param[in] TblData Pointer to the config table
 *
 * \return Execution status
 * \retval BPLIB_SUCCESS Validation was successful
 * \retval BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE: table parameters are out of range
*/
BPLib_Status_t BPLib_NC_MIBConfigPNTblValidateFunc(void *TblData);


/**
 * \brief Validate MIB Per Source Table configurations
 *
 * \par Description
 *      Validate configuration table parameters
 *
 * \par Assumptions, External Events, and Notes:
 *      - This function is called by whatever external task handles table management.
 *        Every time a new MIB Configuration Per Source table is loaded, this function should be called to
 *        validate its parameters.
 *
 * \param[in] TblData Pointer to the config table
 *
 * \return Execution status
 * \retval BPLIB_SUCCESS Validation was successful
 * \retval BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE: table parameters are out of range
 */
BPLib_Status_t BPLib_NC_MIBConfigPSTblValidateFunc(void *TblData);

void BPLib_NC_SetAppState(uint8_t ChanId, BPLib_NC_ApplicationState_t State);

BPLib_NC_ApplicationState_t BPLib_NC_GetAppState(uint8_t ChanId);

/**
  * \brief     Pass updated table configurations to corresponding modules
  * \details   Uses FWP to update the BPLib table pointers, then pass those updated pointer tables to the
  *            module that controls that table
  * \note      As of right now, the API calls to modules that will update tables are commented out, since
  *            some of those functions are not implemented yet
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Initialization was successful
  * \retval    BPLIB_NC_FWP_TBL_WAKEUP_ERR: Something went wrong while attempting to update the table pointer with TABLEP
  * \retval    BPLIB_NC_TBL_WAKEUP_ERR: Something went wrong while attempting to update the tables with module API calls
  */
BPLib_Status_t BPLib_NC_TableWakeUp(void);

#endif // BPLIB_NC_H