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

#include "bplib_os.h"

#include "bplib_mem.h"
#include "bplib_mem_internal.h"
#include "test_BPLib_MEM.h"

const uint32 UT_TESTBLOCKTYPE_SIG = 0x5f33c01a;

void UT_AltHandler_PointerReturnForSignature(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bp_val_t RefSig = UT_Hook_GetArgValueByName(Context, "search_key_value", bp_val_t);

    if (RefSig == UT_TESTBLOCKTYPE_SIG)
    {
        UserObj = NULL;
    }

    UT_Stub_SetReturnValue(FuncKey, UserObj);
}

void test_BPLib_MEM_LockInit(void)
{
    /* Test function for:
     * void BPLib_MEM_LockInit(void)
     */

    UT_SetDefaultReturnValue(UT_KEY(bplib_os_createlock), 1);

    /* Call it twice, first time should init, second time should skip init */
    UtAssert_VOIDCALL(BPLib_MEM_LockInit());
    UtAssert_VOIDCALL(BPLib_MEM_LockInit());
}

void test_BPLib_MEM_LockPrepare(void)
{
    /* Test function for:
     * BPLib_MEM_lock_t *BPLib_MEM_LockPrepare(void *resource_addr)
     */

    UtAssert_NOT_NULL(BPLib_MEM_LockPrepare(NULL));
}

void test_BPLib_MEM_LockResource(void)
{
    /* Test function for:
     * BPLib_MEM_lock_t *BPLib_MEM_LockResource(void *resource_addr)
     */

    UtAssert_NOT_NULL(BPLib_MEM_LockResource(NULL));
}

void test_BPLib_MEM_LockWait(void)
{
    /* Test function for:
     * bool BPLib_MEM_LockWait(BPLib_MEM_lock_t *lock, uint64_t until_dtntime)
     */

    BPLib_MEM_lock_t *lock = BPLib_MEM_LockPrepare(NULL);

    UT_SetDefaultReturnValue(UT_KEY(bplib_os_get_dtntime_ms), 1000);
    UtAssert_BOOL_FALSE(BPLib_MEM_LockWait(lock, 0));
    UtAssert_BOOL_TRUE(BPLib_MEM_LockWait(lock, 5000));
    UT_SetDefaultReturnValue(UT_KEY(bplib_os_wait_until_ms), BP_TIMEOUT);
    UtAssert_BOOL_FALSE(BPLib_MEM_LockWait(lock, 5000));
}

void test_BPLib_MEM_BlockFromExternalId(void)
{
    /* Test function for:
     * BPLib_MEM_block_t *BPLib_MEM_BlockFromExternalId(BPLib_MEM_t *pool, bp_handle_t handle)
     */
    UT_BPLib_MEM_buf_t buf;
    bp_handle_t          id1;
    bp_handle_t          id2;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(BPLib_MEM_BlockFromExternalId(&buf.pool, BP_INVALID_HANDLE));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypePrimary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypePrimary, 0);

    UtAssert_BOOL_TRUE(bp_handle_is_valid(id1 = BPLib_MEM_GetExternalId(&buf.blk[0].header.base_link)));
    UtAssert_BOOL_TRUE(bp_handle_is_valid(id2 = BPLib_MEM_GetExternalId(&buf.blk[1].header.base_link)));
    UtAssert_NULL(BPLib_MEM_BlockFromExternalId(&buf.pool, id1));
    UtAssert_NULL(BPLib_MEM_BlockFromExternalId(&buf.pool, id2));

    buf.pool.admin_block.u.admin.num_bufs_total = 3;
    UtAssert_ADDRESS_EQ(BPLib_MEM_BlockFromExternalId(&buf.pool, id1), &buf.blk[0].header.base_link);
    UtAssert_ADDRESS_EQ(BPLib_MEM_BlockFromExternalId(&buf.pool, id2), &buf.blk[1].header.base_link);
}

void test_BPLib_MEM_GetBlockFromLink(void)
{
    /* Test function for:
     * BPLib_MEM_block_t *BPLib_MEM_GetBlockFromLink(BPLib_MEM_block_t *lblk)
     */
    struct
    {
        BPLib_MEM_block_t base_blk;
        uint8_t             unused1[3];
        BPLib_MEM_block_t sublink_blk1;
        uint8_t             unused2[7];
        BPLib_MEM_block_t sublink_blk2;
    } test_block;

    memset(&test_block, 0, sizeof(test_block));

    UtAssert_NULL(BPLib_MEM_GetBlockFromLink(NULL));
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetBlockFromLink(&test_block.base_blk), &test_block.base_blk);

    test_block.base_blk.type = BPLib_MEM_BlocktypeUndefined;
    BPLib_MEM_InitListHead(&test_block.base_blk, &test_block.sublink_blk1);
    BPLib_MEM_InitListHead(&test_block.base_blk, &test_block.sublink_blk2);

    UtAssert_ADDRESS_EQ(BPLib_MEM_GetBlockFromLink(&test_block.sublink_blk1), &test_block.base_blk);
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetBlockFromLink(&test_block.sublink_blk2), &test_block.base_blk);

    test_block.sublink_blk2.parent_offset = 0; /* Invalid, but forces check path */
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetBlockFromLink(&test_block.sublink_blk2), &test_block.sublink_blk2);

    test_block.base_blk.type = BPLib_MEM_BlocktypeAdmin;
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetBlockFromLink(&test_block.base_blk), &test_block.base_blk);
}

