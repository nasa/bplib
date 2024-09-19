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

#include "test_BPLib_MEM.h"
#include "bplib_s_qm_ducts.h"

void UT_AltHandler_MoveQueue(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    BPLib_STOR_CACHE_Flow_t  *flow = UserObj;
    BPLib_STOR_CACHE_Block_t *blk;

    blk = BPLib_STOR_CACHE_SubqPullSingle(&flow->ingress.base_subq);
    if (blk)
    {
        BPLib_STOR_CACHE_SubqPushSingle(&flow->egress.base_subq, blk);
    }
}

void test_BPLib_STOR_CACHE_SubqMergeList(void)
{
    /* Test function for:
     * uint32_t BPLib_STOR_CACHE_SubqMergeList(BPLib_STOR_CACHE_SubqBase_t *subq_dst, BPLib_STOR_CACHE_Block_t *list)
     */
    UT_BPLib_STOR_CACHE_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypeFlow, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypeFlow, 0);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_SubqMergeList(&buf.blk[0].u.flow.fblock.ingress.base_subq,
                                                   &buf.blk[1].u.flow.fblock.ingress.base_subq.block_list),
                       0);

    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_STOR_CACHE_BlocktypePrimary, 0);
    BPLib_STOR_CACHE_SubqPushSingle(&buf.blk[1].u.flow.fblock.ingress.base_subq, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_SubqMergeList(&buf.blk[0].u.flow.fblock.ingress.base_subq,
                                                   &buf.blk[1].u.flow.fblock.ingress.base_subq.block_list),
                       1);
    UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.push_count, 1);
    UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.pull_count, 0);
}

void test_BPLib_STOR_CACHE_SubqMoveAll(void)
{
    /* Test function for:
     * uint32_t BPLib_STOR_CACHE_SubqMoveAll(BPLib_STOR_CACHE_SubqBase_t *subq_dst, BPLib_STOR_CACHE_SubqBase_t *subq_src)
     */
    UT_BPLib_STOR_CACHE_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypeFlow, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypeFlow, 0);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_SubqMoveAll(&buf.blk[0].u.flow.fblock.ingress.base_subq,
                                                 &buf.blk[1].u.flow.fblock.ingress.base_subq),
                       0);

    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_STOR_CACHE_BlocktypePrimary, 0);
    BPLib_STOR_CACHE_SubqPushSingle(&buf.blk[1].u.flow.fblock.ingress.base_subq, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_SubqMoveAll(&buf.blk[0].u.flow.fblock.ingress.base_subq,
                                                 &buf.blk[1].u.flow.fblock.ingress.base_subq),
                       1);
    UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.push_count, 1);
    UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.pull_count, 0);
    UtAssert_UINT32_EQ(buf.blk[1].u.flow.fblock.ingress.base_subq.push_count, 1);
    UtAssert_UINT32_EQ(buf.blk[1].u.flow.fblock.ingress.base_subq.pull_count, 1);
}

void test_BPLib_STOR_CACHE_SubqDropAll(void)
{
    /* Test function for:
     * uint32_t BPLib_STOR_CACHE_SubqDropAll(BPLib_STOR_CACHE_Pool_t *pool, BPLib_STOR_CACHE_SubqBase_t *subq)
     */
    UT_BPLib_STOR_CACHE_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypeFlow, 0);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_SubqDropAll(&buf.pool, &buf.blk[0].u.flow.fblock.ingress.base_subq), 0);

    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypePrimary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_STOR_CACHE_BlocktypePrimary, 0);
    BPLib_STOR_CACHE_SubqPushSingle(&buf.blk[0].u.flow.fblock.ingress.base_subq, &buf.blk[1].header.base_link);
    BPLib_STOR_CACHE_SubqPushSingle(&buf.blk[0].u.flow.fblock.ingress.base_subq, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_SubqDropAll(&buf.pool, &buf.blk[0].u.flow.fblock.ingress.base_subq), 2);
    UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.push_count, 2);
    UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.pull_count, 2);
}

