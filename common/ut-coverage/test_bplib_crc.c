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
#include "test_bplib_common.h"
#include "crc.h"

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
}

void Test_bplib_crc_get_width(void)
{
    /* Test function for:
     * uint8_t     bplib_crc_get_width(bplib_crc_parameters_t *params);
     */

    UtAssert_UINT32_EQ(bplib_crc_get_width(&BPLIB_CRC32_CASTAGNOLI), 32);
    UtAssert_UINT32_EQ(bplib_crc_get_width(&BPLIB_CRC16_X25), 16);
    UtAssert_UINT32_EQ(bplib_crc_get_width(&BPLIB_CRC_NONE), 0);
}

void Test_bplib_crc_initial_value(void)
{
    /* Test function for:
     * bp_crcval_t bplib_crc_initial_value(bplib_crc_parameters_t *params);
     */

    UtAssert_UINT32_EQ(bplib_crc_initial_value(&BPLIB_CRC32_CASTAGNOLI), 0xffffffff);
    UtAssert_UINT32_EQ(bplib_crc_initial_value(&BPLIB_CRC16_X25), 0xffff);
    UtAssert_UINT32_EQ(bplib_crc_initial_value(&BPLIB_CRC_NONE), 0);
}

void Test_bplib_crc_update(void)
{
    /* Test function for:
     * bp_crcval_t bplib_crc_update(bplib_crc_parameters_t *params, bp_crcval_t crc, const void *data, size_t size);
     */

    UtAssert_UINT32_EQ(bplib_crc_update(&BPLIB_CRC32_CASTAGNOLI, 0x12345678, "aa", 2), 0x13ef76f0);
    UtAssert_UINT32_EQ(bplib_crc_update(&BPLIB_CRC16_X25, 0x1234, "bb", 2), 0x9cae);
    UtAssert_UINT32_EQ(bplib_crc_update(&BPLIB_CRC_NONE, 0x12345678, "cc", 2), 0x12345678);
}

void Test_bplib_crc_finalize(void)
{
    /* Test function for:
     * bp_crcval_t bplib_crc_finalize(bplib_crc_parameters_t *params, bp_crcval_t crc);
     */

    UtAssert_UINT32_EQ(bplib_crc_finalize(&BPLIB_CRC32_CASTAGNOLI, 0x12345678), 0xe195d3b7);
    UtAssert_UINT32_EQ(bplib_crc_finalize(&BPLIB_CRC16_X25, 0x1234), 0xd3b7);
    UtAssert_UINT32_EQ(bplib_crc_finalize(&BPLIB_CRC_NONE, 0x12345678), 0);
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
}

void TestBplibCommon_CRC_Setup(void)
{
    UtAssert_VOIDCALL(bplib_crc_init());
}

void TestBplibCommon_CRC_Execute(void)
{
    Test_bplib_crc_get_name();
    Test_bplib_crc_get_width();
    Test_bplib_crc_initial_value();
    Test_bplib_crc_update();
    Test_bplib_crc_finalize();
    Test_bplib_crc_get();
}
