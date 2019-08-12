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
#include "dacs.h"
#include "bundle.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_ACS_REC_TYPE_INDEX       0
#define BP_ACS_REC_STATUS_INDEX     1
#define BP_ACS_ACK_MASK             0x80    /* if set, then custody successfully transfered */

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * dacs_insert - Attempts to insert a value into the dacs tree and sets 
 *
 * value: The value to attempt to insert into the dacs tree. [INPUT]
 * dacs: A ptr to the dacs to try to insert a value into. [OUTPUT]
 * flags: A ptr to a uint16_t used to set and store flags pertaining to the creation
 *      of the current dacs. [OUTPUT]
 * returns: True if value was inserted, or false indicating dacs should be stored and
 *      new dacs started
 *-------------------------------------------------------------------------------------*/
static bool dacs_insert(bp_dacs_t* dacs, uint32_t value, uint16_t* flags)
{
    rb_tree_status_t status = rb_tree_insert(value, &dacs->tree);
    if (status == RB_FAIL_TREE_FULL) 
    {
        /* We failed the insert because the tree is full therefore we need to
         * store this dacs and start a new one. */
        *flags |= BP_FLAG_RBTREEFULL;
        return false;
    }
    else if (status == RB_FAIL_INSERT_DUPLICATE)
    {
        /* Duplicate values are fine and are treated as a success */
        *flags |= BP_FLAG_DUPLICATES;
        return true;
    }
    else
    {
        /* Insertion successful */
        return true;
    }
}

/*--------------------------------------------------------------------------------------
 * dacs_write -
 *
 *  rec - buffer containing the ACS record [OUTPUT]
 *  size - size of buffer [INPUT]
 *  max_fills_per_dacs - the maximum number of allowable fills for each dacs
 *  tree - a rb_tree ptr containing the cid ranges for the bundle. The tree nodes will 
 *      be deleted as they are written to the dacs. [OUTPUT]
 *  iter - a ptr to a ptr the next rb_node in the tree to extract the fill information
 *      and then delete. [OUTPUT]
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
static int dacs_write(uint8_t* rec, int size, int max_fills_per_dacs, rb_tree_t* tree, rb_node_t** iter)
{
    bp_sdnv_t cid = { 0, 2, 4 };
    bp_sdnv_t fill = { 0, 0, 2 };
    uint16_t flags = 0;

    /* Write Record Information */
    rec[BP_ACS_REC_TYPE_INDEX] = BP_ACS_REC_TYPE; /* record type */
    rec[BP_ACS_REC_STATUS_INDEX] = BP_ACS_ACK_MASK;

    /* Write First CID and Fills */
    int count_fills = 0; /* The number of fills that have occured so far. */

    /* Store the previous and next range fills. */
    rb_range_t range;
    rb_range_t prev_range;

    /* Get the first available range from the rb tree and fill it. */
    rb_tree_get_next_rb_node(tree, iter, &range, true, false);
    cid.value = range.value;
    fill.index = sdnv_write(rec, size, cid, &flags);
    fill.value = range.offset + 1;
    fill.index = sdnv_write(rec, size, fill, &flags);    
    count_fills += 2;

    /* Traverse tree in order and write out fills to dacs. */
    while (count_fills < max_fills_per_dacs && *iter != NULL)
    {
        prev_range = range;
        rb_tree_get_next_rb_node(tree, iter, &range, true, false);        

        /* Write range of missing cid.
           Calculate the missing values between the current and previous node. */
        fill.value = range.value - (prev_range.value + prev_range.offset + 1);
        fill.index = sdnv_write(rec, size, fill, &flags);

        /* Write range of received cids. */
        fill.value = range.offset + 1;
        fill.index = sdnv_write(rec, size, fill, &flags);    
        count_fills += 2;        
    }

    /* Success Oriented Error Checking */
    if(flags != 0)
    {
        return bplog(BP_BUNDLEPARSEERR,
                     "Flags raised during processing of ACS (%08X)\n", flags); 
    } 

    /* Return Block Size */
    return fill.index;
}

/*--------------------------------------------------------------------------------------
 * dacs_store -
 *
 *  Notes:
 *-------------------------------------------------------------------------------------*/
