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

#include "../../qm/inc/bplib_stor_qm_ducts.h"

#include "test_bplib_stor_cache.h"
#include "utilities/bplib_stor_cache_utils.h"

BPLib_STOR_CACHE_Block_t retblk;

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
    int   count  = UT_GetStubCount(UT_KEY(BPLib_STOR_QM_DuctTryPull));
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

BPLib_STOR_CACHE_Block_t *Test_BPLib_STOR_CACHE_InstantiateStub(BPLib_STOR_CACHE_Ref_t parent_ref, void *init_arg)
{
    memset(&retblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    return &retblk;
}

int Test_BPLib_STOR_CACHE_ConfigureStub(BPLib_STOR_CACHE_Block_t *svc, int key, BPLib_STOR_CACHE_ModuleValtype_t vt, const void *val)
{
    return UT_DEFAULT_IMPL(Test_BPLib_STOR_CACHE_ConfigureStub);
}

int Test_BPLib_STOR_CACHE_QueryStub(BPLib_STOR_CACHE_Block_t *svc, int key, BPLib_STOR_CACHE_ModuleValtype_t vt, const void **val)
{
    return UT_DEFAULT_IMPL(Test_BPLib_STOR_CACHE_QueryStub);
}

int Test_BPLib_STOR_CACHE_StartstopStub(BPLib_STOR_CACHE_Block_t *svc)
{
    return UT_DEFAULT_IMPL(Test_BPLib_STOR_CACHE_StartstopStub);
}

int Test_BPLib_STOR_CACHE_OffloadStub(BPLib_STOR_CACHE_Block_t *svc, bp_sid_t *sid, BPLib_STOR_CACHE_Block_t *pblk)
{
    return UT_DEFAULT_IMPL(Test_BPLib_STOR_CACHE_OffloadStub);
}

int Test_BPLib_STOR_CACHE_RestoreStub(BPLib_STOR_CACHE_Block_t *svc, bp_sid_t sid, BPLib_STOR_CACHE_Block_t **pblk)
{
    return UT_DEFAULT_IMPL(Test_BPLib_STOR_CACHE_RestoreStub);
}

int Test_BPLib_STOR_CACHE_ReleaseStub(BPLib_STOR_CACHE_Block_t *svc, bp_sid_t sid)
{
    return UT_DEFAULT_IMPL(Test_BPLib_STOR_CACHE_ReleaseStub);
}

void UT_lib_ingress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
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
}

void UT_lib_egress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
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
}

void UT_lib_baseintf_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
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

void test_setup_cpool_block(BPLib_STOR_CACHE_Pool_t *pool, BPLib_STOR_CACHE_BlockContent_t *b, BPLib_STOR_CACHE_Blocktype_t blktype,
                            uint32 sig)
{
    b->header.base_link.type         = blktype;
    b->header.content_type_signature = sig;
    test_make_cblock_singleton_link(pool, &b->header.base_link);

    /*
     * Need to also initialize the fields within the content part, this calls the real internal function to do so.
     * It is done here because it is needed to be valid more often than not; test cases may unset/corrupt fields
     * if there is a specific path to target
     */
    memset(&b->u, 0, sizeof(b->u));
    switch (blktype)
    {
        case BPLib_STOR_CACHE_BlocktypeAdmin:
            BPLib_STOR_CACHE_SubqInit(&b->header.base_link, &b->u.admin.free_blocks);
            BPLib_STOR_CACHE_SubqInit(&b->header.base_link, &b->u.admin.recycle_blocks);
            BPLib_STOR_CACHE_InitListHead(&b->header.base_link, &b->u.admin.active_list);

        default:
            break;
    }
}

void test_setup_cpool_allocation(BPLib_STOR_CACHE_Pool_t *pool, BPLib_STOR_CACHE_BlockContent_t *db, BPLib_STOR_CACHE_BlockContent_t *apib)
{
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;
    void                          *api_content;

    test_setup_cpool_block(pool, &pool->admin_block, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    test_setup_cpool_block(pool, db, BPLib_STOR_CACHE_BlocktypeUndefined, 0);
    if (apib != NULL)
    {
        test_setup_cpool_block(pool, apib, BPLib_STOR_CACHE_BlocktypeApi, 0);
        api_content = &apib->u;
    }
    else
    {
        api_content = NULL;
    }

    admin = BPLib_STOR_CACHE_GetAdmin(pool);
    BPLib_STOR_CACHE_SubqPushSingle(&admin->free_blocks, &db->header.base_link);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_AltHandler_PointerReturn, api_content);
}


void Test_BplibStorCache_Register(void)
{
    ADD_TEST(Test_BPLib_STOR_CACHE_BblockPrimaryAlloc);
    // ADD_TEST(Test_BPLib_STOR_CACHE_EntryMakePending);
    // ADD_TEST(Test_BPLib_STOR_CACHE_Attach);
    // ADD_TEST(Test_BPLib_STOR_CACHE_Detach);
    // ADD_TEST(Test_BPLib_STOR_CACHE_RegisterModuleService);
    // ADD_TEST(Test_BPLib_STOR_CACHE_Configure);
    // ADD_TEST(Test_BPLib_STOR_CACHE_Query);
    // ADD_TEST(Test_BPLib_STOR_CACHE_Start);
    // ADD_TEST(Test_BPLib_STOR_CACHE_Stop);
    // ADD_TEST(Test_BPLib_STOR_CACHE_DebugScanQueue);
    // ADD_TEST(Test_BPLib_STOR_CACHE_EgressImpl);
    // ADD_TEST(Test_BPLib_STOR_CACHE_FlushPending);
    // ADD_TEST(Test_BPLib_STOR_CACHE_DoPoll);
    // ADD_TEST(Test_BPLib_STOR_CACHE_DoRouteUp);
    // ADD_TEST(Test_BPLib_STOR_CACHE_DoIntfStatechange);
    // ADD_TEST(Test_BPLib_STOR_CACHE_EventImpl);
    // ADD_TEST(Test_BPLib_STOR_CACHE_ProcessPending);
    // ADD_TEST(Test_BPLib_STOR_CACHE_DestructState);
    // ADD_TEST(Test_BPLib_STOR_CACHE_ConstructEntry);
    // ADD_TEST(Test_BPLib_STOR_CACHE_DestructEntry);
    // ADD_TEST(Test_BPLib_STOR_CACHE_ConstructBlockref);
    // ADD_TEST(Test_BPLib_STOR_CACHE_DestructBlockref);
    // ADD_TEST(Test_BPLib_STOR_CACHE_ConstructState);
    // ADD_TEST(Test_BPLib_STOR_CACHE_EntryTreeInsertUnsorted);
}
