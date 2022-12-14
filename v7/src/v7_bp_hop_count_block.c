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

static void v7_encode_bp_hop_count_block_impl(v7_encode_state_t *enc, const void *arg)
{
    const bp_hop_count_block_t *v = arg;

    v7_encode_bp_integer(enc, &v->hopLimit);
    v7_encode_bp_integer(enc, &v->hopCount);
}

void v7_encode_bp_hop_count_block(v7_encode_state_t *enc, const bp_hop_count_block_t *v)
{
    v7_encode_container(enc, 2, v7_encode_bp_hop_count_block_impl, v);
}

static void v7_decode_bp_hop_count_block_impl(v7_decode_state_t *dec, void *arg)
{
    bp_hop_count_block_t *v = arg;

    v7_decode_bp_integer(dec, &v->hopLimit);
    v7_decode_bp_integer(dec, &v->hopCount);
}

void v7_decode_bp_hop_count_block(v7_decode_state_t *dec, bp_hop_count_block_t *v)
{
    v7_decode_container(dec, 2, v7_decode_bp_hop_count_block_impl, v);
}
