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

#ifndef TEST_BPLIB_STOR_QM_H
#define TEST_BPLIB_STOR_QM_H

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_api_types.h"

#include "bplib_mem.h"
#include "bplib_rbt.h"

#include "bplib_stor_cache.h"

#include "bplib_stor_ps_file_offload.h"

#include "bplib_stor_qm.h"

typedef struct
{
    BPLib_STOR_CACHE_Pool_t               pool;
    BPLib_STOR_CACHE_BlockContent_t blk[3];
} UT_BPLib_STOR_QM_Buf_t;

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

void Test_BplibCacheCustody_Register(void);

// File Offload API Stubs
// TODO Rename to STOR_PS from STOR_CACHE.
BPLib_STOR_CACHE_Block_t *Test_BPLib_STOR_QM_InstantiateStub(BPLib_STOR_CACHE_Ref_t parent_ref, void *init_arg);
int Test_BPLib_STOR_QM_ConfigureStub(BPLib_STOR_CACHE_Pool_t *svc, BPLib_Handle_t cache_intf_id, int key, BPLib_STOR_QM_ModuleValtype_t vt, const void *val);
int Test_BPLib_STOR_CACHE_QueryStub(BPLib_STOR_CACHE_Pool_t *svc, BPLib_Handle_t module_intf_id, int key, BPLib_STOR_QM_ModuleValtype_t vt, const void **val);
int Test_BPLib_STOR_CACHE_StartstopStub(BPLib_STOR_CACHE_Block_t *svc, BPLib_Handle_t module_intf_id);
int Test_BPLib_STOR_CACHE_OffloadStub(BPLib_STOR_CACHE_Block_t *svc, BPLib_STOR_CACHE_Sid_t *sid, BPLib_STOR_CACHE_Block_t *pblk);
int Test_BPLib_STOR_CACHE_RestoreStub(BPLib_STOR_CACHE_Block_t *svc, BPLib_STOR_CACHE_Sid_t sid, BPLib_STOR_CACHE_Block_t **pblk);
int Test_BPLib_STOR_CACHE_ReleaseStub(BPLib_STOR_CACHE_Block_t *svc, BPLib_STOR_CACHE_Sid_t sid);

void UT_lib_ingress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_egress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_baseintf_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_sizet_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_uint64_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_int8_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_bool_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);

void Test_BplibCache_ClaApi_Register(void);
void Test_BplibCache_DataServiceApi_Register(void);
void Test_BplibCache_Routing_Register(void);

#endif // TEST_BPLIB_STOR_QM_H
