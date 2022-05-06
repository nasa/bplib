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

#ifndef V7_RBTREE_H
#define V7_RBTREE_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_api_types.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/*
 * Link structure for use in red-black storage trees
 *
 * For a bit more memory efficiency this steals one bit from
 * the key value to track the node color (red/black).  This means
 * keys are effectively limited to 63 bits.
 */
typedef struct bplib_rbt_link
{
    bp_val_t               key_value_and_color;
    struct bplib_rbt_link *left;
    struct bplib_rbt_link *right;
    struct bplib_rbt_link *parent;
} bplib_rbt_link_t;

/**
 * @brief Basic R-B tree parent structure
 */
typedef struct bplib_rbt_root
{
    int               black_height; /* debug facility */
    bplib_rbt_link_t *root;         /* The current root of the tree. This may change as rebalance occurs. */
} bplib_rbt_root_t;

typedef struct bplib_rbt_iter
{
    const bplib_rbt_link_t *position;
} bplib_rbt_iter_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Red Black Tree API */

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Initializes a newly allocated tree root object
 *
 * The tree will be empty after this call.  This must only be invoked
 * on new root objects, or else memory may be leaked.
 *
 * @param tree The tree object to initialize
 */
void bplib_rbt_init_root(bplib_rbt_root_t *tree);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Checks if the given node is a member of the tree
 *
 * @param[in] tree   The tree to check
 * @param[in] node   The node to check
 * @return true if the node is currently a member of the specified tree
 * @return false if the node is not currently a member of the specified tree
 */
bool bplib_rbt_is_member(const bplib_rbt_root_t *tree, const bplib_rbt_link_t *node);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Searches the Red-Black tree for a node matching a given value
 *
 * The tree is searched for a matching value, and if found, the pointer to that node
 * is returned.
 *
 * @param[in] search_key_value The value to search for
 * @param[in] tree             The tree to search in
 * @return Pointer to the matching node
 * @retval NULL if no matching node was found
 */
bplib_rbt_link_t *bplib_rbt_search(bp_val_t search_key_value, const bplib_rbt_root_t *tree);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Inserts a value into the Red-Black tree and rebalances it accordingly.
 *
 * Sets the new_node to the given value, and inserts it into the tree.  If the insertion caused
 * any unbalance or constraint violation condition, that condition will be corrected before the
 * function returns.
 *
 * @note This routine both sets the key value and inserts it into the tree, there is _not_
 * a separate function to set the key.  This is intentional, because the key determines the
 * correct location in the tree, and must be treated as "const" once it has been set.
 *
 * In order to change a key value, the old value should be removed first, and then this
 * function can be used again to set the new key value.
 *
 * @param[in]    insert_key_value Value to insert, which is saved into into the new node
 * @param[inout] tree             Tree to insert into.  Rebalancing may change the current logical root node of the
 * tree.
 * @param[inout] link_block       Memory block to store the value, contents will be overwitten.  Must be allocated by
 * caller.
 * @retval BP_SUCCESS if node was successfully inserted
 * @retval BP_DUPLICATE if the tree already contained the specified value (duplicates are not allowed)
 */
int bplib_rbt_insert_value(bp_val_t insert_key_value, bplib_rbt_root_t *tree, bplib_rbt_link_t *link_block);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Extracts a single value from the Red-Black tree and rebalances it accordingly.
 *
 * Locates the given value in the tree, and removes that value from the tree.  If the removal causes
 * any unbalance or constraint violation condition, that condition will be corrected before the
 * function returns.
 *
 * @param[in]    extract_key_value Value to extract
 * @param[inout] tree              Tree to extract from.
 * @param[out]   link_block        Memory block formerly used to store the value (may be freed or recycled by caller)
 * @retval BP_SUCCESS if node was successfully extracted
 * @retval BP_ERROR if the tree did not contain the specified value
 */
int bplib_rbt_extract_value(bp_val_t extract_key_value, bplib_rbt_root_t *tree, bplib_rbt_link_t **link_block);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Extracts a node from the Red-Black tree and rebalances it accordingly.
 *
 * This extract function may be used when a node is located by other means (such as another data index that
 * may exist in the parent program) and needs to be extracted from this tree.  If the removal causes
 * any unbalance or constraint violation condition, that condition will be corrected before the
 * function returns.
 *
 * @param[inout] tree              Tree to extract from.
 * @param[in]    remove_node       Memory block to remove
 * @retval BP_SUCCESS if node was successfully extracted
 * @retval BP_ERROR if the tree did not contain the specified value
 */
int bplib_rbt_extract_node(bplib_rbt_root_t *tree, bplib_rbt_link_t *removed_node);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Position an iterator at the first key value in the tree that is greater than or equal to a minimum bound
 *
 * @param minimum_value The minimum bound value
 * @param tree          The tree object
 * @param iter          The iterator object to position
 * @returns integer status code
 * @retval BP_SUCCESS if iterator is valid
 */
int bplib_rbt_iter_goto_min(bp_val_t minimum_value, const bplib_rbt_root_t *tree, bplib_rbt_iter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Position an iterator at the last key value in the tree that is less than or equal to a maximum bound
 *
 * @param maximum_value The maximum bound value
 * @param tree          The tree object
 * @param iter          The iterator object to position
 * @returns integer status code
 * @retval BP_SUCCESS if iterator is valid
 */
int bplib_rbt_iter_goto_max(bp_val_t maximum_value, const bplib_rbt_root_t *tree, bplib_rbt_iter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Move an iterator forward by one step in the tree
 *
 * Sets the iterator to the immediate successor of the current node.
 * This allows the caller to perform a ascending-order tree traversal.
 *
 * @param iter          The iterator object to move
 * @retval BP_SUCCESS if iterator is valid
 */
int bplib_rbt_iter_next(bplib_rbt_iter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Move an iterator backward by one step in the tree
 *
 * Sets the iterator to the immediate predecessor of the current node.
 * This allows the caller to perform a descending-order tree traversal.
 *
 * @param iter          The iterator object to move
 * @retval BP_SUCCESS if iterator is valid
 */
int bplib_rbt_iter_prev(bplib_rbt_iter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Gets the logical key value associated with the node
 *
 * Every node in an R-B tree has a key value associated with it, which dermines its logical order
 * and position in the tree.
 *
 * This function returns the key value associated with the given node.
 *
 * @param node The node to check
 * @return The key value from this node
 */
bp_val_t bplib_rbt_get_key_value(const bplib_rbt_link_t *node);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Checks if a node is red (Debug)
 *
 * Checks if the given node is red or black in the R-B tree balancing logic.
 *
 * @note This routine is intended for debug and test/validation purposes only. Normal application
 * code should not be concerned with the internal color of a tree node and should not typically
 * need to check this.
 *
 * @param node The node to check
 * @return true If the node is red
 * @return false If the node is black
 */
bool bplib_rbt_node_is_red(const bplib_rbt_link_t *node);

#endif /* RB_TREE_H */
