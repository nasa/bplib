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
#include "bplib_os.h"
#include "rh_hash.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define MAX_KEY_SIZE    512
#define EMPTY_ENTRY     0 // must be 0 because rh_hash->table initialized to 0's
#define NULL_INDEX      UINT_MAX

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * hash_key
 *----------------------------------------------------------------------------*/
unsigned int hash_key(bp_val_t key)
{
    uint8_t*        ptr = (uint8_t*)&key;
    unsigned int    h   = 0;
    unsigned int    i   = 0;

    for(i = 0; i < sizeof(key); i++)
    {
        h += *ptr;
        h += ( h << 10 );
        h ^= ( h >> 6 );
        ptr++;
    }

    h += ( h << 3 );
    h ^= ( h >> 11 );
    h += ( h << 15 );

    return h;
}

/*----------------------------------------------------------------------------
 * get_node
 *----------------------------------------------------------------------------*/
int get_node(rh_hash_t* rh_hash, bp_val_t key)
{
    /* Grab Hash Entry */
    unsigned int index = hash_key(key) % rh_hash->size;

    /* Search */
    while(index != NULL_INDEX && rh_hash->table[index].chain != EMPTY_ENTRY)
    {
        /* Compare Hash Key to Key */
        if(rh_hash->table[index].key != key)
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

    return NULL_INDEX;
}

/*----------------------------------------------------------------------------
 * add_node
 *----------------------------------------------------------------------------*/
void add_node(rh_hash_t* rh_hash, bp_val_t key, void* data)
{
    /* Constrain the Hash */
    unsigned int hash = hash_key(key);
    unsigned int curr_index = hash % rh_hash->size;
    
    /* Add Entry Directly to Hash */
    if(rh_hash->table[curr_index].chain == EMPTY_ENTRY)
    {
        rh_hash->table[curr_index].key   = key;
        rh_hash->table[curr_index].data  = data;
        rh_hash->table[curr_index].chain = 1;
        rh_hash->table[curr_index].hash  = hash;
        rh_hash->table[curr_index].next  = NULL_INDEX;
        rh_hash->table[curr_index].prev  = NULL_INDEX;
    }
    /* Add Entry Linked into Hash */
    else
    {
        /* Find First Open Hash Slot */
        unsigned int open_index = (curr_index + 1) % rh_hash->size;
        while(rh_hash->table[open_index].chain != EMPTY_ENTRY)
        {
            assert(open_index != curr_index); // previous checks prevent this
            open_index = (open_index + 1) % rh_hash->size;
        }

        /* Get Indices into List */
        unsigned int next_index = rh_hash->table[curr_index].next;
        unsigned int prev_index = rh_hash->table[curr_index].prev;

        if(rh_hash->table[curr_index].chain == 1) // collision
        {
            /* Transverse to End of Chain */
            prev_index = curr_index;
            while(next_index != NULL_INDEX)
            {
                prev_index = next_index;
                next_index = rh_hash->table[next_index].next;
            }

            /* Add Entry to Open Slot at End of Chain */
            rh_hash->table[prev_index].next  = open_index;
            rh_hash->table[open_index].key   = key;
            rh_hash->table[open_index].data  = data;
            rh_hash->table[open_index].chain = rh_hash->table[prev_index].chain + 1;
            rh_hash->table[open_index].hash  = hash;
            rh_hash->table[open_index].next  = NULL_INDEX;
            rh_hash->table[open_index].prev  = prev_index;

            /* Check For New Max Chain */
            if(rh_hash->table[open_index].chain > rh_hash->max_chain)
            {
                rh_hash->max_chain = rh_hash->table[open_index].chain;
            }
        }
        else // chain > 1, robin hood the slot
        {
            /* Bridge Over Current Slot */
            if(next_index != NULL_INDEX) rh_hash->table[next_index].prev = prev_index;
            rh_hash->table[prev_index].next = next_index;

            /* Transverse to End of Chain */
            while(next_index != NULL_INDEX)
            {
                rh_hash->table[next_index].chain--;
                prev_index = next_index;
                next_index = rh_hash->table[next_index].next;
            }

            /* Copy Current Slot to Open Slot at End of Chain */
            rh_hash->table[prev_index].next  = open_index;
            rh_hash->table[open_index].key   = rh_hash->table[curr_index].key;
            rh_hash->table[open_index].data  = rh_hash->table[curr_index].data;
            rh_hash->table[open_index].chain = rh_hash->table[prev_index].chain + 1;
            rh_hash->table[open_index].hash  = rh_hash->table[curr_index].hash;
            rh_hash->table[open_index].next  = NULL_INDEX;
            rh_hash->table[open_index].prev  = prev_index;

            /* Check For New Max Chain */
            if(rh_hash->table[open_index].chain > rh_hash->max_chain)
            {
                rh_hash->max_chain = rh_hash->table[open_index].chain;
            }

            /* Add Entry to Current Slot */
            rh_hash->table[curr_index].key   = key;
            rh_hash->table[curr_index].data  = data;
            rh_hash->table[curr_index].chain = 1;
            rh_hash->table[curr_index].hash  = hash;
            rh_hash->table[curr_index].next  = NULL_INDEX;
            rh_hash->table[curr_index].prev  = NULL_INDEX;
        }
    }
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*----------------------------------------------------------------------------
 * Create - initializes hash structure
 *----------------------------------------------------------------------------*/
int rh_hash_create(rh_hash_t* rh_hash, int hash_size)
{
    int i;
    
    if(hash_size <= 0) return BP_PARMERR;

    rh_hash->table = (rh_hash_node_t*)malloc(hash_size * sizeof(rh_hash_node_t));
    for(i = 0; i < hash_size; i++) rh_hash->table[i].chain = EMPTY_ENTRY;

    rh_hash->size = hash_size;
    rh_hash->curr_index = 0;
    rh_hash->num_entries = 0;
    rh_hash->max_chain = 0;
    
    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * Destroy - frees memory associated with hash structure
 *----------------------------------------------------------------------------*/
int rh_hash_destroy(rh_hash_t* rh_hash)
{
    rh_hash_clear(rh_hash);

    free(rh_hash->table);

    return BP_SUCCESS;
}

/*----------------------------------------------------------------------------
 * Add
 *----------------------------------------------------------------------------*/
int rh_hash_add(rh_hash_t* rh_hash, bp_val_t key, void* data, bool overwrite)
{
    int status = BP_SUCCESS;
    
    /* Insert Entry into Dictionary */
    unsigned int index = get_node(rh_hash, key); // TODO: is this necessary, roll into add_node
    if(index == NULL_INDEX)
    {
        /* Add Node */
        add_node(rh_hash, key, data);
        rh_hash->num_entries++;
    }
    else if(overwrite)
    {
        /* Overwrite with New Data */
        rh_hash->table[index].data = data;
    }
    else
    {
        /* Refuse to Overwrite Existing Node */
        status = BP_ERROR;
    }

    /* Return Status */
    return status;
}

/*----------------------------------------------------------------------------
 * Get
 *----------------------------------------------------------------------------*/
int rh_hash_get(rh_hash_t* rh_hash, bp_val_t key, void** data)
{
    if(data == NULL) return BP_PARMERR;
    
    unsigned int index = get_node(rh_hash, key);
    if(index != NULL_INDEX)
    {
        *data = rh_hash->table[index].data;
        return BP_SUCCESS;
    }

    return BP_ERROR;
}

/*----------------------------------------------------------------------------
 * Find
 *----------------------------------------------------------------------------*/
int rh_hash_find(rh_hash_t* rh_hash, bp_val_t key)
{
    unsigned int index = get_node(rh_hash, key);
    if(index != NULL_INDEX)
    {
        return BP_SUCCESS;
    }
    
    return BP_ERROR;
}

/*----------------------------------------------------------------------------
 * Remove
 *----------------------------------------------------------------------------*/
int rh_hash_remove(rh_hash_t* rh_hash, bp_val_t key)
{
    int status = BP_SUCCESS;

    /* Check Pointers */
    unsigned int index = get_node(rh_hash, key);
    if(index != NULL_INDEX)
    {
        /* Get List Indices */
        unsigned int next_index = rh_hash->table[index].next;
        unsigned int prev_index = rh_hash->table[index].prev;

        if((rh_hash->table[index].chain == 1) && (next_index != NULL_INDEX))
        {
            rh_hash->table[next_index].chain = EMPTY_ENTRY;

            /* Copy Next Item into Start of Chain */
            rh_hash->table[index].key   = rh_hash->table[next_index].key;
            rh_hash->table[index].data  = rh_hash->table[next_index].data;
            rh_hash->table[index].hash  = rh_hash->table[next_index].hash;
            rh_hash->table[index].next  = rh_hash->table[next_index].next;
            rh_hash->table[index].prev  = NULL_INDEX;

            /* Goto Next Entry (for later transversal) */
            next_index = rh_hash->table[index].next;
            if(next_index != NULL_INDEX) rh_hash->table[next_index].prev = index;
        }
        else
        {
            rh_hash->table[index].chain = EMPTY_ENTRY;

            /* Bridge Over Removed Entry */
            if(next_index != NULL_INDEX) rh_hash->table[next_index].prev = prev_index;
            if(prev_index != NULL_INDEX) rh_hash->table[prev_index].next = next_index;
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
        status = BP_ERROR;
    }

    /* Return Status */
    return status;
}

/*----------------------------------------------------------------------------
 * Clear
 *----------------------------------------------------------------------------*/
int rh_hash_clear(rh_hash_t* rh_hash)
{
    /* Clear Hash */
    for(unsigned int i = 0; rh_hash->num_entries > 0 && i < rh_hash->size; i++)
    {
        if(rh_hash->table[i].chain != EMPTY_ENTRY)
        {
            rh_hash->table[i].chain = EMPTY_ENTRY;
            rh_hash->num_entries--;
        }
    }

    /* Clear Attributes */
    rh_hash->max_chain = 0;
    
    /* Return Success */
    return BP_SUCCESS;
}
