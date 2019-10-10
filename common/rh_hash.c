/************************************************************************
 * File: rh_hash.c
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
#include "rh_hash.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define EMPTY_CHAIN     0               /* chain set to this value to indicate empty slot */
#define NULL_INDEX      BP_MAX_INDEX    /* 0 is a valid index so max_val is used */
#define HASH_CID(cid)   (cid)           /* identify function for now */

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * get_node
 *----------------------------------------------------------------------------*/
static int get_node(rh_hash_t* rh_hash, bp_val_t cid)
{
    /* Grab Hash Entry */
    bp_index_t index = HASH_CID(cid) % rh_hash->size;

    /* Search */
    if(rh_hash->table[index].chain != EMPTY_CHAIN)
    {
        while(index != NULL_INDEX)
        {
            /* Compare Hash Key to Key */
            if(rh_hash->table[index].bundle.cid != cid)
            {
                /* Go to next */
                index = rh_hash->table[index].next;
            }
            else
            {
                /* Return match */
                return index;
            }                
        }
    }

    return NULL_INDEX;
}

/*----------------------------------------------------------------------------
 * overwrite_node
 *----------------------------------------------------------------------------*/
static int overwrite_node(rh_hash_t* rh_hash, bp_index_t index, bp_active_bundle_t bundle, bool overwrite)
{
    if(overwrite)
    {
        /* Set Data */
        rh_hash->table[index].bundle = bundle;

        /* Bridge Over Entry */
        bp_index_t before_index = rh_hash->table[index].before;
        bp_index_t after_index = rh_hash->table[index].after;
        if(before_index != NULL_INDEX) rh_hash->table[before_index].after = after_index;

        /* Check if Overwriting Oldest */
        if(index == rh_hash->oldest_entry)  rh_hash->oldest_entry = rh_hash->table[index].after;

        /* Set Current Entry to Newest */
        rh_hash->table[index].after    = NULL_INDEX;
        rh_hash->table[index].before   = rh_hash->newest_entry;
        rh_hash->newest_entry          = index;

        /* Return Success */
        return BP_SUCCESS;
    }
    else
    {
        /* Return Failure */
        return BP_DUPLICATECID;
    }
}

/*----------------------------------------------------------------------------
 * write_node
 *----------------------------------------------------------------------------*/
