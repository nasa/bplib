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

#include "cfe.h"
#include "bplib.h"
#include "bplib_os.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_MAX_LOG_ENTRY_SIZE 256

#ifndef BP_BPLIB_INFO_EID
#define BP_BPLIB_INFO_EID 0xFF
#endif

/******************************************************************************
 FILE DATA
 ******************************************************************************/

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
            {
                pathptr++;
            }
            else
            {
                pathptr = (char *)file;
            }

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
        {
            *flags |= event;
        }
        return BP_ERROR;
    }
    else
    {
        return BP_SUCCESS;
    }
}
