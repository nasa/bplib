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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "v7_decode_internal.h"
#include "v7_encode_internal.h"

/*
 * -----------------------------------------------------------------------------------
 * IMPLEMENTATION
 * Helpers for encoding/decoding of individual protocol elements
 * -----------------------------------------------------------------------------------
 */

void v7_encode_bp_custody_acceptance_seqlist_impl(v7_encode_state_t *enc, const void *arg)
{
    const bp_custody_accept_payload_block_t *v = arg;
    bp_integer_t                             n;

    for (n = 0; n < v->num_entries && !enc->error; ++n)
    {
        v7_encode_bp_integer(enc, &v->sequence_nums[n]);
    }
}

static void v7_encode_bp_custody_acknowledement_record_impl(v7_encode_state_t *enc, const void *arg)
{
    const bp_custody_accept_payload_block_t *v = arg;

    v7_encode_bp_endpointid_buffer(enc, &v->flow_source_eid);
    v7_encode_container(enc, v->num_entries, v7_encode_bp_custody_acceptance_seqlist_impl, v);
}

void v7_encode_bp_custody_acknowledement_record(v7_encode_state_t *enc, const bp_custody_accept_payload_block_t *v)
{
    v7_encode_container(enc, 2, v7_encode_bp_custody_acknowledement_record_impl, v);
}

void v7_decode_bp_custody_acceptance_seqlist_impl(v7_decode_state_t *dec, void *arg)
{
    bp_custody_accept_payload_block_t *v = arg;
    
    while (v->num_entries < BP_DACS_MAX_SEQ_PER_PAYLOAD)
    {
        v7_decode_bp_integer(dec, &v->sequence_nums[v->num_entries]);
        if (dec->error)
        {
            break;
        }

        ++v->num_entries;
    }
}

void v7_decode_bp_custody_acknowledement_record_impl(v7_decode_state_t *dec, void *arg)
{
    bp_custody_accept_payload_block_t *v = arg;

    v7_decode_bp_endpointid_buffer(dec, &v->flow_source_eid);
    v7_decode_container(dec, QCBOR_MAX_ITEMS_IN_ARRAY, v7_decode_bp_custody_acceptance_seqlist_impl, v);
}

void v7_decode_bp_custody_acknowledement_record(v7_decode_state_t *dec, bp_custody_accept_payload_block_t *v)
{
    v7_decode_container(dec, 2, v7_decode_bp_custody_acknowledement_record_impl, v);
}
