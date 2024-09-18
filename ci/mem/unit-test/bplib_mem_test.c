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

#include "test_bplib_mpool.h"
#include "bplib_mem.h"
#include "bplib_os.h"

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

void test_bplib_mpool_lock_init(void)
{
    /* Test function for:
     * void bplib_mpool_lock_init(void)
     */

    UT_SetDefaultReturnValue(UT_KEY(bplib_os_createlock), 1);

    /* Call it twice, first time should init, second time should skip init */
    UtAssert_VOIDCALL(bplib_mpool_lock_init());
    UtAssert_VOIDCALL(bplib_mpool_lock_init());
}

void test_bplib_mpool_lock_prepare(void)
{
    /* Test function for:
     * bplib_mpool_lock_t *bplib_mpool_lock_prepare(void *resource_addr)
     */

    UtAssert_NOT_NULL(bplib_mpool_lock_prepare(NULL));
}

void test_bplib_mpool_lock_resource(void)
{
    /* Test function for:
     * bplib_mpool_lock_t *bplib_mpool_lock_resource(void *resource_addr)
     */

    UtAssert_NOT_NULL(bplib_mpool_lock_resource(NULL));
}

void test_bplib_mpool_lock_wait(void)
{
    /* Test function for:
     * bool bplib_mpool_lock_wait(bplib_mpool_lock_t *lock, uint64_t until_dtntime)
     */

    bplib_mpool_lock_t *lock = bplib_mpool_lock_prepare(NULL);

    UT_SetDefaultReturnValue(UT_KEY(bplib_os_get_dtntime_ms), 1000);
    UtAssert_BOOL_FALSE(bplib_mpool_lock_wait(lock, 0));
    UtAssert_BOOL_TRUE(bplib_mpool_lock_wait(lock, 5000));
    UT_SetDefaultReturnValue(UT_KEY(bplib_os_wait_until_ms), BP_TIMEOUT);
    UtAssert_BOOL_FALSE(bplib_mpool_lock_wait(lock, 5000));
}

void test_bplib_mpool_block_from_external_id(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_block_from_external_id(bplib_mpool_t *pool, bp_handle_t handle)
     */
    UT_bplib_mpool_buf_t buf;
    bp_handle_t          id1;
    bp_handle_t          id2;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(bplib_mpool_block_from_external_id(&buf.pool, BP_INVALID_HANDLE));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_primary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_primary, 0);

    UtAssert_BOOL_TRUE(bp_handle_is_valid(id1 = bplib_mpool_get_external_id(&buf.blk[0].header.base_link)));
    UtAssert_BOOL_TRUE(bp_handle_is_valid(id2 = bplib_mpool_get_external_id(&buf.blk[1].header.base_link)));
    UtAssert_NULL(bplib_mpool_block_from_external_id(&buf.pool, id1));
    UtAssert_NULL(bplib_mpool_block_from_external_id(&buf.pool, id2));

    buf.pool.admin_block.u.admin.num_bufs_total = 3;
    UtAssert_ADDRESS_EQ(bplib_mpool_block_from_external_id(&buf.pool, id1), &buf.blk[0].header.base_link);
    UtAssert_ADDRESS_EQ(bplib_mpool_block_from_external_id(&buf.pool, id2), &buf.blk[1].header.base_link);
}

void test_bplib_mpool_get_block_from_link(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_get_block_from_link(bplib_mpool_block_t *lblk)
     */
    struct
    {
        bplib_mpool_block_t base_blk;
        uint8_t             unused1[3];
        bplib_mpool_block_t sublink_blk1;
        uint8_t             unused2[7];
        bplib_mpool_block_t sublink_blk2;
    } test_block;

    memset(&test_block, 0, sizeof(test_block));

    UtAssert_NULL(bplib_mpool_get_block_from_link(NULL));
    UtAssert_ADDRESS_EQ(bplib_mpool_get_block_from_link(&test_block.base_blk), &test_block.base_blk);

    test_block.base_blk.type = bplib_mpool_blocktype_undefined;
    bplib_mpool_init_list_head(&test_block.base_blk, &test_block.sublink_blk1);
    bplib_mpool_init_list_head(&test_block.base_blk, &test_block.sublink_blk2);

    UtAssert_ADDRESS_EQ(bplib_mpool_get_block_from_link(&test_block.sublink_blk1), &test_block.base_blk);
    UtAssert_ADDRESS_EQ(bplib_mpool_get_block_from_link(&test_block.sublink_blk2), &test_block.base_blk);

    test_block.sublink_blk2.parent_offset = 0; /* Invalid, but forces check path */
    UtAssert_ADDRESS_EQ(bplib_mpool_get_block_from_link(&test_block.sublink_blk2), &test_block.sublink_blk2);

    test_block.base_blk.type = bplib_mpool_blocktype_admin;
    UtAssert_ADDRESS_EQ(bplib_mpool_get_block_from_link(&test_block.base_blk), &test_block.base_blk);
}

void test_bplib_mpool_get_block_content(void)
{
    /* Test function for:
     * bplib_mpool_block_content_t *bplib_mpool_get_block_content(bplib_mpool_block_t *cb)
     * const bplib_mpool_block_content_t *bplib_mpool_get_block_content_const(const bplib_mpool_block_t *cb)
     *
     * NOTE: these two functions are identical except for the const-ness of input and output pointers.
     */
    bplib_mpool_block_content_t my_block;

    memset(&my_block, 0, sizeof(my_block));

    UtAssert_NULL(bplib_mpool_get_block_content(NULL));
    UtAssert_NULL(bplib_mpool_get_block_content_const(NULL));
    UtAssert_NULL(bplib_mpool_get_block_content(&my_block.header.base_link));
    UtAssert_NULL(bplib_mpool_get_block_content_const(&my_block.header.base_link));

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_primary, 0);
    UtAssert_ADDRESS_EQ(bplib_mpool_get_block_content(&my_block.header.base_link), &my_block);
    UtAssert_ADDRESS_EQ(bplib_mpool_get_block_content_const(&my_block.header.base_link), &my_block);
}

