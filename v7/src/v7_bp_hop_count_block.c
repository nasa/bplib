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

typedef enum bp_hop_count_field
{
    bp_hop_count_field_undef,
    bp_hop_count_field_limit,
    bp_hop_count_field_count,
    bp_hop_count_field_done
} bp_hop_count_field_t;

/*
 * -----------------------------------------------------------------------------------
 * IMPLEMENTATION
 * Helpers for encoding/decoding of individual protocol elements
 * -----------------------------------------------------------------------------------
 */

void v7_encode_bp_hop_count_block_impl(v7_encode_state_t *enc, const void *arg)
{
    const bp_hop_count_block_t *v        = arg;
    bp_hop_count_field_t        field_id = bp_hop_count_field_undef;

    while (field_id < bp_hop_count_field_done && !enc->error)
    {
        switch (field_id)
        {
            case bp_hop_count_field_limit:
                v7_encode_bp_integer(enc, &v->hopLimit);
                break;
            case bp_hop_count_field_count:
                v7_encode_bp_integer(enc, &v->hopCount);
                break;
            default:
                break;
        }

        ++field_id;
    }
}

void v7_encode_bp_hop_count_block(v7_encode_state_t *enc, const bp_hop_count_block_t *v)
{
    v7_encode_container(enc, 2, v7_encode_bp_hop_count_block_impl, v);
}

void v7_decode_bp_hop_count_block_impl(v7_decode_state_t *dec, void *arg)
{
    bp_hop_count_block_t *v        = arg;
    bp_hop_count_field_t  field_id = bp_hop_count_field_undef;

    while (field_id < bp_hop_count_field_done && !dec->error && !cbor_value_at_end(dec->cbor))
    {
        switch (field_id)
        {
            case bp_hop_count_field_limit:
                v7_decode_bp_integer(dec, &v->hopLimit);
                break;
            case bp_hop_count_field_count:
                v7_decode_bp_integer(dec, &v->hopCount);
                break;
            default:
                break;
        }

        ++field_id;
    }
}

void v7_decode_bp_hop_count_block(v7_decode_state_t *dec, bp_hop_count_block_t *v)
{
    v7_decode_container(dec, 2, v7_decode_bp_hop_count_block_impl, v);
}
