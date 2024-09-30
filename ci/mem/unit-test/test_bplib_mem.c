/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/*
 * Include
 */

#include <string.h>

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_mem_test_utils.h"

#include "test_bplib_mem.h"

#include "bplib_mem.h"
#include "bplib_mem_internal.h"

const uint32 UT_TESTBLOCKTYPE_SIG = 0x5f33c01a;

#define TEST_BPLIB_MEM_POOL_SIZE 16384
void *BPLib_MEM_TestPoolPtr;

void UT_AltHandler_PointerReturnForSignature(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bp_val_t RefSig = UT_Hook_GetArgValueByName(Context, "search_key_value", bp_val_t);

    if (RefSig == UT_TESTBLOCKTYPE_SIG)
    {
        UserObj = NULL;
    }

    UT_Stub_SetReturnValue(FuncKey, UserObj);
}

void *TestUtil_BPLib_OS_Calloc(size_t size)
{
    /* Allocate Memory Block */
    return calloc(size, 1);
}

void TestUtil_BPLib_OS_Free(void *ptr)  // TODO Verify usage
{
    /* Free Memory Block */
    free(ptr);
}

// Memory Allocator Tests

/**
 * Test_BPLib_MEM_Init - Test function for int BPLib_MEM_Init()
 */
void Test_BPLib_MEM_Init(void)
{
    UtAssert_INT32_EQ(BPLib_MEM_Init(), BPLIB_SUCCESS);
}

/**
 * Test_BPLIB_MEM_PoolCreate
 */
void Test_BPLib_MEM_PoolCreate(void)
{
    BPLib_MEM_TestPoolPtr = TestUtil_BPLib_OS_Calloc(TEST_BPLIB_MEM_POOL_SIZE);
    UtAssert_NOT_NULL(BPLib_MEM_TestPoolPtr);
    UtAssert_NOT_NULL(BPLib_MEM_PoolCreate(BPLib_MEM_TestPoolPtr, TEST_BPLIB_MEM_POOL_SIZE));
}

void Test_BPLib_MEM_InitListHead(void)
{
    /* Test function for:
     * void BPLib_MEM_InitListHead(BPLib_MEM_Block_t *base_block, BPLib_MEM_Block_t *list_head)
     */
    BPLib_MEM_BlockContent_t my_block;

    memset(&my_block, 0, sizeof(my_block));

    UtAssert_VOIDCALL(BPLib_MEM_InitListHead(&my_block.header.base_link,
                                             &my_block.u.admin.recycle_blocks.block_list));
}

void Test_BPLib_MEM_SubqInit(void)
{
    BPLib_MEM_BlockContent_t my_block;
    memset(&my_block, 0, sizeof(my_block));
    
    UtAssert_VOIDCALL(BPLib_MEM_SubqInit(&my_block.header.base_link, &my_block.u.admin.free_blocks));
}

void Test_BPLib_MEM_BlockFromExternalId(void)
{
    /* Test function for:
     * BPLib_MEM_Block_t *BPLib_MEM_BlockFromExternalId(bplib_mpool_t *pool, bp_handle_t handle)
     */
    UT_BPLib_MEM_Buf_t   buf;
    bp_handle_t          id1;
    bp_handle_t          id2;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(BPLib_MEM_BlockFromExternalId(&buf.pool, BP_INVALID_HANDLE));

    
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeGeneric, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeGeneric, 0);
    
    UtAssert_BOOL_TRUE(bp_handle_is_valid(id1 = BPLib_MEM_GetExternalId(&buf.blk[0].header.base_link)));
    UtAssert_BOOL_TRUE(bp_handle_is_valid(id2 = BPLib_MEM_GetExternalId(&buf.blk[1].header.base_link)));
    UtAssert_NULL(BPLib_MEM_BlockFromExternalId(&buf.pool, id1));
    UtAssert_NULL(BPLib_MEM_BlockFromExternalId(&buf.pool, id2));

    buf.pool.admin_block.u.admin.num_bufs_total = 3;
    UtAssert_ADDRESS_EQ(BPLib_MEM_BlockFromExternalId(&buf.pool, id1), &buf.blk[0].header.base_link);
    UtAssert_ADDRESS_EQ(BPLib_MEM_BlockFromExternalId(&buf.pool, id2), &buf.blk[1].header.base_link);
}

