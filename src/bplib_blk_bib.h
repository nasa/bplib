/************************************************************************
 * File: bplib_blk_bib.h
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

#ifndef __BPLIB_BLK_BIB_H__
#define __BPLIB_BLK_BIB_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_sdnv.h"
#include "bplib_os.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_BIB_BLK_TYPE         0xD

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    bp_sdnv_t   block_flags;
    bp_sdnv_t   block_length;
    bp_sdnv_t   security_target_count;
    bp_sdnv_t   security_target_type;
    bp_sdnv_t   security_target_sequence;
    bp_sdnv_t   cipher_suite_id;
    bp_sdnv_t   cipher_suite_flags;
    bp_sdnv_t   security_result_count;
    uint8_t     security_result_type;
    bp_sdnv_t   security_result_length;
    
    // Security data is specified as a union here to avoid mallocing a pointer when
    // creating block definitions.
    union
    {
        uint16_t crc16;
        uint32_t crc32;
    } security_result_data;
} bp_blk_bib_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int bplib_blk_bib_init      (bp_blk_bib_t* bib);
int bplib_blk_bib_read      (void* contents, int size, bp_blk_bib_t* bib, bool update_indices);
int bplib_blk_bib_write     (void* block, int size, bp_blk_bib_t* bib, bool update_indices);
int bplib_blk_bib_update    (void* block, int size, void* payload, int payload_size, bp_blk_bib_t* bib);
int bplib_blk_bib_verify    (void* payload, int payload_size, bp_blk_bib_t* bib);

#endif  /* __BPLIB_BLK_BIB_H__ */
