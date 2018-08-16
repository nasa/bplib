/******************************************************************************
 * Filename:        bplib_blk_pay.h
 * Purpose:         Bundle Protocol Payload Block (header) 
 * Design Notes:
 ******************************************************************************/

#ifndef __BPLIB_BLK_PAY_H__
#define __BPLIB_BLK_PAY_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>
#include "bplib.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_PROTO_PAY_BLK_TYPE           0x1
#define BP_PROTO_STAT_REC_TYPE          0x10 // Status Report
#define BP_PROTO_CS_REC_TYPE            0x20 // Custody Signal
#define BP_PROTO_ACS_REC_TYPE           0x40 // Aggregate Custody Signal

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Generic Payload Block */
int bplib_blk_pay_write     (void* block, int size);
int bplib_blk_pay_update    (void* block, int size, int payload_size);

/* Aggregate Custody Signal */
int bplib_rec_acs_process   (void* block, int size, uint8_t rec_status,
                             bp_sid_t* active_table, int table_size, 
                             bp_store_relinquish_t relinquish, int store_handle);

int bplib_rec_acs_write     (uint8_t* rec, int size, int delivered, 
                             uint32_t first_cid, uint32_t* fills, int num_fills);

#endif  /* __BPLIB_BLK_PAY_H__ */
