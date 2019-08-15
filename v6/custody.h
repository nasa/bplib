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

#ifndef __BPLIB_CUSTODY_H__
#define __BPLIB_CUSTODY_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"
#include "bundle.h"
#include "pri.h"
#include "bib.h"
#include "pay.h"
#include "cteb.h"
#include "rb_tree.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    uint32_t        cstnode;    /* custody node of bundle sender */
    uint32_t        cstserv;    /* custody service of bundle sender */
    uint32_t        last_time;  /* time of last dacs generated */
    int             lock;       /* for thread safe operations on dacs */
    rb_tree_t       tree;       /* balanced tree to store bundle ids */
    uint8_t*        recbuf;     /* buffer to hold built DACS record */
    int             recbuf_size;/* size of the recbuf above */
    bp_bundle_t     bundle;
} bp_custody_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int     custody_initialize      (bp_custody_t* custody, bp_attr_t* attr, bp_store_t store, bp_ipn_t srcnode, bp_ipn_t srcserv, uint16_t* flags);
void    custody_uninitialize    (bp_custody_t* custody);

int     custody_acknowledge     (bp_custody_t* custody, bp_blk_cteb_t* cteb, uint32_t sysnow, int timeout, uint16_t* flags);
int     custody_check           (bp_custody_t* custody, uint32_t period, uint32_t sysnow, int timeout, uint16_t* flags);
int     custody_process         (bp_custody_t* custody, uint8_t* rec, int rec_size, int* acks, bp_sid_t* sids, int table_size, uint16_t* flags);

#endif  /* __BPLIB_CUSTODY_H__ */
