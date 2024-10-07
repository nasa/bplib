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

#include "bplib_stor_cache_block.h"

void test_v7_block_decode_pri(void)
{
    /* Test function for:
     * int v7_block_decode_pri(BPLib_STOR_CACHE_BblockPrimary_t *cpb, const void *data_ptr, size_t data_size)
     */
    BPLib_STOR_CACHE_BblockPrimary_t cpb;
    uint8_t                      data;
    size_t                       data_size = 100 * sizeof(uint8_t);

    memset(&cpb, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&data, 0, sizeof(uint8_t));

    UT_SetDefaultReturnValue(UT_KEY(cbor_parser_init), CborUnknownError);
    UtAssert_INT32_NEQ(v7_block_decode_pri(&cpb, &data, data_size), 0);

    UT_SetDefaultReturnValue(UT_KEY(cbor_parser_init), CborNoError);
    UtAssert_INT32_NEQ(v7_block_decode_pri(&cpb, &data, data_size), 0);
}

void test_v7_block_decode_canonical(void)
{
    /* Test function for:
     * int v7_block_decode_canonical(BPLib_STOR_CACHE_BblockCanonical_t *ccb, const void *data_ptr, size_t data_size,
     * bp_blocktype_t payload_block_hint)
     */
    BPLib_STOR_CACHE_BblockCanonical_t ccb;
    uint8_t                        data[100]          = {0};
    size_t                         data_size          = 100 * sizeof(uint8_t);
    bp_blocktype_t                 payload_block_hint = bp_blocktype_ciphertextPayloadBlock;

    memset(&ccb, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));

    UT_SetDefaultReturnValue(UT_KEY(cbor_parser_init), CborNoError);
    UtAssert_INT32_NEQ(v7_block_decode_canonical(&ccb, data, data_size, payload_block_hint), 0);

    UT_SetDefaultReturnValue(UT_KEY(cbor_parser_init), CborUnknownError);
    UtAssert_INT32_NEQ(v7_block_decode_canonical(&ccb, data, data_size, payload_block_hint), 0);
}

void test_v7_save_and_verify_block(void)
{
    /* Test function for:
     * size_t v7_save_and_verify_block(BPLib_STOR_CACHE_Block_t *head, const uint8_t *block_base, size_t block_size,
     * bp_crctype_t crc_type, bp_crcval_t crc_check)
     */
    BPLib_STOR_CACHE_Block_t head;
    uint8_t             block_base;
    size_t              block_size;
    bp_crctype_t        crc_type;
    bp_crcval_t         crc_check;

    memset(&head, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&block_base, 0, sizeof(uint8_t));
    block_size = 100;
    crc_type   = 0;
    crc_check  = 0;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_StreamWrite), 100);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_StreamWrite), UT_V7_uint64_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_CrcGetWidth), UT_V7_int8_Handler, NULL);
    UtAssert_INT32_NEQ(v7_save_and_verify_block(&head, &block_base, block_size, crc_type, crc_check), 0);
}

void TestV7DecodecApi_Rgister(void)
{
    UtTest_Add(test_v7_block_decode_pri, NULL, NULL, "Test v7 block_decode_pri");
    UtTest_Add(test_v7_block_decode_canonical, NULL, NULL, "Test v7_block_decode_canonical");
    UtTest_Add(test_v7_save_and_verify_block, NULL, NULL, "Test v7_save_and_verify_block");
}
