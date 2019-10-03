/************************************************************************
 * File: v6.h
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

#ifndef __BPLIB_V6_H__
#define __BPLIB_V6_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

/* v6 blocks */
#include "pri.h"
#include "bib.h"
#include "pay.h"
#include "cteb.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_PRI_VERSION                  0x06

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

/* Aggregate Custody Signal Definitions */
#define BP_ACS_REC_TYPE_INDEX       0
#define BP_ACS_REC_STATUS_INDEX     1
#define BP_ACS_ACK_MASK             0x80    /* if set, then custody successfully transfered */

/* Processing Control Flags */
#define BP_PCF_FRAGMENT_MASK            0x000001    /* bundle is a fragement */
#define BP_PCF_ADMIN_MASK               0x000002    /* bundle is an administrative record */
#define BP_PCF_NOFRAG_MASK              0x000004    /* bundle must not be fragmented */
#define BP_PCF_CSTRQST_MASK             0x000008    /* custody transfer is requested */
#define BP_PCF_SINGLETON_MASK           0x000010    /* destination endpoint is a singleton */
#define BP_PCF_ACKRQST_MASK             0x000020    /* acknowledgement is requested from application */
#define BP_PCF_COS_MASK                 0x000180    /* class of service */
#define BP_PCF_COS_SHIFT                7
#define BP_PCF_RPTRCV_MASK              0x004000    /* report reception */
#define BP_PCF_RPTACT_MASK              0x008000    /* report acceptance */
#define BP_PCF_RPTFRW_MASK              0x010000    /* report forwarding */
#define BP_PCF_RPTDLV_MASK              0x020000    /* report delivery */
#define BP_PCF_RPTDLT_MASK              0x040000    /* report deletion */

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Version 6 Bundle Blocks */
typedef struct {
    bp_blk_pri_t        primary_block;
    bp_blk_cteb_t       custody_block;
    bp_blk_bib_t        integrity_block;
    bp_blk_pay_t        payload_block;
} bp_v6blocks_t;

#endif  /* __BPLIB_V6_H__ */
