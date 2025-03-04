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
#include "bplib_cla_internal.h"
#include "bplib_bi.h"
#include "bplib_em.h"
#include "bplib_fwp.h"

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
} CLAType_t;

typedef struct
{
    char        CtrlMsgTag[8]; /* "BPNMSG" */
    uint32_t    SessionID;
    uint32_t    BundleID;
    CLAType_t   ClaType;
    uint8_t     MsgTypes;
} BPLib_CLA_CtrlMsg_t;

typedef enum
{
    BPLIB_CLA_TORNDOWN = 0, /* Default to torn down */
    BPLIB_CLA_SETUP    = 1,
    BPLIB_CLA_STARTED  = 2,
    BPLIB_CLA_STOPPED  = 3,
} BPLib_CLA_ContactRunState_t;

/**
 * \brief Global Contacts Configuration
 */
typedef struct
{
    uint32_t            ContactId;
    BPLib_EID_Pattern_t DestEIDs[BPLIB_MAX_CONTACT_DEST_EIDS];
    CLAType_t           CLAType;
     /**
      * CLAddr uses BPLIB_MAX_EID_LENGTH, but initialization in
      * bpnode_contact_tbl.c is
      * .CLAddr = "127.0.0.1"
      * which makes it a string containing an IP address, not an EID.
      */
    char                CLAddr[BPLIB_MAX_EID_LENGTH];
    int32_t             PortNum;
    uint32_t            DestLTPEngineID;
    uint32_t            SendBytePerCycle;
    uint32_t            ReceiveBytePerCycle;
    uint32_t            RetransmitTimeout;
    uint32_t            CSTimeTrigger;
    uint32_t            CSSizeTrigger;
} BPLib_CLA_ContactsSet_t;

struct BPLib_CLA_ContactsTable
{
    BPLib_CLA_ContactsSet_t ContactSet[BPLIB_MAX_NUM_CONTACTS];
};

typedef struct BPLib_CLA_ContactsTable BPLib_CLA_ContactsTable_t;

/* =================== */
/* Function Prototypes */
/* =================== */

/**
 * \brief Convergence Layer Adaptor initialization
 *
 *  \par Description
 *       CLA initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
BPLib_Status_t BPLib_CLA_Init(void);

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
BPLib_Status_t BPLib_CLA_Ingress(BPLib_Instance_t* Inst, uint8_t ContId,
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
BPLib_Status_t BPLib_CLA_Egress(BPLib_Instance_t* Inst, uint8_t ContId, void *BundleOut,
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
 * \param[in] ContactId (uint16_t) Contact ID from the Contacts Configuration to setup
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_CLA_UNKNOWN_CONTACT: Contact ID could not be found in the Contacts Configuration
 * \retval    BPLIB_CLA_CONTACTS_MAX_REACHED: Setting up contact would exceed maximum simultaneous
 *                                            contacts allowed
 */
BPLib_Status_t BPLib_CLA_ContactSetup(uint16_t ContactId);

/**
 * \brief     Pass Contact ID to start, on to CLA proxy
 * \param[in] ContactId (uint16_t) Contact ID from the Contacts Configuration to start
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_CLA_ERROR: TODO
 */
BPLib_Status_t BPLib_CLA_ContactStart(uint16_t ContactId);

/**
 * \brief     Pass Contact ID to stop, on to CLA proxy
 * \param[in] ContactId (uint16_t) Contact ID from the Contacts Configuration to stop
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_CLA_ERROR: TODO
 */
BPLib_Status_t BPLib_CLA_ContactStop(uint16_t ContactId);

/**
 * \brief     If the contact has been stopped, deconfigure the CLA via BI, CT, EBP, and CLA
 * \param[in] ContactId (uint16_t) Contact ID from the Contacts Configuration to teardown
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_CLA_CONTACT_RUNNING: Contact has not been stopped
 */
BPLib_Status_t BPLib_CLA_ContactTeardown(uint16_t ContactId);

/**
  * \brief     Get access to the run state of a particular contact
  * \param[in] ContactId (uint16_t) Contact ID from the Contacts Configuration of whose run
  *                                 state should be returned
  * \return    Contact run state
  */
BPLib_CLA_ContactRunState_t BPLib_CLA_GetContactRunState(uint16_t ContactId);

/**
  * \brief     Set the run state of the provided contact
  * \param[in] ContactId (uint16_t) Contact ID from the Contacts Configuration whose run state is being requested to change
  * \param[in] RunState  (BPLib_CLA_ContactRunState_t) Requested run state of the provided contact ID from the Contacts Configuration
  * \return    Execution status
  * \retval    BPLIB_SUCCESS: Successfully changed the run state of the provided contact ID to the provided run state
  * \retval    BPLIB_CLA_UNKNOWN_CONTACT: Provided contact ID does not match a contact ID in the Contacts Configuration
  * \retval    BPLIB_CLA_INCORRECT_STATE: The intended run state for the contact with the provided contact ID was incompatible with the
  *                                       current run state of the contact
  */
 BPLib_Status_t BPLib_CLA_SetContactRunState(uint16_t ContactId, BPLib_CLA_ContactRunState_t RunState);

#endif /* BPLIB_CLA_H */
