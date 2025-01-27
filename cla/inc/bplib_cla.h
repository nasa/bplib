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

/*
** Include
*/

#include "bplib_api_types.h"
#include "bplib_cfg.h"
#include "bplib_qm.h"

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
}CLAType_t;

typedef struct
{
    char        CtrlMsgTag[8]; /* "BPNMSG" */
    uint32_t    SeesionID;
    uint32_t    BundleID;
    CLAType_t   ClaType;
    uint8_t     MsgTypes;
} BPLib_CLA_CtrlMsg_t;

/*
** Contacts Table
*/

typedef struct
{
    uint32_t    ContactID;
    char        DestEIDs[BPLIB_MAX_EID_LENGTH];
    CLAType_t   CLAType;  
    char        CLAddr[BPLIB_MAX_EID_LENGTH];
    int32_t     PortNum;
    uint32_t    DestLTPEngineID;
    uint32_t    SendBytePerCycle;
    uint32_t    ReceiveBytePerCycle;
    uint32_t    RetransmitTimeout;
    uint32_t    CSTimeTrigger;
    uint32_t    CSSizeTrigger;
} BPLib_CLA_ContactsSet_t;

struct BPLib_CLA_ContactsTable
{
    BPLib_CLA_ContactsSet_t ContactSet[BPLIB_MAX_NUM_CONTACTS];
};

/*
** Table Type def
*/
 typedef struct BPLib_CLA_ContactsTable BPLib_CLA_ContactsTable_t;  

/*
** Exported Functions
*/

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
 *  \param[in] inst Pointer to a valid Queue Table Instance
 *  \param[in] ContactsTbl - Pointer of the Contact Table
 *  \param[in] Bundle - Pointer to the received bundle
 *  \param[in] Size - Size of the received bundle
 *  \param[in] Timeout - Time for pending on CL
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS when BPLib_CLA_Ingress was successful
 */
BPLib_Status_t BPLib_CLA_Ingress(BPLib_Instance_t* inst, uint8_t ContId, const void *Bundle, size_t Size, uint32_t Timeout);

/**
 * \brief CLA Egress function
 *
 *  \par Description
 *       Receive bundle from Bundle Interface and send it to CL 
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] inst Pointer to a valid Queue Table Instance
 *  \param[in] ContactsTbl - Pointer of the Contact Table
 *  \param[in] Bundle - Pointer to the received bundle
 *  \param[in] Size - Size of the received bundle
 *  \param[in] Timeout - Time for pending on Bundle Queue
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS when BPLib_CLA_Egress was successful
 */
BPLib_Status_t BPLib_CLA_Egress(BPLib_Instance_t* inst, uint8_t ContId, void *Bundle, size_t *Size, uint32_t Timeout);

/**
 * \brief Validate Contact Table configurations
 *
 *  \par Description
 *       Validate configuration table parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management. 
 *         Every time a new Contact table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 */
BPLib_Status_t BPLib_CLA_ContactsTblValidateFunc(void *TblData);

#endif /* BPLIB_CLA_H */
