/************************************************************************
 * File: cteb.h
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

#ifndef _cteb_h_
#define _cteb_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bundle_types.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    bp_field_t  bf;
    bp_field_t  blklen;
    bp_field_t  cid;
    char        csteid[BP_MAX_EID_STRING];
    bp_ipn_t    cstnode;
    bp_ipn_t    cstserv;
} bp_blk_cteb_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int cteb_read     (void* block, int size, bp_blk_cteb_t* cteb, bool update_indices, uint16_t* flags);
int cteb_write    (void* block, int size, bp_blk_cteb_t* cteb, bool update_indices, uint16_t* flags);

#endif  /* _cteb_h_ */
