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

#ifndef _BPLIB_OS_H_
#define _BPLIB_OS_H_

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
 DEFINES
 ******************************************************************************/

/* Operating System Abstraction Return Codes */
#define BP_OS_SUCCESS           1
#define BP_OS_TIMEOUT           0
#define BP_OS_ERROR             (-1)

/* Maximum Encoded Value */
#define BP_MAX_ENCODED_VALUE    ULONG_MAX

/* Logging */
#ifdef _GNU_
#define VARG_CHECK(f, a, b) __attribute__((format(f, a, b)))
#else
#define VARG_CHECK(f, a, b)
#endif
#define bplog(err,...)  bplib_os_log(__FILE__,__LINE__,err,__VA_ARGS__)

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Encoded Value (must be consistent with Maximum Encoded Value)*/
typedef unsigned long bp_val_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

void    bplib_os_init           (void);
int     bplib_os_log            (const char* file, unsigned int line, int error, const char* fmt, ...) VARG_CHECK(printf, 4, 5);
int     bplib_os_systime        (unsigned long* sysnow); /* seconds */
void    bplib_os_sleep          (int seconds);
int     bplib_os_createlock     (void);
void    bplib_os_destroylock    (int handle);
void    bplib_os_lock           (int handle);
void    bplib_os_unlock         (int handle);
void    bplib_os_signal         (int handle);
int     bplib_os_waiton         (int handle, int timeout_ms);
int     bplib_os_format         (char* dst, size_t len, const char* fmt, ...) VARG_CHECK(printf, 3, 4);
int     bplib_os_strnlen        (const char* str, int maxlen);

#endif /* _BPLIB_OS_H_ */
