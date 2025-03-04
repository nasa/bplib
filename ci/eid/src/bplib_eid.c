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

/* ======== */
/* Includes */
/* ======== */

#include "bplib_eid.h"

/* =========== */
/* Global Data */
/* =========== */

const BPLib_EID_t BPLIB_EID_DTN_NONE = {.Scheme       = BPLIB_EID_SCHEME_DTN,
                                        .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT,
                                        .Allocator    = 0,
                                        .Node         = 0,
                                        .Service      = 0};

const BPLib_EID_t BPLIB_EID_IPN_NONE_2D = {.Scheme       = BPLIB_EID_SCHEME_IPN,
                                           .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT,
                                           .Allocator    = 0,
                                           .Node         = 0,
                                           .Service      = 0};

const BPLib_EID_t BPLIB_EID_IPN_NONE_3D = {.Scheme       = BPLIB_EID_SCHEME_IPN,
                                           .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_THREE_DIGIT,
                                           .Allocator    = 0,
                                           .Node         = 0,
                                           .Service      = 0};

const BPLib_EID_t BPLIB_EID_INSTANCE = {.Scheme       = BPLIB_LOCAL_EID_SCHEME,
                                        .IpnSspFormat = BPLIB_LOCAL_EID_IPN_SSP_FORMAT,
                                        .Allocator    = BPLIB_LOCAL_EID_ALLOCATOR,
                                        .Node         = BPLIB_LOCAL_EID_NODE_NUM,
                                        .Service      = BPLIB_LOCAL_EID_SERVICE_NUM};

/* ==================== */
/* Function Definitions */
/* ==================== */

bool BPLib_EID_IsValid(BPLib_EID_t EID)
{
    bool IsValid;

    if (EID.Scheme == BPLIB_EID_SCHEME_DTN)
    {
        /* Only dtn:none is accepted in the DTN scheme right now */
        IsValid = BPLib_EID_IsMatch(EID, BPLIB_EID_DTN_NONE);
    }
    else if (EID.Scheme == BPLIB_EID_SCHEME_IPN)
    {
        if (EID.IpnSspFormat == BPLIB_EID_IPN_SSP_FORMAT_THREE_DIGIT)
        {
            if (EID.Allocator == 0)
            {
                if (EID.Node == 0)
                {
                    if (EID.Service == 0)
                    { /* Allocator = 0, Node = 0, Service = 0 */
                        /* 
                         * Within the ipn URI scheme, the 'null' EID is represented by the Null
                         * ipn URI (Section 3.1).  This means that the URIs dtn:none
                         * (Section 4.2.5.1.1 of [RFC9171]), ipn:0.0, and ipn:0.0.0 all refer to
                         * the BPv7 'null' endpoint.
                         */
                        IsValid = true;
                    }
                    else
                    { /* Allocator = 0, Node = 0, Service > 0 */
                        /*
                         * This means that any ipn URI with a zero (0) Allocator Identifier and
                         * a zero (0) Node Number, but a non-zero Service Number component is
                         * invalid.  Such ipn URIs MUST NOT be composed, and processors of such
                         * ipn URIs MUST consider them as the Null ipn URI.
                         */
                        IsValid = false;
                    }
                }
                else
                { /* Allocator = 0, Node > 0, Service = ?? */
                    IsValid = true;
                }
            }
            else
            { /* Allocator > 0 */
                if (EID.Node == 0)
                { /* Allocator > 0, Node = 0, Service = ?? */
                    /*
                     * It is RECOMMENDED that Node Number zero (0) not be assigned by an
                     * Allocator to avoid confusion with the Null ipn URI
                     */ 
                    IsValid = false;
                }
                else
                { /* Allocator > 0, Node > 0, Service = ?? */
                    IsValid = true;
                }
            }
        }
        else if (EID.IpnSspFormat == BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT)
        {
            if (EID.Allocator == 0)
            {
                if (EID.Node == 0)
                {
                    if (EID.Service == 0)
                    { /* Allocator = 0, Node = 0, Service = 0 */
                        /*
                         * Within the ipn URI scheme, the 'null' EID is represented by the Null
                         * ipn URI (Section 3.1).  This means that the URIs dtn:none
                         * (Section 4.2.5.1.1 of [RFC9171]), ipn:0.0, and ipn:0.0.0 all refer to
                         * the BPv7 'null' endpoint.
                         */
                        IsValid = true;
                    }
                    else
                    { /* Allocator = 0, Node = 0, Service != 0 */
                        /* Anonymous Node 0 requires a Service number of 0 */
                        IsValid = false;
                    }
                }
                else
                { /* Allocator = 0, Node > 0, Service = ?? */
                    IsValid = true;
                }
            }
            else
            { /* Allocator > 0, Node = ??, Service = ?? */
                /* Allocator values need to be zero when not using them */
                IsValid = false;
            }
        }
        else
        { /* Invalid IPN SSP format */
            IsValid = false;
        }
    }
    else
    {
        IsValid = false;
    }

    return IsValid;
}

