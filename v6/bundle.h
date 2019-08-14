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
#include "pri.h"
#include "bib.h"
#include "pay.h"
#include "cteb.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_BUNDLE_HDR_BUF_SIZE          128

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Payload Data */
typedef struct {
    bool                request_custody;    /* boolean whether original bundle requested custody on payload delivery */
    int                 payloadsize;        /* size of the payload */
} bp_payload_data_t;

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

/* Bundle Data */
typedef struct {
    bp_store_t*         service;
    int                 handle;
    void*               data;
} bp_data_store_t;

/* Bundle Blocks */
typedef struct {
    bp_blk_pri_t        primary_block;
    bp_blk_cteb_t       custody_block;
    bp_blk_bib_t        integrity_block;
    bp_blk_pay_t        payload_block;
} bp_bundle_blocks_t;

/* Bundle Control Structure */
typedef struct {
    bp_attr_t*          attributes;             /* pointer to the channel attributes */
    bp_ipn_t            local_node;             /* source node for bundle */
    bp_ipn_t            local_service;          /* source service for bundle */
    bp_ipn_t            destination_node;       /* destination node for bundle */
    bp_ipn_t            destination_service;    /* destination service for bundle */
    bp_ipn_t            report_node;            /* destination node for bundle reports */
    bp_ipn_t            report_service;         /* destination service for bundle reports */ 
    bp_data_store_t     bundle_store;           /* populated in initialization function */
    bp_data_store_t     payload_store;          /* populated in initialization function */    
    bp_bundle_blocks_t  blocks;                 /* populated in initialization function */
} bp_bundle_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int     bundle_initialize   (bp_bundle_t* bundle, bp_attr_t* attr, bp_ipn_t srcnode, bp_ipn_t srcserv, bp_ipn_t dstnode, bp_ipn_t destsrv, bp_store_t* service, uint16_t* flags);
void    bundle_uninitialize (bp_bundle_t* bundle);

int     bundle_update       (bp_bundle_t* bundle, uint16_t* flags);
int     bundle_send         (bp_bundle_t* bundle, uint8_t* pay_buf, int pay_len, int timeout, uint16_t* flags);
int     bundle_receive      (bp_bundle_t* bundle, void** block, int* block_size, uint32_t sysnow, int timeout, uint16_t* flags);

#endif  /* __BPLIB_BUNDLE_H__ */
