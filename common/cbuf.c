/************************************************************************
 * File: cbuf.c
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
#include "bundle_types.h"
#include "cbuf.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * Create - initializes circular buffer structure
 *----------------------------------------------------------------------------*/
int cbuf_create(cbuf_t* cbuf, int size)
{
    /* Check Buffer Size */
    if(size <= 0 || (unsigned long)size > BP_MAX_INDEX) return BP_PARMERR;
           
    /* Allocate Circular Buffer */
    cbuf->table = (bp_active_bundle_t*)malloc(sizeof(bp_active_bundle_t) * size);
    if(cbuf->table == NULL) return BP_FAILEDMEM;
            
    /* Initialize Circular to Empty */
    memset(cbuf->table, 0, sizeof(bp_active_bundle_t) * cbuf->size);
    
    /* Initialize Circular Buffer Attributes */
    cbuf->size = size;
    cbuf->num_entries = 0;
    cbuf->oldest_cid = 0;
    
    /* Return Success */
    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * Destroy - frees memory associated with circular buffer
 *----------------------------------------------------------------------------*/
int cbuf_destroy(cbuf_t* cbuf)
{
    if(cbuf->table) free(cbuf->table);
    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * Add
 *----------------------------------------------------------------------------*/
int cbuf_add(cbuf_t* cbuf, bp_active_bundle_t bundle, bool overwrite)
{
    /* Add Bundle */
    bp_index_t ati = bundle.cid % cbuf->size;
    if(cbuf->table[ati].cid == bundle.cid && !overwrite)
    {        
        return BP_DUPLICATECID;
    }
    else
    {
        if(cbuf->table[ati].cid != bundle.cid) cbuf->num_entries++;
        cbuf->table[ati] = bundle;
    }
    
    /* Return Success */
    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * cbuf_next
 *----------------------------------------------------------------------------*/
int cbuf_next(cbuf_t* cbuf, bp_val_t max_cid, bp_active_bundle_t* bundle)
{
    while(cbuf->oldest_cid != max_cid)
    {
        bp_index_t ati = cbuf->oldest_cid % cbuf->size;
        if(cbuf->table[ati].sid == BP_SID_VACANT)
        {
            cbuf->oldest_cid++; 
        }
        else
        {
            *bundle = cbuf->table[ati];
            return BP_SUCCESS;
        }
    }

    return BP_TIMEOUT;
}

/*----------------------------------------------------------------------------
 * Remove
 *----------------------------------------------------------------------------*/
int cbuf_remove(cbuf_t* cbuf, bp_val_t cid, bp_active_bundle_t* bundle)
{
    bp_index_t ati = cid % cbuf->size;
    if( (cbuf->table[ati].sid != BP_SID_VACANT) &&
        (cbuf->table[ati].cid == cid) )
    {
        if(bundle) *bundle = cbuf->table[ati];
        cbuf->table[ati].sid = BP_SID_VACANT;
        cbuf->num_entries--;
        return BP_SUCCESS;
    }

    return BP_CIDNOTFOUND;
}
