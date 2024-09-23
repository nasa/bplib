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

#include "bplib.h"


#define BPLIB_MAX_EID_LENGTH    256
#define BPLIB_MAX_NUM_CONTACTS  1
#define BPLIB_CLA_PERF_ID       182


/*
** Contacts Table
*/
typedef enum 
{
    UDPType = 0x00000000,
    TCPType = 0x00000001,
    EPPType = 0x00000002,
    LTPType = 0x00000003,
}CLAType_t;

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
} BPLib_ContactsSet_t;

struct BPLib_ContactsTable
{
    BPLib_ContactsSet_t ContactSet[BPLIB_MAX_NUM_CONTACTS];
};

/*
** Table Type def
*/
 typedef struct BPLib_ContactsTable BPLib_ContactsTable_t;  

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
int BPLib_CLA_Init(void);

/* CLA I/O (bundle data units) */

/**
 * \brief CLA Ingress function
 *
 *  \par Description
 *       Receive bundle from CL and pass it Bundle Interface 
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *  \param[in] ContactsTbl - Pointer of the Contact Table
 *  \param[in] Bundle - Pointer to the received bundle
 *  \param[in] Size - Size of the received bundle
 *  \param[in] Timeout - Time for pending on CL
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS when BPLib_CLA_Ingress was successful
 */
int BPLib_CLA_Ingress(uint8_t ContId, const void *Bundle, size_t Size, uint32_t Timeout);

/**
 * \brief CLA Egress function
 *
 *  \par Description
 *       Receive bundle from Bundle Interface and send it to CL 
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *  \param[in] ContactsTbl - Pointer of the Contact Table
 *  \param[in] Bundle - Pointer to the received bundle
 *  \param[in] Size - Size of the received bundle
 *  \param[in] Timeout - Time for pending on Bundle Queue
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS when BPLib_CLA_Egress was successful
 */
int BPLib_CLA_Egress(uint8_t ContId, void *Bundle, size_t *Size, uint32_t Timeout);

#endif /* BPLIB_CLA_H */