void Test_BPLib_MEM_GetGenericDataCapacity(void)
{
    /* Test function for:
     * size_t BPLib_MEM_GetGenericDataCapacity(const BPLib_MEM_Block_t *cb)
     */
    BPLib_MEM_BlockContent_t my_block;

    memset(&my_block, 0, sizeof(my_block));

    UtAssert_ZERO(BPLib_MEM_GetGenericDataCapacity(&my_block.header.base_link));

    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypeGeneric, 0);
    UtAssert_UINT32_EQ(BPLib_MEM_GetGenericDataCapacity(&my_block.header.base_link),
                       sizeof(BPLib_MEM_BlockBuffer_t));
}

void Test_BPLib_MEM_Maintain(void)
{
    /* Test function for:
     * void BPLib_MEM_Maintain(BPLib_MEM_Pool_t *pool)
     */
    UT_BPLib_MEM_Buf_t               buf;
    BPLib_MEM_BlockAdminContent_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    UtAssert_VOIDCALL(BPLib_MEM_Maintain(&buf.pool));

    admin = BPLib_MEM_GetAdmin(&buf.pool);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_AltHandler_PointerReturn, NULL);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeGeneric, 0);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, &buf.blk[0].header.base_link);

    UtAssert_VOIDCALL(BPLib_MEM_Maintain(&buf.pool));
}

void Test_BPLib_MEM_LockWait(void)
{
    /* Test function for:
     * bool BPLib_MEM_LockWait(BPLib_MEM_Lock_t *lock, uint64_t until_dtntime)
     */

    uint64_t time;
    BPLib_MEM_Lock_t *lock = BPLib_MEM_LockPrepare(NULL);

    UtAssert_BOOL_FALSE(BPLib_MEM_LockWait(lock, 0));
    time = BPLib_MEM_OS_GetDtnTimeMs();
    UtAssert_BOOL_TRUE(BPLib_MEM_LockWait(lock, time + 5000));
    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_OS_WaitUntilMs), BPLIB_MEM_TIMEOUT);
    UtAssert_BOOL_FALSE(BPLib_MEM_LockWait(lock, 5000));
}

void Test_BPLib_MEM_CollectBlocks(void)
{
    /* Test function for:
     * uint32_t Test_BPLib_MEM_CollectBlocks(BPLib_MEM_Pool_t *pool, uint32_t limit)
     */
    UT_BPLib_MEM_Buf_t               buf;
    BPLib_MEM_BlockAdminContent_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeApi, 0);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_AltHandler_PointerReturn, &buf.blk[0].u);
    buf.blk[0].u.api.api.destruct = Test_BPLib_MEM_CallbackStub;

    admin = BPLib_MEM_GetAdmin(&buf.pool);

    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeApi, 0);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, &buf.blk[1].header.base_link);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeApi, 0);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(BPLib_MEM_CollectBlocks(&buf.pool, 1), 1);
    UtAssert_STUB_COUNT(Test_BPLib_MEM_CallbackStub, 0);  // TODO Passes, but may be invalid check value.
    UtAssert_UINT32_EQ(BPLib_MEM_CollectBlocks(&buf.pool, 1), 1);
    UtAssert_STUB_COUNT(Test_BPLib_MEM_CallbackStub, 0);  // TODO Passes, but may be invalid check value.

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_AltHandler_PointerReturn, NULL);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeGeneric, 0);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, &buf.blk[0].header.base_link);

    UtAssert_UINT32_EQ(BPLib_MEM_CollectBlocks(&buf.pool, 10), 1);
}

#ifdef EXAMP
void test_bplib_mpool_recycle_block(void)
{
    /* Test function for:
     * void bplib_mpool_recycle_block(bplib_mpool_block_t *blk)
     */
    UT_bplib_mpool_buf_t               buf;
    bplib_mpool_block_admin_content_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    admin = bplib_mpool_get_admin(&buf.pool);

    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_primary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_primary, 0);

    bplib_mpool_insert_after(&admin->active_list, &buf.blk[0].header.base_link);
    bplib_mpool_insert_after(&admin->active_list, &buf.blk[1].header.base_link);

    UtAssert_VOIDCALL(bplib_mpool_recycle_block(&buf.blk[1].header.base_link));
    UtAssert_ADDRESS_EQ(bplib_mpool_get_next_block(&admin->recycle_blocks.block_list), &buf.blk[1].header.base_link);
    UtAssert_ADDRESS_EQ(bplib_mpool_get_next_block(&admin->active_list), &buf.blk[0].header.base_link);
}
#endif // EXAMP

