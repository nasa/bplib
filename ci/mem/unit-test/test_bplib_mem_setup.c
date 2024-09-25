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

/**
 * Include
 */

#include <string.h>

#include "test_bplib_mem.h"

#include "bplib_mem.h"
#include "bplib_mem_internal.h"


/**
 * Global Data
 */

/**
 * Macro Definitions
 */

/**
 * Functions
 */

void Test_Bplib_MEM_ResetTestEnvironment(void)
{
    UT_ResetState(0);
}

void UT_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    UT_Stub_SetReturnValue(FuncKey, UserObj);
}

int Test_BPLib_MEM_CallbackStub(void *arg, BPLib_MEM_Block_t *blk)
{
    return UT_DEFAULT_IMPL(Test_BPLib_MEM_CallbackStub);
}

void test_make_singleton_link(BPLib_MEM_Pool_t *parent_pool, BPLib_MEM_Block_t *b)
{
    if (parent_pool != NULL)
    {
        b->parent_offset = ((uintptr_t)b - (uintptr_t)parent_pool) / sizeof(BPLib_MEM_BlockContent_t);
    }
    else
    {
        b->parent_offset = 0;
    }

    b->next = b;
    b->prev = b;
}

void test_setup_mpblock(BPLib_MEM_Pool_t *pool, BPLib_MEM_BlockContent_t *b, BPLib_MEM_Blocktype_t blktype,
                        uint32 sig)
{
    b->header.base_link.type         = blktype;
    b->header.content_type_signature = sig;
    test_make_singleton_link(pool, &b->header.base_link);

    /*
     * Need to also initialize the fields within the content part, this calls the real internal function to do so.
     * It is done here because it is needed to be valid more often than not; test cases may unset/corrupt fields
     * if there is a specific path to target
     */
    memset(&b->u, 0, sizeof(b->u));
    switch (blktype)
    {
        case BPLib_MEM_BlocktypeAdmin:
            BPLib_MEM_SubqInit(&b->header.base_link, &b->u.admin.free_blocks);
            BPLib_MEM_SubqInit(&b->header.base_link, &b->u.admin.recycle_blocks);
            BPLib_MEM_InitListHead(&b->header.base_link, &b->u.admin.active_list);

        default:
            break;
    }
}

void test_setup_allocation(BPLib_MEM_Pool_t *pool, BPLib_MEM_BlockContent_t *db, BPLib_MEM_BlockContent_t *apib)
{
    BPLib_MEM_BlockAdminContent_t *admin;
    void                              *api_content;

    test_setup_mpblock(pool, &pool->admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(pool, db, BPLib_MEM_BlocktypeUndefined, 0);
    if (apib != NULL)
    {
        test_setup_mpblock(pool, apib, BPLib_MEM_BlocktypeApi, 0);
        api_content = &apib->u;
    }
    else
    {
        api_content = NULL;
    }

    admin = BPLib_MEM_GetAdmin(pool);
    BPLib_MEM_SubqPushSingle(&admin->free_blocks, &db->header.base_link);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_AltHandler_PointerReturn, api_content);
}

// UtTest_Setup is in bplib_mem_test_utils.c

