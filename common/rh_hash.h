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
#ifndef __RH_HASH__
#define __RH_HASH__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    bp_val_t        key;
    void*           data;
    unsigned int    chain;  // depth of the chain to reach this entry, 0 indicates empty
    unsigned int    hash;   // unconstrained hash value
    unsigned int    next;
    unsigned int    prev;
} rh_hash_node_t;

typedef struct {
    rh_hash_node_t* table;
    unsigned int size;
    unsigned int num_entries;
    unsigned int max_chain;
    unsigned int curr_index;
} rh_hash_t;

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int rh_hash_create  (rh_hash_t* rh_hash, int hash_size);
int rh_hash_destroy (rh_hash_t* rh_hash);
int rh_hash_add     (rh_hash_t* rh_hash, bp_val_t key, void* data, bool overwrite);
int rh_hash_get     (rh_hash_t* rh_hash, bp_val_t key, void** data);
int rh_hash_find    (rh_hash_t* rh_hash, bp_val_t key);
int rh_hash_remove  (rh_hash_t* rh_hash, bp_val_t key);
int rh_hash_clear   (rh_hash_t* rh_hash);

#endif