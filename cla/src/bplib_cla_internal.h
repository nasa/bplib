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

/* ======== */
/* Includes */
/* ======== */

#include "bplib_cla.h"

/* ======== */
/* Typedefs */
/* ======== */

/* This ID set is used to maintain a BundleID and SessionID map*/
typedef struct
{
    uint32_t  BundleID;
    uint32_t  SessionID;
} BPLib_CLA_IDSet_t;

/* =========== */
/* Global Data */
/* =========== */

/**
  * \brief Array use to track the run states of all active contacts. The run states
  *        are associated with the contact ID via the index into the array.
  *        Ex: BPLib_CLA_ContactRunStates[0] is the contact run state for contact ID 0
  */
extern BPLib_CLA_ContactRunState_t BPLib_CLA_ContactRunStates[];

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
  * \retval    BPLIB_CLA_INVALID_CONTACT_ID: Provided contact ID does not match a contact ID in the Contacts Configuration
  */
 BPLib_Status_t BPLib_CLA_SetContactRunState(uint32_t ContactId, BPLib_CLA_ContactRunState_t RunState);

#endif /* BPLIB_CLA_INTERNAL_H */