static void write_node(rh_hash_t* rh_hash, bp_index_t index, bp_active_bundle_t bundle, uint32_t hash)
{
    rh_hash->table[index].hash      = hash;
    rh_hash->table[index].bundle    = bundle;
    rh_hash->table[index].chain     = rh_hash->table[index].chain + 1;
    rh_hash->table[index].next      = NULL_INDEX;
    rh_hash->table[index].prev      = NULL_INDEX;
    rh_hash->table[index].after     = NULL_INDEX;
    rh_hash->table[index].before    = rh_hash->newest_entry;

    /* Update Time Order */
    if(rh_hash->oldest_entry == NULL_INDEX) 
    {
        /* First Entry */
        rh_hash->oldest_entry = index;
        rh_hash->newest_entry = index;
    }
    else
    {
        /* Not First Entry */
        rh_hash->table[rh_hash->newest_entry].after = index;
        rh_hash->newest_entry = index;
    }
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * Create - initializes hash structure
 *----------------------------------------------------------------------------*/
int rh_hash_create(rh_hash_t** rh_hash, int size)
{
    int i;
    
    /* Check Hash Size */
    if(size <= 0 || (unsigned long)size > BP_MAX_INDEX) return BP_PARMERR;

    /* Allocate Hash Structure */
    *rh_hash = (rh_hash_t*)malloc(sizeof(rh_hash_t));
    
    /* Allocate Hash Table */
    (*rh_hash)->table = (rh_hash_node_t*)malloc(size * sizeof(rh_hash_node_t));
    if((*rh_hash)->table == NULL) return BP_FAILEDMEM;
            
    /* Initialize Hash Table to Empty */
    for(i = 0; i < size; i++)
    {
        (*rh_hash)->table[i].chain  = EMPTY_CHAIN;
        (*rh_hash)->table[i].next   = NULL_INDEX;
        (*rh_hash)->table[i].prev   = NULL_INDEX;
        (*rh_hash)->table[i].before = NULL_INDEX;
        (*rh_hash)->table[i].after  = NULL_INDEX;
    }

    /* Initialize Hash Table Attributes */
    (*rh_hash)->size            = size;
    (*rh_hash)->num_entries     = 0;
    (*rh_hash)->max_chain       = 0;
    (*rh_hash)->oldest_entry    = NULL_INDEX;
    (*rh_hash)->newest_entry    = NULL_INDEX;
    
    /* Return Success */
    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * Destroy - frees memory associated with hash structure
 *----------------------------------------------------------------------------*/
int rh_hash_destroy(rh_hash_t* rh_hash)
{
    if(rh_hash)
    {
        if(rh_hash->table) free(rh_hash->table);
        free(rh_hash);
    }
    
    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * Add
 *----------------------------------------------------------------------------*/
int rh_hash_add(rh_hash_t* rh_hash, bp_active_bundle_t bundle, bool overwrite)
{
    /* Constrain the Hash */
    bp_val_t cid = bundle.cid;    
    uint32_t hash = HASH_CID(cid);
    bp_index_t curr_index = hash % rh_hash->size;
    
    /* Add Entry to Hash */
    if(rh_hash->table[curr_index].chain == EMPTY_CHAIN)
    {
        write_node(rh_hash, curr_index, bundle, hash);
    }
    else /* collision */
    {
        /* Check Current Slot for Duplicate */
        if(rh_hash->table[curr_index].bundle.cid == cid)
        {
            return overwrite_node(rh_hash, curr_index, bundle, overwrite);
        }
        
        /* Find First Open Hash Slot */
        bp_index_t open_index = (curr_index + 1) % rh_hash->size;
        while( (rh_hash->table[open_index].chain != EMPTY_CHAIN) &&
               (open_index != curr_index) )
        {
            open_index = (open_index + 1) % rh_hash->size;
        } 

        /* Check for Full Hash */
        if(open_index == curr_index)
        {
            return BP_ACTIVETABLEFULL;
        }
        
        /* Get Indices into List */
        bp_index_t next_index = rh_hash->table[curr_index].next;
        bp_index_t prev_index = rh_hash->table[curr_index].prev;

        /* Insert Node (chain == 1) */
        if(rh_hash->table[curr_index].chain == 1)
        {
            /* Transverse to End of Chain */
            prev_index = curr_index;
            while(next_index != NULL_INDEX)
            {
                /* Check Next Slot for Duplicate */
                if(rh_hash->table[next_index].bundle.cid == cid)
                {
                    return overwrite_node(rh_hash, next_index, bundle, overwrite);
                }
                
                /* Go To Next Slot */
                prev_index = next_index;
                next_index = rh_hash->table[next_index].next;
            }

            /* Set Insertion Point to Open Slot */
            curr_index = open_index;

            /* Add Entry to Open Slot at End of Chain */
            write_node(rh_hash, curr_index, bundle, hash);
            rh_hash->table[prev_index].next = curr_index;
            rh_hash->table[curr_index].prev = prev_index;
            
            /* Check For New Max Chain */
            if(rh_hash->table[curr_index].chain > rh_hash->max_chain)
            {
                rh_hash->max_chain = rh_hash->table[curr_index].chain;
            }
        }
        else /* Robin Hood Insertion (chain > 1) */
        {
            /* Scan for Duplicate */
            bp_index_t scan_index = next_index;
            while(scan_index != NULL_INDEX)
            {
                /* Check Scan Slot for Duplicate */
                if(rh_hash->table[scan_index].bundle.cid == cid)
                {
                    return overwrite_node(rh_hash, scan_index, bundle, overwrite);
                }
                
                /* Go To Next Slot */
                scan_index = rh_hash->table[scan_index].next;
            }

            /* Bridge Over Current Slot */
            if(next_index != NULL_INDEX) rh_hash->table[next_index].prev = prev_index;
            rh_hash->table[prev_index].next = next_index;

            /* Transverse to End of Chain */
            while(next_index != NULL_INDEX)
            {
                /* Decrement Chain */
                rh_hash->table[next_index].chain--;

                /* Go To Next Slot */
                prev_index = next_index;
                next_index = rh_hash->table[next_index].next;
            }

            /* Copy Current Slot to Open Slot at End of Chain */
            rh_hash->table[prev_index].next     = open_index;            
            rh_hash->table[open_index].bundle   = rh_hash->table[curr_index].bundle;
            rh_hash->table[open_index].chain    = rh_hash->table[prev_index].chain + 1;
            rh_hash->table[open_index].hash     = rh_hash->table[curr_index].hash;
            rh_hash->table[open_index].next     = NULL_INDEX;
            rh_hash->table[open_index].prev     = prev_index;
            rh_hash->table[open_index].after    = rh_hash->table[curr_index].after;
            rh_hash->table[open_index].before   = rh_hash->table[curr_index].before;
            
            /* Check to Update Oldest Entry */
            if(rh_hash->oldest_entry == curr_index) rh_hash->oldest_entry = open_index;

            /* Check For New Max Chain */
            if(rh_hash->table[open_index].chain > rh_hash->max_chain)
            {
                rh_hash->max_chain = rh_hash->table[open_index].chain;
            }

            /* Add Entry to Current Slot */
            write_node(rh_hash, open_index, bundle, hash);
            rh_hash->table[curr_index].chain = 1;
        }
    }

    /* New Entry Added */
    rh_hash->num_entries++;

    /* Return Success */
    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * Next
 *----------------------------------------------------------------------------*/
int rh_hash_next(rh_hash_t* rh_hash, bp_val_t max_cid, bp_active_bundle_t* bundle)
{
    (void)max_cid;
    
    if(rh_hash->oldest_entry != NULL_INDEX)
    {
        if(bundle) *bundle = rh_hash->table[rh_hash->oldest_entry].bundle;
        rh_hash->oldest_entry = rh_hash->table[rh_hash->oldest_entry].after;
        return BP_SUCCESS;        
    }

    return BP_CIDNOTFOUND;
}

/*----------------------------------------------------------------------------
 * Remove
 *----------------------------------------------------------------------------*/
int rh_hash_remove(rh_hash_t* rh_hash, bp_val_t cid, bp_active_bundle_t* bundle)
{
    int status = BP_SUCCESS;

    /* Check Pointers */
    bp_index_t index = get_node(rh_hash, cid);
    if(index != NULL_INDEX)
    {
        /* Return Bundle */
        if(bundle) *bundle = rh_hash->table[index].bundle;
        
        /* Get List Indices */
        bp_index_t next_index   = rh_hash->table[index].next;
        bp_index_t prev_index   = rh_hash->table[index].prev;
        bp_index_t after_index  = rh_hash->table[index].after;
        bp_index_t before_index = rh_hash->table[index].before;

        if((rh_hash->table[index].chain == 1) && (next_index != NULL_INDEX))
        {
            rh_hash->table[next_index].chain = EMPTY_CHAIN;

            /* Copy Next Item into Start of Chain */
            rh_hash->table[index].bundle    = rh_hash->table[next_index].bundle;
            rh_hash->table[index].hash      = rh_hash->table[next_index].hash;
            rh_hash->table[index].next      = rh_hash->table[next_index].next;
            rh_hash->table[index].prev      = NULL_INDEX;

            /* Goto Next Entry (for later transversal) */
            next_index = rh_hash->table[index].next;
            if(next_index != NULL_INDEX) rh_hash->table[next_index].prev = index;
        }
        else
        {
            rh_hash->table[index].chain = EMPTY_CHAIN;

            /* Update Time Order */
            if(index == rh_hash->newest_entry)  rh_hash->newest_entry = before_index;
            if(index == rh_hash->oldest_entry)  rh_hash->oldest_entry = after_index;
            
            /* Bridge Over Removed Entry */
            if(next_index != NULL_INDEX)    rh_hash->table[next_index].prev = prev_index;
            if(prev_index != NULL_INDEX)    rh_hash->table[prev_index].next = next_index;
            if(after_index != NULL_INDEX)   rh_hash->table[after_index].before = before_index;
            if(before_index != NULL_INDEX)  rh_hash->table[before_index].after = after_index;            
        }

        /* Transverse to End of Chain */
        while(next_index != NULL_INDEX)
        {
            rh_hash->table[next_index].chain--;
            next_index = rh_hash->table[next_index].next;
        }

        /* Update Statistics */
        rh_hash->num_entries--;
    }
    else
    {
        /* Key Not Found */
        status = BP_CIDNOTFOUND;
    }

    /* Return Status */
    return status;
}

/*----------------------------------------------------------------------------
 * rh_hash_count
 *----------------------------------------------------------------------------*/
int rh_hash_count(rh_hash_t* rh_hash, bp_val_t max_cid)
{
    (void)max_cid;
    return rh_hash->num_entries;
}
