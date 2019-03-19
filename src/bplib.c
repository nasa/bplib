/************************************************************************
 * File: bplib.c
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#include "bplib.h"
#include "bplib_os.h"
#include "bplib_sdnv.h"
#include "bplib_blk.h"
#include "bplib_blk_pri.h"
#include "bplib_blk_cteb.h"
#include "bplib_blk_bib.h"
#include "bplib_blk_pay.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#ifndef BP_DEFAULT_MAX_CHANNELS
#define BP_DEFAULT_MAX_CHANNELS         4
#endif

#ifndef BP_DEFAULT_ACTIVE_TABLE_SIZE
#define BP_DEFAULT_ACTIVE_TABLE_SIZE    16384
#endif

#ifndef BP_DEFAULT_MAX_CONCURRENT_DACS
#define BP_DEFAULT_MAX_CONCURRENT_DACS  4   // maximum number of custody eids to keep track of
#endif

#ifndef BP_DEFAULT_MAX_FILLS_PER_DACS
#define BP_DEFAULT_MAX_FILLS_PER_DACS   64
#endif

#ifndef BP_DEFAULT_PAY_CRC
#define BP_DEFAULT_PAY_CRC              BP_BIB_CRC16
#endif

#ifndef BP_DEFAULT_TIMEOUT
#define BP_DEFAULT_TIMEOUT              10
#endif

#ifndef BP_DEFAULT_CREATE_TIME_SYS
#define BP_DEFAULT_CREATE_TIME_SYS      true
#endif

#ifndef BP_DEFAULT_CREATE_SECS
#define BP_DEFAULT_CREATE_SECS          0
#endif

#ifndef BP_DEFAULT_CSTRQST
#define BP_DEFAULT_CSTRQST              true
#endif

#ifndef BP_DEFAULT_LIFETIME
#define BP_DEFAULT_LIFETIME             0
#endif

#ifndef BP_DEFAULT_BUNDLE_MAXLENGTH
#define BP_DEFAULT_BUNDLE_MAXLENGTH     4096
#endif

#ifndef BP_DEFAULT_SEQ_RESET_PERIOD
#define BP_DEFAULT_SEQ_RESET_PERIOD     0
#endif

#ifndef BP_DEFAULT_PROC_ADMIN_ONLY
#define BP_DEFAULT_PROC_ADMIN_ONLY      false
#endif

#ifndef BP_DEFAULT_WRAP_RESPONSE
#define BP_DEFAULT_WRAP_RESPONSE        BP_WRAP_RESEND
#endif

#ifndef BP_DEFAULT_WRAP_TIMEOUT
#define BP_DEFAULT_WRAP_TIMEOUT         1000    // milliseconds
#endif

#ifndef BP_DEFAULT_CID_REUSE
#define BP_DEFAULT_CID_REUSE            false
#endif

#ifndef BP_DEFAULT_DACS_RATE
#define BP_DEFAULT_DACS_RATE            5       // seconds
#endif

#ifndef BP_DEFAULT_ORIGINATION
#define BP_DEFAULT_ORIGINATION          true
#endif

#ifndef BP_DEFAULT_BP_VERSION
#define BP_DEFAULT_BP_VERSION           BP_PRI_VERSION
#endif

#ifndef LIBID
#define LIBID                           "unversioned"
#endif

#define BP_EMPTY                        (-1)
#define BP_MAX_FILL                     0x3FFF
#define BP_BUNDLE_HDR_BUF_SIZE          128
#define BP_NUM_EXCLUDE_REGIONS          8


/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* --------------- Storage Types ------------------- */

/* Payload Storage Block */
typedef struct {
    bool                request_custody;    // boolean whether original bundle requested custody on payload delivery
    int                 payloadsize;        // size of the payload
} bp_payload_store_t;

/* Bundle Storage Block */
typedef struct {
    uint32_t            exprtime;           // absolute time when bundle expires
    bp_sdnv_t           cidsdnv;            // SDNV of custody id field of bundle
    int                 cteboffset;         // offset of the CTEB block of bundle
    int                 biboffset;          // offset of the BIB block of bundle
    int                 payoffset;          // offset of the payload block of bundle
    int                 headersize;         // size of the header (portion of buffer below used)
    int                 bundlesize;         // total size of the bundle (header and payload)
    uint8_t             header[BP_BUNDLE_HDR_BUF_SIZE]; // header portion of bundle
} bp_bundle_store_t;

/* --------------- Bundle Types ------------------- */

/* Data Bundle */
typedef struct {
    bp_blk_pri_t        primary_block;
    bp_blk_cteb_t       custody_block;
    bp_blk_bib_t        integrity_block;
    bp_blk_pay_t        payload_block;
    int                 maxlength;  // maximum size of bundle in bytes (includes header blocks)
    int                 originate;  // 1: originated bundle, 0: forwarded bundle
    bp_bundle_store_t   bundle_store;
    bp_payload_store_t  payload_store;
} bp_data_bundle_t;

/* DTN Aggregate Custody Signal Bundle */
typedef struct {
    bp_blk_pri_t        primary_block;
    bp_blk_bib_t        integrity_block;
    bp_blk_pay_t        payload_block;
    uint32_t            cstnode;
    uint32_t            cstserv;
    uint32_t            first_cid;
    uint32_t            last_cid;
    uint32_t*           fills;
    int                 num_fills;
    bool                delivered;  // false: forwarded to destination, true: delivered to application
    bool                sent;       // true: DACS wer sent since last check
    uint8_t*            paybuf;     // buffer to hold built DACS record
    int                 paybuf_size;
    bp_bundle_store_t   bundle_store;
} bp_dacs_bundle_t;

/* --------------- Application Types ------------------- */

/* Active Table */
typedef struct {
    bp_sid_t*           sid;
    uint32_t*           retx;
    uint32_t            oldest_cid;
    uint32_t            current_cid;
} bp_active_table_t;

/* Channel Control Block */
typedef struct {
    int                 index;
    bp_attr_t           attributes;

    int                 data_bundle_lock;
    int                 dacs_bundle_lock;
    int                 active_table_signal;

    uint32_t            local_node;
    uint32_t            local_service;

    bp_store_t          storage;
    int                 data_store_handle;
    int                 payload_store_handle;
    int                 dacs_store_handle;

    bp_data_bundle_t    data_bundle;
    bp_dacs_bundle_t*   dacs_bundle;

    bp_active_table_t   active_table;

    int                 num_dacs;
    int                 dacs_rate;              // number of seconds to wait between sending ACS bundles
    uint32_t            last_dacs;              // time of last dacs to be sent

    bp_stats_t          stats;

    int                 timeout;                // seconds, zero for infinite
    int                 proc_admin_only;        // process only administrative records (for sender only agents)
    int                 wrap_response;          // what to do when active table wraps
    int                 cid_reuse;              // favor reusing CID when retransmitting
} bp_channel_t;

/******************************************************************************
 FILE DATA
 ******************************************************************************/

static bp_channel_t* channels;
static int channels_max;
static int channels_lock;

/******************************************************************************
 CONST FILE DATA
 ******************************************************************************
 * Notes:
 * 1. The block length field for every bundle block MUST be set to a positive
 *    integer.  The option to update the fields of the bundle reserves the width
 *    of the blklen field and goes back and writes the value after the entire
 *    block is written.  If the blklen field was variable, the code would have
 *    to make a first pass to calculate the block length and then a second pass
 *    to use that block length - that would be too much processing.
 */

static const bp_blk_pri_t native_data_pri_blk = {
    .version            = BP_DEFAULT_BP_VERSION,
                            /*          Value         Index       Width   */
    .pcf                = { 0,                          1,          3 },
    .blklen             = { 0,                          4,          1 },
    .dstnode            = { 0,                          5,          4 },
    .dstserv            = { 0,                          9,          2 },
    .srcnode            = { 0,                          11,         4 },
    .srcserv            = { 0,                          15,         2 },
    .rptnode            = { 0,                          17,         4 },
    .rptserv            = { 0,                          21,         2 },
    .cstnode            = { 0,                          23,         4 },
    .cstserv            = { 0,                          27,         2 },
    .createsec          = { BP_DEFAULT_CREATE_SECS,     29,         6 },
    .createseq          = { 0,                          35,         4 },
    .lifetime           = { BP_DEFAULT_LIFETIME,        39,         4 },
    .dictlen            = { 0,                          43,         1 },
    .fragoffset         = { 0,                          44,         4 },
    .paylen             = { 0,                          48,         4 },
    .is_admin_rec       = false,
    .request_custody    = BP_DEFAULT_CSTRQST,
    .allow_frag         = false,
    .is_frag            = false,
    .report_deletion    = false
};

static const bp_blk_pri_t native_dacs_pri_blk = {
    .version            = BP_DEFAULT_BP_VERSION,
                            /*          Value         Index       Width   */
    .pcf                = { 0,                          1,          3 },
    .blklen             = { 0,                          4,          1 },
    .dstnode            = { 0,                          5,          4 },
    .dstserv            = { 0,                          9,          2 },
    .srcnode            = { 0,                          11,         4 },
    .srcserv            = { 0,                          15,         2 },
    .rptnode            = { 0,                          17,         4 },
    .rptserv            = { 0,                          21,         2 },
    .cstnode            = { 0,                          23,         4 },
    .cstserv            = { 0,                          27,         2 },
    .createsec          = { BP_DEFAULT_CREATE_SECS,     29,         6 },
    .createseq          = { 0,                          35,         4 },
    .lifetime           = { BP_DEFAULT_LIFETIME,        39,         4 },
    .dictlen            = { 0,                          43,         1 },
    .fragoffset         = { 0,                          44,         4 },
    .paylen             = { 0,                          48,         4 },
    .is_admin_rec       = true,
    .request_custody    = false,
    .allow_frag         = false,
    .is_frag            = false,
    .report_deletion    = false,
};

