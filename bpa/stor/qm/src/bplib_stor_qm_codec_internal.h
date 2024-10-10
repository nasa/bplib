/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
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
 */

#ifndef BPLIB_STOR_CACHE_CODEC_INTERNAL_H
#define BPLIB_STOR_CACHE_CODEC_INTERNAL_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_time.h"
#include "bplib_mem.h"
#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_decode.h"
#include "bplib_stor_cache_encode.h"
#include "bplib_stor_qm_codec.h"
#include "bplib_stor_cache_block.h"
#include "bplib_stor_cache_mpstream.h"
#include "crc.h"

typedef struct
{
    const bp_canonical_bundle_block_t *encode_block;
    bp_canonical_bundle_block_t       *decode_block;
    const void                        *content_vptr;
    size_t                            *content_offset_out;
    size_t                             content_size;

} v7_canonical_block_info_t;

typedef struct v7_bitmap_table
{
    size_t       offset;
    bp_integer_t mask;
} v7_bitmap_table_t;

bplib_crc_parameters_t *v7_codec_get_crc_algorithm(bp_crctype_t crctype);
size_t                  v7_sum_preencoded_size(BPLib_STOR_CACHE_Block_t *list);
void                    v7_init_ipn_eid(bp_endpointid_buffer_t *eid, bp_ipn_t node, bp_ipn_t service);
size_t                  v7_sum_preencoded_size(BPLib_STOR_CACHE_Block_t *list);
#endif /* BPLIB_STOR_CACHE_CODEC_INTERNAL_H */
