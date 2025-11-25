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
    char ExpandedEventText[BPLIB_EM_EXPANDED_EVENT_SIZE];
    int ExpandedLength;
    va_list EventTextArgPtr;

    // Default to success status
    Status = BPLIB_SUCCESS;

    // Gather conversion specifiers from remaining arguments
    va_start(EventTextArgPtr, Spec);
    ExpandedLength = vsnprintf(ExpandedEventText, BPLIB_EM_EXPANDED_EVENT_SIZE, Spec, EventTextArgPtr);
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
