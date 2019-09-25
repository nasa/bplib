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
 EXTERNS
 ******************************************************************************/

extern int ut_crc (void);
extern int ut_rb_tree (void);

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int bplib_unittest (void)
{
    int failures = 0;
    
    /* Run Unit Tests */
    
#ifdef UNITTESTS
    failures += ut_crc();
    failures += ut_rb_tree();
#endif
    
    /* Return Failures */

    return failures;
}