void Test_BPLIB_MEM_RecycleBlock(void)
{
    /* Test function for:
     * void BPLib_MEM_RecycleBlock(BPLib_MEM_Block_t *blk)
     */
    UT_BPLib_MEM_Buf_t               buf;
    BPLib_MEM_BlockAdminContent_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);

    admin = BPLib_MEM_GetAdmin(&buf.pool);

    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeApi, 0);
    test_make_singleton_link(&buf.pool, &buf.blk[0].header.base_link);

    UtPrintf("InsertAfter 1");
    buf.blk[0].header.base_link.next=&buf.blk[0].header.base_link;
    BPLib_MEM_InsertAfter(&admin->active_list, &buf.blk[0].header.base_link);

    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeApi, 0);
    test_make_singleton_link(&buf.pool, &buf.blk[1].header.base_link);

    UtPrintf("&b = %lx b->next = %lx", (uint64_t)&buf.blk[1].header.base_link,
             (uint64_t)&buf.blk[1].header.base_link.next);

//    UtPrintf("InsertAfter 2");
//    BPLib_MEM_InsertAfter(&admin->active_list, &buf.blk[1].header.base_link);

    UtAssert_VOIDCALL(BPLIB_MEM_RecycleBlock(&buf.blk[1].header.base_link));
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetNextBlock(&admin->recycle_blocks.block_list), &buf.blk[1].header.base_link);
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetNextBlock(&admin->active_list), &buf.blk[0].header.base_link);
}

void Test_BPLib_MEM_RecycleAllBlocksInList(void)
{
    /* Test function for:
     * void BPLib_MEM_Recycle_allBlocksInList9R0(bplib_mpool_t *pool, BPLib_MEM_Block_t *list)
     */
    UT_BPLib_MEM_Buf_t               buf;
    BPLib_MEM_BlockAdminContent_t   *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    admin = BPLib_MEM_GetAdmin(&buf.pool);

    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeAdmin, 0);
    BPLib_MEM_InsertAfter(&admin->active_list, &buf.blk[0].header.base_link);
    BPLib_MEM_InsertAfter(&admin->active_list, &buf.blk[1].header.base_link);

    UtAssert_VOIDCALL(BPLib_MEM_RecycleAllBlocksInList(&buf.pool, &admin->active_list));

    UtAssert_BOOL_TRUE(BPLib_MEM_IsEmptyListHead(&admin->active_list));
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetNextBlock(&admin->recycle_blocks.block_list), &buf.blk[1].header.base_link);
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetPrevBlock(&admin->recycle_blocks.block_list), &buf.blk[0].header.base_link);
}

void Test_BPLib_MEM_GetParentPoolFromLink(void)
{
    /* Test function for:
     * bplib_mpool_t *BPLib_MEM_GetParentPoolFromLink(BPLib_MEM_Block_t *cb)
     */

    UT_BPLib_MEM_Buf_t buf;
    BPLib_MEM_BlockAdminContent_t   *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    admin = BPLib_MEM_GetAdmin(&buf.pool);

    UtAssert_NULL(BPLib_MEM_GetParentPoolFromLink(NULL));

    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeAdmin, 0);
    BPLib_MEM_InsertAfter(&admin->active_list, &buf.blk[0].header.base_link);

    UtAssert_ADDRESS_EQ(BPLib_MEM_GetParentPoolFromLink(&buf.blk[0].header.base_link),
                        &admin->active_list);
}

void Test_BPLib_MEM_InitBaseObject(void)
{
    /* Test function for:
     * void BPLib_MEM_InitBaseObject(BPLib_MEM_BlockHeader_t *block_hdr, uint16_t user_content_length, uint32_t
     * content_type_signature)
     */

    static const uint32         UT_SIG = 0x2c9f27d0;
    static const uint16         UT_LEN = sizeof(BPLib_MEM_GenericDataContent_t);
    BPLib_MEM_BlockContent_t    my_block;

    memset(&my_block, 0, sizeof(my_block));

    UtAssert_VOIDCALL(BPLib_MEM_InitBaseObject(&my_block.header, UT_LEN, UT_SIG));

    UtAssert_UINT32_EQ(my_block.header.content_type_signature, UT_SIG);
    UtAssert_UINT16_EQ(my_block.header.user_content_length, UT_LEN);
}

