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
#include "bplib_sdnv.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_PAY_BLK_TYPE           0x1
#define BP_STAT_REC_TYPE          0x10 // Status Report
#define BP_CS_REC_TYPE            0x20 // Custody Signal
#define BP_ACS_REC_TYPE           0x40 // Aggregate Custody Signal

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
int bplib_rec_acs_process   (void* block, int size,
                             bp_sid_t* active_table, int table_size, 
                             bp_store_relinquish_t relinquish, int store_handle);

int bplib_rec_acs_write     (uint8_t* rec, int size, bool delivered, 
                             uint32_t first_cid, uint32_t* fills, int num_fills);

#endif  /* __BPLIB_BLK_PAY_H__ */
