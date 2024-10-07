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

#ifndef BPLIB_STOR_CACHE_ENCODE_INTERNAL_H
#define BPLIB_STOR_CACHE_ENCODE_INTERNAL_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_stor_cache_codec_internal.h"
#include "cbor.h"

typedef struct v7_encode_state
{
    bool error;

    bool                    crc_flag;
    bp_crcval_t             crc_val;
   BPLib_STOR_CACHE_CrcParameters_t *crc_params;

    CborEncoder *cbor;

    size_t total_bytes_encoded;

    v7_chunk_writer_func_t next_writer;
    void                  *next_writer_arg;
} v7_encode_state_t;

typedef struct
{
    bp_adminrectype_t                encode_rectype;
    const bp_canonical_block_data_t *payload_data;

} v7_admin_rec_payload_encode_info_t;

/*
 * Generic encode/decode container (aka CBOR array) helpers.
 * This handles the general process of encoding and decoding a container, specifically:
 *  - capture the parent state
 *  - create a new sub-state for the container
 *  - call the function with that new state
 *  - close out the sub-state after function returns
 *  - resume in the parent.
 */
typedef void (*v7_encode_func_t)(v7_encode_state_t *enc, const void *arg);

void v7_encode_container(v7_encode_state_t *enc, size_t entries, v7_encode_func_t func, const void *arg);

/* Component encoders */
void v7_encode_small_int(v7_encode_state_t *enc, int val);
void v7_encode_crc(v7_encode_state_t *enc);
void v7_encode_bp_integer(v7_encode_state_t *enc, const bp_integer_t *v);
void v7_encode_bp_blocknum(v7_encode_state_t *enc, const bp_blocknum_t *v);
void v7_encode_bp_blocktype(v7_encode_state_t *enc, const bp_blocktype_t *v);
void v7_encode_bp_crctype(v7_encode_state_t *enc, const bp_crctype_t *v);
void v7_encode_bp_dtntime(v7_encode_state_t *enc, const bp_dtntime_t *v);
void v7_encode_bp_endpointid_buffer(v7_encode_state_t *enc, const bp_endpointid_buffer_t *v);
void v7_encode_bitmap(v7_encode_state_t *enc, const uint8_t *v, const v7_bitmap_table_t *ptbl);

/* Block encoders */
void v7_encode_bp_primary_block(v7_encode_state_t *enc, const bp_primary_block_t *v);
void v7_encode_bp_admin_record_payload(v7_encode_state_t *enc, const bp_canonical_block_buffer_t *v);
void v7_encode_bp_previous_node_block(v7_encode_state_t *enc, const bp_previous_node_block_t *v);
void v7_encode_bp_bundle_age_block(v7_encode_state_t *enc, const bp_bundle_age_block_t *v);
void v7_encode_bp_hop_count_block(v7_encode_state_t *enc, const bp_hop_count_block_t *v);
void v7_encode_bp_custody_tracking_block(v7_encode_state_t *enc, const bp_custody_tracking_block_t *v);
void v7_encode_bp_custody_acknowledement_record(v7_encode_state_t *enc, const BPLIB_CT_AcceptPayloadBlock_t *v);

void v7_encode_bp_canonical_bundle_block(v7_encode_state_t *enc, const bp_canonical_bundle_block_t *v,
                                         const v7_canonical_block_info_t *info);
void v7_encode_bp_canonical_block_buffer(v7_encode_state_t *enc, const bp_canonical_block_buffer_t *v,
                                         const void *content_ptr, size_t content_length,
                                         size_t *content_encoded_offset);

int       v7_encoder_mpstream_write(void *arg, const void *ptr, size_t sz);
int       v7_encoder_write_crc(v7_encode_state_t *enc);
CborError v7_encoder_write_wrapper(void *arg, const void *ptr, size_t sz, CborEncoderAppendType at);
void      v7_encode_bp_adminrec_payload_impl(v7_encode_state_t *enc, const void *arg);
void      v7_encode_bp_custody_acceptance_seqlist_impl(v7_encode_state_t *enc, const void *arg);
void      v7_encode_bp_endpointid_scheme(v7_encode_state_t *enc, const bp_endpointid_scheme_t *v);
void      v7_encode_bp_ipn_nodenumber(v7_encode_state_t *enc, const bp_ipn_nodenumber_t *v);
void      v7_encode_bp_ipn_servicenumber(v7_encode_state_t *enc, const bp_ipn_servicenumber_t *v);
void      v7_encode_bp_ipn_uri_ssp(v7_encode_state_t *enc, const bp_ipn_uri_ssp_t *v);
void      v7_encode_bp_ipn_uri_ssp_impl(v7_encode_state_t *enc, const void *arg);
void      v7_encode_bp_endpointid_buffer_impl(v7_encode_state_t *enc, const void *arg);
void      v7_encode_bp_hop_count_block_impl(v7_encode_state_t *enc, const void *arg);
void      v7_encode_bp_bundle_processing_control_flags(v7_encode_state_t                          *enc,
                                                       const bp_bundle_processing_control_flags_t *v);
void      v7_encode_bp_sequencenumber(v7_encode_state_t *enc, const bp_sequencenumber_t *v);
void      v7_encode_bp_creation_timestamp_impl(v7_encode_state_t *enc, const void *arg);
void      v7_encode_bp_lifetime(v7_encode_state_t *enc, const bp_lifetime_t *v);
void      v7_encode_bp_adu_length(v7_encode_state_t *enc, const bp_adu_length_t *v);
void      v7_encode_bp_primary_block_impl(v7_encode_state_t *enc, const void *arg);
void      v7_encode_bp_creation_timestamp(v7_encode_state_t *enc, const bp_creation_timestamp_t *v);

#endif /* BPLIB_STOR_CACHE_ENCODE_INTERNAL_H */
