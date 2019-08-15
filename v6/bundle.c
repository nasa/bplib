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
#include "block.h"
#include "pri.h"
#include "bib.h"
#include "pay.h"
#include "sdnv.h"
#include "bplib_os.h"
#include "bundle.h"


/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_NUM_EXCLUDE_REGIONS          8

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
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bundle_build -
 *
 *  This initializes a new bundle
 *-------------------------------------------------------------------------------------*/
static int bundle_build(bp_bundle_t* bundle, bp_blk_pri_t* pri, bp_blk_pay_t* pay, uint8_t* hdr_buf, int hdr_len, uint16_t* flags)
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
 * bundle_enqueue -
 *-------------------------------------------------------------------------------------*/
static int bundle_enqueue(bp_bundle_t* bundle, bool set_time, int timeout, uint16_t* flags)
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

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bundle_initialize -
 *-------------------------------------------------------------------------------------*/
int bundle_initialize(bp_bundle_t* bundle, bp_attr_t* attr, bp_store_t store, bp_ipn_t srcnode, bp_ipn_t srcserv, bp_ipn_t dstnode, bp_ipn_t dstserv, uint16_t* flags)
{
    /* Initialize Bundle Parameters */
    bundle->attributes          = attr;
    bundle->local_node          = srcnode;
    bundle->local_service       = srcserv;
    bundle->destination_node    = dstnode;
    bundle->destination_service = dstserv;
    bundle->report_node         = 0;
    bundle->report_service      = 0;
    bundle->prebuilt            = false;
    
    /* Initialize Bundle Store */
    bundle->store   = store;
    bundle->handle  = bundle->store.create(bundle->attributes->storage_service_parm);
    
    /* Handle Errors */
    if(bundle->handle < 0)
    {
        bundle_uninitialize(bundle);
        return bplog(BP_FAILEDSTORE, "Failed to create storage handle for bundles\n");
    }
    
    /* Initialize New Bundle */
    return bundle_build(bundle, NULL, NULL, NULL, 0, flags);
}

/*--------------------------------------------------------------------------------------
 * bundle_uninitialize -
 *-------------------------------------------------------------------------------------*/
void bundle_uninitialize(bp_bundle_t* bundle)
{
    if(bundle->handle >= 0) bundle->store.destroy(bundle->handle);
}

/*--------------------------------------------------------------------------------------
 * bundle_send -
 *-------------------------------------------------------------------------------------*/
