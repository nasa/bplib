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

void test_v7_block_encode_pri(void)
{
    /* Test function for:
     * int v7_block_encode_pri(bplib_mpool_bblock_primary_t *cpb)
     */
    bplib_mpool_bblock_primary_t cpb;

    cpb.data.logical.version = 7;
    cpb.data.logical.crctype = bp_crctype_CRC16;
    cpb.data.logical.crcval  = 2;
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_get_parent_pool_from_link), UT_V7_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_crc_get_width), UT_V7_int8_Handler, NULL);
    UtAssert_UINT8_NEQ(v7_block_encode_pri(&cpb), 0);

    cpb.data.logical.version        = 7;
    cpb.data.logical.crctype        = bp_crctype_CRC32C;
    cpb.data.logical.crcval         = 2;
    cpb.data.logical.destinationEID = (bp_endpointid_buffer_t) {.scheme = bp_endpointid_scheme_ipn};
    cpb.data.logical.sourceEID      = (bp_endpointid_buffer_t) {.scheme = bp_endpointid_scheme_ipn};
    cpb.data.logical.reportEID      = (bp_endpointid_buffer_t) {.scheme = bp_endpointid_scheme_ipn};
    UtAssert_UINT8_NEQ(v7_block_encode_pri(&cpb), 0);
}

void test_v7_block_encode_pay(void)
{
    /* Test function for:
     * int v7_block_encode_pay(bplib_mpool_bblock_canonical_t *ccb, const void *data_ptr, size_t data_size)
     */
    bplib_mpool_bblock_canonical_t ccb;
    const char                          *data_ptr="Hello";
    size_t                         data_size = sizeof(data_ptr);

    memset(&ccb, 0, sizeof(bplib_mpool_bblock_canonical_t));

    UtAssert_INT32_EQ(v7_block_encode_pay(&ccb, data_ptr, data_size), 0);
}

void test_v7_block_encode_canonical(void)
{
    /* Test function for:
     * int v7_block_encode_canonical(bplib_mpool_bblock_canonical_t *ccb)
     */
    bplib_mpool_bblock_canonical_t ccb;

    memset(&ccb, 0, sizeof(bplib_mpool_bblock_canonical_t));

    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_undefined;
    UtAssert_INT32_EQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_payloadBlock;
    UtAssert_INT32_EQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_bundleAuthenicationBlock;
    UtAssert_INT32_EQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_payloadIntegrityBlock;
    UtAssert_INT32_EQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_payloadConfidentialityBlock;
    UtAssert_INT32_EQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_previousHopInsertionBlock;
    UtAssert_INT32_EQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_previousNode;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_bundleAge;
    UtAssert_INT32_EQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_metadataExtensionBlock;
    UtAssert_INT32_EQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_extensionSecurityBlock;
    UtAssert_INT32_EQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_hopCount;
    UtAssert_INT32_EQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_custodyTrackingBlock;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_adminRecordPayloadBlock;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_custodyAcceptPayloadBlock;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
}

void test_v7_encoder_mpstream_write(void)
{
    /* Test function for:
     * int v7_encoder_mpstream_write(void *arg, const void *ptr, size_t sz)
     */
    void  *arg = NULL;
    void  *ptr = NULL;
    size_t sz  = 0;

    UT_SetDefaultReturnValue(UT_KEY(bplib_mpool_stream_write), 0);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_stream_write), UT_V7_uint64_Handler, NULL);
    UtAssert_INT32_EQ(v7_encoder_mpstream_write(arg, ptr, sz), 0);

    sz = 1;
    UtAssert_INT32_NEQ(v7_encoder_mpstream_write(arg, ptr, sz), 0);
}

void test_v7_encoder_write_crc(void)
{
    /* Test function for:
     * int v7_encoder_write_crc(v7_encode_state_t *enc)
     */
    v7_encode_state_t enc;

    memset(&enc, 0, sizeof(v7_encode_state_t));

    enc.next_writer = test_bplib_v7_writer_func_stub;
    UT_SetHandlerFunction(UT_KEY(bplib_crc_get_width), UT_V7_uint8_Handler, NULL);
    UT_SetDefaultReturnValue(UT_KEY(bplib_crc_get_width), 16);
    UtAssert_INT32_EQ(v7_encoder_write_crc(&enc), 0);
}

void test_v7_encoder_write_wrapper(void)
{
    /* Test function for:
     * CborError v7_encoder_write_wrapper(void *arg, const void *ptr, size_t sz, CborEncoderAppendType at)
     */
    v7_encode_state_t     enc;
    void                 *ptr = NULL;
    size_t                sz  = 0;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    enc.crc_flag = false;
    UT_SetDefaultReturnValue(UT_KEY(test_bplib_v7_writer_func_stub), BP_ERROR);
    enc.next_writer = test_bplib_v7_writer_func_stub;
    UtAssert_INT32_NEQ(v7_encoder_write_wrapper(&enc, ptr, sz), 0);

    enc.crc_flag = true;
    UT_SetDefaultReturnValue(UT_KEY(test_bplib_v7_writer_func_stub), BP_SUCCESS);
    UtAssert_INT32_EQ(v7_encoder_write_wrapper(&enc, ptr, sz), 0);
}

void TestV7EncodeApi_Rgister(void)
{
    UtTest_Add(test_v7_block_encode_pri, NULL, NULL, "Test v7_block_encode_pri");
    UtTest_Add(test_v7_block_encode_pay, NULL, NULL, "Test v7_block_encode_pay");
    UtTest_Add(test_v7_block_encode_canonical, NULL, NULL, "Test v7_block_encode_canonical");
    UtTest_Add(test_v7_encoder_mpstream_write, NULL, NULL, "Test v7_encoder_mpstream_write");
    UtTest_Add(test_v7_encoder_write_crc, NULL, NULL, "Test v7_encoder_write_crc");
    UtTest_Add(test_v7_encoder_write_wrapper, NULL, NULL, "Test v7_encoder_write_wrapper");
}
