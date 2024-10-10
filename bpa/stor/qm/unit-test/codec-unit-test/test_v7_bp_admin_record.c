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

void test_v7_encode_bp_adminrec_payload_impl(void)
{
    /* Test function for:
     * void v7_encode_bp_adminrec_payload_impl(v7_encode_state_t *enc, const void *arg)
     */
    v7_encode_state_t                  enc;
    v7_admin_rec_payload_encode_info_t admin_rec_payload;
    bp_canonical_block_data_t          data;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&admin_rec_payload, 0, sizeof(v7_admin_rec_payload_encode_info_t));
    memset(&data, 0, sizeof(bp_canonical_block_data_t));

    enc.error                        = false;
    admin_rec_payload.encode_rectype = bp_adminrectype_custodyAcknowledgement;
    admin_rec_payload.payload_data   = &data;
    UtAssert_VOIDCALL(v7_encode_bp_adminrec_payload_impl(&enc, &admin_rec_payload));

    enc.error                        = false;
    admin_rec_payload.encode_rectype = bp_adminrectype_undefined;
    UtAssert_VOIDCALL(v7_encode_bp_adminrec_payload_impl(&enc, &admin_rec_payload));
}

void test_v7_decode_bp_adminrec_payload_impl(void)
{
    /* Test function for:
     * void v7_decode_bp_adminrec_payload_impl(v7_decode_state_t *dec, void *arg)
     */
    v7_decode_state_t                  dec;
    v7_admin_rec_payload_decode_info_t admin_rec_payload;
    CborValue                          cval;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&admin_rec_payload, 0, sizeof(v7_admin_rec_payload_decode_info_t));
    memset(&cval, 0, sizeof(CborValue));
    admin_rec_payload.decode_rectype = bp_adminrectype_custodyAcknowledgement;
    cval.remaining                   = 10;
    dec.cbor                         = &cval;
    UtAssert_VOIDCALL(v7_decode_bp_adminrec_payload_impl(&dec, &admin_rec_payload));

    dec.error  = false;
    cval.extra = bp_adminrectype_custodyAcknowledgement;
    UtAssert_VOIDCALL(v7_decode_bp_adminrec_payload_impl(&dec, &admin_rec_payload));
}

void test_v7_decode_bp_admin_record_payload(void)
{
    /* Test function for:
     * void v7_decode_bp_admin_record_payload(v7_decode_state_t *dec, bp_canonical_block_buffer_t *v)
     */
    v7_decode_state_t           dec;
    bp_canonical_block_buffer_t v;
    CborValue                   cval;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(v7_decode_state_t));
    memset(&cval, 0, sizeof(CborValue));
    cval.type                   = CborArrayType;
    dec.cbor                    = &cval;
    dec.cbor->remaining         = 10;
    dec.error                   = false;
    v.canonical_block.blockType = CborArrayType;
    cval.extra                  = bp_adminrectype_custodyAcknowledgement;
    UtAssert_VOIDCALL(v7_decode_bp_admin_record_payload(&dec, &v));
}

void TestBpV7AdminRecord_Register(void)
{
    UtTest_Add(test_v7_encode_bp_adminrec_payload_impl, NULL, NULL, "Test v7_encode_bp_adminrec_payload_impl");
    UtTest_Add(test_v7_decode_bp_adminrec_payload_impl, NULL, NULL, "Test v7_decode_bp_adminrec_payload_impl");
    UtTest_Add(test_v7_decode_bp_admin_record_payload, NULL, NULL, "Test v7_decode_bp_admin_record_payload");
}