static const bp_blk_cteb_t native_cteb_blk = {
                            /*          Value             Index       Width   */
    .bf                 = { 0,                              1,          1 },
    .blklen             = { 0,                              2,          1 },
    .cid                = { 0,                              3,          4 },
    .csteid             = { '\0' },
    .cstnode            = 0,
    .cstserv            = 0
};

static const bp_blk_bib_t native_bib_blk = {
                            /*          Value             Index       Width   */
    .bf                 = { 0,                              1,          1 },
    .blklen             = { 0,                              2,          1 },
    .paytype            = { BP_DEFAULT_PAY_CRC,             3,          2 },
    .paycrc             = { 0,                              5,          3 }
};

static const bp_blk_pay_t native_pay_blk = {
                            /*          Value             Index       Width   */
    .bf                 = { 0,                              1,          1 },
    .blklen             = { 0,                              2,          4 },
    .payptr             = NULL,
    .paysize            = 0
};

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * initialize_orig_bundle - Initialize Bundle Header from Channel Struct
 *
 *  This is only done when changes to the bundle's channel parameters are made
 *
 *  Does not populate following:
 *      - creation time (when using system time)
 *      - creation sequence
 *      - fragment offset
 *      - total payload length
 *      - custody id
 *      - payload crc
 *      - payload block length
 *-------------------------------------------------------------------------------------*/
static int initialize_orig_bundle(bp_data_bundle_t* bundle)
{
    bp_bundle_store_t*  ds      = &bundle->bundle_store;
    uint8_t*            hdrbuf  = ds->header;

    /* Initialize Storage */
    bplib_os_memset(ds, 0, sizeof(bp_bundle_store_t));
    ds->cidsdnv = native_cteb_blk.cid;

    /* Initialize Header */
    if(bundle->primary_block.request_custody)
    {
        ds->cteboffset = bplib_blk_pri_write (&hdrbuf[0],              BP_BUNDLE_HDR_BUF_SIZE,                  &bundle->primary_block,   false);
        ds->biboffset  = bplib_blk_cteb_write(&hdrbuf[ds->cteboffset], BP_BUNDLE_HDR_BUF_SIZE - ds->cteboffset, &bundle->custody_block,   false) + ds->cteboffset;
        ds->payoffset  = bplib_blk_bib_write (&hdrbuf[ds->biboffset],  BP_BUNDLE_HDR_BUF_SIZE - ds->biboffset,  &bundle->integrity_block, false) + ds->biboffset;
        ds->headersize = bplib_blk_pay_write (&hdrbuf[ds->payoffset],  BP_BUNDLE_HDR_BUF_SIZE - ds->payoffset,  &bundle->payload_block,   false) + ds->payoffset;
    }
    else
    {
        ds->biboffset  = bplib_blk_pri_write (&hdrbuf[0],              BP_BUNDLE_HDR_BUF_SIZE,                  &bundle->primary_block,   false);
        ds->payoffset  = bplib_blk_bib_write (&hdrbuf[ds->biboffset],  BP_BUNDLE_HDR_BUF_SIZE - ds->biboffset,  &bundle->integrity_block, false) + ds->biboffset;
        ds->headersize = bplib_blk_pay_write (&hdrbuf[ds->payoffset],  BP_BUNDLE_HDR_BUF_SIZE - ds->payoffset,  &bundle->payload_block,   false) + ds->payoffset;
    }

    return ds->headersize;
}

/*--------------------------------------------------------------------------------------
 * initialize_forw_bundle -
 *
 *  This is done for every bundle that is forwarded
 *-------------------------------------------------------------------------------------*/
