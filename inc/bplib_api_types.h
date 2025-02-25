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
#include <string.h>

/*
** Type Definitions
*/

/**
 * @brief Canonical block type
 *
 * @note The numeric values match the block type values in BPv7 section 9.1. The v6 types 
 *       are not included.
 */
enum BPLib_BlockType
{
    BPLib_BlockType_Reserved = 0,
    BPLib_BlockType_Payload = 1,
    BPLib_BlockType_PrevNode = 6,
    BPLib_BlockType_Age = 7,
    BPLib_BlockType_HopCount = 10
};

typedef uint8_t BPLib_BlockType_t;

/**
 * \brief BPLib status type for type safety
 */
typedef int32_t BPLib_Status_t;

typedef struct BPLib_handle
{
    uint32_t hdl;
} BPLib_Handle_t;

// Integer typedefs

typedef uintmax_t BPLib_Val_t;
typedef intmax_t  BPLib_Sval_t;
typedef uint16_t  BPLib_Index_t;

// Bundle Protocol Typedefs

/* IPN Schema Endpoint ID Integer Definition */
typedef BPLib_Val_t BPLib_Ipn_t;

/* combine IPN node+service */
typedef struct BPLib_IpnAddr
{
    BPLib_Ipn_t node_number;
    BPLib_Ipn_t service_number;
} BPLib_IpnAddr_t;

/*
** Macros
*/

#define BPLIB_BUNDLE_PROTOCOL_VERSION       (7)     /** @brief Version of Bundle Protocol being implemented */

/** @defgroup BPLib_ReturnCodes BPLib Return Codes
 * @{
 */
/* General Return Codes */
#define BPLIB_TBL_UPDATED                   ((BPLib_Status_t)  1)  /* Table has been updated */
#define BPLIB_SUCCESS                       ((BPLib_Status_t)  0)  /* Successful execution */
#define BPLIB_ERROR                         ((BPLib_Status_t) -1)  /* Failed execution */
#define BPLIB_UNIMPLEMENTED                 ((BPLib_Status_t) -2)  /* Unimplemented function */
#define BPLIB_UNKNOWN                       ((BPLib_Status_t) -3)  /* Unknown return status */
#define BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE   ((BPLib_Status_t) -4)  /* Table validation error code */
#define BPLIB_RBT_DUPLICATE                 ((BPLib_Status_t) -5)  /* BPLib Red-Black Tree (RBT) Duplicate Search Result */
#define BPLIB_TIMEOUT                       ((BPLib_Status_t) -6)  /* Timeout pending on a queue */
#define BPLIB_NULL_PTR_ERROR                ((BPLib_Status_t) -7)  /* Null pointer error */
#define BPLIB_BUF_LEN_ERROR                 ((BPLib_Status_t) -8)  /* Buffer length error */
#define BPLIB_INVALID_EID                   ((BPLib_Status_t) -9)  /* Invalid endpoint identification */
#define BPLIB_INVALID_EID_PATTERN           ((BPLib_Status_t) -10) /* Invalid endpoint identification pattern */
/*
#define BPLIB_GENERIC_ERROR_11              ((BPLib_Status_t) -11) // Error description
#define BPLIB_GENERIC_ERROR_12              ((BPLib_Status_t) -12) // Error description
#define BPLIB_GENERIC_ERROR_13              ((BPLib_Status_t) -13) // Error description
#define BPLIB_GENERIC_ERROR_14              ((BPLib_Status_t) -14) // Error description
#define BPLIB_GENERIC_ERROR_15              ((BPLib_Status_t) -15) // Error description
#define BPLIB_GENERIC_ERROR_16              ((BPLib_Status_t) -16) // Error description
#define BPLIB_GENERIC_ERROR_17              ((BPLib_Status_t) -17) // Error description
#define BPLIB_GENERIC_ERROR_18              ((BPLib_Status_t) -18) // Error description
#define BPLIB_GENERIC_ERROR_19              ((BPLib_Status_t) -19) // Error description
#define BPLIB_GENERIC_ERROR_20              ((BPLib_Status_t) -20) // Error description
#define BPLIB_GENERIC_ERROR_21              ((BPLib_Status_t) -21) // Error description
#define BPLIB_GENERIC_ERROR_22              ((BPLib_Status_t) -22) // Error description
#define BPLIB_GENERIC_ERROR_23              ((BPLib_Status_t) -23) // Error description
#define BPLIB_GENERIC_ERROR_24              ((BPLib_Status_t) -24) // Error description
#define BPLIB_GENERIC_ERROR_25              ((BPLib_Status_t) -25) // Error description
*/

