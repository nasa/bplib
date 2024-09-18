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

#include "test_bplib_mpool.h"

void test_bplib_mpool_bblock_primary_cast(void)
{
    /* Test function for:
     * bplib_mpool_bblock_primary_t *bplib_mpool_bblock_primary_cast(bplib_mpool_block_t *cb);
     */
    bplib_mpool_block_content_t   my_block;
    bplib_mpool_bblock_primary_t *pri;
    bplib_mpool_block_t          *cb = &my_block.header.base_link;

    UtAssert_NULL(bplib_mpool_bblock_primary_cast(NULL));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_NULL(bplib_mpool_bblock_primary_cast(cb));

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_primary, 0);
    UtAssert_ADDRESS_EQ(pri = bplib_mpool_bblock_primary_cast(cb), &my_block.u);

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_canonical, 0);
    UtAssert_NULL(bplib_mpool_bblock_primary_cast(cb));

    /* Ensure that the logical accessors work (inline) */
    UtAssert_NOT_NULL(bplib_mpool_bblock_primary_get_logical(pri));
}

void test_bplib_mpool_bblock_canonical_cast(void)
{
    /* Test function for:
     * bplib_mpool_bblock_canonical_t *bplib_mpool_bblock_canonical_cast(bplib_mpool_block_t *cb);
     */
    bplib_mpool_block_content_t     my_block;
    bplib_mpool_bblock_canonical_t *canonical;
    bplib_mpool_block_t            *cb = &my_block.header.base_link;

    UtAssert_NULL(bplib_mpool_bblock_canonical_cast(NULL));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_NULL(bplib_mpool_bblock_canonical_cast(cb));

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_canonical, 0);
    UtAssert_ADDRESS_EQ(canonical = bplib_mpool_bblock_canonical_cast(cb), &my_block.u);

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_primary, 0);
    UtAssert_NULL(bplib_mpool_bblock_canonical_cast(cb));

    /* Ensure that the logical accessors work (inline) */
    UtAssert_NOT_NULL(bplib_mpool_bblock_canonical_get_logical(canonical));
    UtAssert_VOIDCALL(bplib_mpool_bblock_canonical_set_content_position(canonical, 1, 2));
    UtAssert_UINT32_EQ(bplib_mpool_bblock_canonical_get_content_length(canonical), 2);
    UtAssert_UINT32_EQ(bplib_mpool_bblock_canonical_get_content_offset(canonical), 1);
}

void test_bplib_mpool_bblock_cbor_cast(void)
{
    /* Test function for:
     * void *bplib_mpool_bblock_cbor_cast(bplib_mpool_block_t *cb);
     */
    bplib_mpool_block_content_t my_block;
    bplib_mpool_block_t        *cb = &my_block.header.base_link;

    UtAssert_NULL(bplib_mpool_bblock_cbor_cast(NULL));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_NULL(bplib_mpool_bblock_cbor_cast(cb));

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_generic, MPOOL_CACHE_CBOR_DATA_SIGNATURE);
    UtAssert_ADDRESS_EQ(bplib_mpool_bblock_cbor_cast(cb), &my_block.u);
}

void test_bplib_mpool_bblock_cbor_set_size(void)
{
    /* Test function for:
     * void bplib_mpool_bblock_cbor_set_size(bplib_mpool_block_t *cb, size_t user_content_size);
     */
    bplib_mpool_block_content_t my_block;

    UtAssert_VOIDCALL(bplib_mpool_bblock_cbor_set_size(NULL, 0));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_VOIDCALL(bplib_mpool_bblock_cbor_set_size(&my_block.header.base_link, 0));

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_generic, ~MPOOL_CACHE_CBOR_DATA_SIGNATURE);
    UtAssert_VOIDCALL(bplib_mpool_bblock_cbor_set_size(&my_block.header.base_link, 0));

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_canonical, MPOOL_CACHE_CBOR_DATA_SIGNATURE);
    UtAssert_VOIDCALL(bplib_mpool_bblock_cbor_set_size(&my_block.header.base_link, 0));

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_generic, MPOOL_CACHE_CBOR_DATA_SIGNATURE);
    UtAssert_VOIDCALL(bplib_mpool_bblock_cbor_set_size(&my_block.header.base_link, 123));

    UtAssert_UINT32_EQ(my_block.header.user_content_length, 123);
}