void test_bplib_mpool_block_dereference_content(void)
{
    /* Test function for:
     * bplib_mpool_block_content_t *bplib_mpool_block_dereference_content(bplib_mpool_block_t *cb)
     */

    bplib_mpool_block_content_t my_block;
    bplib_mpool_block_content_t my_ref;

    UtAssert_NULL(bplib_mpool_block_dereference_content(NULL));

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_primary, 0);
    UtAssert_ADDRESS_EQ(bplib_mpool_block_dereference_content(&my_block.header.base_link), &my_block);

    test_setup_mpblock(NULL, &my_ref, bplib_mpool_blocktype_ref, 0);
    my_ref.u.ref.pref_target = &my_block;
    UtAssert_ADDRESS_EQ(bplib_mpool_block_dereference_content(&my_ref.header.base_link), &my_block);
}

void test_bplib_mpool_get_user_data_offset_by_blocktype(void)
{
    /* Test function for:
     * size_t bplib_mpool_get_user_data_offset_by_blocktype(bplib_mpool_blocktype_t bt)
     */

    UtAssert_UINT32_EQ(bplib_mpool_get_user_data_offset_by_blocktype(bplib_mpool_blocktype_undefined), SIZE_MAX);
    UtAssert_UINT32_EQ(bplib_mpool_get_user_data_offset_by_blocktype(bplib_mpool_blocktype_max), SIZE_MAX);
    UtAssert_UINT32_EQ(bplib_mpool_get_user_data_offset_by_blocktype(bplib_mpool_blocktype_generic), 0);
}

void test_bplib_mpool_get_generic_data_capacity(void)
{
    /* Test function for:
     * size_t bplib_mpool_get_generic_data_capacity(const bplib_mpool_block_t *cb)
     */
    bplib_mpool_block_content_t my_block;

    memset(&my_block, 0, sizeof(my_block));

    UtAssert_ZERO(bplib_mpool_get_generic_data_capacity(&my_block.header.base_link));

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_generic, 0);
    UtAssert_UINT32_EQ(bplib_mpool_get_generic_data_capacity(&my_block.header.base_link),
                       sizeof(bplib_mpool_block_buffer_t));
}

void test_bplib_mpool_init_secondary_link(void)
{
    /* Test function for:
     * void bplib_mpool_init_secondary_link(bplib_mpool_block_t *base_block, bplib_mpool_block_t *secondary_link,
     * bplib_mpool_blocktype_t block_type)
     */
    bplib_mpool_block_content_t my_block;

    memset(&my_block, 0, sizeof(my_block));

    UtAssert_VOIDCALL(
        bplib_mpool_init_secondary_link(NULL, &my_block.header.base_link, bplib_mpool_blocktype_secondary_generic));
    UtAssert_VOIDCALL(bplib_mpool_init_secondary_link(
        &my_block.header.base_link, &my_block.u.primary.pblock.cblock_list, bplib_mpool_blocktype_list_head));
}

void test_bplib_mpool_init_list_head(void)
{
    /* Test function for:
     * void bplib_mpool_init_list_head(bplib_mpool_block_t *base_block, bplib_mpool_block_t *list_head)
     */
    bplib_mpool_block_content_t my_block;

    memset(&my_block, 0, sizeof(my_block));

    UtAssert_VOIDCALL(bplib_mpool_init_list_head(&my_block.header.base_link, &my_block.u.primary.pblock.cblock_list));
}

void test_bplib_mpool_insert_after(void)
{
    /* Test function for:
     * void bplib_mpool_insert_after(bplib_mpool_block_t *list, bplib_mpool_block_t *node)
     */
    struct
    {
        bplib_mpool_block_t base_blk;
        bplib_mpool_block_t head_blk;
    } list;
    bplib_mpool_block_t content[2];

    memset(&list, 0, sizeof(list));
    memset(&content, 0, sizeof(content));
    test_make_singleton_link(NULL, &content[0]);
    test_make_singleton_link(NULL, &content[1]);

    UtAssert_VOIDCALL(bplib_mpool_init_list_head(&list.base_blk, &list.head_blk));

    UtAssert_VOIDCALL(bplib_mpool_insert_after(&list.head_blk, &content[0]));
    UtAssert_VOIDCALL(bplib_mpool_insert_after(&list.head_blk, &content[1]));

    UtAssert_ADDRESS_EQ(list.head_blk.prev, &content[0]);
    UtAssert_ADDRESS_EQ(list.head_blk.next, &content[1]);
    UtAssert_ADDRESS_EQ(content[1].prev, &list.head_blk);
    UtAssert_ADDRESS_EQ(content[1].next, &content[0]);
    UtAssert_ADDRESS_EQ(content[0].prev, &content[1]);
    UtAssert_ADDRESS_EQ(content[0].next, &list.head_blk);
}

