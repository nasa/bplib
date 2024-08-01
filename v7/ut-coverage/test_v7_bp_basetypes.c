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

void test_v7_encode_small_int(void)
{
    /* Test function for:
     * void v7_encode_small_int(v7_encode_state_t *enc, int val)
     */
    v7_encode_state_t enc;
    int               val = 0;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    enc.error = false;

    //UT_SetDefaultReturnValue(UT_KEY(cbor_encode_int), CborUnknownError);
    UtAssert_VOIDCALL(v7_encode_small_int(&enc, val));
}

void test_v7_decode_small_int(void)
{
    /* Test function for:
     * int  v7_decode_small_int(v7_decode_state_t *dec)
     */
    v7_decode_state_t   dec;
    QCBORDecodeContext  cval;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&cval, 0, sizeof(QCBORDecodeContext));

    dec.error = false;
    dec.cbor  = &cval;
    UtAssert_VOIDCALL(v7_decode_small_int(&dec));

    dec.error      = false;
    UtAssert_VOIDCALL(v7_decode_small_int(&dec));
}

void test_v7_decode_bp_integer(void)
{
    /* Test function for:
     * void v7_decode_bp_integer(v7_decode_state_t *dec, bp_integer_t *v)
     */
    v7_decode_state_t   dec;
    bp_integer_t        v;
    QCBORDecodeContext  cval;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_integer_t));
    memset(&cval, 0, sizeof(QCBORDecodeContext));

    dec.error = false;
    dec.cbor  = &cval;
    UtAssert_VOIDCALL(v7_decode_bp_integer(&dec, &v));

    dec.error      = false;
    UtAssert_VOIDCALL(v7_decode_bp_integer(&dec, &v));
}

void test_v7_encode_bp_integer(void)
{
    /* Test function for:
     * void v7_encode_bp_integer(v7_encode_state_t *enc, const bp_integer_t *v)
     */
    v7_encode_state_t   enc;
    bp_integer_t        v;
    QCBOREncodeContext  cval;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&v, 0, sizeof(bp_integer_t));
    memset(&cval, 0, sizeof(QCBOREncodeContext));

    enc.error = false;
    enc.cbor  = &cval;
    UtAssert_VOIDCALL(v7_encode_bp_integer(&enc, &v));
    
}

void test_v7_decode_bp_blocknum(void)
{
    /* Test function for:
     * void v7_decode_bp_blocknum(v7_decode_state_t *dec, bp_blocknum_t *v)
     */
    v7_decode_state_t dec;
    bp_blocknum_t     v;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_blocknum_t));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_blocknum(&dec, &v));
}

void test_v7_decode_bp_blocktype(void)
{
    /* Test function for:
     * void v7_decode_bp_blocktype(v7_decode_state_t *dec, bp_blocktype_t *v)
     */
    v7_decode_state_t dec;
    bp_blocktype_t    v;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_blocktype_t));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_blocktype(&dec, &v));
}

void test_v7_decode_bp_crctype(void)
{
    /* Test function for:
     * void v7_decode_bp_crctype(v7_decode_state_t *dec, bp_crctype_t *v)
     */
    v7_decode_state_t dec;
    bp_crctype_t      v;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_crctype_t));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_crctype(&dec, &v));
}

void TestBpV7Basetypes_Register(void)
{
    UtTest_Add(test_v7_encode_small_int, NULL, NULL, "Test v7_encode_small_int");
    UtTest_Add(test_v7_decode_small_int, NULL, NULL, "Test v7_decode_small_int");
    UtTest_Add(test_v7_decode_bp_integer, NULL, NULL, "Test v7_decode_bp_integer");
    UtTest_Add(test_v7_encode_bp_integer, NULL, NULL, "Test v7_encode_bp_integer");
    UtTest_Add(test_v7_decode_bp_blocknum, NULL, NULL, "Test v7_decode_bp_blocknum");
    UtTest_Add(test_v7_decode_bp_blocktype, NULL, NULL, "Test v7_decode_bp_blocktype");
    UtTest_Add(test_v7_decode_bp_crctype, NULL, NULL, "Test v7_decode_bp_crctype");
}
