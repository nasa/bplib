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

/**
 * @file
 *
 * Private header file for internal CLA functions
 */


#ifndef BPLIB_CLA_INTERNAL_H
#define BPLIB_CLA_INTERNAL_H

/* ======== */
/* Includes */
/* ======== */

#include "bplib_cla.h"

extern volatile BPLib_CLA_ContactRunState_t BPLib_CLA_ContactRunStates[BPLIB_MAX_NUM_CONTACTS];

/* ======== */
/* Typedefs */
/* ======== */

/* This ID set is used to maintain a BundleID and SessionID map*/
typedef struct
{
    uint32_t  BundleID;
    uint32_t  SessionID;
} BPLib_CLA_IDSet_t;

/* =================== */
/* Function Prototypes */
/* =================== */

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
 *  \param[in] Size Message length
 * 
 *  \retval boolean true/false
 */
bool BPLib_CLA_IsAControlMsg(const void *MsgPtr, size_t Size);

/**
 * \brief Process received control messages from CLs
 *
 *  \par Description
 *       Process received control messages from CLs
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] CtrlMsgPtr Pointer for the control message
 * 
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Operation was successful
 */
BPLib_Status_t BPLib_CLA_ProcessControlMessage(BPLib_CLA_CtrlMsg_t* CtrlMsgPtr);

/**
  * \brief     Set the run state of the provided contact
  * \param[in] ContactId (uint32_t) Contact ID from the Contacts Configuration whose run state is being requested to change
  * \param[in] RunState  (BPLib_CLA_ContactRunState_t) Requested run state of the provided contact ID from the Contacts Configuration
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Successfully changed the run state of the provided contact ID to the provided run state
  * \retval    BPLIB_INVALID_CONT_ID_ERR: Provided contact ID does not match a contact ID in the Contacts Configuration
  */
 BPLib_Status_t BPLib_CLA_SetContactRunState(uint32_t ContactId, BPLib_CLA_ContactRunState_t RunState);

#endif /* BPLIB_CLA_INTERNAL_H */