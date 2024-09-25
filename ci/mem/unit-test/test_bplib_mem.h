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

#include "../src/bplib_mem_internal.h"

#include "bplib_mem.h"

typedef struct
{
    BPLib_MEM_Pool_t         pool;
    BPLib_MEM_BlockContent_t blk[3];
} UT_BPLib_MEM_Buf_t;

/* helper functions used in multiple tests */
void UT_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
int  test_BPLib_MEM_CallbackStub(void *arg, BPLib_MEM_Block_t *blk);
void test_make_singleton_link(BPLib_MEM_Pool_t *parent_pool, BPLib_MEM_Block_t *b);
void test_setup_mpblock(BPLib_MEM_Pool_t *pool, BPLib_MEM_BlockContent_t *b, BPLib_MEM_Blocktype_t blktype,
                        uint32 sig);
void test_setup_allocation(BPLib_MEM_Pool_t *pool, BPLib_MEM_BlockContent_t *db, BPLib_MEM_BlockContent_t *apib);

/* Registration functions */
void Test_BPLib_MEM_Register(void);
void Test_BPLib_MEM_Subqs_Register(void);

void Test_BPLib_MEM_SubqMergeList(void);
void Test_BPLib_MEM_SubqMoveAll(void);
void Test_BPLib_MEM_SubqDropAll(void);
void Test_BPLib_MEM_SubqPullSingle(void);

#endif // TEST_BPLIB_MPOOL_H