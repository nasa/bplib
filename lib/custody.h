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
#include "rb_tree.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Acknowledge Call-Back */
typedef int (*bp_acknowledge_t) (void* parm, bp_val_t cid);

/* Custody Structure */
typedef struct {
    bp_attr_t       attributes;     /* its own copy and version of attributes */
    bp_val_t        last_time;      /* time of last dacs generated */
    int             lock;           /* for thread safe operations on dacs */
    rb_tree_t       tree;           /* balanced tree to store bundle ids */
    uint8_t*        recbuf;         /* buffer to hold built DACS record */
    int             recbuf_size;    /* size of the recbuf above */
    bp_bundle_t     bundle;
} bp_custody_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int     custody_initialize      (bp_custody_t* custody, bp_route_t route, bp_attr_t* attributes, uint16_t* flags);
void    custody_uninitialize    (bp_custody_t* custody);
int     custody_send            (bp_custody_t* custody, bp_val_t period, bp_val_t sysnow, bp_generate_t gen, void* parm, int timeout, uint16_t* flags);
int     custody_receive         (bp_custody_t* custody, bp_custodian_t* custodian, bp_val_t sysnow, bp_generate_t gen, void* parm, int timeout, uint16_t* flags);
int     custody_acknowledge     (bp_custody_t* custody, bp_custodian_t* custodian, bp_acknowledge_t ack, void* parm, uint16_t* flags);

#endif  /* __BPLIB_CUSTODY_H__ */
