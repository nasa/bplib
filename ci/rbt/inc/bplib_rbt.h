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

#ifndef BPLIB_RBT_H
#define BPLIB_RBT_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_api_types.h"

#define BPLIB_LOCAL_SCOPE static

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
typedef struct BPLib_RBT_Link
{
    BPLib_Val_t               key_value_and_color;
    struct BPLib_RBT_Link *left;
    struct BPLib_RBT_Link *right;
    struct BPLib_RBT_Link *parent;
} BPLib_RBT_Link_t;

/**
 * @brief Comparison function for nodes with the same key value
 *
 * This RB tree implementation optionally allows for trees with duplicate keys.
 * If this feature is used, then this function provices a secondary comparison
 * for nodes which have the same primary key value.
 *
 * @retval 0 if node is equal to the reference object
 * @returns positive integer value if node is logically greater than the reference object
 * @returns negative integer value if node is logically less than the reference object
 *
 */
typedef int (*BPLib_RBT_CompareFunc_t)(const BPLib_RBT_Link_t *node, void *arg);

/**
 * @brief Basic R-B tree parent structure
 */
typedef struct BPLib_RBT_Root
{
    int               black_height; /* debug facility */
    BPLib_RBT_Link_t *root;         /* The current root of the tree. This may change as rebalance occurs. */
} BPLib_RBT_Root_t;

typedef struct BPLib_RBT_Iter
{
    const BPLib_RBT_Link_t *position;
} BPLib_RBT_Iter_t;

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
void BPLib_RBT_InitRoot(BPLib_RBT_Root_t *tree);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Checks if the given tree has any nodes
 *
 * @param[in] tree   The tree to check
 * @return true if the tree is currently empty
 * @return false if the tree is not currently empty
 */
bool BPLib_RBT_TreeIsEmpty(const BPLib_RBT_Root_t *tree);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Checks if the given node is a member of the tree
 *
 * @param[in] tree   The tree to check
 * @param[in] node   The node to check
 * @return true if the node is currently a member of the specified tree
 * @return false if the node is not currently a member of the specified tree
 */
bool BPLib_RBT_NodeIsMember(const BPLib_RBT_Root_t *tree, const BPLib_RBT_Link_t *node);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Searches the Red-Black tree for a node matching a given value
 *
 * The tree is searched for a matching value, and if found, the pointer to that node
 * is returned.
 *
 * This implementation allows multiple nodes with the same key. In order to search for
 * a specific node in a tree that has multiple enties with the same key, a compare function
 * must be provided by the caller.
 *
 * If the compare function is NULL, then the first node encountered that has a matching key
 * will be returned.  If more than one matching key exists, then it is not defined which
 * node will be returned unless the compare_func is non-NULL.
 *
 * @param[in] search_key_value The value to search for
 * @param[in] tree             The tree to search in
 * @param[in] compare_func     Comparison function to be called on every node with matching key
 * @param[in] compare_arg      Opaque argument passed through to comparison function
 * @return Pointer to the matching node
 * @retval NULL if no matching node was found
 */
BPLib_RBT_Link_t *BPLib_RBT_SearchGeneric(BPLib_Val_t search_key_value, const BPLib_RBT_Root_t *tree,
                                                  BPLib_RBT_CompareFunc_t compare_func, void *compare_arg);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Searches the Red-Black tree for a node matching a given value
 *
 * The tree is searched for a matching value, and if found, the pointer to that node
 * is returned.
 *
 * This simplified function can be used on trees that have unique keys
 *
 * @param[in] search_key_value The value to search for
 * @param[in] tree             The tree to search in
 * @return Pointer to the matching node
 * @retval NULL if no matching node was found
 */
