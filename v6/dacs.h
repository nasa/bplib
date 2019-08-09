/************************************************************************
 * File: dacs.h
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

#ifndef __BPLIB_DACS_H__
#define __BPLIB_DACS_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"
#include "pri.h"
#include "bib.h"
#include "pay.h"
#include "cteb.h"
#include "rb_tree.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    int     max_acks;
    int     max_fills;
    int     max_gaps;
    int     num_acks;
    void*   acks_list;
} bp_dacs_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int     dacs_initialize     (bp_dacs_t* dacs, int local_node, int local_service, bp_attr_t* attr);
void    dacs_uninitialize   (bp_dacs_t* dacs);

int     dacs_acknowledge    (bp_dacs_t* dacs, bp_blk_cteb_t* cteb, uint32_t sysnow, int timeout, bp_store_enqueue_t enqueue, int store_handle, uint16_t* dacsflags);
int     dacs_check          (bp_dacs_t* dacs, uint32_t period, uint32_t sysnow, int timeout, bp_store_enqueue_t enqueue, int store_handle, uint16_t* dacsflags);
int     dacs_process        (void* rec, int size, int* acks, bp_sid_t* sids, int table_size, bp_store_relinquish_t relinquish, int store_handle, uint16_t* dacsflags);

#endif  /* __BPLIB_DACS_H__ */