void test_bplib_mpool_insert_before(void)
{
    /* Test function for:
     * void bplib_mpool_insert_before(bplib_mpool_block_t *list, bplib_mpool_block_t *node)
     */

    struct
    {
        bplib_mpool_block_t base_blk;
        bplib_mpool_block_t head_blk;
    } list;
    bplib_mpool_block_t content[2];

    memset(&list, 0, sizeof(list));
    memset(&content, 0, sizeof(content));
    test_make_singleton_link(NULL, &content[0]);
    test_make_singleton_link(NULL, &content[1]);

    UtAssert_VOIDCALL(bplib_mpool_init_list_head(&list.base_blk, &list.head_blk));

    UtAssert_VOIDCALL(bplib_mpool_insert_before(&list.head_blk, &content[0]));
    UtAssert_VOIDCALL(bplib_mpool_insert_before(&list.head_blk, &content[1]));

    UtAssert_ADDRESS_EQ(list.head_blk.prev, &content[1]);
    UtAssert_ADDRESS_EQ(list.head_blk.next, &content[0]);
    UtAssert_ADDRESS_EQ(content[0].prev, &list.head_blk);
    UtAssert_ADDRESS_EQ(content[0].next, &content[1]);
    UtAssert_ADDRESS_EQ(content[1].prev, &content[0]);
    UtAssert_ADDRESS_EQ(content[1].next, &list.head_blk);
}

void test_bplib_mpool_extract_node(void)
{
    /* Test function for:
     * void bplib_mpool_extract_node(bplib_mpool_block_t *node)
     */

    struct
    {
        bplib_mpool_block_t base_blk;
        bplib_mpool_block_t head_blk;
    } list;
    bplib_mpool_block_t content[2];

    memset(&list, 0, sizeof(list));
    memset(&content, 0, sizeof(content));
    test_make_singleton_link(NULL, &content[0]);
    test_make_singleton_link(NULL, &content[1]);

    UtAssert_VOIDCALL(bplib_mpool_init_list_head(&list.base_blk, &list.head_blk));

    UtAssert_VOIDCALL(bplib_mpool_insert_before(&list.head_blk, &content[0]));
    UtAssert_VOIDCALL(bplib_mpool_insert_before(&list.head_blk, &content[1]));
    UtAssert_VOIDCALL(bplib_mpool_extract_node(&content[0]));

    UtAssert_BOOL_TRUE(bplib_mpool_is_link_attached(&content[1]));
    UtAssert_BOOL_TRUE(bplib_mpool_is_link_unattached(&content[0]));

    UtAssert_VOIDCALL(bplib_mpool_extract_node(&content[1]));
    UtAssert_BOOL_TRUE(bplib_mpool_is_empty_list_head(&list.head_blk));
    UtAssert_BOOL_TRUE(bplib_mpool_is_link_unattached(&content[1]));
    UtAssert_BOOL_TRUE(bplib_mpool_is_link_unattached(&content[0]));
}

void test_bplib_mpool_merge_list(void)
{
    /* Test function for:
     * void bplib_mpool_merge_list(bplib_mpool_block_t *dest, bplib_mpool_block_t *src)
     */
    struct
    {
        bplib_mpool_block_t base_blk;
        bplib_mpool_block_t head_blk;
    } list[2];
    bplib_mpool_block_t  content[2];
    bplib_mpool_block_t *blk;

    memset(&list, 0, sizeof(list));
    memset(&content, 0, sizeof(content));
    test_make_singleton_link(NULL, &content[0]);
    test_make_singleton_link(NULL, &content[1]);

    UtAssert_VOIDCALL(bplib_mpool_init_list_head(&list[0].base_blk, &list[0].head_blk));
    UtAssert_VOIDCALL(bplib_mpool_init_list_head(&list[1].base_blk, &list[1].head_blk));
    UtAssert_VOIDCALL(bplib_mpool_insert_before(&list[0].head_blk, &content[0]));
    UtAssert_VOIDCALL(bplib_mpool_insert_before(&list[1].head_blk, &content[1]));

    UtAssert_VOIDCALL(bplib_mpool_merge_list(&list[1].head_blk, &list[0].head_blk));

    /* Note "merge" puts everything into one list, including both heads */
    /* follow the links forward and backward */
    blk = &list[1].head_blk;

    UtAssert_ADDRESS_EQ(blk = bplib_mpool_get_next_block(blk), &content[1]);
    UtAssert_ADDRESS_EQ(blk = bplib_mpool_get_next_block(blk), &list[0].head_blk);
    UtAssert_ADDRESS_EQ(blk = bplib_mpool_get_next_block(blk), &content[0]);
    UtAssert_ADDRESS_EQ(blk = bplib_mpool_get_next_block(blk), &list[1].head_blk);

    UtAssert_ADDRESS_EQ(blk = bplib_mpool_get_prev_block(blk), &content[0]);
    UtAssert_ADDRESS_EQ(blk = bplib_mpool_get_prev_block(blk), &list[0].head_blk);
    UtAssert_ADDRESS_EQ(blk = bplib_mpool_get_prev_block(blk), &content[1]);
    UtAssert_ADDRESS_EQ(blk = bplib_mpool_get_prev_block(blk), &list[1].head_blk);
}

void test_bplib_mpool_get_user_content_size(void)
{
    /* Test function for:
     * size_t bplib_mpool_get_user_content_size(const bplib_mpool_block_t *cb)
     */
    bplib_mpool_block_content_t my_block;

    UtAssert_ZERO(bplib_mpool_get_user_content_size(NULL));

    memset(&my_block, 0, sizeof(my_block));
    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_generic, 0);
    my_block.header.user_content_length = 14;

    UtAssert_UINT32_EQ(bplib_mpool_get_user_content_size(&my_block.header.base_link), 14);
}

