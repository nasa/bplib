/************************************************************************
 * File: bundle.c
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
#include "bundle.h"
#include "blocks.h"
#include "sdnv.h"
#include "bplib_os.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_NUM_EXCLUDE_REGIONS          8

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
    return blocks_build(bundle, NULL, NULL, NULL, 0, flags);
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
        blocks_build(bundle, NULL, NULL, NULL, 0, flags);
    }
    
    /* Update Payload */
    blocks->payload_block.payptr = (uint8_t*)pay;
    blocks->payload_block.paysize = pay_size;

    /* Store Bundle */
    status = blocks_enqueue(bundle, true, timeout, flags);
    
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
                status = blocks_build(bundle, &pri_blk, &pay_blk, hdr_buf, hdr_index, flags);
                if(status == BP_SUCCESS)
                {
                    /* Store Forwarded Bundle */
                    status = blocks_enqueue(bundle, false, timeout, flags);
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
