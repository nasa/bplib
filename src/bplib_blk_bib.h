/******************************************************************************
 * Filename:        bplib_blk_bib.h
 * Purpose:         Bundle Protocol Bundle Integrity Block (header) 
 * Design Notes:
 ******************************************************************************/

#ifndef __BPLIB_BLK_BIB_H__
#define __BPLIB_BLK_BIB_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_BIB_BLK_TYPE         0xD

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    bp_sdnv_t   bf;
    bp_sdnv_t   blklen;
    bp_sdnv_t   paytype;
    bp_sdnv_t   paycrc;
} bp_blk_bib_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int bplib_blk_bib_read      (void* contents, int size, bp_blk_bib_t* bib);
int bplib_blk_bib_write     (void* block, int size, bp_blk_bib_t* bib);
int bplib_blk_bib_update    (void* block, int size, void* payload, int payload_size, bp_blk_bib_t* bib);
int bplib_blk_bib_verify    (void* payload, int payload_size, bp_blk_bib_t* bib);

#endif  /* __BPLIB_BLK_BIB_H__ */
