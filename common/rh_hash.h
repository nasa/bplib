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
 DEFINES
 ******************************************************************************/

#define RH_HASH_MAX_INDEX       UINT16_MAX

#define RH_SUCCESS              1
#define RH_INSERT_DUPLICATE     (-1)
#define RH_HASH_FULL            (-2)
#define RH_INVALID_HASH_SIZE    (-3)
#define RH_INVALID_PARM         (-4)
#define RH_MEMORY_ERROR         (-5)
#define RH_KEY_NOT_FOUND        (-6)

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef uint16_t    rh_index_t;

typedef struct {
    bp_val_t        key;
    void*           data;
    uint32_t        hash;   // unconstrained hash value
    rh_index_t      chain;  // depth of the chain to reach this entry, 0 indicates empty
    rh_index_t      next;   // next entry in chain
    rh_index_t      prev;   // previous entry in chain
    rh_index_t      after;  // next entry added to hash (time ordered)
    rh_index_t      before; // previous entry added to hash (time ordered)
} rh_hash_node_t;

typedef struct {
    rh_hash_node_t* table;
    rh_index_t      size;
    rh_index_t      num_entries;
    rh_index_t      oldest_entry;
    rh_index_t      newest_entry;
    rh_index_t      max_chain;
} rh_hash_t;

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int rh_hash_create  (rh_hash_t* rh_hash, int hash_size);
int rh_hash_destroy (rh_hash_t* rh_hash);
int rh_hash_add     (rh_hash_t* rh_hash, bp_val_t key, void* data, bool overwrite);
int rh_hash_get     (rh_hash_t* rh_hash, bp_val_t key, void** data);
int rh_hash_remove  (rh_hash_t* rh_hash, bp_val_t key);
int rh_hash_clear   (rh_hash_t* rh_hash);

#endif