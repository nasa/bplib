/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */

/*
 * BACKGROUND RECOMMENDED READING:
 *
 * https://en.wikipedia.org/wiki/Red%E2%80%93black_tree
 *
 * This implements the red-black tree as described in the above algorithm.
 * For any developer making changes to this code, it is highly recommended
 * to be familiar with the theory behind these operations.
 */

#ifndef BPLIB_RBT_IMPL_H
#define BPLIB_RBT_IMPL_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_api_types.h"
#include "bplib_rbt.h"

/*
 * mark for conditions that must be true in the implementation.
 * Normally these are no-op and simply left the source as documentation,
 * but during debug this could be changed to an assert or similar.
 */
#define BPLIB_RBT_REQUIRED_CONDITION(...)

/******************************************************************************
 DEFINES
 ******************************************************************************/

/*
 * To improve memory efficiency, this implementation steals 1 bit from
 * the key value field to track the node color.  This means that the
 * effective key size is limited to 63 bits on a 64-bit CPU platform.
 *
 * Without this bit stealing, an entire extra 64-bit word would be needed to
 * keep the color value.
 */
#define BPLIB_RBT_RED_FLAG   0x01
#define BPLIB_RBT_COLOR_MASK (~((BPLib_Val_t)BPLIB_RBT_RED_FLAG))

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Sets a node to be black
 *
 * @note Node must not be NULL
 *
 * @param node A ptr to an BPLib_RBT_Link_t to set the color to black
 */
