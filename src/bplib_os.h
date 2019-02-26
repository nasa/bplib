/************************************************************************
 * File: bplib_os.h
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
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

/******************************************************************************
 DEFINES
 ******************************************************************************/

/* Logging */
#ifdef _GNU_
#define VARG_CHECK(f, a, b) __attribute__((format(f, a, b)))
#else
#define VARG_CHECK(f, a, b)
#endif
#define bplog(err,...)  bplib_os_log(__FILE__,__LINE__,err,__VA_ARGS__)

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

void        bplib_os_init           (void);
int         bplib_os_log            (const char* file, unsigned int line, int error, const char* fmt, ...) VARG_CHECK(printf, 4, 5);
void        bplib_os_memset         (void* addr, int val, int len);
void        bplib_os_memcpy         (void* dst, void* src, int len);
size_t      bplib_os_strnlen        (const char *str, size_t maxlen);
uint32_t    bplib_os_systime        (void); // seconds
int         bplib_os_createlock     (void);
void        bplib_os_destroylock    (int handle);
void        bplib_os_lock           (int handle);
void        bplib_os_unlock         (int handle);
void        bplib_os_signal         (int handle);
int         bplib_os_waiton         (int handle, int timeout_ms);

#endif /* _BPLIB_OS_H_ */