void test_bplib_mpool_bblock_primary_alloc(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_bblock_primary_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg,
     * uint8_t priority, uint64_t timeout);
     */
    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(bplib_mpool_bblock_primary_alloc(&buf.pool, 1234, NULL, 0, 0));

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    UtAssert_ADDRESS_EQ(bplib_mpool_bblock_primary_alloc(&buf.pool, 1234, NULL, 0, 0), &buf.blk[0]);
}

void test_bplib_mpool_bblock_canonical_alloc(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_bblock_canonical_alloc(bplib_mpool_t *pool, uint32_t magic_number, void
     * *init_arg);
     */

    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(bplib_mpool_bblock_canonical_alloc(&buf.pool, 1234, NULL));

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    UtAssert_ADDRESS_EQ(bplib_mpool_bblock_canonical_alloc(&buf.pool, 1234, NULL), &buf.blk[0]);
}

void test_bplib_mpool_bblock_cbor_alloc(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_bblock_cbor_alloc(bplib_mpool_t *pool);
     */

    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(bplib_mpool_bblock_cbor_alloc(&buf.pool));

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    UtAssert_ADDRESS_EQ(bplib_mpool_bblock_cbor_alloc(&buf.pool), &buf.blk[0]);
}

void test_bplib_mpool_bblock_cbor_append(void)
{
    /* Test function for:
     * void bplib_mpool_bblock_cbor_append(bplib_mpool_block_t *head, bplib_mpool_block_t *blk);
     */
    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_generic, MPOOL_CACHE_CBOR_DATA_SIGNATURE);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_generic, MPOOL_CACHE_CBOR_DATA_SIGNATURE);

    UtAssert_VOIDCALL(bplib_mpool_bblock_cbor_append(&buf.blk[0].header.base_link, &buf.blk[1].header.base_link));
}

void test_bplib_mpool_bblock_primary_append(void)
{
    /* Test function for:
     * void bplib_mpool_bblock_primary_append(bplib_mpool_bblock_primary_t *cpb, bplib_mpool_block_t *ccb);
     */

    UT_bplib_mpool_buf_t         buf;
    bp_canonical_block_buffer_t *b;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_primary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_canonical, 0);

    UtAssert_VOIDCALL(bplib_mpool_bblock_primary_append(&buf.blk[0].u.primary.pblock, &buf.blk[1].header.base_link));

    /* Test the special case if inserting blocknum 1, it should go last */
    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_canonical, 0);
    b                           = bplib_mpool_bblock_canonical_get_logical(&buf.blk[2].u.canonical.cblock);
    b->canonical_block.blockNum = 1;
    UtAssert_VOIDCALL(bplib_mpool_bblock_primary_append(&buf.blk[0].u.primary.pblock, &buf.blk[2].header.base_link));
}

void test_bplib_mpool_bblock_primary_locate_canonical(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_bblock_primary_locate_canonical(bplib_mpool_bblock_primary_t *cpb,
     * bp_blocktype_t block_type);
     */

    UT_bplib_mpool_buf_t         buf;
    bp_canonical_block_buffer_t *b;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_primary, 0);
    UtAssert_NULL(bplib_mpool_bblock_primary_locate_canonical(&buf.blk[0].u.primary.pblock, bp_blocktype_undefined));

    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_canonical, 0);
    bplib_mpool_insert_after(&buf.blk[0].u.primary.pblock.cblock_list, &buf.blk[1].header.base_link);
    b                            = bplib_mpool_bblock_canonical_get_logical(&buf.blk[1].u.canonical.cblock);
    b->canonical_block.blockType = bp_blocktype_bundleAge;

    /* canonical block of a different type should not be found */
    UtAssert_NULL(bplib_mpool_bblock_primary_locate_canonical(&buf.blk[0].u.primary.pblock, bp_blocktype_hopCount));

    /* canonical block of the correct type should be found */
    UtAssert_ADDRESS_EQ(
        bplib_mpool_bblock_primary_locate_canonical(&buf.blk[0].u.primary.pblock, bp_blocktype_bundleAge), &buf.blk[1]);

    /* include a block that is NOT a canonical block */
    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_generic, 0);
    bplib_mpool_insert_before(&buf.blk[0].u.primary.pblock.cblock_list, &buf.blk[2].header.base_link);
    UtAssert_NULL(bplib_mpool_bblock_primary_locate_canonical(&buf.blk[0].u.primary.pblock, bp_blocktype_hopCount));
}

