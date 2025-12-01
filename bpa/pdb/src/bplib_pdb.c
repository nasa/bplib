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

/*
** Include
*/

#include "bplib_pdb.h"


/*
** Function Definitions
*/

int BPLib_PDB_Init(void) {
    return BPLIB_SUCCESS;
}

/* Validate Custodian Auth table data */
BPLib_Status_t BPLib_PDB_CustodianAuthTblValidateFunc(void *TblData)
{
    BPLib_Status_t           ReturnCode = BPLIB_SUCCESS;

    /* Validate data values are within allowed range */

    return ReturnCode;
}

/* Validate Custody Auth table data */
BPLib_Status_t BPLib_PDB_CustodyAuthTblValidateFunc(void *TblData)
{
    BPLib_Status_t           ReturnCode = BPLIB_SUCCESS;

    /* Validate data values are within allowed range */

    return ReturnCode;
}

/* Validate Report-To Auth table data */
BPLib_Status_t BPLib_PDB_ReportToAuthTblValidateFunc(void *TblData)
{
    BPLib_Status_t           ReturnCode = BPLIB_SUCCESS;

    return ReturnCode;
}

/* Validate Source Auth table data */
BPLib_Status_t BPLib_PDB_SrcAuthTblValidateFunc(void *TblData)
{
    BPLib_Status_t           ReturnCode = BPLIB_SUCCESS;

    return ReturnCode;
}

/* Validate Source Latency table data */
BPLib_Status_t BPLib_PDB_SrcLatencyTblValidateFunc(void *TblData)
{
    BPLib_Status_t           ReturnCode = BPLIB_SUCCESS;

    return ReturnCode;
}