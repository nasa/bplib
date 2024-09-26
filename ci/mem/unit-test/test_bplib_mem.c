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

void TestUtil_BPLib_OS_Free(void *ptr)
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
    BPLib_MEM_TestPoolPtr = TestUtil_BPLib_OS_Calloc(16384);
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
     * bplib_mpool_block_t *BPLib_MEM_BlockFromExternalId(bplib_mpool_t *pool, bp_handle_t handle)
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
     * bool bplib_mpool_lock_wait(bplib_mpool_lock_t *lock, uint64_t until_dtntime)
     */

    BPLib_MEM_Lock_t *lock = BPLib_MEM_LockPrepare(NULL);

    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_OS_GetDtnTimeMs), 1000);
    UtAssert_BOOL_FALSE(BPLib_MEM_LockWait(lock, 0));
    UtAssert_BOOL_TRUE(BPLib_MEM_LockWait(lock, 5000)); // TODO FAIL
    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_OS_WaitUntilMs), BPLIB_MEM_TIMEOUT);
    UtAssert_BOOL_FALSE(BPLib_MEM_LockWait(lock, 5000));
}

void Test_BPLib_MEM_RegisterBlocktypeInternal(void)
{
    UT_BPLib_MEM_Buf_t buf;
    BPLib_MEM_Lock_t *lock;

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeApi, 0);
    lock   = BPLib_MEM_LockResource(&buf.pool);
    UtAssert_INT32_EQ(BPLib_MEM_RegisterBlocktypeInternal(&buf.pool, UT_TESTBLOCKTYPE_SIG,  // TODO FAIL
                                                          &buf.blk[2].u.api.api, sizeof(uint32)),
                      BPLIB_SUCCESS
                     );
    BPLib_MEM_LockRelease(lock);
}

void Test_BPLib_MEM_RegisterBlocktype(void)
{
    /* Test function for:
     * int bplib_mpool_register_blocktype(bplib_mpool_t *pool, uint32_t magic_number, const bplib_mpool_blocktype_api_t
     * *api, size_t user_content_size)
     */
    UT_BPLib_MEM_Buf_t buf;

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeApi, 0);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_AltHandler_PointerReturnForSignature, &buf.blk[1].u);

    UtAssert_INT32_EQ(  // TODO FAIL
        BPLib_MEM_RegisterBlocktype(&buf.pool, UT_TESTBLOCKTYPE_SIG, &buf.blk[2].u.api.api, sizeof(uint32)),
        BPLIB_SUCCESS);
    UtAssert_INT32_EQ(BPLib_MEM_RegisterBlocktype(&buf.pool, UT_TESTBLOCKTYPE_SIG, &buf.blk[2].u.api.api, 0),
                      BPLIB_ERROR);

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeApi, 0);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_AltHandler_PointerReturnForSignature, &buf.blk[1].u);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_RBT_InsertValueGeneric), BPLIB_MEM_RBT_DUPLICATE);
    UtAssert_INT32_EQ(BPLib_MEM_RegisterBlocktype(&buf.pool, UT_TESTBLOCKTYPE_SIG, NULL, 0), BPLIB_MEM_RBT_DUPLICATE); // TODO FAIL

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeApi, 0);
    UtAssert_INT32_EQ(  // TODO FAIL
        BPLib_MEM_RegisterBlocktype(&buf.pool, UT_TESTBLOCKTYPE_SIG, &buf.blk[2].u.api.api, sizeof(uint32)),
        BPLIB_MEM_RBT_DUPLICATE);
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
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeAdmin, 0);  // TODO Try type Api here.
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_AltHandler_PointerReturn, &buf.blk[0].u);
    buf.blk[0].u.api.api.destruct = Test_BPLib_MEM_CallbackStub;

    admin = BPLib_MEM_GetAdmin(&buf.pool);

    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeAdmin, 0);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, &buf.blk[1].header.base_link);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeAdmin, 0);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(BPLib_MEM_CollectBlocks(&buf.pool, 1), 1);
    UtAssert_STUB_COUNT(Test_BPLib_MEM_CallbackStub, 1);
    UtAssert_UINT32_EQ(BPLib_MEM_CollectBlocks(&buf.pool, 1), 1);
    UtAssert_STUB_COUNT(Test_BPLib_MEM_CallbackStub, 2);

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeApi, 0);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_AltHandler_PointerReturn, NULL);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeGeneric, 0);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, &buf.blk[0].header.base_link);

    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeAdmin, 0);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, &buf.blk[1].header.base_link);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeAdmin, 0);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(BPLib_MEM_CollectBlocks(&buf.pool, 10), 3);
}

void Test_BPLib_MEM_RecycleBlockInternal(void)  // TODO Add this test.
{

}

void Test_BPLIB_MEM_RecycleBlock(void)
{
    /* Test function for:
     * void bplib_mpool_recycle_block(bplib_mpool_block_t *blk)
     */
    UT_BPLib_MEM_Buf_t               buf;
    BPLib_MEM_BlockAdminContent_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    admin = BPLib_MEM_GetAdmin(&buf.pool);

    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeAdmin, 0);

    BPLib_MEM_InsertAfter(&admin->active_list, &buf.blk[0].header.base_link);
    BPLib_MEM_InsertAfter(&admin->active_list, &buf.blk[1].header.base_link);

    UtAssert_VOIDCALL(BPLIB_MEM_RecycleBlock(&buf.blk[1].header.base_link));
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetNextBlock(&admin->recycle_blocks.block_list), &buf.blk[1].header.base_link);
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetNextBlock(&admin->active_list), &buf.blk[0].header.base_link);
}

