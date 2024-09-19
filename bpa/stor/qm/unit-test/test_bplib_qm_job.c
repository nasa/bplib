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

struct UT_job_block
{
    BPLib_STOR_CACHE_BlockHeader_t blkh;
    BPLib_STOR_CACHE_Job_t          job;
};

union UT_job_block_content
{
    BPLib_STOR_CACHE_BlockContent_t reserved_space;
    BPLib_STOR_CACHE_Block_t         block;
    struct UT_job_block         content;
};

struct UT_job_poolbuf
{
    BPLib_STOR_CACHE_Pool_t              pool;
    union UT_job_block_content u;
};

void test_BPLib_STOR_CACHE_JobInit(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_JobInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Job_t *jblk)
     */
    struct UT_job_block buf;

    memset(&buf, 0, sizeof(buf));

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_JobInit(&buf.blkh.base_link, &buf.job));
}

void test_BPLib_STOR_CACHE_JobCast(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Job_t *BPLib_STOR_CACHE_JobCast(BPLib_STOR_CACHE_Block_t *cb)
     */
    BPLib_STOR_CACHE_Block_t blk;

    UtAssert_NULL(BPLib_STOR_CACHE_JobCast(NULL));

    memset(&blk, 0, sizeof(blk));
    UtAssert_NULL(BPLib_STOR_CACHE_JobCast(&blk));

    blk.type = BPLib_STOR_CACHE_BlocktypeJob;
    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_JobCast(&blk), &blk);
}

void test_BPLib_STOR_CACHE_JobMarkActive(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_JobMarkActive(BPLib_STOR_CACHE_Job_t *job)
     */
    struct UT_job_poolbuf              buf;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;
    BPLib_STOR_CACHE_Job_t                 *job;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.u.reserved_space, BPLib_STOR_CACHE_BlocktypeGeneric, 0);

    job   = &buf.u.content.job;
    admin = BPLib_STOR_CACHE_GetAdmin(&buf.pool);

    BPLib_STOR_CACHE_JobInit(&buf.u.block, job);

    job->handler = test_BPLib_STOR_CACHE_CallbackStub;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_JobMarkActive(job));
    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_GetNextBlock(&admin->active_list), job);

    job->handler = NULL;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_JobMarkActive(job));
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsEmptyListHead(&admin->active_list));
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsLinkUnattached(&job->link));
}

void test_BPLib_STOR_CACHE_JobGetNextActive(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Job_t *BPLib_STOR_CACHE_JobGetNextActive(BPLib_STOR_CACHE_Pool_t *pool)
     */

    struct UT_job_poolbuf              buf;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;
    BPLib_STOR_CACHE_Job_t                 *job;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.u.reserved_space, BPLib_STOR_CACHE_BlocktypeGeneric, 0);

    job   = &buf.u.content.job;
    admin = BPLib_STOR_CACHE_GetAdmin(&buf.pool);

    BPLib_STOR_CACHE_JobInit(&buf.u.block, job);
    job->handler = test_BPLib_STOR_CACHE_CallbackStub;
    BPLib_STOR_CACHE_InsertAfter(&admin->active_list, &job->link);

    UtAssert_ADDRESS_EQ(BPLib_STOR_CACHE_JobGetNextActive(&buf.pool), job);
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsEmptyListHead(&admin->active_list));
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsLinkUnattached(&job->link));

    /* insert something into active_list that is NOT a job */
    BPLib_STOR_CACHE_InsertAfter(&admin->active_list, &buf.u.block);
    UtAssert_NULL(BPLib_STOR_CACHE_JobGetNextActive(&buf.pool));
}

void test_BPLib_STOR_CACHE_JobRunAll(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_JobRunAll(BPLib_STOR_CACHE_Pool_t *pool, void *arg)
     */

    struct UT_job_poolbuf              buf;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;
    BPLib_STOR_CACHE_Job_t                 *job;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.u.reserved_space, BPLib_STOR_CACHE_BlocktypeGeneric, 0);

    job   = &buf.u.content.job;
    admin = BPLib_STOR_CACHE_GetAdmin(&buf.pool);

    BPLib_STOR_CACHE_JobInit(&buf.u.block, job);
    job->handler = test_BPLib_STOR_CACHE_CallbackStub;
    BPLib_STOR_CACHE_InsertAfter(&admin->active_list, &job->link);

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_JobRunAll(&buf.pool, NULL));
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsEmptyListHead(&admin->active_list));
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsLinkUnattached(&job->link));

    job->handler = NULL;
    BPLib_STOR_CACHE_InsertAfter(&admin->active_list, &job->link);

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_JobRunAll(&buf.pool, NULL));
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsEmptyListHead(&admin->active_list));
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_IsLinkUnattached(&job->link));
}

void TestBplibMpoolJob_Register(void)
{
    UtTest_Add(test_BPLib_STOR_CACHE_JobInit, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_JobInit");
    UtTest_Add(test_BPLib_STOR_CACHE_JobCast, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_JobCast");
#ifdef jphfix
    UtTest_Add(test_BPLib_STOR_CACHE_JobCancelInternal, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_JobCancelInternal");
    UtTest_Add(test_BPLib_STOR_CACHE_JobMarkActiveInternal, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_JobMarkActiveInternal");
#endif
    UtTest_Add(test_BPLib_STOR_CACHE_JobMarkActive, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_JobMarkActive");
    UtTest_Add(test_BPLib_STOR_CACHE_JobGetNextActive, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_JobGetNextActive");
    UtTest_Add(test_BPLib_STOR_CACHE_JobRunAll, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_JobRunAll");
}
