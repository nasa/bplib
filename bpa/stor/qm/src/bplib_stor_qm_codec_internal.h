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

#ifndef BPLIB_STOR_QM_CODEC_INTERNAL_H
#define BPLIB_STOR_QM_CODEC_INTERNAL_H

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
#include "bplib_crc.h"

typedef struct
{
    const BPLib_STOR_CACHE_CanonicalBundleBlock_t *encode_block;
    BPLib_STOR_CACHE_CanonicalBundleBlock_t       *decode_block;
    const void                        *content_vptr;
    size_t                            *content_offset_out;
    size_t                             content_size;

} canonical_block_info_t;

typedef struct bitmap_table
{
    size_t       offset;
    uint64_t mask;
} bitmap_table_t;

BPLib_CRC_Parameters_t *codec_get_crc_algorithm(BPLib_CRC_Type_t crctype);
size_t                  sum_preencoded_size(BPLib_STOR_CACHE_Block_t *list);
void                    init_ipn_eid(BPLib_STOR_CACHE_EidBuffer_t *eid, BPLib_Ipn_t node, BPLib_Ipn_t service);
size_t                  sum_preencoded_size(BPLib_STOR_CACHE_Block_t *list);
#endif /* BPLIB_STOR_QM_CODEC_INTERNAL_H */
