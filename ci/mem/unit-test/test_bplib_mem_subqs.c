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

#include "bplib_mem.h"
#include "bplib_mem_internal.h"
#include "test_bplib_mem.h"


void UT_AltHandler_MoveQueue(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    BPLib_MEM_SubqBase_t *subq = UserObj;
    BPLib_MEM_Block_t *blk;

    blk = BPLib_MEM_SubqPullSingle(subq);
    if (blk)
    {
        BPLib_MEM_SubqPushSingle(subq, blk);
    }
}

void Test_BPLib_MEM_SubqMergeList(void)
{
    /* Test function for:
     * uint32_t bplib_mpool_subq_merge_list(bplib_mpool_subq_base_t *subq_dst, bplib_mpool_block_t *list)
     */
    UT_BPLib_MEM_Buf_t buf;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeAdmin, 0);
    UtAssert_UINT32_EQ(BPLib_MEM_SubqMergeList(&buf.blk[0].u.admin.recycle_blocks,
                                               buf.blk[1].u.admin.recycle_blocks.block_list.next),
                       0);

    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeAdmin, 0);
    BPLib_MEM_SubqPushSingle(&buf.blk[1].u.admin.recycle_blocks, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(BPLib_MEM_SubqMergeList(&buf.blk[0].u.admin.recycle_blocks,
                                               buf.blk[1].u.admin.recycle_blocks.block_list.next),
                       1);

    UtAssert_UINT32_EQ(buf.blk[0].u.admin.recycle_blocks.push_count, 1);
    UtAssert_UINT32_EQ(buf.blk[0].u.admin.recycle_blocks.pull_count, 0);}

void Test_BPLib_MEM_SubqMoveAll(void)
{
    /* Test function for:
     * uint32_t BPLib_MEM_SubqMoveAll(BPLib_MEM_SubqBase_t *subq_dst, BPLib_MEM_SubqBase_t *subq_src)
     */
    UT_BPLib_MEM_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeAdmin, 0);
    UtAssert_UINT32_EQ(BPLib_MEM_SubqMoveAll(&buf.blk[0].u.admin.recycle_blocks,
                                             &buf.blk[1].u.admin.recycle_blocks),
                       0);

    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeAdmin, 0);
    BPLib_MEM_SubqPushSingle(&buf.blk[1].u.admin.recycle_blocks, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(BPLib_MEM_SubqMoveAll(&buf.blk[0].u.admin.recycle_blocks,
                                             &buf.blk[1].u.admin.recycle_blocks),
                       1);
    UtAssert_UINT32_EQ(buf.blk[0].u.admin.recycle_blocks.push_count, 1);
    UtAssert_UINT32_EQ(buf.blk[0].u.admin.recycle_blocks.pull_count, 0);
    UtAssert_UINT32_EQ(buf.blk[1].u.admin.recycle_blocks.push_count, 1);
    UtAssert_UINT32_EQ(buf.blk[1].u.admin.recycle_blocks.pull_count, 1);
}

void Test_BPLib_MEM_SubqDropAll(void)
{
    /* Test function for:
     * uint32_t BPLib_MEM_SubqDropAll(BPLib_MEM_Pool_t *pool, BPLib_MEM_SubqBase_t *subq)
     */
    UT_BPLib_MEM_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeAdmin, 0);
    UtAssert_UINT32_EQ(BPLib_MEM_SubqDropAll(&buf.pool, &buf.blk[0].u.admin.recycle_blocks), 0);

    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeAdmin, 0);
    BPLib_MEM_SubqPushSingle(&buf.blk[0].u.admin.recycle_blocks, &buf.blk[1].header.base_link);
    BPLib_MEM_SubqPushSingle(&buf.blk[0].u.admin.recycle_blocks, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(BPLib_MEM_SubqDropAll(&buf.pool, &buf.blk[0].u.admin.recycle_blocks), 2);
    UtAssert_UINT32_EQ(buf.blk[0].u.admin.recycle_blocks.push_count, 2);
    UtAssert_UINT32_EQ(buf.blk[0].u.admin.recycle_blocks.pull_count, 2);
}