void test_bplib_mpool_read_refcount(void)
{
    /* Test function for:
     * size_t bplib_mpool_read_refcount(const bplib_mpool_block_t *cb)
     */
    bplib_mpool_block_content_t my_block;

    UtAssert_ZERO(bplib_mpool_read_refcount(NULL));

    memset(&my_block, 0, sizeof(my_block));
    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_generic, 0);
    my_block.header.refcount = 6;

    UtAssert_UINT32_EQ(bplib_mpool_read_refcount(&my_block.header.base_link), 6);
}

void test_bplib_mpool_get_parent_pool_from_link(void)
{
    /* Test function for:
     * bplib_mpool_t *bplib_mpool_get_parent_pool_from_link(bplib_mpool_block_t *cb)
     */

    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(bplib_mpool_get_parent_pool_from_link(NULL));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_primary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_primary, 0);

    UtAssert_ADDRESS_EQ(bplib_mpool_get_parent_pool_from_link(&buf.blk[0].header.base_link), &buf.pool);
    UtAssert_ADDRESS_EQ(bplib_mpool_get_parent_pool_from_link(&buf.blk[0].u.primary.pblock.cblock_list), &buf.pool);
}

void test_bplib_mpool_generic_data_cast(void)
{
    /* Test function for:
     * void *bplib_mpool_generic_data_cast(bplib_mpool_block_t *cb, uint32_t required_magic)
     * bplib_mpool_block_t *bplib_mpool_generic_data_uncast(void *blk, bplib_mpool_blocktype_t parent_bt, uint32_t
     * required_magic)
     *
     * These two functions are inverse ops so they are paired together
     */
    static const uint32         UT_SIG = 0x4953ab26;
    bplib_mpool_block_content_t my_block;
    bplib_mpool_block_content_t my_ref;

    UtAssert_NULL(bplib_mpool_generic_data_cast(NULL, 0));

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_admin, 0);
    UtAssert_NULL(bplib_mpool_generic_data_cast(&my_block.header.base_link, 0));
    UtAssert_NULL(bplib_mpool_generic_data_uncast(&my_block.u, bplib_mpool_blocktype_admin, 0));

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_generic, UT_SIG);

    UtAssert_ADDRESS_EQ(bplib_mpool_generic_data_cast(&my_block.header.base_link, UT_SIG), &my_block.u);
    UtAssert_NULL(bplib_mpool_generic_data_cast(&my_block.header.base_link, ~UT_SIG));

    UtAssert_ADDRESS_EQ(bplib_mpool_generic_data_uncast(&my_block.u, bplib_mpool_blocktype_generic, UT_SIG), &my_block);
    UtAssert_NULL(bplib_mpool_generic_data_uncast(&my_block.u, bplib_mpool_blocktype_generic, ~UT_SIG));
    UtAssert_NULL(bplib_mpool_generic_data_uncast(&my_block.u, bplib_mpool_blocktype_generic, ~UT_SIG));
    UtAssert_NULL(bplib_mpool_generic_data_uncast(&my_block.u, bplib_mpool_blocktype_flow, UT_SIG));

    test_setup_mpblock(NULL, &my_ref, bplib_mpool_blocktype_ref, 0);
    my_ref.u.ref.pref_target = &my_block;
    UtAssert_ADDRESS_EQ(bplib_mpool_generic_data_cast(&my_ref.header.base_link, UT_SIG), &my_block.u);
}

void test_bplib_mpool_init_base_object(void)
{
    /* Test function for:
     * void bplib_mpool_init_base_object(bplib_mpool_block_header_t *block_hdr, uint16_t user_content_length, uint32_t
     * content_type_signature)
     */

    static const uint32         UT_SIG = 0x2c9f27d0;
    static const uint16         UT_LEN = sizeof(bplib_mpool_generic_data_content_t);
    bplib_mpool_block_content_t my_block;

    memset(&my_block, 0, sizeof(my_block));

    UtAssert_VOIDCALL(bplib_mpool_init_base_object(&my_block.header, UT_LEN, UT_SIG));

    UtAssert_UINT32_EQ(my_block.header.content_type_signature, UT_SIG);
    UtAssert_UINT16_EQ(my_block.header.user_content_length, UT_LEN);
}

void test_bplib_mpool_alloc_block_internal(void)
{
    /* Test function for:
     * bplib_mpool_block_content_t *bplib_mpool_alloc_block_internal(bplib_mpool_t *pool, bplib_mpool_blocktype_t
     * blocktype, uint32_t content_type_signature, void *init_arg, uint8_t priority)
     */

    UT_bplib_mpool_buf_t               buf;
    bplib_mpool_block_admin_content_t *admin;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(bplib_mpool_alloc_block_internal(&buf.pool, bplib_mpool_blocktype_undefined, 0, NULL, 0));
    UtAssert_NULL(bplib_mpool_alloc_block_internal(&buf.pool, bplib_mpool_blocktype_max, 0, NULL, 0));
    UtAssert_NULL(bplib_mpool_alloc_block_internal(&buf.pool, bplib_mpool_blocktype_generic, 0, NULL, 0));

    /* Nominal (need to do each blocktype that has a different init) */
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    admin                 = bplib_mpool_get_admin(&buf.pool);
    admin->num_bufs_total = 3;
    UtAssert_ADDRESS_EQ(bplib_mpool_alloc_block_internal(&buf.pool, bplib_mpool_blocktype_generic, 0, NULL, 0),
                        &buf.blk[0]);
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    UtAssert_ADDRESS_EQ(bplib_mpool_alloc_block_internal(&buf.pool, bplib_mpool_blocktype_primary, 0, NULL, 0),
                        &buf.blk[0]);
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    UtAssert_ADDRESS_EQ(bplib_mpool_alloc_block_internal(&buf.pool, bplib_mpool_blocktype_canonical, 0, NULL, 0),
                        &buf.blk[0]);
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    UtAssert_ADDRESS_EQ(bplib_mpool_alloc_block_internal(&buf.pool, bplib_mpool_blocktype_flow, 0, NULL, 0),
                        &buf.blk[0]);

    /* free_blocks list empty (just repeat w/o resetting the state) */
    UtAssert_NULL(bplib_mpool_alloc_block_internal(&buf.pool, bplib_mpool_blocktype_generic, 0, NULL, 0));

    /* Too Big */
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    buf.blk[1].u.api.user_content_size = 16 + sizeof(bplib_mpool_block_buffer_t);
    UtAssert_NULL(bplib_mpool_alloc_block_internal(&buf.pool, bplib_mpool_blocktype_generic, 0, NULL, 0));

    /* No API */
    test_setup_allocation(&buf.pool, &buf.blk[0], NULL);
    UtAssert_NULL(bplib_mpool_alloc_block_internal(&buf.pool, bplib_mpool_blocktype_generic, 0, NULL, 0));
}