void test_BPLib_STOR_CACHE_SubqPullSingle(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_SubqPullSingle(BPLib_STOR_CACHE_SubqBase_t *subq)
     */
    UT_BPLib_STOR_CACHE_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypeFlow, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypeFlow, 0);

    UtAssert_NULL(BPLib_STOR_CACHE_SubqPullSingle(&buf.blk[0].u.flow.fblock.ingress.base_subq));
}

void test_BPLib_STOR_CACHE_FlowCast(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Flow_t *BPLib_STOR_CACHE_FlowCast(BPLib_STOR_CACHE_Block_t *cb)
     */
    BPLib_STOR_CACHE_BlockContent_t my_block;
    BPLib_STOR_CACHE_Block_t        *cb = &my_block.header.base_link;

    UtAssert_NULL(BPLib_STOR_CACHE_FlowCast(NULL));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_NULL(BPLib_STOR_CACHE_FlowCast(cb));

    test_setup_mpblock(NULL, &my_block, BPLib_STOR_CACHE_BlocktypeFlow, 0);
    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_FlowCast(cb), &my_block.u);

    test_setup_mpblock(NULL, &my_block, BPLib_STOR_CACHE_BlocktypeGeneric, 0);
    UtAssert_NULL(BPLib_STOR_CACHE_FlowCast(cb));
}

void test_BPLib_STOR_CACHE_FlowAlloc(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_FlowAlloc(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number, void *init_arg)
     */
    UT_BPLib_STOR_CACHE_Buf_t buf;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(BPLib_STOR_CACHE_FlowAlloc(&buf.pool, 1234, NULL));

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_FlowAlloc(&buf.pool, 1234, NULL), &buf.blk[0]);
}

void test_BPLib_STOR_CACHE_FlowDisable(void)
{
    /* Test function for:
     * uint32_t BPLib_STOR_CACHE_FlowDisable(BPLib_STOR_CACHE_SubqWorkitem_t *subq)
     */
    UT_BPLib_STOR_CACHE_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypeFlow, 0);

    UtAssert_ZERO(BPLib_STOR_CACHE_FlowDisable(&buf.blk[0].u.flow.fblock.ingress));
}

void test_BPLib_STOR_CACHE_FlowEnable(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_FlowEnable(BPLib_STOR_CACHE_SubqWorkitem_t *subq, uint32_t depth_limit)
     */
    UT_BPLib_STOR_CACHE_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypeFlow, 0);

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FlowEnable(&buf.blk[0].u.flow.fblock.ingress, 1));
}

void test_BPLib_STOR_CACHE_FlowTryPush(void)
{
    /* Test function for:
     * bool BPLib_STOR_CACHE_FlowTryPush(BPLib_STOR_CACHE_SubqWorkitem_t *subq_dst, BPLib_STOR_CACHE_Block_t *qblk, uint64_t
     * abs_timeout)
     */
    UT_BPLib_STOR_CACHE_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypeFlow, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypePrimary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_STOR_CACHE_BlocktypePrimary, 0);

    UtAssert_BOOL_FALSE(BPLib_STOR_CACHE_FlowTryPush(&buf.blk[0].u.flow.fblock.ingress, &buf.blk[1].header.base_link, 0));
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 0);

    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsLinkUnattached(&buf.blk[1].header.base_link));
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FlowEnable(&buf.blk[0].u.flow.fblock.ingress, 1));
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_FlowTryPush(&buf.blk[0].u.flow.fblock.ingress, &buf.blk[1].header.base_link, 0));
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 1);
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsLinkAttached(&buf.blk[1].header.base_link));

    UtAssert_BOOL_FALSE(BPLib_STOR_CACHE_FlowTryPush(&buf.blk[0].u.flow.fblock.ingress, &buf.blk[2].header.base_link, 0));
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 1);
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsLinkUnattached(&buf.blk[2].header.base_link));

    /* This time use a nonzero timeout */
    UT_SetHandlerFunction(UT_KEY(bplib_os_wait_until_ms), UT_AltHandler_MoveQueue, &buf.blk[0].u.flow.fblock);
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_FlowTryPush(&buf.blk[0].u.flow.fblock.ingress, &buf.blk[2].header.base_link, 100));
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 2);
}

