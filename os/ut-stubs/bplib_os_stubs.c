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

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in bplib_os header
 */

#include <stdarg.h>

#include "bplib_os.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_broadcast_signal()
 * ----------------------------------------------------
 */
void bplib_os_broadcast_signal(bp_handle_t h)
{
    UT_GenStub_AddParam(bplib_os_broadcast_signal, bp_handle_t, h);

    UT_GenStub_Execute(bplib_os_broadcast_signal, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_broadcast_signal_and_unlock()
 * ----------------------------------------------------
 */
void bplib_os_broadcast_signal_and_unlock(bp_handle_t h)
{
    UT_GenStub_AddParam(bplib_os_broadcast_signal_and_unlock, bp_handle_t, h);

    UT_GenStub_Execute(bplib_os_broadcast_signal_and_unlock, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_calloc()
 * ----------------------------------------------------
 */
void *bplib_os_calloc(size_t size)
{
    UT_GenStub_SetupReturnBuffer(bplib_os_calloc, void *);

    UT_GenStub_AddParam(bplib_os_calloc, size_t, size);

    UT_GenStub_Execute(bplib_os_calloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_os_calloc, void *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_createlock()
 * ----------------------------------------------------
 */
bp_handle_t bplib_os_createlock(void)
{
    UT_GenStub_SetupReturnBuffer(bplib_os_createlock, bp_handle_t);

    UT_GenStub_Execute(bplib_os_createlock, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_os_createlock, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_destroylock()
 * ----------------------------------------------------
 */
void bplib_os_destroylock(bp_handle_t h)
{
    UT_GenStub_AddParam(bplib_os_destroylock, bp_handle_t, h);

    UT_GenStub_Execute(bplib_os_destroylock, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_enable_log_flags()
 * ----------------------------------------------------
 */
void bplib_os_enable_log_flags(uint32_t enable_mask)
{
    UT_GenStub_AddParam(bplib_os_enable_log_flags, uint32_t, enable_mask);

    UT_GenStub_Execute(bplib_os_enable_log_flags, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_free()
 * ----------------------------------------------------
 */
void bplib_os_free(void *ptr)
{
    UT_GenStub_AddParam(bplib_os_free, void *, ptr);

    UT_GenStub_Execute(bplib_os_free, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_get_dtntime_raw()
 * ----------------------------------------------------
 */
uint64_t bplib_os_get_dtntime_raw(void)
{
    UT_GenStub_SetupReturnBuffer(bplib_os_get_dtntime_raw, uint64_t);

    UT_GenStub_Execute(bplib_os_get_dtntime_raw, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_os_get_dtntime_raw, uint64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_get_monotime_raw()
 * ----------------------------------------------------
 */
uint64_t bplib_os_get_monotime_raw(void)
{
    UT_GenStub_SetupReturnBuffer(bplib_os_get_monotime_raw, uint64_t);

    UT_GenStub_Execute(bplib_os_get_monotime_raw, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_os_get_monotime_raw, uint64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_init()
 * ----------------------------------------------------
 */
void bplib_os_init(void)
{

    UT_GenStub_Execute(bplib_os_init, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_lock()
 * ----------------------------------------------------
 */
void bplib_os_lock(bp_handle_t h)
{
    UT_GenStub_AddParam(bplib_os_lock, bp_handle_t, h);

    UT_GenStub_Execute(bplib_os_lock, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_log()
 * ----------------------------------------------------
 */
int bplib_os_log(const char *file, unsigned int line, uint32_t *flags, uint32_t event, const char *fmt, ...)
{
    va_list UtStub_ArgList;

    UT_GenStub_SetupReturnBuffer(bplib_os_log, int);

    UT_GenStub_AddParam(bplib_os_log, const char *, file);
    UT_GenStub_AddParam(bplib_os_log, unsigned int, line);
    UT_GenStub_AddParam(bplib_os_log, uint32_t *, flags);
    UT_GenStub_AddParam(bplib_os_log, uint32_t, event);
    UT_GenStub_AddParam(bplib_os_log, const char *, fmt);

    va_start(UtStub_ArgList, fmt);
    UT_GenStub_Execute(bplib_os_log, Va, NULL, UtStub_ArgList);
    va_end(UtStub_ArgList);

    return UT_GenStub_GetReturnValue(bplib_os_log, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_random()
 * ----------------------------------------------------
 */
uint32_t bplib_os_random(void)
{
    UT_GenStub_SetupReturnBuffer(bplib_os_random, uint32_t);

    UT_GenStub_Execute(bplib_os_random, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_os_random, uint32_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_signal()
 * ----------------------------------------------------
 */
void bplib_os_signal(bp_handle_t h)
{
    UT_GenStub_AddParam(bplib_os_signal, bp_handle_t, h);

    UT_GenStub_Execute(bplib_os_signal, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_sleep()
 * ----------------------------------------------------
 */
void bplib_os_sleep(int seconds)
{
    UT_GenStub_AddParam(bplib_os_sleep, int, seconds);

    UT_GenStub_Execute(bplib_os_sleep, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_unlock()
 * ----------------------------------------------------
 */
void bplib_os_unlock(bp_handle_t h)
{
    UT_GenStub_AddParam(bplib_os_unlock, bp_handle_t, h);

    UT_GenStub_Execute(bplib_os_unlock, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_os_wait_until_ms()
 * ----------------------------------------------------
 */
int bplib_os_wait_until_ms(bp_handle_t h, BPLib_TIME_MonotonicTime_t abs_time)
{
    UT_GenStub_SetupReturnBuffer(bplib_os_wait_until_ms, int);

    UT_GenStub_AddParam(bplib_os_wait_until_ms, bp_handle_t, h);
    UT_GenStub_AddParam(bplib_os_wait_until_ms, BPLib_TIME_MonotonicTime_t, abs_time);

    UT_GenStub_Execute(bplib_os_wait_until_ms, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_os_wait_until_ms, int);
}

