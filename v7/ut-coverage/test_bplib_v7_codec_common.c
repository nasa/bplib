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

static void UT_AltHandler_SizeReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    size_t retval;

    if (UserObj)
    {
        retval = *((size_t *)UserObj);
    }
    else
    {
        retval = 0;
    }

    UT_Stub_SetReturnValue(FuncKey, retval);
}

void Test_v7_compute_full_bundle_size(void)
{
    bplib_mpool_bblock_primary_t cpb;
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_bblock_canonical_cast), UT_AltHandler_PointerReturn, NULL);
    UtAssert_INT32_NEQ(v7_compute_full_bundle_size(&cpb), 0);
    cpb.bundle_encode_size_cache = 0;
    bplib_mpool_block_t cpb1;
    cpb.cblock_list.next = &cpb1;
    UtAssert_INT32_NEQ(v7_compute_full_bundle_size(&cpb), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_get_parent_pool_from_link), UT_AltHandler_PointerReturn, NULL);
    cpb.bundle_encode_size_cache = 0;
    cpb.block_encode_size_cache  = 0;
    UtAssert_INT32_EQ(v7_compute_full_bundle_size(&cpb), 0);
}

void Test_v7_copy_full_bundle_out(void)
{
    bplib_mpool_bblock_primary_t cpb;
    cpb.cblock_list.type            = bplib_mpool_blocktype_canonical;
    cpb.chunk_list.type             = bplib_mpool_blocktype_canonical;
    cpb.data.logical.destinationEID = (bp_endpointid_buffer_t) {.scheme = bp_endpointid_scheme_ipn};
    cpb.data.logical.sourceEID      = (bp_endpointid_buffer_t) {.scheme = bp_endpointid_scheme_ipn};
    cpb.data.logical.reportEID      = (bp_endpointid_buffer_t) {.scheme = bp_endpointid_scheme_ipn};
    bplib_mpool_block_t cpb1;
    cpb1.type            = bplib_mpool_blocktype_flow;
    cpb.cblock_list.next = &cpb1;
    uint8_t *buffer[16384];
    size_t   buf_sz = 16384;
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_bblock_cbor_export), UT_AltHandler_SizeReturn, NULL);
    UtAssert_INT32_NEQ(v7_copy_full_bundle_out(&cpb, buffer, buf_sz), 0);
    buf_sz = 1;
    UtAssert_INT32_EQ(v7_copy_full_bundle_out(&cpb, buffer, buf_sz), 0);
}

void Test_v7_copy_full_bundle_in(void)
{
    bplib_mpool_ref_t flow_ref;
    size_t            remain_sz = sizeof(flow_ref);
    memset(&flow_ref, 0x9F, sizeof(bplib_mpool_ref_t));
    bplib_mpool_bblock_primary_t pb;
    pb.cblock_list.parent_offset   = 100;
    pb.chunk_list.parent_offset    = 100;
    pb.data.logical.version        = 7;
    pb.data.logical.crctype        = bp_crctype_CRC16;
    pb.data.logical.destinationEID = (bp_endpointid_buffer_t) {.scheme = bp_endpointid_scheme_ipn};
    pb.data.logical.sourceEID      = (bp_endpointid_buffer_t) {.scheme = bp_endpointid_scheme_ipn};
    pb.data.logical.reportEID      = (bp_endpointid_buffer_t) {.scheme = bp_endpointid_scheme_ipn};
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_get_parent_pool_from_link), UT_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_bblock_canonical_alloc), UT_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_bblock_canonical_cast), UT_AltHandler_PointerReturn, NULL);
    UtAssert_INT32_EQ(v7_copy_full_bundle_in(&pb, &flow_ref, remain_sz), 0);
    pb.block_encode_size_cache = 0;
    UtAssert_INT32_EQ(v7_copy_full_bundle_in(&pb, &flow_ref, remain_sz), 0);
}

void TestBplibV7Codec_Register(void)
{
    UtTest_Add(Test_v7_compute_full_bundle_size, NULL, NULL, "BPLIB V7 compute_full_bundle_size");
    UtTest_Add(Test_v7_copy_full_bundle_out, NULL, NULL, "BPLIB V7 copy_full_bundle_out");
    UtTest_Add(Test_v7_copy_full_bundle_in, NULL, NULL, "BPLIB V7 copy_full_bundle_in");
}
