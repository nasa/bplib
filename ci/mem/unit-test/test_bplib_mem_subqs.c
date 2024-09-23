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
    #ifdef BPLIB_MEM_SUBQ_TEST
    BPLib_MEM_SubqBase_t *subq = UserObj;
    BPLib_MEM_Block_t *blk;

    blk = BPLib_MEM_SubqPullSingle(&subq->block_list);
    if (blk)
    {
        BPLib_MEM_SubqPushSingle(&subq->block_list, blk);
    }
    #endif // BPLIB_MEM_SUBQ_TEST
}

void Test_BPLib_MEM_SubqMergeList(void)
{
    #ifdef BPLIB_MEM_SUBQ_TESTS
    /* Test function for:
     * uint32_t bplib_mpool_subq_merge_list(bplib_mpool_subq_base_t *subq_dst, bplib_mpool_block_t *list)
     */
    UT_BPLib_MEM_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeGeneric, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeGeneric, 0);
    UtAssert_UINT32_EQ(BPLib_MEM_SubqMergeList(&buf.blk[0].u.generic_data,
                                               &buf.blk[1].u.generic_data.user_data_start),
                       0);

    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeGeneric, 0);
    BPLib_MEM_SubqPushSingle(&buf.blk[1].u.generic_data, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(BPLib_MEM_SubqMergeList(&buf.blk[0].u.generic_data,
                                                   &buf.blk[1].u.generic_data.user_data_start),
                       1);
    // TODO Is there a valid test for push_count on list of generic?
    // UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.push_count, 1);
    // UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.pull_count, 0);
    #endif // BPLIB_MEM_SUBQ_TESTS
}

void Test_BPLib_MEM_SubqMoveAll(void)
{
    #ifdef BPLIB_MEM_SUBQ_TEST
    /* Test function for:
     * uint32_t bplib_mpool_subq_move_all(bplib_mpool_subq_base_t *subq_dst, bplib_mpool_subq_base_t *subq_src)
     */
    UT_BPLib_MEM_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeGeneric, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeGeneric, 0);
    UtAssert_UINT32_EQ(BPLib_MEM_SubqMoveAll(&buf.blk[0].u.generic_data,
                                                 &buf.blk[1].u.generic_data.user_data_start),
                       0);

    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeGeneric, 0);
    BPLib_MEM_SubqPushSingle(&buf.blk[1].u.generic_data, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(BPLib_MEM_SubqMoveAll(&buf.blk[0].u.generic_data,
                                                 &buf.blk[1].u.generic_data.user_data_start),
                       1);
    // TODO Are there values to check for generic push/pull?
    // UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.push_count, 1);
    // UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.pull_count, 0);
    // UtAssert_UINT32_EQ(buf.blk[1].u.flow.fblock.ingress.base_subq.push_count, 1);
    // UtAssert_UINT32_EQ(buf.blk[1].u.flow.fblock.ingress.base_subq.pull_count, 1);
    #endif // BPLIB_MEM_SUBQ_TEST
}

void Test_BPLib_MEM_SubqDropAll(void)
{
    /* Test function for:
     * uint32_t bplib_mpool_subq_drop_all(bplib_mpool_t *pool, bplib_mpool_subq_base_t *subq)
     */
    #ifdef BPLIB_MEM_SUBQ_TEST
    UT_BPLib_MEM_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeGeneric, 0);
    UtAssert_UINT32_EQ(BPLib_MEM_SubqDropAll(&buf.pool, &buf.blk[0].u.generic_data.user_data_start), 0);

    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeGeneric, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeGeneric, 0);
    BPLib_MEM_SubqPushSingle(&buf.blk[0].u.generic_data, &buf.blk[1].u.generic_data.user_data_start);
    BPLib_MEM_SubqPushSingle(&buf.blk[0].u.generic_data, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(BPLib_MEM_SubqDropAll(&buf.pool, &buf.blk[0].u.generic_data.user_data_start), 2);
    
    // TODO push counts for generic subqs?
    // UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.push_count, 2);
    // UtAssert_UINT32_EQ(buf.blk[0].u.flow.fblock.ingress.base_subq.pull_count, 2);
    #endif // BPLIB_MEM_SUBQ_TEST
}

void Test_BPLib_MEM_SubqPullSingle(void)
{
    #ifdef BPLIB_MEM_SUBQ_TEST
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_subq_pull_single(bplib_mpool_subq_base_t *subq)
     */
    UT_BPLib_MEM_Buf_t buf;

    memset(&buf, 0, sizeof(buf));
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeGeneric, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeGeneric, 0);

    UtAssert_NULL(bplib_mpool_subq_pull_single(&buf.blk[0].u.generic_data));
    #endif // BPLIB_MEM_SUBQ_TEST
}
