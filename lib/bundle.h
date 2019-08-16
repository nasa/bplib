/************************************************************************
 * File: bundle.h
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

#ifndef __BPLIB_BUNDLE_H__
#define __BPLIB_BUNDLE_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"

/* v6 blocks */
#include "pri.h"
#include "bib.h"
#include "pay.h"
#include "cteb.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_BUNDLE_HDR_BUF_SIZE  128

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Bundle Data */
typedef struct {
    uint32_t            exprtime;           /* absolute time when bundle expires */
    bp_sdnv_t           cidsdnv;            /* SDNV of custody id field of bundle */
    int                 cteboffset;         /* offset of the CTEB block of bundle */
    int                 biboffset;          /* offset of the BIB block of bundle */
    int                 payoffset;          /* offset of the payload block of bundle */
    int                 headersize;         /* size of the header (portion of buffer below used) */
    int                 bundlesize;         /* total size of the bundle (header and payload) */
    uint8_t             header[BP_BUNDLE_HDR_BUF_SIZE]; /* header portion of bundle */
} bp_bundle_data_t;

/* Version 6 Bundle Blocks */
typedef struct {
    bool                prebuilt;               /* does pre-built bundle header need initialization */
    bp_blk_pri_t        primary_block;
    bp_blk_cteb_t       custody_block;
    bp_blk_bib_t        integrity_block;
    bp_blk_pay_t        payload_block;
} bp_v6blocks_t;

/* Bundle Control Structure */
typedef struct {
    bp_store_t          store;                  /* storage service call-backs */
    bp_route_t          route;                  /* addressing information */
    bp_attr_t*          attributes;             /* -pointer- to the channel attributes */
    int                 handle;                 /* storage service handle for bundle data */
    bp_bundle_data_t    data;                   /* serialized and stored bundle data */
    bp_v6blocks_t       v6blocks;               /* populated in initialization function */
} bp_bundle_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int     bundle_initialize   (bp_bundle_t* bundle, bp_route_t route, bp_store_t store, bp_attr_t* attr, uint16_t* flags);
void    bundle_uninitialize (bp_bundle_t* bundle);
int     bundle_send         (bp_bundle_t* bundle, uint8_t* pay, int pay_size, int timeout, uint16_t* flags);
int     bundle_receive      (bp_bundle_t* bundle, uint8_t** block, int* block_size, uint32_t sysnow, int timeout, uint16_t* flags);

/* v6 bundle functions */
int     v6blocks_build      (bp_bundle_t* bundle, bp_blk_pri_t* pri, bp_blk_pay_t* pay, uint8_t* hdr_buf, int hdr_len, uint16_t* flags);
int     v6blocks_write      (bp_bundle_t* bundle, bool set_time, int timeout, uint16_t* flags);
int     v6blocks_read       (bp_bundle_t* bundle, uint8_t** block, int* block_size, uint32_t sysnow, int timeout, uint16_t* flags);



#endif  /* __BPLIB_BUNDLE_H__ */
