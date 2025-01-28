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

const BPLib_EID_PatternMatch_t DTN_NONE = {.Scheme       = BPLIB_EID_SCHEME_DTN,
                                           .IpnSspFormat = 2,
                                           .MaxAuthority = 0,
                                           .MinAuthority = 0,
                                           .MaxNode      = 0,
                                           .MinNode      = 0,
                                           .MaxService   = 0,
                                           .MinService   = 0};

const BPLib_EID_PatternMatch_t IPN_2D_NONE = {.Scheme       = BPLIB_EID_SCHEME_IPN,
                                              .IpnSspFormat = 2,
                                              .MaxAuthority = 0,
                                              .MinAuthority = 0,
                                              .MaxNode      = 0,
                                              .MinNode      = 0,
                                              .MaxService   = 0,
                                              .MinService   = 0};

const BPLib_EID_PatternMatch_t IPN_3D_NONE = {.Scheme       = BPLIB_EID_SCHEME_IPN,
                                              .IpnSspFormat = 3,
                                              .MaxAuthority = 0,
                                              .MinAuthority = 0,
                                              .MaxNode      = 0,
                                              .MinNode      = 0,
                                              .MaxService   = 0,
                                              .MinService   = 0};

/* ==================== */
/* Function Definitions */
/* ==================== */

bool BPLib_EID_IsValid(BPLib_EID_t EID)
{
    bool IsValid;

    IsValid = true;

    if (EID.Scheme == BPLIB_EID_SCHEME_DTN)
    {
        /* Only dtn:none is accepted in the DTN scheme right now */
        IsValid = BPLib_EID_IsMatch(EID, DTN_NONE);
    }
    else if (EID.Scheme == BPLIB_EID_SCHEME_IPN)
    {
        /* Authority values need to be zero when not using them */
        if (EID.IpnSspFormat == 2 && EID.Authority != 0)
        {
            IsValid = false;
        }
        else
        {
            /* Anonymous Node 0 requires a Service number of 0 */
            if (EID.Node == 0 && EID.Service != 0)
            {
                IsValid = false;
            }
        }
    }

    return IsValid;
}

bool BPLib_EID_IsMatch(BPLib_EID_t EID_Actual, BPLib_EID_PatternMatch_t EID_Pattern)
{
    bool IsMatch;

    IsMatch = true;

    /* Input verification */
    if (EID_Pattern.MaxAuthority < EID_Pattern.MinAuthority ||
        EID_Pattern.MaxNode      < EID_Pattern.MinNode      ||
        EID_Pattern.MaxService   < EID_Pattern.MinService)
    {
        IsMatch = false;
    }
    else
    {
        if (EID_Actual.Scheme == EID_Pattern.Scheme)
        { /* The EID schemes are compatible for comparison */
            if (EID_Actual.IpnSspFormat == EID_Pattern.IpnSspFormat)
            { /* IPN formats are compatible for comparison */
                if (EID_Actual.IpnSspFormat == 3)
                { /* EID has an Authority */
                    /* Check for valid Authority values */
                    IsMatch &= (EID_Actual.Authority <= EID_Pattern.MaxAuthority &&
                                EID_Actual.Authority >= EID_Pattern.MinAuthority);
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

    return IsMatch;
}