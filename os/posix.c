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
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#include "bplib.h"
#include "bplib_os.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define UNIX_SECS_AT_2000     946684800
#define BP_MAX_LOG_ENTRY_SIZE 256
#define BP_MAX_LOCKS          128

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct
{
    pthread_cond_t  cond;
    pthread_mutex_t mutex;
} bplib_os_lock_t;

/******************************************************************************
 FILE DATA
 ******************************************************************************/

static bplib_os_lock_t *locks[BP_MAX_LOCKS] = {0};
static pthread_mutex_t  lock_of_locks;

static struct timespec prevnow;

static uint32_t flag_log_enable = BP_FLAG_NONCOMPLIANT | BP_FLAG_DROPPED | BP_FLAG_BUNDLE_TOO_LARGE |
                                  BP_FLAG_UNKNOWNREC | BP_FLAG_INVALID_CIPHER_SUITEID |
                                  BP_FLAG_INVALID_BIB_RESULT_TYPE | BP_FLAG_INVALID_BIB_TARGET_TYPE |
                                  BP_FLAG_FAILED_TO_PARSE | BP_FLAG_API_ERROR;

static inline uint64_t bplib_timespec_to_u64(const struct timespec *ts)
{
    uint64_t result;

    /* Convert to milliseconds */
    result = (uint64_t)ts->tv_sec * 1000;
    result += (ts->tv_nsec / 1000000);

    return result;
}
/******************************************************************************
 EXPORTED UTILITY FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_os_enable_log_flags -
 *-------------------------------------------------------------------------------------*/
