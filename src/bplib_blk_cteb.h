/************************************************************************
 * File: bplib_blk_cteb.h
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

#ifndef __BPLIB_BLK_CTEB_H__
#define __BPLIB_BLK_CTEB_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"
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

int bplib_blk_cteb_read     (void* block, int size, bp_blk_cteb_t* cteb, bool update_indices);
int bplib_blk_cteb_write    (void* block, int size, bp_blk_cteb_t* cteb, bool update_indices);

#endif  /* __BPLIB_BLK_CTEB_H__ */
