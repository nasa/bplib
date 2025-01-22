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
    DATA_ACCOUTNING = 0,    // Per IANA
    FILE_DELIVERY   = 1,    // Per IANA
    RESERVED        = 2,    // Per IANA
    // 3 - 63 are unassigned services
    CFDP_SENDER     = 64,
    CFDP_RECEIVER   = 65,
    // 66 - 1023 are unassigned services reserved for SANA
    // 1024 - 65535 are unassigned services
    // >= 65535 are unassigned services reserved for private or experimental use
} BPLib_EID_Services_t;

/* ================== */
/* Exported Functions */
/* ================== */

/**
 * \brief     Checks if the EID matches an expected, valid pattern
 * \details   TODO
 * \note      TODO
 * \param[in] EID (BPLib_EID_t) EID struct consisting of an Authority, Node, and Service number
 * \return    EID validity
 * \retval    true: The EID matches the criteria required to be valid
 * \retval    false: The EID is invalid in some way
 * \secreflist
 * \refitem   BPLib_EID_t
 * \endsecreflist
 * \anchor    BPLib_EID_IsValid
 */
bool BPLib_EID_IsValid(BPLib_EID_t EID);

#endif /* BPLIB_EID_H */