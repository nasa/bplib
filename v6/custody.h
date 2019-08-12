/************************************************************************
 * File: dacs.h
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

#ifndef __BPLIB_DACS_H__
#define __BPLIB_DACS_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"
#include "pri.h"
#include "bib.h"
#include "pay.h"
#include "cteb.h"
#include "rb_tree.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    int             max_fills;
    int             max_gaps;
    uint32_t        cstnode;
    uint32_t        cstserv;
    rb_tree_t       tree;       /* balanced tree to store bundle ids */
    uint32_t        last_dacs;  /* time of last dacs generated */
    uint8_t*        recbuf;     /* buffer to hold built DACS record */
    int             recbuf_size;
    bp_bundle_t     bundle;
} bp_dacs_t;

typedef struct {
    int             max_dacs;
    int             num_dacs;
    bp_dacs_t*      dacs;
} bp_custody_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int     dacs_initialize     (bp_custody_t* custody, bp_ipn_t srcnode, bp_ipn_t srcserv, bp_ipn_t dstnode, bp_ipn_t destsrv, bp_store_t* store, bp_attr_t* attr, uint16_t* flags);
void    dacs_uninitialize   (bp_custody_t* custody);

int     dacs_acknowledge    (bp_custody_t* custody, bp_blk_cteb_t* cteb, uint32_t sysnow, int timeout, uint16_t* flags);
int     dacs_check          (bp_custody_t* custody, uint32_t period, uint32_t sysnow, int timeout, uint16_t* flags);
int     dacs_process        (void* rec, int size, int* acks, bp_sid_t* sids, int table_size, uint16_t* flags);

#endif  /* __BPLIB_DACS_H__ */
