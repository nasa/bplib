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

void TestBplibMpool_ResetTestEnvironment(void)
{
    UT_ResetState(0);
}

void UT_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    UT_Stub_SetReturnValue(FuncKey, UserObj);
}

int test_bplib_mpool_callback_stub(void *arg, bplib_mpool_block_t *blk)
{
    return UT_DEFAULT_IMPL(test_bplib_mpool_callback_stub);
}

void test_make_singleton_link(bplib_mpool_t *parent_pool, bplib_mpool_block_t *b)
{
    if (parent_pool != NULL)
    {
        b->parent_offset = ((uintptr_t)b - (uintptr_t)parent_pool) / sizeof(bplib_mpool_block_content_t);
    }
    else
    {
        b->parent_offset = 0;
    }

    b->next = b;
    b->prev = b;
}

void test_setup_mpblock(bplib_mpool_t *pool, bplib_mpool_block_content_t *b, bplib_mpool_blocktype_t blktype,
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
        case bplib_mpool_blocktype_primary:
            bplib_mpool_bblock_primary_init(&b->header.base_link, &b->u.primary.pblock);
            break;
        case bplib_mpool_blocktype_canonical:
            bplib_mpool_bblock_canonical_init(&b->header.base_link, &b->u.canonical.cblock);
            break;
        case bplib_mpool_blocktype_admin:
            bplib_mpool_subq_init(&b->header.base_link, &b->u.admin.free_blocks);
            bplib_mpool_subq_init(&b->header.base_link, &b->u.admin.recycle_blocks);
            bplib_mpool_init_list_head(&b->header.base_link, &b->u.admin.active_list);
            break;
        case bplib_mpool_blocktype_flow:
            bplib_mpool_flow_init(&b->header.base_link, &b->u.flow.fblock);
            break;

        default:
            break;
    }
}

void test_setup_allocation(bplib_mpool_t *pool, bplib_mpool_block_content_t *db, bplib_mpool_block_content_t *apib)
{
    bplib_mpool_block_admin_content_t *admin;
    void                              *api_content;

    test_setup_mpblock(pool, &pool->admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(pool, db, bplib_mpool_blocktype_undefined, 0);
    if (apib != NULL)
    {
        test_setup_mpblock(pool, apib, bplib_mpool_blocktype_api, 0);
        api_content = &apib->u;
    }
    else
    {
        api_content = NULL;
    }

    admin = bplib_mpool_get_admin(pool);
    bplib_mpool_subq_push_single(&admin->free_blocks, &db->header.base_link);
    UT_SetHandlerFunction(UT_KEY(bplib_rbt_search_generic), UT_AltHandler_PointerReturn, api_content);
}

void UtTest_Setup(void)
{
    TestBplibMpoolBase_Register();
    TestBplibMpoolRef_Register();
    TestBplibMpoolBBlocks_Register();
    TestBplibMpoolJob_Register();
    TestBplibMpoolFlows_Register();
    TestBplibMpoolMPStream_Register();
}