/** @defgroup BPLib_ErrorCodes BPLib Error Code Defines
 * @{
 */
/* Framework Proxy Errors */
#define BPLIB_FWP_CALLBACK_INIT_ERROR       ((BPLib_Status_t) -26)

/* Time Management Errors */
#define BPLIB_TIME_UNDEF_DELTA_ERROR        ((BPLib_Status_t) -27)
#define BPLIB_TIME_WRITE_ERROR              ((BPLib_Status_t) -28)
#define BPLIB_TIME_READ_ERROR               ((BPLib_Status_t) -29)
#define BPLIB_TIME_UNINIT_ERROR             ((BPLib_Status_t) -30)

/* Event Management Errors */
#define BPLIB_EM_STRING_TRUNCATED           ((BPLib_Status_t) -31)
#define BPLIB_EM_ILLEGAL_APP_ID             ((BPLib_Status_t) -32)
#define BPLIB_EM_UNKNOWN_FILTER             ((BPLib_Status_t) -33)
#define BPLIB_EM_BAD_ARGUMENT               ((BPLib_Status_t) -34)
#define BPLIB_EM_INVALID_PARAMETER          ((BPLib_Status_t) -35)
#define BPLIB_EM_APP_NOT_REGISTERED         ((BPLib_Status_t) -36)
#define BPLIB_EM_APP_SQUELCHED              ((BPLib_Status_t) -37)
#define BPLIB_EM_EXPANDED_TEXT_ERROR        ((BPLib_Status_t) -38)

/* PerfLog Proxy Errors*/
#define BPLIB_PL_NULL_CALLBACK_ERROR        ((BPLib_Status_t) -39)

/* Node Configuration (NC) errors */
#define BPLIB_NC_INIT_CONFIG_PTRS_ERROR     ((BPLib_Status_t) -40)
#define BPLIB_NC_INVALID_MIB_ITEM_INDEX     ((BPLib_Status_t) -41)
#define BPLIB_NC_INVALID_MID_VALUE          ((BPLib_Status_t) -42)

/* CLA Errors*/
#define BPLIB_CLA_TIMEOUT                   ((BPLib_Status_t) -43)

/* Payload Interface Errors */
#define BPLIB_PI_INVALID_CONFIG_ERROR       ((BPLib_Status_t) -44)
#define BPLIB_PI_TIMEOUT                    ((BPLib_Status_t) -45)

/* Admin Statistics (AS) Errors */
#define BPLIB_AS_INIT_MUTEX_ERR             ((BPLib_Status_t) -46)
#define BPLIB_AS_INVALID_EID                ((BPLib_Status_t) -47)
#define BPLIB_AS_UNKNOWN_NODE_CNTR          ((BPLib_Status_t) -48)
#define BPLIB_AS_UNKNOWN_SRC_CNTR           ((BPLib_Status_t) -49)
#define BPLIB_AS_UNKNOWN_MIB_ARRAY_EID      ((BPLib_Status_t) -50)
#define BPLIB_AS_MIB_KEY_ARRAY_FULL         ((BPLib_Status_t) -51)
#define BPLIB_AS_MIB_KEYS_OVERLAP           ((BPLib_Status_t) -52)
#define BPLIB_AS_INVALID_MIB_INDEX          ((BPLib_Status_t) -53)
#define BPLIB_AS_NO_KEYS_GIVEN              ((BPLib_Status_t) -54)

/* Queue Manager (QM) Errors */
#define BPLIB_QM_PUSH_ERROR                 ((BPLib_Status_t) -55)

/* MEM Errors */
#define BPLIB_MEM_INITMEM_UNALIGN           ((BPLib_Status_t) -56)


#define BPLIB_NC_TBL_UPDATE_ERR             ((BPLib_Status_t) -57)

// TODO TIME Helpers

// Candidates for inclusion in bplib_time.h or in bplib_stor_cache_types.h.
#define BPLIB_TIME_TO_INT(t)   ((t).Time)
#define BPLIB_TIME_FROM_INT(t) { (t) }
#define BPLIB_TIME_IS_VALID(t) (BPLIB_TIME_TO_INT(t) != 0)
#define BPLIB_TIME_IS_INFINITE(t) (BPLIB_TIME_TO_INT(t) == UINT64_MAX)

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_API_TYPES_H */
