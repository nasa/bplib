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
#include "bplib_pi.h"
#include "bplib_cla.h"
#include "bplib_arp.h"
#include "bplib_pdb.h"
#include "bplib_fwp.h"

/* ======== */
/* Typedefs */
/* ======== */

typedef struct
{
    BPLib_PI_ChannelTable_t*     ChanTblPtr;
    BPLib_CLA_ContactsTable_t*   ContactsTblPtr;
    BPLib_ARP_CRSTable_t*        CrsTblPtr;
    BPLib_PDB_CustodianTable_t*  CustodianTblPtr;
    BPLib_PDB_CustodyTable_t*    CustodyTblPtr;
    BPLib_NC_MIBConfigPNTable_t* MibPnTblPtr;
    BPLib_NC_MIBConfigPSTable_t* MibPsTblPtr;
    BPLib_PDB_ReportToTable_t*   ReportTblPtr;
    BPLib_PDB_SrcAuthTable_t*    AuthTblPtr;
    BPLib_PDB_SrcLatencyTable_t* LatTblPtr;
    BPLib_STOR_StorageTable_t*   StorTblPtr;
} BPLib_NC_ConfigPtrs_t;

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
  * \brief Indicator for type of configuration
  */
typedef enum
{
    BPLIB_CHANNEL                   =  0, /* Channel configuration */
    BPLIB_CONTACTS                  =  1, /* Contacts configuration */
    BPLIB_COMPRESSED_REPORTING      =  2, /* Compressed Reporting configuration */
    BPLIB_CUSTODIAN_AUTH_POLICY     =  3, /* Custodian Authorization Policy configuration */
    BPLIB_CUSTODY_AUTH_POLICY       =  4, /* Custody Authorization Policy configuration */
    BPLIB_MIB_PER_NODE              =  5, /* MIB per Node configuration */
    BPLIB_MIB_PER_SRC               =  6, /* MIB per Source configuration */
    BPLIB_REPORT_TO_EID_AUTH_POLICY =  7, /* Report-to-EID Authorization Policy configuration */
    BPLIB_SRC_AUTH_POLICY           =  8, /* Source Authorization Policy configuration */
    BPLIB_SRC_LATENCY_POLICY        =  9, /* Source Latency Policy configuration */
    BPLIB_STORAGE                   = 10, /* Storage configuration */
    BPLIB_ADU_PROXY                 = 11, /* FWP's ADU Proxy configuration; confined to BPNode */
} BPLib_NC_ConfigType_t;

/* =========== */
/* Global Data */
/* =========== */

extern BPLib_NC_ConfigPtrs_t BPLib_NC_ConfigPtrs;

/* =================== */
/* Function Prototypes */
/* =================== */

/**
  * \brief     Initialize NC
  * \details   Node Configuration initialization
  * \param[in] ConfigPtrs (BPLib_NC_ConfigPtrs_t*) Pointer to configurations for BPLib populated by BPNode
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Initialization was successful
  * \retval    BPLIB_FWP_CONFIG_PTRS_INIT_ERROR: At least one passed in configuration is NULL
  */
BPLib_Status_t BPLib_NC_Init(BPLib_NC_ConfigPtrs_t* ConfigPtrs);

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
  * \brief     Pass updated configurations to corresponding modules
  * \details   Uses FWP to update the BPLib configuration pointers, then pass those updated pointer
  *            configurations to the module that controls that configuration
  * \note      As of right now, the API calls to modules that will update configurations are commented out, since
  *            some of those functions are not implemented yet
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Successful execution without updates to configurations
  * \retval    BPLIB_TBL_UPDATED: Successful execution with configuration updates
  * \retval    BPLIB_ERROR: An error occured while attempting to refresh/update configurations
  */
BPLib_Status_t BPLib_NC_ConfigUpdate(void);

#endif // BPLIB_NC_H