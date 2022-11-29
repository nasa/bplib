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

#ifndef TEST_BPLIB_MPOOL_H
#define TEST_BPLIB_MPOOL_H

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "v7_mpool_internal.h"

typedef struct
{
    bplib_mpool_t               pool;
    bplib_mpool_block_content_t blk[3];
} UT_bplib_mpool_buf_t;

/* helper functions used in multiple tests */
void UT_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
int  test_bplib_mpool_callback_stub(void *arg, bplib_mpool_block_t *blk);
void test_make_singleton_link(bplib_mpool_t *parent_pool, bplib_mpool_block_t *b);
void test_setup_mpblock(bplib_mpool_t *pool, bplib_mpool_block_content_t *b, bplib_mpool_blocktype_t blktype,
                        uint32 sig);
void test_setup_allocation(bplib_mpool_t *pool, bplib_mpool_block_content_t *db, bplib_mpool_block_content_t *apib);

void TestBplibMpool_ResetTestEnvironment(void);

/* Registration functions */
void TestBplibMpoolBBlocks_Register(void);
void TestBplibMpoolFlows_Register(void);
void TestBplibMpoolJob_Register(void);
void TestBplibMpoolRef_Register(void);
void TestBplibMpoolBase_Register(void);
void TestBplibMpoolMPStream_Register(void);

#endif
