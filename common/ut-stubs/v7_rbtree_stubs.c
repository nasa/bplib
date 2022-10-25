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
 * Auto-Generated stub implementations for functions defined in v7_rbtree header
 */

#include "v7_rbtree.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_rbt_extract_node()
 * ----------------------------------------------------
 */
int bplib_rbt_extract_node(bplib_rbt_root_t *tree, bplib_rbt_link_t *link_block)
{
    UT_GenStub_SetupReturnBuffer(bplib_rbt_extract_node, int);

    UT_GenStub_AddParam(bplib_rbt_extract_node, bplib_rbt_root_t *, tree);
    UT_GenStub_AddParam(bplib_rbt_extract_node, bplib_rbt_link_t *, link_block);

    UT_GenStub_Execute(bplib_rbt_extract_node, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_rbt_extract_node, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_rbt_get_key_value()
 * ----------------------------------------------------
 */
bp_val_t bplib_rbt_get_key_value(const bplib_rbt_link_t *node)
{
    UT_GenStub_SetupReturnBuffer(bplib_rbt_get_key_value, bp_val_t);

    UT_GenStub_AddParam(bplib_rbt_get_key_value, const bplib_rbt_link_t *, node);

    UT_GenStub_Execute(bplib_rbt_get_key_value, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_rbt_get_key_value, bp_val_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_rbt_init_root()
 * ----------------------------------------------------
 */
void bplib_rbt_init_root(bplib_rbt_root_t *tree)
{
    UT_GenStub_AddParam(bplib_rbt_init_root, bplib_rbt_root_t *, tree);

    UT_GenStub_Execute(bplib_rbt_init_root, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_rbt_insert_value_generic()
 * ----------------------------------------------------
 */
int bplib_rbt_insert_value_generic(bp_val_t insert_key_value, bplib_rbt_root_t *tree, bplib_rbt_link_t *link_block,
                                   bplib_rbt_compare_func_t compare_func, void *compare_arg)
{
    UT_GenStub_SetupReturnBuffer(bplib_rbt_insert_value_generic, int);

    UT_GenStub_AddParam(bplib_rbt_insert_value_generic, bp_val_t, insert_key_value);
    UT_GenStub_AddParam(bplib_rbt_insert_value_generic, bplib_rbt_root_t *, tree);
    UT_GenStub_AddParam(bplib_rbt_insert_value_generic, bplib_rbt_link_t *, link_block);
    UT_GenStub_AddParam(bplib_rbt_insert_value_generic, bplib_rbt_compare_func_t, compare_func);
    UT_GenStub_AddParam(bplib_rbt_insert_value_generic, void *, compare_arg);

    UT_GenStub_Execute(bplib_rbt_insert_value_generic, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_rbt_insert_value_generic, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_rbt_iter_goto_max()
 * ----------------------------------------------------
 */
int bplib_rbt_iter_goto_max(bp_val_t maximum_value, const bplib_rbt_root_t *tree, bplib_rbt_iter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(bplib_rbt_iter_goto_max, int);

    UT_GenStub_AddParam(bplib_rbt_iter_goto_max, bp_val_t, maximum_value);
    UT_GenStub_AddParam(bplib_rbt_iter_goto_max, const bplib_rbt_root_t *, tree);
    UT_GenStub_AddParam(bplib_rbt_iter_goto_max, bplib_rbt_iter_t *, iter);

    UT_GenStub_Execute(bplib_rbt_iter_goto_max, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_rbt_iter_goto_max, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_rbt_iter_goto_min()
 * ----------------------------------------------------
 */
int bplib_rbt_iter_goto_min(bp_val_t minimum_value, const bplib_rbt_root_t *tree, bplib_rbt_iter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(bplib_rbt_iter_goto_min, int);

    UT_GenStub_AddParam(bplib_rbt_iter_goto_min, bp_val_t, minimum_value);
    UT_GenStub_AddParam(bplib_rbt_iter_goto_min, const bplib_rbt_root_t *, tree);
    UT_GenStub_AddParam(bplib_rbt_iter_goto_min, bplib_rbt_iter_t *, iter);

    UT_GenStub_Execute(bplib_rbt_iter_goto_min, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_rbt_iter_goto_min, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_rbt_iter_next()
 * ----------------------------------------------------
 */
int bplib_rbt_iter_next(bplib_rbt_iter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(bplib_rbt_iter_next, int);

    UT_GenStub_AddParam(bplib_rbt_iter_next, bplib_rbt_iter_t *, iter);

    UT_GenStub_Execute(bplib_rbt_iter_next, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_rbt_iter_next, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_rbt_iter_prev()
 * ----------------------------------------------------
 */
int bplib_rbt_iter_prev(bplib_rbt_iter_t *iter)
{
    UT_GenStub_SetupReturnBuffer(bplib_rbt_iter_prev, int);

    UT_GenStub_AddParam(bplib_rbt_iter_prev, bplib_rbt_iter_t *, iter);

    UT_GenStub_Execute(bplib_rbt_iter_prev, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_rbt_iter_prev, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_rbt_node_is_member()
 * ----------------------------------------------------
 */
bool bplib_rbt_node_is_member(const bplib_rbt_root_t *tree, const bplib_rbt_link_t *node)
{
    UT_GenStub_SetupReturnBuffer(bplib_rbt_node_is_member, bool);

    UT_GenStub_AddParam(bplib_rbt_node_is_member, const bplib_rbt_root_t *, tree);
    UT_GenStub_AddParam(bplib_rbt_node_is_member, const bplib_rbt_link_t *, node);

    UT_GenStub_Execute(bplib_rbt_node_is_member, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_rbt_node_is_member, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_rbt_node_is_red()
 * ----------------------------------------------------
 */
bool bplib_rbt_node_is_red(const bplib_rbt_link_t *node)
{
    UT_GenStub_SetupReturnBuffer(bplib_rbt_node_is_red, bool);

    UT_GenStub_AddParam(bplib_rbt_node_is_red, const bplib_rbt_link_t *, node);

    UT_GenStub_Execute(bplib_rbt_node_is_red, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_rbt_node_is_red, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_rbt_search_generic()
 * ----------------------------------------------------
 */
bplib_rbt_link_t *bplib_rbt_search_generic(bp_val_t search_key_value, const bplib_rbt_root_t *tree,
                                           bplib_rbt_compare_func_t compare_func, void *compare_arg)
{
    UT_GenStub_SetupReturnBuffer(bplib_rbt_search_generic, bplib_rbt_link_t *);

    UT_GenStub_AddParam(bplib_rbt_search_generic, bp_val_t, search_key_value);
    UT_GenStub_AddParam(bplib_rbt_search_generic, const bplib_rbt_root_t *, tree);
    UT_GenStub_AddParam(bplib_rbt_search_generic, bplib_rbt_compare_func_t, compare_func);
    UT_GenStub_AddParam(bplib_rbt_search_generic, void *, compare_arg);

    UT_GenStub_Execute(bplib_rbt_search_generic, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_rbt_search_generic, bplib_rbt_link_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_rbt_tree_is_empty()
 * ----------------------------------------------------
 */
bool bplib_rbt_tree_is_empty(const bplib_rbt_root_t *tree)
{
    UT_GenStub_SetupReturnBuffer(bplib_rbt_tree_is_empty, bool);

    UT_GenStub_AddParam(bplib_rbt_tree_is_empty, const bplib_rbt_root_t *, tree);

    UT_GenStub_Execute(bplib_rbt_tree_is_empty, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_rbt_tree_is_empty, bool);
}
