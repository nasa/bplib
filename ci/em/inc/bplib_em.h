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

/* ======== */
/* Typedefs */
/* ======== */
typedef enum
{
    BPLib_EM_EventType_UNKNOWN = 0,
    BPLib_EM_EventType_DEBUG   = 1,
    BPLib_EM_EventType_INFO    = 2,
    BPLib_EM_EventType_WARNING = 3,
    BPLib_EM_EventType_ERROR   = 4,
    BPLibEM_EventType_CRITICAL = 5
} BPLib_EM_EventType_t;

// typedef struct
// {
//     uint32_t ReturnValue;
// } BPL_Status_t;

// #define BPL_STATUS_SUCCESS             (0u)
// #define BPL_STATUS_ERROR_GENERAL       (1u)
// #define BPL_STATUS_ERROR_INPUT_INVALID (2u)
// #define BPL_STATUS_ERROR_PROXY_INIT    (3u)

// typedef struct
// {
//     BPL_Status_t (*Initialize_Impl)(void);
//     BPL_Status_t (*SendEvent_Impl)(uint16_t EventID, BPL_EM_EventType_t EventType,
//                                    char const * EventText, va_list EventTextArgPtr);
// } BPL_EM_ProxyCallbacks_t;

/* ================== */
/* Exported Functions */
/* ================== */
BPLib_Status_t BPLib_EM_Init(void);


BPLib_Status_t BPLib_EM_Register();

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
BPLib_Status_t BPLib_EM_SendEvent(uint16_t EventID, BPLib_EM_EventType_t EventType,
                                char const * EventText, ...);
void BPLib_EM_Deinitialize(void);

#endif /* BPLIB_EM_H */
