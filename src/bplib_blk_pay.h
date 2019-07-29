/************************************************************************
 * File: bplib_blk_pay.h
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

#ifndef __BPLIB_BLK_PAY_H__
#define __BPLIB_BLK_PAY_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"
#include "bplib_sdnv.h"
#include "rb_tree.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_PAY_BLK_TYPE           0x1
#define BP_STAT_REC_TYPE          0x10 /* Status Report */
#define BP_CS_REC_TYPE            0x20 /* Custody Signal */
#define BP_ACS_REC_TYPE           0x40 /* Aggregate Custody Signal */

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    bp_sdnv_t   bf;
    bp_sdnv_t   blklen;
    uint8_t*    payptr;
    int         paysize;
} bp_blk_pay_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Generic Payload Block */
int bplib_blk_pay_read      (void* block, int size, bp_blk_pay_t* pay, bool update_indices);
int bplib_blk_pay_write     (void* block, int size, bp_blk_pay_t* pay, bool update_indices);

/* Aggregate Custody Signal */
int bplib_rec_acs_process   (void* block, int size, int* acks,
                             bp_sid_t* sids, int table_size, 
                             bp_store_relinquish_t relinquish, int store_handle);

int bplib_rec_acs_write     (uint8_t* rec, int size, int max_fills_per_dacs, 
                             rb_tree_t* tree,
                             rb_node_t** iter);

#endif  /* __BPLIB_BLK_PAY_H__ */