void test_BPLib_MEM_GetBlockContent(void)
{
    /* Test function for:
     * BPLib_MEM_BlockContent_t *BPLib_MEM_GetBlockContent(BPLib_MEM_block_t *cb)
     * const BPLib_MEM_BlockContent_t *BPLib_MEM_GetBlockContentConst(const BPLib_MEM_block_t *cb)
     *
     * NOTE: these two functions are identical except for the const-ness of input and output pointers.
     */
    BPLib_MEM_BlockContent_t my_block;

    memset(&my_block, 0, sizeof(my_block));

    UtAssert_NULL(BPLib_MEM_GetBlockContent(NULL));
    UtAssert_NULL(BPLib_MEM_GetBlockContentConst(NULL));
    UtAssert_NULL(BPLib_MEM_GetBlockContent(&my_block.header.base_link));
    UtAssert_NULL(BPLib_MEM_GetBlockContentConst(&my_block.header.base_link));

    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypePrimary, 0);
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetBlockContent(&my_block.header.base_link), &my_block);
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetBlockContentConst(&my_block.header.base_link), &my_block);
}

void test_BPLib_MEM_BlockDereferenceContent(void)
{
    /* Test function for:
     * BPLib_MEM_BlockContent_t *BPLib_MEM_BlockDereferenceContent(BPLib_MEM_block_t *cb)
     */

    BPLib_MEM_BlockContent_t my_block;
    BPLib_MEM_BlockContent_t my_ref;

    UtAssert_NULL(BPLib_MEM_BlockDereferenceContent(NULL));

    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypePrimary, 0);
    UtAssert_ADDRESS_EQ(BPLib_MEM_BlockDereferenceContent(&my_block.header.base_link), &my_block);

    test_setup_mpblock(NULL, &my_ref, BPLib_MEM_BlocktypeRef, 0);
    my_ref.u.ref.pref_target = &my_block;
    UtAssert_ADDRESS_EQ(BPLib_MEM_BlockDereferenceContent(&my_ref.header.base_link), &my_block);
}

void test_BPLib_MEM_GetUserDataOffsetByBlocktype(void)
{
    /* Test function for:
     * size_t BPLib_MEM_GetUserDataOffsetByBlocktype(BPLib_MEM_blocktype_t bt)
     */

    UtAssert_UINT32_EQ(BPLib_MEM_GetUserDataOffsetByBlocktype(BPLib_MEM_BlocktypeUndefined), SIZE_MAX);
    UtAssert_UINT32_EQ(BPLib_MEM_GetUserDataOffsetByBlocktype(BPLib_MEM_BlocktypeMax), SIZE_MAX);
    UtAssert_UINT32_EQ(BPLib_MEM_GetUserDataOffsetByBlocktype(BPLib_MEM_BlocktypeGeneric), 0);
}

void test_BPLib_MEM_GetGenericDataCapacity(void)
{
    /* Test function for:
     * size_t BPLib_MEM_GetGenericDataCapacity(const BPLib_MEM_block_t *cb)
     */
    BPLib_MEM_BlockContent_t my_block;

    memset(&my_block, 0, sizeof(my_block));

    UtAssert_ZERO(BPLib_MEM_GetGenericDataCapacity(&my_block.header.base_link));

    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypeGeneric, 0);
    UtAssert_UINT32_EQ(BPLib_MEM_GetGenericDataCapacity(&my_block.header.base_link),
                       sizeof(BPLib_MEM_BlockBuffer_t));
}

void test_BPLib_MEM_InitSecondaryLink(void)
{
    /* Test function for:
     * void BPLib_MEM_InitSecondaryLink(BPLib_MEM_block_t *base_block, BPLib_MEM_block_t *secondary_link,
     * BPLib_MEM_blocktype_t block_type)
     */
    BPLib_MEM_BlockContent_t my_block;

    memset(&my_block, 0, sizeof(my_block));

    UtAssert_VOIDCALL(
        BPLib_MEM_InitSecondaryLink(NULL, &my_block.header.base_link, BPLib_MEM_BlocktypeSecondaryGeneric));
    UtAssert_VOIDCALL(BPLib_MEM_InitSecondaryLink(
        &my_block.header.base_link, &my_block.u.primary.pblock.cblock_list, BPLib_MEM_BlocktypeListHead));
}

void test_BPLib_MEM_InitListHead(void)
{
    /* Test function for:
     * void BPLib_MEM_InitListHead(BPLib_MEM_block_t *base_block, BPLib_MEM_block_t *list_head)
     */
    BPLib_MEM_BlockContent_t my_block;

    memset(&my_block, 0, sizeof(my_block));

    UtAssert_VOIDCALL(BPLib_MEM_InitListHead(&my_block.header.base_link, &my_block.u.primary.pblock.cblock_list));
}

void test_BPLib_MEM_InsertAfter(void)
{
    /* Test function for:
     * void BPLib_MEM_InsertAfter(BPLib_MEM_block_t *list, BPLib_MEM_block_t *node)
     */
    struct
    {
        BPLib_MEM_block_t base_blk;
        BPLib_MEM_block_t head_blk;
    } list;
    BPLib_MEM_block_t content[2];

    memset(&list, 0, sizeof(list));
    memset(&content, 0, sizeof(content));
    test_make_singleton_link(NULL, &content[0]);
    test_make_singleton_link(NULL, &content[1]);

    UtAssert_VOIDCALL(BPLib_MEM_InitListHead(&list.base_blk, &list.head_blk));

    UtAssert_VOIDCALL(BPLib_MEM_InsertAfter(&list.head_blk, &content[0]));
    UtAssert_VOIDCALL(BPLib_MEM_InsertAfter(&list.head_blk, &content[1]));

    UtAssert_ADDRESS_EQ(list.head_blk.prev, &content[0]);
    UtAssert_ADDRESS_EQ(list.head_blk.next, &content[1]);
    UtAssert_ADDRESS_EQ(content[1].prev, &list.head_blk);
    UtAssert_ADDRESS_EQ(content[1].next, &content[0]);
    UtAssert_ADDRESS_EQ(content[0].prev, &content[1]);
    UtAssert_ADDRESS_EQ(content[0].next, &list.head_blk);
}

