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

static void test_setup_append_mps_block(BPLib_STOR_CACHE_Pool_t *pool, BPLib_STOR_CACHE_Stream_t *mps, BPLib_STOR_CACHE_BlockContent_t *b,
                                        uint8_t byte_val, size_t amount)
{
    test_setup_mpblock(pool, b, BPLib_STOR_CACHE_BlocktypeGeneric, BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE);
    b->header.user_content_length = amount;
    memset(&b->u, byte_val, amount);
    BPLib_STOR_CACHE_InsertBefore(&mps->head, &b->header.base_link);
}

void test_BPLib_STOR_CACHE_StartStreamInit(void)
{
    /* Test function for:
     * void   BPLib_STOR_CACHE_StartStreamInit(BPLib_STOR_CACHE_Stream_t *mps, BPLib_STOR_CACHE_Pool_t *pool, BPLib_STOR_CACHE_StreamDir_t
     * dir);
     */
    BPLib_STOR_CACHE_Stream_t mps;

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_StartStreamInit(&mps, NULL, BPLib_STOR_CACHE_StreamDirUndefined));
}

void test_BPLib_STOR_CACHE_StreamWrite(void)
{
    /* Test function for:
     * size_t BPLib_STOR_CACHE_StreamWrite(BPLib_STOR_CACHE_Stream_t *mps, const void *data, size_t size);
     */
    UT_BPLib_STOR_CACHE_Buf_t buf;
    BPLib_STOR_CACHE_Stream_t mps;
    uint8_t              data[BPLIB_MEM_MIN_USER_BLOCK_SIZE];

    memset(&buf, 0, sizeof(buf));
    memset(&data, 0xEE, sizeof(data));

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_StartStreamInit(&mps, NULL, BPLib_STOR_CACHE_StreamDirUndefined));
    UtAssert_ZERO(BPLib_STOR_CACHE_StreamWrite(&mps, data, sizeof(data)));

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_StartStreamInit(&mps, &buf.pool, BPLib_STOR_CACHE_StreamDirWrite));
    UtAssert_ZERO(BPLib_STOR_CACHE_StreamWrite(&mps, data, 0));
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_StreamWrite(&mps, data, sizeof(data) / 2), sizeof(data) / 2);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_StreamWrite(&mps, data, sizeof(data)), sizeof(data) / 2);
    UtAssert_ZERO(BPLib_STOR_CACHE_StreamWrite(&mps, data, sizeof(data)));
}

void test_BPLib_STOR_CACHE_StreamRead(void)
{
    /* Test function for:
     * size_t BPLib_STOR_CACHE_StreamRead(BPLib_STOR_CACHE_Stream_t *mps, void *data, size_t size);
     */
    UT_BPLib_STOR_CACHE_Buf_t buf;
    BPLib_STOR_CACHE_Stream_t mps;
    uint8_t              data[BPLIB_MEM_MIN_USER_BLOCK_SIZE];

    memset(&buf, 0, sizeof(buf));
    memset(&data, 0, sizeof(data));

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_StartStreamInit(&mps, NULL, BPLib_STOR_CACHE_StreamDirUndefined));
    UtAssert_ZERO(BPLib_STOR_CACHE_StreamRead(&mps, data, sizeof(data)));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_StartStreamInit(&mps, &buf.pool, BPLib_STOR_CACHE_StreamDirRead));
    UtAssert_ZERO(BPLib_STOR_CACHE_StreamRead(&mps, data, 0));
    UtAssert_ZERO(BPLib_STOR_CACHE_StreamRead(&mps, data, sizeof(data)));

    test_setup_append_mps_block(&buf.pool, &mps, &buf.blk[0], 0xAA, sizeof(data) / 2);
    test_setup_append_mps_block(&buf.pool, &mps, &buf.blk[1], 0xBB, sizeof(data) / 2);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_StreamRead(&mps, data, sizeof(data) / 4), sizeof(data) / 4);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_StreamRead(&mps, data, sizeof(data)), (3 * sizeof(data)) / 4);
    UtAssert_ZERO(BPLib_STOR_CACHE_StreamRead(&mps, data, sizeof(data)));
}

