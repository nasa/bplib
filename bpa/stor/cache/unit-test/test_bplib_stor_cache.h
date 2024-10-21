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

#ifndef TEST_BPLIB_STOR_CACHE_H
#define TEST_BPLIB_STOR_CACHE_H

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_api_types.h"

#include "bplib_stor_qm.h"

#include "bplib_mem.h"
#include "bplib_rbt.h"

#include "bplib_stor_cache.h"
#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_internal.h"
#include "bplib_stor_cache_base_internal.h"
#include "bplib_stor_cache_ref.h"
#include "bplib_stor_cache_block.h"
#include "bplib_stor_cache_ref.h"

typedef struct
{
    BPLib_STOR_CACHE_Pool_t               pool;
    BPLib_STOR_CACHE_BlockContent_t blk[3];
} UT_BPLib_STOR_CACHE_Buf_t;

void UT_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
int  Test_BPLib_STOR_CACHE_CallbackStub(void *arg, BPLib_STOR_CACHE_Block_t *blk);
void test_make_cblock_singleton_link(BPLib_STOR_CACHE_Pool_t *parent_pool, BPLib_STOR_CACHE_Block_t *b);
void test_setup_cpool_block(BPLib_STOR_CACHE_Pool_t *pool, BPLib_STOR_CACHE_BlockContent_t *b, BPLib_STOR_CACHE_Blocktype_t blktype,
                            uint32 sig);
void test_setup_cpool_allocation(BPLib_STOR_CACHE_Pool_t *pool, BPLib_STOR_CACHE_BlockContent_t *db, BPLib_STOR_CACHE_BlockContent_t *apib);

void test_setup_cache_state(BPLib_STOR_CACHE_Block_t *sblk);
void UT_cache_sizet_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_cache_uint64_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_cache_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_cache_egress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_cache_valid_bphandle_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_cache_bool_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_cache_GetTime_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_cache_int8_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);

void Test_BPLib_STOR_CACHE_Create(void);
void Test_BPLib_STOR_CACHE_BblockPrimaryAlloc(void);
void Test_BPLib_STOR_CACHE_EntryMakePending(void);
void Test_BPLib_STOR_QM_Attach(void);
void Test_BPLib_STOR_QM_Detach(void);
void Test_BPLib_STOR_QM_RegisterModuleService(void);
void Test_BPLib_STOR_QM_Configure(void);
void Test_BPLib_STOR_QM_Query(void);
void Test_BPLib_STOR_QM_Start(void);
void Test_BPLib_STOR_QM_Stop(void);
void Test_BPLib_STOR_CACHE_DebugScanQueue(void);
void Test_BPLib_STOR_CACHE_EgressImpl(void);
void Test_BPLib_STOR_CACHE_FlushPending(void);
void Test_BPLib_STOR_CACHE_DoPoll(void);
#ifdef UNUSED_ENTRIES_MAKE_PENDING
void Test_BPLib_STOR_CACHE_EntriesMakePending(void);
#endif // UNUSED_ENTRIES_MAKE_PENDING
void Test_BPLib_STOR_CACHE_DoIntfStatechange(void);
void Test_BPLib_STOR_CACHE_EventImpl(void);
void Test_BPLib_STOR_CACHE_ProcessPending(void);
void Test_BPLib_STOR_CACHE_DestructState(void);
void Test_BPLib_STOR_CACHE_ConstructEntry(void);
void Test_BPLib_STOR_CACHE_DestructEntry(void);
void Test_BPLib_STOR_CACHE_ConstructBlockref(void);
void Test_BPLib_STOR_CACHE_DestructBlockref(void);
void Test_BPLib_STOR_CACHE_ConstructState(void);
void Test_BPLib_STOR_CACHE_EntryTreeInsertUnsorted(void);

void UT_lib_ingress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_egress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_baseintf_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_sizet_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_uint64_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_int8_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_bool_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);

void Test_BplibStorCache_Register(void);
void Test_BplibStorCacheFsm_Register(void);

#endif // TEST_BPLIB_STOR_CACHE_H
