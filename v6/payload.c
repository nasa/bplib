/************************************************************************
 * File: payload.c
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
#include "payload.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * payload_initialize - 
 *-------------------------------------------------------------------------------------*/
int payload_initialize(bp_payload_t* payload, bp_attr_t* attr, bp_store_t* store, uint16_t* flags)
{
    (void)flags;
    
    /* Initialize Attributes */
    payload->attributes = attr;
    
    /* Initialize Bundle Store */
    payload->store   = store;
    payload->handle  = payload->store->create(payload->attributes->storage_service_parm);
    
    /* Handle Errors */
    if(payload->handle < 0)
    {
        payload_uninitialize(payload);
        return bplog(BP_FAILEDSTORE, "Failed to create storage handle for payloads\n");
    }
    
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * payload_uninitialize - 
 *-------------------------------------------------------------------------------------*/
void payload_uninitialize(bp_payload_t* payload)
{
    if(payload)
    {
        if(payload->handle >= 0) payload->store->destroy(payload->handle);
    }    
}

/*--------------------------------------------------------------------------------------
 * payload_receive -
 *-------------------------------------------------------------------------------------*/
int payload_receive(bp_payload_t* payload, bool cst_rqst, uint8_t* pay, int pay_size, int timeout, uint16_t* flags)
{
    int                 status  = BP_SUCCESS;
    bp_payload_data_t*  data    = &payload->data;

    /* Initialize Payload Store */
    data->payloadsize     = pay_size;
    data->request_custody = cst_rqst;

    /* Enqueue Payload into Storage */
    int enstat = payload->store->enqueue(payload->handle, data, sizeof(bp_payload_data_t), pay, pay_size, timeout);
    if(enstat <= 0)
    {
        status = bplog(BP_FAILEDSTORE, "Failed (%d) to store payload\n", enstat);
        *flags |= BP_FLAG_STOREFAILURE;
    }
    
    return status;
}
