/******************************************************************************
 * Filename:        bplib_blk_cteb.h
 * Purpose:         Bundle Protocol Custody Transfer Extension Block (header) 
 * Design Notes:
 ******************************************************************************/

#ifndef __BPLIB_BLK_CTEB_H__
#define __BPLIB_BLK_CTEB_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>
#include "bplib.h"
#include "bplib_sdnv.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_CTEB_BLK_TYPE          0xA

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    bp_sdnv_t bf;
    bp_sdnv_t blklen;
    bp_sdnv_t cid;
    char      csteid[BP_MAX_EID_STRING];
    bp_ipn_t  cstnode;
    bp_ipn_t  cstserv;
} bp_blk_cteb_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int bplib_blk_cteb_read     (void* block, int size, bp_blk_cteb_t* cteb, int update_indices);
int bplib_blk_cteb_write    (void* block, int size, bp_blk_cteb_t* cteb, int update_indices);

#endif  /* __BPLIB_BLK_CTEB_H__ */