void test_BPLib_STOR_CACHE_FlowTryMoveAll(void)
{
    /* Test function for:
     * uint32_t BPLib_STOR_CACHE_FlowTryMoveAll(BPLib_STOR_CACHE_SubqWorkitem_t *subq_dst, BPLib_STOR_CACHE_SubqWorkitem_t
     * *subq_src, uint64_t abs_timeout)
     */
    UT_BPLib_STOR_CACHE_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypeFlow, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypePrimary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_STOR_CACHE_BlocktypePrimary, 0);

    UtAssert_ZERO(
        BPLib_STOR_CACHE_FlowTryMoveAll(&buf.blk[0].u.flow.fblock.egress, &buf.blk[0].u.flow.fblock.ingress, 0));
    /* Even though the above did nothing it still signals on the lock */
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 1);

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FlowEnable(&buf.blk[0].u.flow.fblock.ingress, 1));
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FlowEnable(&buf.blk[0].u.flow.fblock.egress, 1));
    UtAssert_VOIDCALL(
        BPLib_STOR_CACHE_SubqPushSingle(&buf.blk[0].u.flow.fblock.ingress.base_subq, &buf.blk[1].header.base_link));
    UtAssert_UINT32_EQ(
        BPLib_STOR_CACHE_FlowTryMoveAll(&buf.blk[0].u.flow.fblock.egress, &buf.blk[0].u.flow.fblock.ingress, 0), 1);
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 2);

    UT_SetHandlerFunction(UT_KEY(bplib_os_wait_until_ms), UT_AltHandler_MoveQueue, &buf.blk[0].u.flow.fblock);
    UT_SetDeferredRetcode(UT_KEY(bplib_os_wait_until_ms), 2, BP_TIMEOUT);
    UtAssert_VOIDCALL(
        BPLib_STOR_CACHE_SubqPushSingle(&buf.blk[0].u.flow.fblock.ingress.base_subq, &buf.blk[2].header.base_link));
    UtAssert_ZERO(
        BPLib_STOR_CACHE_FlowTryMoveAll(&buf.blk[0].u.flow.fblock.ingress, &buf.blk[0].u.flow.fblock.egress, 100));
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 2);
}

void test_BPLib_STOR_CACHE_FlowTryPull(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_FlowTryPull(BPLib_STOR_CACHE_SubqWorkitem_t *subq_src, uint64_t abs_timeout)
     */
    UT_BPLib_STOR_CACHE_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypeFlow, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypePrimary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_STOR_CACHE_BlocktypePrimary, 0);

    UtAssert_NULL(BPLib_STOR_CACHE_FlowTryPull(&buf.blk[0].u.flow.fblock.ingress, 0));
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 0);

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FlowEnable(&buf.blk[0].u.flow.fblock.ingress, 1));
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FlowEnable(&buf.blk[0].u.flow.fblock.egress, 1));
    UtAssert_VOIDCALL(
        BPLib_STOR_CACHE_SubqPushSingle(&buf.blk[0].u.flow.fblock.egress.base_subq, &buf.blk[1].header.base_link));

    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_FlowTryPull(&buf.blk[0].u.flow.fblock.egress, 0), &buf.blk[1]);
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 1);
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsLinkUnattached(&buf.blk[1].header.base_link));

    /* This time use a nonzero timeout */
    UtAssert_VOIDCALL(
        BPLib_STOR_CACHE_SubqPushSingle(&buf.blk[0].u.flow.fblock.ingress.base_subq, &buf.blk[1].header.base_link));
    UT_SetHandlerFunction(UT_KEY(bplib_os_wait_until_ms), UT_AltHandler_MoveQueue, &buf.blk[0].u.flow.fblock);
    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_FlowTryPull(&buf.blk[0].u.flow.fblock.egress, 100), &buf.blk[1]);
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 2);
}

