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
static int dacs_enqueue(bp_custody_t* custody, uint32_t sysnow, int timeout, uint16_t* flags)
{
    int send_status = BP_SUCCESS;
    int ret_status = BP_SUCCESS;

    /* If the tree has nodes, initialize the iterator for traversing the tree in order */
    rb_node_t* iter;
    rb_tree_get_first_rb_node(&custody->tree, &iter);

    /* Continue to delete nodes from the tree and write them to DACS until the tree is empty */
    while (!rb_tree_is_empty(&custody->tree))
    {
        /* Build DACS - will remove nodes from the tree */
        int dacs_size = dacs_write(custody->recbuf, custody->recbuf_size, custody->bundle.attributes->max_fills_per_dacs, &custody->tree, &iter, flags);
        if(dacs_size > 0)
        {
            send_status = bundle_send(&custody->bundle, custody->recbuf, dacs_size, timeout, flags);
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
                custody->last_time = sysnow;
            }
        }
    }

    /* Return Status */
    return ret_status;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * custody_initialize - Allocates resources for DACS and initializes control structures
 *-------------------------------------------------------------------------------------*/
int custody_initialize(bp_custody_t* custody, bp_attr_t* attr, bp_store_t* store, bp_ipn_t srcnode, bp_ipn_t srcserv, uint16_t* flags)
{
    int status;

    /* Set Attributes */
    custody->bundle.attributes = attr;
    
    /* Create DACS Lock */
    custody->lock = bplib_os_createlock();
    if(custody->lock < 0)
    {
        custody_uninitialize(custody);
        return bplog(BP_FAILEDOS, "Failed to create a lock for custody processing\n");
    }

    /* Initialize DACS Bundle */
    status = bundle_initialize(&custody->bundle, attr, store, srcnode, srcserv, 0, 0, flags);
    if(status != BP_SUCCESS)
    {
        custody_uninitialize(custody);
        return status;
    }
    
    /* Allocate Memory for Channel DACS Bundle Fills */
    custody->recbuf_size = sizeof(uint16_t) * custody->bundle.attributes->max_fills_per_dacs + 32; // 2 bytes per fill plus payload block header
    custody->recbuf = (uint8_t*)malloc(custody->recbuf_size); 
    if(custody->recbuf == NULL)
    {
        custody_uninitialize(custody);
        return bplog(BP_FAILEDMEM, "Failed to allocate memory for channel DACS fills and payload\n");                    
    }
    else
    {
        memset(custody->recbuf, 0, custody->recbuf_size);
    }

    /* Allocate Memory for DACS Channel Tree to Store Bundle IDs */
    status = rb_tree_create(custody->bundle.attributes->max_gaps_per_dacs, &custody->tree);
    if(status != RB_SUCCESS)
    {
        custody_uninitialize(custody);
        return bplog(BP_FAILEDMEM, "Failed to allocate memory for channel DACS tree\n");
    }
    
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * custody_uninitialize - Frees resources for DACS
 *-------------------------------------------------------------------------------------*/
void custody_uninitialize(bp_custody_t* custody)
{
    if(custody)
    {
        if(custody->recbuf)
        {
            free(custody->recbuf);
        }

        if(custody->tree.max_size > 0) 
        {
            rb_tree_destroy(&custody->tree);
        }
    }    
}

/*--------------------------------------------------------------------------------------
 * custody_acknowledge -
 *-------------------------------------------------------------------------------------*/
int custody_acknowledge(bp_custody_t* custody, bp_blk_cteb_t* cteb, uint32_t sysnow, int timeout, uint16_t* flags)
{
    int status = BP_SUCCESS;
    bplib_os_lock(custody->lock);
    {
        if(custody->cstnode == cteb->cstnode && custody->cstserv == cteb->cstserv)
        {
            /* Insert Custody ID directly into current custody tree */
            rb_tree_status_t insert_status = rb_tree_insert(cteb->cid.value, &custody->tree);
            if(insert_status == RB_FAIL_TREE_FULL) 
            {
                /* Flag Full Tree - possibly the tree size is configured to be too small */
                *flags |= BP_FLAG_RBTREEFULL;

                /* Store DACS */
                dacs_enqueue(custody, sysnow, timeout, flags);

                /* Start New DACS */
                if(rb_tree_insert(cteb->cid.value, &custody->tree) != RB_SUCCESS)
                {
                    /* There is no valid reason for an insert to fail on an empty tree */
                    status = BP_FAILEDRESPONSE;
                }
            }
            else if(insert_status == RB_FAIL_INSERT_DUPLICATE)
            {
                /* Duplicate values are fine and are treated as a success */
                *flags |= BP_FLAG_DUPLICATES;
            }
            else if(insert_status != RB_SUCCESS)
            {
                /* Tree error unexpected */
                status = BP_FAILEDRESPONSE;        
            }
        }
        else
        {
            /* Store DACS */
            dacs_enqueue(custody, sysnow, timeout, flags);

            /* Initial New DACS Bundle */
            custody->cstnode = cteb->cstnode;
            custody->cstserv = cteb->cstserv;

            /* Start New DACS */
            if(rb_tree_insert(cteb->cid.value, &custody->tree) != RB_SUCCESS)
            {
                /* There is no valid reason for an insert to fail on an empty tree */
                status = BP_FAILEDRESPONSE;
            }
        }
    }
    bplib_os_unlock(custody->lock);            
    
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
    int ret_status = BP_SUCCESS;

    /* Timeout Any Pending Acknowledgments */
    if(period > 0)
    {
        if( (sysnow >= (custody->last_time + period)) && 
            !rb_tree_is_empty(&custody->tree) )
        {
            int status = dacs_enqueue(custody, sysnow, timeout, flags);
            if(status != BP_SUCCESS && ret_status == BP_SUCCESS)
            {
                ret_status = status;
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
int custody_process(bp_custody_t* custody, uint8_t* rec, int rec_size, int* acks, bp_sid_t* sids, int table_size, uint16_t* flags)
{
    uint32_t i;
    bp_sdnv_t cid = { 0, 2, 0 };
    bp_sdnv_t fill = { 0, 0, 0 };
    int cidin = true;
    uint8_t acs_status = rec[BP_ACS_REC_STATUS_INDEX];
    bool ack_success = (acs_status & BP_ACS_ACK_MASK) == BP_ACS_ACK_MASK;
    
    /* Initialize Acknowledgment Count */
    *acks = 0;

    /* Read First Custody ID */
    fill.index = sdnv_read(rec, rec_size, &cid, flags);
    if(*flags != 0)
    {
        return bplog(BP_BUNDLEPARSEERR, "Failed to read first custody ID (%08X)\n", *flags);
    }

    /* Process Though Fills */
    while((int)fill.index < rec_size)
    {
        /* Read Fill */
        fill.index = sdnv_read(rec, rec_size, &fill, flags);
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
                    custody->bundle.store->relinquish(custody->bundle.handle, sid);
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