void test_BPLib_STOR_CACHE_StreamSeek(void)
{
    /* Test function for:
     * size_t BPLib_STOR_CACHE_StreamSeek(BPLib_STOR_CACHE_Stream_t *mps, size_t target_position);
     */
    UT_BPLib_STOR_CACHE_Buf_t buf;
    BPLib_STOR_CACHE_Stream_t mps;
    uint8_t              data[BPLIB_MEM_MIN_USER_BLOCK_SIZE];

    memset(&buf, 0, sizeof(buf));
    memset(&data, 0xEE, sizeof(data));

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_StartStreamInit(&mps, NULL, BPLib_STOR_CACHE_StreamDirUndefined));
    UtAssert_ZERO(BPLib_STOR_CACHE_StreamSeek(&mps, 0));
    UtAssert_ZERO(BPLib_STOR_CACHE_StreamSeek(&mps, sizeof(data)));
    UtAssert_ZERO(BPLib_STOR_CACHE_StreamTell(&mps));

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);

    /* Write streams are seekable beyond the end, which should extend the data */
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_StartStreamInit(&mps, &buf.pool, BPLib_STOR_CACHE_StreamDirWrite));
    test_setup_append_mps_block(&buf.pool, &mps, &buf.blk[2], 0xCC, BPLIB_MEM_MIN_USER_BLOCK_SIZE);
    mps.last_eblk       = &buf.blk[2].header.base_link;
    mps.stream_position = BPLIB_MEM_MIN_USER_BLOCK_SIZE;
    mps.curr_pos        = BPLIB_MEM_MIN_USER_BLOCK_SIZE;
    mps.curr_limit      = BPLIB_MEM_MIN_USER_BLOCK_SIZE;
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_StreamSeek(&mps, BPLIB_MEM_MIN_USER_BLOCK_SIZE + sizeof(data)),
                       BPLIB_MEM_MIN_USER_BLOCK_SIZE + sizeof(data));
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_StreamSeek(&mps, 1), 1);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_StreamSeek(&mps, sizeof(data) / 2), sizeof(data) / 2);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_StreamTell(&mps), sizeof(data) / 2);

    /* Read streams are NOT seekable beyond the end */
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_StartStreamInit(&mps, &buf.pool, BPLib_STOR_CACHE_StreamDirRead));
    test_setup_append_mps_block(&buf.pool, &mps, &buf.blk[0], 0xAA, sizeof(data) / 2);
    test_setup_append_mps_block(&buf.pool, &mps, &buf.blk[1], 0xBB, sizeof(data) / 2);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_StreamSeek(&mps, 2 * sizeof(data)), sizeof(data));
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_StreamSeek(&mps, sizeof(data) / 4), sizeof(data) / 4);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_StreamTell(&mps), sizeof(data) / 4);
}

void test_BPLib_STOR_CACHE_StreamAttach(void)
{
    /* Test function for:
     * void   BPLib_STOR_CACHE_StreamAttach(BPLib_STOR_CACHE_Stream_t *mps, BPLib_STOR_CACHE_Block_t *head);
     */
    BPLib_STOR_CACHE_Stream_t mps;
    BPLib_STOR_CACHE_Block_t  list;

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_StartStreamInit(&mps, NULL, BPLib_STOR_CACHE_StreamDirUndefined));

    BPLib_STOR_CACHE_InitListHead(NULL, &list);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_StreamAttach(&mps, &list));
}

void test_BPLib_STOR_CACHE_StreamClose(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_StreamClose(BPLib_STOR_CACHE_Stream_t *mps);
     */
    BPLib_STOR_CACHE_Pool_t        pool;
    BPLib_STOR_CACHE_Stream_t mps;

    test_setup_mpblock(&pool, &pool.admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_StartStreamInit(&mps, &pool, BPLib_STOR_CACHE_StreamDirUndefined));
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_StreamClose(&mps));
}

void TestBplibMpoolMPStream_Register(void)
{
    UtTest_Add(test_BPLib_STOR_CACHE_StartStreamInit, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_STOR_CACHE_StartStreamInit");
    UtTest_Add(test_BPLib_STOR_CACHE_StreamWrite, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_StreamWrite");
    UtTest_Add(test_BPLib_STOR_CACHE_StreamRead, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_StreamRead");
    UtTest_Add(test_BPLib_STOR_CACHE_StreamSeek, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_StreamSeek");
    UtTest_Add(test_BPLib_STOR_CACHE_StreamAttach, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_StreamAttach");
    UtTest_Add(test_BPLib_STOR_CACHE_StreamClose, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_STOR_CACHE_StreamClose");
}
