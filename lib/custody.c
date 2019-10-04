/************************************************************************
 * File: custody.c
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
#include "custody.h"
#include "v6.h"

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * custody_generate -
 *
 *  Notes:
 *-------------------------------------------------------------------------------------*/
static int custody_generate(bp_custody_t* custody, unsigned long sysnow, int timeout, uint16_t* flags)
{
    int send_status = BP_SUCCESS;
    int ret_status = BP_SUCCESS;

    /* If the tree has nodes, initialize the iterator for traversing the tree in order */
    rb_tree_goto_first(&custody->tree);

    /* Continue to delete nodes from the tree and write them to DACS until the tree is empty */
    while (!rb_tree_is_empty(&custody->tree))
    {
        /* Build DACS - will remove nodes from the tree */
        int dacs_size = v6_populate_acknowledgment(custody, flags);
        if(dacs_size > 0)
        {
            send_status = bundle_generate(&custody->bundle, custody->recbuf, dacs_size, timeout, flags);
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
int custody_initialize(bp_custody_t* custody, bp_route_t route, bp_attr_t* attributes, bp_generate_t generate, bp_acknowledge_t acknowledge, void* parm, uint16_t* flags)
{
    int status;

    /* Set Custody Parameters */
    custody->attributes = *attributes;
    custody->attributes.request_custody = false;
    custody->attributes.admin_record = true;
    custody->acknowledge = acknowledge;
    custody->ackparm = parm;
    custody->last_time = 0;
    custody->lock = BP_INVALID_HANDLE;
    custody->recbuf = 0;
    custody->tree.max_size = 0;
            
    /* Initialize DACS Bundle */    
    route.destination_node = 0;
    route.destination_service = 0;
    status = bundle_initialize(&custody->bundle, route, &custody->attributes, generate, parm, flags);
    if(status != BP_SUCCESS)
    {
        /* Does not un-initialize bundle since the bundle initialization 
         * function takes care of that.  Also note that this must be first
         * so that future calls to custody_uninitialize are safe. */
        return status;
    }
    
    /* Create DACS Lock */
    custody->lock = bplib_os_createlock();
    if(custody->lock == BP_INVALID_HANDLE)
    {
        custody_uninitialize(custody);
        return bplog(BP_FAILEDOS, "Failed to create a lock for custody processing\n");
    }

    /* Allocate Memory for Channel DACS Bundle Fills */
    custody->recbuf_size = sizeof(uint16_t) * custody->bundle.attributes->max_fills_per_dacs + 6; // 2 bytes per fill plus payload block header
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
    if(status != BP_SUCCESS)
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
        /* Destroy Lock */
        if(custody->lock != BP_INVALID_HANDLE)
        {
            bplib_os_destroylock(custody->lock);
            custody->lock = BP_INVALID_HANDLE;
        }
        
        /* Free Buffer for DACS */
        if(custody->recbuf)
        {
            free(custody->recbuf);
            custody->recbuf = NULL;
        }

        /* Free Tree */
        rb_tree_destroy(&custody->tree);
    }    
}

/*--------------------------------------------------------------------------------------
 * custody_send -
 *
 *  Notes:  may or may not perform enqueue depending if DACS needs to be sent
 *          based on whether or not it has been too long a time without sending a DACS
 *-------------------------------------------------------------------------------------*/
int custody_send(bp_custody_t* custody, bp_val_t period, int timeout, uint16_t* flags)
{   
    int ret_status = BP_SUCCESS;

    /* Timeout Any Pending Acknowledgments */
    if(period > 0)
    {
        /* Get Time */
        unsigned long sysnow = 0;
        if(bplib_os_systime(&sysnow) == BP_OS_ERROR)
        {
            *flags |= BP_FLAG_UNRELIABLETIME;
        }

        /* Check If Custody Ready to Send */
        bplib_os_lock(custody->lock);
        {
            if( (sysnow >= (custody->last_time + period)) && 
                !rb_tree_is_empty(&custody->tree) )
            {
                int status = custody_generate(custody, sysnow, timeout, flags);
                if(status != BP_SUCCESS && ret_status == BP_SUCCESS)
                {
                    ret_status = status;
                }
            }
        }
        bplib_os_unlock(custody->lock);
    }
    
    /* Return Status */
    return ret_status;
}

/*--------------------------------------------------------------------------------------
 * custody_receive -
 *-------------------------------------------------------------------------------------*/
int custody_receive(bp_custody_t* custody, bp_custodian_t* custodian, int timeout, uint16_t* flags)
{
    int status = BP_SUCCESS;

    /* Get Time */
    unsigned long sysnow = 0;
    if(bplib_os_systime(&sysnow) == BP_OS_ERROR)
    {
        *flags |= BP_FLAG_UNRELIABLETIME;
    }

    /* Take Custody */
    bplib_os_lock(custody->lock);
    {
        if(custody->bundle.route.destination_node == custodian->node && custody->bundle.route.destination_service == custodian->service)
        {
            /* Insert Custody ID directly into current custody tree */
            int insert_status = rb_tree_insert(custodian->cid, &custody->tree);
            if(insert_status == BP_CUSTODYTREEFULL) 
            {
                /* Flag Full Tree - possibly the tree size is configured to be too small */
                *flags |= BP_FLAG_RBTREEFULL;

                /* Store DACS */
                custody_generate(custody, sysnow, timeout, flags);

                /* Start New DACS */
                if(rb_tree_insert(custodian->cid, &custody->tree) != BP_SUCCESS)
                {
                    /* There is no valid reason for an insert to fail on an empty tree */
                    status = BP_FAILEDRESPONSE;
                }
            }
            else if(insert_status == BP_DUPLICATECID)
            {
                /* Duplicate values are fine and are treated as a success */
                *flags |= BP_FLAG_DUPLICATES;
            }
            else if(insert_status != BP_SUCCESS)
            {
                /* Tree error unexpected */
                status = BP_FAILEDRESPONSE;        
            }
        }
        else
        {
            /* Store DACS */
            if(!rb_tree_is_empty(&custody->tree))
            {
                custody_generate(custody, sysnow, timeout, flags);
            }

            /* Initial New DACS Bundle */
            custody->bundle.route.destination_node = custodian->node;
            custody->bundle.route.destination_service = custodian->service;
            custody->bundle.prebuilt = false;

            /* Start New DACS */
            if(rb_tree_insert(custodian->cid, &custody->tree) != BP_SUCCESS)
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
 * custody_acknowledge -
 *-------------------------------------------------------------------------------------*/
int custody_acknowledge(bp_custody_t* custody, bp_custodian_t* custodian, uint16_t* flags)
{
    return v6_receive_acknowledgment(custody, custodian, flags);
}
