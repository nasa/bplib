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

/* BP types are based on C99 standard types */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Return Codes */
#define BP_SUCCESS   0
#define BP_ERROR     (-1)
#define BP_TIMEOUT   (-2)
#define BP_DUPLICATE (-3)

#define BP_GET_MAXVAL(t) (UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64 - (sizeof(t) * 8)))

/* this isn't a type, but is required for all the API definitions, so it fits here */
#ifndef BP_LOCAL_SCOPE
#define BP_LOCAL_SCOPE static
#endif

typedef uintmax_t bp_val_t;
typedef intmax_t  bp_sval_t;
typedef uint16_t  bp_index_t;

/* Encoded Value (bounds size of bundle field values) */
#define BP_MAX_ENCODED_VALUE BP_GET_MAXVAL(bp_val_t)

/* Index (bounds size of active table) */
#define BP_MAX_INDEX BP_GET_MAXVAL(bp_index_t)

typedef struct bp_handle
{
    uint32_t hdl;
} bp_handle_t;

/* Handles */
#define BP_INVALID_HANDLE \
    (const bp_handle_t)   \
    {                     \
        0                 \
    } /* used for integers (os locks, storage services) */

/**
 * @brief Abstract definition of bp_desc for external use
 */
struct bp_desc;

typedef struct bp_desc   bp_desc_t;
typedef struct bp_socket bp_socket_t;

typedef struct bplib_mpool_block bplib_mpool_block_t;
typedef struct bplib_mpool       bplib_mpool_t;

typedef struct bplib_cache_module_api bplib_cache_module_api_t;

/**
 * @brief A reference to another mpool block
 *
 * @note At this scope the definition of this pointer is abstract.  Application
 * code should use bplib_mpool_dereference() to obtain the pointer to the block
 * that this refers to.
 */
typedef struct bplib_mpool_block_content *bplib_mpool_ref_t;

/**
 * @brief Callback frunction for various mpool block actions
 *
 * This is a generic API for a function to handle various events/conditions
 * that occur at the block level.  The generic argument supplies the context
 * information.
 *
 * The return value depends on the context and may or may not be used, it
 * should should return 0 unless otherwise specified.
 */
typedef int (*bplib_mpool_callback_func_t)(void *, bplib_mpool_block_t *);

/**
 * @brief Type of block CRC calculated by bplib
 *
 * @note the numeric values of this enumeration match the crctype values in the BPv7 spec.
 */
typedef enum bp_crctype
{

    /**
     * @brief No CRC is present.
     */
    bp_crctype_none = 0,

    /**
     * @brief A standard X-25 CRC-16 is present.
     */
    bp_crctype_CRC16 = 1,

    /**
     * @brief A CRC-32 (Castagnoli) is present.
     */
    bp_crctype_CRC32C = 2

} bp_crctype_t;

/*
 * To keep the interface consistent the digest functions do I/O as 32 bit values.
 * For CRC algorithms of lesser width, the value is right-justified (LSB/LSW)
 */
typedef uint32_t bp_crcval_t;

/* IPN Schema Endpoint ID Integer Definition */
typedef bp_val_t bp_ipn_t;

/* combine IPN node+service */
typedef struct bp_ipn_addr
{
    bp_ipn_t node_number;
    bp_ipn_t service_number;
} bp_ipn_addr_t;

/* Storage ID */
typedef unsigned long bp_sid_t;

typedef enum
{
    bplib_policy_delivery_none, /**< best effort handling only, bundle may be forward directly to CLA, no need to store
                                 */
    bplib_policy_delivery_local_ack, /**< use local storage of bundle, locally acknowledge but no node-to-node custody
                                        transfer */
    bplib_policy_delivery_custody_tracking /**< enable full custody transfer signals and acknowledgement (not yet
                                              implemented) */
} bplib_policy_delivery_t;

typedef struct bplib_connection
{
    bp_ipn_addr_t local_ipn;
    bp_ipn_addr_t remote_ipn;
    bp_ipn_addr_t report_ipn;

    bp_val_t lifetime;
    bp_val_t local_retx_interval;

    bool is_admin_service;
    bool allow_fragmentation;

    bp_crctype_t crctype;

    bplib_policy_delivery_t local_delivery_policy;

} bplib_connection_t;