void test_BPLib_STOR_CACHE_FlowModifyFlags(void)
{
    /* Test function for:
     * bool BPLib_STOR_CACHE_FlowModifyFlags(BPLib_STOR_CACHE_Block_t *cb, uint32_t set_bits, uint32_t clear_bits)
     */
    UT_BPLib_STOR_CACHE_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_STOR_CACHE_BlocktypeFlow, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_STOR_CACHE_BlocktypePrimary, 0);

    UtAssert_BOOL_FALSE(BPLib_STOR_CACHE_FlowModifyFlags(&buf.blk[1].header.base_link, 1, 2));
    UtAssert_BOOL_FALSE(BPLib_STOR_CACHE_FlowModifyFlags(&buf.blk[0].header.base_link, 0, 0));
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_FlowModifyFlags(&buf.blk[0].header.base_link, 1, 2));
}

static int ut_flow_statechange_check(void *arg, BPLib_STOR_CACHE_Block_t *jblk)
{
    return 0;
}

void test_BPLib_STOR_CACHE_FlowEventHandler(void)
{
    /* Test function for:
     * static int BPLib_STOR_CACHE_FlowEventHandler(void *arg, BPLib_STOR_CACHE_Block_t *jblk)
     */

    BPLib_STOR_CACHE_BlockContent_t my_block;
    BPLib_STOR_CACHE_Flow_t         *flow;
    BPLib_STOR_CACHE_CallbackFunc_t evhandler;

    memset(&my_block, 0, sizeof(my_block));
    test_setup_mpblock(NULL, &my_block, BPLib_STOR_CACHE_BlocktypeFlow, 0);

    flow                                = &my_block.u.flow.fblock;
    evhandler                           = flow->statechange_job.base_job.handler;
    flow->statechange_job.event_handler = ut_flow_statechange_check;

    UtAssert_INT32_EQ(evhandler(NULL, NULL), -1);
    UtAssert_INT32_EQ(evhandler(NULL, &my_block.header.base_link), 0);
    flow->pending_state_flags = BPLIB_MEM_FLOW_FLAGS_ADMIN_UP | BPLIB_MEM_FLOW_FLAGS_OPER_UP;
    UtAssert_INT32_EQ(evhandler(NULL, &my_block.header.base_link), 0);
    flow->pending_state_flags |= BPLIB_MEM_FLOW_FLAGS_POLL;
    UtAssert_INT32_EQ(evhandler(NULL, &my_block.header.base_link), 0);
    flow->pending_state_flags &= ~BPLIB_MEM_FLOW_FLAGS_OPER_UP;
    UtAssert_INT32_EQ(evhandler(NULL, &my_block.header.base_link), 0);
}

void TestBplibMpoolFlows_Register(void)
{
    UtTest_Add(test_BPLib_STOR_CACHE_SubqMergeList, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_SubqMergeList");
    UtTest_Add(test_BPLib_STOR_CACHE_SubqMoveAll, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_SubqMoveAll");
    UtTest_Add(test_BPLib_STOR_CACHE_SubqDropAll, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_SubqDropAll");
    UtTest_Add(test_BPLib_STOR_CACHE_SubqPullSingle, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_SubqPullSingle");
    UtTest_Add(test_BPLib_STOR_CACHE_FlowCast, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_FlowCast");
    UtTest_Add(test_BPLib_STOR_CACHE_FlowAlloc, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_FlowAlloc");
    UtTest_Add(test_BPLib_STOR_CACHE_FlowDisable, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_FlowDisable");
    UtTest_Add(test_BPLib_STOR_CACHE_FlowEnable, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_FlowEnable");
    UtTest_Add(test_BPLib_STOR_CACHE_FlowTryPush, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_FlowTryPush");
    UtTest_Add(test_BPLib_STOR_CACHE_FlowTryMoveAll, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_FlowTryMoveAll");
    UtTest_Add(test_BPLib_STOR_CACHE_FlowTryPull, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_FlowTryPull");
    UtTest_Add(test_BPLib_STOR_CACHE_FlowModifyFlags, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_FlowModifyFlags");
    UtTest_Add(test_BPLib_STOR_CACHE_FlowEventHandler, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_FlowEventHandler");
}
