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
#include "test_bplib_mem_common.h"
#include "bplib_crc_private.h"

#define UT_BPLIB_CRC6_POLY 0x3U

static bp_crcval_t UT_bplib_crc_digest_CRC6_ITU(bp_crcval_t crc, const void *ptr, size_t size);

/*
 * Define a Test CRC with a length that is not a multiple of 8.
 * In particular this catches the "leftover bits" case of value reflection.
 *
 * The polynomial here is from the definition of CRC-6/ITU, which reflects
 * the bits in the output (this is important).  A "real-world" CRC was chosen
 * here so the published check value should match and therefore confirm that the
 * implementation was done currectly.
 */
bplib_crc_parameters_t UT_BPLIB_CRC6 = {.name                  = "CRC-6/ITU",
                                        .length                = 6,
                                        .should_reflect_output = true,
                                        .digest                = UT_bplib_crc_digest_CRC6_ITU,
                                        .initial_value         = 0x0,
                                        .final_xor             = 0x0};

static uint8_t UT_BPLIB_CRC6_ITU_TABLE[256];

bp_crcval_t UT_bplib_crc_digest_CRC6_ITU(bp_crcval_t crc, const void *ptr, size_t size)
{
    const uint8_t *byte = ptr;

    while (size > 0)
    {
        crc = UT_BPLIB_CRC6_ITU_TABLE[((crc << 2) ^ BPLIB_CRC_REFLECT_TABLE[*byte]) & 0xFF];
        ++byte;
        --size;
    }

    return crc;
}

void Test_bplib_crc_get_name(void)
{
    /* Test function for:
     * const char *bplib_crc_get_name(bplib_crc_parameters_t *params);
     */
    const char *cn;

    UtAssert_NOT_NULL(cn = bplib_crc_get_name(&BPLIB_CRC32_CASTAGNOLI));
    UtAssert_STRINGBUF_EQ(cn, UTASSERT_STRINGBUF_NULL_TERM, "CRC-32 Castagnoli", UTASSERT_STRINGBUF_NULL_TERM);

    UtAssert_NOT_NULL(cn = bplib_crc_get_name(&BPLIB_CRC16_X25));
    UtAssert_STRINGBUF_EQ(cn, UTASSERT_STRINGBUF_NULL_TERM, "CRC-16 X25", UTASSERT_STRINGBUF_NULL_TERM);

    UtAssert_NOT_NULL(cn = bplib_crc_get_name(&BPLIB_CRC_NONE));
    UtAssert_STRINGBUF_EQ(cn, UTASSERT_STRINGBUF_NULL_TERM, "No CRC", UTASSERT_STRINGBUF_NULL_TERM);

    UtAssert_NOT_NULL(cn = bplib_crc_get_name(&UT_BPLIB_CRC6));
    UtAssert_STRINGBUF_EQ(cn, UTASSERT_STRINGBUF_NULL_TERM, "CRC-6/ITU", UTASSERT_STRINGBUF_NULL_TERM);
}

void Test_bplib_crc_get_width(void)
{
    /* Test function for:
     * uint8_t     bplib_crc_get_width(bplib_crc_parameters_t *params);
     */

    UtAssert_UINT32_EQ(bplib_crc_get_width(&BPLIB_CRC32_CASTAGNOLI), 32);
    UtAssert_UINT32_EQ(bplib_crc_get_width(&BPLIB_CRC16_X25), 16);
    UtAssert_UINT32_EQ(bplib_crc_get_width(&BPLIB_CRC_NONE), 0);

    UtAssert_UINT32_EQ(bplib_crc_get_width(&UT_BPLIB_CRC6), 6);
}

void Test_bplib_crc_initial_value(void)
{
    /* Test function for:
     * bp_crcval_t bplib_crc_initial_value(bplib_crc_parameters_t *params);
     */

    UtAssert_UINT32_EQ(bplib_crc_initial_value(&BPLIB_CRC32_CASTAGNOLI), 0xffffffff);
    UtAssert_UINT32_EQ(bplib_crc_initial_value(&BPLIB_CRC16_X25), 0xffff);
    UtAssert_UINT32_EQ(bplib_crc_initial_value(&BPLIB_CRC_NONE), 0);

    UtAssert_UINT32_EQ(bplib_crc_initial_value(&UT_BPLIB_CRC6), 0);
}

void Test_bplib_crc_update(void)
{
    /* Test function for:
     * bp_crcval_t bplib_crc_update(bplib_crc_parameters_t *params, bp_crcval_t crc, const void *data, size_t size);
     */

    UtAssert_UINT32_EQ(bplib_crc_update(&BPLIB_CRC32_CASTAGNOLI, 0x12345678, "aa", 2), 0x13ef76f0);
    UtAssert_UINT32_EQ(bplib_crc_update(&BPLIB_CRC16_X25, 0x1234, "bb", 2), 0x9cae);
    UtAssert_UINT32_EQ(bplib_crc_update(&BPLIB_CRC_NONE, 0x12345678, "cc", 2), 0x12345678);

    UtAssert_UINT32_EQ(bplib_crc_update(&UT_BPLIB_CRC6, 0x23, "dd", 2), 0x02);
}

void Test_bplib_crc_finalize(void)
{
    /* Test function for:
     * bp_crcval_t bplib_crc_finalize(bplib_crc_parameters_t *params, bp_crcval_t crc);
     */

    UtAssert_UINT32_EQ(bplib_crc_finalize(&BPLIB_CRC32_CASTAGNOLI, 0x12345678), 0xe195d3b7);
    UtAssert_UINT32_EQ(bplib_crc_finalize(&BPLIB_CRC16_X25, 0x1234), 0xd3b7);
    UtAssert_UINT32_EQ(bplib_crc_finalize(&BPLIB_CRC_NONE, 0x12345678), 0);

    UtAssert_UINT32_EQ(bplib_crc_finalize(&UT_BPLIB_CRC6, 0x13), 0x32);
}

void Test_bplib_crc_get(void)
{
    /* Test function for:
     * bp_crcval_t bplib_crc_get(const void *data, const uint32_t length, bplib_crc_parameters_t *params);
     */

    /*
     * Note that because the CRC algorithms here are fully self contained and
     * do not call any library routines, these calculations should be correct
     * per the definition of CRC-16/X.25 and CRC-32/C.
     */

    UtAssert_UINT32_EQ(bplib_crc_get("123456789", 9, &BPLIB_CRC32_CASTAGNOLI), 0xe3069283);
    UtAssert_UINT32_EQ(bplib_crc_get("123456789", 9, &BPLIB_CRC16_X25), 0x906e);
    UtAssert_UINT32_EQ(bplib_crc_get("123456789", 9, &BPLIB_CRC_NONE), 0);

    UtAssert_UINT32_EQ(bplib_crc_get("123456789", 9, &UT_BPLIB_CRC6), 0x06);
}

void TestBplibMemCommon_CRC_Setup(void)
{
    UtAssert_VOIDCALL(bplib_crc_init());

    uint8_t byte;

    /* precompute the table for test CRCs */
    byte = 0;
    do
    {
        UT_BPLIB_CRC6_ITU_TABLE[byte] = bplib_precompute_crc_byte(6, byte, UT_BPLIB_CRC6_POLY);
        ++byte;
    }
    while (byte != 0);
}

void TestBplibMemCommon_CRC_Execute(void)
{
    Test_bplib_crc_get_name();
    Test_bplib_crc_get_width();
    Test_bplib_crc_initial_value();
    Test_bplib_crc_update();
    Test_bplib_crc_finalize();
    Test_bplib_crc_get();
}