// MEM Query Tests
void Test_BPLib_MEM_QueryMemCurrentUse(void)
{
    /* Test function for:
     * size_t BPLib_MEM_QueryMemCurrentUse(BPLib_MEM_Pool_t *pool)
     */

    BPLib_MEM_Pool_t pool;

    memset(&pool, 0, sizeof(pool));

    test_setup_mpblock(&pool, &pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);

    UtAssert_UINT32_EQ(BPLib_MEM_QueryMemCurrentUse(&pool), 0);
}

void Test_BPLib_MEM_QueryMemMaxUse(void)
{
    /* Test function for:
     * size_t BPLib_MEM_QueryMemMaxUse(bplib_mpool_t *pool)
     */
    BPLib_MEM_Pool_t pool;

    memset(&pool, 0, sizeof(pool));

    test_setup_mpblock(&pool, &pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);

    UtAssert_UINT32_EQ(BPLib_MEM_QueryMemMaxUse(&pool), 0);
}

// MEM Block Offset Tests
void Test_BPLib_MEM_GetUserDataOffsetByBlocktype(void)
{
    /* Test function for:
     * size_t BPLib_MEM_Get_user_dataOffsetByBlocktype(BPLib_MEM_Blocktype_t bt)
     */

    UtAssert_UINT32_EQ(BPLib_MEM_GetUserDataOffsetByBlocktype(BPLib_MEM_BlocktypeUndefined), SIZE_MAX);
    UtAssert_UINT32_EQ(BPLib_MEM_GetUserDataOffsetByBlocktype(BPLib_MEM_BlocktypeMax), SIZE_MAX);
    UtAssert_UINT32_EQ(BPLib_MEM_GetUserDataOffsetByBlocktype(BPLib_MEM_BlocktypeGeneric), 0);
}

void Test_BPLib_MEM_Register(void)
{
    // Memory Allocator Tests
    ADD_TEST(Test_BPLib_MEM_Init);
    ADD_TEST(Test_BPLib_MEM_PoolCreate);
    ADD_TEST(Test_BPLib_MEM_BlockFromExternalId);
    ADD_TEST(Test_BPLib_MEM_InitListHead);
    ADD_TEST(Test_BPLib_MEM_SubqInit);
    ADD_TEST(Test_BPLib_MEM_GetGenericDataCapacity);
    ADD_TEST(Test_BPLib_MEM_Maintain);

    // MEM Lock Tests
    ADD_TEST(Test_BPLib_MEM_LockWait);

    // MEM Pool Block Tests
    ADD_TEST(Test_BPLib_MEM_CollectBlocks);
    ADD_TEST(Test_BPLIB_MEM_RecycleBlock);
    ADD_TEST(Test_BPLib_MEM_RecycleAllBlocksInList);
    ADD_TEST(Test_BPLib_MEM_GetParentPoolFromLink);
    ADD_TEST(Test_BPLib_MEM_InitBaseObject);

    // MEM Query Tests
    ADD_TEST(Test_BPLib_MEM_QueryMemCurrentUse);
    ADD_TEST(Test_BPLib_MEM_QueryMemMaxUse);

    // MEM Block Offset Tests
    ADD_TEST(Test_BPLib_MEM_GetUserDataOffsetByBlocktype);
}

void Test_BPLib_MEM_Subqs_Register(void)
{
    ADD_TEST(Test_BPLib_MEM_SubqMergeList);
    ADD_TEST(Test_BPLib_MEM_SubqMoveAll);
    ADD_TEST(Test_BPLib_MEM_SubqDropAll);
    ADD_TEST(Test_BPLib_MEM_SubqPushSingle);
    ADD_TEST(Test_BPLib_MEM_SubqPullSingle);
    ADD_TEST(Test_BPLib_MEM_ListIterGotoFirst);
    ADD_TEST(Test_BPLib_MEM_ListIterGotoLast);
    ADD_TEST(Test_BPLib_MEM_ListIterForward);
    ADD_TEST(Test_BPLib_MEM_ListIterReverse);
    ADD_TEST(Test_BPLib_MEM_ForeachItemInList);
    ADD_TEST(Test_BPLib_MEM_SearchList);
}