void test_bplib_mpool_bblock_primary_drop_encode(void)
{
    /* Test function for:
     * void bplib_mpool_bblock_primary_drop_encode(bplib_mpool_bblock_primary_t *cpb);
     */
    UT_bplib_mpool_buf_t               buf;
    bplib_mpool_block_admin_content_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    admin = bplib_mpool_get_admin(&buf.pool);

    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_primary, 0);
    UtAssert_VOIDCALL(bplib_mpool_bblock_primary_drop_encode(&buf.blk[0].u.primary.pblock));

    /* confirms nothing got recycled (empty cbor block list) */
    UtAssert_BOOL_TRUE(bplib_mpool_is_empty_list_head(&admin->recycle_blocks->block_list));

    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_generic, MPOOL_CACHE_CBOR_DATA_SIGNATURE);
    bplib_mpool_insert_after(&buf.blk[0].u.primary.pblock.chunk_list, &buf.blk[1].header.base_link);
    UtAssert_VOIDCALL(bplib_mpool_bblock_primary_drop_encode(&buf.blk[0].u.primary.pblock));

    /* confirms that cbor block got recycled */
    UtAssert_BOOL_FALSE(bplib_mpool_is_empty_list_head(&admin->recycle_blocks->block_list));
}

void test_bplib_mpool_bblock_canonical_drop_encode(void)
{
    /* Test function for:
     * void bplib_mpool_bblock_canonical_drop_encode(bplib_mpool_bblock_canonical_t *ccb);
     */
    UT_bplib_mpool_buf_t               buf;
    bplib_mpool_block_admin_content_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    admin = bplib_mpool_get_admin(&buf.pool);

    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_canonical, 0);
    buf.blk[0].u.canonical.cblock.block_encode_size_cache = 32;
    UtAssert_VOIDCALL(bplib_mpool_bblock_canonical_drop_encode(&buf.blk[0].u.canonical.cblock));
    UtAssert_ZERO(buf.blk[0].u.canonical.cblock.block_encode_size_cache);

    /* confirms nothing got recycled (empty cbor block list) */
    UtAssert_BOOL_TRUE(bplib_mpool_is_empty_list_head(&admin->recycle_blocks->block_list));

    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_generic, MPOOL_CACHE_CBOR_DATA_SIGNATURE);
    buf.blk[0].u.canonical.cblock.block_encode_size_cache = 32;
    bplib_mpool_insert_after(&buf.blk[0].u.canonical.cblock.chunk_list, &buf.blk[1].header.base_link);
    UtAssert_VOIDCALL(bplib_mpool_bblock_canonical_drop_encode(&buf.blk[0].u.canonical.cblock));
    UtAssert_ZERO(buf.blk[0].u.canonical.cblock.block_encode_size_cache);

    /* confirms that cbor block got recycled */
    UtAssert_BOOL_FALSE(bplib_mpool_is_empty_list_head(&admin->recycle_blocks->block_list));

    /* confirm that a primary block associated with this canonical block is also invalidated */
    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_primary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_canonical, 0);
    bplib_mpool_insert_after(&buf.blk[0].u.primary.pblock.cblock_list, &buf.blk[1].header.base_link);
    buf.blk[0].u.primary.pblock.bundle_encode_size_cache  = 16;
    buf.blk[1].u.canonical.cblock.block_encode_size_cache = 32;
    buf.blk[1].u.canonical.cblock.bundle_ref              = &buf.blk[0].u.primary.pblock;
    UtAssert_VOIDCALL(bplib_mpool_bblock_canonical_drop_encode(&buf.blk[1].u.canonical.cblock));
    UtAssert_ZERO(buf.blk[0].u.primary.pblock.bundle_encode_size_cache);
    UtAssert_ZERO(buf.blk[1].u.canonical.cblock.block_encode_size_cache);
}

