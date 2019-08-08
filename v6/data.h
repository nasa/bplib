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

#ifndef __BPLIB_DATA_H__
#define __BPLIB_DATA_H__

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

#define BP_MAX_EID_STRING               128
#define BP_BUNDLE_HDR_BUF_SIZE          128

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Payload Storage Block */
typedef struct {
    bool                request_custody;    /* boolean whether original bundle requested custody on payload delivery */
    int                 payloadsize;        /* size of the payload */
} bp_payload_store_t;

/* Bundle Storage Block */
typedef struct {
    uint32_t            exprtime;           /* absolute time when bundle expires */
    bp_sdnv_t           cidsdnv;            /* SDNV of custody id field of bundle */
    int                 cteboffset;         /* offset of the CTEB block of bundle */
    int                 biboffset;          /* offset of the BIB block of bundle */
    int                 payoffset;          /* offset of the payload block of bundle */
    int                 headersize;         /* size of the header (portion of buffer below used) */
    int                 bundlesize;         /* total size of the bundle (header and payload) */
    uint8_t             header[BP_BUNDLE_HDR_BUF_SIZE]; /* header portion of bundle */
} bp_bundle_store_t;

/* Data Structure */
typedef struct {
    int maxlength;
    void* bundle;
} bp_data_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int     data_forward    (bp_data_t* data, bp_blk_pri_t* pri, bp_blk_pay_t* pay, uint8_t* hdr_buf, int hdr_len, int timeout, bp_store_enqueue_t enqueue, int handle, uint16_t* dataflags);
int     data_send       (bp_data_t* data, uint8_t* pay_buf, int pay_len, int timeout, bp_store_enqueue_t enqueue, int handle, uint16_t* dataflags);
int     data_receive    (bp_data_t* data, bool request_custody, int timeout, bp_store_enqueue_t enqueue, int handle, uint16_t* dataflags);

#endif  /* __BPLIB_DATA_H__ */
