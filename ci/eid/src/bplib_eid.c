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

    IsValid = true;

    if (EID.Authority == 0)
    {
        if (EID.Node == 0)
        { /* dtn:none */

        }
        else
        { /* 2-digit IPN */

        }
    }
    else
    { /* 3-digit IPN */

    }

    return IsValid;
}

bool BPLib_EID_IsMatch(BPLib_EID_t EID_Actual, BPLib_EID_t EID_Reference)
{
    bool IsMatch;

    IsMatch = false;

    if (EID_Actual.Authority == EID_Reference.Authority || EID_Reference.Authority == BPLIB_EID_WILDCARD)
    { /* Authority in EIDs match or the reference authority is a wildcard */
        if (EID_Actual.Node == EID_Reference.Node || EID_Reference.Node == BPLIB_EID_WILDCARD)
        { /* Node in EIDs match or the reference node is a wildcard */
            if (EID_Actual.Service == EID_Reference.Service || EID_Reference.Service == BPLIB_EID_WILDCARD)
            { /* Services in EIDs match or the reference service is a wildcard */
                IsMatch = true;
            }
        }
    }

    return IsMatch;
}