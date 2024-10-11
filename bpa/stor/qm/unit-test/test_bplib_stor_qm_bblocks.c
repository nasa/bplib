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

#include "test_bplib_stor_qm.h"

void Test_BPLib_STOR_CACHE_BblockPrimaryCast(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_BblockPrimary_t *BPLib_STOR_CACHE_BblockPrimaryCast(BPLib_STOR_CACHE_Block_t *cb);
     */
    BPLib_STOR_CACHE_BlockContent_t   my_block;
    BPLib_STOR_CACHE_BblockPrimary_t *pri;
    BPLib_STOR_CACHE_Block_t          *cb = &my_block.header.base_link;

    UtAssert_NULL(BPLib_STOR_CACHE_BblockPrimaryCast(NULL));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_NULL(BPLib_STOR_CACHE_BblockPrimaryCast(cb));

    test_setup_cpool_block(NULL, &my_block, BPLib_STOR_CACHE_BlocktypePrimary, 0);
    UtAssert_ADDRESS_EQ(pri = BPLib_STOR_CACHE_BblockPrimaryCast(cb), &my_block.u);

    test_setup_cpool_block(NULL, &my_block, BPLib_STOR_CACHE_BlocktypeCanonical, 0);
    UtAssert_NULL(BPLib_STOR_CACHE_BblockPrimaryCast(cb));

    /* Ensure that the logical accessors work (inline) */
    UtAssert_NOT_NULL(BPLib_STOR_CACHE_BblockPrimaryGetLogical(pri));
}

void Test_BPLib_STOR_CACHE_BblockCanonicalCast(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_BblockCanonical_t *BPLib_STOR_CACHE_BblockCanonicalCast(BPLib_STOR_CACHE_Block_t *cb);
     */
    BPLib_STOR_CACHE_BlockContent_t     my_block;
    BPLib_STOR_CACHE_BblockCanonical_t *canonical;
    BPLib_STOR_CACHE_Block_t            *cb = &my_block.header.base_link;

    UtAssert_NULL(BPLib_STOR_CACHE_BblockCanonicalCast(NULL));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_NULL(BPLib_STOR_CACHE_BblockCanonicalCast(cb));

    test_setup_cpool_block(NULL, &my_block, BPLib_STOR_CACHE_BlocktypeCanonical, 0);
    UtAssert_ADDRESS_EQ(canonical = BPLib_STOR_CACHE_BblockCanonicalCast(cb), &my_block.u);

    test_setup_cpool_block(NULL, &my_block, BPLib_STOR_CACHE_BlocktypePrimary, 0);
    UtAssert_NULL(BPLib_STOR_CACHE_BblockCanonicalCast(cb));

    /* Ensure that the logical accessors work (inline) */
    UtAssert_NOT_NULL(BPLib_STOR_CACHE_BblockCanonicalGetLogical(canonical));
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_BblockCanonicalSetContentPosition(canonical, 1, 2));
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_BblockCanonicalGetContentLength(canonical), 2);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_BblockCanonicalGetContentOffset(canonical), 1);
}

void Test_BPLib_STOR_CACHE_BblockCborCast(void)
{
    /* Test function for:
     * void *BPLib_STOR_CACHE_BblockCborCast(BPLib_STOR_CACHE_Block_t *cb);
     */
    BPLib_STOR_CACHE_BlockContent_t my_block;
    BPLib_STOR_CACHE_Block_t        *cb = &my_block.header.base_link;

    UtAssert_NULL(BPLib_STOR_CACHE_BblockCborCast(NULL));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_NULL(BPLib_STOR_CACHE_BblockCborCast(cb));

    test_setup_cpool_block(NULL, &my_block, BPLib_STOR_CACHE_BlocktypeGeneric, BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE);
    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_BblockCborCast(cb), &my_block.u);
}

void Test_BPLib_STOR_CACHE_BblockCborSetSize(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_BblockCborSetSize(BPLib_STOR_CACHE_Block_t *cb, size_t user_content_size);
     */
    BPLib_STOR_CACHE_BlockContent_t my_block;

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_BblockCborSetSize(NULL, 0));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_BblockCborSetSize(&my_block.header.base_link, 0));

    test_setup_cpool_block(NULL, &my_block, BPLib_STOR_CACHE_BlocktypeGeneric, ~BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_BblockCborSetSize(&my_block.header.base_link, 0));

    test_setup_cpool_block(NULL, &my_block, BPLib_STOR_CACHE_BlocktypeCanonical, BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_BblockCborSetSize(&my_block.header.base_link, 0));

    test_setup_cpool_block(NULL, &my_block, BPLib_STOR_CACHE_BlocktypeGeneric, BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_BblockCborSetSize(&my_block.header.base_link, 123));

    UtAssert_UINT32_EQ(my_block.header.user_content_length, 123);
}

