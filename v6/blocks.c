/************************************************************************
 * File: blocks.c
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
#include "blocks.h"
#include "pri.h"
#include "bib.h"
#include "pay.h"
#include "sdnv.h"
#include "bplib_os.h"
#include "bundle.h"

/******************************************************************************
 FILE DATA
 ******************************************************************************
 * Notes:
 *    The block length field for every blocks block MUST be set to a positive
 *    integer.  The option to update the fields of the blocks reserves the width
 *    of the blklen field and goes back and writes the value after the entire
 *    block is written.  If the blklen field was variable, the code would have
 *    to make a first pass to calculate the block length and then a second pass
 *    to use that block length - that would be too much processing.
 */

static const bp_blk_pri_t bundle_pri_blk = {
    .version            = BP_PRI_VERSION,
                         /* Value   Index   Width */
    .pcf                = { 0,      1,      3   },
    .blklen             = { 0,      4,      1   },
    .dstnode            = { 0,      5,      4   },
    .dstserv            = { 0,      9,      2   },
    .srcnode            = { 0,      11,     4   },
    .srcserv            = { 0,      15,     2   },
    .rptnode            = { 0,      17,     4   },
    .rptserv            = { 0,      21,     2   },
    .cstnode            = { 0,      23,     4   },
    .cstserv            = { 0,      27,     2   },
    .createsec          = { 0,      29,     6   },
    .createseq          = { 0,      35,     4   },
    .lifetime           = { 0,      39,     4   },
    .dictlen            = { 0,      43,     1   },
    .fragoffset         = { 0,      44,     4   },
    .paylen             = { 0,      48,     4   },
    .is_admin_rec       = false,
    .is_frag            = false,
    .allow_frag         = false,
    .cst_rqst           = true
};

static const bp_blk_cteb_t bundle_cteb_blk = {
                         /* Value   Index   Width */
    .bf                 = { 0,      1,      1   },
    .blklen             = { 0,      2,      1   },
    .cid                = { 0,      3,      4   },
    .csteid             = { '\0' },
    .cstnode            = 0,
    .cstserv            = 0
};

static const bp_blk_bib_t bundle_bib_blk = {
                                /* Value    Index  Width */
    .block_flags              = {  0,       1,     1   },
    .block_length             = {  0,       2,     4   },
    .security_target_count    = {  1,       6,     1   },
    .security_target_type     = {  1,       7,     1   },
    .security_target_sequence = {  0,       8,     1   },
    .cipher_suite_id          = {  0,       9,     1   },
    .cipher_suite_flags       = {  0,       10,    1   },
    .security_result_count    = {  1,       11,    1   },
    .security_result_type     =    0,
    .security_result_length   = {  1,       13,    1   },
};

static const bp_blk_pay_t bundle_pay_blk = {
                               /* Value     Index   Width */
    .bf                       = { 0,        1,      1   },
    .blklen                   = { 0,        2,      4   },
    .payptr                   = NULL,
    .paysize                  = 0
};

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * blocks_build -
 *
 *  This initializes a new bundle
 *-------------------------------------------------------------------------------------*/