int bundle_send(bp_bundle_t* bundle, uint8_t* pay, int pay_size, int timeout, uint16_t* flags)
{
    int status;
    bp_bundle_blocks_t* blocks = &bundle->blocks;

    /* Check if Re-initialization Needed */
    if(bundle->prebuilt == false)
    {
        bundle_build(bundle, NULL, NULL, NULL, 0, flags);
    }
    
    /* Update Payload */
    blocks->payload_block.payptr = (uint8_t*)pay;
    blocks->payload_block.paysize = pay_size;

    /* Store Bundle */
    status = bundle_enqueue(bundle, true, timeout, flags);
    
    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bundle_receive -
 *-------------------------------------------------------------------------------------*/
int bundle_receive(bp_bundle_t* bundle, uint8_t** block, int* block_size, uint32_t sysnow, int timeout, uint16_t* flags)
{
    int                 status = BP_SUCCESS;

    uint8_t*            buffer = *block;
    int                 size = *block_size;
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
    
    /* Parse Primary Block */
    exclude[ei++] = index;
    status = pri_read(buffer, size, &pri_blk, true, flags);
    if(status <= 0) return bplog(status, "Failed to parse primary block of size %d\n", size);
    else            index += status;
    exclude[ei++] = index;

    /* Check Unsupported */
    if(pri_blk.dictlen.value != 0)
    {
        *flags |= BP_FLAG_NONCOMPLIANT;
        return bplog(BP_UNSUPPORTED, "Unsupported bundle attempted to be processed (%d)\n", pri_blk.dictlen.value);
    }

    /* Check Life Time */
    if((pri_blk.lifetime.value != 0) && (sysnow >= (pri_blk.lifetime.value + pri_blk.createsec.value)))
    {
        return bplog(BP_EXPIRED, "Expired bundled attempted to be processed \n");
    }

    /* Parse and Process Remaining Blocks */
    while(status == BP_SUCCESS && index < size)
    {
        /* Read Block Information */
        uint8_t blk_type = buffer[index];

        /* Check Block Type */
        if(blk_type == BP_BIB_BLK_TYPE)
        {
            /* Mark Start of BIB Region */
            bib_present = true;
            bib_index = index;
            exclude[ei++] = index;
            
            /* Read BIB */
            status = bib_read(&buffer[bib_index], size - bib_index, &bib_blk, true, flags);
            if(status <= 0) return bplog(status, "Failed to parse BIB block at offset %d\n", bib_index);
            else            index += status;

            /* Mark End of BIB Region */
            exclude[ei++] = index;
        }
        else if(blk_type == BP_CTEB_BLK_TYPE)
        {
            /* Mark Start of CTEB Region */
            cteb_present = true;
            cteb_index = index;

            /* Read CTEB */
            status = cteb_read(&buffer[bib_index], size - cteb_index, &cteb_blk, true, flags);
            if(status <= 0) return bplog(status, "Failed to parse CTEB block at offset %d\n", cteb_index);
            else            index += status;
        }
        else if(blk_type != BP_PAY_BLK_TYPE) /* skip over block */
        {
            bp_sdnv_t blk_flags = { 0, 1, 0 };
            bp_sdnv_t blk_len = { 0, 0, 0 };
            int start_index = index;
            int data_index = 0; /* start of the block after the block length, set below */
            
            blk_len.index = sdnv_read(&buffer[start_index], size - start_index, &blk_flags, flags);
            data_index = sdnv_read(&buffer[start_index], size - start_index, &blk_len, flags);

            /* Check Parsing Status */
            if(*flags & (BP_FLAG_SDNVOVERFLOW | BP_FLAG_SDNVINCOMPLETE))
            {
                status = bplog(BP_BUNDLEPARSEERR, "Failed (%0X) to parse block at index %d\n", *flags, start_index);
            }
            else
            {
                index += data_index + blk_len.value;
            }

            /* Mark Processing as Incomplete */            
            *flags |= BP_FLAG_INCOMPLETE;
            bplog(BP_UNSUPPORTED, "Skipping over unrecognized block of type 0x%02X and size %d\n", blk_type, blk_len.value);

            /* Should transmit status report that block cannot be processed */
            if(blk_flags.value & BP_BLK_NOTIFYNOPROC_MASK) *flags |= BP_FLAG_NONCOMPLIANT;

            /* Delete bundle since block not recognized */
            if(blk_flags.value & BP_BLK_DELETENOPROC_MASK)
            {
                status = bplog(BP_DROPPED, "Dropping bundle with unrecognized block\n");
            }
    
            /* Check if Block Should be Included */
            if(blk_flags.value & BP_BLK_DROPNOPROC_MASK)
            {
                /* Exclude Block */
                exclude[ei++] = start_index;
                exclude[ei++] = index;
            }
            else
            {
                /* Mark As Forwarded without Processed */
                blk_flags.value |= BP_BLK_FORWARDNOPROC_MASK;
                sdnv_write(&buffer[start_index], size - start_index, blk_flags, flags);
            }
        }
        else /* payload block */
        {
            pay_index = index;
            exclude[ei++] = index; /* start of payload header */
            status = pay_read(&buffer[pay_index], size - pay_index, &pay_blk, true, flags);
            if(status <= 0) return bplog(status, "Failed (%d) to read payload block\n", status);
            else            index += status;
            exclude[ei++] = index + pay_blk.paysize;

            /* Perform Integrity Check */
            if(bib_present)
            {
                status = bib_verify(pay_blk.payptr, pay_blk.paysize, &bib_blk, flags);
                if(status <= 0) return bplog(status, "Bundle failed integrity check\n");
            }

            /* Check Size of Payload */
            if(pri_blk.is_admin_rec && pay_blk.paysize < 2)
            {
                return bplog(BP_BUNDLEPARSEERR, "Invalid block length: %d\n", pay_blk.paysize);
            }

            /* Process Payload */
            if(pri_blk.dstnode.value != bundle->local_node) /* forward bundle (dst node != local node) */
            {
                /* Handle Custody Request */
                if(pri_blk.cst_rqst)
                {
                    pri_blk.rptnode.value = 0;
                    pri_blk.rptserv.value = 0;
                    pri_blk.cstnode.value = bundle->local_node;
                    pri_blk.cstserv.value = bundle->local_service;                        
                }

                /* Copy Non-excluded Header Regions */
                uint8_t hdr_buf[BP_BUNDLE_HDR_BUF_SIZE];
                int hdr_index = 0;
                int i;
                for(i = 1; (i + 1) < ei; i+=2)
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
                        memcpy(&hdr_buf[hdr_index], &buffer[start_index], bytes_to_copy);
                        hdr_index += bytes_to_copy;
                    }
                }

                /* Initialize Forwarded Bundle */
                status = bundle_build(bundle, &pri_blk, &pay_blk, hdr_buf, hdr_index, flags);
                if(status == BP_SUCCESS)
                {
                    /* Store Forwarded Bundle */
                    status = bundle_enqueue(bundle, false, timeout, flags);
                }

                /* Handle Custody Transfer */
                if(status == BP_SUCCESS && pri_blk.cst_rqst)
                {
                    if(!cteb_present)
                    {
                        *flags |= BP_FLAG_NONCOMPLIANT;
                        status = bplog(BP_UNSUPPORTED, "Only aggregate custody supported\n");
                    }
                    else
                    {
                        *block = NULL;
                        *block_size = 0;
                        status = BP_PENDINGBUNDLECUSTODY;
                    }
                }
            }
            else if((bundle->local_service != 0) && (pri_blk.dstserv.value != bundle->local_service))
            {
                return bplog(BP_WRONGCHANNEL, "Wrong channel to store bundle (%lu, %lu)\n", (unsigned long)pri_blk.dstserv.value, (unsigned long)bundle->local_service);
            }
            else if(pri_blk.is_admin_rec) /* Administrative Record */
            {
                /* Read Record Information */
                uint32_t rec_type = buffer[index++];

                /* Process Record */
                if(rec_type == BP_ACS_REC_TYPE)
                {
                    /* Return Aggregate Custody Signal for Custody Processing */
                    *block = &buffer[index];
                    *block_size = size - index;
                    status = BP_PENDINGACKNOWLEDGMENT;
                }
                else if(rec_type == BP_CS_REC_TYPE)     status = bplog(BP_UNSUPPORTED, "Custody signal bundles are not supported\n");
                else if(rec_type == BP_STAT_REC_TYPE)   status = bplog(BP_UNSUPPORTED, "Status report bundles are not supported\n");
                else                                    status = bplog(BP_UNKNOWNREC, "Unknown administrative record: %u\n", (unsigned int)rec_type);
            }
            else 
            {
                /* Return Payload for Payload Processing */
                *block = &buffer[index];
                *block_size = size - index;
                status = BP_PENDINGPAYLOAD;
                
                /* Handle Custody Transfer */
                if(pri_blk.cst_rqst)
                {
                    if(cteb_present)
                    {
                        status = BP_PENDINGPAYLOADCUSTODY;
                    }
                    else
                    {
                        *flags |= BP_FLAG_NONCOMPLIANT;
                        bplog(BP_UNSUPPORTED, "Bundle requesting custody, but only aggregate custody supported\n");
                    }
                }
            }
            
            /* Force Exit After Payload Block */
            break;
        }
    }
    
    /* Return Status */
    return status;
}
