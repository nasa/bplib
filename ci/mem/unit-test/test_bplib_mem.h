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
int  Test_BPLib_MEM_CallbackStub(void *arg, BPLib_MEM_Block_t *blk);
void test_make_singleton_link(BPLib_MEM_Pool_t *parent_pool, BPLib_MEM_Block_t *b);
void test_setup_mpblock(BPLib_MEM_Pool_t *pool, BPLib_MEM_BlockContent_t *b, BPLib_MEM_Blocktype_t blktype,
                        uint32 sig);
void test_setup_allocation(BPLib_MEM_Pool_t *pool, BPLib_MEM_BlockContent_t *db, BPLib_MEM_BlockContent_t *apib);

/* Registration functions */
void Test_BPLib_MEM_Register(void);
void Test_BPLib_MEM_Subqs_Register(void);

// Memory Allocator Tests
void Test_BPLib_MEM_Init(void);
void Test_BPLib_MEM_PoolCreate(void);
void Test_BPLib_MEM_InitListHead(void);
void Test_BPLib_MEM_SubqInit(void);
void Test_BPLib_MEM_BlockFromExternalId(void);
void Test_BPLib_MEM_GetGenericDataCapacity(void);
void Test_BPLib_MEM_Maintain(void);

// MEM Lock Tests
void Test_BPLib_MEM_LockWait(void);

// MEM Pool Block Tests
void Test_BPLib_MEM_RegisterBlocktypeInternal(void);
void Test_BPLib_MEM_RegisterBlocktype(void);
void Test_BPLib_MEM_SubqPushSingle(void);
void Test_BPLib_MEM_SubqPullSingle(void);
void Test_BPLib_MEM_CollectBlocks(void);
void Test_BPLib_MEM_RecycleBlockInternal(void);
void Test_BPLIB_MEM_RecycleBlock(void);
void Test_BPLib_MEM_RecycleAllBlocksInList(void);
void Test_BPLib_MEM_GetBlockContentConst(void);
void Test_BPLib_MEM_GetUserContentSize(void);
void Test_BPLib_MEM_GetParentPoolFromLink(void);
void Test_BPLib_MEM_InitBaseObject(void);
void Test_BPLib_MEM_AllocBlockInternal(void);
void Test_BPLib_MEM_GenericDataAlloc(void);

// MEM Query Tests
void Test_BPLib_MEM_QueryMemCurrentUse(void);
void Test_BPLib_MEM_QueryMemMaxUse(void);

// MEM Ref Tests
// TODO Does MEM need Refs?
void Test_BPLib_MEM_BlockDereferenceContent(void);
void Test_BPLib_MEM_ReadRefcount(void);

// MEM Block Offset Tests
void Test_BPLib_MEM_GetUserDataOffsetByBlocktype(void);

// MEM Subq Tests
void Test_BPLib_MEM_SubqMergeList(void);
void Test_BPLib_MEM_SubqMoveAll(void);
void Test_BPLib_MEM_SubqDropAll(void);

// MEM Subq-as-List Iterator Tests
void Test_BPLib_MEM_ListIterGotoFirst(void);
void Test_BPLib_MEM_ListIterGotoLast(void);
void Test_BPLib_MEM_ListIterForward(void);
void Test_BPLib_MEM_ListIterReverse(void);
void Test_BPLib_MEM_ForeachItemInList(void);
void Test_BPLib_MEM_SearchList(void);

// MEM Debug Tests
void Test_BPLib_MEM_DebugPrintListStats(void);
void Test_BPLib_MEM_DebugScan(void);

#endif // TEST_BPLIB_MPOOL_H