int blocks_build(bp_bundle_t* bundle, bp_blk_pri_t* pri, bp_blk_pay_t* pay, uint8_t* hdr_buf, int hdr_len, uint16_t* flags)
{
    int status;
    int hdr_index;

    bp_bundle_data_t*       data    = &bundle->data;
    bp_bundle_blocks_t*     blocks  = &bundle->blocks;

    /* Initialize Data Storage Memory */
    hdr_index = 0;
    memset(data, 0, sizeof(bp_bundle_data_t));

    /* Initialize Primary Block */
    if(pri)
    {
        /* User Provided Primary Block */
        blocks->primary_block = *pri;

        /* Set Pre-Built Flag to FALSE */
        bundle->prebuilt = false;
    }
    else
    {
        /* Library Provided Primary Block */
        blocks->primary_block                   = bundle_pri_blk;
        blocks->primary_block.dstnode.value     = bundle->destination_node;
        blocks->primary_block.dstserv.value     = bundle->destination_service;
        blocks->primary_block.srcnode.value     = bundle->local_node;
        blocks->primary_block.srcserv.value     = bundle->local_service;
        blocks->primary_block.rptnode.value     = bundle->report_node;
        blocks->primary_block.rptserv.value     = bundle->report_service;
        blocks->primary_block.cstnode.value     = bundle->local_node;
        blocks->primary_block.cstserv.value     = bundle->local_service;
        blocks->primary_block.lifetime.value    = bundle->attributes->lifetime;
        blocks->primary_block.allow_frag        = bundle->attributes->allow_fragmentation;
        blocks->primary_block.cst_rqst          = bundle->attributes->request_custody;

        /* Set Pre-Built Flag to TRUE */
        bundle->prebuilt = true;
    }

    /* Write Primary Block */
    status = pri_write(data->header, BP_BUNDLE_HDR_BUF_SIZE, &blocks->primary_block, false, flags);
    if(status <= 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%d) to write primary block of bundle\n", status);
    hdr_index += status;

    /* Write Custody Block */
    if(blocks->primary_block.cst_rqst)
    {
        /* Initialize Block */
        blocks->custody_block = bundle_cteb_blk;
        blocks->custody_block.cid.value = 0; /* Set Initial Custody ID to Zero */
        bplib_ipn2eid(blocks->custody_block.csteid, BP_MAX_EID_STRING, bundle->local_node, bundle->local_service); /* Set Custodian EID */

        /* Populate Data with Block */
        data->cidsdnv = blocks->custody_block.cid;
        data->cteboffset = hdr_index;
        status = cteb_write(&data->header[hdr_index], BP_BUNDLE_HDR_BUF_SIZE - hdr_index, &blocks->custody_block, false, flags);

        /* Check Status */
        if(status <= 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%d) to write custody block of bundle\n", status);
        hdr_index += status;
    }
    else
    {
        data->cteboffset = 0;
    }

    /* Write Integrity Block */
    if(bundle->attributes->integrity_check)
    {
        /* Initialize Block */
        blocks->integrity_block = bundle_bib_blk;
        blocks->integrity_block.cipher_suite_id.value = bundle->attributes->cipher_suite;
        /* Populate Data */
        data->biboffset = hdr_index;
        status = bib_write(&data->header[hdr_index], BP_BUNDLE_HDR_BUF_SIZE - hdr_index, &blocks->integrity_block, false, flags);

        /* Check Status */
        if(status <= 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%d) to write integrity block of bundle\n", status);
        hdr_index += status;
    }
    else
    {
        data->biboffset = 0;
    }

    /* Copy Non-excluded Header Regions */
    if(hdr_index + hdr_len < BP_BUNDLE_HDR_BUF_SIZE)
    {
        memcpy(&data->header[hdr_index], hdr_buf, hdr_len);
        hdr_index += hdr_len;
    }
    else
    {
        return bplog(BP_BUNDLETOOLARGE, "Non-excluded forwarded bundle exceed maximum header size (%d)\n", hdr_index);
    }
        
    /* Initialize Payload Block */
    if(pay)
    {
        /* User Provided Payload Block */
        blocks->payload_block = *pay;
    }
    else
    {
        /* Library Provided Payload Block */
        blocks->payload_block = bundle_pay_blk;
    }

    /* Initialize Payload Block Offset */
    data->payoffset = hdr_index;

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * blocks_enqueue -
 *-------------------------------------------------------------------------------------*/
int blocks_enqueue(bp_bundle_t* bundle, bool set_time, int timeout, uint16_t* flags)
{
    int                     status          = 0;
    int                     payload_offset  = 0;
    bp_bundle_data_t*       data            = &bundle->data;
    bp_bundle_blocks_t*     blocks          = &bundle->blocks;
    bp_blk_pri_t*           pri             = &blocks->primary_block;
    bp_blk_bib_t*           bib             = &blocks->integrity_block;
    bp_blk_pay_t*           pay             = &blocks->payload_block;

    /* Check Fragmentation */
    if(pay->paysize > bundle->attributes->maxlength)
    {
        if(bundle->attributes->allow_fragmentation)
        {
            pri->is_frag = true;            
        }
        else
        {
            return bplog(BP_BUNDLETOOLARGE, "Unable (%d) to fragment forwarded bundle (%d > %d)\n", BP_UNSUPPORTED, pay->paysize, bundle->attributes->maxlength);
        }
    }

    /* Check if Time Needs to be Set  */
    if(set_time)
    {
        /* Set Creation Time */
        pri->createsec.value = bplib_os_systime();
        sdnv_write(data->header, BP_BUNDLE_HDR_BUF_SIZE, pri->createsec, flags);

        /* Set Sequence */
        sdnv_write(data->header, BP_BUNDLE_HDR_BUF_SIZE, pri->createseq, flags);
    }

    /* Set Expiration Time of Bundle */
    if(pri->lifetime.value != 0)    data->exprtime = pri->createsec.value + pri->lifetime.value;
    else                            data->exprtime = 0;

    /* Enqueue Bundle */
    while(payload_offset < pay->paysize)
    {
        /* Calculate Storage Header Size and Fragment Size */
        int payload_remaining = pay->paysize - payload_offset;
        int fragment_size = bundle->attributes->maxlength <  payload_remaining ? bundle->attributes->maxlength : payload_remaining;

        /* Update Primary Block Fragmentation */
        if(pri->is_frag)
        {
            pri->fragoffset.value = payload_offset;
            pri->paylen.value = pay->paysize;
            sdnv_write(data->header, BP_BUNDLE_HDR_BUF_SIZE, pri->fragoffset, flags);
            sdnv_write(data->header, BP_BUNDLE_HDR_BUF_SIZE, pri->paylen, flags);
        }

        /* Update Integrity Block */
        if(data->biboffset != 0)
        {
            bib_update(&data->header[data->biboffset], BP_BUNDLE_HDR_BUF_SIZE - data->biboffset, &pay->payptr[payload_offset], fragment_size, bib, flags);
        }
        
        /* Write Payload Block (static portion) */
        pay->blklen.value = fragment_size;
        status = pay_write(&data->header[data->payoffset], BP_BUNDLE_HDR_BUF_SIZE - data->payoffset, pay, false, flags);
        if(status <= 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%d) to write payload block (static portion) of bundle\n", status);
        data->headersize = data->payoffset + status;
        data->bundlesize = data->headersize + fragment_size;

        /* Enqueue Bundle */
        int storage_header_size = sizeof(bp_bundle_data_t) - (BP_BUNDLE_HDR_BUF_SIZE - data->headersize);
        status = bundle->store.enqueue(bundle->handle, data, storage_header_size, &pay->payptr[payload_offset], fragment_size, timeout);
        if(status <= 0) return bplog(status, "Failed (%d) to store bundle in storage system\n", status);
        payload_offset += fragment_size;
    }

    /* Increment Sequence Count (done here since now bundle successfully stored) */
    if(set_time) pri->createseq.value++;

    /* Return Payload Bytes Stored */
    return BP_SUCCESS;
}