void test_bplib_mpool_generic_data_alloc(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_generic_data_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg)
     */

    UT_bplib_mpool_buf_t buf;
    uint8_t              my_constructor_val;

    memset(&buf, 0, sizeof(buf));

    /* Nominal (this is just a wrapper around bplib_mpool_alloc_block_internal) */
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    buf.blk[1].u.api.api.construct = test_bplib_mpool_callback_stub;
    UtAssert_ADDRESS_EQ(bplib_mpool_generic_data_alloc(&buf.pool, 0, &my_constructor_val), &buf.blk[0]);

    /* Failure of constructor still returns the block */
    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    UT_SetDefaultReturnValue(UT_KEY(test_bplib_mpool_callback_stub), BP_ERROR);
    buf.blk[1].u.api.api.construct = test_bplib_mpool_callback_stub;
    UtAssert_ADDRESS_EQ(bplib_mpool_generic_data_alloc(&buf.pool, 0, &my_constructor_val), &buf.blk[0]);
}

void test_bplib_mpool_recycle_all_blocks_in_list(void)
{
    #ifdef STOR // cache
    /* Test function for:
     * void bplib_mpool_recycle_all_blocks_in_list(bplib_mpool_t *pool, bplib_mpool_block_t *list)
     */
    UT_bplib_mpool_buf_t               buf;
    bplib_mpool_block_admin_content_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    admin = bplib_mpool_get_admin(&buf.pool);

    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_primary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_primary, 0);
    bplib_mpool_insert_after(&admin->active_list, &buf.blk[0].header.base_link);
    bplib_mpool_insert_after(&admin->active_list, &buf.blk[1].header.base_link);

    UtAssert_VOIDCALL(bplib_mpool_recycle_all_blocks_in_list(&buf.pool, &admin->active_list));

    UtAssert_BOOL_TRUE(bplib_mpool_is_empty_list_head(&admin->active_list));
    UtAssert_ADDRESS_EQ(bplib_mpool_get_next_block(&admin->recycle_blocks.block_list), &buf.blk[1].header.base_link);
    UtAssert_ADDRESS_EQ(bplib_mpool_get_prev_block(&admin->recycle_blocks.block_list), &buf.blk[0].header.base_link);

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_primary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_canonical, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_canonical, 0);
    bplib_mpool_insert_after(&buf.blk[0].u.primary.pblock.cblock_list, &buf.blk[1].header.base_link);
    bplib_mpool_insert_after(&buf.blk[0].u.primary.pblock.cblock_list, &buf.blk[2].header.base_link);
    UtAssert_VOIDCALL(bplib_mpool_recycle_all_blocks_in_list(NULL, &buf.blk[0].u.primary.pblock.cblock_list));
    UtAssert_ADDRESS_EQ(bplib_mpool_get_next_block(&admin->recycle_blocks.block_list), &buf.blk[2].header.base_link);
    UtAssert_ADDRESS_EQ(bplib_mpool_get_prev_block(&admin->recycle_blocks.block_list), &buf.blk[1].header.base_link);
    #endif // STOR cache
}

void test_bplib_mpool_recycle_block(void)
{
    #ifdef STOR // cache
    /* Test function for:
     * void bplib_mpool_recycle_block(bplib_mpool_block_t *blk)
     */
    UT_bplib_mpool_buf_t               buf;
    bplib_mpool_block_admin_content_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    admin = bplib_mpool_get_admin(&buf.pool);

    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_primary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_primary, 0);

    bplib_mpool_insert_after(&admin->active_list, &buf.blk[0].header.base_link);
    bplib_mpool_insert_after(&admin->active_list, &buf.blk[1].header.base_link);

    UtAssert_VOIDCALL(bplib_mpool_recycle_block(&buf.blk[1].header.base_link));
    UtAssert_ADDRESS_EQ(bplib_mpool_get_next_block(&admin->recycle_blocks.block_list), &buf.blk[1].header.base_link);
    UtAssert_ADDRESS_EQ(bplib_mpool_get_next_block(&admin->active_list), &buf.blk[0].header.base_link);
    #endif // STOR cache
}

void test_bplib_mpool_list_iter_goto_first(void)
{
    /* Test function for:
     * int bplib_mpool_list_iter_goto_first(const bplib_mpool_block_t *list, bplib_mpool_list_iter_t *iter)
     */
    bplib_mpool_block_t     list;
    bplib_mpool_list_iter_t it;

    memset(&list, 0, sizeof(list));
    UtAssert_INT32_EQ(bplib_mpool_list_iter_goto_first(&list, &it), BP_ERROR);

    bplib_mpool_init_list_head(NULL, &list);
    UtAssert_INT32_EQ(bplib_mpool_list_iter_goto_first(&list, &it), BP_ERROR);
}

