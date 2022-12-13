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

#ifndef V7_DECODE_INTERNAL_H
#define V7_DECODE_INTERNAL_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "v7_codec_internal.h"
#include "cbor.h"

typedef struct v7_decode_state
{
    bool           error;
    const uint8_t *base;
    CborValue     *cbor;
} v7_decode_state_t;

/*
 * Generic encode/decode container (aka CBOR array) helpers.
 * This handles the general process of encoding and decoding a container, specifically:
 *  - capture the parent state
 *  - create a new sub-state for the container
 *  - call the function with that new state
 *  - close out the sub-state after function returns
 *  - resume in the parent.
 */
typedef void (*v7_decode_func_t)(v7_decode_state_t *dec, void *arg);

void v7_decode_container(v7_decode_state_t *dec, size_t entries, v7_decode_func_t func, void *arg);

/* Component decoders */
int  v7_decode_small_int(v7_decode_state_t *dec);
void v7_decode_crc(v7_decode_state_t *dec, bp_crcval_t *v);

void v7_decode_bitmap(v7_decode_state_t *dec, uint8_t *v, const v7_bitmap_table_t *ptbl);
void v7_decode_bp_blocknum(v7_decode_state_t *dec, bp_blocknum_t *v);
void v7_decode_bp_blocktype(v7_decode_state_t *dec, bp_blocktype_t *v);
void v7_decode_bp_integer(v7_decode_state_t *dec, bp_integer_t *v);
void v7_decode_bp_dtntime(v7_decode_state_t *dec, bp_dtntime_t *v);
void v7_decode_bp_crctype(v7_decode_state_t *dec, bp_crctype_t *v);
void v7_decode_bp_endpointid_buffer(v7_decode_state_t *dec, bp_endpointid_buffer_t *v);

/* Block decoders */
void v7_decode_bp_primary_block(v7_decode_state_t *dec, bp_primary_block_t *v);
void v7_decode_bp_admin_record_payload(v7_decode_state_t *dec, bp_canonical_block_buffer_t *v);
void v7_decode_bp_previous_node_block(v7_decode_state_t *dec, bp_previous_node_block_t *v);
void v7_decode_bp_bundle_age_block(v7_decode_state_t *dec, bp_bundle_age_block_t *v);
void v7_decode_bp_hop_count_block(v7_decode_state_t *dec, bp_hop_count_block_t *v);
void v7_decode_bp_custody_tracking_block(v7_decode_state_t *dec, bp_custody_tracking_block_t *v);
void v7_decode_bp_custody_acknowledement_record(v7_decode_state_t *dec, bp_custody_accept_payload_block_t *v);

void v7_decode_bp_canonical_bundle_block(v7_decode_state_t *dec, bp_canonical_bundle_block_t *v,
                                         v7_canonical_block_info_t *info);
void v7_decode_bp_canonical_block_buffer(v7_decode_state_t *dec, bp_canonical_block_buffer_t *v,
                                         size_t *content_encoded_offset, size_t *content_length);

#endif /* V7_DECODE_INTERNAL_H */