void Test_BPLib_STOR_CACHE_BblockCanonicalAlloc(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BblockCanonicalAlloc(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number, void
     * *init_arg);
     */

    UT_BPLib_STOR_QM_Buf_t buf;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(BPLib_STOR_CACHE_BblockCanonicalAlloc(&buf.pool, 1234, NULL));

    test_setup_cpool_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_BblockCanonicalAlloc(&buf.pool, 1234, NULL), &buf.blk[0]);
}

void Test_BPLib_STOR_CACHE_BblockCborAlloc(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BblockCborAlloc(BPLib_STOR_CACHE_Pool_t *pool);
     */

    UT_BPLib_STOR_QM_Buf_t buf;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(BPLib_STOR_CACHE_BblockCborAlloc(&buf.pool));

    test_setup_cpool_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_BblockCborAlloc(&buf.pool), &buf.blk[0]);
}

void Test_BPLib_STOR_CACHE_BblockCborAppend(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_BblockCborAppend(BPLib_STOR_CACHE_Block_t *head, BPLib_STOR_CACHE_Block_t *blk);
     */
    UT_BPLib_STOR_QM_Buf_t buf;

    memset(&buf, 0, sizeof(buf));

    test_setup_cpool_block(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypeGeneric, BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE);
    test_setup_cpool_block(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypeGeneric, BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE);

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_BblockCborAppend(&buf.blk[0].header.base_link, &buf.blk[1].header.base_link));
}

void Test_BPLib_STOR_CACHE_BblockPrimaryAppend(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_BblockPrimaryAppend(BPLib_STOR_CACHE_BblockPrimary_t *cpb, BPLib_STOR_CACHE_Block_t *ccb);
     */

    UT_BPLib_STOR_QM_Buf_t         buf;
    bp_canonical_block_buffer_t *b;

    memset(&buf, 0, sizeof(buf));

    test_setup_cpool_block(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypePrimary, 0);
    test_setup_cpool_block(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypeCanonical, 0);

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_BblockPrimaryAppend(&buf.blk[0].u.primary.pblock, &buf.blk[1].header.base_link));

    /* Test the special case if inserting blocknum 1, it should go last */
    test_setup_cpool_block(&buf.pool, &buf.blk[2], BPLib_STOR_CACHE_BlocktypeCanonical, 0);
    b                           = BPLib_STOR_CACHE_BblockCanonicalGetLogical(&buf.blk[2].u.canonical.cblock);
    b->canonical_block.blockNum = 1;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_BblockPrimaryAppend(&buf.blk[0].u.primary.pblock, &buf.blk[2].header.base_link));
}

void Test_BPLib_STOR_CACHE_BblockPrimaryLocateCanonical(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BblockPrimaryLocateCanonical(BPLib_STOR_CACHE_BblockPrimary_t *cpb,
     * bp_blocktype_t block_type);
     */

    UT_BPLib_STOR_QM_Buf_t         buf;
    bp_canonical_block_buffer_t *b;

    memset(&buf, 0, sizeof(buf));

    test_setup_cpool_block(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypePrimary, 0);
    UtAssert_NULL(BPLib_STOR_CACHE_BblockPrimaryLocateCanonical(&buf.blk[0].u.primary.pblock, bp_blocktype_undefined));

    test_setup_cpool_block(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypeCanonical, 0);
    BPLib_STOR_CACHE_InsertAfter(&buf.blk[0].u.primary.pblock.cblock_list, &buf.blk[1].header.base_link);
    b                            = BPLib_STOR_CACHE_BblockCanonicalGetLogical(&buf.blk[1].u.canonical.cblock);
    b->canonical_block.blockType = bp_blocktype_bundleAge;

    /* canonical block of a different type should not be found */
    UtAssert_NULL(BPLib_STOR_CACHE_BblockPrimaryLocateCanonical(&buf.blk[0].u.primary.pblock, bp_blocktype_hopCount));

    /* canonical block of the correct type should be found */
    UtAssert_ADDRESS_EQ(
        BPLib_STOR_CACHE_BblockPrimaryLocateCanonical(&buf.blk[0].u.primary.pblock, bp_blocktype_bundleAge), &buf.blk[1]);

    /* include a block that is NOT a canonical block */
    test_setup_cpool_block(&buf.pool, &buf.blk[2], BPLib_STOR_CACHE_BlocktypeGeneric, 0);
    BPLib_STOR_CACHE_InsertBefore(&buf.blk[0].u.primary.pblock.cblock_list, &buf.blk[2].header.base_link);
    UtAssert_NULL(BPLib_STOR_CACHE_BblockPrimaryLocateCanonical(&buf.blk[0].u.primary.pblock, bp_blocktype_hopCount));
}

