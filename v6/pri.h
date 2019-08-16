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

#ifndef __BPLIB_PRI_H__
#define __BPLIB_PRI_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"
#include "sdnv.h"
#include "v6.h"

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
    bool        is_frag;                /* 0: is not a fragment, 1: is a fragment */
    bool        allow_frag;             /* 0: fragmentation not allowed, 1: fragmentation allowed */
    bool        cst_rqst;               /* 0: custody not requested, 1: custody requested */
} bp_blk_pri_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int pri_read      (void* block, int size, bp_blk_pri_t* pri, bool update_indices, uint16_t* flags);
int pri_write     (void* block, int size, bp_blk_pri_t* pri, bool update_indices, uint16_t* flags);
int pri_display   (bp_blk_pri_t* pri);

#endif  /* __BPLIB_PRI_H__ */
