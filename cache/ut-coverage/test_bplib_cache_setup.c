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
#include "test_bplib_cache.h"

bplib_mpool_block_t retblk;

void test_setup_cache_state(bplib_mpool_block_t *sblk)
{
    bplib_mpool_list_iter_t list_it;
    bplib_mpool_block_t     pending_list;
    bplib_mpool_list_iter_goto_first(&pending_list, &list_it);
}

void UT_cache_sizet_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    size_t retval = 0;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void UT_cache_uint64_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32    StatusCode;
    uint64_t Result;

    UT_Stub_GetInt32StatusCode(Context, &StatusCode);

    Result = StatusCode;

    UT_Stub_SetReturnValue(FuncKey, Result);
}

void UT_cache_valid_bphandle_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bp_handle_t retval = BPLIB_HANDLE_RAM_STORE_BASE;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void UT_cache_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    UT_Stub_SetReturnValue(FuncKey, UserObj);
}

void UT_cache_egress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    void *retval = NULL;
    int   count  = UT_GetStubCount(UT_KEY(bplib_mpool_flow_try_pull));
    if (count > 10)
    {
        UT_Stub_SetReturnValue(FuncKey, retval);
    }
    else
    {
        UT_Stub_SetReturnValue(FuncKey, UserObj);
    }
}

void UT_cache_bool_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bool retval = true;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void UT_cache_GetTime_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    size_t retval = 1000;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void UT_cache_int8_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int8 retval = 0;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

bplib_mpool_block_t *test_bplib_cache_instantiate_stub(bplib_mpool_ref_t parent_ref, void *init_arg)
{
    memset(&retblk, 0, sizeof(bplib_mpool_block_t));
    return &retblk;
}

int test_bplib_cache_configure_stub(bplib_mpool_block_t *svc, int key, bplib_cache_module_valtype_t vt, const void *val)
{
    return UT_DEFAULT_IMPL(test_bplib_cache_configure_stub);
}

int test_bplib_cache_query_stub(bplib_mpool_block_t *svc, int key, bplib_cache_module_valtype_t vt, const void **val)
{
    return UT_DEFAULT_IMPL(test_bplib_cache_query_stub);
}

int test_bplib_cache_startstop_stub(bplib_mpool_block_t *svc)
{
    return UT_DEFAULT_IMPL(test_bplib_cache_startstop_stub);
}

int test_bplib_cache_offload_stub(bplib_mpool_block_t *svc, bp_sid_t *sid, bplib_mpool_block_t *pblk)
{
    return UT_DEFAULT_IMPL(test_bplib_cache_offload_stub);
}

int test_bplib_cache_restore_stub(bplib_mpool_block_t *svc, bp_sid_t sid, bplib_mpool_block_t **pblk)
{
    return UT_DEFAULT_IMPL(test_bplib_cache_restore_stub);
}

int test_bplib_cache_release_stub(bplib_mpool_block_t *svc, bp_sid_t sid)
{
    return UT_DEFAULT_IMPL(test_bplib_cache_release_stub);
}

void UtTest_Setup(void)
{
    TestBplibCacheCustody_Register();
    TestBplibCacheFsm_Register();
    TestBplibCache_Register();
}