void Test_BPLib_MEM_SubqPushSingle(void)
{
    UT_BPLib_MEM_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeAdmin, 0);
    UtAssert_VOIDCALL(BPLib_MEM_SubqPushSingle(&buf.blk[0].u.admin.recycle_blocks,
                                               &buf.blk[1].header.base_link));
    UtAssert_UINT32_EQ(buf.blk[0].u.admin.recycle_blocks.push_count, 1);
    UtAssert_UINT32_EQ(buf.blk[0].u.admin.recycle_blocks.pull_count, 0);
}

void Test_BPLib_MEM_SubqPullSingle(void)
{
    /* Test function for:
     * BPLib_MEM_Block_t *BPLib_MEM_SubqPullSingle(BPLib_MEM_SubqBase_t *subq)
     */
    UT_BPLib_MEM_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeAdmin, 0);

    UtAssert_NULL(BPLib_MEM_SubqPullSingle(&buf.blk[0].u.admin.recycle_blocks));
}

void Test_BPLib_MEM_ListIterGotoFirst(void)
{
    /* Test function for:
     * int bplib_mpool_list_iter_goto_first(const bplib_mpool_block_t *list, bplib_mpool_list_iter_t *iter)
     */
    BPLib_MEM_Block_t     list;
    BPLib_MEM_ListIter_t it;

    memset(&list, 0, sizeof(list));
    UtAssert_INT32_EQ(BPLib_MEM_ListIterGotoFirst(&list, &it), BPLIB_ERROR);

    BPLib_MEM_InitListHead(NULL, &list);
    UtAssert_INT32_EQ(BPLib_MEM_ListIterGotoFirst(&list, &it), BPLIB_ERROR);
}

void Test_BPLib_MEM_ListIterGotoLast(void)
{
    /* Test function for:
     * int bplib_mpool_list_iter_goto_last(const bplib_mpool_block_t *list, bplib_mpool_list_iter_t *iter)
     */

    BPLib_MEM_Block_t     list;
    BPLib_MEM_ListIter_t it;

    memset(&list, 0, sizeof(list));
    UtAssert_INT32_EQ(BPLib_MEM_ListIterGotoLast(&list, &it), BPLIB_ERROR);

    BPLib_MEM_InitListHead(NULL, &list);
    UtAssert_INT32_EQ(BPLib_MEM_ListIterGotoLast(&list, &it), BPLIB_ERROR);
}

void Test_BPLib_MEM_ListIterForward(void)
{
    /* Test function for:
     * int bplib_mpool_list_iter_forward(bplib_mpool_list_iter_t *iter)
     */
    BPLib_MEM_ListIter_t     it;
    BPLib_MEM_Block_t         list;
    BPLib_MEM_BlockContent_t my_block;

    memset(&it, 0, sizeof(it));
    UtAssert_INT32_EQ(BPLib_MEM_ListIterForward(&it), BPLIB_ERROR);

    memset(&list, 0, sizeof(list));
    BPLib_MEM_InitListHead(NULL, &list);
    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypeGeneric, 0);
    BPLib_MEM_InsertAfter(&list, &my_block.header.base_link);

    it.pending_entry = &my_block.header.base_link;
    UtAssert_INT32_EQ(BPLib_MEM_ListIterForward(&it), BPLIB_SUCCESS);

    UtAssert_ADDRESS_EQ(it.position, &my_block);
    UtAssert_ADDRESS_EQ(it.pending_entry, &list);

    UtAssert_INT32_EQ(BPLib_MEM_ListIterForward(&it), BPLIB_ERROR);
}

