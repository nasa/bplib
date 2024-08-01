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

void test_v7_block_decode_pri(void)
{
    /* Test function for:
     * int v7_block_decode_pri(bplib_mpool_bblock_primary_t *cpb, const void *data_ptr, size_t data_size)
     */
    bplib_mpool_bblock_primary_t cpb;
    uint8_t                      data;
    size_t                       data_size = 100 * sizeof(uint8_t);

    memset(&cpb, 0, sizeof(bplib_mpool_bblock_primary_t));
    memset(&data, 0, sizeof(uint8_t));

    UtAssert_INT32_NEQ(v7_block_decode_pri(&cpb, &data, data_size), 0);

    UT_SetDefaultReturnValue(UT_KEY(QCBORDecode_GetError), QCBOR_ERR_ENCODE_UNSUPPORTED);
    UtAssert_INT32_NEQ(v7_block_decode_pri(&cpb, &data, data_size), 0);
}

void test_v7_block_decode_canonical(void)
{
    /* Test function for:
     * int v7_block_decode_canonical(bplib_mpool_bblock_canonical_t *ccb, const void *data_ptr, size_t data_size,
     * bp_blocktype_t payload_block_hint)
     */
    bplib_mpool_bblock_canonical_t ccb;
    uint8_t                        data[100]          = {0};
    size_t                         data_size          = 100 * sizeof(uint8_t);
    bp_blocktype_t                 payload_block_hint = bp_blocktype_ciphertextPayloadBlock;

    memset(&ccb, 0, sizeof(bplib_mpool_bblock_canonical_t));

    UtAssert_INT32_NEQ(v7_block_decode_canonical(&ccb, data, data_size, payload_block_hint), 0);

    UtAssert_INT32_NEQ(v7_block_decode_canonical(&ccb, data, data_size, payload_block_hint), 0);
}

void test_v7_save_and_verify_block(void)
{
    /* Test function for:
     * size_t v7_save_and_verify_block(bplib_mpool_block_t *head, const uint8_t *block_base, size_t block_size,
     * bp_crctype_t crc_type, bp_crcval_t crc_check)
     */
    bplib_mpool_block_t head;
    uint8_t             block_base;
    size_t              block_size;
    bp_crctype_t        crc_type;
    bp_crcval_t         crc_check;

    memset(&head, 0, sizeof(bplib_mpool_block_t));
    memset(&block_base, 0, sizeof(uint8_t));
    block_size = 100;
    crc_type   = 0;
    crc_check  = 0;

    UT_SetDefaultReturnValue(UT_KEY(bplib_mpool_stream_write), 100);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_stream_write), UT_V7_uint64_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_crc_get_width), UT_V7_int8_Handler, NULL);
    UtAssert_INT32_NEQ(v7_save_and_verify_block(&head, &block_base, block_size, crc_type, crc_check), 0);
}

void TestV7DecodecApi_Rgister(void)
{
    UtTest_Add(test_v7_block_decode_pri, NULL, NULL, "Test v7 block_decode_pri");
    UtTest_Add(test_v7_block_decode_canonical, NULL, NULL, "Test v7_block_decode_canonical");
    UtTest_Add(test_v7_save_and_verify_block, NULL, NULL, "Test v7_save_and_verify_block");
}
