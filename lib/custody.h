/************************************************************************
 * File: custody.h
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

#ifndef _custody_h_
#define _custody_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bundle_types.h"

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int     custody_initialize      (bp_custody_t* custody, bp_route_t route, bp_attr_t* attributes, bp_create_func_t create, bp_remove_func_t remove, void* parm, uint16_t* flags);
void    custody_uninitialize    (bp_custody_t* custody);

#endif  /* _custody_h_ */
