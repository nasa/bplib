/************************************************************************
 * File: unittest.h
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
#ifndef _unittest_h_
#define _unittest_h_

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int bplib_unittest_crc(void);
int bplib_unittest_rb_tree(void);
int bplib_unittest_rh_hash(void);
int bplib_unittest_flash(void);

#endif /* _unittest_h_ */
