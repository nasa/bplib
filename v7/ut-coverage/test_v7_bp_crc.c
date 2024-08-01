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

void test_v7_encode_crc(void)
{
    /* Test function for:
     * void v7_encode_crc(v7_encode_state_t *enc)
     */
    v7_encode_state_t enc;
    QCBOREncodeContext  cbor;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&cbor, 0, sizeof(QCBOREncodeContext));
    
    enc.cbor = &cbor;

    UT_SetHandlerFunction(UT_KEY(bplib_crc_get_width), UT_V7_uint8_Handler, NULL);
    UT_SetDefaultReturnValue(UT_KEY(bplib_crc_get_width), 32);
    UtAssert_VOIDCALL(v7_encode_crc(&enc));

    UT_SetHandlerFunction(UT_KEY(bplib_crc_get_width), UT_V7_uint8_Handler, NULL);
    UT_SetDefaultReturnValue(UT_KEY(bplib_crc_get_width), 40);
    UtAssert_VOIDCALL(v7_encode_crc(&enc));
}

void test_v7_decode_crc(void)
{
    /* Test function for:
     * void v7_decode_crc(v7_decode_state_t *dec, bp_crcval_t *v)
     */
    v7_decode_state_t   dec;
    bp_crcval_t         v;
    QCBORDecodeContext  cval;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_crcval_t));
    memset(&cval, 0, sizeof(QCBORDecodeContext));
    dec.error      = false;
    dec.cbor       = &cval;

    UT_SetHandlerFunction(UT_KEY(bplib_crc_get_width), UT_V7_uint8_Handler, NULL);
    UT_SetDefaultReturnValue(UT_KEY(bplib_crc_get_width), 40);
    UtAssert_VOIDCALL(v7_decode_crc(&dec, &v));

}

void TestV7BpCrc_Rgister(void)
{
    UtTest_Add(test_v7_encode_crc, NULL, NULL, "Test v7_encode_crc");
    UtTest_Add(test_v7_decode_crc, NULL, NULL, "Test v7_decode_crc");
}
