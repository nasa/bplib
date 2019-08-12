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
#include "custody.h"
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
static int dacs_write(uint8_t* rec, int size, int max_fills_per_dacs, rb_tree_t* tree, rb_node_t** iter, uint16_t* flags)
{
    bp_sdnv_t cid = { 0, 2, 4 };
    bp_sdnv_t fill = { 0, 0, 2 };
 
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
    fill.index = sdnv_write(rec, size, cid, flags);
    fill.value = range.offset + 1;
    fill.index = sdnv_write(rec, size, fill, flags);    
    count_fills += 2;

    /* Traverse tree in order and write out fills to dacs. */
    while (count_fills < max_fills_per_dacs && *iter != NULL)
    {
        prev_range = range;
        rb_tree_get_next_rb_node(tree, iter, &range, true, false);        

        /* Write range of missing cid.
           Calculate the missing values between the current and previous node. */
        fill.value = range.value - (prev_range.value + prev_range.offset + 1);
        fill.index = sdnv_write(rec, size, fill, flags);

        /* Write range of received cids. */
        fill.value = range.offset + 1;
        fill.index = sdnv_write(rec, size, fill, flags);    
        count_fills += 2;        
    }

    /* Success Oriented Error Checking */
    if(*flags != 0)
    {
        return bplog(BP_BUNDLEPARSEERR, "Flags raised during processing of ACS (%08X)\n", flags); 
    } 

    /* Return Block Size */
    return fill.index;
}

/*--------------------------------------------------------------------------------------
 * dacs_enqueue -
 *
 *  Notes:
 *-------------------------------------------------------------------------------------*/