void bplib_os_enable_log_flags(uint32_t enable_mask)
{
    flag_log_enable = enable_mask;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_os_init -
 *-------------------------------------------------------------------------------------*/
void bplib_os_init()
{
    pthread_mutexattr_t locks_attr;
    pthread_mutexattr_init(&locks_attr);
    pthread_mutexattr_settype(&locks_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&lock_of_locks, &locks_attr);

    clock_gettime(CLOCK_REALTIME, &prevnow);

    srand((unsigned int)prevnow.tv_nsec);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_log -
 *
 * Returns - the error code passed in (for convenience)
 *-------------------------------------------------------------------------------------*/
int bplib_os_log(const char *file, unsigned int line, uint32_t *flags, uint32_t event, const char *fmt, ...)
{
    if ((flag_log_enable & event) == event)
    {
        char    formatted_string[BP_MAX_LOG_ENTRY_SIZE];
        va_list args;
        int     vlen, msglen;

        /* Build Formatted String */
        va_start(args, fmt);
        vlen   = vsnprintf(formatted_string, BP_MAX_LOG_ENTRY_SIZE - 1, fmt, args);
        msglen = vlen < BP_MAX_LOG_ENTRY_SIZE - 1 ? vlen : BP_MAX_LOG_ENTRY_SIZE - 1;
        va_end(args);

        /* Log Message */
        if (msglen > 0)
        {
            char  log_message[BP_MAX_LOG_ENTRY_SIZE];
            char *pathptr;

            formatted_string[msglen] = '\0';

            /* Chop Path in Filename */
            pathptr = strrchr(file, '/');
            if (pathptr)
                pathptr++;
            else
                pathptr = (char *)file;

            /* Create Log Message */
            if (event != BP_FLAG_DIAGNOSTIC)
            {
                msglen = snprintf(log_message, BP_MAX_LOG_ENTRY_SIZE, "%s:%u:%08X:%s", pathptr, line, event,
                                  formatted_string);
            }
            else
            {
                msglen = snprintf(log_message, BP_MAX_LOG_ENTRY_SIZE, "%s:%u:%s", pathptr, line, formatted_string);
            }

            /* Provide Truncation Indicator */
            if (msglen > (BP_MAX_LOG_ENTRY_SIZE - 2))
            {
                log_message[BP_MAX_LOG_ENTRY_SIZE - 2] = '#';
            }

            /* Display Log Message */
            fputs(log_message, stderr);
        }
    }

    /* Set Event Flag and Return */
    if (event > 0)
    {
        if (flags)
            *flags |= event;
        return BP_ERROR;
    }
    else
    {
        return BP_SUCCESS;
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_os_get_dtntime_ms - returns milliseconds since DTN epoch
 * this should be compatible with the BPv7 time definition
 *-------------------------------------------------------------------------------------*/
uint64_t bplib_os_get_dtntime_ms(void)
{
    struct timespec now;

    /*
     * This needs to map to the DTN epoch of 2000-01-01 00:00:00 UTC, so it needs to use
     * REALTIME here.  For now this ignores the leap second problem (spec section 4.2.6
     * only says "DTN time is not affected by leap seconds" but not exactly what is
     * meant by that).  There are two ways to ignore leap seconds - just count
     * real seconds that have elapsed and make no corrections at all for leaps (which
     * is what TAI does), or to pretend there are always exactly 86400 seconds in a "day"
     * and ignore the fact that leap seconds break this assumption (which is what POSIX
     * does).
     *
     * The result is that in 2022 there will be a 5 second difference in the result, depending
     * on which method of ignoring the leap second problem is chosen.  This will use the
     * POSIX method because that's what the system clock gives us.
     */

    /*
     * Get System Time.
     * Worth also noting here that CLOCK_REALTIME is a settable/modifiable
     * clock in POSIX systems, which means there is no guarantee that this is
     * monotonically increasing - it may jump/step, and may go backwards.
     * There may be a justification for using CLOCK_MONOTONIC for timeout
     * counters or other items that are always based on relative time.
     */
    if (clock_gettime(CLOCK_REALTIME, &now) < 0 || now.tv_sec < UNIX_SECS_AT_2000)
    {
        return 0; /* This is BP-speak for unknown time */
    }

    /* Convert to DTN epoch */
    now.tv_sec -= UNIX_SECS_AT_2000;

    /* Convert to milliseconds */
    return bplib_timespec_to_u64(&now);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_systime - returns seconds
 *-------------------------------------------------------------------------------------*/
int bplib_os_systime(unsigned long *sysnow)
{
    int status = BP_SUCCESS;

    /* Get System Time */
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    unsigned long elapsed_secs  = now.tv_sec - UNIX_SECS_AT_2000;
    unsigned long previous_secs = prevnow.tv_sec - UNIX_SECS_AT_2000;
    prevnow                     = now;

    /* Return Time */
    if (sysnow)
        *sysnow = elapsed_secs;

    /* Check Reliability */
    if ((now.tv_sec < UNIX_SECS_AT_2000) || /* time nonsensical */
        (previous_secs > elapsed_secs))     /* time going backwards */
    {
        status = BP_ERROR;
    }

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_sleep
 *-------------------------------------------------------------------------------------*/
void bplib_os_sleep(int seconds)
{
    sleep(seconds);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_random -
 *-------------------------------------------------------------------------------------*/
uint32_t bplib_os_random(void)
{
    return (uint32_t)rand();
}

/*--------------------------------------------------------------------------------------
 * bplib_os_createlock -
 *-------------------------------------------------------------------------------------*/
bp_handle_t bplib_os_createlock(void)
{
    bp_handle_t handle = BP_INVALID_HANDLE;

    pthread_mutex_lock(&lock_of_locks);
    {
        int i;
        for (i = 0; i < BP_MAX_LOCKS; i++)
        {
            if (locks[i] == NULL)
            {
                locks[i] = (bplib_os_lock_t *)bplib_os_calloc(sizeof(bplib_os_lock_t));
                if (locks[i])
                {
                    pthread_mutexattr_t attr;
                    pthread_mutexattr_init(&attr);
                    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
                    pthread_mutex_init(&locks[i]->mutex, &attr);
                    pthread_cond_init(&locks[i]->cond, NULL);
                    handle = bp_handle_from_serial(i, BPLIB_HANDLE_OS_BASE);
                    break;
                }
            }
        }
    }
    pthread_mutex_unlock(&lock_of_locks);

    return handle;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_destroylock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_destroylock(bp_handle_t h)
{
    int handle = bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE);

    pthread_mutex_lock(&lock_of_locks);
    {
        if (locks[handle])
        {
            pthread_mutex_destroy(&locks[handle]->mutex);
            pthread_cond_destroy(&locks[handle]->cond);
            bplib_os_free(locks[handle]);
            locks[handle] = NULL;
        }
    }
    pthread_mutex_unlock(&lock_of_locks);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_lock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_lock(bp_handle_t h)
{
    int handle = bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE);

    pthread_mutex_lock(&locks[handle]->mutex);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_unlock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_unlock(bp_handle_t h)
{
    int handle = bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE);

    pthread_mutex_unlock(&locks[handle]->mutex);
}

void bplib_os_broadcast_signal_and_unlock(bp_handle_t h)
{
    bplib_os_lock_t *lock = locks[bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE)];

    pthread_cond_broadcast(&lock->cond);
    pthread_mutex_unlock(&lock->mutex);
}

void bplib_os_broadcast_signal(bp_handle_t h)
{
    bplib_os_lock_t *lock = locks[bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE)];

    pthread_cond_broadcast(&lock->cond);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_signal -
 *-------------------------------------------------------------------------------------*/
void bplib_os_signal(bp_handle_t h)
{
    int handle = bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE);

    pthread_cond_signal(&locks[handle]->cond);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_waiton -
 *-------------------------------------------------------------------------------------*/
int bplib_os_waiton(bp_handle_t h, int timeout_ms)
{
    int handle = bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE);
    int status;

    /* Perform Wait */
    if (timeout_ms == -1)
    {
        /* Block Forever until Success */
        status = pthread_cond_wait(&locks[handle]->cond, &locks[handle]->mutex);
        if (status != 0)
        {
            status = BP_ERROR;
        }
        else
        {
            status = BP_SUCCESS;
        }
    }
    else if (timeout_ms > 0)
    {
        /* Build Time Structure */
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += (time_t)(timeout_ms / 1000);
        ts.tv_nsec += (timeout_ms % 1000) * 1000000L;
        if (ts.tv_nsec >= 1000000000L)
        {
            ts.tv_nsec -= 1000000000L;
            ts.tv_sec++;
        }

        /* Block on Timed Wait and Update Timeout */
        status = pthread_cond_timedwait(&locks[handle]->cond, &locks[handle]->mutex, &ts);
        if (status == ETIMEDOUT)
        {
            status = BP_TIMEOUT;
        }
        else
        {
            status = BP_SUCCESS;
        }
    }
    else /* timeout_ms = 0 */
    {
        /* conditional does not support a non-blocking attempt
         * so treat it as an immediate timeout */
        status = BP_TIMEOUT;
    }

    /* Return Status */
    return status;
}

int bplib_os_wait_until_ms(bp_handle_t h, uint64_t abs_dtntime_ms)
{
    bplib_os_lock_t *lock = locks[bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE)];
    struct timespec  until_time;
    int              status;

    if (abs_dtntime_ms == BP_DTNTIME_INFINITE)
    {
        /* Block Forever until Success */
        status = pthread_cond_wait(&lock->cond, &lock->mutex);
    }
    else
    {
        until_time.tv_sec  = (abs_dtntime_ms / 1000);
        until_time.tv_nsec = (abs_dtntime_ms % 1000) * 1000000;

        /* change the epoch from DTN (2000) to UNIX (1970) */
        until_time.tv_sec += UNIX_SECS_AT_2000;

        /* Block on Timed Wait and Update Timeout */
        status = pthread_cond_timedwait(&lock->cond, &lock->mutex, &until_time);
    }

    /* check for timeout error explicitly and translate to BP_TIMEOUT */
    if (status == ETIMEDOUT)
    {
        return BP_TIMEOUT;
    }

    /* other unexpected/unhandled errors become BP_ERROR */
    if (status != 0)
    {
        return BP_ERROR;
    }

    /* status of 0 indicates success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_format -
 *-------------------------------------------------------------------------------------*/
int bplib_os_format(char *dst, size_t len, const char *fmt, ...)
{
    va_list args;
    int     vlen;

    /* Build Formatted String */
    va_start(args, fmt);
    vlen = vsnprintf(dst, len, fmt, args);
    va_end(args);

    /* Return Error Code */
    return vlen;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_strnlen -
 *-------------------------------------------------------------------------------------*/
int bplib_os_strnlen(const char *str, int maxlen)
{
    int len;
    for (len = 0; len < maxlen; len++)
    {
        if (str[len] == '\0')
        {
            return len;
        }
    }
    return maxlen;
}
