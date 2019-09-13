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
#include "rb_tree.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Acknowledge Call-Back */
typedef int (*bp_acknowledge_t) (void* parm, bp_val_t cid);

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int dacs_write  (uint8_t* rec, int size, int max_fills_per_dacs, rb_tree_t* tree, rb_node_t** iter, uint16_t* flags);
int dacs_read   (uint8_t* rec, int rec_size, bp_acknowledge_t ack, void* ack_parm, uint16_t* flags);

#endif  /* __BPLIB_DACS_H__ */
