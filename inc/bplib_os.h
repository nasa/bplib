/************************************************************************
 * File: bplib_os.h
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
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

#ifndef _bplib_os_h_
#define _bplib_os_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>

/******************************************************************************
 COMPILE TIME CONFIGURATION SETTINGS
 ******************************************************************************/

#ifndef BP_VAL_TYPE
#define BP_VAL_TYPE unsigned long
#endif

#ifndef BP_INDEX_TYPE
#define BP_INDEX_TYPE uint16_t
#endif

#ifndef BP_LOCAL_SCOPE
#define BP_LOCAL_SCOPE static
#endif

#ifdef _GNU_
#define VARG_CHECK(f, a, b) __attribute__((format(f, a, b)))
#else
#define VARG_CHECK(f, a, b)
#endif

/******************************************************************************
 DEFINES
 ******************************************************************************/

/* Macros */
#define BP_GET_MAXVAL(t)        (0xFFFFFFFFFFFFFFFFllu >> (64 - (sizeof(t) * 8)))
#define bplog(flags,evt,...)    bplib_os_log(__FILE__,__LINE__,flags,evt,__VA_ARGS__)

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Encoded Value (bounds size of bundle field values) */
typedef BP_VAL_TYPE bp_val_t;
#define BP_MAX_ENCODED_VALUE BP_GET_MAXVAL(bp_val_t)

/* Index (bounds size of active table) */
typedef BP_INDEX_TYPE bp_index_t;
#define BP_MAX_INDEX BP_GET_MAXVAL(bp_index_t)

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

void        bplib_os_init           (void);
int         bplib_os_log            (const char* file, unsigned int line, uint32_t* flags, uint32_t error, const char* fmt, ...) VARG_CHECK(printf, 4, 5);
int         bplib_os_systime        (unsigned long* sysnow); /* seconds */
void        bplib_os_sleep          (int seconds);
uint32_t    bplib_os_random         (void);
int         bplib_os_createlock     (void);
void        bplib_os_destroylock    (int handle);
void        bplib_os_lock           (int handle);
void        bplib_os_unlock         (int handle);
void        bplib_os_signal         (int handle);
int         bplib_os_waiton         (int handle, int timeout_ms);
int         bplib_os_format         (char* dst, size_t len, const char* fmt, ...) VARG_CHECK(printf, 3, 4);
int         bplib_os_strnlen        (const char* str, int maxlen);
void*       bplib_os_calloc         (size_t size);
void        bplib_os_free           (void* ptr);
size_t      bplib_os_memused        (void);
size_t      bplib_os_memhigh        (void);

#endif /* _bplib_os_h_ */
