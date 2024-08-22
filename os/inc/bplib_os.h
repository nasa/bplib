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

#ifndef BPLIB_OS_H
#define BPLIB_OS_H

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

bp_handle_t test_bp_hdl;

/******************************************************************************
 COMPILE TIME CONFIGURATION SETTINGS
 ******************************************************************************/

/******************************************************************************
 DEFINES
 ******************************************************************************/

/* Macros */
#define bplog(flags, evt, ...) bplib_os_log(__FILE__, __LINE__, flags, evt, __VA_ARGS__)

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

void        bplib_os_init(void);
void        bplib_os_enable_log_flags(uint32_t enable_mask);
int         bplib_os_log(const char *file, unsigned int line, uint32_t *flags, uint32_t event, const char *fmt, ...);
int         bplib_os_systime(unsigned long *sysnow); /* seconds */
uint64_t    bplib_os_get_dtntime_ms(void);
void        bplib_os_sleep(int seconds);
uint32_t    bplib_os_random(void);
bp_handle_t bplib_os_createlock(void);
void        bplib_os_destroylock(bp_handle_t h);
void        bplib_os_lock(bp_handle_t h);
void        bplib_os_unlock(bp_handle_t h);
void        bplib_os_broadcast_signal(bp_handle_t h);
void        bplib_os_broadcast_signal_and_unlock(bp_handle_t h);
void        bplib_os_signal(bp_handle_t h);
int         bplib_os_waiton(bp_handle_t h, int timeout_ms);
int         bplib_os_wait_until_ms(bp_handle_t h, uint64_t abs_dtntime_ms);
void       *bplib_os_calloc(size_t size);
void        bplib_os_free(void *ptr);

#endif /* BPLIB_OS_H */