void test_bplib_mpool_list_iter_goto_last(void)
{
    /* Test function for:
     * int bplib_mpool_list_iter_goto_last(const bplib_mpool_block_t *list, bplib_mpool_list_iter_t *iter)
     */

    bplib_mpool_block_t     list;
    bplib_mpool_list_iter_t it;

    memset(&list, 0, sizeof(list));
    UtAssert_INT32_EQ(bplib_mpool_list_iter_goto_last(&list, &it), BP_ERROR);

    bplib_mpool_init_list_head(NULL, &list);
    UtAssert_INT32_EQ(bplib_mpool_list_iter_goto_last(&list, &it), BP_ERROR);
}

void test_bplib_mpool_list_iter_forward(void)
{
    /* Test function for:
     * int bplib_mpool_list_iter_forward(bplib_mpool_list_iter_t *iter)
     */
    bplib_mpool_list_iter_t     it;
    bplib_mpool_block_t         list;
    bplib_mpool_block_content_t my_block;

    memset(&it, 0, sizeof(it));
    UtAssert_INT32_EQ(bplib_mpool_list_iter_forward(&it), BP_ERROR);

    memset(&list, 0, sizeof(list));
    bplib_mpool_init_list_head(NULL, &list);
    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_generic, 0);
    bplib_mpool_insert_after(&list, &my_block.header.base_link);

    it.pending_entry = &my_block.header.base_link;
    UtAssert_INT32_EQ(bplib_mpool_list_iter_forward(&it), BP_SUCCESS);

    UtAssert_ADDRESS_EQ(it.position, &my_block);
    UtAssert_ADDRESS_EQ(it.pending_entry, &list);

    UtAssert_INT32_EQ(bplib_mpool_list_iter_forward(&it), BP_ERROR);
}

void test_bplib_mpool_list_iter_reverse(void)
{
    /* Test function for:
     * int bplib_mpool_list_iter_reverse(bplib_mpool_list_iter_t *iter)
     */
    bplib_mpool_list_iter_t     it;
    bplib_mpool_block_t         list;
    bplib_mpool_block_content_t my_block;

    memset(&it, 0, sizeof(it));

    UtAssert_INT32_EQ(bplib_mpool_list_iter_reverse(&it), BP_ERROR);

    memset(&list, 0, sizeof(list));
    bplib_mpool_init_list_head(NULL, &list);
    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_generic, 0);
    bplib_mpool_insert_after(&list, &my_block.header.base_link);

    it.pending_entry = &my_block.header.base_link;
    UtAssert_INT32_EQ(bplib_mpool_list_iter_reverse(&it), BP_SUCCESS);

    UtAssert_ADDRESS_EQ(it.position, &my_block);
    UtAssert_ADDRESS_EQ(it.pending_entry, &list);

    UtAssert_INT32_EQ(bplib_mpool_list_iter_reverse(&it), BP_ERROR);
}

void test_bplib_mpool_foreach_item_in_list(void)
{
    /* Test function for:
     * int bplib_mpool_foreach_item_in_list(bplib_mpool_block_t *list, bool always_remove, bplib_mpool_callback_func_t
     * callback_fn, void *callback_arg)
     */

    bplib_mpool_block_t         list;
    bplib_mpool_block_content_t my_block;

    memset(&list, 0, sizeof(list));
    bplib_mpool_init_list_head(NULL, &list);

    UtAssert_ZERO(bplib_mpool_foreach_item_in_list(&list, false, test_bplib_mpool_callback_stub, NULL));
    UtAssert_STUB_COUNT(test_bplib_mpool_callback_stub, 0);

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_generic, 0);
    bplib_mpool_insert_after(&list, &my_block.header.base_link);

    UtAssert_INT32_EQ(bplib_mpool_foreach_item_in_list(&list, false, test_bplib_mpool_callback_stub, NULL), 1);
    UtAssert_STUB_COUNT(test_bplib_mpool_callback_stub, 1);

    UtAssert_INT32_EQ(bplib_mpool_foreach_item_in_list(&list, true, test_bplib_mpool_callback_stub, NULL), 1);
    UtAssert_STUB_COUNT(test_bplib_mpool_callback_stub, 2);
    UtAssert_BOOL_TRUE(bplib_mpool_is_link_unattached(&my_block.header.base_link));
}

void test_bplib_mpool_search_list(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_search_list(const bplib_mpool_block_t *list, bplib_mpool_callback_func_t
     * match_fn, void *match_arg)
     */
    bplib_mpool_block_t         list;
    bplib_mpool_block_content_t my_block;

    memset(&list, 0, sizeof(list));
    bplib_mpool_init_list_head(NULL, &list);

    UtAssert_NULL(bplib_mpool_search_list(&list, test_bplib_mpool_callback_stub, NULL));
    UtAssert_STUB_COUNT(test_bplib_mpool_callback_stub, 0);

    test_setup_mpblock(NULL, &my_block, bplib_mpool_blocktype_generic, 0);
    bplib_mpool_insert_after(&list, &my_block.header.base_link);

    UtAssert_ADDRESS_EQ(bplib_mpool_search_list(&list, test_bplib_mpool_callback_stub, NULL),
                        &my_block.header.base_link);
    UtAssert_STUB_COUNT(test_bplib_mpool_callback_stub, 1);

    UT_SetDefaultReturnValue(UT_KEY(test_bplib_mpool_callback_stub), 1);
    UtAssert_NULL(bplib_mpool_search_list(&list, test_bplib_mpool_callback_stub, NULL));
    UtAssert_STUB_COUNT(test_bplib_mpool_callback_stub, 2);
}

