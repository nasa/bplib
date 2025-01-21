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

bool BPLib_EID_IsValid(uint64_t Authority, uint64_t Node, uint64_t Service)
{
    bool IsValid;

    IsValid = false;

    if (Authority == DTN_AUTH)
    {
        if (Node == 0)
        {
            /* dtn:none */
        }
        else
        {
            /* 2-digit IPN */
        }
    }
    else
    {
        /* 3-digit IPN */
    }
}