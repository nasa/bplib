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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_rbt_impl.h"

/*--------------------------------------------------------------------------------------
 * BPLib_RBT_InitRoot - Initializes a root node to a safe state
 *
 * tree: A ptr to a BPLib_RBT_Root_t
 *--------------------------------------------------------------------------------------*/
void BPLib_RBT_InitRoot(BPLib_RBT_Root_t *tree)
{
    memset(tree, 0, sizeof(*tree));
}

/*--------------------------------------------------------------------------------------
 * BPLib_RBT_TreeIsEmpty - Checks if the given tree is empty
 *
 * tree: A ptr to a BPLib_RBT_Root_t
 * returns: true if the tree is empty
 *--------------------------------------------------------------------------------------*/
bool BPLib_RBT_TreeIsEmpty(const BPLib_RBT_Root_t *tree)
{
    return (tree->root == NULL);
}

/*--------------------------------------------------------------------------------------
 * BPLib_RBT_NodeIsMember - Checks if the given node is a member of the tree
 *
 * tree: A ptr to a BPLib_RBT_Root_t
 * node: Memory block to test
 * returns: true if the node is a member of the tree, false otherwise
 *--------------------------------------------------------------------------------------*/
bool BPLib_RBT_NodeIsMember(const BPLib_RBT_Root_t *tree, const BPLib_RBT_Link_t *node)
{
    const BPLib_RBT_Link_t *root_node;

    /* follow parent links until arrival at the tree root.  If a member
     * of the tree, this should arrive at the same root node as pointed to
     * in the "tree" structure */
    root_node = node;
    while (root_node->parent != NULL)
    {
        root_node = root_node->parent;
    }

    return (root_node == tree->root);
}

/*--------------------------------------------------------------------------------------
 * BPLib_RBT_SearchGeneric - Searches a rb_tree for a node containing a given value.
 *
 * tree: A ptr to a BPLib_RBT_Root_t to search. [INPUT]
 * value: The value to search for within the BPLib_RBT_Root_t. [INPUT]
 * returns: A ptr to a BPLib_RBT_Link_t to populate with the identified node. This is set to NULL
 *      if no node is found.
 *--------------------------------------------------------------------------------------*/
BPLib_RBT_Link_t *BPLib_RBT_SearchGeneric(BPLib_Val_t search_key_value, const BPLib_RBT_Root_t *tree,
                                          BPLib_RBT_CompareFunc_t compare_func, void *compare_arg)
{
    BPLib_RBT_Link_t *curr_ptr;
    int               compare_result;

    curr_ptr = tree->root;

    while (curr_ptr != NULL)
    {
        compare_result = BPLib_RBT_CompareKeyImpl(search_key_value, BPLib_RBT_GetKeyValueImpl(curr_ptr));
        if (compare_result == 0 && compare_func != NULL)
        {
            compare_result = compare_func(curr_ptr, compare_arg);
        }

        if (compare_result == 0)
        {
            /* found a key match */
            break;
        }

        if (compare_result > 0)
        {
            curr_ptr = curr_ptr->right;
        }
        else
        {
            curr_ptr = curr_ptr->left;
        }
    }

    return curr_ptr;
}

/*--------------------------------------------------------------------------------------
 * BPLib_RBT_InsertValueGeneric - Inserts a value into the red black tree and rebalances it accordingly.
 *
 * value - The value to insert into the BPLib_RBT_Root_t. [INPUT]
 * tree: A ptr to a BPLib_RBT_Root_t to insert the value into. [OUTPUT]
 * new_node: Memory block for storage of the new value [INPUT]
 * returns: Status code indicating the result of the insertion.
 *--------------------------------------------------------------------------------------*/
BPLib_Status_t BPLib_RBT_InsertValueGeneric(BPLib_Val_t insert_key_value, BPLib_RBT_Root_t *tree,
                                            BPLib_RBT_Link_t *link_block, BPLib_RBT_CompareFunc_t compare_func,
                                            void *compare_arg)
{
    int status;

    if (tree->root == link_block || BPLib_RBT_NodeIsAttachedImpl(link_block))
    {
        return BPLIB_ERROR;
    }

    /*
     * The new node will always be connected as a leaf, so assume its child
     * pointers are both NULL.
     *
     * This sets the key value and also paints it red, which is the initial
     * condition for insert because it does not affect the count of black nodes
     * from the root to the leaf.
     */
    BPLib_RBT_InitKeyValueImpl(link_block, insert_key_value);

    status = BPLib_RBT_InsertAsLeafImpl(tree, link_block, compare_func, compare_arg);
    if (status == BPLIB_SUCCESS)
    {
        /* Correct any violations within the red black tree due to the insertion. */
        BPLib_RBT_InsertRebalanceImpl(tree, link_block);
    }
    else
    {
        /* some use cases use a zero key to mean that it is not in a tree, so be sure to
         * clear the link if not successfully inserted so it does not have a value. */
        memset(link_block, 0, sizeof(*link_block));
    }

    return status;
}

/*--------------------------------------------------------------------------------------
 * BPLib_RBT_ExtractNode - Removes a single node from the red black tree and rebalances it accordingly.
 *
 * tree: A ptr to a BPLib_RBT_Root_t to delete value from. [OUTPUT]
 * link_block: Memory block that previously stored the value
 * returns: Status code indicating the result of the deletion.
 *--------------------------------------------------------------------------------------*/
BPLib_Status_t BPLib_RBT_ExtractNode(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *link_block)
{
    if (tree->root != link_block && !BPLib_RBT_NodeIsAttachedImpl(link_block))
    {
        return BPLIB_ERROR;
    }

    /*
     * start by forcing this node into a leaf position.
     * This is achieved by swapping node positions with other
     * items, as needed.
     *
     * Where swaps are done, the original colors are maintained in their
     * original tree locations, such that the R-B constraints are unchanged,
     * only the values will reside in different spots.
     *
     * As noted, this will temporarily render an out-of-order
     * tree, but the misplaced node will be the one being deleted,
     * so once that is finally done, it will be OK again.
     */
    BPLib_RBT_DeleteMakeLeafImpl(tree, link_block);

    /*
     * Rebalance the tree anticipating the removal of the subject leaf
     */
    BPLib_RBT_DeleteRebalanceImpl(tree, link_block);

    /*
     * Do the actual disconnect
     */
    *(BPLib_RBT_FindParentRefImpl(tree, link_block)) = NULL;

    /* reset this entire link block - this also clears the node value and resets it to 0 */
    memset(link_block, 0, sizeof(*link_block));

    return BPLIB_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * BPLib_RBT_GetKeyValue
 *
 * returns: Value of the given node
 *--------------------------------------------------------------------------------------*/
BPLib_Val_t BPLib_RBT_GetKeyValue(const BPLib_RBT_Link_t *node)
{
    return BPLib_RBT_GetKeyValueImpl(node);
}

/*--------------------------------------------------------------------------------------
 * BPLib_RBT_NodeIsRed (externally exposed for debug/testing purposes)
 *
 * returns: True if the given node is red, false if black
 *--------------------------------------------------------------------------------------*/
bool BPLib_RBT_NodeIsRed(const BPLib_RBT_Link_t *node)
{
    return BPLib_RBT_IsRedImpl(node);
}