void test_bplib_mpool_register_blocktype(void)
{
    /* Test function for:
     * int bplib_mpool_register_blocktype(bplib_mpool_t *pool, uint32_t magic_number, const bplib_mpool_blocktype_api_t
     * *api, size_t user_content_size)
     */
    UT_bplib_mpool_buf_t buf;

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_api, 0);
    UT_SetHandlerFunction(UT_KEY(bplib_rbt_search_generic), UT_AltHandler_PointerReturnForSignature, &buf.blk[1].u);

    UtAssert_INT32_EQ(
        bplib_mpool_register_blocktype(&buf.pool, UT_TESTBLOCKTYPE_SIG, &buf.blk[2].u.api.api, sizeof(uint32)),
        BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_mpool_register_blocktype(&buf.pool, UT_TESTBLOCKTYPE_SIG, &buf.blk[2].u.api.api, 0),
                      BP_ERROR);

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_api, 0);
    UT_SetHandlerFunction(UT_KEY(bplib_rbt_search_generic), UT_AltHandler_PointerReturnForSignature, &buf.blk[1].u);
    UT_SetDefaultReturnValue(UT_KEY(bplib_rbt_insert_value_generic), BP_DUPLICATE);
    UtAssert_INT32_EQ(bplib_mpool_register_blocktype(&buf.pool, UT_TESTBLOCKTYPE_SIG, NULL, 0), BP_DUPLICATE);

    test_setup_allocation(&buf.pool, &buf.blk[0], &buf.blk[1]);
    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_api, 0);
    UtAssert_INT32_EQ(
        bplib_mpool_register_blocktype(&buf.pool, UT_TESTBLOCKTYPE_SIG, &buf.blk[2].u.api.api, sizeof(uint32)),
        BP_DUPLICATE);
}

void test_bplib_mpool_collect_blocks(void)
{
    /* Test function for:
     * uint32_t bplib_mpool_collect_blocks(bplib_mpool_t *pool, uint32_t limit)
     */
    UT_bplib_mpool_buf_t               buf;
    bplib_mpool_block_admin_content_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_api, 0);
    UT_SetHandlerFunction(UT_KEY(bplib_rbt_search_generic), UT_AltHandler_PointerReturn, &buf.blk[0].u);
    buf.blk[0].u.api.api.destruct = test_bplib_mpool_callback_stub;

    admin = bplib_mpool_get_admin(&buf.pool);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_primary, 0);
    bplib_mpool_subq_push_single(&admin->recycle_blocks, &buf.blk[1].header.base_link);
    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_canonical, 0);
    bplib_mpool_subq_push_single(&admin->recycle_blocks, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(bplib_mpool_collect_blocks(&buf.pool, 1), 1);
    UtAssert_STUB_COUNT(test_bplib_mpool_callback_stub, 1);
    UtAssert_UINT32_EQ(bplib_mpool_collect_blocks(&buf.pool, 1), 1);
    UtAssert_STUB_COUNT(test_bplib_mpool_callback_stub, 2);

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    UT_SetHandlerFunction(UT_KEY(bplib_rbt_search_generic), UT_AltHandler_PointerReturn, NULL);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_generic, 0);
    bplib_mpool_subq_push_single(&admin->recycle_blocks, &buf.blk[0].header.base_link);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_flow, 0);
    bplib_mpool_subq_push_single(&admin->recycle_blocks, &buf.blk[1].header.base_link);
    test_setup_mpblock(&buf.pool, &buf.blk[2], bplib_mpool_blocktype_ref, 0);
    bplib_mpool_subq_push_single(&admin->recycle_blocks, &buf.blk[2].header.base_link);

    UtAssert_UINT32_EQ(bplib_mpool_collect_blocks(&buf.pool, 10), 3);
}

void test_bplib_mpool_maintain(void)
{
    /* Test function for:
     * void bplib_mpool_maintain(bplib_mpool_t *pool)
     */
    UT_bplib_mpool_buf_t               buf;
    bplib_mpool_block_admin_content_t *admin;

    memset(&buf, 0, sizeof(buf));

    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    UtAssert_VOIDCALL(bplib_mpool_maintain(&buf.pool));

    admin = bplib_mpool_get_admin(&buf.pool);

    UT_SetHandlerFunction(UT_KEY(bplib_rbt_search_generic), UT_AltHandler_PointerReturn, NULL);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_generic, 0);
    bplib_mpool_subq_push_single(&admin->recycle_blocks, &buf.blk[0].header.base_link);

    UtAssert_VOIDCALL(bplib_mpool_maintain(&buf.pool));
}

void test_bplib_mpool_query_mem_current_use(void)
{
    /* Test function for:
     * size_t bplib_mpool_query_mem_current_use(bplib_mpool_t *pool)
     */

    bplib_mpool_t pool;

    memset(&pool, 0, sizeof(pool));

    test_setup_mpblock(&pool, &pool.admin_block, bplib_mpool_blocktype_admin, 0);

    UtAssert_UINT32_EQ(bplib_mpool_query_mem_current_use(&pool), 0);
}

void test_bplib_mpool_query_mem_max_use(void)
{
    /* Test function for:
     * size_t bplib_mpool_query_mem_max_use(bplib_mpool_t *pool)
     */
    bplib_mpool_t pool;

    memset(&pool, 0, sizeof(pool));

    test_setup_mpblock(&pool, &pool.admin_block, bplib_mpool_blocktype_admin, 0);

    UtAssert_UINT32_EQ(bplib_mpool_query_mem_max_use(&pool), 0);
    UtAssert_INT32_EQ(BPLib_MEM_Init(), BPLIB_SUCCESS);
}

