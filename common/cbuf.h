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
#ifndef _cbuf_h_
#define _cbuf_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bundle_types.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    bp_active_bundle_t* table;
    bp_index_t          size;
    bp_val_t            oldest_cid;
} cbuf_t;

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int cbuf_create     (cbuf_t* cbuf, int size);
int cbuf_destroy    (cbuf_t* cbuf);
int cbuf_add        (cbuf_t* cbuf, bp_active_bundle_t bundle, bool overwrite);
int cbuf_next       (cbuf_t* cbuf, bp_val_t max_cid, bp_active_bundle_t* bundle);
int cbuf_remove     (cbuf_t* cbuf, bp_val_t cid, bp_active_bundle_t* bundle);
int cbuf_count      (cbuf_t* cbuf, bp_val_t max_cid);

#endif /* _cbuf_h_ */