/************************************************************************
 * File: bundle_types.h
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

#ifndef _bundle_types_h_
#define _bundle_types_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "rb_tree.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_BUNDLE_HDR_BUF_SIZE  128

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Call-Backs */
typedef int (*bp_generate_t) (void* parm, bool is_record, uint8_t* payload, int size, int timeout);
typedef int (*bp_acknowledge_t) (void* parm, bp_val_t cid);

/* Bundle Field (fixed size) */
typedef struct {
    bp_val_t            value;          /* value of field */
    bp_val_t            index;          /* offset into memory block to write value */
    bp_val_t            width;          /* number of bytes in memory block value uses */
} bp_field_t;

/* Active Bundle */
typedef struct {
    bp_sid_t            sid;            /* storage id */
    bp_val_t            retx;           /* retransmit time */
    bp_val_t            cid;            /* custody id */
} bp_active_bundle_t;

/* Bundle Data */
typedef struct {
    bp_val_t            exprtime;       /* absolute time when bundle expires */
    bp_field_t          cidfield;       /* SDNV of custody id field of bundle */
    int                 cteboffset;     /* offset of the CTEB block of bundle */
    int                 biboffset;      /* offset of the BIB block of bundle */
    int                 payoffset;      /* offset of the payload block of bundle */
    int                 headersize;     /* size of the header (portion of buffer below used) */
    int                 bundlesize;     /* total size of the bundle (header and payload) */
    uint8_t             header[BP_BUNDLE_HDR_BUF_SIZE]; /* header portion of bundle */
} bp_bundle_data_t;

/* Bundle Custodian */
typedef struct {
    uint8_t*            rec;            /* payload | aggregate custody signal */
    int                 rec_size;       /* size of payload | size of aggregate custody signal */
    bp_ipn_t            node;           /* custody node of bundle */
    bp_ipn_t            service;        /* custody service of bundle */
    bp_val_t            cid;            /* custody id of bundle */
} bp_custodian_t;

/* Bundle Structure */
typedef struct {
    bp_route_t          route;          /* addressing information */
    bp_attr_t*          attributes;     /* -pointer- to the channel attributes */
    bp_generate_t       generate;       /* function that puts bundle into storage */
    void*               genparm;        /* pass through parameter for generate function */
    bp_bundle_data_t    data;           /* serialized and stored bundle data */
    bool                prebuilt;       /* does pre-built bundle header need initialization */
    void*               blocks;         /* populated in initialization function */
} bp_bundle_t;

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

#endif  /* _bundle_types_h_ */
