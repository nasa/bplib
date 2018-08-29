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

#define BP_PRI_VERSION                  0x06

#define BP_PCF_FRAGMENT_MASK            0x000001    // bundle is a fragement
#define BP_PCF_ADMIN_MASK               0x000002    // bundle is an administrative record
#define BP_PCF_NOFRAG_MASK              0x000004    // bundle must not be fragmented
#define BP_PCF_CSTRQST_MASK             0x000008    // custody transfer is requested
#define BP_PCF_SINGLETON_MASK           0x000010    // destination endpoint is a singleton
#define BP_PCF_ACKRQST_MASK             0x000020    // acknowledgement is requested from application
#define BP_PCF_COS_MASK                 0x000180    // class of service
#define BP_PCF_COS_SHIFT                7
#define BP_PCF_RPTRCV_MASK              0x004000    // report reception
#define BP_PCF_RPTACT_MASK              0x008000    // report acceptance
#define BP_PCF_RPTFRW_MASK              0x010000    // report forwarding
#define BP_PCF_RPTDLV_MASK              0x020000    // report delivery
#define BP_PCF_RPTDLT_MASK              0x040000    // report deletion

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Bundle Primary Block */
typedef struct {
    /* field data */
    uint8_t     version;
    bp_sdnv_t   pcf;
    bp_sdnv_t   blklen;
    bp_sdnv_t   dstnode;
    bp_sdnv_t   dstserv;
    bp_sdnv_t   srcnode;
    bp_sdnv_t   srcserv;
    bp_sdnv_t   rptnode;
    bp_sdnv_t   rptserv;
    bp_sdnv_t   cstnode;
    bp_sdnv_t   cstserv; 
    bp_sdnv_t   createtms;
    bp_sdnv_t   createtmns;
    bp_sdnv_t   createseq;
    bp_sdnv_t   lifetime;
    bp_sdnv_t   dictlen;
    bp_sdnv_t   fragoffset;
    bp_sdnv_t   paylen;
    /* meta information */
    int         is_admin_rec;           // 0: not admin, 1: is admin
    int         request_custody;        // 0: not requested, 1: requested
    int         allow_frag;             // 0: do not allow, 1: allow (for created bundles, if allowed, it will be used)
    int         is_frag;                // 0: is not a fragment, 1: is a fragment
    int         report_deletion;        // 0: do not report, 1: report
} bp_blk_pri_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int bplib_blk_pri_read      (void* block, int size, bp_blk_pri_t* pri, int update_indices);
int bplib_blk_pri_write     (void* block, int size, bp_blk_pri_t* pri, int update_indices);

#endif  /* __BPLIB_BLK_PRI_H__ */
