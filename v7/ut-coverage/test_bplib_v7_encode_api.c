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
#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"
#include "v7_codec.h"
#include "v7_mpool_bblocks.h"
#include "v7_mpool_ref.h"
#include "v7_types.h"
#include "crc.h"
#include <stdlib.h>

#include "test_bplib_v7.h"

#define BPCAT_BUNDLE_BUFFER_SIZE 16384

static void V7Codec_CopyFull_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint64_t retval = 0;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

static void V7Codec_CRC_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint8_t retval = 0;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void Test_v7_block_encode_pri(void)
{
    bplib_mpool_bblock_primary_t cpb;
    cpb.data.logical.version = 7;
    cpb.data.logical.crctype = bp_crctype_CRC16;
    cpb.data.logical.crcval  = 2;
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_get_parent_pool_from_link), V7Codec_CopyFull_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_crc_get_width), V7Codec_CRC_Handler, NULL);
    UtAssert_UINT8_NEQ(v7_block_encode_pri(&cpb), 0);
    cpb.data.logical.version        = 7;
    cpb.data.logical.crctype        = bp_crctype_CRC32C;
    cpb.data.logical.crcval         = 2;
    cpb.data.logical.destinationEID = (bp_endpointid_buffer_t) {.scheme = bp_endpointid_scheme_ipn};
    cpb.data.logical.sourceEID      = (bp_endpointid_buffer_t) {.scheme = bp_endpointid_scheme_ipn};
    cpb.data.logical.reportEID      = (bp_endpointid_buffer_t) {.scheme = bp_endpointid_scheme_ipn};
    UtAssert_UINT8_EQ(v7_block_encode_pri(&cpb), 0);
}

/* int v7_block_encode_pay(bplib_mpool_bblock_canonical_t *ccb, const void *data_ptr, size_t data_size) */
void Test_v7_block_encode_pay(void)
{
    bplib_mpool_bblock_canonical_t ccb;
    void                          *data_ptr  = calloc(1, 1000);
    size_t                         data_size = 1000;
    UtAssert_INT32_EQ(v7_block_encode_pay(&ccb, data_ptr, data_size), 0);
    if (data_ptr)
        free(data_ptr);
}

void Test_v7_block_encode_canonical(void)
{
    bplib_mpool_bblock_canonical_t ccb;
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_payloadBlock;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_bundleAuthenicationBlock;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_payloadIntegrityBlock;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_payloadConfidentialityBlock;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_previousHopInsertionBlock;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_previousNode;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_bundleAge;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_metadataExtensionBlock;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_extensionSecurityBlock;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_hopCount;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_custodyTrackingBlock;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_adminRecordPayloadBlock;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
    ccb.canonical_logical_data.canonical_block.blockType = bp_blocktype_custodyAcceptPayloadBlock;
    UtAssert_INT32_NEQ(v7_block_encode_canonical(&ccb), 0);
}

void TestBplibV7Encode_Register(void)
{
    UtTest_Add(Test_v7_block_encode_pri, NULL, NULL, "BPLIB V7 block_encode_pri");
    UtTest_Add(Test_v7_block_encode_pay, NULL, NULL, "BPLIB V7 block_encode_pay");
    UtTest_Add(Test_v7_block_encode_canonical, NULL, NULL, "BPLIB v7 block_encode_canonical");
}
