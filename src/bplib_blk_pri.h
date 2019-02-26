/************************************************************************
 * File: bplib_blk_pri.h
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

#ifndef __BPLIB_BLK_PRI_H__
#define __BPLIB_BLK_PRI_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>
#include "bplib.h"
#include "bplib_sdnv.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_PRI_VERSION                  0x06

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
    bp_sdnv_t   createsec;
    bp_sdnv_t   createseq;
    bp_sdnv_t   lifetime;
    bp_sdnv_t   dictlen;
    bp_sdnv_t   fragoffset;
    bp_sdnv_t   paylen;
    /* meta information */
    bool        is_admin_rec;           // 0: not admin, 1: is admin
    bool        request_custody;        // 0: not requested, 1: requested
    bool        allow_frag;             // 0: do not allow, 1: allow (for created bundles, if allowed, it will be used)
    bool        is_frag;                // 0: is not a fragment, 1: is a fragment
    bool        report_deletion;        // 0: do not report, 1: report
} bp_blk_pri_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int bplib_blk_pri_read      (void* block, int size, bp_blk_pri_t* pri, bool update_indices);
int bplib_blk_pri_write     (void* block, int size, bp_blk_pri_t* pri, bool update_indices);
int bplib_blk_pri_display   (bp_blk_pri_t* pri);

#endif  /* __BPLIB_BLK_PRI_H__ */
