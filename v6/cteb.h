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

#ifndef __BPLIB_CTEB_H__
#define __BPLIB_CTEB_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"

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

#endif  /* __BPLIB_CTEB_H__ */