void test_BPLib_MEM_InsertBefore(void)
{
    /* Test function for:
     * void BPLib_MEM_InsertBefore(BPLib_MEM_block_t *list, BPLib_MEM_block_t *node)
     */

    struct
    {
        BPLib_MEM_block_t base_blk;
        BPLib_MEM_block_t head_blk;
    } list;
    BPLib_MEM_block_t content[2];

    memset(&list, 0, sizeof(list));
    memset(&content, 0, sizeof(content));
    test_make_singleton_link(NULL, &content[0]);
    test_make_singleton_link(NULL, &content[1]);

    UtAssert_VOIDCALL(BPLib_MEM_InitListHead(&list.base_blk, &list.head_blk));

    UtAssert_VOIDCALL(BPLib_MEM_InsertBefore(&list.head_blk, &content[0]));
    UtAssert_VOIDCALL(BPLib_MEM_InsertBefore(&list.head_blk, &content[1]));

    UtAssert_ADDRESS_EQ(list.head_blk.prev, &content[1]);
    UtAssert_ADDRESS_EQ(list.head_blk.next, &content[0]);
    UtAssert_ADDRESS_EQ(content[0].prev, &list.head_blk);
    UtAssert_ADDRESS_EQ(content[0].next, &content[1]);
    UtAssert_ADDRESS_EQ(content[1].prev, &content[0]);
    UtAssert_ADDRESS_EQ(content[1].next, &list.head_blk);
}

void test_BPLib_MEM_ExtractNode(void)
{
    /* Test function for:
     * void BPLib_MEM_ExtractNode(BPLib_MEM_block_t *node)
     */

    struct
    {
        BPLib_MEM_block_t base_blk;
        BPLib_MEM_block_t head_blk;
    } list;
    BPLib_MEM_block_t content[2];

    memset(&list, 0, sizeof(list));
    memset(&content, 0, sizeof(content));
    test_make_singleton_link(NULL, &content[0]);
    test_make_singleton_link(NULL, &content[1]);

    UtAssert_VOIDCALL(BPLib_MEM_InitListHead(&list.base_blk, &list.head_blk));

    UtAssert_VOIDCALL(BPLib_MEM_InsertBefore(&list.head_blk, &content[0]));
    UtAssert_VOIDCALL(BPLib_MEM_InsertBefore(&list.head_blk, &content[1]));
    UtAssert_VOIDCALL(BPLib_MEM_ExtractNode(&content[0]));

    UtAssert_BOOL_TRUE(BPLib_MEM_IsLinkAttached(&content[1]));
    UtAssert_BOOL_TRUE(BPLib_MEM_IsLinkUnattached(&content[0]));

    UtAssert_VOIDCALL(BPLib_MEM_ExtractNode(&content[1]));
    UtAssert_BOOL_TRUE(BPLib_MEM_IsEmptyListHead(&list.head_blk));
    UtAssert_BOOL_TRUE(BPLib_MEM_IsLinkUnattached(&content[1]));
    UtAssert_BOOL_TRUE(BPLib_MEM_IsLinkUnattached(&content[0]));
}

void test_BPLib_MEM_MergeList(void)
{
    /* Test function for:
     * void BPLib_MEM_MergeList(BPLib_MEM_block_t *dest, BPLib_MEM_block_t *src)
     */
    struct
    {
        BPLib_MEM_block_t base_blk;
        BPLib_MEM_block_t head_blk;
    } list[2];
    BPLib_MEM_block_t  content[2];
    BPLib_MEM_block_t *blk;

    memset(&list, 0, sizeof(list));
    memset(&content, 0, sizeof(content));
    test_make_singleton_link(NULL, &content[0]);
    test_make_singleton_link(NULL, &content[1]);

    UtAssert_VOIDCALL(BPLib_MEM_InitListHead(&list[0].base_blk, &list[0].head_blk));
    UtAssert_VOIDCALL(BPLib_MEM_InitListHead(&list[1].base_blk, &list[1].head_blk));
    UtAssert_VOIDCALL(BPLib_MEM_InsertBefore(&list[0].head_blk, &content[0]));
    UtAssert_VOIDCALL(BPLib_MEM_InsertBefore(&list[1].head_blk, &content[1]));

    UtAssert_VOIDCALL(BPLib_MEM_MergeList(&list[1].head_blk, &list[0].head_blk));

    /* Note "merge" puts everything into one list, including both heads */
    /* follow the links forward and backward */
    blk = &list[1].head_blk;

    UtAssert_ADDRESS_EQ(blk = BPLib_MEM_GetNextBlock(blk), &content[1]);
    UtAssert_ADDRESS_EQ(blk = BPLib_MEM_GetNextBlock(blk), &list[0].head_blk);
    UtAssert_ADDRESS_EQ(blk = BPLib_MEM_GetNextBlock(blk), &content[0]);
    UtAssert_ADDRESS_EQ(blk = BPLib_MEM_GetNextBlock(blk), &list[1].head_blk);

    UtAssert_ADDRESS_EQ(blk = BPLib_MEM_GetPrevBlock(blk), &content[0]);
    UtAssert_ADDRESS_EQ(blk = BPLib_MEM_GetPrevBlock(blk), &list[0].head_blk);
    UtAssert_ADDRESS_EQ(blk = BPLib_MEM_GetPrevBlock(blk), &content[1]);
    UtAssert_ADDRESS_EQ(blk = BPLib_MEM_GetPrevBlock(blk), &list[1].head_blk);
}

void test_BPLib_MEM_GetUserContentSize(void)
{
    /* Test function for:
     * size_t BPLib_MEM_GetUserContentSize(const BPLib_MEM_block_t *cb)
     */
    BPLib_MEM_BlockContent_t my_block;

    UtAssert_ZERO(BPLib_MEM_GetUserContentSize(NULL));

    memset(&my_block, 0, sizeof(my_block));
    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypeGeneric, 0);
    my_block.header.user_content_length = 14;

    UtAssert_UINT32_EQ(BPLib_MEM_GetUserContentSize(&my_block.header.base_link), 14);
}

