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
        if (EID.IpnFormat == 2)
        {
            if (EID.Authority != 0)
            {
                IsValid = false;
            }
        }
    }

    return IsValid;
}

bool BPLib_EID_IsMatch(BPLib_EID_t EID_Actual, BPLib_EID_PatternMatch_t EID_Pattern)
{
    if (EID_Actual.Scheme == EID_Pattern.Scheme)
    { /* The EID schemes are compatible for comparison */
        if (EID_Actual.IpnFormat == EID_Pattern.IpnFormat)
        { /* IPN formats are compatible for comparison */
            if (EID_Actual.IpnFormat == 3)
            { /* IPN format contains an Authority member, so compare the Authorities */
                if (EID_Actual.Authority  > EID_Pattern.MaxAuthority ||
                    EID_Actual.Authority  < EID_Pattern.MinAuthority &&
                    EID_Actual.Authority != BPLIB_EID_WILDCARD)
                { /* Given Authority value is out of range and not a wildcard */
                    return false;
                }
            }

            if (EID_Actual.Node  > EID_Pattern.MaxNode ||
                EID_Actual.Node  < EID_Pattern.MinNode &&
                EID_Actual.Node != BPLIB_EID_WILDCARD)
            { /* Given Node value is out of range and not a wildcard */
                return false;
            }

            if (EID_Actual.Service  > EID_Pattern.MaxService ||
                EID_Actual.Service  < EID_Pattern.MinService &&
                EID_Actual.Service != BPLIB_EID_WILDCARD)
            { /* Given Service value is out of range and not a wildcard */
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}