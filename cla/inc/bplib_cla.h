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

#ifndef BPLIB_CLA_H
#define BPLIB_CLA_H

/* ======== */
/* Includes */
/* ======== */

#include <stdio.h>

#include "bplib_api_types.h"
#include "bplib_eventids.h"
#include "bplib_cfg.h"
#include "bplib_eid.h"
#include "bplib_qm.h"
#include "bplib_em.h"

/* ======== */
/* Typedefs */
/* ======== */

/* There are 4 types of control message types, received from CL*/
typedef enum
{
    SentIt              = 0, /* LTP Only, CL sends out */
    SessionComplete     = 1, /*For LTP, Other end received*/
    SessionCancelled    = 2, /* Session is cancelled, do nothing*/
    SessionStarted      = 3 /* Session started, do nothing*/
}BPLib_CLA_CtrlMsgTypes_t;

typedef enum
{
    UDPType = 0x00000000,
    TCPType = 0x00000001,
    EPPType = 0x00000002,
    LTPType = 0x00000003,
} BPLib_CLA_Type_t;

typedef struct
{
    char        CtrlMsgTag[8]; /* "BPNMSG" */
    uint32_t    SessionID;
    uint32_t    BundleID;
    BPLib_CLA_Type_t   ClaType;
    uint8_t     MsgTypes;
} BPLib_CLA_CtrlMsg_t;

typedef enum
{
    BPLIB_CLA_TORNDOWN = 0, /* Default to torn down */
    BPLIB_CLA_SETUP    = 1,
    BPLIB_CLA_STARTED  = 2,
    BPLIB_CLA_STOPPED  = 3,
    BPLIB_CLA_EXITED   = 4,
    BPLIB_CLA_PAUSED   = 5
} BPLib_CLA_ContactRunState_t;

/* =========== */
/* Global Data */
/* =========== */

/**
  * \brief Array use to track the run states of all active contacts. The run states
  *        are associated with the contact ID via the index into the array.
  *        Ex: BPLib_CLA_ContactRunStates[0] is the contact run state for contact ID 0
  */
 extern BPLib_CLA_ContactRunState_t BPLib_CLA_ContactRunStates[];

/**
 * \brief Global Contacts Configuration
 */
typedef struct
{
    BPLib_EID_Pattern_t DestEIDs[BPLIB_MAX_CONTACT_DEST_EIDS];
    BPLib_CLA_Type_t    CLAType;

    char                ClaInAddr[BPLIB_MAX_IP_LENGTH];
    char                ClaOutAddr[BPLIB_MAX_IP_LENGTH];
    uint16_t            ClaInPort;
    uint16_t            ClaOutPort;

    size_t              IngressBitsPerCycle;
    size_t              EgressBitsPerCycle;

    uint32_t            RetransmitTimeout;
    uint32_t            CSTimeTrigger;
    uint32_t            CSSizeTrigger;
} BPLib_CLA_ContactsSet_t;

typedef struct
{
    BPLib_CLA_ContactsSet_t ContactSet[BPLIB_MAX_NUM_CONTACTS];
} BPLib_CLA_ContactsTable_t;

/* =================== */
/* Function Prototypes */
/* =================== */

/* CLA I/O (bundle data units) */

/**
 * \brief CLA Ingress function
 *
 *  \par Description
 *       Receive bundle from CL and pass it Bundle Interface
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] Inst Pointer to a valid BPLib_Instance_t
 *  \param[in] ContId Contact ID
 *  \param[in] Bundle Pointer to bundle to ingress
 *  \param[in] Size Size of the bundle to ingress
 *  \param[in] Timeout Time for pending on CL
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS when BPLib_CLA_Ingress was successful
 */
BPLib_Status_t BPLib_CLA_Ingress(BPLib_Instance_t* Inst, uint32_t ContId,
                                    const void *Bundle, size_t Size, uint32_t Timeout);

/**
 * \brief CLA Egress function
 *
 *  \par Description
 *       Receive bundle from Bundle Interface and send it to CL
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] Inst Pointer to a valid BPLib_Instance_t
 *  \param[in] ContId Contact ID
 *  \param[in] BundleOut Pointer to put egressing bundle into
 *  \param[in] Size Size of the received bundle to return
 *  \param[in] BufLen Length of the buffer provided (the bundle pointer)
 *  \param[in] Timeout Time to pend on contact egress queue (in milliseconds)
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS when BPLib_CLA_Egress was successful
 */
BPLib_Status_t BPLib_CLA_Egress(BPLib_Instance_t* Inst, uint32_t ContId, void *BundleOut,
                                size_t *Size, size_t BufLen, uint32_t Timeout);

