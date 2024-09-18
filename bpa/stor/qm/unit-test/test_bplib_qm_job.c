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

#include "test_bplib_mpool.h"

struct UT_job_block
{
    bplib_mpool_block_header_t blkh;
    bplib_mpool_job_t          job;
};

union UT_job_block_content
{
    bplib_mpool_block_content_t reserved_space;
    bplib_mpool_block_t         block;
    struct UT_job_block         content;
};

struct UT_job_poolbuf
{
    bplib_mpool_t              pool;
    union UT_job_block_content u;
};

void test_bplib_mpool_job_init(void)
{
    /* Test function for:
     * void bplib_mpool_job_init(bplib_mpool_block_t *base_block, bplib_mpool_job_t *jblk)
     */
    struct UT_job_block buf;

    memset(&buf, 0, sizeof(buf));

    UtAssert_VOIDCALL(bplib_mpool_job_init(&buf.blkh.base_link, &buf.job));
}

void test_bplib_mpool_job_cast(void)
{
    /* Test function for:
     * bplib_mpool_job_t *bplib_mpool_job_cast(bplib_mpool_block_t *cb)
     */
    bplib_mpool_block_t blk;

    UtAssert_NULL(bplib_mpool_job_cast(NULL));

    memset(&blk, 0, sizeof(blk));
    UtAssert_NULL(bplib_mpool_job_cast(&blk));

    blk.type = bplib_mpool_blocktype_job;
    UtAssert_ADDRESS_EQ(bplib_mpool_job_cast(&blk), &blk);
}

void test_bplib_mpool_job_mark_active(void)
{
    /* Test function for:
     * void bplib_mpool_job_mark_active(bplib_mpool_job_t *job)
     */
    struct UT_job_poolbuf              buf;
    bplib_mpool_block_admin_content_t *admin;
    bplib_mpool_job_t                 *job;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.u.reserved_space, bplib_mpool_blocktype_generic, 0);

    job   = &buf.u.content.job;
    admin = bplib_mpool_get_admin(&buf.pool);

    bplib_mpool_job_init(&buf.u.block, job);

    job->handler = test_bplib_mpool_callback_stub;
    UtAssert_VOIDCALL(bplib_mpool_job_mark_active(job));
    UtAssert_ADDRESS_EQ(bplib_mpool_get_next_block(&admin->active_list), job);

    job->handler = NULL;
    UtAssert_VOIDCALL(bplib_mpool_job_mark_active(job));
    UtAssert_BOOL_TRUE(bplib_mpool_is_empty_list_head(&admin->active_list));
    UtAssert_BOOL_TRUE(bplib_mpool_is_link_unattached(&job->link));
}

void test_bplib_mpool_job_get_next_active(void)
{
    /* Test function for:
     * bplib_mpool_job_t *bplib_mpool_job_get_next_active(bplib_mpool_t *pool)
     */

    struct UT_job_poolbuf              buf;
    bplib_mpool_block_admin_content_t *admin;
    bplib_mpool_job_t                 *job;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.u.reserved_space, bplib_mpool_blocktype_generic, 0);

    job   = &buf.u.content.job;
    admin = bplib_mpool_get_admin(&buf.pool);

    bplib_mpool_job_init(&buf.u.block, job);
    job->handler = test_bplib_mpool_callback_stub;
    bplib_mpool_insert_after(&admin->active_list, &job->link);

    UtAssert_ADDRESS_EQ(bplib_mpool_job_get_next_active(&buf.pool), job);
    UtAssert_BOOL_TRUE(bplib_mpool_is_empty_list_head(&admin->active_list));
    UtAssert_BOOL_TRUE(bplib_mpool_is_link_unattached(&job->link));

    /* insert something into active_list that is NOT a job */
    bplib_mpool_insert_after(&admin->active_list, &buf.u.block);
    UtAssert_NULL(bplib_mpool_job_get_next_active(&buf.pool));
}

void test_bplib_mpool_job_run_all(void)
{
    /* Test function for:
     * void bplib_mpool_job_run_all(bplib_mpool_t *pool, void *arg)
     */

    struct UT_job_poolbuf              buf;
    bplib_mpool_block_admin_content_t *admin;
    bplib_mpool_job_t                 *job;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.u.reserved_space, bplib_mpool_blocktype_generic, 0);

    job   = &buf.u.content.job;
    admin = bplib_mpool_get_admin(&buf.pool);

    bplib_mpool_job_init(&buf.u.block, job);
    job->handler = test_bplib_mpool_callback_stub;
    bplib_mpool_insert_after(&admin->active_list, &job->link);

    UtAssert_VOIDCALL(bplib_mpool_job_run_all(&buf.pool, NULL));
    UtAssert_BOOL_TRUE(bplib_mpool_is_empty_list_head(&admin->active_list));
    UtAssert_BOOL_TRUE(bplib_mpool_is_link_unattached(&job->link));

    job->handler = NULL;
    bplib_mpool_insert_after(&admin->active_list, &job->link);

    UtAssert_VOIDCALL(bplib_mpool_job_run_all(&buf.pool, NULL));
    UtAssert_BOOL_TRUE(bplib_mpool_is_empty_list_head(&admin->active_list));
    UtAssert_BOOL_TRUE(bplib_mpool_is_link_unattached(&job->link));
}

void TestBplibMpoolJob_Register(void)
{
    UtTest_Add(test_bplib_mpool_job_init, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_job_init");
    UtTest_Add(test_bplib_mpool_job_cast, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_job_cast");
#ifdef jphfix
    UtTest_Add(test_bplib_mpool_job_cancel_internal, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_job_cancel_internal");
    UtTest_Add(test_bplib_mpool_job_mark_active_internal, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_job_mark_active_internal");
#endif
    UtTest_Add(test_bplib_mpool_job_mark_active, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_job_mark_active");
    UtTest_Add(test_bplib_mpool_job_get_next_active, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_job_get_next_active");
    UtTest_Add(test_bplib_mpool_job_run_all, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_job_run_all");
}
