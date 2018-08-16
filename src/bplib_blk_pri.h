/******************************************************************************
 * Filename:        bplib_blk_pri.h
 * Purpose:         Bundle Protocol Primary Block (header) 
 * Design Notes:
 ******************************************************************************/

#ifndef __BPLIB_BLK_PRI_H__
#define __BPLIB_BLK_PRI_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>
#include "bplib.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_PRI_VERSION        0
#define BP_PRI_BLK_LENGTH     52

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Bundle Meta Data */
typedef struct {
    uint32_t    version;
    uint32_t    blklen;
    uint32_t    dstnode;
    uint32_t    dstserv;
    uint32_t    srcnode;
    uint32_t    srcserv;
    uint32_t    rptnode;
    uint32_t    rptserv;
    uint32_t    cstnode;
    uint32_t    cstserv; 
    bp_time_t   createtime;
    uint32_t    createseq;
    uint32_t    lifetime;
    uint32_t    dictlen;
    uint32_t    fragoffset;
    uint32_t    paylen;
    /* meta information */
    int         is_admin_rec;       // 0: not admin, 1: is admin
    int         is_frag;            // 0: unfragmented, 1: fragmented
    int         request_custody;    // 0: not requested, 1: requested
    int         allow_frag;         // 0: do not allow, 1: allow
    int         report_deletion;    // 0: do not report, 1: report
    /* state data */
    uint32_t    sequence;
    int         creation_time_sys;      // 1: use system time, 0: use provided channel value
} bp_blk_pri_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int bplib_blk_pri_read      (void* block, int size, bp_blk_pri_t* pri);
int bplib_blk_pri_write     (void* block, int size, bp_blk_pri_t* pri);
int bplib_blk_pri_setfrag   (void* block, uint32_t fragoffset);
int bplib_blk_pri_settime   (void* block, bp_time_t tm);
int bplib_blk_pri_setseq    (void* block, uint32_t seq);
int bplib_blk_pri_setpaylen (void* block, uint32_t paylen);
int bplib_blk_pri_setdst    (void* block, uint32_t dstnode, uint32_t dstserv);

#endif  /* __BPLIB_BLK_PRI_H__ */
