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
#include "bundle_types.h"
#include "rb_tree.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Custody Structure */
typedef struct {
    bp_attr_t           attributes;     /* its own copy and version of attributes */
    bp_acknowledge_t    acknowledge;    /* call-back that acknowledges bundle reception */
    void*               ackparm;        /* parameter passed to acknowledge call-back */
    bp_val_t            last_time;      /* time of last DACS generated */
    int                 lock;           /* for thread safe operations on DACS */
    rb_tree_t           tree;           /* balanced tree to store bundle CID */
    uint8_t*            recbuf;         /* buffer to hold built DACS record */
    int                 recbuf_size;    /* size of the buffer above */
    bp_bundle_t         bundle;         /* record bundle (DACS) */
} bp_custody_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int     custody_initialize      (bp_custody_t* custody, bp_route_t route, bp_attr_t* attributes, bp_generate_t generate, bp_acknowledge_t acknowledge, void* parm, uint16_t* flags);
void    custody_uninitialize    (bp_custody_t* custody);
int     custody_send            (bp_custody_t* custody, bp_val_t period, int timeout, uint16_t* flags);
int     custody_receive         (bp_custody_t* custody, bp_custodian_t* custodian, int timeout, uint16_t* flags);
int     custody_acknowledge     (bp_custody_t* custody, bp_custodian_t* custodian, uint16_t* flags);

#endif  /* __BPLIB_CUSTODY_H__ */
