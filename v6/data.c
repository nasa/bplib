/************************************************************************
 * File: bplib_blk_pay.c
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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "block.h"
#include "pri.h"
#include "bib.h"
#include "pay.h"
#include "sdnv.h"
#include "bplib_os.h"
#include "data.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/


/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Data Bundle */
typedef struct {
    bp_blk_pri_t        primary_block;
    bp_blk_cteb_t       custody_block;
    bp_blk_bib_t        integrity_block;
    bp_blk_pay_t        payload_block;
    int                 maxlength;  /* maximum size of bundle in bytes (includes header blocks) */
    int                 originate;  /* 1: originated bundle, 0: forwarded bundle */
    bp_bundle_store_t   bundle_store;
    bp_payload_store_t  payload_store;
} data_bundle_t;

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
    .integrity_check    = BP_DEFAULT_ICHECK
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
                                /* Value                         Index  Width */
    .block_flags              = {  0,                            1,     1     },
    .block_length             = {  0,                            2,     4     },
    .security_target_count    = {  1,                            6,     1     },
    .security_target_type     = {  1,                            7,     1     },
    .security_target_sequence = {  0,                            8,     1     },
    .cipher_suite_id          = {  BP_DEFAULT_CIPHER_SUITE,      9,     1     },
    .cipher_suite_flags       = {  0,                            10,    1     },
    .security_result_count    = {  1,                            11,    1     },
    .security_result_type     =    0,
    .security_result_length   = {  1,                            13,    1     },
};