void test_bplib_mpool_create(void)
{
    /* Test function for:
     * bplib_mpool_t *bplib_mpool_create(void *pool_mem, size_t pool_size)
     */
    UT_bplib_mpool_buf_t buf;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(bplib_mpool_create(NULL, sizeof(buf)));
    UtAssert_NULL(bplib_mpool_create(&buf, 1));
    UtAssert_ADDRESS_EQ(bplib_mpool_create(&buf, sizeof(buf)), &buf);
}

void test_bplib_mpool_debug_scan(void)
{
    /* Test function for:
     * void bplib_mpool_debug_scan(bplib_mpool_t *pool)
     *
     * Note this is not really part of the real code, it is for debugging only,
     * and thus may not need a coverage test.  But for now, call it for completeness.
     */
    UT_bplib_mpool_buf_t buf;
    bplib_mpool_t       *pool;

    memset(&buf, 0, sizeof(buf));
    UtAssert_NOT_NULL(pool = bplib_mpool_create(&buf, sizeof(buf)));

    buf.blk[0].header.base_link.type = bplib_mpool_blocktype_primary;
    buf.blk[1].header.base_link.type = bplib_mpool_blocktype_canonical;
    buf.blk[2].header.base_link.type = bplib_mpool_blocktype_generic;

    UtAssert_VOIDCALL(bplib_mpool_debug_scan(pool));

    buf.blk[0].header.base_link.type = bplib_mpool_blocktype_ref;
    buf.blk[1].header.base_link.type = bplib_mpool_blocktype_flow;
    buf.blk[2].header.base_link.type = bplib_mpool_blocktype_max;

    UtAssert_VOIDCALL(bplib_mpool_debug_scan(pool));
}

void TestBplibMpoolBase_Register(void)
{
    UtTest_Add(test_bplib_mpool_lock_init, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_lock_init");
    UtTest_Add(test_bplib_mpool_lock_prepare, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_lock_prepare");
    UtTest_Add(test_bplib_mpool_lock_resource, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_lock_resource");
    UtTest_Add(test_bplib_mpool_lock_wait, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_lock_wait");
    UtTest_Add(test_bplib_mpool_block_from_external_id, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_block_from_external_id");
    UtTest_Add(test_bplib_mpool_get_block_from_link, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_get_block_from_link");
    UtTest_Add(test_bplib_mpool_get_block_content, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_get_block_content");
    UtTest_Add(test_bplib_mpool_block_dereference_content, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_block_dereference_content");
    UtTest_Add(test_bplib_mpool_get_user_data_offset_by_blocktype, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_get_user_data_offset_by_blocktype");
    UtTest_Add(test_bplib_mpool_get_generic_data_capacity, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_get_generic_data_capacity");
    UtTest_Add(test_bplib_mpool_init_secondary_link, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_init_secondary_link");
    UtTest_Add(test_bplib_mpool_init_list_head, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_init_list_head");
    UtTest_Add(test_bplib_mpool_insert_after, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_insert_after");
    UtTest_Add(test_bplib_mpool_insert_before, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_insert_before");
    UtTest_Add(test_bplib_mpool_extract_node, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_extract_node");
    UtTest_Add(test_bplib_mpool_merge_list, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_merge_list");
    UtTest_Add(test_bplib_mpool_get_user_content_size, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_get_user_content_size");
    UtTest_Add(test_bplib_mpool_read_refcount, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_read_refcount");
    UtTest_Add(test_bplib_mpool_get_parent_pool_from_link, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_get_parent_pool_from_link");
    UtTest_Add(test_bplib_mpool_generic_data_cast, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_generic_data_[un]cast");
    UtTest_Add(test_bplib_mpool_init_base_object, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_init_base_object");
    UtTest_Add(test_bplib_mpool_alloc_block_internal, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_alloc_block_internal");
    UtTest_Add(test_bplib_mpool_generic_data_alloc, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_generic_data_alloc");
    UtTest_Add(test_bplib_mpool_recycle_all_blocks_in_list, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_recycle_all_blocks_in_list");
    UtTest_Add(test_bplib_mpool_recycle_block, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_recycle_block");
    UtTest_Add(test_bplib_mpool_list_iter_goto_first, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_list_iter_goto_first");
    UtTest_Add(test_bplib_mpool_list_iter_goto_last, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_list_iter_goto_last");
    UtTest_Add(test_bplib_mpool_list_iter_forward, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_list_iter_forward");
    UtTest_Add(test_bplib_mpool_list_iter_reverse, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_list_iter_reverse");
    UtTest_Add(test_bplib_mpool_foreach_item_in_list, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_foreach_item_in_list");
    UtTest_Add(test_bplib_mpool_search_list, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_search_list");
    UtTest_Add(test_bplib_mpool_register_blocktype, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_register_blocktype");
    UtTest_Add(test_bplib_mpool_collect_blocks, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_collect_blocks");
    UtTest_Add(test_bplib_mpool_maintain, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_maintain");
    UtTest_Add(test_bplib_mpool_query_mem_current_use, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_query_mem_current_use");
    UtTest_Add(test_bplib_mpool_query_mem_max_use, TestBplibMpool_ResetTestEnvironment, NULL,
               "bplib_mpool_query_mem_max_use");
    UtTest_Add(test_bplib_mpool_create, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_create");
    UtTest_Add(test_bplib_mpool_debug_scan, TestBplibMpool_ResetTestEnvironment, NULL, "bplib_mpool_debug_scan");
}
