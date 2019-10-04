/************************************************************************
 * File: rh_hash.h
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
#ifndef _rh_hash_h_
#define _rh_hash_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bundle_types.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    uint32_t            hash;   // unconstrained hash value
    bp_active_bundle_t  bundle;
    bp_index_t          chain;  // depth of the chain to reach this entry, 0 indicates empty
    bp_index_t          next;   // next entry in chain
    bp_index_t          prev;   // previous entry in chain
    bp_index_t          after;  // next entry added to hash (time ordered)
    bp_index_t          before; // previous entry added to hash (time ordered)
} rh_hash_node_t;

typedef struct {
    rh_hash_node_t*     table;
    bp_index_t          size;
    bp_index_t          num_entries;
    bp_index_t          oldest_entry;
    bp_index_t          newest_entry;
    bp_index_t          max_chain;
} rh_hash_t;

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int rh_hash_create  (rh_hash_t* rh_hash, int size);
int rh_hash_destroy (rh_hash_t* rh_hash);
int rh_hash_add     (rh_hash_t* rh_hash, bp_active_bundle_t bundle, bool overwrite);
int rh_hash_get     (rh_hash_t* rh_hash, bp_val_t cid, bp_active_bundle_t* bundle);
int rh_hash_remove  (rh_hash_t* rh_hash, bp_val_t cid, bp_active_bundle_t* bundle);
int rh_hash_clear   (rh_hash_t* rh_hash);

#endif /* _rh_hash_h_ */