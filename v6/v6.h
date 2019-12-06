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

#ifndef _v6_h_
#define _v6_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bundle_types.h"
#include "rb_tree.h"

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
#define BP_ACS_REC_TYPE_INDEX           0
#define BP_ACS_REC_STATUS_INDEX         1
#define BP_ACS_ACK_MASK                 0x80    /* if set, then custody successfully transfered */

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
 EXPORTED FUNCTIONS
 ******************************************************************************/

int v6_initialize               (bp_bundle_t* bundle, bp_route_t route, bp_attr_t attributes, uint16_t* flags);
int v6_uninitialize             (bp_bundle_t* bundle);
int v6_populate_bundle          (bp_bundle_t* bundle, uint16_t* flags);
int v6_send_bundle              (bp_bundle_t* bundle, uint8_t* buffer, int size, bp_create_func_t create, void* parm, int timeout, uint16_t* flags);
int v6_receive_bundle           (bp_bundle_t* bundle, uint8_t* buffer, int size, bp_payload_t* payload, uint16_t* flags);
int v6_update_bundle            (bp_bundle_data_t* data, bp_val_t cid, uint16_t* flags);
int v6_populate_acknowledgment  (uint8_t* rec, int size, int max_fills, rb_tree_t* tree, uint16_t* flags);
int v6_receive_acknowledgment   (uint8_t* rec, int size, int* num_acks, bp_remove_func_t remove, void* parm, uint16_t* flags);
int v6_routeinfo                (void* bundle, int size, bp_route_t* route);
int v6_display                  (void* bundle, int size, uint16_t* flags);

#endif  /* _v6_h_ */
