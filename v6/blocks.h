/************************************************************************
 * File: blocks.h
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

#ifndef __BPLIB_BLK_H__
#define __BPLIB_BLK_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "pri.h"
#include "bib.h"
#include "pay.h"
#include "cteb.h"
#include "bundle.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

/* Block Processing Control Flags */
#define BP_BLK_REPALL_MASK              0x000001    /* block must be replicated in every fragment */
#define BP_BLK_NOTIFYNOPROC_MASK        0x000002    /* transmit status report if block cannot be processed */
#define BP_BLK_DELETENOPROC_MASK        0x000004    /* delete bundle if block cannot be processed */
#define BP_BLK_LASTBLOCK_MASK           0x000008    /* last block in bundle */
#define BP_BLK_DROPNOPROC_MASK          0x000010    /* drop block if block cannot be processed */
#define BP_BLK_FORWARDNOPROC_MASK       0x000020    /* block was forwarded without being processed */
#define BP_BLK_EIDREF_MASK              0x000040    /* block contains an EID reference field */

/* Block Type Definitions */
#define BP_PAY_BLK_TYPE                 0x1
#define BP_CTEB_BLK_TYPE                0xA
#define BP_BIB_BLK_TYPE                 0xD

/* Record Type Definitions */    
#define BP_STAT_REC_TYPE                0x10 /* Status Report */
#define BP_CS_REC_TYPE                  0x20 /* Custody Signal */
#define BP_ACS_REC_TYPE                 0x40 /* Aggregate Custody Signal */
    
/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Bundle Blocks */
typedef struct {
    bp_blk_pri_t        primary_block;
    bp_blk_cteb_t       custody_block;
    bp_blk_bib_t        integrity_block;
    bp_blk_pay_t        payload_block;
} bp_bundle_blocks_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int blocks_build    (bp_bundle_t* bundle, bp_blk_pri_t* pri, bp_blk_pay_t* pay, uint8_t* hdr_buf, int hdr_len, uint16_t* flags);
int blocks_enqueue  (bp_bundle_t* bundle, bool set_time, int timeout, uint16_t* flags);

#endif  /* __BPLIB_BLK_H__ */
