/************************************************************************
 * File: cbuf.h
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
#ifndef __CBUF__
#define __CBUF__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bundle.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    bp_active_bundle_t* table;
    bp_index_t          size;
    bp_index_t          num_entries;
    bp_val_t            oldest_cid;
} cbuf_t;

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int cbuf_create     (cbuf_t* cbuf, int size);
int cbuf_destroy    (cbuf_t* cbuf);
int cbuf_add        (cbuf_t* cbuf, bp_active_bundle_t bundle, bool overwrite);
int cbuf_get        (cbuf_t* cbuf, bp_val_t cid, bp_active_bundle_t* bundle);
int cbuf_remove     (cbuf_t* cbuf, bp_val_t cid, bp_active_bundle_t* bundle);
int cbuf_clear      (cbuf_t* cbuf);

#endif