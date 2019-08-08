/************************************************************************
 * File: pri.h
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

#ifndef __BPLIB_BLK_PRI_H__
#define __BPLIB_BLK_PRI_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"
#include "sdnv.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_PRI_VERSION                  0x06

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Bundle Primary Block */
typedef struct {
    /* field data */
    uint8_t     version;
    bp_sdnv_t   pcf;
    bp_sdnv_t   blklen;
    bp_sdnv_t   dstnode;
    bp_sdnv_t   dstserv;
    bp_sdnv_t   srcnode;
    bp_sdnv_t   srcserv;
    bp_sdnv_t   rptnode;
    bp_sdnv_t   rptserv;
    bp_sdnv_t   cstnode;
    bp_sdnv_t   cstserv; 
    bp_sdnv_t   createsec;
    bp_sdnv_t   createseq;
    bp_sdnv_t   lifetime;
    bp_sdnv_t   dictlen;
    bp_sdnv_t   fragoffset;
    bp_sdnv_t   paylen;
    /* meta information */
    bool        is_admin_rec;           /* 0: not admin, 1: is admin */
    bool        request_custody;        /* 0: not requested, 1: requested */
    bool        allow_frag;             /* 0: do not allow, 1: allow (for created bundles, if allowed, it will be used) */
    bool        is_frag;                /* 0: is not a fragment, 1: is a fragment */
    bool        integrity_check;        /* 0: do not include an integrity check, 1: include bundle integrity block */
} bp_blk_pri_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int pri_read      (void* block, int size, bp_blk_pri_t* pri, bool update_indices);
int pri_write     (void* block, int size, bp_blk_pri_t* pri, bool update_indices);
int pri_display   (bp_blk_pri_t* pri);

#endif  /* __BPLIB_BLK_PRI_H__ */