void test_BPLib_MEM_ReadRefcount(void)
{
    /* Test function for:
     * size_t BPLib_MEM_ReadRefcount(const BPLib_MEM_block_t *cb)
     */
    BPLib_MEM_BlockContent_t my_block;

    UtAssert_ZERO(BPLib_MEM_ReadRefcount(NULL));

    memset(&my_block, 0, sizeof(my_block));
    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypeGeneric, 0);
    my_block.header.refcount = 6;

    UtAssert_UINT32_EQ(BPLib_MEM_ReadRefcount(&my_block.header.base_link), 6);
}

void test_BPLib_MEM_GetParentPoolFromLink(void)
{
    /* Test function for:
     * BPLib_MEM_t *BPLib_MEM_GetParentPoolFromLink(BPLib_MEM_block_t *cb)
     */

    UT_BPLib_MEM_buf_t buf;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(BPLib_MEM_GetParentPoolFromLink(NULL));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypePrimary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypePrimary, 0);

    UtAssert_ADDRESS_EQ(BPLib_MEM_GetParentPoolFromLink(&buf.blk[0].header.base_link), &buf.pool);
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetParentPoolFromLink(&buf.blk[0].u.primary.pblock.cblock_list), &buf.pool);
}

void test_BPLib_MEM_GenericDataCast(void)
{
    /* Test function for:
     * void *BPLib_MEM_GenericDataCast(BPLib_MEM_block_t *cb, uint32_t required_magic)
     * BPLib_MEM_block_t *BPLib_MEM_GenericDataUncast(void *blk, BPLib_MEM_blocktype_t parent_bt, uint32_t
     * required_magic)
     *
     * These two functions are inverse ops so they are paired together
     */
    static const uint32         UT_SIG = 0x4953ab26;
    BPLib_MEM_BlockContent_t my_block;
    BPLib_MEM_BlockContent_t my_ref;

    UtAssert_NULL(BPLib_MEM_GenericDataCast(NULL, 0));

    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypeAdmin, 0);
    UtAssert_NULL(BPLib_MEM_GenericDataCast(&my_block.header.base_link, 0));
    UtAssert_NULL(BPLib_MEM_GenericDataUncast(&my_block.u, BPLib_MEM_BlocktypeAdmin, 0));

    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypeGeneric, UT_SIG);

    UtAssert_ADDRESS_EQ(BPLib_MEM_GenericDataCast(&my_block.header.base_link, UT_SIG), &my_block.u);
    UtAssert_NULL(BPLib_MEM_GenericDataCast(&my_block.header.base_link, ~UT_SIG));

    UtAssert_ADDRESS_EQ(BPLib_MEM_GenericDataUncast(&my_block.u, BPLib_MEM_BlocktypeGeneric, UT_SIG), &my_block);
    UtAssert_NULL(BPLib_MEM_GenericDataUncast(&my_block.u, BPLib_MEM_BlocktypeGeneric, ~UT_SIG));
    UtAssert_NULL(BPLib_MEM_GenericDataUncast(&my_block.u, BPLib_MEM_BlocktypeGeneric, ~UT_SIG));
    UtAssert_NULL(BPLib_MEM_GenericDataUncast(&my_block.u, BPLib_MEM_BlocktypeFlow, UT_SIG));

    test_setup_mpblock(NULL, &my_ref, BPLib_MEM_BlocktypeRef, 0);
    my_ref.u.ref.pref_target = &my_block;
    UtAssert_ADDRESS_EQ(BPLib_MEM_GenericDataCast(&my_ref.header.base_link, UT_SIG), &my_block.u);
}

void test_BPLib_MEM_InitBaseObject(void)
{
    /* Test function for:
     * void BPLib_MEM_InitBaseObject(BPLib_MEM_BlockHeader_t *block_hdr, uint16_t user_content_length, uint32_t
     * content_type_signature)
     */

    static const uint32         UT_SIG = 0x2c9f27d0;
    static const uint16         UT_LEN = sizeof(BPLib_MEM_GenericDataContent_t);
    BPLib_MEM_BlockContent_t my_block;

    memset(&my_block, 0, sizeof(my_block));

    UtAssert_VOIDCALL(BPLib_MEM_InitBaseObject(&my_block.header, UT_LEN, UT_SIG));

    UtAssert_UINT32_EQ(my_block.header.content_type_signature, UT_SIG);
    UtAssert_UINT16_EQ(my_block.header.user_content_length, UT_LEN);
}

