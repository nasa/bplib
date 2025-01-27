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

    if (EID.Scheme == BPLIB_EID_SCHEME_INT)
    {
        /* NOTE: 2-digit IPNs will have a default Authority of 0 */
        if (EID.IPN.IPN3.Authority == 0)
        {
            if (EID.IPN.IPN2.Node == 0)
            { /* dtn:none */
                IsValid = true;
            }
            else
            { /* 2-digit IPN */
                IsValid = true;
            }
        }
        else
        { /* 3-digit IPN */
            IsValid = true;
        }
    }
    else
    {
        IsValid = false;
    }

    return IsValid;
}

bool BPLib_EID_IsMatch(BPLib_EID_t EID_Actual, BPLib_EID_t EID_Reference)
{
    bool IsMatch;

    IsMatch = false;

    if (EID_Actual.Scheme == EID_Reference.Scheme)
    { /* The EID schemes are compatible for comparison */
        if (EID_Actual.IPN.IPN3.Authority    == EID_Reference.IPN.IPN3.Authority ||
            EID_Actual.IPN.IPN3.Authority    == BPLIB_EID_WILDCARD               ||
            EID_Reference.IPN.IPN3.Authority == BPLIB_EID_WILDCARD)
        { /* Authority in EIDs match or the either EID's authority is a wildcard */
            if (EID_Actual.IPN.IPN3.Node    == EID_Reference.IPN.IPN3.Node ||
                EID_Actual.IPN.IPN3.Node    == BPLIB_EID_WILDCARD          ||
                EID_Reference.IPN.IPN3.Node == BPLIB_EID_WILDCARD)
            { /* Node in EIDs match or either EID's node is a wildcard */
                if (EID_Actual.IPN.IPN3.Service    == EID_Reference.IPN.IPN3.Service ||
                    EID_Actual.IPN.IPN3.Service    == BPLIB_EID_WILDCARD             ||
                    EID_Reference.IPN.IPN3.Service == BPLIB_EID_WILDCARD)
                { /* Services in EIDs match or either EID's service is a wildcard */
                    IsMatch = true;
                }
            }
        }
    }

    return IsMatch;
}