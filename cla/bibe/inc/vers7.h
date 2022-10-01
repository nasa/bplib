/************************************************************************
 * File: vers7.h
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *
 * Maintainer(s):
 *  Karl Sipfle, for Code 580 NASA GSFC
 *
 * Copyright © 2020-2022 United States Government as represented by 
 * the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *************************************************************************/

#ifndef _v7_h_
#define _v7_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

//#include "bundle_types.h"
#include "rb_tree.h"
#include "../../tinycbor-main/src/cbor.h"  // KRS (ION also has a cbor.h)
//#include "pri.h"  // KRS

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_PRI_VERSION                  0x07

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
//no #define BP_BIB_BLK_TYPE                 0x3  // that value was for v6 payload integrity block
#define BP_BIB_BLK_TYPE                 0xB

/* BIB Definitions */
#define BP_BIB_INTEGRITY_SIGNATURE      5

/* Bundle Creation Time Handling */
#define BP_TTL_CREATION_TIME            0           /* time-to-live extension block may be used */
#define BP_UNKNOWN_CREATION_TIME        1           /* unreliable time source */
#define BP_BEST_EFFORT_LIFETIME         1576800000  /* 50 years; ground systems using 'int' type for time have maximum of 68 years */

/* Record Type Definitions */    
#define BP_STAT_REC_TYPE                0x10        /* Status Report */
//v6 #define BP_CS_REC_TYPE                  0x20        /* Custody Signal */
//v6 #define BP_ACS_REC_TYPE                 0x40        /* Aggregate Custody Signal */
#define BP_ADMIN_REC_TYPE_BIBE          0x03        // KRS
#define BP_ADMIN_REC_TYPE_CUSTODY_SIGNAL 0x04       // KRS


/* Aggregate Custody Signal Definitions */
#define BP_ACS_REC_TYPE_INDEX           0
#define BP_ACS_REC_STATUS_INDEX         1
#define BP_ACS_ACK_MASK                 0x80        /* if set, then custody successfully transfered */

/* Processing Control Flags */
#define BP_PCF_FRAGMENT_MASK            0x000001    /* bundle is a fragement */
#define BP_PCF_ADMIN_MASK               0x000002    /* payload is an administrative record */
#define BP_PCF_NOFRAG_MASK              0x000004    /* bundle must not be fragmented */
#define BP_PCF_ACKRQST_MASK             0x000020    /* acknowledgement is requested from application */
#define BP_PCF_STATUS_TIME_RQST_MASK    0x000040    /* new v7  status time is requested in all status reports */
#define BP_PCF_RPTRCV_MASK              0x004000    /* report reception */
#define BP_PCF_RPTFRW_MASK              0x010000    /* report forwarding */
#define BP_PCF_RPTDLV_MASK              0x020000    /* report delivery */
#define BP_PCF_RPTDLT_MASK              0x040000    /* report deletion */

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

//int v7_display(void* bundle, int size, uint32_t* flags);
//int v7_display_primary_block(void* bundle);  // KRS
//void dump(uint8_t* buffer, size_t normal_length, size_t max_length, bool enable);  // KRS

#endif  /* _v7_h_ */
