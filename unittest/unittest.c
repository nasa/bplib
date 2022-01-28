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
#include "bplib_store_flash.h"

/******************************************************************************
 EXTERNS
 ******************************************************************************/

extern int ut_crc(void);
extern int ut_rb_tree(void);
extern int ut_rh_hash(void);
extern int ut_flash(void);

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * CRC Unit Test -
 *--------------------------------------------------------------------------------------*/
int bplib_unittest_crc(void)
{
#ifdef UNITTESTS
    return ut_crc();
#else
    return 0;
#endif
}

/*--------------------------------------------------------------------------------------
 * RB Tree Unit Test -
 *--------------------------------------------------------------------------------------*/
int bplib_unittest_rb_tree(void)
{
#ifdef UNITTESTS
    return ut_rb_tree();
#else
    return 0;
#endif
}

/*--------------------------------------------------------------------------------------
 * RH Hash Unit Test -
 *--------------------------------------------------------------------------------------*/
int bplib_unittest_rh_hash(void)
{
#ifdef UNITTESTS
    return ut_rh_hash();
#else
    return 0;
#endif
}

/*--------------------------------------------------------------------------------------
 * Flash Unit Test -
 *--------------------------------------------------------------------------------------*/
int bplib_unittest_flash(void)
{
#ifdef UNITTESTS
    bplib_store_flash_uninit(); /* should always be safe to call */
    return ut_flash();
#else
    return 0;
#endif
}
