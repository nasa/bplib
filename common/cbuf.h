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

/* Circular Buffer Control Structure */
typedef struct
{
    bp_active_bundle_t *table;       /* circular array of active bundkes */
    bp_index_t          size;        /* maximum (allocated) size of circular array */
    bp_index_t          num_entries; /* number of bundles in the circular array */
    bp_val_t            newest_cid;  /* most recent custody id to be inserted into array */
    bp_val_t            oldest_cid;  /* oldest custody id still present in the array */
} cbuf_t;

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

int cbuf_create(cbuf_t **cbuf, int size);
int cbuf_destroy(cbuf_t *cbuf);
int cbuf_add(cbuf_t *cbuf, bp_active_bundle_t bundle, bool overwrite);
int cbuf_next(cbuf_t *cbuf, bp_active_bundle_t *bundle);
int cbuf_remove(cbuf_t *cbuf, bp_val_t cid, bp_active_bundle_t *bundle);
int cbuf_available(cbuf_t *cbuf, bp_val_t cid);
int cbuf_count(cbuf_t *cbuf);

#endif /* _cbuf_h_ */
