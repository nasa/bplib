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

const BPLib_EID_t BPLIB_EID_INSTANCE = {
                                .Scheme       = BPLIB_LOCAL_EID_SCHEME,
                                .IpnSspFormat = BPLIB_LOCAL_EID_IPN_SSP_FORMAT,
                                .Allocator    = BPLIB_LOCAL_EID_ALLOCATOR,
                                .Node         = BPLIB_LOCAL_EID_NODE_NUM,
                                .Service      = BPLIB_LOCAL_EID_SERVICE_NUM
};

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_EID_IsMatch()
 * ----------------------------------------------------
 */
bool BPLib_EID_IsMatch(BPLib_EID_t EID_Actual, BPLib_EID_t EID_Reference)
{
    UT_GenStub_SetupReturnBuffer(BPLib_EID_IsMatch, bool);

    UT_GenStub_AddParam(BPLib_EID_IsMatch, BPLib_EID_t, EID_Actual);
    UT_GenStub_AddParam(BPLib_EID_IsMatch, BPLib_EID_t, EID_Reference);

    UT_GenStub_Execute(BPLib_EID_IsMatch, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_EID_IsMatch, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_EID_IsValid()
 * ----------------------------------------------------
 */
bool BPLib_EID_IsValid(BPLib_EID_t EID)
{
    UT_GenStub_SetupReturnBuffer(BPLib_EID_IsValid, bool);

    UT_GenStub_AddParam(BPLib_EID_IsValid, BPLib_EID_t, EID);

    UT_GenStub_Execute(BPLib_EID_IsValid, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_EID_IsValid, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_EID_NodeIsMatch()
 * ----------------------------------------------------
 */
bool BPLib_EID_NodeIsMatch(BPLib_EID_t EID_Actual, BPLib_EID_t EID_Reference)
{
    UT_GenStub_SetupReturnBuffer(BPLib_EID_NodeIsMatch, bool);

    UT_GenStub_AddParam(BPLib_EID_NodeIsMatch, BPLib_EID_t, EID_Actual);
    UT_GenStub_AddParam(BPLib_EID_NodeIsMatch, BPLib_EID_t, EID_Reference);

    UT_GenStub_Execute(BPLib_EID_NodeIsMatch, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_EID_NodeIsMatch, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_EID_PatternIsMatch()
 * ----------------------------------------------------
 */
bool BPLib_EID_PatternIsMatch(BPLib_EID_t EID_Actual, BPLib_EID_Pattern_t EID_Pattern)
{
    UT_GenStub_SetupReturnBuffer(BPLib_EID_PatternIsMatch, bool);

    UT_GenStub_AddParam(BPLib_EID_PatternIsMatch, BPLib_EID_t, EID_Actual);
    UT_GenStub_AddParam(BPLib_EID_PatternIsMatch, BPLib_EID_Pattern_t, EID_Pattern);

    UT_GenStub_Execute(BPLib_EID_PatternIsMatch, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_EID_PatternIsMatch, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_EID_PatternIsValid()
 * ----------------------------------------------------
 */
bool BPLib_EID_PatternIsValid(BPLib_EID_Pattern_t EID_Pattern)
{
    UT_GenStub_SetupReturnBuffer(BPLib_EID_PatternIsValid, bool);

    UT_GenStub_AddParam(BPLib_EID_PatternIsValid, BPLib_EID_Pattern_t, EID_Pattern);

    UT_GenStub_Execute(BPLib_EID_PatternIsValid, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_EID_PatternIsValid, bool);
}
