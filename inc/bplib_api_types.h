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

#ifndef BPLIB_API_TYPES_H
#define BPLIB_API_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Include Files
*/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
** Type Definitions
*/

/**
 * \brief BPLib status type for type safety
 */
typedef int32_t BPLib_Status_t;

/*
** Macros
*/

/* General Return Codes */

#define BPLIB_SUCCESS                       ((BPLib_Status_t)  0)
#define BPLIB_ERROR                         ((BPLib_Status_t) -1)
#define BPLIB_UNIMPLEMENTED                 ((BPLib_Status_t) -2)
#define BPLIB_UNKNOWN                       ((BPLib_Status_t) -3)

/* Framework Proxy Errors */
#define BPLIB_FWP_CALLBACK_INIT_ERROR       ((BPLib_Status_t) -4)

/* Time Management Errors */
#define BPLIB_TIME_UNDEF_DELTA_ERROR        ((BPLib_Status_t) -5)
#define BPLIB_TIME_WRITE_ERROR              ((BPLib_Status_t) -6)
#define BPLIB_TIME_READ_ERROR               ((BPLib_Status_t) -7)
#define BPLIB_TIME_UNINIT_ERROR             ((BPLib_Status_t) -8)

/* Event Management Errors */
#define BPLIB_EM_STRING_TRUNCATED           ((BPLib_Status_t) -9u)
#define BPLIB_EM_ILLEGAL_APP_ID             ((BPLib_Status_t) -10u)
#define BPLIB_EM_UNKNOWN_FILTER             ((BPLib_Status_t) -11u)
#define BPLIB_EM_BAD_ARGUMENT               ((BPLib_Status_t) -12u)
#define BPLIB_EM_INVALID_PARAMETER          ((BPLib_Status_t) -13u)
#define BPLIB_EM_APP_NOT_REGISTERED         ((BPLib_Status_t) -14u)
#define BPLIB_EM_APP_SQUELCHED              ((BPLib_Status_t) -15u)
#define BPLIB_EM_EXPANDED_TEXT_ERROR        ((BPLib_Status_t) -16u)

/* PerfLog Proxy Errors*/
#define BPLIB_PL_NULL_CALLBACK_ERROR        ((BPLib_Status_t) -17u)

/* ADU Proxy Errors */
#define BPLIB_ADU_ADD_CHAN_ERR              ((BPLib_Status_t) -18u)
#define BPLIB_ADU_ADD_STAT_ERR              ((BPLib_Status_t) -19u)
#define BPLIB_ADU_START_CHAN_ERR            ((BPLib_Status_t) -20u)
#define BPLIB_ADU_START_STAT_ERR            ((BPLib_Status_t) -21u)
#define BPLIB_ADU_START_SUB_ERR             ((BPLib_Status_t) -22u)
#define BPLIB_ADU_STOP_CHAN_ERR             ((BPLib_Status_t) -23u)
#define BPLIB_ADU_STOP_STAT_ERR             ((BPLib_Status_t) -24u)
#define BPLIB_ADU_STOP_UNSUB_ERR            ((BPLib_Status_t) -25u)

/* Node Configuration (NC) errors */
#define BPLIB_NC_INVALID_MIB_ITEM_INDEX     ((BPLib_Status_t) -26u)
#define BPLIB_NC_INVALID_MID_VALUE          ((BPLib_Status_t) -27u)

/* CLA Errors*/
#define BPLIB_CLA_TIMEOUT                   ((BPLib_Status_t) -28u)

/* Payload Interface Errors */
#define BPLIB_PI_INVALID_CONFIG_ERROR       ((BPLib_Status_t) -29u)

/* Admin Statistics Errors */
#define BPLIB_INVALID_NODE_CNTR_INDEX       ((BPLib_Status_t) -30u)

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_API_TYPES_H */