void Test_BPLib_STOR_CACHE_BblockPrimaryDropEncode(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_BblockPrimaryDropEncode(BPLib_STOR_CACHE_BblockPrimary_t *cpb);
     */
    UT_BPLib_STOR_QM_Buf_t               buf;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_cpool_block(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    admin = BPLib_STOR_CACHE_GetAdmin(&buf.pool);

    test_setup_cpool_block(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypePrimary, 0);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_BblockPrimaryDropEncode(&buf.blk[0].u.primary.pblock));

    /* confirms nothing got recycled (empty cbor block list) */
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsEmptyListHead(&admin->recycle_blocks.block_list));

    test_setup_cpool_block(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypeGeneric, BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE);
    BPLib_STOR_CACHE_InsertAfter(&buf.blk[0].u.primary.pblock.chunk_list, &buf.blk[1].header.base_link);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_BblockPrimaryDropEncode(&buf.blk[0].u.primary.pblock));

    /* confirms that cbor block got recycled */
    UtAssert_BOOL_FALSE(BPLib_STOR_CACHE_IsEmptyListHead(&admin->recycle_blocks.block_list));
}

void Test_BPLib_STOR_CACHE_BblockCanonicalDropEncode(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_BblockCanonicalDropEncode(BPLib_STOR_CACHE_BblockCanonical_t *ccb);
     */
    UT_BPLib_STOR_QM_Buf_t               buf;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_cpool_block(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    admin = BPLib_STOR_CACHE_GetAdmin(&buf.pool);

    test_setup_cpool_block(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypeCanonical, 0);
    buf.blk[0].u.canonical.cblock.block_encode_size_cache = 32;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_BblockCanonicalDropEncode(&buf.blk[0].u.canonical.cblock));
    UtAssert_ZERO(buf.blk[0].u.canonical.cblock.block_encode_size_cache);

    /* confirms nothing got recycled (empty cbor block list) */
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsEmptyListHead(&admin->recycle_blocks.block_list));

    test_setup_cpool_block(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypeGeneric, BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE);
    buf.blk[0].u.canonical.cblock.block_encode_size_cache = 32;
    BPLib_STOR_CACHE_InsertAfter(&buf.blk[0].u.canonical.cblock.chunk_list, &buf.blk[1].header.base_link);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_BblockCanonicalDropEncode(&buf.blk[0].u.canonical.cblock));
    UtAssert_ZERO(buf.blk[0].u.canonical.cblock.block_encode_size_cache);

    /* confirms that cbor block got recycled */
    UtAssert_BOOL_FALSE(BPLib_STOR_CACHE_IsEmptyListHead(&admin->recycle_blocks.block_list));

    /* confirm that a primary block associated with this canonical block is also invalidated */
    memset(&buf, 0, sizeof(buf));
    test_setup_cpool_block(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_cpool_block(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypePrimary, 0);
    test_setup_cpool_block(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypeCanonical, 0);
    BPLib_STOR_CACHE_InsertAfter(&buf.blk[0].u.primary.pblock.cblock_list, &buf.blk[1].header.base_link);
    buf.blk[0].u.primary.pblock.bundle_encode_size_cache  = 16;
    buf.blk[1].u.canonical.cblock.block_encode_size_cache = 32;
    buf.blk[1].u.canonical.cblock.bundle_ref              = &buf.blk[0].u.primary.pblock;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_BblockCanonicalDropEncode(&buf.blk[1].u.canonical.cblock));
    UtAssert_ZERO(buf.blk[0].u.primary.pblock.bundle_encode_size_cache);
    UtAssert_ZERO(buf.blk[1].u.canonical.cblock.block_encode_size_cache);
}

