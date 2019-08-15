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
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bundle_initialize -
 *-------------------------------------------------------------------------------------*/
int bundle_initialize(bp_bundle_t* bundle, bp_route_t route, bp_store_t store, bp_attr_t* attr, uint16_t* flags)
{
    /* Initialize Bundle Parameters */
    bundle->route               = route;
    bundle->store               = store;
    bundle->attributes          = attr;
    
    /* Initialize Bundle Store */
    bundle->handle  = bundle->store.create(bundle->attributes->storage_service_parm);
    if(bundle->handle < 0)
    {
        bundle_uninitialize(bundle);
        return bplog(BP_FAILEDSTORE, "Failed to create storage handle for bundles\n");
    }
    
    /* Initialize New Bundle */
    return blocks_build(&bundle->blocks, &bundle->data, bundle->attributes, bundle->route, NULL, NULL, NULL, 0, flags);
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
    bp_bundle_blocks_t* blocks = &bundle->blocks;

    /* Check if Re-initialization Needed */
    if(bundle->blocks.prebuilt == false)
    {
        blocks_build(&bundle->blocks, &bundle->data, bundle->attributes, bundle->route, NULL, NULL, NULL, 0, flags);
    }
    
    /* Update Payload */
    blocks->payload_block.payptr = (uint8_t*)pay;
    blocks->payload_block.paysize = pay_size;

    /* Store Bundle */
    return blocks_write(&bundle->blocks, &bundle->data, bundle->attributes, true, bundle->store.enqueue, bundle->handle, timeout, flags);

}

/*--------------------------------------------------------------------------------------
 * bundle_receive -
 *-------------------------------------------------------------------------------------*/
int bundle_receive(bp_bundle_t* bundle, uint8_t** block, int* block_size, uint32_t sysnow, int timeout, uint16_t* flags)
{
    /* Read Bundle */
    return blocks_read(&bundle->blocks, &bundle->data, bundle->attributes, bundle->route, block, block_size, sysnow, timeout, flags);
}