static int initialize_forw_bundle(bp_data_bundle_t* bundle, bp_blk_pri_t* pri, bp_blk_pay_t* pay, uint32_t local_node, uint32_t local_service, bool cteb_present, uint8_t* buffer, int* exclude, int num_excludes, uint16_t* procflags)
{
    int i, status;
    int hdr_index;

    bp_bundle_store_t*  ds      = &bundle->bundle_store;
    bp_blk_cteb_t*      fcteb   = &bundle->custody_block;
    bp_blk_bib_t*       fbib    = &bundle->integrity_block;

    /* Initialize Data Storage Memory */
    hdr_index = 0;
    bplib_os_memset(ds, 0, sizeof(bp_bundle_store_t));

    /* Initialize Primary Block */
    bundle->primary_block = *pri;

    /* Accept Custody */
    if(bundle->primary_block.request_custody)
    {
        if(cteb_present == false)
        {
            *procflags |= BP_FLAG_NONCOMPLIANT;
            return bplog(BP_UNSUPPORTED, "Only aggregate custody supported\n");
        }

        bundle->primary_block.rptnode.value = local_node;
        bundle->primary_block.rptserv.value = local_service;
        bundle->primary_block.cstnode.value = local_node;
        bundle->primary_block.cstserv.value = local_service;
    }

    /* Write Primary Block */
    status = bplib_blk_pri_write(ds->header, BP_BUNDLE_HDR_BUF_SIZE, &bundle->primary_block, false);
    if(status <= 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%d) to write primary block of forwarded bundle\n", status);
    hdr_index += status;

    /* Write Custody Block */
    if(bundle->primary_block.request_custody)
    {
        fcteb->cstnode = local_node;
        fcteb->cstserv = local_service;
        bplib_ipn2eid(fcteb->csteid, BP_MAX_EID_STRING, local_node, local_service);
        ds->cidsdnv = fcteb->cid;
        ds->cteboffset = hdr_index;
        status = bplib_blk_cteb_write(&ds->header[ds->cteboffset], BP_BUNDLE_HDR_BUF_SIZE - ds->cteboffset, fcteb, false);
        if(status <= 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%d) to write custody block of forwarded bundle\n", status);
        hdr_index += status;
    }

    /* Write Integrity Block */
    ds->biboffset = hdr_index;
    status = bplib_blk_bib_write(&ds->header[ds->biboffset], BP_BUNDLE_HDR_BUF_SIZE - ds->biboffset, fbib, false);
    if(status <= 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%d) to write integrity block of forwarded bundle\n", status);
    hdr_index += status;

    /* Copy Non-excluded Header Regions */
    for(i = 1; (i + 1) < num_excludes; i+=2)
    {
        int start_index = exclude[i];
        int stop_index = exclude[i + 1];
        int bytes_to_copy = stop_index - start_index;
        if((hdr_index + bytes_to_copy) >= BP_BUNDLE_HDR_BUF_SIZE)
        {
            return bplog(BP_BUNDLETOOLARGE, "Non-excluded forwarded blocks exceed maximum header size (%d)\n", hdr_index);
        }
        else
        {
            bplib_os_memcpy(&ds->header[hdr_index], &buffer[start_index], bytes_to_copy);
            hdr_index += bytes_to_copy;
        }
    }

    /* Initialize Payload Block */
    bundle->payload_block = *pay;

    /* Initialize Payload Block Offset */
    ds->payoffset = hdr_index;

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * store_data_bundle -
 *-------------------------------------------------------------------------------------*/
static int store_data_bundle(bp_data_bundle_t* bundle, bp_store_enqueue_t enqueue, int handle, int timeout, uint16_t* storflags)
{
    int                 status          = 0;
    int                 payload_offset  = 0;
    bp_blk_pri_t*       pri             = &bundle->primary_block;
    bp_blk_pay_t*       pay             = &bundle->payload_block;
    bp_bundle_store_t*  ds              = &bundle->bundle_store;;

    /* Check Fragmentation */
    if(!pri->is_frag && (pay->paysize > bundle->maxlength))
    {
        return bplog(BP_BUNDLETOOLARGE, "Bundle is not being fragmented yet the payload is too large (%d)\n", pay->paysize);
    }

    /* Originator Specific Steps */
    if(bundle->originate)
    {
        /* Set Creation Time */
        pri->createsec.value = bplib_os_systime();
        bplib_sdnv_write(ds->header, BP_BUNDLE_HDR_BUF_SIZE, pri->createsec, storflags);

        /* Set Sequence */
        bplib_sdnv_write(ds->header, BP_BUNDLE_HDR_BUF_SIZE, pri->createseq, storflags);
    }

    /* Set Expiration Time of Bundle */
    if(pri->lifetime.value != 0)    ds->exprtime = pri->createsec.value + pri->lifetime.value;
    else                            ds->exprtime = 0;

    /* Enqueue Bundle */
    while(payload_offset < pay->paysize)
    {
        /* Calculate Storage Header Size and Fragment Size */
        int payload_remaining = pay->paysize - payload_offset;
        int fragment_size = bundle->maxlength <  payload_remaining ? bundle->maxlength : payload_remaining;

        /* Update Primary Block Fragmentation */
        if(pri->is_frag)
        {
            pri->fragoffset.value = payload_offset;
            pri->paylen.value = pay->paysize;
            bplib_sdnv_write(ds->header, BP_BUNDLE_HDR_BUF_SIZE, pri->fragoffset, storflags);
            bplib_sdnv_write(ds->header, BP_BUNDLE_HDR_BUF_SIZE, pri->paylen, storflags);
        }

        /* Update Integrity Block */
        bplib_blk_bib_update(&ds->header[ds->biboffset], BP_BUNDLE_HDR_BUF_SIZE - ds->biboffset, &pay->payptr[payload_offset], fragment_size, &bundle->integrity_block);

        /* Write Payload Block (static portion) */
        pay->blklen.value = fragment_size;
        status = bplib_blk_pay_write(&ds->header[ds->payoffset], BP_BUNDLE_HDR_BUF_SIZE - ds->payoffset, pay, false);
        if(status <= 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%d) to write payload block (static portion) of bundle\n", status);
        ds->headersize = ds->payoffset + status;
        ds->bundlesize = ds->headersize + fragment_size;

        /* Enqueue Bundle */
        int storage_header_size = sizeof(bp_bundle_store_t) - (BP_BUNDLE_HDR_BUF_SIZE - ds->headersize);
        status = enqueue(handle, ds, storage_header_size, &pay->payptr[payload_offset], fragment_size, timeout);
        if(status <= 0) return bplog(status, "Failed (%d) to store bundle in storage system\n", status);
        payload_offset += fragment_size;
    }

    /* Increment Sequence Count (done here since now bundle successfully stored) */
    if(bundle->originate) pri->createseq.value++;

    /* Return Payload Bytes Stored */
    return payload_offset;
}

/*--------------------------------------------------------------------------------------
 * initialize_dacs_bundle - Initialize ACS Bundle Header from Channel Struct
 *
 *  Does not populate following (see update_dacs_header):
 *      - creation time (when using system time)
 *      - creation sequence
 *      - total payload length
 *      - payload crc
 *      - payload block length
 *-------------------------------------------------------------------------------------*/
static int initialize_dacs_bundle(bp_channel_t* ch, int dac_entry, uint32_t dstnode, uint32_t dstserv)
{
    bp_dacs_bundle_t*   bundle  = &ch->dacs_bundle[dac_entry];
    bp_bundle_store_t*  ds      = &bundle->bundle_store;
    uint8_t*            hdrbuf  = ds->header;
    uint16_t            flags   = 0;

    /* Initialize Storage */
    bplib_os_memset(ds, 0, sizeof(bp_bundle_store_t));

    /* Initialize Header */
    ds->biboffset  = bplib_blk_pri_write (&hdrbuf[0],             BP_BUNDLE_HDR_BUF_SIZE,                 &bundle->primary_block,   false);
    ds->payoffset  = bplib_blk_bib_write (&hdrbuf[ds->biboffset], BP_BUNDLE_HDR_BUF_SIZE - ds->biboffset, &bundle->integrity_block, false) + ds->biboffset;
    ds->headersize = bplib_blk_pay_write (&hdrbuf[ds->payoffset], BP_BUNDLE_HDR_BUF_SIZE - ds->payoffset, &bundle->payload_block,   false) + ds->payoffset;

    bundle->primary_block.dstnode.value = dstnode;
    bundle->primary_block.dstserv.value = dstserv;

    bplib_sdnv_write(hdrbuf, BP_BUNDLE_HDR_BUF_SIZE, bundle->primary_block.dstnode, &flags);
    bplib_sdnv_write(hdrbuf, BP_BUNDLE_HDR_BUF_SIZE, bundle->primary_block.dstserv, &flags);

    /* Return Status */
    if(flags != 0)  return BP_BUNDLEPARSEERR;
    else            return ds->headersize;
}

/*--------------------------------------------------------------------------------------
 * store_dacs_bundle -
 *
 *  Notes:
 *-------------------------------------------------------------------------------------*/
static int store_dacs_bundle(bp_channel_t* ch, bp_dacs_bundle_t* dacs, uint32_t sysnow, int timeout, uint16_t* dacsflags)
{
    int dacs_size, enstat, storage_header_size;

    bp_bundle_store_t* ds = &dacs->bundle_store;
    bp_blk_pri_t* pri = &dacs->primary_block;

    /* Build DACS */
    dacs_size = bplib_rec_acs_write(dacs->paybuf, dacs->paybuf_size, dacs->delivered, dacs->first_cid, dacs->fills, dacs->num_fills);
    ds->bundlesize = ds->headersize + dacs_size;
    storage_header_size = sizeof(bp_bundle_store_t) - (BP_BUNDLE_HDR_BUF_SIZE - ds->headersize);

    /* Set Creation Time */
    pri->createsec.value = sysnow;
    bplib_sdnv_write(ds->header, BP_BUNDLE_HDR_BUF_SIZE, pri->createsec, dacsflags);

    /* Set Sequence */
    bplib_sdnv_write(ds->header, BP_BUNDLE_HDR_BUF_SIZE, pri->createseq, dacsflags);
    pri->createseq.value++;

    /* Update Bundle Integrity Block */
    bplib_blk_bib_update(&ds->header[ds->biboffset], BP_BUNDLE_HDR_BUF_SIZE - ds->biboffset, dacs->paybuf, dacs_size, &dacs->integrity_block);

    /* Update Payload Block */
    dacs->payload_block.payptr = dacs->paybuf;
    dacs->payload_block.paysize = dacs_size;
    dacs->payload_block.blklen.value = dacs_size;
    bplib_sdnv_write(&ds->header[ds->payoffset], BP_BUNDLE_HDR_BUF_SIZE - ds->payoffset, dacs->payload_block.blklen, dacsflags);

    /* Send (enqueue) DACS */
    enstat = ch->storage.enqueue(ch->dacs_store_handle, ds, storage_header_size, dacs->paybuf, dacs_size, timeout);

    /* Check Storage Status */
    if(enstat <= 0)
    {
        *dacsflags |= BP_FLAG_STOREFAILURE;
        return bplog(enstat, "Failed (%d) to store DACS for transmission, bundle dropped\n", enstat);
    }
    else // successfully enqueued
    {
        dacs->sent = true;
        return dacs_size;
    }
}

/*--------------------------------------------------------------------------------------
 * update_dacs_bundle -
 *
 *  Notes:
 *  1) may or may not perform enqueue depending if DACS needs to be sent
 *  2) delivered refers to payloads; the alternative is a forwarded bundle
 *-------------------------------------------------------------------------------------*/
static int update_dacs_bundle(bp_channel_t* ch, bp_blk_cteb_t* cteb, bool delivered, int timeout, uint16_t* dacsflags)
{
    int i;
    bp_dacs_bundle_t* dacs;
    bool store_dacs = false;

    /* Find DACS Entry */
    dacs = NULL;
    for(i = 0; i < ch->num_dacs; i++)
    {
        if(ch->dacs_bundle[i].cstnode == cteb->cstnode && ch->dacs_bundle[i].cstserv == cteb->cstserv)
        {
            dacs = &ch->dacs_bundle[i];
            break;
        }
    }

    /* Handle Entry Not Found */
    if(dacs == NULL)
    {
        if(ch->num_dacs < ch->attributes.max_concurrent_dacs)
        {
            /* Set Pointers */
            int dacs_entry = ch->num_dacs++;
            dacs = &ch->dacs_bundle[dacs_entry];

            /* Initial DACS Bundle */
            dacs->cstnode = cteb->cstnode;
            dacs->cstserv = cteb->cstserv;
            dacs->num_fills = 0;
            initialize_dacs_bundle(ch, dacs_entry, cteb->cstnode, cteb->cstserv);
        }
        else
        {
            /* No Room in Table for Another Source */
            *dacsflags |= BP_FLAG_TOOMANYSOURCES;
            return bplog(BP_FAILEDRESPONSE, "No room in DACS table for another source %d.%d\n", cteb->cstnode, cteb->cstserv);
        }
    }

    /* Populate/Send ACS Bundle(s) */
    if(dacs != NULL)
    {
        if(dacs->num_fills == 0)
        {
            /* Start New ACS */
            dacs->delivered = delivered;
            dacs->first_cid = cteb->cid.value;
            dacs->last_cid = cteb->cid.value;
            dacs->fills[0] = 1;
            dacs->num_fills = 1;
        }
        else if(cteb->cid.value <= dacs->last_cid)
        {
            /* Mark CID Going Backwards */
            *dacsflags |= BP_FLAG_CIDWENTBACKWARDS;
            store_dacs = true;
        }
        else if(dacs->delivered != delivered)
        {
            /* Mark Mixed Response */
            *dacsflags |= BP_FLAG_MIXEDRESPONSE;
            store_dacs = true;
        }
        else // cid > dacs->last_cid
        {
            /* Update Fill */
            uint32_t cid_delta = cteb->cid.value - dacs->last_cid;
            uint32_t hop_val = cid_delta - 1;
            int fill_index = dacs->num_fills - 1;

            dacs->last_cid = cteb->cid.value; // save last CID

            if((fill_index + 2) < ch->attributes.max_fills_per_dacs)
            {
                if(hop_val == 0)
                {
                    dacs->fills[fill_index]++;
                    if(dacs->fills[fill_index] >= BP_MAX_FILL)
                    {
                        dacs->fills[fill_index + 1] = 0;
                        dacs->fills[fill_index + 2] = 0;
                        dacs->num_fills += 2;
                    }
                }
                else if(hop_val < BP_MAX_FILL)
                {
                    dacs->fills[fill_index + 1] = hop_val;
                    dacs->fills[fill_index + 2] = 1;
                    dacs->num_fills += 2;
                }
                else // best effort
                {
                    dacs->fills[fill_index + 1] = BP_MAX_FILL;
                    dacs->fills[fill_index + 2] = 0;
                    dacs->num_fills += 2;
                    *dacsflags |= BP_FLAG_FILLOVERFLOW;
                    store_dacs = true;
                }
            }
            else
            {
                *dacsflags |= BP_FLAG_TOOMANYFILLS;
                store_dacs = true;
            }
        }

        /* Store DACS */
        if(store_dacs)
        {
            uint32_t sysnow = bplib_os_systime();
            store_dacs_bundle(ch, dacs, sysnow, timeout, dacsflags);

            /* Start New DTN ACS */
            dacs->delivered = delivered;
            dacs->first_cid = cteb->cid.value;
            dacs->last_cid = cteb->cid.value;
            dacs->fills[0] = 1;
            dacs->num_fills = 1;
        }
    }

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * getset_opt - Get/Set utility function
 *
 *  - getset --> false: get, true: set
 *  - assumes parameter checking has already been performed
 *-------------------------------------------------------------------------------------*/
static int getset_opt(int c, int opt, void* val, int len, bool getset)
{
    bp_channel_t* ch = &channels[c];

    /* Select and Process Option */
    switch(opt)
    {
        case BP_OPT_DSTNODE_D:
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* node = (bp_ipn_t*)val;
            if(getset)  ch->data_bundle.primary_block.dstnode.value = *node;
            else        *node = ch->data_bundle.primary_block.dstnode.value;
            break;
        }
        case BP_OPT_DSTSERV_D:
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* service = (bp_ipn_t*)val;
            if(getset)  ch->data_bundle.primary_block.dstserv.value = *service;
            else        *service = ch->data_bundle.primary_block.dstserv.value;
            break;
        }
        case BP_OPT_RPTNODE_D:
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* node = (bp_ipn_t*)val;
            if(getset)  ch->data_bundle.primary_block.rptnode.value = *node;
            else        *node = ch->data_bundle.primary_block.rptnode.value;
            break;
        }
        case BP_OPT_RPTSERV_D:
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* service = (bp_ipn_t*)val;
            if(getset)  ch->data_bundle.primary_block.rptserv.value = *service;
            else        *service = ch->data_bundle.primary_block.rptserv.value;
            break;
        }
        case BP_OPT_CSTNODE_D:
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* node = (bp_ipn_t*)val;
            if(getset)  ch->data_bundle.primary_block.cstnode.value = *node;
            else        *node = ch->data_bundle.primary_block.cstnode.value;
            break;
        }
        case BP_OPT_CSTSERV_D:
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* service = (bp_ipn_t*)val;
            if(getset)  ch->data_bundle.primary_block.cstserv.value = *service;
            else        *service = ch->data_bundle.primary_block.cstserv.value;
            break;
        }
        case BP_OPT_SETSEQUENCE_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            uint32_t* seq = (uint32_t*)val;
            if(getset)  ch->data_bundle.primary_block.createseq.value = *seq;
            else        *seq = ch->data_bundle.primary_block.createseq.value;
            break;
        }
        case BP_OPT_LIFETIME_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* lifetime = (int*)val;
            if(getset)  ch->data_bundle.primary_block.lifetime.value = *lifetime;
            else        *lifetime = ch->data_bundle.primary_block.lifetime.value;
            break;
        }
        case BP_OPT_CSTRQST_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(*enable != true && *enable != false) return BP_PARMERR;
            if(getset)  ch->data_bundle.primary_block.request_custody = *enable;
            else        *enable = ch->data_bundle.primary_block.request_custody;
            break;
        }
        case BP_OPT_ALLOWFRAG_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(*enable != true && *enable != false) return BP_PARMERR;
            if(getset)
            {
                ch->data_bundle.primary_block.allow_frag = *enable;
                ch->data_bundle.primary_block.is_frag = *enable;
            }
            else
            {
                *enable = ch->data_bundle.primary_block.allow_frag;
                *enable = ch->data_bundle.primary_block.is_frag;
            }
            break;
        }
        case BP_OPT_PAYCRC_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* type = (int*)val;
            if(getset)  ch->data_bundle.integrity_block.paytype.value = *type;
            else        *type = ch->data_bundle.integrity_block.paytype.value;
            break;
        }
        case BP_OPT_TIMEOUT:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* timeout = (int*)val;
            if(getset)  ch->timeout = *timeout;
            else        *timeout = ch->timeout;
            break;
        }
        case BP_OPT_BUNDLELEN:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* maxlen = (int*)val;
            if(getset)  ch->data_bundle.maxlength = *maxlen;
            else        *maxlen = ch->data_bundle.maxlength;
            break;
        }
        case BP_OPT_ORIGINATE:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(*enable != true && *enable != false) return BP_PARMERR;
            if(getset)  ch->data_bundle.originate = *enable;
            else        *enable = ch->data_bundle.originate;
            break;
        }
        case BP_OPT_PROCADMINONLY:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(*enable != true && *enable != false) return BP_PARMERR;
            if(getset)  ch->proc_admin_only = *enable;
            else        *enable = ch->proc_admin_only;
            break;
        }
        case BP_OPT_WRAPRSP:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* wrap = (int*)val;
            if(*wrap != BP_WRAP_RESEND && *wrap != BP_WRAP_BLOCK && *wrap != BP_WRAP_DROP) return BP_PARMERR;
            if(getset)  ch->wrap_response = *wrap;
            else        *wrap = ch->wrap_response;
            break;
        }
        case BP_OPT_CIDREUSE:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(*enable != true && *enable != false) return BP_PARMERR;
            if(getset)  ch->cid_reuse = *enable;
            else        *enable = ch->cid_reuse;
            break;
        }
        case BP_OPT_ACSRATE:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* rate = (int*)val;
            if(getset)  ch->dacs_rate = *rate;
            else        *rate = ch->dacs_rate;
            break;
        }
        default:
        {
            /* Option Not Found */
            return bplog(BP_PARMERR, "Config. Option Not Found (%d)\n", opt);
        }
    }

    /* Re-initialize Bundles */
    if(getset) initialize_orig_bundle(&ch->data_bundle);

    /* Option Successfully Processed */
    return BP_SUCCESS;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_init - initializes bp library
 *-------------------------------------------------------------------------------------*/
