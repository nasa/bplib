/************************************************************************
 * File: pay.h
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

#ifndef __BPLIB_PAY_H__
#define __BPLIB_PAY_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    bp_field_t  bf;
    bp_field_t  blklen;
    uint8_t*    payptr;
    int         paysize;
} bp_blk_pay_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int pay_read      (void* block, int size, bp_blk_pay_t* pay, bool update_indices, uint16_t* flags);
int pay_write     (void* block, int size, bp_blk_pay_t* pay, bool update_indices, uint16_t* flags);

#endif  /* __BPLIB_PAY_H__ */
