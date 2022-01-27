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

#include "bplib_api_types.h"

/******************************************************************************
 COMPILE TIME CONFIGURATION SETTINGS
 ******************************************************************************/

#ifdef _GNU_
#define VARG_CHECK(f, a, b) __attribute__((format(f, a, b)))
#else
#define VARG_CHECK(f, a, b)
#endif

/******************************************************************************
 DEFINES
 ******************************************************************************/

/* Macros */
#define bplog(flags,evt,...)    bplib_os_log(__FILE__,__LINE__,flags,evt,__VA_ARGS__)

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

void        bplib_os_init           (void);
int         bplib_os_log            (const char* file, unsigned int line, uint32_t* flags, uint32_t event, const char* fmt, ...) VARG_CHECK(printf, 5, 6);
int         bplib_os_systime        (unsigned long* sysnow); /* seconds */
void        bplib_os_sleep          (int seconds);
uint32_t    bplib_os_random         (void);
bp_handle_t bplib_os_createlock     (void);
void        bplib_os_destroylock    (bp_handle_t h);
void        bplib_os_lock           (bp_handle_t h);
void        bplib_os_unlock         (bp_handle_t h);
void        bplib_os_signal         (bp_handle_t h);
int         bplib_os_waiton         (bp_handle_t h, int timeout_ms);
int         bplib_os_format         (char* dst, size_t len, const char* fmt, ...) VARG_CHECK(printf, 3, 4);
int         bplib_os_strnlen        (const char* str, int maxlen);
void*       bplib_os_calloc         (size_t size);
void        bplib_os_free           (void* ptr);
size_t      bplib_os_memused        (void);
size_t      bplib_os_memhigh        (void);

#endif /* _bplib_os_h_ */
