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

#include "bplib_api_types.h"

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"

#include "test_bplib_stor_qm.h"

// TODO Temporarily contains only what's required to provide stubs.


void test_setup_cache_state(BPLib_STOR_CACHE_Block_t *sblk)
{
    BPLib_STOR_CACHE_ListIter_t list_it;
    BPLib_STOR_CACHE_Block_t     pending_list;

    BPLib_STOR_CACHE_InitListHead(NULL, &pending_list);
    BPLib_STOR_CACHE_ListIterGotoFirst(&pending_list, &list_it);
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
    BPLib_Handle_t retval = BPLIB_HANDLE_RAM_STORE_BASE;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void UT_cache_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    UT_Stub_SetReturnValue(FuncKey, UserObj);
}

void UT_cache_egress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    #ifdef QM
    void *retval = NULL;
    int   count  = UT_GetStubCount(UT_KEY(BPLib_STOR_QM_DuctTryPull));
    if (count > 10)
    {
        UT_Stub_SetReturnValue(FuncKey, retval);
    }
    else
    {
        UT_Stub_SetReturnValue(FuncKey, UserObj);
    }
    #endif // QM
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

void UT_lib_ingress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    #ifdef QM
    void *retval = NULL;
    int   count  = UT_GetStubCount(UT_KEY(BPLib_STOR_QM_DuctTryPull));
    if (count > 15)
    {
        UT_Stub_SetReturnValue(FuncKey, retval);
    }
    else
    {
        UT_Stub_SetReturnValue(FuncKey, UserObj);
    }
    #endif // QM
}

void UT_lib_egress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    #ifdef QM
    void *retval = NULL;
    int   count  = UT_GetStubCount(UT_KEY(BPLib_STOR_QM_DuctTryPull));
    if (count > 20)
    {
        UT_Stub_SetReturnValue(FuncKey, retval);
    }
    else
    {
        UT_Stub_SetReturnValue(FuncKey, UserObj);
    }
    #endif // QM
}

void UT_lib_baseintf_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    #ifdef QM
    void *retval = NULL;
    int   count  = UT_GetStubCount(UT_KEY(BPLib_STOR_QM_DuctTryPull));
    if (count > 25)
    {
        UT_Stub_SetReturnValue(FuncKey, retval);
    }
    else
    {
        UT_Stub_SetReturnValue(FuncKey, UserObj);
    }
    #endif // QM
}

void UT_lib_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    UT_Stub_SetReturnValue(FuncKey, UserObj);
}

void UT_lib_sizet_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    size_t retval = 0;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void UT_lib_uint64_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32    StatusCode;
    uint64_t Result;

    UT_Stub_GetInt32StatusCode(Context, &StatusCode);

    Result = StatusCode;

    UT_Stub_SetReturnValue(FuncKey, Result);
}

void UT_lib_int8_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int8 retval = 0;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void UT_lib_bool_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bool retval = true;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void UT_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    UT_Stub_SetReturnValue(FuncKey, UserObj);
}

int Test_BPLib_STOR_CACHE_CallbackStub(void *arg, BPLib_STOR_CACHE_Block_t *blk)
{
    return UT_DEFAULT_IMPL(Test_BPLib_STOR_CACHE_CallbackStub);
}

void test_make_cblock_singleton_link(BPLib_STOR_CACHE_Pool_t *parent_pool, BPLib_STOR_CACHE_Block_t *b)
{
    if (parent_pool != NULL)
    {
        b->parent_offset = ((uintptr_t)b - (uintptr_t)parent_pool) / sizeof(BPLib_STOR_CACHE_BlockContent_t);
    }
    else
    {
        b->parent_offset = 0;
    }

    b->next = b;
    b->prev = b;
}

void UtTest_Setup(void)
{
    // TODO Test_BplibStorQM_Register();
    // TODO QM Test_BplibStorQM_ClaApi_Register();
    // TODO QM Test_BplibStorQM_DataServiceApi_Register();
    // TODO QM Test_BplibStorQM_Routing_Register();
}
