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
#include <stdarg.h>

/* ====== */
/* Macros */
/* ====== */

#define BPLIB_EM_EXPANDED_EVENT_SIZE (300u)

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

/* ================== */
/* Exported Functions */
/* ================== */

/**
  * \brief   Events Management initialization
  * \details Initialize values to perform event management
  * \note    As of right now, this function always returns BPLIB_SUCCESS
  * \return  Execution status
  * \retval  BPLIB_SUCCESS: Initialization was successful
  */
BPLib_Status_t BPLib_EM_Init(void);

/**
  * \brief   Platform-independent software event generation
  * \details Description
  *          Platform-independent routine generation of a software event
  * \return  Execution status
  * \retval  BPLIB_SUCCESS (0): Event was successful generated
  * \retval  BPLIB_EM_EXPANDED_TEXT_ERROR (-14u): An error occured when using vsprintf
  */
BPLib_Status_t BPLib_EM_SendEvent(uint16_t EventID, BPLib_EM_EventType_t EventType, char const* EventText, ...);

#endif /* BPLIB_EM_H */
