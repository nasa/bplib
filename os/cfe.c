/************************************************************************
 * File: bplib_os_singlethread.c
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

#include "cfe.h"

#include "bp/bplib.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_MAX_LOG_ENTRY_SIZE       256

#ifndef BPLIB_CFE_SECS_AT_2000
#define BPLIB_CFE_SECS_AT_2000      946684800
#endif

#ifndef BPLIB_CFE_LOG_EID
#define BPLIB_CFE_LOG_EID           0xFFFF
#endif

/******************************************************************************
 FILE DATA
 ******************************************************************************/


/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_os_init -
 *-------------------------------------------------------------------------------------*/
void bplib_os_init(void)
{
}

/*--------------------------------------------------------------------------------------
 * bplib_os_log -
 *
 * 	Returns - the error code passed in (for convenience)
 *-------------------------------------------------------------------------------------*/
int bplib_os_log(const char* file, unsigned int line, int error, const char* fmt, ...)
{
    (void)file;
    (void)line;
    
    char formatted_string[BP_MAX_LOG_ENTRY_SIZE];
    va_list args;
    int vlen, msglen;

    /* Build Formatted String */
    va_start(args, fmt);
    vlen = vsnprintf(formatted_string, BP_MAX_LOG_ENTRY_SIZE - 1, fmt, args);
    msglen = vlen < BP_MAX_LOG_ENTRY_SIZE - 1 ? vlen : BP_MAX_LOG_ENTRY_SIZE - 1;
    va_end(args);
    if (msglen < 0) return error; // nothing to do
    formatted_string[msglen] = '\0';

    /* Handle Log Message */
    CFE_EVS_SendEvent(BPLIB_CFE_LOG_EID, CFE_EVS_INFORMATION, "[%d] %s", error, formatted_string);

    /* Return Error Code */
    return error;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_systime - returns seconds
 *-------------------------------------------------------------------------------------*/
unsigned long bplib_os_systime(void)
{
    CFE_TIME_SysTime_t sys_time = CFE_TIME_GetTime();
    return sys_time.Seconds - BPLIB_CFE_SECS_AT_2000;
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
int bplib_os_createlock(void)
{
    return 0;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_destroylock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_destroylock(int handle)
{
    (void)handle;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_lock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_lock(int handle)
{
    (void)handle;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_unlock -
 *-------------------------------------------------------------------------------------*/
void bplib_os_unlock(int handle)
{
    (void)handle;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_signal -
 *-------------------------------------------------------------------------------------*/
void bplib_os_signal(int handle)
{
    (void)handle;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_waiton -
 *-------------------------------------------------------------------------------------*/
int bplib_os_waiton(int handle, int timeout_ms)
{
    (void)handle;
    (void)timeout_ms;
    return 0;
}

/*--------------------------------------------------------------------------------------
 * bplib_os_format -
 *-------------------------------------------------------------------------------------*/
int bplib_os_format(char* dst, size_t len, const char* fmt, ...)
{
    va_list args;
    int vlen;

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
int bplib_os_strnlen(const char* str, int maxlen)
{
    int len;
    for(len = 0; len < maxlen; len++)
    {
        if(str[len] == '\0')
        {
            return len;
        }
    }
    return maxlen;
}
