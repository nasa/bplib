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

#include "stdio.h"
#include "stdarg.h"
#include "stdbool.h"
#include "string.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define UT_MAX_ASSERT 128

/******************************************************************************
 LOCAL DATA
 ******************************************************************************/

static int _ut_failures = 0;

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * ut_reset - sets failures to zero
 *--------------------------------------------------------------------------------------*/
void ut_reset(void)
{
    _ut_failures = 0;
}

/*--------------------------------------------------------------------------------------
 * _ut_assert - called through ut_assert macro
 *--------------------------------------------------------------------------------------*/
bool _ut_assert(bool e, const char *file, int line, const char *fmt, ...)
{
    if (!e)
    {
        char    formatted_string[UT_MAX_ASSERT];
        char    log_message[UT_MAX_ASSERT];
        va_list args;
        int     vlen, msglen;
        char   *pathptr;

        /* Build Formatted String */
        va_start(args, fmt);
        vlen   = vsnprintf(formatted_string, UT_MAX_ASSERT - 1, fmt, args);
        msglen = vlen < UT_MAX_ASSERT - 1 ? vlen : UT_MAX_ASSERT - 1;
        va_end(args);
        if (msglen < 0)
            formatted_string[0] = '\0';
        else
            formatted_string[msglen] = '\0';

        /* Chop Path in Filename */
        pathptr = strrchr(file, '/');
        if (pathptr)
            pathptr++;
        else
            pathptr = (char *)file;

        /* Create Log Message */
        msglen = snprintf(log_message, UT_MAX_ASSERT, "Failure @ %s:%d:%s", pathptr, line, formatted_string);
        if (msglen > (UT_MAX_ASSERT - 1))
        {
            log_message[UT_MAX_ASSERT - 1] = '#';
        }

        /* Display Log Message */
        printf("%s", log_message);

        /* Count Error */
        _ut_failures++;
    }

    return e;
}

/*--------------------------------------------------------------------------------------
 * ut_failures -
 *--------------------------------------------------------------------------------------*/
int ut_failures(void)
{
    return _ut_failures;
}
