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

void test_v7_encode_bitmap(void)
{
    /* Test function for:
     * void v7_encode_bitmap(v7_encode_state_t *enc, const uint8_t *v, const v7_bitmap_table_t *ptbl)
     */
    v7_encode_state_t enc;
    uint8_t           v[5];
    v7_bitmap_table_t ptbl[2] = {{0}, {0}};

    memset(&enc, 0, sizeof(v7_decode_state_t));
    enc.error    = true;
    ptbl[0].mask = 1;

    UtAssert_VOIDCALL(v7_encode_bitmap(&enc, v, ptbl));
}

void test_v7_decode_bitmap(void)
{
    /* Test function for:
     * void v7_decode_bitmap(v7_decode_state_t *dec, uint8_t *v, const v7_bitmap_table_t *ptbl)
     */
    v7_decode_state_t dec;
    uint8_t           v;
    v7_bitmap_table_t ptbl[2] = {{0}, {0}};

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(uint8_t));
    dec.error    = true;
    ptbl[0].mask = 1;

    UtAssert_VOIDCALL(v7_decode_bitmap(&dec, &v, ptbl));
}

void TestBpV7Bitmap_Register(void)
{
    UtTest_Add(test_v7_encode_bitmap, NULL, NULL, "Test v7_decode_bitmap");
    UtTest_Add(test_v7_decode_bitmap, NULL, NULL, "Test v7_decode_bitmap");
}