void test_BPLib_MEM_AllocBlockInternal(void)
{
    /* Test function for:
     * BPLib_MEM_BlockContent_t *BPLib_MEM_AllocBlockInternal(BPLib_MEM_t *pool, BPLib_MEM_blocktype_t
     * blocktype, uint32_t content_type_signature, void *init_arg, uint8_t priority)
     */

    UT_BPLib_MEM_buf_t               buf;
    BPLib_MEM_BlockAdminContent_t *admin;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(BPLib_MEM_AllocBlockInternal(&buf.pool, BPLib_MEM_BlocktypeUndefined, 0, NULL, 0));
    UtAssert_NULL(BPLib_MEM_AllocBlockInternal(&buf.pool, BPLib_MEM_BlocktypeMax, 0, NULL, 0));
    UtAssert_NULL(BPLib_MEM_AllocBlockInternal(&buf.pool, BPLib_MEM_BlocktypeGeneric, 0, NULL, 0));

    /* Nominal (need to do each blocktype that has a different init) */
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    admin                 = BPLib_MEM_GetAdmin(&buf.pool);
    admin->num_bufs_total = 3;
    UtAssert_ADDRESS_EQ(BPLib_MEM_AllocBlockInternal(&buf.pool, BPLib_MEM_BlocktypeGeneric, 0, NULL, 0),
                        &buf.blk[0]);
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    UtAssert_ADDRESS_EQ(BPLib_MEM_AllocBlockInternal(&buf.pool, BPLib_MEM_BlocktypePrimary, 0, NULL, 0),
                        &buf.blk[0]);
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    UtAssert_ADDRESS_EQ(BPLib_MEM_AllocBlockInternal(&buf.pool, BPLib_MEM_BlocktypeCanonical, 0, NULL, 0),
                        &buf.blk[0]);
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    UtAssert_ADDRESS_EQ(BPLib_MEM_AllocBlockInternal(&buf.pool, BPLib_MEM_BlocktypeFlow, 0, NULL, 0),
                        &buf.blk[0]);

    /* free_blocks list empty (just repeat w/o resetting the state) */
    UtAssert_NULL(BPLib_MEM_AllocBlockInternal(&buf.pool, BPLib_MEM_BlocktypeGeneric, 0, NULL, 0));

    /* Too Big */
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    buf.blk[1].u.api.user_content_size = 16 + sizeof(BPLib_MEM_BlockBuffer_t);
    UtAssert_NULL(BPLib_MEM_AllocBlockInternal(&buf.pool, BPLib_MEM_BlocktypeGeneric, 0, NULL, 0));

    /* No API */
    test_setup_allocation(&buf.pool, &buf.blk[0], NULL);
    UtAssert_NULL(BPLib_MEM_AllocBlockInternal(&buf.pool, BPLib_MEM_BlocktypeGeneric, 0, NULL, 0));
}

void test_BPLib_MEM_GenericDataAlloc(void)
{
    /* Test function for:
     * BPLib_MEM_block_t *BPLib_MEM_GenericDataAlloc(BPLib_MEM_t *pool, uint32_t magic_number, void *init_arg)
     */

    UT_BPLib_MEM_buf_t buf;
    uint8_t              my_constructor_val;

    memset(&buf, 0, sizeof(buf));

    /* Nominal (this is just a wrapper around BPLib_MEM_AllocBlockInternal) */
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    buf.blk[1].u.api.api.construct = test_BPLib_MEM_CallbackStub;
    UtAssert_ADDRESS_EQ(BPLib_MEM_GenericDataAlloc(&buf.pool, 0, &my_constructor_val), &buf.blk[0]);

    /* Failure of constructor still returns the block */
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    UT_SetDefaultReturnValue(UT_KEY(test_BPLib_MEM_CallbackStub), BP_ERROR);
    buf.blk[1].u.api.api.construct = test_BPLib_MEM_CallbackStub;
    UtAssert_ADDRESS_EQ(BPLib_MEM_GenericDataAlloc(&buf.pool, 0, &my_constructor_val), &buf.blk[0]);
}

void test_BPLib_MEM_RecycleAllBlocksInList(void)
{
    /* Test function for:
     * void BPLib_MEM_RecycleAllBlocksInList(BPLib_MEM_t *pool, BPLib_MEM_block_t *list)
     */
    UT_BPLib_MEM_buf_t               buf;
    BPLib_MEM_BlockAdminContent_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    admin = BPLib_MEM_GetAdmin(&buf.pool);

    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypePrimary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypePrimary, 0);
    BPLib_MEM_InsertAfter(&admin->active_list, &buf.blk[0].header.base_link);
    BPLib_MEM_InsertAfter(&admin->active_list, &buf.blk[1].header.base_link);

    UtAssert_VOIDCALL(BPLib_MEM_RecycleAllBlocksInList(&buf.pool, &admin->active_list));

    UtAssert_BOOL_TRUE(BPLib_MEM_IsEmptyListHead(&admin->active_list));
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetNextBlock(&admin->recycle_blocks.block_list), &buf.blk[1].header.base_link);
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetPrevBlock(&admin->recycle_blocks.block_list), &buf.blk[0].header.base_link);

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypePrimary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeCanonical, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeCanonical, 0);
    BPLib_MEM_InsertAfter(&buf.blk[0].u.primary.pblock.cblock_list, &buf.blk[1].header.base_link);
    BPLib_MEM_InsertAfter(&buf.blk[0].u.primary.pblock.cblock_list, &buf.blk[2].header.base_link);
    UtAssert_VOIDCALL(BPLib_MEM_RecycleAllBlocksInList(NULL, &buf.blk[0].u.primary.pblock.cblock_list));
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetNextBlock(&admin->recycle_blocks.block_list), &buf.blk[2].header.base_link);
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetPrevBlock(&admin->recycle_blocks.block_list), &buf.blk[1].header.base_link);
}

void test_BPLib_MEM_RecycleBlock(void)
{
    /* Test function for:
     * void BPLib_MEM_RecycleBlock(BPLib_MEM_block_t *blk)
     */
    UT_BPLib_MEM_buf_t               buf;
    BPLib_MEM_BlockAdminContent_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    admin = BPLib_MEM_GetAdmin(&buf.pool);

    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypePrimary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypePrimary, 0);

    BPLib_MEM_InsertAfter(&admin->active_list, &buf.blk[0].header.base_link);
    BPLib_MEM_InsertAfter(&admin->active_list, &buf.blk[1].header.base_link);

    UtAssert_VOIDCALL(BPLib_MEM_RecycleBlock(&buf.blk[1].header.base_link));
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetNextBlock(&admin->recycle_blocks.block_list), &buf.blk[1].header.base_link);
    UtAssert_ADDRESS_EQ(BPLib_MEM_GetNextBlock(&admin->active_list), &buf.blk[0].header.base_link);
}

