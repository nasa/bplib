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

#ifndef TEST_BPLIB_CACHE_H
#define TEST_BPLIB_CACHE_H

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"
#include "bplib_api_types.h"
#include "v7_cache_internal.h"

void test_setup_cache_state(BPLib_MEM_block_t *sblk);
void UT_cache_sizet_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_cache_uint64_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_cache_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_cache_egress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_cache_valid_bphandle_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_cache_bool_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_cache_GetTime_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_cache_int8_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void TestBplibCacheCustody_Register(void);
void TestBplibCacheFsm_Register(void);
void TestBplibCache_Register(void);
BPLib_MEM_block_t *test_bplib_cache_instantiate_stub(BPLib_MEM_ref_t parent_ref, void *init_arg);
int                  test_bplib_cache_configure_stub(BPLib_MEM_block_t *svc, int key, bplib_cache_module_valtype_t vt,
                                                     const void *val);
int test_bplib_cache_query_stub(BPLib_MEM_block_t *svc, int key, bplib_cache_module_valtype_t vt, const void **val);
int test_bplib_cache_startstop_stub(BPLib_MEM_block_t *svc);
int test_bplib_cache_offload_stub(BPLib_MEM_block_t *svc, bp_sid_t *sid, BPLib_MEM_block_t *pblk);
int test_bplib_cache_restore_stub(BPLib_MEM_block_t *svc, bp_sid_t sid, BPLib_MEM_block_t **pblk);
int test_bplib_cache_release_stub(BPLib_MEM_block_t *svc, bp_sid_t sid);

#endif
