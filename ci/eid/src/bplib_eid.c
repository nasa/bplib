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

/* ==================== */
/* Function Definitions */
/* ==================== */

bool BPLib_EID_IsValid(BPLib_EID_t EID)
{
    bool IsValid;

    IsValid = false;

    if (EID.Scheme == BPLIB_EID_SCHEME_DTN)
    {
        if (EID.Node == 0)
        {
            if (EID.Service == 0)
            {
                IsValid = true;
            }
        }
    }
    else if (EID.Scheme == BPLIB_EID_SCHEME_IPN)
    {
        if (EID.SspFormat == 2 && EID.Authority != 0)
        {
            IsValid = false;
        }
        else
        {
            IsValid = true;
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
            if (EID_Actual.SspFormat == EID_Pattern.SspFormat)
            { /* IPN formats are compatible for comparison */
                if (EID_Actual.SspFormat == 3)
                { /* EID has an Authority */
                    /* Check for valid Authority values */
                    IsMatch &= (EID_Actual.Authority <= EID_Pattern.MaxAuthority  &&
                                EID_Actual.Authority >= EID_Pattern.MinAuthority) ||
                               (EID_Actual.Authority == BPLIB_EID_WILDCARD);
                }

                /* Check for valid Node values */
                IsMatch &= (EID_Actual.Node <= EID_Pattern.MaxNode  &&
                            EID_Actual.Node >= EID_Pattern.MinNode) ||
                           (EID_Actual.Node == BPLIB_EID_WILDCARD);

                /* Check for valid Service values */
                IsMatch &= (EID_Actual.Service <= EID_Pattern.MaxService  &&
                            EID_Actual.Service >= EID_Pattern.MinService) ||
                           (EID_Actual.Service == BPLIB_EID_WILDCARD);
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