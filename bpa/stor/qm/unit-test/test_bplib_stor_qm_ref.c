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
#include "bplib_stor_cache_ref.h"

void test_BPLib_STOR_CACHE_RefCreate(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Ref_t BPLib_STOR_CACHE_RefCreate(BPLib_STOR_CACHE_Block_t *blk)
     */
    BPLib_STOR_CACHE_BlockContent_t my_block;

    UtAssert_NULL(BPLib_STOR_CACHE_RefCreate(NULL));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_NULL(BPLib_STOR_CACHE_RefCreate(&my_block.header.base_link));

    test_setup_cpool_block(NULL, &my_block, BPLib_STOR_CACHE_BlocktypeGeneric, 0);
    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_RefCreate(&my_block.header.base_link), &my_block);
    UtAssert_UINT16_EQ(my_block.header.refcount, 1);
}

void test_BPLib_STOR_CACHE_RefDuplicate(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Ref_t BPLib_STOR_CACHE_RefDuplicate(BPLib_STOR_CACHE_Ref_t refptr)
     */
    BPLib_STOR_CACHE_BlockContent_t my_block;

    memset(&my_block, 0, sizeof(my_block));
    my_block.header.refcount = 1;

    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_RefDuplicate(&my_block), &my_block);
    UtAssert_UINT16_EQ(my_block.header.refcount, 2);
}

void test_BPLib_STOR_CACHE_RefFromBlock(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Ref_t BPLib_STOR_CACHE_RefFromBlock(BPLib_STOR_CACHE_Block_t *rblk)
     */
    BPLib_STOR_CACHE_BlockContent_t my_block;
    BPLib_STOR_CACHE_BlockContent_t my_ref;

    memset(&my_block, 0, sizeof(my_block));
    memset(&my_ref, 0, sizeof(my_ref));

    UtAssert_NULL(BPLib_STOR_CACHE_RefFromBlock(NULL));
    UtAssert_NULL(BPLib_STOR_CACHE_RefFromBlock(&my_block.header.base_link));

    test_setup_cpool_block(NULL, &my_ref, BPLib_STOR_CACHE_BlocktypeRef, 0);
    my_ref.u.ref.pref_target = &my_block;
    my_block.header.refcount = 1;
    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_RefFromBlock(&my_ref.header.base_link), &my_block);
}

void test_BPLib_STOR_CACHE_RefRelease(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_RefRelease(BPLib_STOR_CACHE_Ref_t refptr)
     */
    UT_BPLib_STOR_QM_Buf_t               buf;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_RefRelease(NULL));

    memset(&buf, 0, sizeof(buf));
    test_setup_cpool_block(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_cpool_block(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypeGeneric, 0);
    buf.blk[0].header.refcount = 2;

    admin = BPLib_STOR_CACHE_GetAdmin(&buf.pool);

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_RefRelease(&buf.blk[0]));
    UtAssert_UINT16_EQ(buf.blk[0].header.refcount, 1);
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsEmptyListHead(&admin->recycle_blocks->block_list));
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsLinkUnattached(&buf.blk[0].header.base_link));

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_RefRelease(&buf.blk[0]));
    UtAssert_ZERO(buf.blk[0].header.refcount);
    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_GetPrevBlock(&admin->recycle_blocks->block_list), &buf.blk[0]);

    test_setup_cpool_block(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypeGeneric, 0);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_RefRelease(&buf.blk[1]));
    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_GetPrevBlock(&admin->recycle_blocks->block_list), &buf.blk[1]);
}

void test_BPLib_STOR_CACHE_RefMakeBlock(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_RefMakeBlock(BPLib_STOR_CACHE_Ref_t refptr, uint32_t magic_number, void *init_arg)
     */
    UT_BPLib_STOR_QM_Buf_t buf;

    memset(&buf, 0, sizeof(buf));

    /* Nominal (need to do each blocktype that has a different init) */
    test_setup_cpool_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    test_setup_cpool_block(&buf.pool, &buf.blk[2], BPLib_STOR_CACHE_BlocktypeGeneric, 0);
    buf.blk[2].header.refcount = 1;

    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_RefMakeBlock(&buf.blk[2], 0, NULL), &buf.blk[0]);
    UtAssert_ADDRESS_EQ(buf.blk[0].u.ref.pref_target, &buf.blk[2]);

    /* Trying again should fail because the free list is now empty */
    UtAssert_NULL(BPLib_STOR_CACHE_RefMakeBlock(&buf.blk[2], 0, NULL));
}

void Test_BplibMpoolRef_Register(void)
{
    UtTest_Add(test_BPLib_STOR_CACHE_RefCreate, Test_BplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_RefCreate");
    UtTest_Add(test_BPLib_STOR_CACHE_RefDuplicate, Test_BplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_RefDuplicate");
    UtTest_Add(test_BPLib_STOR_CACHE_RefFromBlock, Test_BplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_RefFromBlock");
    UtTest_Add(test_BPLib_STOR_CACHE_RefRelease, Test_BplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_RefRelease");
    UtTest_Add(test_BPLib_STOR_CACHE_RefMakeBlock, Test_BplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_RefMakeBlock");
}
