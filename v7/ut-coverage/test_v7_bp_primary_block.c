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

void test_v7_encode_bp_bundle_processing_control_flags(void)
{
    /* Test function for:
     * void v7_encode_bp_bundle_processing_control_flags(v7_encode_state_t *enc, const
     * bp_bundle_processing_control_flags_t *v)
     */
    v7_encode_state_t                    enc;
    bp_bundle_processing_control_flags_t v;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&v, 0, sizeof(bp_bundle_processing_control_flags_t));

    enc.error = true;
    UtAssert_VOIDCALL(v7_encode_bp_bundle_processing_control_flags(&enc, &v));
}

void test_v7_encode_bp_sequencenumber(void)
{
    /* Test function for:
     * void v7_encode_bp_sequencenumber(v7_encode_state_t *enc, const bp_sequencenumber_t *v)
     */
    v7_encode_state_t   enc;
    bp_sequencenumber_t v;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&v, 0, sizeof(bp_sequencenumber_t));

    enc.error = true;
    UtAssert_VOIDCALL(v7_encode_bp_sequencenumber(&enc, &v));
}

void test_v7_encode_bp_creation_timestamp_impl(void)
{
    /* Test function for:
     * void v7_encode_bp_creation_timestamp_impl(v7_encode_state_t *enc, const void *arg)
     */
    v7_encode_state_t       enc;
    bp_creation_timestamp_t arg;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&arg, 0, sizeof(bp_creation_timestamp_t));

    enc.error = true;
    UtAssert_VOIDCALL(v7_encode_bp_creation_timestamp_impl(&enc, &arg));
}

void test_v7_encode_bp_creation_timestamp(void)
{
    /* Test function for:
     * void v7_encode_bp_creation_timestamp(v7_encode_state_t *enc, const bp_creation_timestamp_t *v)
     */
    v7_encode_state_t       enc;
    bp_creation_timestamp_t v;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&v, 0, sizeof(bp_creation_timestamp_t));

    enc.error = true;
    UtAssert_VOIDCALL(v7_encode_bp_creation_timestamp(&enc, &v));
}

void test_v7_encode_bp_lifetime(void)
{
    /* Test function for:
     * void v7_encode_bp_lifetime(v7_encode_state_t *enc, const bp_lifetime_t *v)
     */
    v7_encode_state_t enc;
    bp_lifetime_t     v;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&v, 0, sizeof(bp_lifetime_t));

    enc.error = true;
    UtAssert_VOIDCALL(v7_encode_bp_lifetime(&enc, &v));
}

void test_v7_encode_bp_adu_length(void)
{
    /* Test function for:
     * void v7_encode_bp_adu_length(v7_encode_state_t *enc, const bp_adu_length_t *v)
     */
    v7_encode_state_t enc;
    bp_adu_length_t   v;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&v, 0, sizeof(bp_adu_length_t));

    enc.error = true;
    UtAssert_VOIDCALL(v7_encode_bp_adu_length(&enc, &v));
}

void test_v7_encode_bp_primary_block_impl(void)
{
    /* Test function for:
     * void v7_encode_bp_primary_block_impl(v7_encode_state_t *enc, const void *arg)
     */
    v7_encode_state_t  enc;
    bp_primary_block_t v;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&v, 0, sizeof(bp_primary_block_t));

    enc.error = true;
    UtAssert_VOIDCALL(v7_encode_bp_primary_block_impl(&enc, &v));

    v.version                 = 7;
    v.controlFlags.isFragment = true;
    v.crctype                 = bp_crctype_CRC16;
    UtAssert_VOIDCALL(v7_encode_bp_primary_block_impl(&enc, &v));
}

void test_v7_decode_bp_sequencenumber(void)
{
    /* Test function for:
     * void v7_decode_bp_sequencenumber(v7_decode_state_t *dec, bp_sequencenumber_t *v)
     */
    v7_decode_state_t   dec;
    bp_sequencenumber_t v;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_sequencenumber_t));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_sequencenumber(&dec, &v));
}

void test_v7_decode_bp_creation_timestamp_impl(void)
{
    /* Test function for:
     * void v7_decode_bp_creation_timestamp_impl(v7_decode_state_t *dec, void *arg)
     */

    v7_decode_state_t       dec;
    bp_creation_timestamp_t arg;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&arg, 0, sizeof(bp_creation_timestamp_t));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_creation_timestamp_impl(&dec, &arg));
}

