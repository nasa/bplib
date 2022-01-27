/************************************************************************
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *  All Other Rights Reserved.
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be
 *  used, distributed and modified only pursuant to the terms of that
 *  agreement.
 *
 *************************************************************************/

#ifndef _bplib_api_types_h_
#define _bplib_api_types_h_

/* BP types are based on C99 standard types */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BP_GET_MAXVAL(t)        (UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64 - (sizeof(t) * 8)))

/* this isn't a type, but is required for all the API definitions, so it fits here */
#ifndef BP_LOCAL_SCOPE
#define BP_LOCAL_SCOPE static
#endif

#ifndef BP_VAL_TYPE
#define BP_VAL_TYPE unsigned long
#endif

#ifndef BP_INDEX_TYPE
#define BP_INDEX_TYPE uint16_t
#endif


typedef BP_VAL_TYPE bp_val_t;
typedef BP_INDEX_TYPE bp_index_t;

/* Encoded Value (bounds size of bundle field values) */
#define BP_MAX_ENCODED_VALUE BP_GET_MAXVAL(bp_val_t)

/* Index (bounds size of active table) */
#define BP_MAX_INDEX BP_GET_MAXVAL(bp_index_t)

typedef struct bp_handle
{
    uint32_t hdl;
} bp_handle_t;

/* Handles */
#define BP_INVALID_HANDLE               (const bp_handle_t){0} /* used for integers (os locks, storage services) */

/**
 * @brief Abstract definition of bp_desc for external use
 */
struct bp_desc;

typedef struct bp_desc bp_desc_t;

/* IPN Schema Endpoint ID Integer Definition */
typedef bp_val_t bp_ipn_t;

/* Storage ID */
typedef unsigned long bp_sid_t;

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
 * @param h the handle value
 * @param base the object base handle (indicates the type/class of handle)
 *
 * @returns handle as a serial number
 */
static inline bp_handle_t bp_handle_from_serial(int hv, bp_handle_t base)
{
    return (bp_handle_t){ .hdl = (uint32_t)hv + base.hdl };
}


#define BPLIB_HANDLE_RAM_STORE_BASE   (bp_handle_t){ 0x1000000 }
#define BPLIB_HANDLE_FLASH_STORE_BASE (bp_handle_t){ 0x2000000 }
#define BPLIB_HANDLE_FILE_STORE_BASE  (bp_handle_t){ 0x3000000 }
#define BPLIB_HANDLE_OS_BASE          (bp_handle_t){ 0x4000000 }

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /* _bplib_h_ */
