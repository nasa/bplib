/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in bplib_eid header
 */

#include "bplib_eid.h"
#include "utgenstub.h"

/* ======= */
/* Globals */
/* ======= */

const BPLib_EID_t BPLIB_EID_DTN_NONE = {
    .Scheme       = BPLIB_EID_SCHEME_DTN,
    .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT,
    .Allocator    = 0,
    .Node         = 0,
    .Service      = 0
};

const BPLib_EID_t BPLIB_EID_IPN_NONE_2D = {
    .Scheme       = BPLIB_EID_SCHEME_IPN,
    .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT,
    .Allocator    = 0,
    .Node         = 0,
    .Service      = 0
};

const BPLib_EID_t BPLIB_EID_IPN_NONE_3D = {
    .Scheme       = BPLIB_EID_SCHEME_IPN,
    .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_THREE_DIGIT,
    .Allocator    = 0,
    .Node         = 0,
    .Service      = 0
};

BPLib_EID_t BPLIB_EID_INSTANCE = {
    .Scheme       = BPLIB_LOCAL_EID_SCHEME,
    .IpnSspFormat = BPLIB_LOCAL_EID_IPN_SSP_FORMAT,
    .Allocator    = BPLIB_LOCAL_EID_ALLOCATOR,
    .Node         = BPLIB_LOCAL_EID_NODE_NUM,
    .Service      = BPLIB_LOCAL_EID_SERVICE_NUM
};

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_EID_CopyEidPatterns()
 * ----------------------------------------------------
 */
void BPLib_EID_CopyEidPatterns(BPLib_EID_Pattern_t *EID_Actual, BPLib_EID_Pattern_t EID_Reference)
{
    UT_GenStub_AddParam(BPLib_EID_CopyEidPatterns, BPLib_EID_Pattern_t *, EID_Actual);
    UT_GenStub_AddParam(BPLib_EID_CopyEidPatterns, BPLib_EID_Pattern_t, EID_Reference);

    UT_GenStub_Execute(BPLib_EID_CopyEidPatterns, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_EID_CopyEids()
 * ----------------------------------------------------
 */
void BPLib_EID_CopyEids(BPLib_EID_t *EID_Actual, BPLib_EID_t EID_Reference)
{
    UT_GenStub_AddParam(BPLib_EID_CopyEids, BPLib_EID_t *, EID_Actual);
    UT_GenStub_AddParam(BPLib_EID_CopyEids, BPLib_EID_t, EID_Reference);

    UT_GenStub_Execute(BPLib_EID_CopyEids, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_EID_IsMatch()
 * ----------------------------------------------------
 */
bool BPLib_EID_IsMatch(const BPLib_EID_t *EID_Actual, const BPLib_EID_t *EID_Reference)
{
    UT_GenStub_SetupReturnBuffer(BPLib_EID_IsMatch, bool);

    UT_GenStub_AddParam(BPLib_EID_IsMatch, const BPLib_EID_t *, EID_Actual);
    UT_GenStub_AddParam(BPLib_EID_IsMatch, const BPLib_EID_t *, EID_Reference);

    UT_GenStub_Execute(BPLib_EID_IsMatch, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_EID_IsMatch, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_EID_IsValid()
 * ----------------------------------------------------
 */
bool BPLib_EID_IsValid(BPLib_EID_t *EID)
{
    UT_GenStub_SetupReturnBuffer(BPLib_EID_IsValid, bool);

    UT_GenStub_AddParam(BPLib_EID_IsValid, BPLib_EID_t *, EID);

    UT_GenStub_Execute(BPLib_EID_IsValid, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_EID_IsValid, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_EID_NodeIsMatch()
 * ----------------------------------------------------
 */
bool BPLib_EID_NodeIsMatch(const BPLib_EID_t *EID_Actual, const BPLib_EID_t *EID_Reference)
{
    UT_GenStub_SetupReturnBuffer(BPLib_EID_NodeIsMatch, bool);

    UT_GenStub_AddParam(BPLib_EID_NodeIsMatch, const BPLib_EID_t *, EID_Actual);
    UT_GenStub_AddParam(BPLib_EID_NodeIsMatch, const BPLib_EID_t *, EID_Reference);

    UT_GenStub_Execute(BPLib_EID_NodeIsMatch, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_EID_NodeIsMatch, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_EID_PatternIsMatch()
 * ----------------------------------------------------
 */
bool BPLib_EID_PatternIsMatch(BPLib_EID_t *EID_Actual, BPLib_EID_Pattern_t *EID_Pattern)
{
    UT_GenStub_SetupReturnBuffer(BPLib_EID_PatternIsMatch, bool);

    UT_GenStub_AddParam(BPLib_EID_PatternIsMatch, BPLib_EID_t *, EID_Actual);
    UT_GenStub_AddParam(BPLib_EID_PatternIsMatch, BPLib_EID_Pattern_t *, EID_Pattern);

    UT_GenStub_Execute(BPLib_EID_PatternIsMatch, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_EID_PatternIsMatch, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_EID_PatternIsValid()
 * ----------------------------------------------------
 */
bool BPLib_EID_PatternIsValid(BPLib_EID_Pattern_t *EID_Pattern)
{
    UT_GenStub_SetupReturnBuffer(BPLib_EID_PatternIsValid, bool);

    UT_GenStub_AddParam(BPLib_EID_PatternIsValid, BPLib_EID_Pattern_t *, EID_Pattern);

    UT_GenStub_Execute(BPLib_EID_PatternIsValid, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_EID_PatternIsValid, bool);
}
