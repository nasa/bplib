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

void UT_AltHandler_MoveQueue(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bplib_mpool_flow_t  *flow = UserObj;
    bplib_mpool_block_t *blk;

    blk = bplib_mpool_subq_pull_single(&flow->ingress.base_subq);
    if (blk)
    {
        bplib_mpool_subq_push_single(&flow->egress.base_subq, blk);
    }
}

void test_bplib_mpool_subq_merge_list(void)
{
    /* Test function for:
     * uint32_t bplib_mpool_subq_merge_list(bplib_mpool_subq_base_t *subq_dst, bplib_mpool_block_t *list)
     */
    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_flow, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_flow, 0);
    UtAssert_UINT32_EQ(bplib_mpool_subq_merge_list(&buf.blk[0].u.flow.fblock.ingress.base_subq,
                                                   &buf.blk[1].u.flow.fblock.ingress.base_subq.block_list),
                       0);

    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_primary, 0);
    bplib_mpool_subq_push_single(&buf.blk[1].u.flow.fblock.ingress.base_subq, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(bplib_mpool_subq_merge_list(&buf.blk[0].u.flow.fblock.ingress.base_subq,
                                                   &buf.blk[1].u.flow.fblock.ingress.base_subq.block_list),
                       1);
    UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.push_count, 1);
    UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.pull_count, 0);
}

void test_bplib_mpool_subq_move_all(void)
{
    /* Test function for:
     * uint32_t bplib_mpool_subq_move_all(bplib_mpool_subq_base_t *subq_dst, bplib_mpool_subq_base_t *subq_src)
     */
    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_flow, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_flow, 0);
    UtAssert_UINT32_EQ(bplib_mpool_subq_move_all(&buf.blk[0].u.flow.fblock.ingress.base_subq,
                                                 &buf.blk[1].u.flow.fblock.ingress.base_subq),
                       0);

    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_primary, 0);
    bplib_mpool_subq_push_single(&buf.blk[1].u.flow.fblock.ingress.base_subq, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(bplib_mpool_subq_move_all(&buf.blk[0].u.flow.fblock.ingress.base_subq,
                                                 &buf.blk[1].u.flow.fblock.ingress.base_subq),
                       1);
    UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.push_count, 1);
    UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.pull_count, 0);
    UtAssert_UINT32_EQ(buf.blk[1].u.flow.fblock.ingress.base_subq.push_count, 1);
    UtAssert_UINT32_EQ(buf.blk[1].u.flow.fblock.ingress.base_subq.pull_count, 1);
}

void test_bplib_mpool_subq_drop_all(void)
{
    /* Test function for:
     * uint32_t bplib_mpool_subq_drop_all(bplib_mpool_t *pool, bplib_mpool_subq_base_t *subq)
     */
    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_flow, 0);
    UtAssert_UINT32_EQ(bplib_mpool_subq_drop_all(&buf.pool, &buf.blk[0].u.flow.fblock.ingress.base_subq), 0);

    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_primary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_primary, 0);
    bplib_mpool_subq_push_single(&buf.blk[0].u.flow.fblock.ingress.base_subq, &buf.blk[1].header.base_link);
    bplib_mpool_subq_push_single(&buf.blk[0].u.flow.fblock.ingress.base_subq, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(bplib_mpool_subq_drop_all(&buf.pool, &buf.blk[0].u.flow.fblock.ingress.base_subq), 2);
    UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.push_count, 2);
    UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.pull_count, 2);
}

void test_bplib_mpool_subq_pull_single(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_subq_pull_single(bplib_mpool_subq_base_t *subq)
     */
    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_flow, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_flow, 0);

    UtAssert_NULL(bplib_mpool_subq_pull_single(&buf.blk[0].u.flow.fblock.ingress.base_subq));
}

void test_bplib_mpool_flow_cast(void)
{
    /* Test function for:
     * bplib_mpool_flow_t *bplib_mpool_flow_cast(bplib_mpool_block_t *cb)
     */
    bplib_mpool_block_content_t my_block;
    bplib_mpool_block_t        *cb = &my_block.header.base_link;

    UtAssert_NULL(bplib_mpool_flow_cast(NULL));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_NULL(bplib_mpool_flow_cast(cb));

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_flow, 0);
    UtAssert_ADDRESS_EQ(bplib_mpool_flow_cast(cb), &my_block.u);

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_generic, 0);
    UtAssert_NULL(bplib_mpool_flow_cast(cb));
}

void test_bplib_mpool_flow_alloc(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_flow_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg)
     */
    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(bplib_mpool_flow_alloc(&buf.pool, 1234, NULL));

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    UtAssert_ADDRESS_EQ(bplib_mpool_flow_alloc(&buf.pool, 1234, NULL), &buf.blk[0]);
}

