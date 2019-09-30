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
#include "bplib_os.h"
#include "bundle.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bundle_initialize -
 *-------------------------------------------------------------------------------------*/
int bundle_initialize(bp_bundle_t* bundle, bp_route_t route, bp_attr_t* attributes, uint16_t* flags)
{
    /* Initialize Bundle Parameters */
    bundle->route      = route;
    bundle->attributes = attributes;

    /* Initialize New Bundle */
    return v6_build(bundle, NULL, NULL, 0, flags);
}

/*--------------------------------------------------------------------------------------
 * bundle_send -
 *-------------------------------------------------------------------------------------*/
int bundle_send(bp_bundle_t* bundle, bool set_time, uint8_t* pay, int pay_size, bp_generate_t gen, void* parm, int timeout, uint16_t* flags)
{
    /* Check if Re-initialization Needed */
    if(bundle->prebuilt == false)
    {
        v6_build(bundle, NULL, NULL, 0, flags);
    }
    
    /* Store Bundle */
    return v6_write(bundle, set_time, pay, pay_size, gen, parm, timeout, flags);
}

/*--------------------------------------------------------------------------------------
 * bundle_receive -
 *-------------------------------------------------------------------------------------*/
int bundle_receive(bp_bundle_t* bundle, uint8_t* block, int block_size, bp_val_t sysnow, bp_custodian_t* custodian, uint16_t* flags)
{
    /* Read Bundle */
    return v6_read(bundle, block, block_size, sysnow, custodian, flags);
}

/*--------------------------------------------------------------------------------------
 * bundle_update -
 *-------------------------------------------------------------------------------------*/
int bundle_update (bp_bundle_data_t* data, bp_val_t cid, uint16_t* flags)
{
    return v6_update(data, cid, flags);

}
