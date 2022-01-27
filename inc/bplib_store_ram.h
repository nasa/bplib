/************************************************************************
 * File: bplib_store_ram.h
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

#ifndef _bplib_store_ram_h_
#define _bplib_store_ram_h_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Application API */
void bplib_store_ram_init(void);

/* Service API */
bp_handle_t bplib_store_ram_create(int type, bp_ipn_t node, bp_ipn_t service, bool recover, void *parm);

int bplib_store_ram_destroy(bp_handle_t h);
int bplib_store_ram_enqueue(bp_handle_t h, const void *data1, size_t data1_size, const void *data2, size_t data2_size,
                            int timeout);
int bplib_store_ram_dequeue(bp_handle_t h, bp_object_t **object, int timeout);
int bplib_store_ram_retrieve(bp_handle_t h, bp_sid_t sid, bp_object_t **object, int timeout);
int bplib_store_ram_release(bp_handle_t h, bp_sid_t sid);
int bplib_store_ram_relinquish(bp_handle_t h, bp_sid_t sid);
int bplib_store_ram_getcount(bp_handle_t h);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _bplib_store_ram_h_ */