static int dacs_store(bp_dacs_t* bundle, uint32_t sysnow, int timeout, int max_fills_per_dacs, bp_store_enqueue_t enqueue, int store_handle, uint16_t* flags)
{
    int dacs_size, enstat, enstat_fail, storage_header_size;
    bool has_enqueue_failure = false;

    /* If the tree has nodes, initialize the iterator for traversing the tree in order */
    rb_node_t* iter;
    rb_tree_get_first_rb_node(&bundle->tree, &iter);

    /* Continue to delete nodes from the tree and write them to DACS until the tree is empty */
    while (!rb_tree_is_empty(&bundle->tree))
    {
        bp_data_store_t* store = &bundle->dacs_store;
        bp_bundle_data_t* data = (bp_bundle_data_t*)store->data;
        bp_blk_pri_t* pri = &bundle->primary_block;

        /* Build DACS - will remove nodes from the tree */
        dacs_size = dacs_write(bundle->recbuf, bundle->recbuf_size, max_fills_per_dacs, &bundle->tree, &iter);
        data->bundlesize = data->headersize + dacs_size;
        storage_header_size = sizeof(bp_data_store_t) - (BP_BUNDLE_HDR_BUF_SIZE - data->headersize);

        /* Set Creation Time */
        pri->createsec.value = sysnow;
        sdnv_write(data->header, BP_BUNDLE_HDR_BUF_SIZE, pri->createsec, flags);

        /* Set Sequence */
        sdnv_write(data->header, BP_BUNDLE_HDR_BUF_SIZE, pri->createseq, flags);
        pri->createseq.value++;

        /* Update Bundle Integrity Block */
        if(data->biboffset != 0)
        {
            bib_update(&data->header[data->biboffset], BP_BUNDLE_HDR_BUF_SIZE - data->biboffset, bundle->recbuf, dacs_size, &bundle->integrity_block);
        }
        
        /* Update Payload Block */
        bundle->payload_block.payptr = bundle->recbuf;
        bundle->payload_block.paysize = dacs_size;
        bundle->payload_block.blklen.value = dacs_size;
        sdnv_write(&data->header[data->payoffset], BP_BUNDLE_HDR_BUF_SIZE - data->payoffset, bundle->payload_block.blklen, flags);

        /* Send (enqueue) DACS */
        enstat = enqueue(store_handle, data, storage_header_size, bundle->recbuf, dacs_size, timeout);
        bundle->last_dacs = sysnow;
        
        /* Check Storage Status */
        if(enstat <= 0)
        {
            /* Failure enqueing DACS */
            if(!has_enqueue_failure)
            {
                /* Save first failed DACS enqueue to return later */
                enstat_fail = enstat;
                bplog(enstat, "Failed (%d) to store DACS for transmission, bundle dropped\n", enstat);
            }
    
            has_enqueue_failure = true;
        }
        else 
        {
            /* DACS successfully enqueued */
            bundle->last_dacs = sysnow;
        }
    }

    /* Check if one or more of the DACS to be enqueued failed */
    if(has_enqueue_failure)
    {
        *flags |= BP_FLAG_STOREFAILURE;        
        return enstat_fail;
    }

    /* All DACS were successfully enqueued and the CID tree is now empty */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * dacs_new - Populate ACS Bundle Header from Channel Struct
 *
 *  Does not populate following (see update_dacs_header):
 *      - creation time (when using system time)
 *      - creation sequence
 *      - total payload length
 *      - payload crc
 *      - payload block length
 *-------------------------------------------------------------------------------------*/
static int dacs_new(bp_dacs_t* bundle, uint32_t dstnode, uint32_t dstserv, uint16_t* flags)
{
    bp_data_store_t*  data      = &bundle->dacs_store;
    uint8_t*            hdrbuf  = data->header;
    uint16_t            flags   = 0;
    int                 offset  = 0;

    /* Initialize Storage */
    memset(data, 0, sizeof(bp_data_store_t));

    /* Write Primary Block */
    offset = pri_write(&hdrbuf[0], BP_BUNDLE_HDR_BUF_SIZE, &bundle->primary_block, false, flags);

    /* Write Integrity Block */
    if(bundle->blocks.primary_block.integrity_check)
    {
        data->biboffset = offset;
        offset = bib_write(&hdrbuf[offset], BP_BUNDLE_HDR_BUF_SIZE - offset, &bundle->integrity_block, false, flags) + offset;
    }
    else
    {
        data->biboffset = 0;
    }
    
    /* Write Payload Block */
    data->payoffset = offset;
    data->headersize = pay_write(&hdrbuf[offset], BP_BUNDLE_HDR_BUF_SIZE - offset, &bundle->payload_block, false, flags) + offset;

    /* Set Destination EID */
    bundle->primary_block.dstnode.value = dstnode;
    bundle->primary_block.dstserv.value = dstserv;
    sdnv_write(hdrbuf, BP_BUNDLE_HDR_BUF_SIZE, bundle->primary_block.dstnode, &flags);
    sdnv_write(hdrbuf, BP_BUNDLE_HDR_BUF_SIZE, bundle->primary_block.dstserv, &flags);

    /* Set Custody EID */
    bundle->cstnode = dstnode;
    bundle->cstserv = dstserv;
    
    /* Return Status */
    *flags |= flags;
    if(flags != 0)  return BP_BUNDLEPARSEERR;
    else            return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * dacs_update -
 *
 *  Notes:  may or may not perform enqueue depending if DACS needs to be sent
 *          based on whether or not dacs record is full
 *-------------------------------------------------------------------------------------*/
static int dacs_update(bp_dacs_t* bundle, uint32_t cid, uint32_t sysnow, int timeout, int max_fills_per_dacs, bp_store_enqueue_t enqueue, int store_handle, uint16_t* flags)
{
    /* Attempt to insert cid into bundle. */
    bool insert_status = dacs_insert(bundle, cid, flags);
    if(!insert_status)
    {
        /* Store DACS */
        dacs_store(bundle, sysnow, timeout, max_fills_per_dacs, enqueue, store_handle, flags);

        /* Start New DTN ACS */
        insert_status = dacs_insert(bundle, cid, flags);
        if(!insert_status)
        {
            return BP_FAILEDRESPONSE;
        }
    }

    /* Return Success */
    return BP_SUCCESS;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * dacs_initialize - Allocates resources for DACS and initializes control structures
 *-------------------------------------------------------------------------------------*/
int dacs_initialize(bp_custody_t* custody, bp_ipn_t srcnode, bp_ipn_t srcserv, bp_ipn_t dstnode, bp_ipn_t dstserv, bp_store_t* store, bp_attr_t* attr, uint16_t* flags)
{
    int j, status;

    /* Set Number Attributes */
    custody->max_dacs  = attr->max_concurrent_dacs;
    custody->num_dacs  = 0;

    
    /* Allocate Memory for DACS Bundle */
    custody->dacs = malloc(sizeof(bp_dacs_t) * custody->max_dacs);
    if(custody->dacs == NULL)
    {
        dacs_uninitialize(custody);
        return bplog(BP_FAILEDMEM, "Failed to allocate memory for channel dacs\n");                    
    }
    else
    {
        memset(custody->dacs, 0, sizeof(bp_dacs_t) * custody->max_dacs);
    }

    /* Initialize DACS Array */
    for(j = 0; j < custody->max_dacs; j++)
    {
        /* Initialize DACS Channel */
        bp_dacs_t* dacs = (bp_dacs_t*)custody->dacs;
        dacs->max_gaps  = attr->max_gaps_per_dacs;
        dacs->max_fills = attr->max_fills_per_dacs;
        
        /* Allocate Memory for Channel DACS Bundle Fills */
        dacs[j].recbuf_size = sizeof(uint16_t) * custody->dacs->max_fills + 32; // 2 bytes per fill plus payload block header
        dacs[j].recbuf = (uint8_t*)malloc(dacs[j].recbuf_size); 
        if(dacs[j].recbuf == NULL)
        {
            dacs_uninitialize(custody);
            return bplog(BP_FAILEDMEM, "Failed to allocate memory for channel dacs fills and payload\n");                    
        }
        else
        {
            memset(dacs[j].recbuf, 0, dacs[j].recbuf_size);
        }

        /* Allocate Memory for DACS Channel Tree to Store Bundle IDs */
        status = rb_tree_create(dacs[j].max_gaps, &dacs[j].tree);
        if(status != RB_SUCCESS)
        {
            dacs_uninitialize(custody);
            return bplog(BP_FAILEDMEM, "Failed to allocate memory for channel dacs tree\n");
        }

        /* Initialize DACS Channel Bundle */
        status = bundle_initialize(&dacs[j].bundle, srcnode, srcserv, dstnode, dstserv, store, attr, flags);
        if(status != BP_SUCCESS)
        {
            dacs_uninitialize(custody);
            return status;
        }
    }
    
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * dacs_uninitialize - Frees resources for DACS
 *-------------------------------------------------------------------------------------*/
void dacs_uninitialize(bp_custody_t* custody)
{
    int j;
    
    if(dacs)
    {
        bp_dacs_t* dacs = (bp_dacs_t*)dacs->acks_list;

        for(j = 0; j < dacs->max_acks; j++)
        {
            if(dacs[j].recbuf) free(dacs[j].recbuf);
            if(dacs[j].tree.max_size > 0) 
            {
                rb_tree_destroy(&dacs[j].tree);
            }
        }

        free(dacs);
    }    
}

/*--------------------------------------------------------------------------------------
 * dacs_acknowledge -
 *-------------------------------------------------------------------------------------*/
int dacs_acknowledge(bp_dacs_t* dacs, bp_blk_cteb_t* cteb, uint32_t sysnow, int timeout, bp_store_enqueue_t enqueue, int store_handle, uint16_t* flags)
{
    int j;
    int status = BP_SUCCESS;

    bp_dacs_t* dacs = (bp_dacs_t*)dacs->acks_list;

    bplib_os_lock(ch->dacs_bundle_lock);
    {
        /* Find DACS Entry */                   
        bp_dacs_t* bundle = NULL;                    
        for(j = 0; j < dacs->num_acks; j++)
        {
            if(dacs[j].cstnode == cteb->cstnode && dacs[j].cstserv == cteb->cstserv)
            {
                bundle = &dacs[j];
                break;
            }
        }

        /* Handle Entry Not Found */
        if(bundle == NULL)
        {
            if(dacs->num_acks < dacs->max_acks)
            {
                /* Initial DACS Bundle */
                bundle = &dacs[dacs->num_acks++];
                dacs_new(bundle, cteb->cstnode, cteb->cstserv, flags);
            }
            else
            {
                /* No Room in Table for Another Source */
                *flags |= BP_FLAG_TOOMANYSOURCES;
                status = bplog(BP_FAILEDRESPONSE, "No room in DACS table for another source %d.%d\n", cteb->cstnode, cteb->cstserv);
            }
        }

        /* Update DACS */
        if(bundle != NULL)
        {
            status = dacs_update(bundle, cteb->cid.value, sysnow, timeout, dacs->max_fills, enqueue, store_handle, flags);
        }
    }
    bplib_os_unlock(ch->dacs_bundle_lock);            
    
    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * dacs_check -
 *
 *  Notes:  may or may not perform enqueue depending if DACS needs to be sent
 *          based on whether or not it has been too long a time without sending a DACS
 *-------------------------------------------------------------------------------------*/
int dacs_check(bp_dacs_t* dacs, uint32_t period, uint32_t sysnow, int timeout, bp_store_enqueue_t enqueue, int store_handle, uint16_t* flags)
{   
    int j;
    int ret_status = BP_SUCCESS;

    if(period > 0)
    {
        bp_dacs_t* dacs = (bp_dacs_t*)dacs->acks_list;
        for(j = 0; j < dacs->num_acks; j++)
        {
            if( (sysnow >= (dacs[j].last_dacs + period)) && 
                !rb_tree_is_empty(&dacs[j].tree) )
            {
                int status = dacs_store(&dacs[j], sysnow, timeout, dacs->max_fills, enqueue, store_handle, flags);
                if(status != BP_SUCCESS && ret_status == BP_SUCCESS)
                {
                    ret_status = status;
                }
            }
        }
    }

    return ret_status;
}

/*--------------------------------------------------------------------------------------
 * dacs_process -
 *
 *  rec - pointer to ACS administrative record [INPUT]
 *  size - size of record [INPUT]
 *  acks - number of bundles acknowledged
 *
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int dacs_process(void* rec, int size, int* acks, bp_sid_t* sids, int table_size, bp_store_relinquish_t relinquish, int store_handle, uint16_t* flags)
{
    uint32_t i;
    bp_sdnv_t cid = { 0, 2, 0 };
    bp_sdnv_t fill = { 0, 0, 0 };
    uint8_t* buf = (uint8_t*)rec;
    int cidin = true;
    uint16_t flags = 0;
    uint8_t acs_status = buf[BP_ACS_REC_STATUS_INDEX];
    bool ack_success = (acs_status & BP_ACS_ACK_MASK) == BP_ACS_ACK_MASK;
    
    /* Initialize Acknowledgment Count */
    *acks = 0;

    /* Read First Custody ID */
    fill.index = sdnv_read(buf, size, &cid, &flags);
    if(flags != 0)
    {
        *flags |= flags;
        return bplog(BP_BUNDLEPARSEERR, "Failed to read first custody ID (%08X)\n", flags);
    }

    /* Process Though Fills */
    while((int)fill.index < size)
    {

        /* Read Fill */
        fill.index = sdnv_read(buf, size, &fill, &flags);
        if(flags != 0)
        {
            *flags |= flags;
            return bplog(BP_BUNDLEPARSEERR, "Failed to read fill (%08X)\n", flags);
        }

        /* Process Custody IDs */
        if(cidin == true && ack_success)
        {
            /* Free Bundles */
            cidin = false;
            for(i = 0; i < fill.value; i++)
            {
                int ati = (cid.value + i) % table_size;
                bp_sid_t sid = sids[ati];
                if(sid != BP_SID_VACANT)
                {
                    relinquish(store_handle, sid);
                    sids[ati] = BP_SID_VACANT;
                    (*acks)++;
                }
            }
        }
        else
        {
            /* Skip Bundles */
            cidin = true;
        }

        /* Set Next Custody ID */
        cid.value += fill.value;
    }

    return fill.index;
}
