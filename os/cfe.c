/************************************************************************
 * File: bplib_os_cfe.c
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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "cfe.h"
#include "bplib.h"
#include "bplib_os.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_MAX_LOG_ENTRY_SIZE 256
#define BP_RAND_HASH(seed)    ((seed)*2654435761UL) /* knuth multiplier */

#ifndef BP_CFE_SECS_AT_2000
#define BP_CFE_SECS_AT_2000 1325376023 /* TAI */
#endif

#ifndef BP_BPLIB_INFO_EID
#define BP_BPLIB_INFO_EID 0xFF
#endif

/******************************************************************************
 FILE DATA
 ******************************************************************************/

static size_t current_memory_allocated = 0;
static size_t highest_memory_allocated = 0;

static uint32_t flag_log_enable = BP_FLAG_NONCOMPLIANT | BP_FLAG_DROPPED | BP_FLAG_BUNDLE_TOO_LARGE |
                                  BP_FLAG_UNKNOWNREC | BP_FLAG_INVALID_CIPHER_SUITEID |
                                  BP_FLAG_INVALID_BIB_RESULT_TYPE | BP_FLAG_INVALID_BIB_TARGET_TYPE |
                                  BP_FLAG_FAILED_TO_PARSE | BP_FLAG_API_ERROR;

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
void bplib_os_init(void) {}

/*--------------------------------------------------------------------------------------
 * bplib_os_log -
 *
 * Returns - the error code passed in (for convenience)
 *-------------------------------------------------------------------------------------*/
int bplib_os_log(const char *file, unsigned int line, uint32_t *flags, uint32_t event, const char *fmt, ...)
{
    (void)file;
    (void)line;

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

        /* Handle Log Message */
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

            /* Build Log Message */
            msglen = snprintf(log_message, BP_MAX_LOG_ENTRY_SIZE, "%s:%u:%s", pathptr, line, formatted_string);

            /* Provide Truncation Indicator */
            if (msglen > (BP_MAX_LOG_ENTRY_SIZE - 2))
            {
                log_message[BP_MAX_LOG_ENTRY_SIZE - 2] = '#';
            }

            /* Issue Log Message */
            CFE_EVS_SendEvent(BP_BPLIB_INFO_EID, CFE_EVS_EventType_INFORMATION, "%s", log_message);
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
 * bplib_os_systime - returns seconds
 *-------------------------------------------------------------------------------------*/
int bplib_os_systime(unsigned long *sysnow)
{
    assert(sysnow);

    CFE_TIME_SysTime_t sys_time = CFE_TIME_GetTime();
    if (sys_time.Seconds < BP_CFE_SECS_AT_2000)
    {
        *sysnow = sys_time.Seconds;
        return BP_ERROR;
    }
    else
    {
        *sysnow = sys_time.Seconds - BP_CFE_SECS_AT_2000;
        return BP_SUCCESS;
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_os_sleep
 *-------------------------------------------------------------------------------------*/
void bplib_os_sleep(int seconds)
{
    OS_TaskDelay(seconds * 1000);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_random -
 *-------------------------------------------------------------------------------------*/
uint32_t bplib_os_random(void)
{
    CFE_TIME_SysTime_t sys_time = CFE_TIME_GetMET();
    unsigned long      seed     = sys_time.Seconds + sys_time.Subseconds;
    return (uint32_t)BP_RAND_HASH(seed);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_createlock -
 *-------------------------------------------------------------------------------------*/
bp_handle_t bplib_os_createlock(void)
{
    return bp_handle_from_serial(1, BPLIB_HANDLE_OS_BASE);
}

/*--------------------------------------------------------------------------------------
 * bplib_os_destroylock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_destroylock(bp_handle_t h)
{
    (void)h;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_lock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_lock(bp_handle_t h)
{
    (void)h;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_unlock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_unlock(bp_handle_t h)
{
    (void)h;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_signal -
 *-------------------------------------------------------------------------------------*/
void bplib_os_signal(bp_handle_t h)
{
    (void)h;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_waiton -
 *-------------------------------------------------------------------------------------*/
int bplib_os_waiton(bp_handle_t h, int timeout_ms)
{
    (void)h;
    (void)timeout_ms;
    return BP_TIMEOUT;
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

/*----------------------------------------------------------------------------
 * bplib_os_calloc
 *----------------------------------------------------------------------------*/
void *bplib_os_calloc(size_t size)
{
    /* Allocate Memory Block */
    size_t   block_size = size + sizeof(size_t);
    uint8_t *mem_ptr    = (uint8_t *)calloc(block_size, 1);
    if (mem_ptr)
    {
        /* Prepend Amount */
        size_t *size_ptr = (size_t *)mem_ptr;
        *size_ptr        = block_size;

        /* Update Statistics */
        current_memory_allocated += block_size;
        if (current_memory_allocated > highest_memory_allocated)
        {
            highest_memory_allocated = current_memory_allocated;
        }

        /* Return User Block */
        return (mem_ptr + sizeof(size_t));
    }
    else
    {
        return NULL;
    }
}

/*----------------------------------------------------------------------------
 * bplib_os_free
 *----------------------------------------------------------------------------*/
void bplib_os_free(void *ptr)
{
    if (ptr)
    {
        uint8_t *mem_ptr = (uint8_t *)ptr;

        /* Read Amount */
        size_t *size_ptr   = (size_t *)((uint8_t *)mem_ptr - sizeof(size_t));
        size_t  block_size = *size_ptr;

        /* Update Statistics */
        current_memory_allocated -= block_size;

        /* Free Memory Block */
        free(mem_ptr - sizeof(size_t));
    }
}

/*----------------------------------------------------------------------------
 * bplib_os_memused - how many bytes of memory currently allocated
 *----------------------------------------------------------------------------*/
size_t bplib_os_memused(void)
{
    return current_memory_allocated;
}

/*----------------------------------------------------------------------------
 * bplib_os_memhigh - the most total bytes in allocation at any given time
 *----------------------------------------------------------------------------*/
size_t bplib_os_memhigh(void)
{
    return highest_memory_allocated;
}