static inline void BPLib_RBT_SetBlackImpl(BPLib_RBT_Link_t *node)
{
    node->key_value_and_color &= BPLIB_RBT_COLOR_MASK;
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Sets a node to be red
 *
 * @note Node must not be NULL
 *
 * @param node A ptr to an BPLib_RBT_Link_t to set the color to red
 */
static inline void BPLib_RBT_SetRedImpl(BPLib_RBT_Link_t *node)
{
    node->key_value_and_color |= BPLIB_RBT_RED_FLAG;
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief  Swaps the colors of two nodes
 *
 * node2 gets the color of node1, and node1 gets the color of node2
 *
 * @note Nodes must not be NULL
 * @param node1 A ptr to an BPLib_RBT_Link_t to swap the colors
 * @param node2 A ptr to an BPLib_RBT_Link_t to swap the colors
 */
static inline void BPLib_RBT_SwapColorImpl(BPLib_RBT_Link_t *node1, BPLib_RBT_Link_t *node2)
{
    node1->key_value_and_color ^= node2->key_value_and_color & BPLIB_RBT_RED_FLAG;
    node2->key_value_and_color ^= node1->key_value_and_color & BPLIB_RBT_RED_FLAG;
    node1->key_value_and_color ^= node2->key_value_and_color & BPLIB_RBT_RED_FLAG;
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Initially configure the node with the given value
 *
 * Color will be set to RED (default for all new nodes)
 *
 * @note Node must not be NULL
 * @param node A ptr to an BPLib_RBT_Link_t to initialize
 */
static inline void BPLib_RBT_InitKeyValueImpl(BPLib_RBT_Link_t *node, BPLib_Val_t value)
{
    node->key_value_and_color = BPLIB_RBT_RED_FLAG | (value << 1);
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Gets the key value from the given node
 *
 * @note Node must not be NULL
 * @param node A ptr to an BPLib_RBT_Link_t
 */
static inline BPLib_Val_t BPLib_RBT_GetKeyValueImpl(const BPLib_RBT_Link_t *node)
{
    return (node->key_value_and_color >> 1);
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Checks if a node is red
 *
 * This is intended to be used where node is already known to be non-null
 *
 * @param node Pointer to the node to check
 * @retval true if node is red
 * @retval false if node is black
 */
static inline bool BPLib_RBT_IsRedNonNullImpl(const BPLib_RBT_Link_t *node)
{
    return (node->key_value_and_color & BPLIB_RBT_RED_FLAG) != 0;
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Checks if a node is black
 *
 * This is intended to be used where node is already known to be non-null
 *
 * @param node Pointer to the node to check
 * @retval true if node is black
 * @retval false if node is red
 */
static inline bool BPLib_RBT_IsBlackNonNullImpl(const BPLib_RBT_Link_t *node)
{
    return (node->key_value_and_color & BPLIB_RBT_RED_FLAG) == 0;
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Checks if a node is black
 *
 * this checks the color for logical/algorithmic purposes, where a NIL/NULL node
 * may be passed in, and is considered black.
 *
 * @param node Pointer to the node to check
 * @retval true if node is black
 * @retval false if node is red
 */
static inline bool BPLib_RBT_IsBlackImpl(const BPLib_RBT_Link_t *node)
{
    return node == NULL || BPLib_RBT_IsBlackNonNullImpl(node);
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Checks if a node is red
 *
 * this checks the color for logical/algorithmic purposes, where a NIL/NULL node
 * may be passed in, and is considered black.
 *
 * @param node Pointer to the node to check
 * @retval false if node is black
 * @retval true if node is red
 */
static inline bool BPLib_RBT_IsRedImpl(const BPLib_RBT_Link_t *node)
{
    return node != NULL && BPLib_RBT_IsRedNonNullImpl(node);
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Connects a parent and child node
 *
 * Node c becomes the left child of p
 * This is intended to be used where both c and p are already known to be non-null
 *
 * @param p The parent node, must not be NULL
 * @param c The child node, must not be NULL
 */
static inline void BPLib_RBT_ConnectLeftNonNullImpl(BPLib_RBT_Link_t *p, BPLib_RBT_Link_t *c)
{
    p->left   = c;
    c->parent = p;
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Connects a parent and child node
 *
 * Node c becomes the right child of p
 * This is intended to be used where both c and p are already known to be non-null
 *
 * @param p The parent node, must not be NULL
 * @param c The child node, must not be NULL
 */
static inline void BPLib_RBT_ConnectRightNonNullImpl(BPLib_RBT_Link_t *p, BPLib_RBT_Link_t *c)
{
    p->right  = c;
    c->parent = p;
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Connects a parent and child node
 *
 * Node c becomes the left child of p
 * This is intended to be used where c has not been checked and might be NULL
 *
 * @param p The parent node, must not be NULL
 * @param c The child node, may be NULL
 */
static inline void BPLib_RBT_ConnectLeftSafeImpl(BPLib_RBT_Link_t *p, BPLib_RBT_Link_t *c)
{
    p->left = c;
    if (c)
    {
        c->parent = p;
    }
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Connects a parent and child node
 *
 * Node c becomes the right child of p
 * This is intended to be used where c has not been checked and might be NULL
 *
 * @param p The parent node, must not be NULL
 * @param c The child node, may be NULL
 */
static inline void BPLib_RBT_ConnectRightSafeImpl(BPLib_RBT_Link_t *p, BPLib_RBT_Link_t *c)
{
    p->right = c;
    if (c)
    {
        c->parent = p;
    }
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief  Checks if the node is part of a tree
 *
 * NOTE - this is not quite sufficient of a check, as a tree with a single node
 * will also have NULL pointers.  As a minimum one should also check if the node is
 * the root of the tree in addition to this test.
 *
 * @param node Pointer to the node to check
 * @retval true if the node is attached to a tree
 * @retval false if node is not attached
 */
static inline bool BPLib_RBT_NodeIsAttachedImpl(const BPLib_RBT_Link_t *node)
{
    /* if any pointer is set, this is in a tree */
    return (node->parent != NULL || node->left != NULL || node->right != NULL);
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Compares two primary keys
 *
 * The primary keys are used for implementing the basic R-B algorithm
 * The sign of the return value indicates the result of comparison, and
 * is intended to be compatible with C library functions like strcmp()
 *
 * @param key1 The first key value
 * @param key2 The second key value
 * @retval 0 if the two keys are equal
 * @retval 1 if the key1 > key2
 * @retval -1 if the key1 < key2
 */
static inline int BPLib_RBT_CompareKeyImpl(BPLib_Val_t key1, BPLib_Val_t key2)
{
    /**
     * The comments on the return values state that they are not BPLib_Status_t
     * values for code reviews that look for invalid return values.
     */
    if (key1 == key2)
    {
        return 0; // key1 == key2
    }
    else if (key1 > key2)
    {
        return 1; // key1 > key2
    }
    else
    {
        return -1; // key1 < key2
    }
}

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Finds the pointer in the parent node that refers back to this node
 *
 * Either the left or right pointer of the parent should point back to this node.
 * This returns a pointer to that self-reference.  In the event that the node is
 * the root node, then a ref to the tree root is returned.  As such, this never
 * returns a NULL ref.
 *
 * @param tree   Pointer to the entire tree
 * @param node The node to find the parent reference to
 * @return Pointer to the reference in the parent pointing to node
 */
BPLib_RBT_Link_t **BPLib_RBT_FindParentRefImpl(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *node);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Swap two nodes that are known to have a parent/child relationship
 *
 * This may change the root location of the tree, if the parent is the current root
 * This is a slightly faster swap function that only works for nodes that have a
 * parent/child relationship.  To swap other nodes, see BPLib_RBT_SwapDistantImpl().
 *
 * @param tree   Pointer to the entire tree
 * @param parent Pointer to the current parent which should become child
 * @param child  Pointer to the current child which should become parent
 */
void BPLib_RBT_SwapParentChildImpl(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *parent, BPLib_RBT_Link_t *child);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Swap two nodes that are known not to have a parent/child relationship
 *
 * This may change the root location of the tree, if one of the nodes is the current root
 *
 * @param tree   Pointer to the entire tree
 * @param node1  Pointer to the current parent which should become child
 * @param node2  Pointer to the current child which should become parent
 */
void BPLib_RBT_SwapDistantImpl(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *node1, BPLib_RBT_Link_t *node2);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Does a trivial insert of a new node into the rb tree.
 *
 * This traverses the tree to find the correct insertion point, which will always arrive at
 * a leaf node if no duplicates exist.  If a duplicate does exist, this is an error.
 *
 * The new node is always initially painted red, so this insert will never affect the black depth,
 * but may cause a red-red violation to exist if the parent was red.  This can be corrected later.
 *
 * @param insert_key_value The value of the node to attempt to insert or merge into the red black tree.
 * @param tree A ptr to the BPLib_RBT_Root_t to insert values into.
 * @param new_node Pointer to the new node link structure
 * @returns BPLib_Status_t indicating the result of the insertion attempt (BPLIB_RBT_DUPLICATE or BPLIB_SUCCESS)
 */
BPLib_Status_t BPLib_RBT_InsertAsLeafImpl(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *new_node,
                                          BPLib_RBT_CompareFunc_t compare_func, void *compare_arg);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief  Ensure that the red black tree adheres to all constraints
 *
 * This rebalances the nodes if required.  This process should be executed after every
 * insertion to ensure constraints are always adhered to.
 *
 * @note rotations may change the location of the logical root of the tree.
 *
 * @param tree pointer to the tree (required because this may change the logical root)
 * @param initial_subject A ptr to the last inserted node in the red black tree. Must be red (initial color for new
 * nodes)
 *
 */
void BPLib_RBT_InsertRebalanceImpl(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *initial_subject);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief  Forces the specified node into a leaf position
 *
 * The given node is made to be a leaf by swapping positions with other descendant nodes,
 * until arriving at a leaf position.  This is a dependency for eventual node removal.
 *
 * @note after this operation, the  specified node will no longer be in the
 * correct location in the tree, according to its key value.  As the intent is
 * to remove the node, this (temporary) invalid configuration is acceptable.
 *
 * @param tree A ptr to the BPLib_RBT_Root_t to operate on
 * @param node A ptr to the BPLib_RBT_Link_t that is targeted for removal
 */
void BPLib_RBT_DeleteMakeLeafImpl(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *node);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief  Rebalance the BPLib_RBT_Root_t to account for the deletion of a leaf node
 *
 * This rebalances the R-B tree to account for the deletion of a node, which must be a leaf
 * node.  The function BPLib_RBT_DeleteMakeLeafImpl() should be used first, to force the node into
 * a leaf position.
 *
 * @param tree A ptr to the BPLib_RBT_Root_t to rebalance.
 * @param initial_subject A ptr to the BPLib_RBT_Link_t from which to start the rebalancing operation
 *
 */
void BPLib_RBT_DeleteRebalanceImpl(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *initial_subject);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Initializes an interator according to target_value
 *
 * Find a node that is either exactly matching the target value, or if that does not exist,
 * returns the nearest leaf node where that value would be attached.
 *
 * @param target_value The value where the iterator should be positioned
 * @param start_pos The tree (or subtree) to search within
 * @returns Pointer to node nearest to target_value
 */
const BPLib_RBT_Link_t *BPLib_RBT_IterFindClosestImpl(BPLib_Val_t target_value, const BPLib_RBT_Link_t *start_pos);

#endif