void test_v7_decode_bp_creation_timestamp(void)
{
    /* Test function for:
     * void v7_decode_bp_creation_timestamp(v7_decode_state_t *dec, bp_creation_timestamp_t *v)
     */

    v7_decode_state_t       dec;
    bp_creation_timestamp_t v;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_creation_timestamp_t));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_creation_timestamp(&dec, &v));
}

void test_v7_decode_bp_bundle_processing_control_flags(void)
{
    /* Test function for:
     * void v7_decode_bp_bundle_processing_control_flags(v7_decode_state_t *dec, bp_bundle_processing_control_flags_t
     * *v)
     */

    v7_decode_state_t                    dec;
    bp_bundle_processing_control_flags_t v;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_bundle_processing_control_flags_t));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_bundle_processing_control_flags(&dec, &v));
}

void test_v7_decode_bp_lifetime(void)
{
    /* Test function for:
     * void v7_decode_bp_lifetime(v7_decode_state_t *dec, bp_lifetime_t *v)
     */

    v7_decode_state_t dec;
    bp_lifetime_t     v;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_lifetime_t));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_lifetime(&dec, &v));
}

void test_v7_decode_bp_adu_length(void)
{
    /* Test function for:
     * void v7_decode_bp_adu_length(v7_decode_state_t *dec, bp_adu_length_t *v)
     */

    v7_decode_state_t dec;
    bp_adu_length_t   v;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_adu_length_t));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_adu_length(&dec, &v));
}

void test_v7_decode_bp_primary_block_impl(void)
{
    /* Test function for:
     * void v7_decode_bp_primary_block_impl(v7_decode_state_t *dec, void *arg)
     */

    v7_decode_state_t  dec;
    bp_primary_block_t arg;
    QCBORDecodeContext cval;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&arg, 0, sizeof(bp_primary_block_t));
    memset(&cval, 0, sizeof(QCBORDecodeContext));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_primary_block_impl(&dec, &arg));

    dec.error      = false;
    arg.version    = 7;
    dec.cbor       = &cval;
    UtAssert_VOIDCALL(v7_decode_bp_primary_block_impl(&dec, &arg));

    dec.error                   = false;
    arg.controlFlags.isFragment = false;
    UtAssert_VOIDCALL(v7_decode_bp_primary_block_impl(&dec, &arg));
}

void TestV7BpPrimaryBlock_Rgister(void)
{
    UtTest_Add(test_v7_encode_bp_bundle_processing_control_flags, NULL, NULL,
               "Test v7_encode_bp_bundle_processing_control_flags");
    UtTest_Add(test_v7_encode_bp_sequencenumber, NULL, NULL, "Test v7_encode_bp_sequencenumber");
    UtTest_Add(test_v7_encode_bp_creation_timestamp_impl, NULL, NULL, "Test v7_encode_bp_creation_timestamp_impl");
    UtTest_Add(test_v7_encode_bp_creation_timestamp, NULL, NULL, "Test v7_encode_bp_creation_timestamp");
    UtTest_Add(test_v7_encode_bp_lifetime, NULL, NULL, "Test v7_encode_bp_lifetime");
    UtTest_Add(test_v7_encode_bp_adu_length, NULL, NULL, "Test v7_encode_bp_adu_length");
    UtTest_Add(test_v7_encode_bp_primary_block_impl, NULL, NULL, "Test v7_encode_bp_primary_block_impl");
    UtTest_Add(test_v7_decode_bp_sequencenumber, NULL, NULL, "Test v7_decode_bp_sequencenumber");
    UtTest_Add(test_v7_decode_bp_creation_timestamp_impl, NULL, NULL, "Test v7_decode_bp_creation_timestamp_impl");
    UtTest_Add(test_v7_decode_bp_creation_timestamp, NULL, NULL, "Test v7_decode_bp_creation_timestamp");
    UtTest_Add(test_v7_decode_bp_bundle_processing_control_flags, NULL, NULL,
               "Test v7_decode_bp_bundle_processing_control_flags");
    UtTest_Add(test_v7_decode_bp_lifetime, NULL, NULL, "Test v7_decode_bp_lifetime");
    UtTest_Add(test_v7_decode_bp_adu_length, NULL, NULL, "Test v7_decode_bp_adu_length");
    UtTest_Add(test_v7_decode_bp_primary_block_impl, NULL, NULL, "Test v7_decode_bp_primary_block_impl");
}
