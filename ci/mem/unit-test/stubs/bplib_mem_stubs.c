/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in v7_mpool header
 */

#include "v7_mpool.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_block_from_external_id()
 * ----------------------------------------------------
 */
bplib_mpool_block_t *bplib_mpool_block_from_external_id(bplib_mpool_t *pool, bp_handle_t handle)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_block_from_external_id, bplib_mpool_block_t *);

    UT_GenStub_AddParam(bplib_mpool_block_from_external_id, bplib_mpool_t *, pool);
    UT_GenStub_AddParam(bplib_mpool_block_from_external_id, bp_handle_t, handle);

    UT_GenStub_Execute(bplib_mpool_block_from_external_id, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_block_from_external_id, bplib_mpool_block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_create()
 * ----------------------------------------------------
 */
bplib_mpool_t *bplib_mpool_create(void *pool_mem, size_t pool_size)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_create, bplib_mpool_t *);

    UT_GenStub_AddParam(bplib_mpool_create, void *, pool_mem);
    UT_GenStub_AddParam(bplib_mpool_create, size_t, pool_size);

    UT_GenStub_Execute(bplib_mpool_create, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_create, bplib_mpool_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_debug_print_list_stats()
 * ----------------------------------------------------
 */
void bplib_mpool_debug_print_list_stats(bplib_mpool_block_t *list, const char *label)
{
    UT_GenStub_AddParam(bplib_mpool_debug_print_list_stats, bplib_mpool_block_t *, list);
    UT_GenStub_AddParam(bplib_mpool_debug_print_list_stats, const char *, label);

    UT_GenStub_Execute(bplib_mpool_debug_print_list_stats, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_debug_scan()
 * ----------------------------------------------------
 */
void bplib_mpool_debug_scan(bplib_mpool_t *pool)
{
    UT_GenStub_AddParam(bplib_mpool_debug_scan, bplib_mpool_t *, pool);

    UT_GenStub_Execute(bplib_mpool_debug_scan, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_extract_node()
 * ----------------------------------------------------
 */
void bplib_mpool_extract_node(bplib_mpool_block_t *node)
{
    UT_GenStub_AddParam(bplib_mpool_extract_node, bplib_mpool_block_t *, node);

    UT_GenStub_Execute(bplib_mpool_extract_node, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_foreach_item_in_list()
 * ----------------------------------------------------
 */
int bplib_mpool_foreach_item_in_list(bplib_mpool_block_t *list, bool always_remove,
                                     bplib_mpool_callback_func_t callback_fn, void *callback_arg)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_foreach_item_in_list, int);

    UT_GenStub_AddParam(bplib_mpool_foreach_item_in_list, bplib_mpool_block_t *, list);
    UT_GenStub_AddParam(bplib_mpool_foreach_item_in_list, bool, always_remove);
    UT_GenStub_AddParam(bplib_mpool_foreach_item_in_list, bplib_mpool_callback_func_t, callback_fn);
    UT_GenStub_AddParam(bplib_mpool_foreach_item_in_list, void *, callback_arg);

    UT_GenStub_Execute(bplib_mpool_foreach_item_in_list, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_foreach_item_in_list, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_generic_data_alloc()
 * ----------------------------------------------------
 */
bplib_mpool_block_t *bplib_mpool_generic_data_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_generic_data_alloc, bplib_mpool_block_t *);

    UT_GenStub_AddParam(bplib_mpool_generic_data_alloc, bplib_mpool_t *, pool);
    UT_GenStub_AddParam(bplib_mpool_generic_data_alloc, uint32_t, magic_number);
    UT_GenStub_AddParam(bplib_mpool_generic_data_alloc, void *, init_arg);

    UT_GenStub_Execute(bplib_mpool_generic_data_alloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_generic_data_alloc, bplib_mpool_block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_generic_data_cast()
 * ----------------------------------------------------
 */
void *bplib_mpool_generic_data_cast(bplib_mpool_block_t *cb, uint32_t required_magic)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_generic_data_cast, void *);

    UT_GenStub_AddParam(bplib_mpool_generic_data_cast, bplib_mpool_block_t *, cb);
    UT_GenStub_AddParam(bplib_mpool_generic_data_cast, uint32_t, required_magic);

    UT_GenStub_Execute(bplib_mpool_generic_data_cast, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_generic_data_cast, void *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_generic_data_uncast()
 * ----------------------------------------------------
 */
bplib_mpool_block_t *bplib_mpool_generic_data_uncast(void *blk, bplib_mpool_blocktype_t parent_bt,
                                                     uint32_t required_magic)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_generic_data_uncast, bplib_mpool_block_t *);

    UT_GenStub_AddParam(bplib_mpool_generic_data_uncast, void *, blk);
    UT_GenStub_AddParam(bplib_mpool_generic_data_uncast, bplib_mpool_blocktype_t, parent_bt);
    UT_GenStub_AddParam(bplib_mpool_generic_data_uncast, uint32_t, required_magic);

    UT_GenStub_Execute(bplib_mpool_generic_data_uncast, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_generic_data_uncast, bplib_mpool_block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_get_block_from_link()
 * ----------------------------------------------------
 */
bplib_mpool_block_t *bplib_mpool_get_block_from_link(bplib_mpool_block_t *lblk)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_get_block_from_link, bplib_mpool_block_t *);

    UT_GenStub_AddParam(bplib_mpool_get_block_from_link, bplib_mpool_block_t *, lblk);

    UT_GenStub_Execute(bplib_mpool_get_block_from_link, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_get_block_from_link, bplib_mpool_block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_get_generic_data_capacity()
 * ----------------------------------------------------
 */
size_t bplib_mpool_get_generic_data_capacity(const bplib_mpool_block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_get_generic_data_capacity, size_t);

    UT_GenStub_AddParam(bplib_mpool_get_generic_data_capacity, const bplib_mpool_block_t *, cb);

    UT_GenStub_Execute(bplib_mpool_get_generic_data_capacity, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_get_generic_data_capacity, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_get_parent_pool_from_link()
 * ----------------------------------------------------
 */
bplib_mpool_t *bplib_mpool_get_parent_pool_from_link(bplib_mpool_block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_get_parent_pool_from_link, bplib_mpool_t *);

    UT_GenStub_AddParam(bplib_mpool_get_parent_pool_from_link, bplib_mpool_block_t *, cb);

    UT_GenStub_Execute(bplib_mpool_get_parent_pool_from_link, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_get_parent_pool_from_link, bplib_mpool_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_get_user_content_size()
 * ----------------------------------------------------
 */
size_t bplib_mpool_get_user_content_size(const bplib_mpool_block_t *ceb)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_get_user_content_size, size_t);

    UT_GenStub_AddParam(bplib_mpool_get_user_content_size, const bplib_mpool_block_t *, ceb);

    UT_GenStub_Execute(bplib_mpool_get_user_content_size, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_get_user_content_size, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_init_list_head()
 * ----------------------------------------------------
 */
void bplib_mpool_init_list_head(bplib_mpool_block_t *base_block, bplib_mpool_block_t *list_head)
{
    UT_GenStub_AddParam(bplib_mpool_init_list_head, bplib_mpool_block_t *, base_block);
    UT_GenStub_AddParam(bplib_mpool_init_list_head, bplib_mpool_block_t *, list_head);

    UT_GenStub_Execute(bplib_mpool_init_list_head, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_init_secondary_link()
 * ----------------------------------------------------
 */
void bplib_mpool_init_secondary_link(bplib_mpool_block_t *base_block, bplib_mpool_block_t *secondary_link,
                                     bplib_mpool_blocktype_t block_type)
{
    UT_GenStub_AddParam(bplib_mpool_init_secondary_link, bplib_mpool_block_t *, base_block);
    UT_GenStub_AddParam(bplib_mpool_init_secondary_link, bplib_mpool_block_t *, secondary_link);
    UT_GenStub_AddParam(bplib_mpool_init_secondary_link, bplib_mpool_blocktype_t, block_type);

    UT_GenStub_Execute(bplib_mpool_init_secondary_link, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_insert_after()
 * ----------------------------------------------------
 */
void bplib_mpool_insert_after(bplib_mpool_block_t *list, bplib_mpool_block_t *node)
{
    UT_GenStub_AddParam(bplib_mpool_insert_after, bplib_mpool_block_t *, list);
    UT_GenStub_AddParam(bplib_mpool_insert_after, bplib_mpool_block_t *, node);

    UT_GenStub_Execute(bplib_mpool_insert_after, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_insert_before()
 * ----------------------------------------------------
 */
void bplib_mpool_insert_before(bplib_mpool_block_t *list, bplib_mpool_block_t *node)
{
    UT_GenStub_AddParam(bplib_mpool_insert_before, bplib_mpool_block_t *, list);
    UT_GenStub_AddParam(bplib_mpool_insert_before, bplib_mpool_block_t *, node);

    UT_GenStub_Execute(bplib_mpool_insert_before, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_list_iter_forward()
 * ----------------------------------------------------
 */
int bplib_mpool_list_iter_forward(bplib_mpool_list_iter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_list_iter_forward, int);

    UT_GenStub_AddParam(bplib_mpool_list_iter_forward, bplib_mpool_list_iter_t *, iter);

    UT_GenStub_Execute(bplib_mpool_list_iter_forward, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_list_iter_forward, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_list_iter_goto_first()
 * ----------------------------------------------------
 */
int bplib_mpool_list_iter_goto_first(const bplib_mpool_block_t *list, bplib_mpool_list_iter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_list_iter_goto_first, int);

    UT_GenStub_AddParam(bplib_mpool_list_iter_goto_first, const bplib_mpool_block_t *, list);
    UT_GenStub_AddParam(bplib_mpool_list_iter_goto_first, bplib_mpool_list_iter_t *, iter);

    UT_GenStub_Execute(bplib_mpool_list_iter_goto_first, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_list_iter_goto_first, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_list_iter_goto_last()
 * ----------------------------------------------------
 */
int bplib_mpool_list_iter_goto_last(const bplib_mpool_block_t *list, bplib_mpool_list_iter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_list_iter_goto_last, int);

    UT_GenStub_AddParam(bplib_mpool_list_iter_goto_last, const bplib_mpool_block_t *, list);
    UT_GenStub_AddParam(bplib_mpool_list_iter_goto_last, bplib_mpool_list_iter_t *, iter);

    UT_GenStub_Execute(bplib_mpool_list_iter_goto_last, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_list_iter_goto_last, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_list_iter_reverse()
 * ----------------------------------------------------
 */
int bplib_mpool_list_iter_reverse(bplib_mpool_list_iter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_list_iter_reverse, int);

    UT_GenStub_AddParam(bplib_mpool_list_iter_reverse, bplib_mpool_list_iter_t *, iter);

    UT_GenStub_Execute(bplib_mpool_list_iter_reverse, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_list_iter_reverse, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_maintain()
 * ----------------------------------------------------
 */
void bplib_mpool_maintain(bplib_mpool_t *pool)
{
    UT_GenStub_AddParam(bplib_mpool_maintain, bplib_mpool_t *, pool);

    UT_GenStub_Execute(bplib_mpool_maintain, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_merge_list()
 * ----------------------------------------------------
 */
void bplib_mpool_merge_list(bplib_mpool_block_t *dest, bplib_mpool_block_t *src)
{
    UT_GenStub_AddParam(bplib_mpool_merge_list, bplib_mpool_block_t *, dest);
    UT_GenStub_AddParam(bplib_mpool_merge_list, bplib_mpool_block_t *, src);

    UT_GenStub_Execute(bplib_mpool_merge_list, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_query_mem_current_use()
 * ----------------------------------------------------
 */
size_t bplib_mpool_query_mem_current_use(bplib_mpool_t *pool)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_query_mem_current_use, size_t);

    UT_GenStub_AddParam(bplib_mpool_query_mem_current_use, bplib_mpool_t *, pool);

    UT_GenStub_Execute(bplib_mpool_query_mem_current_use, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_query_mem_current_use, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_query_mem_max_use()
 * ----------------------------------------------------
 */
size_t bplib_mpool_query_mem_max_use(bplib_mpool_t *pool)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_query_mem_max_use, size_t);

    UT_GenStub_AddParam(bplib_mpool_query_mem_max_use, bplib_mpool_t *, pool);

    UT_GenStub_Execute(bplib_mpool_query_mem_max_use, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_query_mem_max_use, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_read_refcount()
 * ----------------------------------------------------
 */
size_t bplib_mpool_read_refcount(const bplib_mpool_block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_read_refcount, size_t);

    UT_GenStub_AddParam(bplib_mpool_read_refcount, const bplib_mpool_block_t *, cb);

    UT_GenStub_Execute(bplib_mpool_read_refcount, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_read_refcount, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_recycle_all_blocks_in_list()
 * ----------------------------------------------------
 */
void bplib_mpool_recycle_all_blocks_in_list(bplib_mpool_t *pool, bplib_mpool_block_t *list)
{
    UT_GenStub_AddParam(bplib_mpool_recycle_all_blocks_in_list, bplib_mpool_t *, pool);
    UT_GenStub_AddParam(bplib_mpool_recycle_all_blocks_in_list, bplib_mpool_block_t *, list);

    UT_GenStub_Execute(bplib_mpool_recycle_all_blocks_in_list, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_recycle_block()
 * ----------------------------------------------------
 */
void bplib_mpool_recycle_block(bplib_mpool_block_t *blk)
{
    UT_GenStub_AddParam(bplib_mpool_recycle_block, bplib_mpool_block_t *, blk);

    UT_GenStub_Execute(bplib_mpool_recycle_block, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_register_blocktype()
 * ----------------------------------------------------
 */
int bplib_mpool_register_blocktype(bplib_mpool_t *pool, uint32_t magic_number, const bplib_mpool_blocktype_api_t *api,
                                   size_t user_content_size)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_register_blocktype, int);

    UT_GenStub_AddParam(bplib_mpool_register_blocktype, bplib_mpool_t *, pool);
    UT_GenStub_AddParam(bplib_mpool_register_blocktype, uint32_t, magic_number);
    UT_GenStub_AddParam(bplib_mpool_register_blocktype, const bplib_mpool_blocktype_api_t *, api);
    UT_GenStub_AddParam(bplib_mpool_register_blocktype, size_t, user_content_size);

    UT_GenStub_Execute(bplib_mpool_register_blocktype, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_register_blocktype, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_search_list()
 * ----------------------------------------------------
 */
bplib_mpool_block_t *bplib_mpool_search_list(const bplib_mpool_block_t *list, bplib_mpool_callback_func_t match_fn,
                                             void *match_arg)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_search_list, bplib_mpool_block_t *);

    UT_GenStub_AddParam(bplib_mpool_search_list, const bplib_mpool_block_t *, list);
    UT_GenStub_AddParam(bplib_mpool_search_list, bplib_mpool_callback_func_t, match_fn);
    UT_GenStub_AddParam(bplib_mpool_search_list, void *, match_arg);

    UT_GenStub_Execute(bplib_mpool_search_list, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_search_list, bplib_mpool_block_t *);
}