void Test_BPLib_MEM_RecycleAllBlocksInList(void)
{
    /* Test function for:
     * void bplib_mpool_recycle_all_blocks_in_list(bplib_mpool_t *pool, bplib_mpool_block_t *list)
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

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeAdmin, 0);
    BPLib_MEM_InsertAfter(&buf.blk[0].u.admin.active_list, &buf.blk[1].header.base_link);
    BPLib_MEM_InsertAfter(&buf.blk[0].u.admin.active_list, &buf.blk[2].header.base_link);
    UtAssert_VOIDCALL(BPLib_MEM_RecycleAllBlocksInList(NULL, &buf.blk[0].u.admin.active_list));
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetNextBlock(&admin->recycle_blocks.block_list), &buf.blk[2].header.base_link);
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetPrevBlock(&admin->recycle_blocks.block_list), &buf.blk[1].header.base_link);
}

void Test_BPLib_MEM_GetBlockContentConst(void)
{

}

void Test_BPLib_MEM_GetUserContentSize(void)
{

}

void Test_BPLib_MEM_GetParentPoolFromLink(void)
{

}

void Test_BPLib_MEM_InitBaseObject(void)
{

}

void Test_BPLib_MEM_AllocBlockInternal(void)
{

}

void Test_BPLib_MEM_GenericDataAlloc(void)
{

}

// MEM Query Tests
void Test_BPLib_MEM_QueryMemCurrentUse(void)
{

}

void Test_BPLib_MEM_QueryMemMaxUse(void)
{

}


// MEM Ref Tests
void Test_BPLib_MEM_BlockDereferenceContent(void)
{

}

void Test_BPLib_MEM_ReadRefcount(void)
{

}


// MEM Block Offset Tests
void Test_BPLib_MEM_GetUserDataOffsetByBlocktype(void)
{

}

// MEM Debug Tests
void Test_BPLib_MEM_DebugPrintListStats(void)
{

}

void Test_BPLib_MEM_DebugScan(void)
{

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
    ADD_TEST(Test_BPLib_MEM_RegisterBlocktypeInternal);
    ADD_TEST(Test_BPLib_MEM_RegisterBlocktype);
    // TODO Fix test(s) that abort.
    ADD_TEST(Test_BPLib_MEM_SubqPushSingle);
    ADD_TEST(Test_BPLib_MEM_SubqPullSingle);
    // ADD_TEST(Test_BPLib_MEM_CollectBlocks); TODO Needs more setup
    // ADD_TEST(Test_BPLib_MEM_RecycleBlockInternal);
    // ADD_TEST(Test_BPLIB_MEM_RecycleBlock);
    // ADD_TEST(Test_BPLib_MEM_RecycleAllBlocksInList);
    ADD_TEST(Test_BPLib_MEM_GetBlockContentConst);
    ADD_TEST(Test_BPLib_MEM_GetUserContentSize);
    ADD_TEST(Test_BPLib_MEM_GetParentPoolFromLink);
    ADD_TEST(Test_BPLib_MEM_InitBaseObject);
    ADD_TEST(Test_BPLib_MEM_AllocBlockInternal);
    ADD_TEST(Test_BPLib_MEM_GenericDataAlloc);

    // MEM Query Tests
    ADD_TEST(Test_BPLib_MEM_QueryMemCurrentUse);
    ADD_TEST(Test_BPLib_MEM_QueryMemMaxUse);

    // MEM Ref Tests
    ADD_TEST(Test_BPLib_MEM_BlockDereferenceContent);
    ADD_TEST(Test_BPLib_MEM_ReadRefcount);

    // MEM Block Offset Tests
    ADD_TEST(Test_BPLib_MEM_GetUserDataOffsetByBlocktype);

    // MEM Debug Tests
    ADD_TEST(Test_BPLib_MEM_DebugPrintListStats);
    ADD_TEST(Test_BPLib_MEM_DebugScan);
}

void Test_BPLib_MEM_Subqs_Register(void)
{
    ADD_TEST(Test_BPLib_MEM_SubqMergeList);
    ADD_TEST(Test_BPLib_MEM_SubqMoveAll);
    ADD_TEST(Test_BPLib_MEM_SubqDropAll);
    ADD_TEST(Test_BPLib_MEM_SubqPullSingle);
    ADD_TEST(Test_BPLib_MEM_ListIterGotoFirst);
    ADD_TEST(Test_BPLib_MEM_ListIterGotoLast);
    ADD_TEST(Test_BPLib_MEM_ListIterForward);
    ADD_TEST(Test_BPLib_MEM_ListIterReverse);
    ADD_TEST(Test_BPLib_MEM_ForeachItemInList);
    ADD_TEST(Test_BPLib_MEM_SearchList);
}
