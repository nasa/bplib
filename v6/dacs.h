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

#ifndef _dacs_h_
#define _dacs_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bundle_types.h"
#include "rb_tree.h"

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int dacs_write  (uint8_t* rec, int size, int max_fills_per_dacs, rb_tree_t* tree, uint32_t* flags);
int dacs_read   (uint8_t* rec, int rec_size, int* num_acks, bp_delete_func_t ack, void* ack_parm, uint32_t* flags);

#endif  /* _dacs_h_ */
