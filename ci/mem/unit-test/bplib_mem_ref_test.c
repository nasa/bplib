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
#include "test_BPLib_MEM.h"

void test_BPLib_MEM_RefCreate(void)
{
    /* Test function for:
     * BPLib_MEM_ref_t BPLib_MEM_RefCreate(BPLib_MEM_block_t *blk)
     */
    BPLib_MEM_BlockContent_t my_block;

    UtAssert_NULL(BPLib_MEM_RefCreate(NULL));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_NULL(BPLib_MEM_RefCreate(&my_block.header.base_link));

    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypeGeneric, 0);
    UtAssert_ADDRESS_EQ(BPLib_MEM_RefCreate(&my_block.header.base_link), &my_block);
    UtAssert_UINT16_EQ(my_block.header.refcount, 1);
}

void test_BPLib_MEM_RefDuplicate(void)
{
    /* Test function for:
     * BPLib_MEM_ref_t BPLib_MEM_RefDuplicate(BPLib_MEM_ref_t refptr)
     */
    BPLib_MEM_BlockContent_t my_block;

    memset(&my_block, 0, sizeof(my_block));
    my_block.header.refcount = 1;

    UtAssert_ADDRESS_EQ(BPLib_MEM_RefDuplicate(&my_block), &my_block);
    UtAssert_UINT16_EQ(my_block.header.refcount, 2);
}

void test_BPLib_MEM_RefFromBlock(void)
{
    /* Test function for:
     * BPLib_MEM_ref_t BPLib_MEM_RefFromBlock(BPLib_MEM_block_t *rblk)
     */
    BPLib_MEM_BlockContent_t my_block;
    BPLib_MEM_BlockContent_t my_ref;

    memset(&my_block, 0, sizeof(my_block));
    memset(&my_ref, 0, sizeof(my_ref));

    UtAssert_NULL(BPLib_MEM_RefFromBlock(NULL));
    UtAssert_NULL(BPLib_MEM_RefFromBlock(&my_block.header.base_link));

    test_setup_mpblock(NULL, &my_ref, BPLib_MEM_BlocktypeRef, 0);
    my_ref.u.ref.pref_target = &my_block;
    my_block.header.refcount = 1;
    UtAssert_ADDRESS_EQ(BPLib_MEM_RefFromBlock(&my_ref.header.base_link), &my_block);
}

void test_BPLib_MEM_RefRelease(void)
{
    #ifdef STOR // subq
    /* Test function for:
     * void BPLib_MEM_RefRelease(BPLib_MEM_ref_t refptr)
     */
    UT_BPLib_MEM_buf_t               buf;
    BPLib_MEM_BlockAdminContent_t *admin;

    UtAssert_VOIDCALL(BPLib_MEM_RefRelease(NULL));

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeGeneric, 0);
    buf.blk[0].header.refcount = 2;

    admin = BPLib_MEM_GetAdmin(&buf.pool);

    UtAssert_VOIDCALL(BPLib_MEM_RefRelease(&buf.blk[0]));
    UtAssert_UINT16_EQ(buf.blk[0].header.refcount, 1);
    // STOR subq UtAssert_BOOL_TRUE(BPLib_MEM_IsEmptyListHead(&admin->recycle_blocks->block_list));
    UtAssert_BOOL_TRUE(BPLib_MEM_IsLinkUnattached(&buf.blk[0].header.base_link));

    UtAssert_VOIDCALL(BPLib_MEM_RefRelease(&buf.blk[0]));
    UtAssert_ZERO(buf.blk[0].header.refcount);
    // STOR subq UtAssert_ADDRESS_EQ(BPLib_MEM_GetPrevBlock(&admin->recycle_blocks->block_list), &buf.blk[0]);

    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeGeneric, 0);
    UtAssert_VOIDCALL(BPLib_MEM_RefRelease(&buf.blk[1]));
    // STOR subq UtAssert_ADDRESS_EQ(BPLib_MEM_GetPrevBlock(&admin->recycle_blocks->block_list), &buf.blk[1]);
    #endif // STOR subq
}

void test_BPLib_MEM_RefMakeBlock(void)
{
    /* Test function for:
     * BPLib_MEM_block_t *BPLib_MEM_RefMakeBlock(BPLib_MEM_ref_t refptr, uint32_t magic_number, void *init_arg)
     */
    UT_BPLib_MEM_buf_t buf;

    memset(&buf, 0, sizeof(buf));

    /* Nominal (need to do each blocktype that has a different init) */
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeGeneric, 0);
    buf.blk[2].header.refcount = 1;

    UtAssert_ADDRESS_EQ(BPLib_MEM_RefMakeBlock(&buf.blk[2], 0, NULL), &buf.blk[0]);
    UtAssert_ADDRESS_EQ(buf.blk[0].u.ref.pref_target, &buf.blk[2]);

    /* Trying again should fail because the free list is now empty */
    UtAssert_NULL(BPLib_MEM_RefMakeBlock(&buf.blk[2], 0, NULL));
}

void TestBplibMpoolRef_Register(void)
{
    UtTest_Add(test_BPLib_MEM_RefCreate, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_RefCreate");
    UtTest_Add(test_BPLib_MEM_RefDuplicate, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_RefDuplicate");
    UtTest_Add(test_BPLib_MEM_RefFromBlock, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_RefFromBlock");
    UtTest_Add(test_BPLib_MEM_RefRelease, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_RefRelease");
    UtTest_Add(test_BPLib_MEM_RefMakeBlock, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_RefMakeBlock");
}
