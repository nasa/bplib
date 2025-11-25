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

#ifndef BPLIB_ARP_H
#define BPLIB_ARP_H

/*
** Includes
*/

#include "bplib_api_types.h"
#include "bplib_cfg.h"
#include "bplib_eid.h"

/*
** CRS Table
*/
typedef struct
{
    BPLib_EID_t DestEID;
    uint32_t    TimeTrigger;
    uint32_t    SizeTrigger;
} BPLib_ARP_CRSSet_t;

typedef struct
{
    BPLib_ARP_CRSSet_t CRS_Set[BPLIB_MAX_NUM_CRS];
} BPLib_ARP_CRSTable_t;

/*
** Exported Functions
*/

/**
 * \brief Admin Records Processor initialization
 *
 *  \par Description
 *       ARP initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
int BPLib_ARP_Init(void);

/**
 * \brief Validate CRS Table Table configurations
 *
 *  \par Description
 *       Validate configuration table parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management. 
 *         Every time a new CRS table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 * \retval    BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE: table parameters are out of range
 */
BPLib_Status_t BPLib_ARP_CRSTblValidateFunc(void *TblData);

#endif /* BPLIB_ARP_H */