void test_bplib_mpool_flow_disable(void)
{
    /* Test function for:
     * uint32_t bplib_mpool_flow_disable(bplib_mpool_subq_workitem_t *subq)
     */
    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_flow, 0);

    UtAssert_ZERO(bplib_mpool_flow_disable(&buf.blk[0].u.flow.fblock.ingress));
}

void test_bplib_mpool_flow_enable(void)
{
    /* Test function for:
     * void bplib_mpool_flow_enable(bplib_mpool_subq_workitem_t *subq, uint32_t depth_limit)
     */
    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_flow, 0);

    UtAssert_VOIDCALL(bplib_mpool_flow_enable(&buf.blk[0].u.flow.fblock.ingress, 1));
}

void test_bplib_mpool_flow_try_push(void)
{
    /* Test function for:
     * bool bplib_mpool_flow_try_push(bplib_mpool_subq_workitem_t *subq_dst, bplib_mpool_block_t *qblk, uint64_t
     * abs_timeout)
     */
    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_flow, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_primary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_primary, 0);

    UtAssert_BOOL_FALSE(bplib_mpool_flow_try_push(&buf.blk[0].u.flow.fblock.ingress, &buf.blk[1].header.base_link, 0));
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 0);

    UtAssert_BOOL_TRUE(bplib_mpool_is_link_unattached(&buf.blk[1].header.base_link));
    UtAssert_VOIDCALL(bplib_mpool_flow_enable(&buf.blk[0].u.flow.fblock.ingress, 1));
    UtAssert_BOOL_TRUE(bplib_mpool_flow_try_push(&buf.blk[0].u.flow.fblock.ingress, &buf.blk[1].header.base_link, 0));
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 1);
    UtAssert_BOOL_TRUE(bplib_mpool_is_link_attached(&buf.blk[1].header.base_link));

    UtAssert_BOOL_FALSE(bplib_mpool_flow_try_push(&buf.blk[0].u.flow.fblock.ingress, &buf.blk[2].header.base_link, 0));
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 1);
    UtAssert_BOOL_TRUE(bplib_mpool_is_link_unattached(&buf.blk[2].header.base_link));

    /* This time use a nonzero timeout */
    UT_SetHandlerFunction(UT_KEY(bplib_os_wait_until_ms), UT_AltHandler_MoveQueue, &buf.blk[0].u.flow.fblock);
    UtAssert_BOOL_TRUE(bplib_mpool_flow_try_push(&buf.blk[0].u.flow.fblock.ingress, &buf.blk[2].header.base_link, 100));
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 2);
}

void test_bplib_mpool_flow_try_move_all(void)
{
    /* Test function for:
     * uint32_t bplib_mpool_flow_try_move_all(bplib_mpool_subq_workitem_t *subq_dst, bplib_mpool_subq_workitem_t
     * *subq_src, uint64_t abs_timeout)
     */
    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_flow, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_primary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_primary, 0);

    UtAssert_ZERO(
        bplib_mpool_flow_try_move_all(&buf.blk[0].u.flow.fblock.egress, &buf.blk[0].u.flow.fblock.ingress, 0));
    /* Even though the above did nothing it still signals on the lock */
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 1);

    UtAssert_VOIDCALL(bplib_mpool_flow_enable(&buf.blk[0].u.flow.fblock.ingress, 1));
    UtAssert_VOIDCALL(bplib_mpool_flow_enable(&buf.blk[0].u.flow.fblock.egress, 1));
    UtAssert_VOIDCALL(
        bplib_mpool_subq_push_single(&buf.blk[0].u.flow.fblock.ingress.base_subq, &buf.blk[1].header.base_link));
    UtAssert_UINT32_EQ(
        bplib_mpool_flow_try_move_all(&buf.blk[0].u.flow.fblock.egress, &buf.blk[0].u.flow.fblock.ingress, 0), 1);
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 2);

    UT_SetHandlerFunction(UT_KEY(bplib_os_wait_until_ms), UT_AltHandler_MoveQueue, &buf.blk[0].u.flow.fblock);
    UT_SetDeferredRetcode(UT_KEY(bplib_os_wait_until_ms), 2, BP_TIMEOUT);
    UtAssert_VOIDCALL(
        bplib_mpool_subq_push_single(&buf.blk[0].u.flow.fblock.ingress.base_subq, &buf.blk[2].header.base_link));
    UtAssert_ZERO(
        bplib_mpool_flow_try_move_all(&buf.blk[0].u.flow.fblock.ingress, &buf.blk[0].u.flow.fblock.egress, 100));
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 2);
}

