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
#include "v7_mpstream.h"

static void test_setup_append_mps_block(bplib_mpool_t *pool, bplib_mpool_stream_t *mps, bplib_mpool_block_content_t *b,
                                        uint8_t byte_val, size_t amount)
{
    test_setup_mpblock(pool, b, bplib_mpool_blocktype_generic, MPOOL_CACHE_CBOR_DATA_SIGNATURE);
    b->header.user_content_length = amount;
    memset(&b->u, byte_val, amount);
    bplib_mpool_insert_before(&mps->head, &b->header.base_link);
}

void test_bplib_mpool_start_stream_init(void)
{
    /* Test function for:
     * void   bplib_mpool_start_stream_init(bplib_mpool_stream_t *mps, bplib_mpool_t *pool, bplib_mpool_stream_dir_t
     * dir);
     */
    bplib_mpool_stream_t mps;

    UtAssert_VOIDCALL(bplib_mpool_start_stream_init(&mps, NULL, bplib_mpool_stream_dir_undefined));
}

void test_bplib_mpool_stream_write(void)
{
    /* Test function for:
     * size_t bplib_mpool_stream_write(bplib_mpool_stream_t *mps, const void *data, size_t size);
     */
    UT_bplib_mpool_buf_t buf;
    bplib_mpool_stream_t mps;
    uint8_t              data[BP_MPOOL_MIN_USER_BLOCK_SIZE];

    memset(&buf, 0, sizeof(buf));
    memset(&data, 0xEE, sizeof(data));

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);

    UtAssert_VOIDCALL(bplib_mpool_start_stream_init(&mps, NULL, bplib_mpool_stream_dir_undefined));
    UtAssert_ZERO(bplib_mpool_stream_write(&mps, data, sizeof(data)));

    UtAssert_VOIDCALL(bplib_mpool_start_stream_init(&mps, &buf.pool, bplib_mpool_stream_dir_write));
    UtAssert_ZERO(bplib_mpool_stream_write(&mps, data, 0));
    UtAssert_UINT32_EQ(bplib_mpool_stream_write(&mps, data, sizeof(data) / 2), sizeof(data) / 2);
    UtAssert_UINT32_EQ(bplib_mpool_stream_write(&mps, data, sizeof(data)), sizeof(data) / 2);
    UtAssert_ZERO(bplib_mpool_stream_write(&mps, data, sizeof(data)));
}

void test_bplib_mpool_stream_read(void)
{
    /* Test function for:
     * size_t bplib_mpool_stream_read(bplib_mpool_stream_t *mps, void *data, size_t size);
     */
    UT_bplib_mpool_buf_t buf;
    bplib_mpool_stream_t mps;
    uint8_t              data[BP_MPOOL_MIN_USER_BLOCK_SIZE];

    memset(&buf, 0, sizeof(buf));
    memset(&data, 0, sizeof(data));

    UtAssert_VOIDCALL(bplib_mpool_start_stream_init(&mps, NULL, bplib_mpool_stream_dir_undefined));
    UtAssert_ZERO(bplib_mpool_stream_read(&mps, data, sizeof(data)));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);

    UtAssert_VOIDCALL(bplib_mpool_start_stream_init(&mps, &buf.pool, bplib_mpool_stream_dir_read));
    UtAssert_ZERO(bplib_mpool_stream_read(&mps, data, 0));
    UtAssert_ZERO(bplib_mpool_stream_read(&mps, data, sizeof(data)));

    test_setup_append_mps_block(&buf.pool, &mps, &buf.blk[0], 0xAA, sizeof(data) / 2);
    test_setup_append_mps_block(&buf.pool, &mps, &buf.blk[1], 0xBB, sizeof(data) / 2);
    UtAssert_UINT32_EQ(bplib_mpool_stream_read(&mps, data, sizeof(data) / 4), sizeof(data) / 4);
    UtAssert_UINT32_EQ(bplib_mpool_stream_read(&mps, data, sizeof(data)), (3 * sizeof(data)) / 4);
    UtAssert_ZERO(bplib_mpool_stream_read(&mps, data, sizeof(data)));
}

