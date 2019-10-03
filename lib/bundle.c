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

#include "bundle.h"
#include "v6.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bundle_initialize -
 *-------------------------------------------------------------------------------------*/
int bundle_initialize(bp_bundle_t* bundle, bp_route_t route, bp_attr_t* attributes, bp_generate_t generate, void* parm, uint16_t* flags)
{
    int status = BP_SUCCESS;
    
    /* Initialize Bundle Parameters */
    bundle->route       = route;
    bundle->attributes  = attributes;
    bundle->generate    = generate;
    bundle->genparm     = parm;

    /* Initialize New Bundle */
    if(bundle->attributes->protocol_version == 6)
    {
        bundle->blocks = (bp_v6blocks_t*)malloc(sizeof(bp_v6blocks_t));
        if(bundle->blocks == NULL)
        {
            status = BP_FAILEDMEM;
        }
        else
        {
            status = v6_initialize(bundle, flags);
        }
    }
    else
    {
        status = BP_UNSUPPORTED;        
    }
    
    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bundle_generate -
 *-------------------------------------------------------------------------------------*/
int bundle_generate(bp_bundle_t* bundle, uint8_t* pay, int pay_size, int timeout, uint16_t* flags)
{
    int status = BP_SUCCESS;
    
    /* Check if Re-initialization Needed */
    if(bundle->prebuilt == false)
    {
        status = v6_initialize(bundle, flags);
    }

    /* Send Bundle */
    if(status == BP_SUCCESS)
    {
        status = v6_send(bundle, pay, pay_size, timeout, flags);
    }
    
    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bundle_forward -
 *-------------------------------------------------------------------------------------*/
int bundle_forward(bp_bundle_t* bundle, uint8_t* pay, int pay_size, int timeout, uint16_t* flags)
{
    return v6_send(bundle, pay, pay_size, timeout, flags);
}

/*--------------------------------------------------------------------------------------
 * bundle_receive -
 *-------------------------------------------------------------------------------------*/
int bundle_receive(bp_bundle_t* bundle, uint8_t* block, int block_size, bp_custodian_t* custodian, uint16_t* flags)
{
    return v6_receive(bundle, block, block_size, custodian, flags);
}

/*--------------------------------------------------------------------------------------
 * bundle_update -
 *-------------------------------------------------------------------------------------*/
int bundle_update(bp_bundle_data_t* data, bp_val_t cid, uint16_t* flags)
{
    return v6_update(data, cid, flags);
}

/*--------------------------------------------------------------------------------------
 * bundle_routeinfo -
 *-------------------------------------------------------------------------------------*/
int bundle_routeinfo(void* bundle, int size, bp_route_t* route)
{
    return v6_routeinfo(bundle, size, route);
}