/**
 * \brief Validate Contacts Configuration
 *
 *  \par Description
 *       Validate configuration parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management.
 *         Every time a new Contacts Configuration is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 */
BPLib_Status_t BPLib_CLA_ContactsTblValidateFunc(void *TblData);

/**
 * \brief     Find the requested contact ID in the Contacts Configuration and pass that information to the
 *            CLA proxy to configure the CLA with
 * \param[in] ContactId (uint32_t) Contact ID from the Contacts Configuration to setup
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_CLA_UNKNOWN_CONTACT: Contact ID could not be found in the Contacts Configuration
 * \retval    BPLIB_CLA_CONTACTS_MAX_REACHED: Setting up contact would exceed maximum simultaneous
 *                                            contacts allowed
 */
BPLib_Status_t BPLib_CLA_ContactSetup(uint32_t ContactId);

/**
 * \brief     Pass Contact ID to start, on to CLA proxy
 * \param[in] ContactId (uint32_t) Contact ID from the Contacts Configuration to start
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_CLA_UNKNOWN_CONTACT: Provided contact ID does not match a contact ID in
 *                                       the Contacts Configuration
 * \retval    BPLIB_CLA_INCORRECT_STATE: CLA task not in the correct state for
 *                                       successful operation
 * \retval    BPLIB_INVALID_CONT_ID_ERR: Provided contact ID is invalid
 * \retval    BPLIB_CLA_IO_ERROR: A UDP conntection couldn't be set to running
 */
BPLib_Status_t BPLib_CLA_ContactStart(uint32_t ContactId);

/**
 * \brief     Pass Contact ID to stop, on to CLA proxy
 * \param[in] ContactId (uint32_t) Contact ID from the Contacts Configuration to stop
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_CLA_UNKNOWN_CONTACT: Provided contact ID does not match a contact ID in
 *                                       the Contacts Configuration
 * \retval    BPLIB_CLA_INCORRECT_STATE: CLA task not in the correct state for
 *                                       successful operation
 * \retval    BPLIB_INVALID_CONT_ID_ERR: Provided contact ID is invalid
 */
BPLib_Status_t BPLib_CLA_ContactStop(uint32_t ContactId);

/**
 * \brief     If the contact has been stopped, deconfigure the CLA via BI, CT, EBP, and CLA
 * \param[in] ContactId (uint32_t) Contact ID from the Contacts Configuration to teardown
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_CLA_UNKNOWN_CONTACT: Provided contact ID does not match a contact ID in
 *                                       the Contacts Configuration
 * \retval    BPLIB_CLA_INCORRECT_STATE: CLA task not in the correct state for
 *                                       successful operation
 * \retval    BPLIB_INVALID_CONT_ID_ERR: Provided contact ID is invalid
 */
BPLib_Status_t BPLib_CLA_ContactTeardown(BPLib_Instance_t *Inst, uint32_t ContactId);

/**
  * \brief      Get access to the run state of a particular contact
  * \param[in]  ContactId (uint32_t) Contact ID from the Contacts Configuration of whose run
  *                                  state should be returned
  * \param[out] ReturnState (BPLib_CLA_ContactRunState_t*) If the contact ID is deemed valid,
  *                                                        this will be the state of the request
  *                                                        contact at the contact ID
  * \return     Execution Status
  * \retval     BPLIB_SUCCESS: Successful execution
  * \retval     BPLIB_INVALID_CONT_ID_ERR: Provided contact ID is invalid
  */
BPLib_Status_t BPLib_CLA_GetContactRunState(uint32_t ContactId, BPLib_CLA_ContactRunState_t* ReturnState);

/**
  * \brief     Set the run state of the provided contact to BPLIB_CLA_EXITED
  * \note      This function is for external use of the contact run state accessor. Only exiting the application is allowed
  *            for the non-BPLib-CLA scope.
  * \param[in] ContactId (uint32_t) Contact ID from the Contacts Configuration whose run state is being requested to change
  * \return    Execution status (Status assigned by BPLib_CLA_SetContactRunState)
  * \retval    BPLIB_SUCCESS: Successfully changed the run state of the provided contact ID to the provided run state
  * \retval    BPLIB_INVALID_CONT_ID_ERR: Provided contact ID does not match a contact ID in the Contacts Configuration
  */
BPLib_Status_t BPLib_CLA_SetContactExited(uint32_t ContactId);

BPLib_Status_t BPLib_CLA_SetAutoEgress(uint32_t ContactId, bool AutoEgressEnabled);

bool BPLib_CLA_GetAutoEgress(uint32_t ContactId);

#endif /* BPLIB_CLA_H */
