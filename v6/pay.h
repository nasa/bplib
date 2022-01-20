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

#ifndef _pay_h_
#define _pay_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bundle_types.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    bp_field_t  bf;             /* block flags */
    bp_field_t  blklen;         /* block length */
    const uint8_t* payptr;         /* pointer to payload */
    int         paysize;        /* size in bytes of payload */
} bp_blk_pay_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int pay_read      (const void* block, int size, bp_blk_pay_t* pay, bool update_indices, uint32_t* flags);
int pay_write     (void* block, int size, bp_blk_pay_t* pay, bool update_indices, uint32_t* flags);

#endif  /* _pay_h_ */
