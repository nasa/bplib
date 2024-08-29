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

#ifndef BPLIB_EM_H
#define BPLIB_EM_H

/* ======= */
/* Include */
/* ======= */
#include "bplib_api_types.h"
#include "cfe.h"
#include <stdarg.h>

/* ======== */
/* Typedefs */
/* ======== */
typedef enum
{
    BPLib_EM_EventType_UNKNOWN  = 0,
    BPLib_EM_EventType_DEBUG    = 1,
    BPLib_EM_EventType_INFO     = 2,
    BPLib_EM_EventType_WARNING  = 3,
    BPLib_EM_EventType_ERROR    = 4,
    BPLib_EM_EventType_CRITICAL = 5
} BPLib_EM_EventType_t;

#define BPLIB_EM_MSG_TRUNCATED ('$')

// Defines the max length of the expanded event string
// Limits:
// - must be greater than or equal to 2
// - must be less than or equal to host limit (CFE_MISSION_EVS_MAX_MESSAGE_LENGTH)
#define BPLIB_EM_MAX_MESSAGE_LENGTH (CFE_MISSION_EVS_MAX_MESSAGE_LENGTH)

/* ================== */
/* Exported Functions */
/* ================== */
BPLib_Status_t BPLib_EM_Init(void);

/**
 * \brief Convert event indicators from an integer type to a string
 *
 *  \par Description
 *       Take in an integer-type and translate it into a string
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] Type Integer value of an event type
 *
 *  \return String representation of give event type
 */
char const* BPLib_EM_EventTypeToString(BPLib_EM_EventType_t Type);

BPLib_Status_t BPLib_EM_Register(const void* Filters, uint16_t NumEventFilters, uint16_t FilterScheme);

BPLib_Status_t BPLib_EM_SendEvent(uint16_t EventID, BPLib_EM_EventType_t EventType,
                                  char const* EventText, va_list EventTextArgPtr);

#endif /* BPLIB_EM_H */