void test_BPLib_MEM_ListIterGotoFirst(void)
{
    /* Test function for:
     * int BPLib_MEM_ListIterGotoFirst(const BPLib_MEM_block_t *list, BPLib_MEM_ListIter_t *iter)
     */
    BPLib_MEM_block_t     list;
    BPLib_MEM_ListIter_t it;

    memset(&list, 0, sizeof(list));
    UtAssert_INT32_EQ(BPLib_MEM_ListIterGotoFirst(&list, &it), BP_ERROR);

    BPLib_MEM_InitListHead(NULL, &list);
    UtAssert_INT32_EQ(BPLib_MEM_ListIterGotoFirst(&list, &it), BP_ERROR);
}

void test_BPLib_MEM_ListIterGotoLast(void)
{
    /* Test function for:
     * int BPLib_MEM_ListIterGotoLast(const BPLib_MEM_block_t *list, BPLib_MEM_ListIter_t *iter)
     */

    BPLib_MEM_block_t     list;
    BPLib_MEM_ListIter_t it;

    memset(&list, 0, sizeof(list));
    UtAssert_INT32_EQ(BPLib_MEM_ListIterGotoLast(&list, &it), BP_ERROR);

    BPLib_MEM_InitListHead(NULL, &list);
    UtAssert_INT32_EQ(BPLib_MEM_ListIterGotoLast(&list, &it), BP_ERROR);
}

void test_BPLib_MEM_ListIterForward(void)
{
    /* Test function for:
     * int BPLib_MEM_ListIterForward(BPLib_MEM_ListIter_t *iter)
     */
    BPLib_MEM_ListIter_t     it;
    BPLib_MEM_block_t         list;
    BPLib_MEM_BlockContent_t my_block;

    memset(&it, 0, sizeof(it));
    UtAssert_INT32_EQ(BPLib_MEM_ListIterForward(&it), BP_ERROR);

    memset(&list, 0, sizeof(list));
    BPLib_MEM_InitListHead(NULL, &list);
    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypeGeneric, 0);
    BPLib_MEM_InsertAfter(&list, &my_block.header.base_link);

    it.pending_entry = &my_block.header.base_link;
    UtAssert_INT32_EQ(BPLib_MEM_ListIterForward(&it), BP_SUCCESS);

    UtAssert_ADDRESS_EQ(it.position, &my_block);
    UtAssert_ADDRESS_EQ(it.pending_entry, &list);

    UtAssert_INT32_EQ(BPLib_MEM_ListIterForward(&it), BP_ERROR);
}

void test_BPLib_MEM_ListIterReverse(void)
{
    /* Test function for:
     * int BPLib_MEM_ListIterReverse(BPLib_MEM_ListIter_t *iter)
     */
    BPLib_MEM_ListIter_t     it;
    BPLib_MEM_block_t         list;
    BPLib_MEM_BlockContent_t my_block;

    memset(&it, 0, sizeof(it));

    UtAssert_INT32_EQ(BPLib_MEM_ListIterReverse(&it), BP_ERROR);

    memset(&list, 0, sizeof(list));
    BPLib_MEM_InitListHead(NULL, &list);
    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypeGeneric, 0);
    BPLib_MEM_InsertAfter(&list, &my_block.header.base_link);

    it.pending_entry = &my_block.header.base_link;
    UtAssert_INT32_EQ(BPLib_MEM_ListIterReverse(&it), BP_SUCCESS);

    UtAssert_ADDRESS_EQ(it.position, &my_block);
    UtAssert_ADDRESS_EQ(it.pending_entry, &list);

    UtAssert_INT32_EQ(BPLib_MEM_ListIterReverse(&it), BP_ERROR);
}

void test_BPLib_MEM_ForeachItemInList(void)
{
    /* Test function for:
     * int BPLib_MEM_ForeachItemInList(BPLib_MEM_block_t *list, bool always_remove, BPLib_MEM_CallbackFunc_t
     * callback_fn, void *callback_arg)
     */

    BPLib_MEM_block_t         list;
    BPLib_MEM_BlockContent_t my_block;

    memset(&list, 0, sizeof(list));
    BPLib_MEM_InitListHead(NULL, &list);

    UtAssert_ZERO(BPLib_MEM_ForeachItemInList(&list, false, test_BPLib_MEM_CallbackStub, NULL));
    UtAssert_STUB_COUNT(test_BPLib_MEM_CallbackStub, 0);

    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypeGeneric, 0);
    BPLib_MEM_InsertAfter(&list, &my_block.header.base_link);

    UtAssert_INT32_EQ(BPLib_MEM_ForeachItemInList(&list, false, test_BPLib_MEM_CallbackStub, NULL), 1);
    UtAssert_STUB_COUNT(test_BPLib_MEM_CallbackStub, 1);

    UtAssert_INT32_EQ(BPLib_MEM_ForeachItemInList(&list, true, test_BPLib_MEM_CallbackStub, NULL), 1);
    UtAssert_STUB_COUNT(test_BPLib_MEM_CallbackStub, 2);
    UtAssert_BOOL_TRUE(BPLib_MEM_IsLinkUnattached(&my_block.header.base_link));
}

void test_BPLib_MEM_SearchList(void)
{
    /* Test function for:
     * BPLib_MEM_block_t *BPLib_MEM_SearchList(const BPLib_MEM_block_t *list, BPLib_MEM_CallbackFunc_t
     * match_fn, void *match_arg)
     */
    BPLib_MEM_block_t         list;
    BPLib_MEM_BlockContent_t my_block;

    memset(&list, 0, sizeof(list));
    BPLib_MEM_InitListHead(NULL, &list);

    UtAssert_NULL(BPLib_MEM_SearchList(&list, test_BPLib_MEM_CallbackStub, NULL));
    UtAssert_STUB_COUNT(test_BPLib_MEM_CallbackStub, 0);

    test_setup_mpblock(NULL, &my_block, BPLib_MEM_BlocktypeGeneric, 0);
    BPLib_MEM_InsertAfter(&list, &my_block.header.base_link);

    UtAssert_ADDRESS_EQ(BPLib_MEM_SearchList(&list, test_BPLib_MEM_CallbackStub, NULL),
                        &my_block.header.base_link);
    UtAssert_STUB_COUNT(test_BPLib_MEM_CallbackStub, 1);

    UT_SetDefaultReturnValue(UT_KEY(test_BPLib_MEM_CallbackStub), 1);
    UtAssert_NULL(BPLib_MEM_SearchList(&list, test_BPLib_MEM_CallbackStub, NULL));
    UtAssert_STUB_COUNT(test_BPLib_MEM_CallbackStub, 2);
}

