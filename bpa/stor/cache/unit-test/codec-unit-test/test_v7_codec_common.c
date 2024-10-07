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

void test_v7_compute_full_bundle_size(void)
{
    /* Test function for:
     * size_t v7_compute_full_bundle_size(BPLib_STOR_CACHE_BblockPrimary_t *cpb)
     */
    BPLib_STOR_CACHE_BblockPrimary_t   cpb;
    BPLib_STOR_CACHE_BblockCanonical_t ccb;

    memset(&cpb, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&ccb, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));

    cpb.bundle_encode_size_cache = 0;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetParentPoolFromLink), UT_V7_sizet_Handler, NULL);
    UtAssert_INT32_EQ(v7_compute_full_bundle_size(&cpb), 0);

    cpb.block_encode_size_cache = 1;
    ccb.block_encode_size_cache = 0;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_V7_AltHandler_PointerReturn, NULL);
    UtAssert_INT32_NEQ(v7_compute_full_bundle_size(&cpb), 0);

    cpb.bundle_encode_size_cache = 0;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_V7_AltHandler_PointerReturn, &ccb);
    UtAssert_INT32_EQ(v7_compute_full_bundle_size(&cpb), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_V7_AltHandler_PointerReturn, NULL);
}

void test_v7_copy_full_bundle_out(void)
{
    /* Test function for:
     * size_t v7_copy_full_bundle_out(BPLib_STOR_CACHE_BblockPrimary_t *cpb, void *buffer, size_t buf_sz)
     */
    BPLib_STOR_CACHE_BblockPrimary_t   cpb;
    BPLib_STOR_CACHE_BblockCanonical_t ccb;
    uint8_t                       *buffer[200];
    size_t                         buf_sz = 0;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCborExport), UT_V7_sizet_Handler, NULL);
    UtAssert_INT32_EQ(v7_copy_full_bundle_out(&cpb, buffer, buf_sz), 0);

    buf_sz = 200;
    UtAssert_INT32_NEQ(v7_copy_full_bundle_out(&cpb, buffer, buf_sz), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_V7_AltHandler_PointerReturn, &ccb);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_BblockCborExport), 99);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCborExport), UT_V7_uint64_Handler, NULL);
    UtAssert_INT32_NEQ(v7_copy_full_bundle_out(&cpb, buffer, buf_sz), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_V7_AltHandler_PointerReturn, NULL);
}

void test_v7_copy_full_bundle_in(void)
{
    /* Test function for:
     * size_t v7_copy_full_bundle_in(BPLib_STOR_CACHE_BblockPrimary_t *cpb, const void *buffer, size_t buf_sz)
     */
    BPLib_STOR_CACHE_Ref_t              duct_ref;
    size_t                         remain_sz;
    BPLib_STOR_CACHE_BblockPrimary_t   pblk;
    BPLib_STOR_CACHE_BblockCanonical_t ccb;

    memset(&duct_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&pblk, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&ccb, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));
    remain_sz = 0;

    pblk.cblock_list.type = BPLib_STOR_CACHE_BlocktypeListHead;
    UtAssert_INT32_EQ(v7_copy_full_bundle_in(&pblk, &duct_ref, remain_sz), 0);

    remain_sz                    = 200;
    pblk.block_encode_size_cache = 0;
    UtAssert_INT32_EQ(v7_copy_full_bundle_in(&pblk, &duct_ref, remain_sz), 0);

    memset(&duct_ref, 0x9F, sizeof(BPLib_STOR_CACHE_Ref_t));
    UT_SetDefaultReturnValue(UT_KEY(cbor_parser_init), CborNoError);
    UtAssert_INT32_EQ(v7_copy_full_bundle_in(&pblk, &duct_ref, remain_sz), 0);

    pblk.block_encode_size_cache = 1;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_V7_sizet_Handler, NULL);
    UtAssert_INT32_EQ(v7_copy_full_bundle_in(&pblk, &duct_ref, remain_sz), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_V7_AltHandler_PointerReturn, &ccb);

    UtAssert_INT32_EQ(v7_copy_full_bundle_in(&pblk, &duct_ref, remain_sz), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_V7_AltHandler_PointerReturn, NULL);
}

void test_v7_sum_preencoded_size(void)
{
    /* Test function for:
     * size_t v7_sum_preencoded_size(BPLib_STOR_CACHE_Block_t *list)
     */
    BPLib_STOR_CACHE_Block_t list;
    BPLib_STOR_CACHE_Block_t blk;
    BPLib_STOR_CACHE_Block_t blk1;

    memset(&list, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&blk1, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    blk1.type = BPLib_STOR_CACHE_BlocktypeListHead;
    blk.next  = &blk1;
    list.next = &blk;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetUserContentSize), UT_V7_sizet_Handler, NULL);
    UtAssert_INT32_EQ(v7_sum_preencoded_size(&list), 0);
}

void TestV7CodecCommon_Rgister(void)
{
    UtTest_Add(test_v7_compute_full_bundle_size, NULL, NULL, "Test V7 compute_full_bundle_size");
    UtTest_Add(test_v7_copy_full_bundle_out, NULL, NULL, "Test V7 copy_full_bundle_out");
    UtTest_Add(test_v7_copy_full_bundle_in, NULL, NULL, "Test V7 copy_full_bundle_in");
    UtTest_Add(test_v7_sum_preencoded_size, NULL, NULL, "Test v7_sum_preencoded_size");
}