void test_bplib_mpool_stream_seek(void)
{
    /* Test function for:
     * size_t bplib_mpool_stream_seek(bplib_mpool_stream_t *mps, size_t target_position);
     */
    UT_bplib_mpool_buf_t buf;
    bplib_mpool_stream_t mps;
    uint8_t              data[BP_MPOOL_MIN_USER_BLOCK_SIZE];

    memset(&buf, 0, sizeof(buf));
    memset(&data, 0xEE, sizeof(data));

    UtAssert_VOIDCALL(bplib_mpool_start_stream_init(&mps, NULL, bplib_mpool_stream_dir_undefined));
    UtAssert_ZERO(bplib_mpool_stream_seek(&mps, 0));
    UtAssert_ZERO(bplib_mpool_stream_seek(&mps, sizeof(data)));
    UtAssert_ZERO(bplib_mpool_stream_tell(&mps));

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);

    /* Write streams are seekable beyond the end, which should extend the data */
    UtAssert_VOIDCALL(bplib_mpool_start_stream_init(&mps, &buf.pool, bplib_mpool_stream_dir_write));
    test_setup_append_mps_block(&buf.pool, &mps, &buf.blk[2], 0xCC, BP_MPOOL_MIN_USER_BLOCK_SIZE);
    mps.last_eblk       = &buf.blk[2].header.base_link;
    mps.stream_position = BP_MPOOL_MIN_USER_BLOCK_SIZE;
    mps.curr_pos        = BP_MPOOL_MIN_USER_BLOCK_SIZE;
    mps.curr_limit      = BP_MPOOL_MIN_USER_BLOCK_SIZE;
    UtAssert_UINT32_EQ(bplib_mpool_stream_seek(&mps, BP_MPOOL_MIN_USER_BLOCK_SIZE + sizeof(data)),
                       BP_MPOOL_MIN_USER_BLOCK_SIZE + sizeof(data));
    UtAssert_UINT32_EQ(bplib_mpool_stream_seek(&mps, 1), 1);
    UtAssert_UINT32_EQ(bplib_mpool_stream_seek(&mps, sizeof(data) / 2), sizeof(data) / 2);
    UtAssert_UINT32_EQ(bplib_mpool_stream_tell(&mps), sizeof(data) / 2);

    /* Read streams are NOT seekable beyond the end */
    UtAssert_VOIDCALL(bplib_mpool_start_stream_init(&mps, &buf.pool, bplib_mpool_stream_dir_read));
    test_setup_append_mps_block(&buf.pool, &mps, &buf.blk[0], 0xAA, sizeof(data) / 2);
    test_setup_append_mps_block(&buf.pool, &mps, &buf.blk[1], 0xBB, sizeof(data) / 2);
    UtAssert_UINT32_EQ(bplib_mpool_stream_seek(&mps, 2 * sizeof(data)), sizeof(data));
    UtAssert_UINT32_EQ(bplib_mpool_stream_seek(&mps, sizeof(data) / 4), sizeof(data) / 4);
    UtAssert_UINT32_EQ(bplib_mpool_stream_tell(&mps), sizeof(data) / 4);
}

void test_bplib_mpool_stream_attach(void)
{
    /* Test function for:
     * void   bplib_mpool_stream_attach(bplib_mpool_stream_t *mps, bplib_mpool_block_t *head);
     */
    bplib_mpool_stream_t mps;
    bplib_mpool_block_t  list;

    UtAssert_VOIDCALL(bplib_mpool_start_stream_init(&mps, NULL, bplib_mpool_stream_dir_undefined));

    bplib_mpool_init_list_head(NULL, &list);
    UtAssert_VOIDCALL(bplib_mpool_stream_attach(&mps, &list));
}

void test_bplib_mpool_stream_close(void)
{
    /* Test function for:
     * void bplib_mpool_stream_close(bplib_mpool_stream_t *mps);
     */
    bplib_mpool_t        pool;
    bplib_mpool_stream_t mps;

    test_setup_mpblock(&pool, &pool.admin_block, bplib_mpool_blocktype_admin, 0);

    UtAssert_VOIDCALL(bplib_mpool_start_stream_init(&mps, &pool, bplib_mpool_stream_dir_undefined));
    UtAssert_VOIDCALL(bplib_mpool_stream_close(&mps));
}

void TestBplibMpoolMPStream_Register(void)
{
    UtTest_Add(test_bplib_mpool_start_stream_init, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_start_stream_init");
    UtTest_Add(test_bplib_mpool_stream_write, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_stream_write");
    UtTest_Add(test_bplib_mpool_stream_read, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_stream_read");
    UtTest_Add(test_bplib_mpool_stream_seek, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_stream_seek");
    UtTest_Add(test_bplib_mpool_stream_attach, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_stream_attach");
    UtTest_Add(test_bplib_mpool_stream_close, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_stream_close");
}
