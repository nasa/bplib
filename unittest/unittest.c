/************************************************************************
 * File: unittest.c
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

#include "ut_assert.h"

/******************************************************************************
 EXTERNS
 ******************************************************************************/

extern int ut_crc (void);
extern int ut_rb_tree (void);
extern int ut_rh_hash (void);

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int bplib_unittest (void)
{
    /* Run Unit Tests */
#ifdef UNITTESTS
    ut_crc();
    ut_rb_tree();
    ut_rh_hash();
#endif
    
    /* Return Failures */
    return ut_failures();
}