typedef struct bplib_routetbl bplib_routetbl_t;

/* Storage service - reserved for future use */
typedef struct bp_store
{
    uint32_t reserved;
} bp_store_t;

/* Channel Statistics */
typedef struct
{
    /* Errors */
    uint32_t lost;         /* storage failure (load, process, accept) */
    uint32_t expired;      /* bundles, dacs, and payloads with expired lifetime (load, process, accept) */
    uint32_t unrecognized; /* unable to parse input OR bundle type not supported (process) */
    /* Data Flow */
    uint32_t transmitted_bundles;   /* bundles sent for first time, does not includes re-sends (load) */
    uint32_t transmitted_dacs;      /* dacs sent (load) */
    uint32_t retransmitted_bundles; /* bundles timed-out and resent (load) */
    uint32_t delivered_payloads;    /* payloads delivered to application (accept) */
    uint32_t received_bundles;      /* bundles destined for local node (process) */
    uint32_t forwarded_bundles;     /* bundles received by local node but destined for another node (process) */
    uint32_t received_dacs;         /* dacs destined for local node (process) */
    /* Storage */
    uint32_t stored_bundles;  /* number of data bundles currently in storage */
    uint32_t stored_payloads; /* number of payloads currently in storage */
    uint32_t stored_dacs;     /* number of dacs bundles currently in storage */
    /* Active */
    uint32_t acknowledged_bundles; /* freed by custody signal - process */
    uint32_t active_bundles;       /* number of slots in active table in use */
} bp_stats_t;

typedef enum
{
    bplib_variable_none,            /**< reserved value, keep first */
    bplib_variable_mem_current_use, /**< replaces bplib_os_memused() for external API use */
    bplib_variable_mem_high_use,    /**< replaces bplib_os_memhigh() for external API use */
    bplib_variable_max              /**< reserved value, keep last */
} bplib_variable_t;

/**
 * Checks for validity of given handle
 *
 * @param h the handle value
 * @retval false if the handle is not possibly valid
 * @retval true if the handle may be valid
 */
static inline bool bp_handle_is_valid(bp_handle_t h)
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
static inline int bp_handle_printable(bp_handle_t h)
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
static inline bool bp_handle_equal(bp_handle_t h1, bp_handle_t h2)
{
    return (h1.hdl == h2.hdl);
}

/**
 * Converts the given handle to a serial number
 *
 * This determines the 0-based serial number corresponding to a handle,
 * which can in turn be used to index into a table/array.
 *
 * @sa bp_handle_from_serial
 *
 * @param h the handle value
 * @param base the object base handle (indicates the type/class of handle)
 *
 * @returns handle as a serial number
 */
static inline int bp_handle_to_serial(bp_handle_t h, bp_handle_t base)
{
    return (h.hdl - base.hdl);
}

/**
 * Converts the given serial number given to a handle
 *
 * This determines the handle value corresponding to a 0-based serial number.
 *
 * @sa bp_handle_to_serial
 *
 * @param hv   the handle value
 * @param base the object base handle (indicates the type/class of handle)
 *
 * @returns handle as a serial number
 */
static inline bp_handle_t bp_handle_from_serial(int hv, bp_handle_t base)
{
    return (bp_handle_t) {.hdl = (uint32_t)hv + base.hdl};
}

#define BPLIB_HANDLE_MAX_SERIAL 0xffffff

#define BPLIB_HANDLE_RAM_STORE_BASE \
    (bp_handle_t)                   \
    {                               \
        0x1000000                   \
    }
#define BPLIB_HANDLE_FLASH_STORE_BASE \
    (bp_handle_t)                     \
    {                                 \
        0x2000000                     \
    }
#define BPLIB_HANDLE_FILE_STORE_BASE \
    (bp_handle_t)                    \
    {                                \
        0x3000000                    \
    }
#define BPLIB_HANDLE_OS_BASE \
    (bp_handle_t)            \
    {                        \
        0x4000000            \
    }

#define BPLIB_HANDLE_INTF_BASE \
    (bp_handle_t)              \
    {                          \
        0x5000000              \
    }

#define BPLIB_HANDLE_MPOOL_BASE \
    (bp_handle_t)               \
    {                           \
        0x6000000               \
    }

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_API_TYPES_H */
