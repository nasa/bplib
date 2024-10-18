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

#ifndef BPLIB_STOR_QM_DECODE_INTERNAL
#define BPLIB_STOR_QM_DECODE_INTERNAL

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_stor_qm_codec_internal.h"
#include "cbor.h"

typedef struct decode_state
{
    bool           error;
    const uint8_t *base;
    CborValue     *cbor;
} decode_state_t;

typedef struct
{
    bundle_adminrectype_t          decode_rectype;
    bundle_canonical_block_data_t *payload_data;

} admin_rec_payload_decode_info_t;

/*
 * Generic encode/decode container (aka CBOR array) helpers.
 * This handles the general process of encoding and decoding a container, specifically:
 *  - capture the parent state
 *  - create a new sub-state for the container
 *  - call the function with that new state
 *  - close out the sub-state after function returns
 *  - resume in the parent.
 */
typedef void (*decode_func_t)(decode_state_t *dec, void *arg);

void decode_container(decode_state_t *dec, size_t entries, decode_func_t func, void *arg);

/* Component decoders */
int  decode_small_int(decode_state_t *dec);
void decode_crc(decode_state_t *dec, BPLib_CRC_Val_t *v);

void decode_bitmap(decode_state_t *dec, uint8_t *v, const bitmap_table_t *ptbl);
void decode_bundle_blocknum(decode_state_t *dec, bundle_blocknum_t *v);
void decode_bundle_blocktype(decode_state_t *dec, BPLib_STOR_CACHE_Blocktype_t *v);
void decode_bundle_integer(decode_state_t *dec, uint64_t *v);
void decode_bundle_dtntime(decode_state_t *dec, bundle_dtntime_t *v);
void decode_bundle_crctype(decode_state_t *dec, BPLib_CRC_Type_t *v);
void decode_BPLib_STOR_CACHE_EidBuffer(decode_state_t *dec, BPLib_STOR_CACHE_EidBuffer_t *v);

/* Block decoders */
void decode_bundle_primary_block(decode_state_t *dec, bundle_primary_block_t *v);
void decode_bundle_admin_record_payload(decode_state_t *dec, bundle_canonical_block_buffer_t *v);
void decode_bundle_previous_node_block(decode_state_t *dec, bundle_previous_node_block_t *v);
void decode_bundle_bundle_age_block(decode_state_t *dec, bundle_bundle_age_block_t *v);
void decode_bundle_hop_count_block(decode_state_t *dec, bundle_hop_count_block_t *v);
void decode_bundle_custody_tracking_block(decode_state_t *dec, bundle_custody_tracking_block_t *v);
void decode_bundle_custody_acknowledement_record(decode_state_t *dec, BPLIB_CT_AcceptPayloadBlock_t *v);

void   decode_bundle_canonical_bundle_block(decode_state_t *dec, bundle_canonical_bundle_block_t *v,
                                           canonical_block_info_t *info);
void   decode_bundle_canonical_block_buffer(decode_state_t *dec, bundle_canonical_block_buffer_t *v,
                                           size_t *content_encoded_offset, size_t *content_length);
size_t save_and_verify_block(BPLib_MEM_Block_t *head, const uint8_t *block_base, size_t block_size,
                                BPLib_CRC_Type_t crc_type, BPLib_CRC_Val_t crc_check);
void   decode_bundle_adminrec_payload_impl(decode_state_t *dec, void *arg);
void   decode_bundle_block_processing_flags(decode_state_t *dec, bundle_block_processing_flags_t *v);
void   decode_bundle_endpointid_scheme(decode_state_t *dec, bundle_endpointid_scheme_t *v);
void   decode_bundle_ipn_nodenumber(decode_state_t *dec, bundle_ipn_nodenumber_t *v);
void   decode_bundle_ipn_servicenumber(decode_state_t *dec, bundle_ipn_servicenumber_t *v);
void   decode_bundle_ipn_uri_ssp(decode_state_t *dec, bundle_ipn_uri_ssp_t *v);
void   decode_bundle_ipn_uri_ssp_impl(decode_state_t *dec, void *arg);
void   decode_BPLib_STOR_CACHE_EidBuffer_impl(decode_state_t *dec, void *arg);
void   decode_bundle_hop_count_block_impl(decode_state_t *dec, void *arg);
void   decode_BPLib_STOR_CACHE_SequenceNumber(decode_state_t *dec, BPLib_STOR_CACHE_SequenceNumber_t *v);
void   decode_bundle_creation_timestamp_impl(decode_state_t *dec, void *arg);
void   decode_bundle_creation_timestamp(decode_state_t *dec, bundle_creation_timestamp_t *v);
void   decode_bundle_bundle_processing_control_flags(decode_state_t *dec, bundle_bundle_processing_control_flags_t *v);
void   decode_bundle_lifetime(decode_state_t *dec, bundle_lifetime_t *v);
void   decode_bundle_adu_length(decode_state_t *dec, bundle_adu_length_t *v);
void   decode_bundle_primary_block_impl(decode_state_t *dec, void *arg);
void   decode_bundle_custody_acceptance_seqlist_impl(decode_state_t *dec, void *arg);
void   decode_bundle_custody_acknowledement_record_impl(decode_state_t *dec, void *arg);
void   decode_bundle_canonical_block_buffer_impl(decode_state_t *dec, void *arg);

#endif /* BPLIB_STOR_QM_DECODE_INTERNAL */
