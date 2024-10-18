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

#ifndef BPLIB_STOR_QM_ENCODE_INTERNAL_H
#define BPLIB_STOR_QM_ENCODE_INTERNAL_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_stor_qm_codec_internal.h"
#include "cbor.h"

typedef struct encode_state
{
    bool error;

    bool                    crc_flag;
    BPLib_CRC_Val_t             crc_val;
   BPLib_CRC_Parameters_t *crc_params;

    CborEncoder *cbor;

    size_t total_bytes_encoded;

    chunk_writer_func_t next_writer;
    void                  *next_writer_arg;
} encode_state_t;

typedef struct
{
    bundle_adminrectype_t                encode_rectype;
    const bundle_canonical_block_data_t *payload_data;

} admin_rec_payload_encode_info_t;

/*
 * Generic encode/decode container (aka CBOR array) helpers.
 * This handles the general process of encoding and decoding a container, specifically:
 *  - capture the parent state
 *  - create a new sub-state for the container
 *  - call the function with that new state
 *  - close out the sub-state after function returns
 *  - resume in the parent.
 */
typedef void (*encode_func_t)(encode_state_t *enc, const void *arg);

void encode_container(encode_state_t *enc, size_t entries, encode_func_t func, const void *arg);

/* Component encoders */
void encode_small_int(encode_state_t *enc, int val);
void encode_crc(encode_state_t *enc);
void encode_bundle_integer(encode_state_t *enc, const uint64_t *v);
void encode_bundle_blocknum(encode_state_t *enc, const bundle_blocknum_t *v);
void encode_bundle_blocktype(encode_state_t *enc, const BPLib_STOR_CACHE_Blocktype_t *v);
void encode_bundle_crctype(encode_state_t *enc, const BPLib_CRC_Type_t *v);
void encode_bundle_dtntime(encode_state_t *enc, const bundle_dtntime_t *v);
void encode_BPLib_STOR_CACHE_EidBuffer(encode_state_t *enc, const BPLib_STOR_CACHE_EidBuffer_t *v);
void encode_bitmap(encode_state_t *enc, const uint8_t *v, const bitmap_table_t *ptbl);

/* Block encoders */
void encode_bundle_primary_block(encode_state_t *enc, const bundle_primary_block_t *v);
void encode_bundle_admin_record_payload(encode_state_t *enc, const bundle_canonical_block_buffer_t *v);
void encode_bundle_previous_node_block(encode_state_t *enc, const bundle_previous_node_block_t *v);
void encode_bundle_bundle_age_block(encode_state_t *enc, const bundle_bundle_age_block_t *v);
void encode_bundle_hop_count_block(encode_state_t *enc, const bundle_hop_count_block_t *v);
void encode_bundle_custody_tracking_block(encode_state_t *enc, const bundle_custody_tracking_block_t *v);
void encode_bundle_custody_acknowledement_record(encode_state_t *enc, const BPLIB_CT_AcceptPayloadBlock_t *v);

void encode_bundle_canonical_bundle_block(encode_state_t *enc, const bundle_canonical_bundle_block_t *v,
                                         const canonical_block_info_t *info);
void encode_bundle_canonical_block_buffer(encode_state_t *enc, const bundle_canonical_block_buffer_t *v,
                                         const void *content_ptr, size_t content_length,
                                         size_t *content_encoded_offset);

int       encoder_mpstream_write(void *arg, const void *ptr, size_t sz);
int       encoder_write_crc(encode_state_t *enc);
CborError encoder_write_wrapper(void *arg, const void *ptr, size_t sz, CborEncoderAppendType at);
void      encode_bundle_adminrec_payload_impl(encode_state_t *enc, const void *arg);
void      encode_bundle_custody_acceptance_seqlist_impl(encode_state_t *enc, const void *arg);
void      encode_bundle_endpointid_scheme(encode_state_t *enc, const bundle_endpointid_scheme_t *v);
void      encode_bundle_ipn_nodenumber(encode_state_t *enc, const bundle_ipn_nodenumber_t *v);
void      encode_bundle_ipn_servicenumber(encode_state_t *enc, const bundle_ipn_servicenumber_t *v);
void      encode_bundle_ipn_uri_ssp(encode_state_t *enc, const bundle_ipn_uri_ssp_t *v);
void      encode_bundle_ipn_uri_ssp_impl(encode_state_t *enc, const void *arg);
void      encode_BPLib_STOR_CACHE_EidBuffer_impl(encode_state_t *enc, const void *arg);
void      encode_bundle_hop_count_block_impl(encode_state_t *enc, const void *arg);
void      encode_bundle_bundle_processing_control_flags(encode_state_t                          *enc,
                                                       const bundle_bundle_processing_control_flags_t *v);
void      encode_BPLib_STOR_CACHE_SequenceNumber(encode_state_t *enc, const BPLib_STOR_CACHE_SequenceNumber_t *v);
void      encode_bundle_creation_timestamp_impl(encode_state_t *enc, const void *arg);
void      encode_bundle_lifetime(encode_state_t *enc, const bundle_lifetime_t *v);
void      encode_bundle_adu_length(encode_state_t *enc, const bundle_adu_length_t *v);
void      encode_bundle_primary_block_impl(encode_state_t *enc, const void *arg);
void      encode_bundle_creation_timestamp(encode_state_t *enc, const bundle_creation_timestamp_t *v);

#endif /* BPLIB_STOR_QM_ENCODE_INTERNAL_H */
