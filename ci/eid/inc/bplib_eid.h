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

#ifndef BPLIB_EID_H
#define BPLIB_EID_H

/* ======= */
/* Include */
/* ======= */

#include "bplib_api_types.h"

/* ====== */
/* Macros */
/* ====== */

/* Wildcard designator in EIDs, max value for uint64_t */
#define BPLIB_EID_WILDCARD 0xFFFFFFFF

/* ======== */
/* Typedefs */
/* ======== */

/**
 * \brief EID schema (Authority.Node.Service)
 * \anchor BPLib_EID_t
*/
typedef struct
{
    uint64_t Authority; /* Defines a set of syntactic and semantic rules that fully explain how to parse and interpret the scheme-specific part (SSP) */
    uint64_t Node;      /* System implementing a set of DTN communications protocol services identified by a unique node ID */
    uint64_t Service;   /* DTN communication protocol service */
} BPLib_EID_t;

/**
 * \brief Known, valid authorities
 * \anchor BPLib_EID_Authorities_t
 */
typedef enum
{
    DTN_AUTH = 0,
    IPN_AUTH = 1,
} BPLib_EID_Authorities_t;

typedef enum
{
    // EMERGENCY                 = 0,
    // LATENCY                   = 1,
    // PAYLOAD_SIZING            = 2,
    // ROUTING                   = 3,
    // CONFIGURATION             = 4,
    // RENTENTION_ASSURANCE      = 5,
    // COMMENCE_REGISTRATION     = 6,
    // TERMINATE_REGISTRATION    = 7,
    // SWITCH_REGISTRATION_STATE = 8,
    // TRANSMIT                  = 9,
    // CANCEL_TRANSMIT           = 10,
    // POLL                      = 11,
    // DELIVER                   = 12,
    // STORE                     = 13,
    // SECURITY                  = 14,
    // STORAGE_MANAGEMENT        = 15,
    // TRANSPORT_MANAGEMENT      = 16,
    // USER_DATA_ACCOUNTING      = 17,
    // USER_DEFINED_EXT_BLOCKS   = 18,

    DELIVER                 = 0,
    TRANSFER                = 1,
    SERVICE_POLICY          = 2,
    PRIORITY                = 3,
    STORE                   = 4,
    FRAGMENTATION_CONTROL   = 5,
    DATA_ACCOUTNING         = 6,
    CONFIDENTIALITY         = 7,
    AUTHENTICATION          = 8,
    EXTENSION_BLOCK_SUPPORT = 9,
    BUNDLE_STATUS           = 10,
    EMERGENCY               = 11,
} BPLib_EID_Services_t;

/* ================== */
/* Exported Functions */
/* ================== */

/**
 * \brief     Checks if the EID matches an expected, valid pattern
 * \details   TODO
 * \note      TODO
 * \param[in] Authority (uint64_t) The endpoints authority
 * \param[in] Node (uint64_t) System that implements DTN services
 * \param[in] Service (uint64_t) Communication protocol service
 * \return    EID validity
 * \retval    true: The EID matches the criteria required to be valid
 * \retval    false: The EID is invalid in some way
 * \secreflist
 * \refitem   BPLib_EID_t
 * \endsecreflist
 * \anchor    BPLib_EID_IsValid
 */
bool BPLib_EID_IsValid(uint64_t Authority, uint64_t Node, uint64_t Service);

/* TODO: Documentation */
void BPLib_EID_RequestService(uint64_t Service);

#endif /* BPLIB_EID_H */
