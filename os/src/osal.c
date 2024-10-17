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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "common_types.h"
#include "osapi.h"
#include "bplib.h"
#include "bplib_os.h"

/*
 * The OSAL "OS_GetLocalTime()" API is assumed to use the same POSIX epoch,
 * so the conversion to DTN Time is the same as for POSIX.
 *
 * Note that this does NOT use the CFE TIME "MET" API for DTN time, for several
 * reasons -  the epoch isn't well defined and not easy to convert, and the way
 * it is updated via "time at tone" messages may cause jumps or skips that would
 * potentially disrupt DTN state machines.
 *
 * DTN state machines need a relatively stable clock to work correctly, and thus
 * it is recommended to employ some sort of background clock sync service (i.e.
 * on a ground system this could be NTP, or suitable substitute for flight).
 * Such a service runs independently of BPLib, but should manage the same clock
 * that is being used by OSAL here, so BP/DTN will have a stable clock.
 */
#define OSAL_LOCALTIME_SECS_AT_2000 946684800

/**
 * Conversion factor between OSAL time and DTN time.
 * This is an epoch offset expressed as OS_time_t.  The
 * value should be treated as const, but it must be declared
 * non-const here because it needs to be calculated at runtime
 *
 * The time conversion routines provided by OSAL are all runtime
 * functions and thus cannot be used to initialize const data.
 */
static OS_time_t BPLIB_OSAL_LOCALTIME_DTN_CONV;

/******************************************************************************
 FILE DATA
 ******************************************************************************/

static osal_id_t file_data_lock;

unsigned int bplib_os_next_serial(void)
{
    static unsigned int last_serial_num = 0;
    unsigned int        next_serial;

    OS_MutSemTake(file_data_lock);
    next_serial = ++last_serial_num;
    OS_MutSemGive(file_data_lock);

    return next_serial;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_os_init -
 *-------------------------------------------------------------------------------------*/
void bplib_os_init(void)
{
    OS_MutSemCreate(&file_data_lock, "bplib_osal", 0);

    BPLIB_OSAL_LOCALTIME_DTN_CONV = OS_TimeAssembleFromMilliseconds(OSAL_LOCALTIME_SECS_AT_2000, 0);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_sleep
 *-------------------------------------------------------------------------------------*/
void bplib_os_sleep(int seconds)
{
    OS_TaskDelay(seconds * 1000);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_createlock -
 *-------------------------------------------------------------------------------------*/
bp_handle_t bplib_os_createlock(void)
{
    char      lock_name[OS_MAX_API_NAME];
    int32     status;
    osal_id_t id;

    snprintf(lock_name, sizeof(lock_name), "bpl%02u", bplib_os_next_serial());
    status = OS_CondVarCreate(&id, lock_name, 0);
    if (status != OS_SUCCESS)
    {
        return BP_INVALID_HANDLE;
    }

    return bp_handle_from_serial(OS_ObjectIdToInteger(id), BPLIB_HANDLE_OS_BASE);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_destroylock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_destroylock(bp_handle_t h)
{
    osal_id_t id;

    id = OS_ObjectIdFromInteger(bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE));

    OS_CondVarDelete(id);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_lock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_lock(bp_handle_t h)
{
    osal_id_t id;

    id = OS_ObjectIdFromInteger(bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE));

    OS_CondVarLock(id);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_unlock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_unlock(bp_handle_t h)
{
    osal_id_t id;

    id = OS_ObjectIdFromInteger(bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE));

    OS_CondVarUnlock(id);
}

void bplib_os_broadcast_signal_and_unlock(bp_handle_t h)
{
    osal_id_t id;

    id = OS_ObjectIdFromInteger(bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE));

    OS_CondVarBroadcast(id);
    OS_CondVarUnlock(id);
}

void bplib_os_broadcast_signal(bp_handle_t h)
{
    osal_id_t id;

    id = OS_ObjectIdFromInteger(bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE));

    OS_CondVarBroadcast(id);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_signal -
 *-------------------------------------------------------------------------------------*/
void bplib_os_signal(bp_handle_t h)
{
    osal_id_t id;

    id = OS_ObjectIdFromInteger(bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE));

    OS_CondVarSignal(id);
}

int bplib_os_wait_until_ms(bp_handle_t h, BPLib_TIME_MonotonicTime_t abs_time)
{
    osal_id_t id;
    OS_time_t until_time;
    int32     status;

    id = OS_ObjectIdFromInteger(bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE));

    if (BPLIB_TIME_IS_INFINITE(abs_time))
    {
        /* Block Forever until Success */
        status = OS_CondVarWait(id);
    }
    else
    {
        until_time = OS_TimeFromTotalMilliseconds(BPLIB_TIME_TO_INT(abs_time));

        /* change the epoch from DTN (2000) to OSAL */
        until_time = OS_TimeAdd(until_time, BPLIB_OSAL_LOCALTIME_DTN_CONV);

        /* Block on Timed Wait and Update Timeout */
        status = OS_CondVarTimedWait(id, &until_time);
    }

    /* check for timeout error explicitly and translate to BP_TIMEOUT */
    if (status == OS_ERROR_TIMEOUT)
    {
        return BPLIB_TIMEOUT;
    }

    /* other unexpected/unhandled errors become BPLIB_ERROR */
    if (status != OS_SUCCESS)
    {
        return BPLIB_ERROR;
    }

    /* status of 0 indicates success */
    return BPLIB_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_get_monotime_raw -
 *
 * Returns - the milliseconds elapsed since an undefined (but consistent) epoch
 * The epoch only changes on a system boot, and typically is the boot time.
 *-------------------------------------------------------------------------------------*/
uint64_t bplib_os_get_monotime_raw(void)
{
    /* jphfix */
    return 0;

#ifdef jphfix
    struct timespec tm;

    clock_gettime(CLOCK_MONOTONIC, &tm);

    /* monotonic time can be used directly, epoch does not matter */
    return bplib_timespec_to_u64(&tm);
#endif
}


/*--------------------------------------------------------------------------------------
 * bplib_os_get_dtntime_ms - returns milliseconds since DTN epoch
 * this should be compatible with the BPv7 time definition
 *-------------------------------------------------------------------------------------*/
uint64_t    bplib_os_get_dtntime_raw(void)
{
    OS_time_t ref_tm;

    /*
     * The OSAL "GetLocalTime" API generally returns a POSIX-like time,
     * with an epoch of 1970-01-01 00:00:00 UTC and assuming every "day"
     * has exactly 86400 seconds.  That is, adding (or subtracting)
     * 86400 seconds from a POSIX time will yield the exact same time of
     * day but on the next (or previous) day.  Leap seconds are generally
     * ignored (that is, similar to clock drift or noise that might make
     * it slightly off from true UTC but then possibly corrected over time
     * if running some sort of time sync in the background)
     */
    OS_GetLocalTime(&ref_tm);

    /* Convert to DTN epoch */
    ref_tm = OS_TimeSubtract(ref_tm, BPLIB_OSAL_LOCALTIME_DTN_CONV);

    /* Convert to milliseconds */
    return OS_TimeGetTotalMilliseconds(ref_tm);
}