void test_BPLib_MEM_RegisterBlocktype(void)
{
    /* Test function for:
     * int BPLib_MEM_RegisterBlocktype(BPLib_MEM_t *pool, uint32_t magic_number, const BPLib_MEM_BlocktypeApi_t
     * *api, size_t user_content_size)
     */
    UT_BPLib_MEM_buf_t buf;

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeApi, 0);
    UT_SetHandlerFunction(UT_KEY(bplib_rbt_search_generic), UT_AltHandler_PointerReturnForSignature, &buf.blk[1].u);

    UtAssert_INT32_EQ(
        BPLib_MEM_RegisterBlocktype(&buf.pool, UT_TESTBLOCKTYPE_SIG, &buf.blk[2].u.api.api, sizeof(uint32)),
        BP_SUCCESS);
    UtAssert_INT32_EQ(BPLib_MEM_RegisterBlocktype(&buf.pool, UT_TESTBLOCKTYPE_SIG, &buf.blk[2].u.api.api, 0),
                      BP_ERROR);

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeApi, 0);
    UT_SetHandlerFunction(UT_KEY(bplib_rbt_search_generic), UT_AltHandler_PointerReturnForSignature, &buf.blk[1].u);
    UT_SetDefaultReturnValue(UT_KEY(bplib_rbt_insert_value_generic), BP_DUPLICATE);
    UtAssert_INT32_EQ(BPLib_MEM_RegisterBlocktype(&buf.pool, UT_TESTBLOCKTYPE_SIG, NULL, 0), BP_DUPLICATE);

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeApi, 0);
    UtAssert_INT32_EQ(
        BPLib_MEM_RegisterBlocktype(&buf.pool, UT_TESTBLOCKTYPE_SIG, &buf.blk[2].u.api.api, sizeof(uint32)),
        BP_DUPLICATE);
}

void test_BPLib_MEM_CollectBlocks(void)
{
    /* Test function for:
     * uint32_t BPLib_MEM_CollectBlocks(BPLib_MEM_t *pool, uint32_t limit)
     */
    UT_BPLib_MEM_buf_t               buf;
    BPLib_MEM_BlockAdminContent_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeApi, 0);
    UT_SetHandlerFunction(UT_KEY(bplib_rbt_search_generic), UT_AltHandler_PointerReturn, &buf.blk[0].u);
    buf.blk[0].u.api.api.destruct = test_BPLib_MEM_CallbackStub;

    admin = BPLib_MEM_GetAdmin(&buf.pool);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypePrimary, 0);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, &buf.blk[1].header.base_link);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeCanonical, 0);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(BPLib_MEM_CollectBlocks(&buf.pool, 1), 1);
    UtAssert_STUB_COUNT(test_BPLib_MEM_CallbackStub, 1);
    UtAssert_UINT32_EQ(BPLib_MEM_CollectBlocks(&buf.pool, 1), 1);
    UtAssert_STUB_COUNT(test_BPLib_MEM_CallbackStub, 2);

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    UT_SetHandlerFunction(UT_KEY(bplib_rbt_search_generic), UT_AltHandler_PointerReturn, NULL);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeGeneric, 0);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, &buf.blk[0].header.base_link);
    test_setup_mpblock(&buf.pool, &buf.blk[1], BPLib_MEM_BlocktypeFlow, 0);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, &buf.blk[1].header.base_link);
    test_setup_mpblock(&buf.pool, &buf.blk[2], BPLib_MEM_BlocktypeRef, 0);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(BPLib_MEM_CollectBlocks(&buf.pool, 10), 3);
}

void test_BPLib_MEM_maintain(void)
{
    /* Test function for:
     * void BPLib_MEM_maintain(BPLib_MEM_t *pool)
     */
    UT_BPLib_MEM_buf_t               buf;
    BPLib_MEM_BlockAdminContent_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);
    UtAssert_VOIDCALL(BPLib_MEM_maintain(&buf.pool));

    admin = BPLib_MEM_GetAdmin(&buf.pool);

    UT_SetHandlerFunction(UT_KEY(bplib_rbt_search_generic), UT_AltHandler_PointerReturn, NULL);
    test_setup_mpblock(&buf.pool, &buf.blk[0], BPLib_MEM_BlocktypeGeneric, 0);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, &buf.blk[0].header.base_link);

    UtAssert_VOIDCALL(BPLib_MEM_maintain(&buf.pool));
}

void test_BPLib_MEM_QueryMemCurrentUse(void)
{
    /* Test function for:
     * size_t BPLib_MEM_QueryMemCurrentUse(BPLib_MEM_t *pool)
     */

    BPLib_MEM_t pool;

    memset(&pool, 0, sizeof(pool));

    test_setup_mpblock(&pool, &pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);

    UtAssert_UINT32_EQ(BPLib_MEM_QueryMemCurrentUse(&pool), 0);
}

void test_BPLib_MEM_QueryMemMaxUse(void)
{
    /* Test function for:
     * size_t BPLib_MEM_QueryMemMaxUse(BPLib_MEM_t *pool)
     */
    BPLib_MEM_t pool;

    memset(&pool, 0, sizeof(pool));

    test_setup_mpblock(&pool, &pool.admin_block, BPLib_MEM_BlocktypeAdmin, 0);

    UtAssert_UINT32_EQ(BPLib_MEM_QueryMemMaxUse(&pool), 0);
}

