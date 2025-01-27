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
 * \brief 2-digit IPN (Node.Service)
 * \anchor BPLib_EID_IPN2_t
 */
typedef struct
{
    uint64_t Node;    /* System that implements DTN communications protocol services identified by unique node ID */
    uint64_t Service; /* DTN communication protocol service */
} BPLib_EID_IPN2_t;

/**
 * \brief 3-digit IPN (Authority.Node.Service)
 * \anchor BPLib_EID_IPN3_t
 */
typedef struct
{
    uint64_t Authority; /* Defines syntactic and semantic rules that explain how to parse and interpret scheme-specific part (SSP) */
    uint64_t Node;      /* System that implements DTN communications protocol services identified by unique node ID */
    uint64_t Service;   /* DTN communication protocol service */
} BPLib_EID_IPN3_t;

/**
 * \brief Provides discrete values for the scheme of the EID
 * \anchor BPLib_EID_Scheme_t
 */
typedef enum
{
    BPLIB_EID_SCHEME_UNDEFINED = 0,
    BPLIB_EID_SCHEME_INT       = 1, /* DTN scheme */
    BPLIB_EID_SCHEME_STRING    = 2, /* IPN scheme */
} BPLib_EID_Scheme_t;

/**
 * \brief EID schema (Scheme [Authority.]Node.Service)
 * \secreflist
 * \refitem BPLib_EID_Scheme_t
 * \refitem BPLib_EID_IPN_t
 * \endsecreflist
 * \anchor BPLib_EID_t
*/
typedef struct
{
    BPLib_EID_Scheme_t Scheme; /* The form the EID takes */

    union
    {
        BPLib_EID_IPN2_t IPN2;
        BPLib_EID_IPN3_t IPN3;
    } IPN;
} BPLib_EID_t;

/* ================== */
/* Exported Functions */
/* ================== */

/**
 * \brief     Checks if the EID consists of valid parts
 * \details   Validation is TBD
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