void Test_BPLib_MEM_ListIterReverse(void)
{
    /* Test function for:
     * int bplib_mpool_list_iter_reverse(bplib_mpool_list_iter_t *iter)
     */
    BPLib_MEM_ListIter_t     it;
    BPLib_MEM_Block_t         list;
    BPLib_MEM_BlockContent_t my_block;

    memset(&it, 0, sizeof(it));

    UtAssert_INT32_EQ(BPLib_MEM_ListIterReverse(&it), BPLIB_ERROR);

    memset(&list, 0, sizeof(list));
    BPLib_MEM_InitListHead(NULL, &list);
    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypeGeneric, 0);
    BPLib_MEM_InsertAfter(&list, &my_block.header.base_link);

    it.pending_entry = &my_block.header.base_link;
    UtAssert_INT32_EQ(BPLib_MEM_ListIterReverse(&it), BPLIB_SUCCESS);

    UtAssert_ADDRESS_EQ(it.position, &my_block);
    UtAssert_ADDRESS_EQ(it.pending_entry, &list);

    UtAssert_INT32_EQ(BPLib_MEM_ListIterReverse(&it), BPLIB_ERROR);
}

void Test_BPLib_MEM_ForeachItemInList(void)
{
    /* Test function for:
     * int bplib_mpool_foreach_item_in_list(bplib_mpool_block_t *list, bool always_remove, bplib_mpool_callback_func_t
     * callback_fn, void *callback_arg)
     */

    BPLib_MEM_Block_t         list;
    BPLib_MEM_BlockContent_t my_block;

    memset(&list, 0, sizeof(list));
    BPLib_MEM_InitListHead(NULL, &list);

    UtAssert_ZERO(BPLib_MEM_ForeachItemInList(&list, false, Test_BPLib_MEM_CallbackStub, NULL));
    UtAssert_STUB_COUNT(Test_BPLib_MEM_CallbackStub, 0);

    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypeAdmin, 0);
    BPLib_MEM_InsertAfter(&list, &my_block.header.base_link);

    UtAssert_INT32_EQ(BPLib_MEM_ForeachItemInList(&list, false, Test_BPLib_MEM_CallbackStub, NULL), 1);
    UtAssert_STUB_COUNT(Test_BPLib_MEM_CallbackStub, 1);

    UtAssert_INT32_EQ(BPLib_MEM_ForeachItemInList(&list, true, Test_BPLib_MEM_CallbackStub, NULL), 1);
    UtAssert_STUB_COUNT(Test_BPLib_MEM_CallbackStub, 2);
    UtAssert_BOOL_TRUE(BPLib_MEM_IsLinkUnattached(&my_block.header.base_link));
}

void Test_BPLib_MEM_SearchList(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_search_list(const bplib_mpool_block_t *list, bplib_mpool_callback_func_t
     * match_fn, void *match_arg)
     */
    BPLib_MEM_Block_t         list;
    BPLib_MEM_BlockContent_t my_block;

    memset(&list, 0, sizeof(list));
    BPLib_MEM_InitListHead(NULL, &list);

    UtAssert_NULL(BPLib_MEM_SearchList(&list, Test_BPLib_MEM_CallbackStub, NULL));
    UtAssert_STUB_COUNT(Test_BPLib_MEM_CallbackStub, 0);

    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypeGeneric, 0);
    BPLib_MEM_InsertAfter(&list, &my_block.header.base_link);

    UtAssert_ADDRESS_EQ(BPLib_MEM_SearchList(&list, Test_BPLib_MEM_CallbackStub, NULL),
                        &my_block.header.base_link);
    UtAssert_STUB_COUNT(Test_BPLib_MEM_CallbackStub, 1);

    UT_SetDefaultReturnValue(UT_KEY(Test_BPLib_MEM_CallbackStub), 1);
    UtAssert_NULL(BPLib_MEM_SearchList(&list, Test_BPLib_MEM_CallbackStub, NULL));
    UtAssert_STUB_COUNT(Test_BPLib_MEM_CallbackStub, 2);
}

