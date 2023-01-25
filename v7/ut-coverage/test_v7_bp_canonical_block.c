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

void test_v7_encode_bp_canonical_bundle_block(void)
{
    /* Test function for:
     * void v7_encode_bp_canonical_bundle_block(v7_encode_state_t *enc, const bp_canonical_bundle_block_t *v, const
     * v7_canonical_block_info_t *info)
     */
    v7_encode_state_t           enc;
    bp_canonical_bundle_block_t v;
    v7_canonical_block_info_t   info;
    size_t                      offset_out = 10;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&v, 0, sizeof(bp_canonical_bundle_block_t));
    memset(&info, 0, sizeof(v7_canonical_block_info_t));

    v.blockType             = bp_blocktype_adminRecordPayloadBlock;
    v.crctype               = bp_crctype_CRC16;
    enc.error               = false;
    info.content_offset_out = &offset_out;
    UT_SetDefaultReturnValue(UT_KEY(cbor_encode_int), CborNoError);
    UT_SetDefaultReturnValue(UT_KEY(cbor_encode_uint), CborNoError);
    UT_SetHandlerFunction(UT_KEY(bplib_crc_get_width), UT_V7_int8_Handler, NULL);
    UtAssert_VOIDCALL(v7_encode_bp_canonical_bundle_block(&enc, &v, &info));
}

void test_v7_encode_bp_canonical_block_buffer(void)
{
    /* Test function for:
     * void v7_encode_bp_canonical_block_buffer(v7_encode_state_t *enc, const bp_canonical_block_buffer_t *v, const void
     * *content_ptr, size_t content_length, size_t *content_encoded_offset)
     */
    v7_encode_state_t           enc;
    bp_canonical_block_buffer_t v;
    void                       *content_ptr            = NULL;
    size_t                      content_length         = 100;
    size_t                      content_encoded_offset = 10;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&v, 0, sizeof(bp_canonical_block_buffer_t));

    v.canonical_block.crctype = bp_crctype_CRC16;
    UtAssert_VOIDCALL(
        v7_encode_bp_canonical_block_buffer(&enc, &v, content_ptr, content_length, &content_encoded_offset));
}

void test_v7_decode_bp_block_processing_flags(void)
{
    /* Test function for:
     * void v7_decode_bp_block_processing_flags(v7_decode_state_t *dec, bp_block_processing_flags_t *v)
     */
    v7_decode_state_t           dec;
    bp_block_processing_flags_t v;
    CborValue                   cval;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_block_processing_flags_t));
    memset(&cval, 0, sizeof(CborValue));

    dec.cbor = &cval;
    UtAssert_VOIDCALL(v7_decode_bp_block_processing_flags(&dec, &v));
}

void test_v7_decode_bp_canonical_bundle_block(void)
{
    /* Test function for:
     * void v7_decode_bp_canonical_bundle_block(v7_decode_state_t *dec, bp_canonical_bundle_block_t *v,
     * v7_canonical_block_info_t *info)
     */
    v7_decode_state_t           dec;
    bp_canonical_bundle_block_t v;
    v7_canonical_block_info_t   info;
    CborValue                   cval;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_canonical_bundle_block_t));
    memset(&info, 0, sizeof(v7_canonical_block_info_t));
    memset(&cval, 0, sizeof(CborValue));

    dec.error      = false;
    dec.cbor       = &cval;
    cval.remaining = 10;
    cval.extra     = bp_crctype_CRC16;
    UT_SetDefaultReturnValue(UT_KEY(cbor_value_advance_fixed), CborNoError);
    UtAssert_VOIDCALL(v7_decode_bp_canonical_bundle_block(&dec, &v, &info));
}

void test_v7_decode_bp_canonical_block_buffer_impl(void)
{
    /* Test function for:
     * void v7_decode_bp_canonical_block_buffer_impl(v7_decode_state_t *dec, void *arg)
     */
    v7_decode_state_t           dec;
    v7_canonical_block_info_t   arg;
    CborValue                   cval;
    bp_canonical_bundle_block_t bblk;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&arg, 0, sizeof(v7_canonical_block_info_t));
    memset(&cval, 0, sizeof(CborValue));
    memset(&bblk, 0, sizeof(bp_canonical_bundle_block_t));
    dec.cbor         = &cval;
    arg.decode_block = &bblk;

    UtAssert_VOIDCALL(v7_decode_bp_canonical_block_buffer_impl(&dec, &arg));
}

void TestBpV7CanonicalBlock_Rgister(void)
{
    UtTest_Add(test_v7_encode_bp_canonical_bundle_block, NULL, NULL, "Test v7_encode_bp_canonical_bundle_block");
    UtTest_Add(test_v7_encode_bp_canonical_block_buffer, NULL, NULL, "Test v7_encode_bp_canonical_block_buffer");
    UtTest_Add(test_v7_decode_bp_block_processing_flags, NULL, NULL, "Test v7_decode_bp_block_processing_flags");
    UtTest_Add(test_v7_decode_bp_canonical_bundle_block, NULL, NULL, "Test v7_decode_bp_canonical_bundle_block");
    UtTest_Add(test_v7_decode_bp_canonical_block_buffer_impl, NULL, NULL,
               "Test v7_decode_bp_canonical_block_buffer_impl");
}