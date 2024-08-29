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

/* ================== */
/* Exported functions */
/* ================== */
BPLib_Status_t BPLib_EM_Init(void)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    return Status;
}

char const* BPLib_EM_EventTypeToString(BPLib_EM_EventType_t Type)
{
    // BPLib_EM_EventTypeStrings should always match BPLib_EM_EventType_t.
    static char const * BPLib_EM_EventTypeStrings[] = {
        "UNKNOWN",
        "DEBUG",
        "INFO",
        "WARNING",
        "ERROR",
        "CRITICAL"
    };

    switch (Type)
    {
        case BPLib_EM_EventType_DEBUG:
            return BPLib_EM_EventTypeStrings[1];
        case BPLib_EM_EventType_INFO:
            return BPLib_EM_EventTypeStrings[2];
        case BPLib_EM_EventType_WARNING:
            return BPLib_EM_EventTypeStrings[3];
        case BPLib_EM_EventType_ERROR:
            return BPLib_EM_EventTypeStrings[4];
        case BPLib_EM_EventType_CRITICAL:
            return BPLib_EM_EventTypeStrings[5];
        default:
            /* This default case also captures the BPL_EM_EventType_UNKNOWN case. */
            return BPLib_EM_EventTypeStrings[0];
    }
}

BPLib_Status_t BPLib_EM_Register(const void* Filters, uint16_t NumEventFilters, uint16_t FilterScheme)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    BPLib_FWP_ProxyCallbacks.BPA_EVP_Register(Filters, NumEventFilters, FilterScheme);

    return Status;
}

BPLib_Status_t BPLib_EM_SendEvent(uint16_t EventID, BPLib_EM_EventType_t EventType,
                                  char const* EventText, va_list EventTextArgPtr)
{
    BPLib_Status_t Status;
    // char const* EventTypeString;
    char ExpandedEventText[BPLIB_EM_MAX_MESSAGE_LENGTH];
    int ExpandedLength;

    // Initialize Status to BPLIB_SUCCESS
    Status = BPLIB_SUCCESS;

    // Grab the stringified event type
    // EventTypeString = BPLib_EM_EventTypeToString(EventType);

    // Verify that the max message length for EM is more than a truncation character and >= the pre-defined
    // max length of an EVS message according to CFE
    if (BPLIB_EM_MAX_MESSAGE_LENGTH < 2 || BPLIB_EM_MAX_MESSAGE_LENGTH > CFE_MISSION_EVS_MAX_MESSAGE_LENGTH)
    {
        Status = BPLIB_ERROR;
    }
    else
    {
        memset(&ExpandedEventText, 0, sizeof(ExpandedEventText));
        ExpandedLength = vsnprintf((char*)ExpandedEventText, sizeof(ExpandedEventText),
                                   EventText, EventTextArgPtr);

        if (ExpandedLength >= (int)sizeof(ExpandedEventText))
        {
            // Mark character before zero terminator to indicate truncation
            ExpandedEventText[sizeof(ExpandedEventText) - 2u] = BPLIB_EM_MSG_TRUNCATED;
        }

        // BPLib_FWP_ProxyCallbacks.BPA_EVP_SendEvent(EventID, EventTypeString, ExpandedEventText);
        BPLib_FWP_ProxyCallbacks.BPA_EVP_SendEvent(EventID, EventType, ExpandedEventText);
    }

    return Status;
}
