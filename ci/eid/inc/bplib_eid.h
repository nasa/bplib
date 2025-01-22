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
 * \brief EID schema (Scheme Authority.Node.Service)
 * \anchor BPLib_EID_t
*/
typedef struct
{
    uint8_t  Scheme;    /* The form the EID takes */
    uint64_t Authority; /* Defines a set of syntactic and semantic rules that fully explain how to parse and interpret the scheme-specific part (SSP) */
    uint64_t Node;      /* System implementing a set of DTN communications protocol services identified by a unique node ID */
    uint64_t Service;   /* DTN communication protocol service */
} BPLib_EID_t;

/* ================== */
/* Exported Functions */
/* ================== */

/**
 * \brief     Checks if the EID consists of valid parts
 * \details   TODO
 * \note      TODO
 * \param[in] EID (BPLib_EID_t) EID struct whose validity is in question
 * \return    EID validity
 * \retval    true: The EID matches the criteria required to be valid
 * \retval    false: The EID is invalid in some way
 * \secreflist
 * \refitem   BPLib_EID_t
 * \endsecreflist
 * \anchor    BPLib_EID_IsValid
 */
bool BPLib_EID_IsValid(BPLib_EID_t EID);

/**
 * \brief     Checks if the one EID matches another EID
 * \details   The various members of each EID are compared for equivalence
 *            or whether the reference is a wildcard
 * \note      Wildcards are represented as BPLIB_EID_WILDCARD
 * \param[in] EID_Actual (BPLib_EID_t) EID that is to be evaluated
 * \param[in] EID_Reference (BPLib_EID_t) EID that is to be matched
 * \return    EID match
 * \retval    true: The actual EID does match the reference EID
 * \retval    false: The actual EID does not match the reference EID
 * \secreflist
 * \refitem   BPLib_EID_t
 * \endsecreflist
 * \anchor    BPLib_EID_IsMatch
 */
bool BPLib_EID_IsMatch(BPLib_EID_t EID_Actual, BPLib_EID_t EID_Reference);

#endif /* BPLIB_EID_H */