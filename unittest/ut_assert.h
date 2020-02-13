/************************************************************************
 * File: ut_assert.h
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
#ifndef _ut_assert_h_
#define _ut_assert_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "stdio.h"
#include "stdarg.h"
#include "stdbool.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define ut_assert(e,...)    _ut_assert(e,__FILE__,__LINE__,__VA_ARGS__)
#define ut_check(e)         _ut_assert(e,__FILE__,__LINE__,"Failure")

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

void    ut_reset        (void);
bool    _ut_assert      (bool e, const char* file, int line, const char* fmt, ...);
int     ut_failures     (void);

#endif /* _ut_assert_h_ */
