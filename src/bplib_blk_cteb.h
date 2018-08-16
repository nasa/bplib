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

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_CTEB_BLK_TYPE          0xA
#define BP_CTEB_BLK_LENGTH        12

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    uint32_t cid;
    uint32_t cstnode;
    uint32_t cstserv;
} bp_blk_cteb_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int bplib_blk_cteb_read     (void* contents, int size, bp_blk_cteb_t* cteb);
int bplib_blk_cteb_write    (void* block, int size, bp_blk_cteb_t* cteb);
int bplib_blk_cteb_update   (void* block, int size, uint32_t cid);

#endif  /* __BPLIB_BLK_CTEB_H__ */