void bplib_init(int max_channels)
{
    int i;

    /* Initialize OS Interface */
    bplib_os_init();

    /* Create Channel Lock */
    channels_lock = bplib_os_createlock();

    /* Allocate Channel Memory */
    if(max_channels <= 0)   channels_max = BP_DEFAULT_MAX_CHANNELS;
    else                    channels_max = max_channels;
    channels = (bp_channel_t*)malloc(channels_max * sizeof(bp_channel_t));

    /* Set all channel control blocks to empty */
    for(i = 0; i < channels_max; i++)
    {
        channels[i].index = BP_EMPTY;
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_open -
 *-------------------------------------------------------------------------------------*/
int bplib_open(bp_store_t storage, bp_ipn_t local_node, bp_ipn_t local_service, bp_ipn_t destination_node, bp_ipn_t destination_service, bp_attr_t* attributes)
{
    int i, j, channel;

    assert(storage.create);
    assert(storage.destroy);
    assert(storage.enqueue);
    assert(storage.dequeue);
    assert(storage.retrieve);
    assert(storage.relinquish);
    assert(storage.getcount);

    channel = -1;
    bplib_os_lock(channels_lock);
    {
        /* Find open channel slot */
        for(i = 0; i < channels_max; i++)
        {
            if(channels[i].index == BP_EMPTY)
            {
                /* Clear Channel Memory */
                bplib_os_memset(&channels[i], 0, sizeof(channels[i]));
                
                /* Set Active Table Size Attribute */
                if(attributes && attributes->active_table_size > 0) channels[i].attributes.active_table_size = attributes->active_table_size;
                else channels[i].attributes.active_table_size = BP_DEFAULT_ACTIVE_TABLE_SIZE;
                    
                /* Set Max Concurrent DACS Attribute */
                if(attributes && attributes->max_concurrent_dacs > 0) channels[i].attributes.max_concurrent_dacs = attributes->max_concurrent_dacs;
                else channels[i].attributes.max_concurrent_dacs = BP_DEFAULT_MAX_CONCURRENT_DACS;
                
                /* Set Max Fills per DACS Attribute */
                if(attributes && attributes->max_fills_per_dacs > 0) channels[i].attributes.max_fills_per_dacs = attributes->max_fills_per_dacs;
                else channels[i].attributes.max_fills_per_dacs = BP_DEFAULT_MAX_FILLS_PER_DACS;
                                
                /* Initialize Assets */
                channels[i].data_bundle_lock     = bplib_os_createlock();
                channels[i].dacs_bundle_lock     = bplib_os_createlock();
                channels[i].active_table_signal  = bplib_os_createlock();
                channels[i].local_node           = local_node;
                channels[i].local_service        = local_service;
                channels[i].storage              = storage; // structure copy
                channels[i].data_store_handle    = channels[i].storage.create();
                channels[i].payload_store_handle = channels[i].storage.create();
                channels[i].dacs_store_handle    = channels[i].storage.create();

                /* Check Assets */
                if( channels[i].data_bundle_lock  < 0 ||
                    channels[i].dacs_bundle_lock  < 0 ||
                    channels[i].active_table_signal < 0 )
                {
                    bplib_os_unlock(channels_lock);
                    return bplog(BP_FAILEDOS, "Failed to allocate OS locks for channel\n");
                }
                else if( channels[i].data_store_handle    < 0 ||
                         channels[i].payload_store_handle < 0 ||
                         channels[i].dacs_store_handle    < 0 )
                {
                    bplib_os_unlock(channels_lock);
                    return bplog(BP_FAILEDSTORE, "Failed to create storage handles for channel\n");
                }

                /* Register Channel */
                channels[i].index = i;

                /* Initialize Data Bundle */
                channels[i].data_bundle.primary_block               = native_data_pri_blk;
                channels[i].data_bundle.primary_block.dstnode.value = destination_node;
                channels[i].data_bundle.primary_block.dstserv.value = destination_service;
                channels[i].data_bundle.primary_block.srcnode.value = local_node;
                channels[i].data_bundle.primary_block.srcserv.value = local_service;
                channels[i].data_bundle.primary_block.rptnode.value = local_node;
                channels[i].data_bundle.primary_block.rptserv.value = local_service;
                channels[i].data_bundle.primary_block.cstnode.value = local_node;
                channels[i].data_bundle.primary_block.cstserv.value = local_service;
                channels[i].data_bundle.custody_block               = native_cteb_blk;
                channels[i].data_bundle.custody_block.cid.value     = 0;
                channels[i].data_bundle.custody_block.cstnode       = local_node;
                channels[i].data_bundle.custody_block.cstserv       = local_service;
                channels[i].data_bundle.integrity_block             = native_bib_blk;
                channels[i].data_bundle.payload_block               = native_pay_blk;
                channels[i].data_bundle.maxlength                   = BP_DEFAULT_BUNDLE_MAXLENGTH;
                channels[i].data_bundle.originate                   = BP_DEFAULT_ORIGINATION;

                /* Write EID */
                bplib_ipn2eid(channels[i].data_bundle.custody_block.csteid, BP_MAX_EID_STRING, local_node, local_service);

                /* Allocate Memory for Channel DACS Bundle */
                channels[i].dacs_bundle = (bp_dacs_bundle_t*)malloc(sizeof(bp_dacs_bundle_t) * channels[i].attributes.max_concurrent_dacs);
                if(channels[i].dacs_bundle == NULL)
                {
                    bplib_os_unlock(channels_lock);
                    return bplog(BP_FAILEDMEM, "Failed to allocate memory for channel dacs\n");                    
                }
                else
                {
                    bplib_os_memset(channels[i].dacs_bundle, 0, sizeof(bp_dacs_bundle_t) * channels[i].attributes.max_concurrent_dacs);
                }
                
                /* Initialize DACS Bundle */
                for(j = 0; j < channels[i].attributes.max_concurrent_dacs; j++)
                {
                    /* Allocate Memory for Channel DACS Bundle Fills */
                    channels[i].dacs_bundle[j].fills = (uint32_t*)malloc(sizeof(uint32_t) * channels[i].attributes.max_fills_per_dacs);
                    channels[i].dacs_bundle[j].paybuf_size = sizeof(uint8_t) * sizeof(uint16_t) * channels[i].attributes.max_fills_per_dacs + 32; // 2 bytes per fill plus payload block header
                    channels[i].dacs_bundle[j].paybuf = (uint8_t*)malloc(channels[i].dacs_bundle[j].paybuf_size); 
                    if(channels[i].dacs_bundle[j].fills == NULL || channels[i].dacs_bundle[j].paybuf == NULL)
                    {
                        bplib_os_unlock(channels_lock);
                        return bplog(BP_FAILEDMEM, "Failed to allocate memory for channel dacs fills and payload\n");                    
                    }
                    else
                    {
                        bplib_os_memset(channels[i].dacs_bundle[j].fills, 0, sizeof(uint32_t) * channels[i].attributes.max_fills_per_dacs);
                        bplib_os_memset(channels[i].dacs_bundle[j].paybuf, 0, channels[i].dacs_bundle[j].paybuf_size);
                    }
    
                    /* Initialize DACS Bundle Fields */
                    channels[i].dacs_bundle[j].primary_block               = native_dacs_pri_blk;
                    channels[i].dacs_bundle[j].primary_block.srcnode.value = local_node;
                    channels[i].dacs_bundle[j].primary_block.srcserv.value = local_service;
                    channels[i].dacs_bundle[j].primary_block.rptnode.value = local_node;
                    channels[i].dacs_bundle[j].primary_block.rptserv.value = local_service;
                    channels[i].dacs_bundle[j].primary_block.cstnode.value = local_node;
                    channels[i].dacs_bundle[j].primary_block.cstserv.value = local_service;
                    channels[i].dacs_bundle[j].integrity_block             = native_bib_blk;
                    channels[i].dacs_bundle[j].payload_block               = native_pay_blk;
                }

                /* Allocate Memory for Active Table */
                channels[i].active_table.sid = (bp_sid_t*)malloc(sizeof(bp_sid_t) * channels[i].attributes.active_table_size);
                channels[i].active_table.retx = (uint32_t*)malloc(sizeof(uint32_t) * channels[i].attributes.active_table_size);
                if(channels[i].active_table.sid == NULL || channels[i].active_table.retx == NULL)
                {
                    bplib_os_unlock(channels_lock);
                    return bplog(BP_FAILEDMEM, "Failed to allocate memory for channel active table\n");                    
                }
                else
                {
                    bplib_os_memset(channels[i].active_table.sid, 0, sizeof(uint32_t) * channels[i].attributes.active_table_size);
                    bplib_os_memset(channels[i].active_table.retx, 0, sizeof(uint32_t) * channels[i].attributes.active_table_size);
                }
                
                /* Initialize Data */
                channels[i].active_table.oldest_cid     = 0;
                channels[i].active_table.current_cid    = 0;
                channels[i].num_dacs                    = 0;
                channels[i].dacs_rate                   = BP_DEFAULT_DACS_RATE;
                channels[i].timeout                     = BP_DEFAULT_TIMEOUT;
                channels[i].proc_admin_only             = BP_DEFAULT_PROC_ADMIN_ONLY;
                channels[i].wrap_response               = BP_DEFAULT_WRAP_RESPONSE;
                channels[i].cid_reuse                   = BP_DEFAULT_CID_REUSE;

                /* Populate Initial Data Bundle Storage Header
                 *  only initialize data bundle and not dacs or forwarded bundles
                 *  since for storage the dacs bundles are initialized
                 *  when custody requests arrive, and forwarded bundles are
                 *  initialized each time a bundle is forwarded */
                initialize_orig_bundle(&channels[i].data_bundle);

                /* Set Channel Handle */
                channel = i;
                break;
            }
        }
    }
    bplib_os_unlock(channels_lock);

    /* Return Channel or Error */
    if(channel < 0) return bplog(BP_CHANNELSFULL, "Cannot open channel, not enough room\n");
    else            return channel;
}

/*--------------------------------------------------------------------------------------
 * bplib_close -
 *-------------------------------------------------------------------------------------*/
void bplib_close(int channel)
{
    if(channel >= 0 && channel < channels_max && channels[channel].index != BP_EMPTY)
    {
        bplib_os_lock(channels_lock);
        {
            channels[channel].storage.destroy(channels[channel].data_store_handle);
            channels[channel].storage.destroy(channels[channel].payload_store_handle);
            channels[channel].storage.destroy(channels[channel].dacs_store_handle);
            bplib_os_destroylock(channels[channel].data_bundle_lock);
            bplib_os_destroylock(channels[channel].dacs_bundle_lock);
            bplib_os_destroylock(channels[channel].active_table_signal);
            channels[channel].index = BP_EMPTY;
        }
        bplib_os_unlock(channels_lock);
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_getopt -
 *-------------------------------------------------------------------------------------*/
int bplib_getopt(int channel, int opt, void* val, int len)
{
    int status;

    /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(val == NULL)                            return BP_PARMERR;

    /* Call Internal Function */
    status = getset_opt(channel, opt, val, len, false);

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_setopt -
 *-------------------------------------------------------------------------------------*/
int bplib_setopt(int channel, int opt, void* val, int len)
{
    int status;

     /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(val == NULL)                            return BP_PARMERR;

    /* Call Internal Function */
    status = getset_opt(channel, opt, val, len, true);

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_latchstats -
 *-------------------------------------------------------------------------------------*/
int bplib_latchstats(int channel, bp_stats_t* stats)
{
     /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(stats == NULL)                          return BP_PARMERR;

    /* Shortcut to Channel */
    bp_channel_t* ch = &channels[channel];

    /* Update Store Counts */
    ch->stats.bundles = ch->storage.getcount(ch->data_store_handle);
    ch->stats.payloads = ch->storage.getcount(ch->payload_store_handle);
    ch->stats.records = ch->storage.getcount(ch->dacs_store_handle);

    /* Latch Statistics */
    *stats = ch->stats;

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_store -
 *-------------------------------------------------------------------------------------*/
int bplib_store(int channel, void* payload, int size, int timeout, uint16_t* storflags)
{
    int status;

    /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(payload == NULL)                        return BP_PARMERR;

    /* Lock Data Bundle */
    bplib_os_lock(channels[channel].data_bundle_lock);
    {
        bp_channel_t* ch = &channels[channel];

        if(!ch->data_bundle.originate)
        {
            status = bplog(BP_WRONGORIGINATION, "Cannot originate bundle on channel designated for forwarding\n");
        }
        else
        {
            /* Update Payload */
            ch->data_bundle.payload_block.payptr = (uint8_t*)payload;
            ch->data_bundle.payload_block.paysize = size;

            /* Store Bundle */
            status = store_data_bundle(&ch->data_bundle, ch->storage.enqueue, ch->data_store_handle, timeout, storflags);
        }
    }
    bplib_os_unlock(channels[channel].data_bundle_lock);

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_load -
 *-------------------------------------------------------------------------------------*/
int bplib_load(int channel, void* bundle, int size, int timeout, uint16_t* loadflags)
{
    int status = BP_SUCCESS; // size of bundle returned or error code

    /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(bundle == NULL)                         return BP_PARMERR;

    /* Set Short Cuts */
    bp_channel_t*           ch          = &channels[channel];
    bp_store_dequeue_t      dequeue     = ch->storage.dequeue;
    bp_store_retrieve_t     retrieve    = ch->storage.retrieve;
    bp_store_relinquish_t   relinquish  = ch->storage.relinquish;

    /* Setup State */
    uint32_t                sysnow      = bplib_os_systime();   // get current system time (used for timeouts, seconds)
    bp_bundle_store_t*      ds          = NULL;                 // start out assuming nothing to send
    int                     store       = -1;                   // handle for storage of bundle being loaded
    bp_sid_t                sid         = BP_SID_VACANT;        // storage id points to nothing
    int                     ati         = -1;                   // active table index
    bool                    newcid      = true;                 // whether to assign new custody id and active table entry

    /* Lock DACS Bundle */
    bplib_os_lock(ch->dacs_bundle_lock);
    {
        /* Check DACS Rate */
        if((ch->dacs_rate > 0) && ((ch->last_dacs + ch->dacs_rate) <= sysnow))
        {
            int i;
            for(i = 0; i < ch->num_dacs; i++)
            {
                bp_dacs_bundle_t* dacs = &ch->dacs_bundle[i];

                /* Check for Unsent DACS */
                if(dacs->sent == false && dacs->num_fills > 0)
                {
                    store_dacs_bundle(ch, dacs, sysnow, BP_CHECK, loadflags);
                    dacs->num_fills = 0;
                }

                /* Clear Sent Flag */
                ch->dacs_bundle[i].sent = false;
            }

            /* Update Time of Last Check */
            ch->last_dacs = sysnow;
        }
    }
    bplib_os_unlock(ch->dacs_bundle_lock);

    /* Check if DACS Needs to be Sent */
    if(dequeue(ch->dacs_store_handle, (void**)&ds, NULL, &sid, BP_CHECK) == BP_SUCCESS)
    {
        /* Send DACS - (ds is not null) */
        store = ch->dacs_store_handle;

        /* Set Route Flag */
        *loadflags |= BP_FLAG_ROUTENEEDED;
    }
    else
    {
        /* Send Data - (if ds is set below) */
        store = ch->data_store_handle;
    }

    /* Process Active Table for Timeouts */
    bplib_os_lock(ch->active_table_signal);
    {
        /* Try to Send Timed-out Bundle */
        while((ds == NULL) && (ch->active_table.oldest_cid < ch->active_table.current_cid))
        {
            ati = ch->active_table.oldest_cid % ch->attributes.active_table_size;
            sid = ch->active_table.sid[ati];
            if(sid == BP_SID_VACANT) // entry vacant
            {
                ch->active_table.oldest_cid++;
            }
            else if(retrieve(ch->data_store_handle, (void**)&ds, NULL, sid, BP_CHECK) == BP_SUCCESS)
            {
                if(ds->exprtime != 0 && sysnow >= ds->exprtime) // check lifetime
                {
                    /* Bundle Expired - Clear Entry */
                    relinquish(ch->data_store_handle, sid);
                    ch->active_table.sid[ati] = BP_SID_VACANT;
                    ch->active_table.oldest_cid++;
                    ch->stats.expired++;
                    ds = NULL;
                }
                else if(ch->active_table.retx[ati] != 0 && sysnow >= ch->active_table.retx[ati]) // check timeout
                {
                    /* Retransmit Bundle */
                    ch->stats.retransmitted++;

                    /* Handle Active Table and Custody ID */
                    if(ch->cid_reuse)
                    {
                        /* Set flag to reuse custody id and active table entry */
                        newcid = false;
                    }
                    else
                    {
                        /* Clear Entry (it will be reinserted below at the current pointer) */
                        ch->active_table.sid[ati] = BP_SID_VACANT;
                        ch->active_table.oldest_cid++;
                    }
                }
                else // oldest active bundle still active
                {
                    /* Bundle Not Ready to Retransmit */
                    ds = NULL;

                    /* Check Active Table Has Room
                     * Since next step is to dequeue from storage, need to make
                     * sure that there is room in the active table since we don't
                     * want to dequeue a bundle from storage and have no place
                     * to put it.  Note that it is possible that even if the
                     * active table was full, if the bundle dequeued did not
                     * request custody transfer it could still go out, but the
                     * current design requires that at least one slot in the active
                     * table is open at all times. */
                    ati = ch->active_table.current_cid % ch->attributes.active_table_size;
                    sid = ch->active_table.sid[ati];
                    if(sid != BP_SID_VACANT) // entry vacant
                    {
                        *loadflags |= BP_FLAG_ACTIVETABLEWRAP;

                        if(ch->wrap_response == BP_WRAP_RESEND)
                        {
                            /* Bump Oldest Custody ID */
                            ch->active_table.oldest_cid++;
                            
                            /* Retrieve Bundle from Storage */
                            if(retrieve(ch->data_store_handle, (void**)&ds, NULL, sid, BP_CHECK) != BP_SUCCESS)
                            {
                                /* Failed to Retrieve - Clear Entry (and loop again) */
                                relinquish(ch->data_store_handle, sid);
                                ch->active_table.sid[ati] = BP_SID_VACANT;
                                *loadflags |= BP_FLAG_STOREFAILURE;
                                ch->stats.lost++;
                            }
                            else
                            {
                                /* Force Retransmit - Do Not Reuse Custody ID */
                                ch->stats.retransmitted++;
                                bplib_os_waiton(ch->active_table_signal, BP_DEFAULT_WRAP_TIMEOUT);
                            }
                        }
                        else if(ch->wrap_response == BP_WRAP_BLOCK)
                        {
                            /* Custody ID Wrapped Around to Occupied Slot */                            
                            status = BP_OVERFLOW;                   
                            bplib_os_waiton(ch->active_table_signal, BP_DEFAULT_WRAP_TIMEOUT);
                        }
                        else // if(ch->wrap_response == BP_WRAP_DROP)
                        {
                            /* Bump Oldest Custody ID */
                            ch->active_table.oldest_cid++;

                            /* Clear Entry (and loop again) */
                            relinquish(ch->data_store_handle, sid);
                            ch->active_table.sid[ati] = BP_SID_VACANT;
                            ch->stats.lost++;
                        }
                    }

                    /* Break Out of Loop */
                    break;
                }
            }
            else
            {
                /* Failed to Retrieve Bundle from Storage */
                relinquish(ch->data_store_handle, sid);
                ch->active_table.sid[ati] = BP_SID_VACANT;
                *loadflags |= BP_FLAG_STOREFAILURE;
                ch->stats.lost++;
            }
        }
    }
    bplib_os_unlock(ch->active_table_signal);

    /* Try to Send Stored Bundle (if nothing ready to send yet) */
    while(ds == NULL)
    {
        /* Dequeue Bundle from Storage Service */
        int deq_status = dequeue(ch->data_store_handle, (void**)&ds, NULL, &sid, timeout);
        if(deq_status == BP_SUCCESS)
        {
            if(ds->exprtime != 0 && sysnow >= ds->exprtime)
            {
                /* Bundle Expired Clear Entry (and loop again) */
                relinquish(ch->data_store_handle, sid);
                ch->stats.expired++;
                sid = BP_SID_VACANT;
                ds = NULL;
            }
        }
        else if(deq_status == BP_TIMEOUT)
        {
            /* No Bundles in Storage to Send */
            status = BP_TIMEOUT;
            break;
        }
        else
        {
            /* Failed Storage Service */
            status = BP_FAILEDSTORE;
            *loadflags |= BP_FLAG_STOREFAILURE;
            break;
        }
    }

    /* Process Active Table for Sending Next Bundle */
    bplib_os_lock(ch->active_table_signal);
    {
        /* Check if Bundle Ready to Transmit */
        if(ds != NULL)
        {
            /* Check Buffer Size */
            if(size < ds->bundlesize)
            {
                status = bplog(BP_BUNDLETOOLARGE, "Bundle too large to fit inside buffer (%d %d)\n", size, ds->bundlesize);
                relinquish(store, sid);
                ch->stats.lost++;
            }
            else
            {
                /* If Custody Transfer */
                if(ds->cteboffset != 0)
                {
                    /* Assign Custody ID and Active Table Entry */
                    if(newcid)
                    {
                        ati = ch->active_table.current_cid % ch->attributes.active_table_size;
                        ch->active_table.sid[ati] = sid;
                        ds->cidsdnv.value = ch->active_table.current_cid++;
                        bplib_sdnv_write(&ds->header[ds->cteboffset], ds->bundlesize - ds->cteboffset, ds->cidsdnv, loadflags);
                    }

                    /* Update Retransmit Time */
                    if(ch->timeout > 0) ch->active_table.retx[ati] = sysnow + ch->timeout;
                    else                ch->active_table.retx[ati] = 0;
                }

                /* Successfully Load Bundle to Application and Relinquish Memory */
                bplib_os_memcpy(bundle, ds->header, ds->bundlesize);
                status = ds->bundlesize;
                ch->stats.transmitted++;

                /* If No Custody Transfer - Free Bundle Memory */
                if(ds->cteboffset == 0)
                {
                    relinquish(store, sid);
                }
            }
        }

        /* Update Active Statistic */
        ch->stats.active = ch->active_table.current_cid - ch->active_table.oldest_cid;
    }
    bplib_os_unlock(ch->active_table_signal);

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_process -
 *-------------------------------------------------------------------------------------*/
int bplib_process(int channel, void* bundle, int size, int timeout, uint16_t* procflags)
{
    int                 status;
    bp_channel_t*       ch;

    uint32_t            sysnow;

    uint8_t*            buffer = (uint8_t*)bundle;
    int                 index = 0;

    int                 ei = 0;
    int                 exclude[BP_NUM_EXCLUDE_REGIONS];

    bp_blk_pri_t        pri_blk;

    bool                cteb_present = false;
    int                 cteb_index = 0;
    bp_blk_cteb_t       cteb_blk;

    bool                bib_present = false;
    int                 bib_index = 0;
    bp_blk_bib_t        bib_blk;

    int                 pay_index = 0;
    bp_blk_pay_t        pay_blk;

    /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(bundle == NULL)                         return BP_PARMERR;

    /* Count Reception */
    ch = &channels[channel];
    ch->stats.received++;

    /* Parse Primary Block */
    exclude[ei++] = index;
    status = bplib_blk_pri_read(buffer, size, &pri_blk, true);
    if(status <= 0) return bplog(status, "Failed to parse primary block of size %d\n", size);
    else            index += status;
    exclude[ei++] = index;

    /* Set Processing Flag for Reporting Deletion */
    if(pri_blk.report_deletion) *procflags |= BP_FLAG_REPORTDELETE;

    /* Check Unsupported */
    if(pri_blk.dictlen.value != 0)
    {
        *procflags |= BP_FLAG_NONCOMPLIANT;
        return bplog(BP_UNSUPPORTED, "Unsupported bundle attempted to be processed (%d)\n", pri_blk.dictlen.value);
    }

    /* Check Life Time */
    sysnow = bplib_os_systime();
    if((pri_blk.lifetime.value != 0) && (sysnow >= (pri_blk.lifetime.value + pri_blk.createsec.value)))
    {
        ch->stats.expired++;
        return bplog(BP_EXPIRED, "Expired bundled attempted to be processed \n");
    }

    /* Parse and Process Remaining Blocks */
    while(index < size)
    {
        /* Read Block Information */
        uint8_t blk_type = buffer[index];

        /* Check Block Type */
        if(blk_type == BP_CTEB_BLK_TYPE)
        {
            cteb_present = true;
            cteb_index = index;
            if(pri_blk.request_custody) exclude[ei++] = index;
            status = bplib_blk_cteb_read(&buffer[cteb_index], size - cteb_index, &cteb_blk, true);
            if(status <= 0) return bplog(status, "Failed to parse CTEB block of size %d\n", size - cteb_index);
            else            index += status;
            if(pri_blk.request_custody) exclude[ei++] = index;
        }
        else if(blk_type == BP_BIB_BLK_TYPE)
        {
            bib_present = true;
            bib_index = index;
            exclude[ei++] = index;
            status = bplib_blk_bib_read(&buffer[bib_index], size - bib_index, &bib_blk, true);
            if(status <= 0) return bplog(status, "Failed to parse BIB block of size %d\n", size - bib_index);
            else            index += status;
            exclude[ei++] = index;
        }
        else if(blk_type != BP_PAY_BLK_TYPE) // skip over block
        {
            bp_sdnv_t blk_flags = { 0, 1, 0 };
            bp_sdnv_t blk_len = { 0, 0, 0 };
            int start_index = index;
            blk_len.index = bplib_sdnv_read(&buffer[start_index], size - start_index, &blk_flags, procflags);
            index = bplib_sdnv_read(&buffer[start_index], size - start_index, &blk_len, procflags);

            /* Check Parsing Status */
            if(*procflags & (BP_FLAG_SDNVOVERFLOW | BP_FLAG_SDNVINCOMPLETE))
            {
                return bplog(BP_BUNDLEPARSEERR, "Failed (%0X) to parse block at index %d\n", *procflags, start_index);
            }

            /* Mark Processing as Incomplete */
            *procflags |= BP_FLAG_INCOMPLETE;
            bplog(BP_UNSUPPORTED, "Skipping over unrecognized block of type 0x%02X and size %d\n", blk_type, blk_len.value);

            /* Should transmit status report that block cannot be processed */
            if(blk_flags.value & BP_BLK_NOTIFYNOPROC_MASK) *procflags |= BP_FLAG_NONCOMPLIANT;

            /* Delete bundle since block not recognized */
            if(blk_flags.value & BP_BLK_DELETENOPROC_MASK)
            {
                return bplog(BP_DROPPED, "Dropping bundle with unrecognized block\n");
            }

            /* Should drop block since it cannot be processed */
            if(blk_flags.value & BP_BLK_DROPNOPROC_MASK) *procflags |= BP_FLAG_NONCOMPLIANT;

            /* Mark As Forwarded without Processed */
            blk_flags.value |= BP_BLK_FORWARDNOPROC_MASK;
            bplib_sdnv_write(&buffer[start_index], size - start_index, blk_flags, procflags);
        }
        else // payload block
        {
            pay_index = index;
            exclude[ei++] = index; // start of payload header
            status = bplib_blk_pay_read(&buffer[pay_index], size - pay_index, &pay_blk, true);
            if(status <= 0) return bplog(status, "Failed (%d) to read payload block\n", status);
            else            index += status;
            exclude[ei++] = index + pay_blk.paysize;

            /* Perform Integrity Check */
            if(bib_present)
            {
                status = bplib_blk_bib_verify(pay_blk.payptr, pay_blk.paysize, &bib_blk);
                if(status <= 0) return bplog(status, "Bundle failed integrity check\n");
            }

            /* Check Size of Payload */
            if(pri_blk.is_admin_rec && pay_blk.paysize < 2)
            {
                return bplog(BP_BUNDLEPARSEERR, "Invalid block length: %d\n", pay_blk.paysize);
            }

            /* Process Payload */
            if(pri_blk.is_admin_rec) // Administrative Record
            {
                uint32_t rec_type, rec_status;

                /* Read Record Information */
                rec_type = buffer[index];
                rec_status = buffer[index + 1]; (void)rec_status; // currently not used

                /* Lock Active Table */
                bplib_os_lock(ch->active_table_signal);
                {
                    /* Process Record */
                    if(rec_type == BP_ACS_REC_TYPE)
                    {
                        int acknowledgment_count = 0;
                        status = bplib_rec_acs_process(&buffer[index], size - index, &acknowledgment_count, ch->active_table.sid, ch->attributes.active_table_size, ch->storage.relinquish, ch->data_store_handle);
                        if(acknowledgment_count > 0)
                        {
                            ch->stats.acknowledged += acknowledgment_count;
                            bplib_os_signal(ch->active_table_signal);
                        }
                    }
                    else if(rec_type == BP_CS_REC_TYPE)     status = bplog(BP_UNSUPPORTED, "Custody signal bundles are not supported\n");
                    else if(rec_type == BP_STAT_REC_TYPE)   status = bplog(BP_UNSUPPORTED, "Status report bundles are not supported\n");
                    else                                    status = bplog(BP_UNKNOWNREC, "Unknown administrative record: %u\n", (unsigned int)rec_type);
                }
                bplib_os_unlock(ch->active_table_signal);
            }
            else if(ch->proc_admin_only)
            {
                status = bplog(BP_IGNORE, "Non-administrative bundle ignored\n");
            }
            else if(pri_blk.dstnode.value != ch->local_node) // forward bundle (dst node != local node)
            {
                /* Check Ability to Forward */
                if(ch->data_bundle.originate)
                {
                    status = bplog(BP_WRONGORIGINATION, "Unable to forward bundle on an originating channel\n");
                }
                else if(pay_blk.paysize > ch->data_bundle.maxlength)
                {
                    /* Check Ability to Fragment */
                    if(!pri_blk.allow_frag) status = bplog(BP_BUNDLETOOLARGE, "Unable (%d) to fragment forwarded bundle (%d > %d)\n", BP_UNSUPPORTED, pay_blk.paysize, ch->data_bundle.maxlength);
                    else                    pri_blk.is_frag = true;
                }

                /* Lock Data Bundle */
                bplib_os_lock(ch->data_bundle_lock);
                {
                    /* Initialize Forwarded Bundle */
                    if(status == BP_SUCCESS) status = initialize_forw_bundle(&ch->data_bundle, &pri_blk, &pay_blk, ch->local_node, ch->local_service, cteb_present, buffer, exclude, ei, procflags);

                    /* Store Forwarded Bundle */
                    if(status == BP_SUCCESS) status = store_data_bundle(&ch->data_bundle, ch->storage.enqueue, ch->data_store_handle, timeout, procflags);
                }
                bplib_os_unlock(ch->data_bundle_lock);

                /* Handle Custody Transfer */
                if(status == BP_SUCCESS && pri_blk.request_custody)
                {
                    if(cteb_present)
                    {
                        bplib_os_lock(ch->dacs_bundle_lock);
                        {
                            /* Update DACS (bundle successfully forwarded) */
                            status = update_dacs_bundle(ch, &cteb_blk, false, BP_CHECK, procflags);
                        }
                        bplib_os_unlock(ch->dacs_bundle_lock);
                    }
                    else
                    {
                        *procflags |= BP_FLAG_NONCOMPLIANT;
                        bplog(BP_UNSUPPORTED, "Only aggregate custody supported\n");
                    }
                }
            }
            else if(pri_blk.dstserv.value == ch->local_service) // deliver bundle payload to application
            {
                bp_payload_store_t* pay = &ch->data_bundle.payload_store;
                pay->payloadsize = size - index;

                /* Set Custody Transfer Request */
                pay->request_custody = false;
                if(pri_blk.request_custody)
                {
                    if(cteb_present)
                    {
                        pay->request_custody = true;
                    }
                    else
                    {
                        *procflags |= BP_FLAG_NONCOMPLIANT;
                        bplog(BP_UNSUPPORTED, "Only aggregate custody supported\n");
                    }
                }

                /* Enqueue Payload into Storage */
                status = ch->storage.enqueue(ch->payload_store_handle, pay, sizeof(bp_payload_store_t), &buffer[index], pay->payloadsize, timeout);
                if(status > 0)
                {
                    /* Acknowledge Custody */
                    if(pay->request_custody)
                    {
                        bplib_os_lock(ch->dacs_bundle_lock);
                        {
                            update_dacs_bundle(ch, &cteb_blk, true, BP_CHECK, procflags);
                        }
                        bplib_os_unlock(ch->dacs_bundle_lock);
                    }
                }
                else
                {
                    bplog(BP_FAILEDSTORE, "Failed (%d) to store payload\n", status);
                }
            }
            else // wrong channel
            {
                status = bplog(BP_WRONGCHANNEL, "Wrong channel to service bundle (%lu, %lu)\n", (unsigned long)pri_blk.dstserv.value, (unsigned long)ch->local_service);
            }

            /* Stop Processing Bundle Once Payload Block Reached */
            break;
        }
    }

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_accept -
 *
 *  Returns number of bytes of payload copied (positive), or error code (zero, negative)
 *-------------------------------------------------------------------------------------*/
int bplib_accept(int channel, void* payload, int size, int timeout, uint16_t* acptflags)
{
    (void)acptflags;
    
    int status;

    /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(payload == NULL)                        return BP_PARMERR;

    /* Set Shortcuts */
    bp_channel_t*           ch          = &channels[channel];
    bp_store_dequeue_t      dequeue     = ch->storage.dequeue;
    bp_store_relinquish_t   relinquish  = ch->storage.relinquish;
    uint8_t*                storebuf    = NULL;
    int                     storelen    = 0;
    bp_sid_t                sid         = BP_SID_VACANT;

    /* Dequeue Payload from Storage */
    status = dequeue(ch->payload_store_handle, (void**)&storebuf, &storelen, &sid, timeout);
    if(status == BP_SUCCESS)
    {
        /* Access Payload */
        uint8_t*            payptr      = &storebuf[sizeof(bp_payload_store_t)];
        bp_payload_store_t* paystore    = (bp_payload_store_t*)storebuf;
        int                 paylen      = paystore->payloadsize;

        /* Return Payload to Application */
        if(size >= paylen)
        {
            /* Copy Payload and Set Status */
            bplib_os_memcpy(payload, payptr, paylen);
            ch->stats.delivered++;
            status = paylen;
        }
        else
        {
            status = bplog(BP_PAYLOADTOOLARGE, "Payload too large to fit inside buffer (%d %d)\n", size, paylen);
            ch->stats.lost++;
        }

        /* Relinquish Memory */
        relinquish(ch->payload_store_handle, sid);
    }

    /* Return Status or Payload Size */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_routeinfo -
 *
 *  bundle -                pointer to a bundle (byte array) [INPUT]
 *  size -                  size of bundle being pointed to [INPUT]
 *  destination_node -      read from bundle [OUTPUT]
 *  destination_service -   as read from bundle [OUTPUT]
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_routeinfo(void* bundle, int size, bp_ipn_t* destination_node, bp_ipn_t* destination_service)
{
    int status;
    bp_blk_pri_t pri_blk;
    uint8_t* buffer = (uint8_t*)bundle;

    /* Check Parameters */
    assert(buffer);

    /* Parse Primary Block */
    status = bplib_blk_pri_read(buffer, size, &pri_blk, true);
    if(status <= 0) return status;

    /* Set Addresses */
    if(destination_node)    *destination_node = (bp_ipn_t)pri_blk.dstnode.value;
    if(destination_service) *destination_service = (bp_ipn_t)pri_blk.dstserv.value;

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_eid2ipn -
 *
 *  eid -                   null-terminated string representation of End Point ID [INPUT]
 *  len -                   size in bytes of above string including null termination [INPUT]
 *  node -                  node number as read from eid [OUTPUT]
 *  service -               service number as read from eid [OUTPUT]
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_eid2ipn(const char* eid, int len, bp_ipn_t* node, bp_ipn_t* service)
{
    char eidtmp[BP_MAX_EID_STRING];
    char* node_ptr;
    char* service_ptr;
    char* endptr;
    unsigned long node_result;
    unsigned long service_result;

    /* Sanity Check EID Pointer */
    if(eid == NULL)
    {
        return bplog(BP_INVALIDEID, "EID is null\n");
    }

    /* Sanity Check Length of EID */
    if(len < 8)
    {
        return bplog(BP_INVALIDEID, "EID must be at least 7 characters, act: %d\n", len); // 8 if null-termination is included
    }
    else if(len > BP_MAX_EID_STRING)
    {
        return bplog(BP_INVALIDEID, "EID cannot exceed %d bytes in length, act: %d\n", BP_MAX_EID_STRING, len);
    }

    /* Check IPN Scheme */
    if(eid[0] != 'i' || eid[1] != 'p' || eid[2] != 'n' || eid[3] != ':')
    {
        return bplog(BP_INVALIDEID, "EID (%s) must start with 'ipn:'\n", eid);
    }

    /* Copy EID to Temporary Buffer and Set Pointers */
    strncpy(eidtmp, &eid[4], len - 4);
    node_ptr = eidtmp;
    service_ptr = strchr(node_ptr, '.');
    if(service_ptr != NULL)
    {
        *service_ptr = '\0';
        service_ptr++;
    }
    else
    {
        return bplog(BP_INVALIDEID, "Unable to find dotted notation in EID (%s)\n", eid);
    }

    /* Parse Node Number */
    errno = 0;
    node_result = strtoul(node_ptr, &endptr, 10); // assume IPN node and service numbers always written in base 10
    if( (endptr == node_ptr) ||
        ((node_result == ULONG_MAX || node_result == 0) && errno == ERANGE) )
    {
        return bplog(BP_INVALIDEID, "Unable to parse EID (%s) node number\n", eid);
    }

    /* Parse Service Number */
    errno = 0;
    service_result = strtoul(service_ptr, &endptr, 10); // assume IPN node and service numbers always written in base 10
    if( (endptr == service_ptr) ||
        ((service_result == ULONG_MAX || service_result == 0) && errno == ERANGE) )
    {
        return bplog(BP_INVALIDEID, "Unable to parse EID (%s) service number\n", eid);
    }

    /* Set Outputs */
    *node = (uint32_t)node_result;
    *service = (uint32_t)service_result;
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_ipn2eid -
 *
 *  eid -                   buffer that will hold null-terminated string representation of End Point ID [OUTPUT]
 *  len -                   size in bytes of above buffer [INPUT]
 *  node -                  node number to be written into eid [INPUT]
 *  service -               service number to be written into eid [INPUT]
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_ipn2eid(char* eid, int len, bp_ipn_t node, bp_ipn_t service)
{
    /* Sanity Check EID Buffer Pointer */
    if(eid == NULL)
    {
        return bplog(BP_INVALIDEID, "EID buffer is null\n");
    }

    /* Sanity Check Length of EID Buffer */
    if(len < 8)
    {
        return bplog(BP_INVALIDEID, "EID buffer must be at least 7 characters, act: %d\n", len); // 8 if null-termination is included
    }
    else if(len > BP_MAX_EID_STRING)
    {
        return bplog(BP_INVALIDEID, "EID buffer cannot exceed %d bytes in length, act: %d\n", BP_MAX_EID_STRING, len);
    }

    /* Write EID */
    snprintf(eid, len, "ipn:%lu.%lu", (unsigned long)node, (unsigned long)service);

    return BP_SUCCESS;
}