void Test_BPLib_STOR_CACHE_BblockCborExport(void)
{
    /* Test function for:
     * size_t BPLib_STOR_CACHE_BblockCborExport(BPLib_STOR_CACHE_Block_t *list, void *out_ptr, size_t max_out_size, size_t
     * seek_start, size_t max_count);
     */
    UT_BPLib_STOR_QM_Buf_t buf;
    uint8_t              output[128];

    memset(&buf, 0, sizeof(buf));

    test_setup_cpool_block(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_cpool_block(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypePrimary, 0);
    test_setup_cpool_block(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypeGeneric, BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE);

    /* call with a direct ref to a cbor block (invalid, should be a list) */
    UtAssert_ZERO(
        BPLib_STOR_CACHE_BblockCborExport(&buf.blk[1].header.base_link, output, sizeof(output), 0, sizeof(output)));

    /* nominal call, one zero-size block in the chunk_list  */
    BPLib_STOR_CACHE_InsertAfter(&buf.blk[0].u.primary.pblock.chunk_list, &buf.blk[1].header.base_link);
    UtAssert_ZERO(BPLib_STOR_CACHE_BblockCborExport(&buf.blk[0].u.primary.pblock.chunk_list, output, sizeof(output), 0,
                                                 sizeof(output)));

    /* nominal call, one nonzero-size block in the chunk_list  */
    BPLib_STOR_CACHE_BblockCborSetSize(&buf.blk[1].header.base_link, 32);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_BblockCborExport(&buf.blk[0].u.primary.pblock.chunk_list, output, sizeof(output),
                                                      0, sizeof(output)),
                       32);

    /* call with a seek_start (skips initial bytes, greater than cbor block)  */
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_BblockCborExport(&buf.blk[0].u.primary.pblock.chunk_list, output, sizeof(output),
                                                      48, sizeof(output)),
                       0);

    /* call with a seek_start (skips initial bytes, less than cbor block)  */
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_BblockCborExport(&buf.blk[0].u.primary.pblock.chunk_list, output, sizeof(output),
                                                      16, sizeof(output)),
                       16);

    /* cbor data will not fit into output (variant 1, this copies the portion that will fit) */
    UtAssert_UINT32_EQ(
        BPLib_STOR_CACHE_BblockCborExport(&buf.blk[0].u.primary.pblock.chunk_list, output, sizeof(output), 0, 16), 16);

    /* cbor data will not fit into output (variant 2, this copies none of the block) */
    UtAssert_ZERO(
        BPLib_STOR_CACHE_BblockCborExport(&buf.blk[0].u.primary.pblock.chunk_list, output, 16, 0, sizeof(output)));

    /* include a block that is NOT cbor, should stop at that point */
    test_setup_cpool_block(&buf.pool, &buf.blk[2], BPLib_STOR_CACHE_BlocktypeGeneric, ~BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE);
    BPLib_STOR_CACHE_InsertAfter(&buf.blk[0].u.primary.pblock.chunk_list, &buf.blk[2].header.base_link);
    UtAssert_ZERO(BPLib_STOR_CACHE_BblockCborExport(&buf.blk[0].u.primary.pblock.chunk_list, output, sizeof(output), 0,
                                                 sizeof(output)));
}

#ifdef QM
void Test_BplibMpoolBBlocks_Register(void)
{
    UtTest_Add(Test_BPLib_STOR_CACHE_BblockPrimaryCast, Test_BplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_BblockPrimaryCast");
    UtTest_Add(Test_BPLib_STOR_CACHE_BblockCanonicalCast, Test_BplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_BblockCanonicalCast");
    UtTest_Add(Test_BPLib_STOR_CACHE_BblockCborCast, Test_BplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_BblockCborCast");
    UtTest_Add(Test_BPLib_STOR_CACHE_BblockCborSetSize, Test_BplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_BblockCborSetSize");
    UtTest_Add(Test_BPLib_STOR_CACHE_BblockPrimaryAlloc, Test_BplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_BblockPrimaryAlloc");
    UtTest_Add(Test_BPLib_STOR_CACHE_BblockCanonicalAlloc, Test_BplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_BblockCanonicalAlloc");
    UtTest_Add(Test_BPLib_STOR_CACHE_BblockCborAlloc, Test_BplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_BblockCborAlloc");
    UtTest_Add(Test_BPLib_STOR_CACHE_BblockCborAppend, Test_BplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_BblockCborAppend");
    UtTest_Add(Test_BPLib_STOR_CACHE_BblockPrimaryAppend, Test_BplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_BblockPrimaryAppend");
    UtTest_Add(Test_BPLib_STOR_CACHE_BblockPrimaryLocateCanonical, Test_BplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_BblockPrimaryLocateCanonical");
    UtTest_Add(Test_BPLib_STOR_CACHE_BblockPrimaryDropEncode, Test_BplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_BblockPrimaryDropEncode");
    UtTest_Add(Test_BPLib_STOR_CACHE_BblockCanonicalDropEncode, Test_BplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_BblockCanonicalDropEncode");
    UtTest_Add(Test_BPLib_STOR_CACHE_BblockCborExport, Test_BplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_BblockCborExport");
}
#endif // QM