void test_bplib_mpool_bblock_cbor_export(void)
{
    /* Test function for:
     * size_t bplib_mpool_bblock_cbor_export(bplib_mpool_block_t *list, void *out_ptr, size_t max_out_size, size_t
     * seek_start, size_t max_count);
     */
    UT_bplib_mpool_buf_t buf;
    uint8_t              output[128];

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_primary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_generic, MPOOL_CACHE_CBOR_DATA_SIGNATURE);

    /* call with a direct ref to a cbor block (invalid, should be a list) */
    UtAssert_ZERO(
        bplib_mpool_bblock_cbor_export(&buf.blk[1].header.base_link, output, sizeof(output), 0, sizeof(output)));

    /* nominal call, one zero-size block in the chunk_list  */
    bplib_mpool_insert_after(&buf.blk[0].u.primary.pblock.chunk_list, &buf.blk[1].header.base_link);
    UtAssert_ZERO(bplib_mpool_bblock_cbor_export(&buf.blk[0].u.primary.pblock.chunk_list, output, sizeof(output), 0,
                                                 sizeof(output)));

    /* nominal call, one nonzero-size block in the chunk_list  */
    bplib_mpool_bblock_cbor_set_size(&buf.blk[1].header.base_link, 32);
    UtAssert_UINT32_EQ(bplib_mpool_bblock_cbor_export(&buf.blk[0].u.primary.pblock.chunk_list, output, sizeof(output),
                                                      0, sizeof(output)),
                       32);

    /* call with a seek_start (skips initial bytes, greater than cbor block)  */
    UtAssert_UINT32_EQ(bplib_mpool_bblock_cbor_export(&buf.blk[0].u.primary.pblock.chunk_list, output, sizeof(output),
                                                      48, sizeof(output)),
                       0);

    /* call with a seek_start (skips initial bytes, less than cbor block)  */
    UtAssert_UINT32_EQ(bplib_mpool_bblock_cbor_export(&buf.blk[0].u.primary.pblock.chunk_list, output, sizeof(output),
                                                      16, sizeof(output)),
                       16);

    /* cbor data will not fit into output (variant 1, this copies the portion that will fit) */
    UtAssert_UINT32_EQ(
        bplib_mpool_bblock_cbor_export(&buf.blk[0].u.primary.pblock.chunk_list, output, sizeof(output), 0, 16), 16);

    /* cbor data will not fit into output (variant 2, this copies none of the block) */
    UtAssert_ZERO(
        bplib_mpool_bblock_cbor_export(&buf.blk[0].u.primary.pblock.chunk_list, output, 16, 0, sizeof(output)));

    /* include a block that is NOT cbor, should stop at that point */
    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_generic, ~MPOOL_CACHE_CBOR_DATA_SIGNATURE);
    bplib_mpool_insert_after(&buf.blk[0].u.primary.pblock.chunk_list, &buf.blk[2].header.base_link);
    UtAssert_ZERO(bplib_mpool_bblock_cbor_export(&buf.blk[0].u.primary.pblock.chunk_list, output, sizeof(output), 0,
                                                 sizeof(output)));
}

void TestBplibMpoolBBlocks_Register(void)
{
    UtTest_Add(test_bplib_mpool_bblock_primary_cast, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_bblock_primary_cast");
    UtTest_Add(test_bplib_mpool_bblock_canonical_cast, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_bblock_canonical_cast");
    UtTest_Add(test_bplib_mpool_bblock_cbor_cast, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_bblock_cbor_cast");
    UtTest_Add(test_bplib_mpool_bblock_cbor_set_size, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_bblock_cbor_set_size");
    UtTest_Add(test_bplib_mpool_bblock_primary_alloc, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_bblock_primary_alloc");
    UtTest_Add(test_bplib_mpool_bblock_canonical_alloc, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_bblock_canonical_alloc");
    UtTest_Add(test_bplib_mpool_bblock_cbor_alloc, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_bblock_cbor_alloc");
    UtTest_Add(test_bplib_mpool_bblock_cbor_append, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_bblock_cbor_append");
    UtTest_Add(test_bplib_mpool_bblock_primary_append, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_bblock_primary_append");
    UtTest_Add(test_bplib_mpool_bblock_primary_locate_canonical, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_bblock_primary_locate_canonical");
    UtTest_Add(test_bplib_mpool_bblock_primary_drop_encode, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_bblock_primary_drop_encode");
    UtTest_Add(test_bplib_mpool_bblock_canonical_drop_encode, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_bblock_canonical_drop_encode");
    UtTest_Add(test_bplib_mpool_bblock_cbor_export, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_bblock_cbor_export");
}
