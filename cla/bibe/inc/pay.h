/************************************************************************
 * File: pay.h
 *
 *  Copyright 2019-2022 United States Government as represented by the 
 *  Administrator of the National Aeronautics and Space Administration. 
 *  All Other Rights Reserved.  
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be 
 *  used, distributed and modified only pursuant to the terms of that 
 *  agreement.
 *
 * Maintainer(s):
 *  Karl Sipfle, Code 580 NASA GSFC
 *
 *************************************************************************/

#ifndef _pay_h_
#define _pay_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

//#include "../inc/bplib.h"  // KRS
//#include "../lib/bundle_types.h"  // KRS
#include "../../tinycbor-main/src/cbor.h"  // KRS (ION also has a cbor.h)

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    unsigned long blknum; // v7
    unsigned long bf;           /* block flags */
    unsigned long crctype; // v7
    uint8_t*    payptr;         /* pointer to payload */
    int         paysize;        /* size in bytes of payload */
    int         admin_record_type;          // 3 means BPDU (payload containing encapsulated bundle)  KRS
    uint64_t        bpdu_transmission_id;       // 0 if no CT requested for this admin record  KRS
    uint64_t        bpdu_retransmission_time;   // 0 if no CT requested for this admin record  KRS
    
    int custody_status;  // KRS
    uint64_t first_transmission_id[1000];  // MAGIC NUMBER  KRS
    int num_transmission_ids[1000];  // MAGIC NUMBER  KRS
} bp_blk_pay_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

//int pay_decborize (CborValue* recursed, int size, bp_blk_pay_t* pay, uint32_t* flags, bool admin_record);
//int pay_cborize(CborEncoder* encoder, bp_blk_pay_t* /*, rb_tree_t* custody_tree*/);

#endif  /* _pay_h_ */
 