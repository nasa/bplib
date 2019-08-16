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
#include "bplib_os.h"
#include "dacs.h"
#include "bundle.h"
#include "custody.h"

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * custody_enqueue -
 *
 *  Notes:
 *-------------------------------------------------------------------------------------*/
static int custody_enqueue(bp_custody_t* custody, uint32_t sysnow, int timeout, uint16_t* flags)
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
int custody_initialize(bp_custody_t* custody, bp_route_t route, bp_store_t store, bp_attr_t* attr, uint16_t* flags)
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
    status = bundle_initialize(&custody->bundle, route, store, attr, flags);
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
            int status = custody_enqueue(custody, sysnow, timeout, flags);
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
 * custody_acknowledge -
 *-------------------------------------------------------------------------------------*/
int custody_acknowledge(bp_custody_t* custody, bp_custodian_t* custodian, uint32_t sysnow, int timeout, uint16_t* flags)
{
    int status = BP_SUCCESS;
    bplib_os_lock(custody->lock);
    {
        if(custody->cstnode == custodian->cst.node && custody->cstserv == custodian->cst.service)
        {
            /* Insert Custody ID directly into current custody tree */
            rb_tree_status_t insert_status = rb_tree_insert(custodian->cst.cid, &custody->tree);
            if(insert_status == RB_FAIL_TREE_FULL) 
            {
                /* Flag Full Tree - possibly the tree size is configured to be too small */
                *flags |= BP_FLAG_RBTREEFULL;

                /* Store DACS */
                custody_enqueue(custody, sysnow, timeout, flags);

                /* Start New DACS */
                if(rb_tree_insert(custodian->cst.cid, &custody->tree) != RB_SUCCESS)
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
            custody_enqueue(custody, sysnow, timeout, flags);

            /* Initial New DACS Bundle */
            custody->cstnode = custodian->cst.node;
            custody->cstserv = custodian->cst.service;

            /* Start New DACS */
            if(rb_tree_insert(custodian->cst.cid, &custody->tree) != RB_SUCCESS)
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
 * custody_process -
 *-------------------------------------------------------------------------------------*/
int custody_process(bp_custody_t* custody, bp_custodian_t* custodian, int* acks, bp_sid_t* sids, int table_size, uint16_t* flags)
{
    return dacs_read(custodian->acs.rec, custodian->acs.rec_size, acks, sids, table_size, custody->bundle.store.relinquish, custody->bundle.handle, flags);
}
