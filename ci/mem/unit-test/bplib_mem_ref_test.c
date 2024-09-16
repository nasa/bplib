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

#include "bplib_mem_internal.h"

void test_bplib_mpool_ref_create(void)
{
    /* Test function for:
     * bplib_mpool_ref_t bplib_mpool_ref_create(bplib_mpool_block_t *blk)
     */
    bplib_mpool_block_content_t my_block;

    UtAssert_NULL(bplib_mpool_ref_create(NULL));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_NULL(bplib_mpool_ref_create(&my_block.header.base_link));

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_generic, 0);
    UtAssert_ADDRESS_EQ(bplib_mpool_ref_create(&my_block.header.base_link), &my_block);
    UtAssert_UINT16_EQ(my_block.header.refcount, 1);
}

void test_bplib_mpool_ref_duplicate(void)
{
    /* Test function for:
     * bplib_mpool_ref_t bplib_mpool_ref_duplicate(bplib_mpool_ref_t refptr)
     */
    bplib_mpool_block_content_t my_block;

    memset(&my_block, 0, sizeof(my_block));
    my_block.header.refcount = 1;

    UtAssert_ADDRESS_EQ(bplib_mpool_ref_duplicate(&my_block), &my_block);
    UtAssert_UINT16_EQ(my_block.header.refcount, 2);
}

void test_bplib_mpool_ref_from_block(void)
{
    /* Test function for:
     * bplib_mpool_ref_t bplib_mpool_ref_from_block(bplib_mpool_block_t *rblk)
     */
    bplib_mpool_block_content_t my_block;
    bplib_mpool_block_content_t my_ref;

    memset(&my_block, 0, sizeof(my_block));
    memset(&my_ref, 0, sizeof(my_ref));

    UtAssert_NULL(bplib_mpool_ref_from_block(NULL));
    UtAssert_NULL(bplib_mpool_ref_from_block(&my_block.header.base_link));

    test_setup_mpblock(NULL, &my_ref, bplib_mpool_blocktype_ref, 0);
    my_ref.u.ref.pref_target = &my_block;
    my_block.header.refcount = 1;
    UtAssert_ADDRESS_EQ(bplib_mpool_ref_from_block(&my_ref.header.base_link), &my_block);
}

void test_bplib_mpool_ref_release(void)
{
    /* Test function for:
     * void bplib_mpool_ref_release(bplib_mpool_ref_t refptr)
     */
    UT_bplib_mpool_buf_t               buf;
    bplib_mpool_block_admin_content_t *admin;

    UtAssert_VOIDCALL(bplib_mpool_ref_release(NULL));

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_generic, 0);
    buf.blk[0].header.refcount = 2;

    admin = bplib_mpool_get_admin(&buf.pool);

    UtAssert_VOIDCALL(bplib_mpool_ref_release(&buf.blk[0]));
    UtAssert_UINT16_EQ(buf.blk[0].header.refcount, 1);
    UtAssert_BOOL_TRUE(bplib_mpool_is_empty_list_head(&admin->recycle_blocks.block_list));
    UtAssert_BOOL_TRUE(bplib_mpool_is_link_unattached(&buf.blk[0].header.base_link));

    UtAssert_VOIDCALL(bplib_mpool_ref_release(&buf.blk[0]));
    UtAssert_ZERO(buf.blk[0].header.refcount);
    UtAssert_ADDRESS_EQ(bplib_mpool_get_prev_block(&admin->recycle_blocks.block_list), &buf.blk[0]);

    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_generic, 0);
    UtAssert_VOIDCALL(bplib_mpool_ref_release(&buf.blk[1]));
    UtAssert_ADDRESS_EQ(bplib_mpool_get_prev_block(&admin->recycle_blocks.block_list), &buf.blk[1]);
}

void test_bplib_mpool_ref_make_block(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_ref_make_block(bplib_mpool_ref_t refptr, uint32_t magic_number, void *init_arg)
     */
    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));

    /* Nominal (need to do each blocktype that has a different init) */
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_generic, 0);
    buf.blk[2].header.refcount = 1;

    UtAssert_ADDRESS_EQ(bplib_mpool_ref_make_block(&buf.blk[2], 0, NULL), &buf.blk[0]);
    UtAssert_ADDRESS_EQ(buf.blk[0].u.ref.pref_target, &buf.blk[2]);

    /* Trying again should fail because the free list is now empty */
    UtAssert_NULL(bplib_mpool_ref_make_block(&buf.blk[2], 0, NULL));
}

void TestBplibMpoolRef_Register(void)
{
    UtTest_Add(test_bplib_mpool_ref_create, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_ref_create");
    UtTest_Add(test_bplib_mpool_ref_duplicate, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_ref_duplicate");
    UtTest_Add(test_bplib_mpool_ref_from_block, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_ref_from_block");
    UtTest_Add(test_bplib_mpool_ref_release, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_ref_release");
    UtTest_Add(test_bplib_mpool_ref_make_block, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_ref_make_block");
}