static const bp_blk_pay_t native_pay_blk = {
    .bf                       = { 0,                              1,    1     },
    .blklen                   = { 0,                              2,    4     },
    .payptr                   = NULL,
    .paysize                  = 0
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
static int initialize_orig_bundle(data_bundle_t* bundle)
{
    bp_bundle_store_t*  ds      = &bundle->bundle_store;
    uint8_t*            hdrbuf  = ds->header;
    int                 offset  = 0;

    /* Initialize Storage */
    memset(ds, 0, sizeof(bp_bundle_store_t));
    ds->cidsdnv = native_cteb_blk.cid;

    /* Write Primary Block */
    offset = pri_write(&hdrbuf[0], BP_BUNDLE_HDR_BUF_SIZE, &bundle->primary_block, false);

    /* Write Custody Block */
    if(bundle->primary_block.request_custody)
    {
        ds->cteboffset = offset;
        offset = cteb_write(&hdrbuf[offset], BP_BUNDLE_HDR_BUF_SIZE - offset, &bundle->custody_block, false) + offset;
    }
    else
    {
        ds->cteboffset = 0;
    }

    /* Write Integrity Block */
    if(bundle->primary_block.integrity_check)
    {
        ds->biboffset = offset;
        offset = bib_write(&hdrbuf[offset],  BP_BUNDLE_HDR_BUF_SIZE - offset, &bundle->integrity_block, false) + offset;
    }
    else
    {
        ds->biboffset = 0;
    }

    /* Write Payload Block */
    ds->payoffset = offset;
    ds->headersize = pay_write (&hdrbuf[offset], BP_BUNDLE_HDR_BUF_SIZE - offset, &bundle->payload_block, false) + offset;

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * data_new -
 *
 *  This initializes a new bundle
 *-------------------------------------------------------------------------------------*/
static int data_new(data_bundle_t* bundle, bp_blk_pri_t* pri, bp_blk_pay_t* pay, uint8_t* hdr_buf, int hdr_len, uint16_t* procflags)
{
    int i, status;
    int hdr_index;

    bp_bundle_store_t*  ds      = &bundle->bundle_store;
    bp_blk_cteb_t*      fcteb   = &bundle->custody_block;
    bp_blk_bib_t*       fbib    = &bundle->integrity_block;

    /* Initialize Data Storage Memory */
    hdr_index = 0;
    memset(ds, 0, sizeof(bp_bundle_store_t));

    /* Initialize Primary Block */
    if(pri) bundle->primary_block = *pri;

    /* Write Primary Block */
    status = pri_write(ds->header, BP_BUNDLE_HDR_BUF_SIZE, &bundle->primary_block, false);
    if(status <= 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%d) to write primary block of bundle\n", status);
    hdr_index += status;

    /* Write Custody Block */
    if(bundle->primary_block.request_custody)
    {
        ds->cidsdnv = fcteb->cid;
        ds->cteboffset = hdr_index;
        status = cteb_write(&ds->header[hdr_index], BP_BUNDLE_HDR_BUF_SIZE - hdr_index, fcteb, false);
        if(status <= 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%d) to write custody block of bundle\n", status);
        hdr_index += status;
    }
    else
    {
        ds->cteboffset = 0;
    }

    /* Write Integrity Block */
    if(bundle->primary_block.integrity_check)
    {
        ds->biboffset = hdr_index;
        status = bib_write(&ds->header[hdr_index], BP_BUNDLE_HDR_BUF_SIZE - hdr_index, fbib, false);
        if(status <= 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%d) to write integrity block of bundle\n", status);
        hdr_index += status;
    }
    else
    {
        ds->biboffset = 0;
    }

    /* Copy Non-excluded Header Regions */
    if(hdr_index + hdr_len < BP_BUNDLE_HDR_BUF_SIZE)
    {
        memcpy(&ds->header[hdr_index], hdr_buf, hdr_len);
        hdr_index += hdr_len;
    }
    else
    {
        return bplog(BP_BUNDLETOOLARGE, "Non-excluded forwarded blocks exceed maximum header size (%d)\n", hdr_index);
    }
        
    /* Initialize Payload Block */
    bundle->payload_block = *pay;

    /* Initialize Payload Block Offset */
    ds->payoffset = hdr_index;

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * data_store -
 *-------------------------------------------------------------------------------------*/
static int data_store(data_bundle_t* bundle, int timeout bp_store_enqueue_t enqueue, int handle, uint16_t* storflags)
{
    int                 status          = 0;
    int                 payload_offset  = 0;
    bp_blk_pri_t*       pri             = &bundle->primary_block;
    bp_blk_pay_t*       pay             = &bundle->payload_block;
    bp_bundle_store_t*  ds              = &bundle->bundle_store;;

    /* Check Fragmentation */
    if(pay->paysize > bundle->maxlength)
    {
        if(bundle->allow_frag)
        {
            pri->is_frag = true;            
        }
        else
        {
            return bplog(BP_BUNDLETOOLARGE, "Unable (%d) to fragment forwarded bundle (%d > %d)\n", BP_UNSUPPORTED, pay_blk.paysize, ch->data.maxlength);
        }
    }

    /* Originator Specific Steps */
    if(bundle->originate)
    {
        /* Set Creation Time */
        pri->createsec.value = bplib_os_systime();
        sdnv_write(ds->header, BP_BUNDLE_HDR_BUF_SIZE, pri->createsec, storflags);

        /* Set Sequence */
        sdnv_write(ds->header, BP_BUNDLE_HDR_BUF_SIZE, pri->createseq, storflags);
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
            sdnv_write(ds->header, BP_BUNDLE_HDR_BUF_SIZE, pri->fragoffset, storflags);
            sdnv_write(ds->header, BP_BUNDLE_HDR_BUF_SIZE, pri->paylen, storflags);
        }

        /* Update Integrity Block */
        if(ds->biboffset != 0)
        {
            bib_update(&ds->header[ds->biboffset], BP_BUNDLE_HDR_BUF_SIZE - ds->biboffset, &pay->payptr[payload_offset], fragment_size, &bundle->integrity_block);
        }
        
        /* Write Payload Block (static portion) */
        pay->blklen.value = fragment_size;
        status = pay_write(&ds->header[ds->payoffset], BP_BUNDLE_HDR_BUF_SIZE - ds->payoffset, pay, false);
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
    return BP_SUCCESS;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * data_forward -
 *-------------------------------------------------------------------------------------*/
int data_forward(bp_data_t* data, bp_blk_pri_t* pri, bp_blk_pay_t* pay, uint8_t* hdr_buf, int hdr_len, int timeout, bp_store_enqueue_t enqueue, int handle, uint16_t* dataflags)
{
    int status;
    data_bundle_t* bundle = data->bundle;
    
    /* Check Ability to Forward */
    if(bundle->originate)
    {
        return bplog(BP_WRONGORIGINATION, "Unable to forward bundle on an originating channel\n");
    }
                
    /* Initialize Forwarded Bundle */
    status = data_new(bundle, pri, pay, hdr_buf, hdr_len, dataflags);
    if(status == BP_SUCCESS)
    {
        /* Store Forwarded Bundle */
        status = data_store(bundle, timeout, enqueue, handle, dataflags);
    }
    
    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * data_send -
 *-------------------------------------------------------------------------------------*/
int data_send(bp_data_t* data, uint8_t* pay_buf, int pay_len, int timeout, bp_store_enqueue_t enqueue, int handle, uint16_t* dataflags)
{
    int status = BP_SUCCESS;
    data_bundle_t* bundle = data->bundle;

    /* Check Ability to Send */
    if(!bundle->originate)
    {
        return bplog(BP_WRONGORIGINATION, "Cannot originate bundle on channel designated for forwarding\n");
    }

    /* Update Payload */
    bundle->payload_block.payptr = (uint8_t*)payload;
    bundle->payload_block.paysize = size;

    /* Store Bundle */
    status = data_store(bundle, timeout, enqueue, handle, dataflags);
    
    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * data_receive -
 *-------------------------------------------------------------------------------------*/
int data_receive(bp_data_t* data, bool request_custody, uint8_t* pay_buf, int pay_len, int timeout, bp_store_enqueue_t enqueue, int handle, uint16_t* dataflags)
{
    int status = BP_SUCCESS;
    data_bundle_t* bundle = data->bundle;

    /* Initialize Payload Store */
    bp_payload_store_t* ps = &bundle->payload_store;
    ps->payloadsize = pay_len;
    ps->request_custody = request_custody;

    /* Enqueue Payload into Storage */
    enstat = enqueue(handle, ps, sizeof(bp_payload_store_t), pay_buf, pay_len, timeout);
    if(enstat <= 0)
    {
        status = bplog(BP_FAILEDSTORE, "Failed (%d) to store payload\n", enstat);
        *dataflags |= BP_FLAG_STOREFAILURE;
    }

    /* Return Status */
    return status;
}
