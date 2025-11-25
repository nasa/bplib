/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */

#ifndef BPLIB_PDB_H
#define BPLIB_PDB_H

/*
** Include
*/

#include "bplib_api_types.h"
#include "bplib_cfg.h"
#include "bplib_eid.h"

/*
** Authorized Source EID Table
*/
typedef struct
{
    BPLib_EID_Pattern_t AuthSrcEIDs[BPLIB_MAX_AUTH_SOURCE_EIDS];
} BPLib_PDB_SrcAuthTable_t;

/*
** Custodian Authorization Policy Table
*/
typedef struct
{
    BPLib_EID_Pattern_t AuthCustodians[BPLIB_MAX_AUTH_CUSTODIAN_EIDS];
} BPLib_PDB_CustodianTable_t;

/*
** Custody Authorization Policy Table
*/
typedef struct
{
    BPLib_EID_Pattern_t AuthCustodySrc[BPLIB_MAX_AUTH_CUSTODY_SOURCE_EIDS];
} BPLib_PDB_CustodyTable_t;

/*
** Report-To Authorization EID Table
*/
typedef struct
{
    BPLib_EID_Pattern_t AuthReportToEIDs[BPLIB_MAX_AUTH_REPORT_TO_EIDS];
} BPLib_PDB_ReportToTable_t;

/*
** Source Latency Table
*/
typedef enum
{
    Low =  0,
    Med =  1,
    High = 2,
} BPLib_PDB_SrcLatency;

/**
 * \brief Source Latency Table Entry
 */
typedef struct
{
    BPLib_EID_Pattern_t SrcEID;   ///< Source EID pattern to match
    uint32_t            Latency;  ///< Maximum latency (ms) for the matching EIDs
    uint32_t            Spare;
}BPLib_PDB_SrcLatencySet_t;

typedef struct
{
    BPLib_PDB_SrcLatencySet_t LatencySet[BPLIB_MAX_NUM_LATENCY_POLICY_SETS];
} BPLib_PDB_SrcLatencyTable_t;

/*
** Exported Functions
*/

/**
 * \brief Policy Database initialization
 *
 *  \par Description
 *       PDB initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
int BPLib_PDB_Init(void);

/**
 * \brief Validate Custodian Auth Table configurations
 *
 *  \par Description
 *       Validate configuration table parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management. 
 *         Every time a new Custodian Authorized table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 */
BPLib_Status_t BPLib_PDB_CustodianAuthTblValidateFunc(void *TblData);


/**
 * \brief Validate Custody Auth Table configurations
 *
 *  \par Description
 *       Validate configuration table parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management. 
 *         Every time a new Custody Authorized table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 */
BPLib_Status_t BPLib_PDB_CustodyAuthTblValidateFunc(void *TblData);


/**
 * \brief Validate ReportTo Auth Table configurations
 *
 *  \par Description
 *       Validate configuration table parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management. 
 *         Every time a new Report-To table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 */
BPLib_Status_t BPLib_PDB_ReportToAuthTblValidateFunc(void *TblData);


/**
 * \brief Validate Source Auth Table configurations
 *
 *  \par Description
 *       Validate configuration table parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management. 
 *         Every time a new Authorized Source table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 */
BPLib_Status_t BPLib_PDB_SrcAuthTblValidateFunc(void *TblData);


/**
 * \brief Validate configurations
 *
 *  \par Description
 *       Validate configuration table parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management. 
 *         Every time a new Source Latency table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 */
BPLib_Status_t BPLib_PDB_SrcLatencyTblValidateFunc(void *TblData);


#endif /* BPLIB_PDB_H */