void test_BPLib_MEM_create(void)
{
    /* Test function for:
     * BPLib_MEM_t *BPLib_MEM_create(void *pool_mem, size_t pool_size)
     */
    UT_BPLib_MEM_buf_t buf;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(BPLib_MEM_create(NULL, sizeof(buf)));
    UtAssert_NULL(BPLib_MEM_create(&buf, 1));
    UtAssert_ADDRESS_EQ(BPLib_MEM_create(&buf, sizeof(buf)), &buf);
}

void test_BPLib_MEM_DebugScan(void)
{
    /* Test function for:
     * void BPLib_MEM_DebugScan(BPLib_MEM_t *pool)
     *
     * Note this is not really part of the real code, it is for debugging only,
     * and thus may not need a coverage test.  But for now, call it for completeness.
     */
    UT_BPLib_MEM_buf_t buf;
    BPLib_MEM_t       *pool;

    memset(&buf, 0, sizeof(buf));
    UtAssert_NOT_NULL(pool = BPLib_MEM_create(&buf, sizeof(buf)));

    buf.blk[0].header.base_link.type = BPLib_MEM_BlocktypePrimary;
    buf.blk[1].header.base_link.type = BPLib_MEM_BlocktypeCanonical;
    buf.blk[2].header.base_link.type = BPLib_MEM_BlocktypeGeneric;

    UtAssert_VOIDCALL(BPLib_MEM_DebugScan(pool));

    buf.blk[0].header.base_link.type = BPLib_MEM_BlocktypeRef;
    buf.blk[1].header.base_link.type = BPLib_MEM_BlocktypeFlow;
    buf.blk[2].header.base_link.type = BPLib_MEM_BlocktypeMax;

    UtAssert_VOIDCALL(BPLib_MEM_DebugScan(pool));
}

void TestBplibMpoolBase_Register(void)
{
    UtTest_Add(test_BPLib_MEM_LockInit, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_LockInit");
    UtTest_Add(test_BPLib_MEM_LockPrepare, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_LockPrepare");
    UtTest_Add(test_BPLib_MEM_LockResource, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_LockResource");
    UtTest_Add(test_BPLib_MEM_LockWait, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_LockWait");
    UtTest_Add(test_BPLib_MEM_BlockFromExternalId, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_BlockFromExternalId");
    UtTest_Add(test_BPLib_MEM_GetBlockFromLink, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_GetBlockFromLink");
    UtTest_Add(test_BPLib_MEM_GetBlockContent, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_GetBlockContent");
    UtTest_Add(test_BPLib_MEM_BlockDereferenceContent, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_BlockDereferenceContent");
    UtTest_Add(test_BPLib_MEM_GetUserDataOffsetByBlocktype, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_GetUserDataOffsetByBlocktype");
    UtTest_Add(test_BPLib_MEM_GetGenericDataCapacity, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_GetGenericDataCapacity");
    UtTest_Add(test_BPLib_MEM_InitSecondaryLink, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_InitSecondaryLink");
    UtTest_Add(test_BPLib_MEM_InitListHead, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_InitListHead");
    UtTest_Add(test_BPLib_MEM_InsertAfter, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_InsertAfter");
    UtTest_Add(test_BPLib_MEM_InsertBefore, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_InsertBefore");
    UtTest_Add(test_BPLib_MEM_ExtractNode, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_ExtractNode");
    UtTest_Add(test_BPLib_MEM_MergeList, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_MergeList");
    UtTest_Add(test_BPLib_MEM_GetUserContentSize, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_GetUserContentSize");
    UtTest_Add(test_BPLib_MEM_ReadRefcount, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_ReadRefcount");
    UtTest_Add(test_BPLib_MEM_GetParentPoolFromLink, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_GetParentPoolFromLink");
    UtTest_Add(test_BPLib_MEM_GenericDataCast, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_GenericData_[un]cast");
    UtTest_Add(test_BPLib_MEM_InitBaseObject, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_InitBaseObject");
    UtTest_Add(test_BPLib_MEM_AllocBlockInternal, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_AllocBlockInternal");
    UtTest_Add(test_BPLib_MEM_GenericDataAlloc, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_GenericDataAlloc");
    UtTest_Add(test_BPLib_MEM_RecycleAllBlocksInList, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_RecycleAllBlocksInList");
    UtTest_Add(test_BPLib_MEM_RecycleBlock, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_RecycleBlock");
    UtTest_Add(test_BPLib_MEM_ListIterGotoFirst, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_ListIterGotoFirst");
    UtTest_Add(test_BPLib_MEM_ListIterGotoLast, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_ListIterGotoLast");
    UtTest_Add(test_BPLib_MEM_ListIterForward, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_ListIterForward");
    UtTest_Add(test_BPLib_MEM_ListIterReverse, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_ListIterReverse");
    UtTest_Add(test_BPLib_MEM_ForeachItemInList, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_ForeachItemInList");
    UtTest_Add(test_BPLib_MEM_SearchList, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_SearchList");
    UtTest_Add(test_BPLib_MEM_RegisterBlocktype, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_RegisterBlocktype");
    UtTest_Add(test_BPLib_MEM_CollectBlocks, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_CollectBlocks");
    UtTest_Add(test_BPLib_MEM_maintain, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_maintain");
    UtTest_Add(test_BPLib_MEM_QueryMemCurrentUse, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_QueryMemCurrentUse");
    UtTest_Add(test_BPLib_MEM_QueryMemMaxUse, TestBplibMpool_ResetTestEnvironment, NULL,
               "BPLib_MEM_QueryMemMaxUse");
    UtTest_Add(test_BPLib_MEM_create, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_create");
    UtTest_Add(test_BPLib_MEM_DebugScan, TestBplibMpool_ResetTestEnvironment, NULL, "BPLib_MEM_DebugScan");
}
