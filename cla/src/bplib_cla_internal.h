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

/**
 * @file
 *
 * Private header file for internal CLA functions
 */


#ifndef BPLIB_CLA_INTERNAL_H
#define BPLIB_CLA_INTERNAL_H

/*
** Include
*/
#include "bplib_cla.h"


/*
** Global Data
*/
/* This ID set is used to maintain a BundleID and SessionID map*/
typedef struct
{
    uint32_t  BundleID;
    uint32_t  SessionID;
}BPLib_CLA_IDSet_t;


/*
** Exported Functions
*/

/**
 * \brief Check the received bundle if it is a control message 
 *
 *  \par Description
 *       Check the received bundle if it is a control message
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] candidate bundle pointer
 * 
 *  \retval boolean true/false
 */
bool BPLib_CLA_IsAControlMsg(const void *Bundle);


/**
 * \brief Process received control messages from CLs
 *
 *  \par Description
 *       Process received control messages from CLs
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] none for now
 * 
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Operation was successful
 */
int BPLib_CLA_ProcessControlMessage(void);


#endif /* BPLIB_CLA_INTERNAL_H */