static inline BPLib_RBT_Link_t *BPLib_RBT_SearchUnique(BPLib_Val_t search_key_value,
                                                               const BPLib_RBT_Root_t *tree)
{
    return BPLib_RBT_SearchGeneric(search_key_value, tree, NULL, NULL);
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Inserts a value into the Red-Black tree and rebalances it accordingly.
 *
 * Sets the new_node to the given value, and inserts it into the tree.  If the insertion caused
 * any unbalance or constraint violation condition, that condition will be corrected before the
 * function returns.
 *
 * The "compare_func" is a secondary comparision to handle duplicate keys.  If passed as non-NULL,
 * then duplicate keys will be allowed, and the comparison function will be used to differentiate
 * between nodes with the same key value.  If the comparison function is NULL, then duplicate key
 * values will not be allowed.
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
 * @param[in] compare_func     Comparison function to be called on every node with matching key
 * @param[in] compare_arg      Opaque argument passed through to comparison function
 * @retval BPLIB_SUCCESS if node was successfully inserted
 * @retval BPLIB_RBT_DUPLICATE if the tree already contained the specified value (duplicates are not allowed)
 */
BPLib_Status_t BPLib_RBT_InsertValueGeneric(BPLib_Val_t insert_key_value, BPLib_RBT_Root_t *tree,
                                                BPLib_RBT_Link_t *link_block,
                                                BPLib_RBT_CompareFunc_t compare_func, void *compare_arg);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Inserts a value into the Red-Black tree and rebalances it accordingly.
 *
 * Sets the new_node to the given value, and inserts it into the tree.  If the insertion caused
 * any unbalance or constraint violation condition, that condition will be corrected before the
 * function returns.
 *
 * The "compare_func" is a secondary comparision to handle duplicate keys.  If passed as non-NULL,
 * then duplicate keys will be allowed, and the comparison function will be used to differentiate
 * between nodes with the same key value.  If the comparison function is NULL, then duplicate key
 * values will not be allowed.
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
 * @retval BPLIB_SUCCESS if node was successfully inserted
 * @retval BPLIB_RBT_DUPLICATE if the tree already contained the specified value (duplicates are not allowed)
 */
static inline int BPLib_RBT_InsertValueUnique(BPLib_Val_t insert_key_value, BPLib_RBT_Root_t *tree,
                                                BPLib_RBT_Link_t *link_block)
{
    return BPLib_RBT_InsertValueGeneric(insert_key_value, tree, link_block, NULL, NULL);
}

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
 * @param[in]    link_block        Memory block to remove
 * @retval BPLIB_SUCCESS if node was successfully extracted
 * @retval BPLIB_ERROR if the tree did not contain the specified value
 */
BPLib_Status_t BPLib_RBT_ExtractNode(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *link_block);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Position an iterator at the first key value in the tree that is greater than or equal to a minimum bound
 *
 * @param minimum_value The minimum bound value
 * @param tree          The tree object
 * @param iter          The iterator object to position
 * @returns integer status code
 * @retval BPLIB_SUCCESS if iterator is valid
 */
BPLib_Status_t BPLib_RBT_IterGotoMin(BPLib_Val_t minimum_value, const BPLib_RBT_Root_t *tree,
                                         BPLib_RBT_Iter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Position an iterator at the last key value in the tree that is less than or equal to a maximum bound
 *
 * @param maximum_value The maximum bound value
 * @param tree          The tree object
 * @param iter          The iterator object to position
 * @returns integer status code
 * @retval BPLIB_SUCCESS if iterator is valid
 */
BPLib_Status_t BPLib_RBT_IterGotoMax(BPLib_Val_t maximum_value, const BPLib_RBT_Root_t *tree,
                                         BPLib_RBT_Iter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Move an iterator forward by one step in the tree
 *
 * Sets the iterator to the immediate successor of the current node.
 * This allows the caller to perform a ascending-order tree traversal.
 *
 * @param iter          The iterator object to move
 * @retval BPLIB_SUCCESS if iterator is valid
 */
BPLib_Status_t BPLib_RBT_IterNext(BPLib_RBT_Iter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Move an iterator backward by one step in the tree
 *
 * Sets the iterator to the immediate predecessor of the current node.
 * This allows the caller to perform a descending-order tree traversal.
 *
 * @param iter          The iterator object to move
 * @retval BPLIB_SUCCESS if iterator is valid
 */
BPLib_Status_t BPLib_RBT_IterPrev(BPLib_RBT_Iter_t *iter);

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
BPLib_Val_t BPLib_RBT_GetKeyValue(const BPLib_RBT_Link_t *node);

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
bool BPLib_RBT_NodeIsRed(const BPLib_RBT_Link_t *node);

#endif /* BPLIB_RBT_H */
