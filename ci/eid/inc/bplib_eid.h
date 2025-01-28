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

/* ======== */
/* Typedefs */
/* ======== */

/**
 * \brief Provides discrete values for the scheme of the EID
 * \note https://www.rfc-editor.org/rfc/rfc9171.html#section-9.6
 */
typedef enum
{
    BPLIB_EID_SCHEME_RESERVED = 0,
    BPLIB_EID_SCHEME_DTN      = 1, /* DTN scheme */
    BPLIB_EID_SCHEME_IPN      = 2, /* IPN scheme */
} BPLib_EID_Scheme_t;

/**
 * \brief EID schema (Scheme [Authority.]Node.Service)
 * \ref     BPLib_EID_Scheme_t
*/
typedef struct
{
    uint64_t Scheme;       /* Defines syntactic and semantic rules that explain how to parse and interpret scheme-specific part (SSP) */
    uint64_t IpnSspFormat; /* 2- or 3-digit IPN-style SSP indicator */
    uint64_t Authority;    /* Controlling organziation */
    uint64_t Node;         /* System that implements DTN communications protocol services identified by unique node ID */
    uint64_t Service;      /* DTN communication protocol service */
} BPLib_EID_t;

/**
 * \brief Pattern of acceptable EID values
 *
 * - MaxAuthority is the inclusive maximum value for the Authority component of the IPN-style SSP
 *
 * - MinAuthority is the inclusive minimum value for the Authority component of the IPN-style SSP
 *
 * - MaxNode is the inclusive maximum value for the Node component of the IPN-style SSP
 *
 * - MinNode is the inclusive minimum value for the Node component of the IPN-style SSP
 *
 * - MaxService is the inclusive maximum value for the Service of the IPN-style SSP
 *
 * - MinService is the inclusive minimum value for the Service of the IPN-style SSP
 */
typedef struct
{
    uint64_t Scheme;
    uint64_t IpnSspFormat;
    uint64_t MaxAuthority;
    uint64_t MinAuthority;
    uint64_t MaxNode;
    uint64_t MinNode;
    uint64_t MaxService;
    uint64_t MinService;
} BPLib_EID_PatternMatch_t;

/* =========== */
/* Global Data */
/* =========== */

extern const BPLib_EID_PatternMatch_t DTN_NONE;
extern const BPLib_EID_PatternMatch_t IPN_2D_NONE;
extern const BPLib_EID_PatternMatch_t IPN_3D_NONE;

/* =================== */
/* Function Prototypes */
/* =================== */

/**
 * \brief     Checks if the EID consists of valid parts
 * \details   Validation is TBD
 * \param[in] EID (BPLib_EID_t) EID struct whose validity is in question
 * \return    EID validity
 * \retval    true: The EID matches the criteria required to be valid
 * \retval    false: The EID is invalid in some way
 * \ref       BPLib_EID_t
 */
bool BPLib_EID_IsValid(BPLib_EID_t EID);

/**
 * \brief     Checks if the one EID matches a pattern
 * \details   The various members of each EID are compared for equivalence
 *            within a range
 * \note      Wildcards are represented as a BPLib_EID_PatternMatch_t::Min<Field> == 0
 *            and a BPLib_EID_PatternMatch_t::Max<Field> == 0xFFFF FFFF FFFF FFFF, where
 *            Field is Authority, Node, or Service
 * \param[in] EID_Actual (BPLib_EID_t) EID that is to be evaluated
 * \param[in] EID_Pattern (BPLib_EID_PatternMatch_t) EID pattern that is to be matched
 * \return    EID match
 * \retval    true: The actual EID does match the reference EID pattern
 * \retval    false: The actual EID does not match the reference EID pattern
 * \ref       BPLib_EID_t
 * \ref       BPLib_EID_PatternMatch_t
 */
bool BPLib_EID_IsMatch(BPLib_EID_t EID_Actual, BPLib_EID_PatternMatch_t EID_Pattern);

#endif /* BPLIB_EID_H */