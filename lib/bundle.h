/************************************************************************
 * File: bundle.h
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

#ifndef __BPLIB_BUNDLE_H__
#define __BPLIB_BUNDLE_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"
#include "blocks.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Bundle Control Structure */
typedef struct {
    bp_store_t          store;                  /* storage service call-backs */
    bp_route_t          route;                  /* addressing information */
    bp_attr_t*          attributes;             /* -pointer- to the channel attributes */
    bool                prebuilt;               /* does pre-built bundle header need initialization */
    int                 handle;                 /* storage service handle for bundle data */
    bp_bundle_data_t    data;                   /* serialized and stored bundle data */
    bp_bundle_blocks_t  blocks;                 /* populated in initialization function */
} bp_bundle_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int     bundle_initialize   (bp_bundle_t* bundle, bp_route_t route, bp_store_t store, bp_attr_t* attr, uint16_t* flags);
void    bundle_uninitialize (bp_bundle_t* bundle);

int     bundle_send         (bp_bundle_t* bundle, uint8_t* pay, int pay_size, int timeout, uint16_t* flags);
int     bundle_receive      (bp_bundle_t* bundle, uint8_t** block, int* block_size, uint32_t sysnow, int timeout, uint16_t* flags);

#endif  /* __BPLIB_BUNDLE_H__ */
