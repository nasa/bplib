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

/* CLA Errors*/
#define BPLIB_CLA_TIMEOUT                   ((BPLib_Status_t) -18u)

/* Payload Interface Errors */
#define BPLIB_PI_INVALID_CONFIG_ERROR       ((BPLib_Status_t) -19u)

//BPLIB_TIMEOUT was -5 in the prototype.
#define BPLIB_TIMEOUT                       ((BPLib_Status_t) -20u)

// BPLib Red-Black Tree (RBT) Duplicate Search Result
#define BPLIB_RBT_DUPLICATE                 ((BPLib_Status_t) -21u)

// BPLib_Handle_t - Multi-purpose handle for locks, APIs, and other allocated resources
#define BPLIB_HANDLE_MAX_SERIAL 0xffffff

#define BPLIB_HANDLE_RAM_STORE_BASE \
    (BPLib_Handle_t)                \
    {                               \
        0x1000000                   \
    }
#define BPLIB_HANDLE_FLASH_STORE_BASE \
    (BPLib_Handle_t)                  \
    {                                 \
        0x2000000                     \
    }
#define BPLIB_HANDLE_FILE_STORE_BASE \
    (BPLib_Handle_t)                 \
    {                                \
        0x3000000                    \
    }
#define BPLIB_HANDLE_OS_BASE \
    (BPLib_Handle_t)         \
    {                        \
        0x4000000            \
    }

#define BPLIB_HANDLE_INTF_BASE \
    (BPLib_Handle_t)           \
    {                          \
        0x5000000              \
    }

// BPLIB_HANDLE_MPOOL_BASE is from heritage bplib_api_types.h, hence the "MPOOL".
#define BPLIB_HANDLE_MPOOL_BASE \
    (BPLib_Handle_t)            \
    {                           \
        0x6000000               \
    }

#define BPLIB_INVALID_HANDLE \
    (const BPLib_Handle_t)   \
    {                        \
        0                    \
    } /* used for integers (os locks, storage services) */

/**
 *  Exported BPLib_Handle_t Functions
 */

/**
 * Checks for validity of given handle
 *
 * @param h the handle value
 * @retval false if the handle is not possibly valid
 * @retval true if the handle may be valid
 */
static inline bool BPLib_HandleIsValid(BPLib_Handle_t h)
{
    return (h.hdl != 0);
}

/**
 * Gets the "printable" value of a handle
 *
 * This may be used in conjunction with the "%d" format
 * specifier on printf()-style calls to display the logical
 * value of a handle.
 *
 * @param h the handle value
 * @returns handle value as an "int"
 */
static inline int BPLib_HandlePrintable(BPLib_Handle_t h)
{
    return (int)h.hdl;
}

/**
 * Checks if two handles are equal
 *
 * Since handles are not integers, one cannot use the == operator
 * to check for equality.  This inline function may be used in its place.
 *
 * @param h1 the first handle value
 * @param h2 the second handle value
 * @retval false if the handles are different
 * @retval true if the handles are equal
 */
static inline bool BPLib_HandleEqual(BPLib_Handle_t h1, BPLib_Handle_t h2)
{
    return (h1.hdl == h2.hdl);
}

/**
 * Converts the given handle to a serial number
 *
 * This determines the 0-based serial number corresponding to a handle,
 * which can in turn be used to index into a table/array.
 *
 * @sa BPLib_HandleFromSerial
 *
 * @param h the handle value
 * @param base the object base handle (indicates the type/class of handle)
 *
 * @returns handle as a serial number
 */
static inline int BPLib_HandleToSerial(BPLib_Handle_t h, BPLib_Handle_t base)
{
    return (h.hdl - base.hdl);
}

/**
 * Converts the given serial number given to a handle
 *
 * This determines the handle value corresponding to a 0-based serial number.
 *
 * @sa BPLib_HandleToSerial
 *
 * @param hv   the handle value
 * @param base the object base handle (indicates the type/class of handle)
 *
 * @returns handle as a serial number
 */
static inline BPLib_Handle_t BPLib_HandleFromSerial(int hv, BPLib_Handle_t base)
{
    return (BPLib_Handle_t) {.hdl = (uint32_t)hv + base.hdl};
}

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