bool BPLib_EID_PatternIsValid(BPLib_EID_Pattern_t EID_Pattern)
{
    return (EID_Pattern.MaxAllocator >= EID_Pattern.MinAllocator &&
            EID_Pattern.MaxNode      >= EID_Pattern.MinNode      &&
            EID_Pattern.MaxService   >= EID_Pattern.MinService);
}

bool BPLib_EID_IsMatch(BPLib_EID_t EID_Actual, BPLib_EID_t EID_Reference)
{
    return (EID_Actual.Scheme       == EID_Reference.Scheme       &&
            EID_Actual.IpnSspFormat == EID_Reference.IpnSspFormat &&
            EID_Actual.Allocator    == EID_Reference.Allocator    &&
            EID_Actual.Node         == EID_Reference.Node         &&
            EID_Actual.Service      == EID_Reference.Service);
}

bool BPLib_EID_NodeIsMatch(BPLib_EID_t EID_Actual, BPLib_EID_t EID_Reference)
{
    return (EID_Actual.Scheme       == EID_Reference.Scheme       &&
            EID_Actual.IpnSspFormat == EID_Reference.IpnSspFormat &&
            EID_Actual.Allocator    == EID_Reference.Allocator    &&
            EID_Actual.Node         == EID_Reference.Node);
}

void BPLib_EID_CopyEids(BPLib_EID_t *EID_Actual, BPLib_EID_t EID_Reference)
{
    EID_Actual->Allocator = EID_Reference.Allocator;
    EID_Actual->IpnSspFormat = EID_Reference.IpnSspFormat;
    EID_Actual->Node = EID_Reference.Node;
    EID_Actual->Scheme = EID_Reference.Scheme;
    EID_Actual->Service = EID_Reference.Service;
}

bool BPLib_EID_PatternIsMatch(BPLib_EID_t EID_Actual, BPLib_EID_Pattern_t EID_Pattern)
{
    bool IsMatch;

    IsMatch = true;

    /* Input verification */
    if (BPLib_EID_PatternIsValid(EID_Pattern))
    { /* The pattern is considered valid */
        if (EID_Actual.Scheme == EID_Pattern.Scheme)
        { /* The EID schemes are compatible for comparison */
            if (EID_Actual.IpnSspFormat == EID_Pattern.IpnSspFormat)
            { /* IPN formats are compatible for comparison */
                if (EID_Actual.IpnSspFormat == BPLIB_EID_IPN_SSP_FORMAT_THREE_DIGIT)
                { /* EID has an Allocator */
                    /* Check for valid Allocator values */
                    IsMatch &= (EID_Actual.Allocator <= EID_Pattern.MaxAllocator &&
                                EID_Actual.Allocator >= EID_Pattern.MinAllocator);
                }

                /* Check for valid Node values */
                IsMatch &= (EID_Actual.Node <= EID_Pattern.MaxNode &&
                            EID_Actual.Node >= EID_Pattern.MinNode);

                /* Check for valid Service values */
                IsMatch &= (EID_Actual.Service <= EID_Pattern.MaxService &&
                            EID_Actual.Service >= EID_Pattern.MinService);
            }
            else
            {
                IsMatch = false;
            }
        }
        else
        {
            IsMatch = false;
        }
    }
    else
    {
        IsMatch = false;
    }

    return IsMatch;
}