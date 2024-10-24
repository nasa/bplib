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

#ifndef BPLIB_PDB_H
#define BPLIB_PDB_H

/*
** Include
*/

#include "bplib_api_types.h"
#include "bplib_cfg.h"


/*
** Authorized Source EID Table
*/
typedef struct
{
    char AuthSrcEIDs[BPLIB_MAX_AUTH_SOURCES][BPLIB_MAX_EID_LENGTH];
} BPLib_PD_SrcAuthTable_t;

/*
** Custodian Authorization Policy Table
*/
typedef struct
{
    char AuthCustodians[BPLIB_MAX_AUTH_SOURCES][BPLIB_MAX_EID_LENGTH];
} BPLib_PD_CustodianTable_t;

/*
** Custody Authorization Policy Table
*/
typedef struct
{
    char AuthCustodySrc[BPLIB_MAX_AUTH_SOURCES][BPLIB_MAX_EID_LENGTH];
} BPLib_PD_CustodyTable_t;

/*
** Report-To Authorization EID Table
*/
typedef struct
{
    char AuthReportToEIDs[BPLIB_MAX_AUTH_SOURCES][BPLIB_MAX_EID_LENGTH];
} BPLib_PD_ReportToTable_t;

/*
** Source Latency Table
*/
typedef enum
{
    Low,
    Med,
    High,
} SrcLatency;

typedef struct
{
    char        SrcEID[BPLIB_MAX_EID_LENGTH];
    uint32_t    Latency;
    uint32_t    Spare;    
}BPLib_PD_SrcLatencySet_t;

typedef struct
{
    BPLib_PD_SrcLatencySet_t LatencySet[BPLIB_MAX_NUM_LATENCY_SRC];
} BPLib_PD_SrcLatencyTable_t;

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
 *         Every time a new channel table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 */BPLib_Status_t BPLib_PD_CustodianAuthTblValidateFunc(void *TblData);


/**
 * \brief Validate Custody Auth Table configurations
 *
 *  \par Description
 *       Validate configuration table parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management. 
 *         Every time a new channel table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 */BPLib_Status_t BPLib_PD_CustodyAuthTblValidateFunc(void *TblData);


/**
 * \brief Validate ReportTo Auth Table configurations
 *
 *  \par Description
 *       Validate configuration table parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management. 
 *         Every time a new channel table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 */BPLib_Status_t BPLib_PD_ReportToAuthTblValidateFunc(void *TblData);


/**
 * \brief Validate Source Auth Table configurations
 *
 *  \par Description
 *       Validate configuration table parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management. 
 *         Every time a new channel table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 */BPLib_Status_t BPLib_PD_SrcAuthTblValidateFunc(void *TblData);


/**
 * \brief Validate configurations
 *
 *  \par Description
 *       Validate configuration table parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management. 
 *         Every time a new channel table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 */BPLib_Status_t BPLib_PD_SrcLatencyTblValidateFunc(void *TblData);


#endif /* BPLIB_PDB_H */
