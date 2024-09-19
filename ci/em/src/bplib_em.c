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

#include "bplib_em.h"
#include "bplib_fwp.h"
#include <string.h>
#include <stdio.h>

/* ==================== */
/* Function Definitions */
/* ==================== */

// Event management initialization
BPLib_Status_t BPLib_EM_Init(void)
{
    BPLib_Status_t Status;

    /* Initialize the Status as BPLIB_SUCCESS and
       leave open to change under future circumstances */
    Status = BPLIB_SUCCESS;

    BPLib_FWP_ProxyCallbacks.BPA_EVP_Init();

    return Status;
}

// Event generation
BPLib_Status_t BPLib_EM_SendEvent(uint16_t EventID, BPLib_EM_EventType_t EventType, char const* Spec, ...)
{
    BPLib_Status_t Status;
    char* ExpandedEventText;
    int ExpandedLength;
    va_list EventTextArgPtr;

    // Initialize Status to BPLIB_SUCCESS
    Status = BPLIB_SUCCESS;

    va_start(EventTextArgPtr, Spec);
    ExpandedLength = vsprintf((char*)ExpandedEventText, Spec, EventTextArgPtr);
    va_end(EventTextArgPtr);

    if (ExpandedLength < 0)
    {
        Status = BPLIB_EM_EXPANDED_TEXT_ERROR;
    }
    else
    {
        BPLib_FWP_ProxyCallbacks.BPA_EVP_SendEvent(EventID, EventType, ExpandedEventText);
    }

    return Status;
}