static int dacs_enqueue(bp_dacs_t* dacs, bp_bundle_t* bundle, uint32_t sysnow, int timeout, int max_fills_per_dacs, uint16_t* flags)
{
    int send_status = BP_SUCCESS;
    int ret_status = BP_SUCCESS;

    /* If the tree has nodes, initialize the iterator for traversing the tree in order */
    rb_node_t* iter;
    rb_tree_get_first_rb_node(&dacs->tree, &iter);

    /* Continue to delete nodes from the tree and write them to DACS until the tree is empty */
    while (!rb_tree_is_empty(&dacs->tree))
    {
        /* Build DACS - will remove nodes from the tree */
        int dacs_size = dacs_write(dacs->recbuf, dacs->recbuf_size, max_fills_per_dacs, &dacs->tree, &iter, flags);
        if(dacs_size > 0)
        {
            send_status = bundle_send(bundle, dacs->recbuf, dacs_size, timeout, flags);
            if((send_status <= 0) && (ret_status == BP_SUCCESS))
            {
                /* Save first failed DACS enqueue to return later */
                ret_status = send_status;
                *flags |= BP_FLAG_STOREFAILURE;        
                bplog(send_status, "Failed (%d) to store DACS for transmission, bundle dropped\n", send_status);
            }
            else 
            {
                /* DACS successfully enqueued */
                dacs->last_dacs = sysnow;
            }
        }
    }

    /* Return Status */
    return ret_status;
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
static int dacs_new(bp_dacs_t* dacs, uint32_t dstnode, uint32_t dstserv, uint16_t* flags)
{
    bp_data_store_t*  data      = &dacs->bundle_store;
    uint8_t*            hdrbuf  = data->header;
    uint16_t            flags   = 0;
    int                 offset  = 0;

    /* Initialize Storage */
    memset(data, 0, sizeof(bp_data_store_t));

    /* Write Primary Block */
    offset = pri_write(&hdrbuf[0], BP_BUNDLE_HDR_BUF_SIZE, &dacs->primary_block, false, flags);

    /* Write Integrity Block */
    if(dacs->blocks.primary_block.integrity_check)
    {
        data->biboffset = offset;
        offset = bib_write(&hdrbuf[offset], BP_BUNDLE_HDR_BUF_SIZE - offset, &dacs->integrity_block, false, flags) + offset;
    }
    else
    {
        data->biboffset = 0;
    }
    
    /* Write Payload Block */
    data->payoffset = offset;
    data->headersize = pay_write(&hdrbuf[offset], BP_BUNDLE_HDR_BUF_SIZE - offset, &dacs->payload_block, false, flags) + offset;

    /* Set Destination EID */
    dacs->primary_block.dstnode.value = dstnode;
    dacs->primary_block.dstserv.value = dstserv;
    sdnv_write(hdrbuf, BP_BUNDLE_HDR_BUF_SIZE, dacs->primary_block.dstnode, &flags);
    sdnv_write(hdrbuf, BP_BUNDLE_HDR_BUF_SIZE, dacs->primary_block.dstserv, &flags);

    /* Set Custody EID */
    dacs->cstnode = dstnode;
    dacs->cstserv = dstserv;
    
    /* Return Status */
    if(*flags != 0) return BP_BUNDLEPARSEERR;
    else            return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * dacs_update -
 *
 *  Notes:  may or may not perform enqueue depending if DACS needs to be sent
 *          based on whether or not dacs record is full
 *-------------------------------------------------------------------------------------*/
static int dacs_update(bp_dacs_t* dacs, uint32_t cid, uint32_t sysnow, int timeout, int max_fills_per_dacs, bp_store_enqueue_t enqueue, int store_handle, uint16_t* flags)
{
    /* Attempt to insert cid into dacs. */
    bool insert_status = dacs_insert(dacs, cid, flags);
    if(!insert_status)
    {
        /* Store DACS */
        dacs_enqueue(dacs, sysnow, timeout, max_fills_per_dacs, enqueue, store_handle, flags);

        /* Start New DTN ACS */
        insert_status = dacs_insert(dacs, cid, flags);
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
 * custody_initialize - Allocates resources for DACS and initializes control structures
 *-------------------------------------------------------------------------------------*/
int custody_initialize(bp_custody_t* custody, bp_ipn_t srcnode, bp_ipn_t srcserv, bp_store_t* store, bp_attr_t* attr, uint16_t* flags)
{
    int j, status;

    /* Set Attributes */
    custody->max_dacs  = attr->max_concurrent_dacs;
    custody->num_dacs  = 0;

    /* Create DACS Lock */
    custody->lock = bplib_os_createlock();
    if(custody->lock < 0)
    {
        custody_uninitialize(custody);
        return bplog(BP_FAILEDOS, "Failed to create a lock for custody processing\n");
    }

    /* Initialize DACS Bundle */
    status = dacs_initialize(&custody->dacs, srcnode, srcserv, dstnode, dstserv, store, attr, flags);
    if(status != BP_SUCCESS)
    {
        custody_uninitialize(custody);
        return status;
    }
    
    /* Allocate Memory for DACS Bundle */
    custody->dacs = malloc(sizeof(bp_dacs_t) * custody->max_dacs);
    if(custody->dacs == NULL)
    {
        custody_uninitialize(custody);
        return bplog(BP_FAILEDMEM, "Failed to allocate memory for DACS\n");                    
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
            custody_uninitialize(custody);
            return bplog(BP_FAILEDMEM, "Failed to allocate memory for channel DACS fills and payload\n");                    
        }
        else
        {
            memset(dacs[j].recbuf, 0, dacs[j].recbuf_size);
        }

        /* Allocate Memory for DACS Channel Tree to Store Bundle IDs */
        status = rb_tree_create(dacs[j].max_gaps, &dacs[j].tree);
        if(status != RB_SUCCESS)
        {
            custody_uninitialize(custody);
            return bplog(BP_FAILEDMEM, "Failed to allocate memory for channel DACS tree\n");
        }
    }
    
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * custody_uninitialize - Frees resources for DACS
 *-------------------------------------------------------------------------------------*/
void custody_uninitialize(bp_custody_t* custody)
{
    int j;
    
    if(custody)
    {
        bp_dacs_t* dacs = custody->dacs;
        for(j = 0; j < custody->max_dacs; j++)
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
 * custody_acknowledge -
 *-------------------------------------------------------------------------------------*/
int custody_acknowledge(bp_custody_t* custody, bp_blk_cteb_t* cteb, uint32_t sysnow, int timeout, bp_store_enqueue_t enqueue, int store_handle, uint16_t* flags)
{
    int j;
    int status = BP_SUCCESS;
    bp_dacs_t* dacs_ptr = NULL;                    

    bp_dacs_t* dacs = custody->dacs;
    bplib_os_lock(custody->lock);
    {
        /* Find DACS Entry */                   
        for(j = 0; j < custody->num_dacs; j++)
        {
            if(dacs[j].cstnode == cteb->cstnode && dacs[j].cstserv == cteb->cstserv)
            {
                dacs_ptr = &dacs[j];
                break;
            }
        }

        /* Handle Entry Not Found */
        if(dacs_ptr == NULL)
        {
            if(custody->num_dacs < custody->max_dacs)
            {
                /* Initial DACS Bundle */
                dacs_ptr = &dacs[custody->num_dacs++];
                dacs_new(dacs_ptr, cteb->cstnode, cteb->cstserv, flags);
            }
            else
            {
                /* No Room in Table for Another Source */
                *flags |= BP_FLAG_TOOMANYSOURCES;
                status = bplog(BP_FAILEDRESPONSE, "No room in DACS table for another source %d.%d\n", cteb->cstnode, cteb->cstserv);
            }
        }
    }
    bplib_os_unlock(custody->lock);            

    /* Update DACS */
    if(dacs_ptr != NULL)
    {
        status = dacs_update(dacs_ptr, cteb->cid.value, sysnow, timeout, dacs->max_fills, flags);
    }
    
    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * custody_check -
 *
 *  Notes:  may or may not perform enqueue depending if DACS needs to be sent
 *          based on whether or not it has been too long a time without sending a DACS
 *-------------------------------------------------------------------------------------*/
int custody_check(bp_custody_t* custody, uint32_t period, uint32_t sysnow, int timeout, uint16_t* flags)
{   
    int j, ret_status;

    /* Timeout Any Pending Acknowledgments */
    if(period > 0)
    {
        bp_dacs_t* dacs = custody->dacs;
        for(j = 0; j < custody->num_dacs; j++)
        {
            if( (sysnow >= (dacs[j].last_dacs + period)) && 
                !rb_tree_is_empty(&dacs[j].tree) )
            {
                int status = dacs_enqueue(&dacs[j], sysnow, timeout, dacs->max_fills, flags);
                if(status != BP_SUCCESS && ret_status == BP_SUCCESS)
                {
                    ret_status = status;
                }
            }
        }
    }
    
    /* Return Status */
    return ret_status;
}

/*--------------------------------------------------------------------------------------
 * custody_process -
 *
 *  rec - pointer to ACS administrative record [INPUT]
 *  size - size of record [INPUT]
 *  acks - number of bundles acknowledged
 *
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int custody_process(bp_custody_t* custody, void* rec, int size, int* acks, bp_sid_t* sids, int table_size, uint16_t* flags)
{
    uint32_t i;
    bp_sdnv_t cid = { 0, 2, 0 };
    bp_sdnv_t fill = { 0, 0, 0 };
    uint8_t* buf = (uint8_t*)rec;
    int cidin = true;
    uint8_t acs_status = buf[BP_ACS_REC_STATUS_INDEX];
    bool ack_success = (acs_status & BP_ACS_ACK_MASK) == BP_ACS_ACK_MASK;
    
    /* Initialize Acknowledgment Count */
    *acks = 0;

    /* Read First Custody ID */
    fill.index = sdnv_read(buf, size, &cid, flags);
    if(*flags != 0)
    {
        return bplog(BP_BUNDLEPARSEERR, "Failed to read first custody ID (%08X)\n", *flags);
    }

    /* Process Though Fills */
    while((int)fill.index < size)
    {

        /* Read Fill */
        fill.index = sdnv_read(buf, size, &fill, flags);
        if(*flags != 0)
        {
            return bplog(BP_BUNDLEPARSEERR, "Failed to read fill (%08X)\n", *flags);
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
                    custody->bundle.bundle_store.service->relinquish(custody->bundle.bundle_store.handle, sid);
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
