/************************************************************************
 * File: ut_assert.c
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

#define UT_MAX_ASSERT   128

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
bool _ut_assert(bool e, const char* file, int line, const char* fmt, ...)
{
    if(!e)
    {
        char formatted_string[UT_MAX_ASSERT];
        char log_message[UT_MAX_ASSERT];
        va_list args;
        int vlen, msglen;
        char* pathptr;

        /* Build Formatted String */
        va_start(args, fmt);
        vlen = vsnprintf(formatted_string, UT_MAX_ASSERT - 1, fmt, args);
        msglen = vlen < UT_MAX_ASSERT - 1 ? vlen : UT_MAX_ASSERT - 1;
        va_end(args);
        if (msglen < 0) formatted_string[0] = '\0';
        else            formatted_string[msglen] = '\0';

        /* Chop Path in Filename */
        pathptr = strrchr(file, '/');
        if(pathptr) pathptr++;
        else pathptr = (char*)file;

        /* Create Log Message */
        msglen = snprintf(log_message, UT_MAX_ASSERT, "Failure @ %s:%d:%s", pathptr, line, formatted_string);
        if(msglen > (UT_MAX_ASSERT - 1))
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
int ut_failures (void)
{
    return _ut_failures;
}
