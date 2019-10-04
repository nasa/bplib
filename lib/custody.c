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

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * custody_initialize - Allocates resources for DACS and initializes control structures
 *-------------------------------------------------------------------------------------*/
int custody_initialize(bp_custody_t* custody, bp_route_t route, bp_attr_t* attributes, bp_create_func_t create, bp_remove_func_t remove, void* parm, uint16_t* flags)
{
    int status;

    /* Set Custody Parameters */
    custody->attributes = *attributes;
    custody->attributes.request_custody = false;
    custody->attributes.admin_record = true;
    custody->remove = remove;
    custody->ackparm = parm;
    custody->last_time = 0;
    custody->lock = BP_INVALID_HANDLE;
    custody->recbuf = 0;
    custody->tree.max_size = 0;
            
    /* Initialize DACS Bundle */    
    route.destination_node = 0;
    route.destination_service = 0;
    status = bundle_initialize(&custody->bundle, route, &custody->attributes, create, parm, flags);
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

