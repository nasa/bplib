/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/*
 * Includes
 */
#include "test_bplib_v7.h"

void test_v7_encode_bp_hop_count_block_impl(void)
{
    /* Test function for:
     * void v7_encode_bp_hop_count_block_impl(v7_encode_state_t *enc, const void *arg)
     */
    v7_encode_state_t    enc;
    bp_hop_count_block_t arg;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&arg, 0, sizeof(bp_hop_count_block_t));

    enc.error = true;
    UtAssert_VOIDCALL(v7_encode_bp_hop_count_block_impl(&enc, &arg));
}

void test_v7_decode_bp_hop_count_block_impl(void)
{
    /* Test function for:
     * void v7_decode_bp_hop_count_block_impl(v7_decode_state_t *dec, void *arg)
     */
    v7_decode_state_t    dec;
    bp_hop_count_block_t arg;
    CborValue            cval;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&arg, 0, sizeof(bp_hop_count_block_t));
    memset(&cval, 0, sizeof(CborValue));

    dec.error = false;
    dec.cbor  = &cval;
    UtAssert_VOIDCALL(v7_decode_bp_hop_count_block_impl(&dec, &arg));
}

void test_v7_decode_bp_hop_count_block(void)
{
    /* Test function for:
     * void v7_decode_bp_hop_count_block(v7_decode_state_t *dec, bp_hop_count_block_t *v)
     */
    v7_decode_state_t    dec;
    bp_hop_count_block_t v;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_hop_count_block_t));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_hop_count_block(&dec, &v));
}

void TestV7BpHopCountBlock_Rgister(void)
{
    UtTest_Add(test_v7_encode_bp_hop_count_block_impl, NULL, NULL, "Test v7_encode_bp_hop_count_block_impl");
    UtTest_Add(test_v7_decode_bp_hop_count_block_impl, NULL, NULL, "Test v7_decode_bp_hop_count_block_impl");
    UtTest_Add(test_v7_decode_bp_hop_count_block, NULL, NULL, "Test v7_decode_bp_hop_count_block");
}