void test_bplib_mpool_flow_try_pull(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_flow_try_pull(bplib_mpool_subq_workitem_t *subq_src, uint64_t abs_timeout)
     */
    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_flow, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_primary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_primary, 0);

    UtAssert_NULL(bplib_mpool_flow_try_pull(&buf.blk[0].u.flow.fblock.ingress, 0));
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 0);

    UtAssert_VOIDCALL(bplib_mpool_flow_enable(&buf.blk[0].u.flow.fblock.ingress, 1));
    UtAssert_VOIDCALL(bplib_mpool_flow_enable(&buf.blk[0].u.flow.fblock.egress, 1));
    UtAssert_VOIDCALL(
        bplib_mpool_subq_push_single(&buf.blk[0].u.flow.fblock.egress.base_subq, &buf.blk[1].header.base_link));

    UtAssert_ADDRESS_EQ(bplib_mpool_flow_try_pull(&buf.blk[0].u.flow.fblock.egress, 0), &buf.blk[1]);
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 1);
    UtAssert_BOOL_TRUE(bplib_mpool_is_link_unattached(&buf.blk[1].header.base_link));

    /* This time use a nonzero timeout */
    UtAssert_VOIDCALL(
        bplib_mpool_subq_push_single(&buf.blk[0].u.flow.fblock.ingress.base_subq, &buf.blk[1].header.base_link));
    UT_SetHandlerFunction(UT_KEY(bplib_os_wait_until_ms), UT_AltHandler_MoveQueue, &buf.blk[0].u.flow.fblock);
    UtAssert_ADDRESS_EQ(bplib_mpool_flow_try_pull(&buf.blk[0].u.flow.fblock.egress, 100), &buf.blk[1]);
    UtAssert_STUB_COUNT(bplib_os_broadcast_signal, 2);
}

void test_bplib_mpool_flow_modify_flags(void)
{
    /* Test function for:
     * bool bplib_mpool_flow_modify_flags(bplib_mpool_block_t *cb, uint32_t set_bits, uint32_t clear_bits)
     */
    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_flow, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_primary, 0);

    UtAssert_BOOL_FALSE(bplib_mpool_flow_modify_flags(&buf.blk[1].header.base_link, 1, 2));
    UtAssert_BOOL_FALSE(bplib_mpool_flow_modify_flags(&buf.blk[0].header.base_link, 0, 0));
    UtAssert_BOOL_TRUE(bplib_mpool_flow_modify_flags(&buf.blk[0].header.base_link, 1, 2));
}

static int ut_flow_statechange_check(void *arg, bplib_mpool_block_t *jblk)
{
    return 0;
}

void test_bplib_mpool_flow_event_handler(void)
{
    /* Test function for:
     * static int bplib_mpool_flow_event_handler(void *arg, bplib_mpool_block_t *jblk)
     */

    bplib_mpool_block_content_t my_block;
    bplib_mpool_flow_t         *flow;
    bplib_mpool_callback_func_t evhandler;

    memset(&my_block, 0, sizeof(my_block));
    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_flow, 0);

    flow                                = &my_block.u.flow.fblock;
    evhandler                           = flow->statechange_job.base_job.handler;
    flow->statechange_job.event_handler = ut_flow_statechange_check;

    UtAssert_INT32_EQ(evhandler(NULL, NULL), -1);
    UtAssert_INT32_EQ(evhandler(NULL, &my_block.header.base_link), 0);
    flow->pending_state_flags = BPLIB_MPOOL_FLOW_FLAGS_ADMIN_UP | BPLIB_MPOOL_FLOW_FLAGS_OPER_UP;
    UtAssert_INT32_EQ(evhandler(NULL, &my_block.header.base_link), 0);
    flow->pending_state_flags |= BPLIB_MPOOL_FLOW_FLAGS_POLL;
    UtAssert_INT32_EQ(evhandler(NULL, &my_block.header.base_link), 0);
    flow->pending_state_flags &= ~BPLIB_MPOOL_FLOW_FLAGS_OPER_UP;
    UtAssert_INT32_EQ(evhandler(NULL, &my_block.header.base_link), 0);
}

void TestBplibMpoolFlows_Register(void)
{
    UtTest_Add(test_bplib_mpool_subq_merge_list, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_subq_merge_list");
    UtTest_Add(test_bplib_mpool_subq_move_all, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_subq_move_all");
    UtTest_Add(test_bplib_mpool_subq_drop_all, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_subq_drop_all");
    UtTest_Add(test_bplib_mpool_subq_pull_single, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_subq_pull_single");
    UtTest_Add(test_bplib_mpool_flow_cast, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_flow_cast");
    UtTest_Add(test_bplib_mpool_flow_alloc, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_flow_alloc");
    UtTest_Add(test_bplib_mpool_flow_disable, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_flow_disable");
    UtTest_Add(test_bplib_mpool_flow_enable, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_flow_enable");
    UtTest_Add(test_bplib_mpool_flow_try_push, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_flow_try_push");
    UtTest_Add(test_bplib_mpool_flow_try_move_all, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_flow_try_move_all");
    UtTest_Add(test_bplib_mpool_flow_try_pull, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_flow_try_pull");
    UtTest_Add(test_bplib_mpool_flow_modify_flags, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_flow_modify_flags");
    UtTest_Add(test_bplib_mpool_flow_event_handler, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_flow_event_handler");
}
