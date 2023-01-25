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

void test_v7_encode_container(void)
{
    /* Test function for:
     * void v7_encode_container(v7_encode_state_t *enc, size_t entries, v7_encode_func_t func, const void *arg)
     */
    v7_encode_state_t enc;
    size_t            entries = 1;
    v7_encode_func_t  func    = test_bplib_v7_encode_func_stub;
    void             *arg     = NULL;
    CborEncoder       cval;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&cval, 0, sizeof(CborEncoder));

    enc.cbor  = &cval;
    enc.error = false;
    UT_SetDefaultReturnValue(UT_KEY(cbor_encoder_close_container), CborUnknownError);
    UtAssert_VOIDCALL(v7_encode_container(&enc, entries, func, &arg));

    enc.error = false;
    UT_SetDefaultReturnValue(UT_KEY(cbor_encoder_create_array), CborUnknownError);
    UtAssert_VOIDCALL(v7_encode_container(&enc, entries, func, &arg));
}

void test_v7_decode_container(void)
{
    /* Test function for:
     * void v7_decode_container(v7_decode_state_t *dec, size_t entries, v7_decode_func_t func, void *arg)
     */
    v7_decode_state_t dec;
    size_t            entries = 1;
    v7_decode_func_t  func    = test_bplib_v7_decode_func_stub;
    void             *arg     = NULL;
    CborValue         cval;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&cval, 0, sizeof(CborValue));

    dec.cbor  = &cval;
    dec.error = false;
    UtAssert_VOIDCALL(v7_decode_container(&dec, entries, func, &arg));

    dec.error      = false;
    cval.remaining = 10;
    cval.type      = CborArrayType;
    UT_SetDefaultReturnValue(UT_KEY(cbor_value_enter_container), CborUnknownError);
    UtAssert_VOIDCALL(v7_decode_container(&dec, entries, func, &arg));

    dec.error = false;
    UT_SetDefaultReturnValue(UT_KEY(cbor_value_enter_container), CborNoError);
    UT_SetDefaultReturnValue(UT_KEY(cbor_value_leave_container), CborUnknownError);
    UtAssert_VOIDCALL(v7_decode_container(&dec, entries, func, &arg));
}

void TestV7BpContainer_Rgister(void)
{
    UtTest_Add(test_v7_encode_container, NULL, NULL, "Test v7_encode_container");
    UtTest_Add(test_v7_decode_container, NULL, NULL, "Test v7_decode_container");
}
