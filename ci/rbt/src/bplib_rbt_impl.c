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
 * Internal implementation routine, see declaration for detail
 *--------------------------------------------------------------------------------------*/
BPLib_RBT_Link_t **BPLib_RBT_FindParentRefImpl(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *node)
{
    BPLib_RBT_Link_t **ref;

    if (node->parent == NULL)
    {
        /* this is the root */
        ref = &tree->root;
    }
    else if (node->parent->left == node)
    {
        ref = &node->parent->left;
    }
    else
    {
        ref = &node->parent->right;
    }

    /* debug: confirm that the ref does indeed point back to the subject node */
    BPLIB_RBT_REQUIRED_CONDITION(*ref == node);
    return ref;
}

/*--------------------------------------------------------------------------------------
 * Internal implementation routine, see declaration for detail
 *--------------------------------------------------------------------------------------*/
void BPLib_RBT_SwapParentChildImpl(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *parent, BPLib_RBT_Link_t *child)
{
    BPLib_RBT_Link_t **grandparent_ref;
    BPLib_RBT_Link_t  *saved_left;
    BPLib_RBT_Link_t  *saved_right;

    /* temporarily save off the left/right from the original parent (one of these is the subject child) */
    saved_left  = parent->left;
    saved_right = parent->right;

    /* The grandparent should now refer to the child */
    grandparent_ref  = BPLib_RBT_FindParentRefImpl(tree, parent);
    child->parent    = parent->parent;
    *grandparent_ref = child;

    /* both of the original grandchildren (if present) will now become the parent's immediate children */
    BPLib_RBT_ConnectLeftSafeImpl(parent, child->left);
    BPLib_RBT_ConnectRightSafeImpl(parent, child->right);

    if (saved_left == child)
    {
        /* original child was on the left side of original parent */
        /* reattach original sibling now as right child of new parent */
        BPLib_RBT_ConnectRightSafeImpl(child, saved_right);
        BPLib_RBT_ConnectLeftNonNullImpl(child, parent);
    }
    else
    {
        /* original child was on the right side of original parent */
        /* reattach original sibling now as left child of new parent */
        BPLib_RBT_ConnectLeftSafeImpl(child, saved_left);
        BPLib_RBT_ConnectRightNonNullImpl(child, parent);
    }
}

/*--------------------------------------------------------------------------------------
 * Internal implementation routine, see declaration for detail
 *--------------------------------------------------------------------------------------*/
void BPLib_RBT_SwapDistantImpl(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *node1, BPLib_RBT_Link_t *node2)
{
    BPLib_RBT_Link_t *saved_left;
    BPLib_RBT_Link_t *saved_right;
    BPLib_RBT_Link_t *saved_parent;

    /* switch parent pointers */
    *(BPLib_RBT_FindParentRefImpl(tree, node1)) = node2;
    *(BPLib_RBT_FindParentRefImpl(tree, node2)) = node1;
    saved_parent                                = node1->parent;
    node1->parent                               = node2->parent;
    node2->parent                               = saved_parent;

    /* temporarily save off the left/right from the original nodes */
    saved_left  = node1->left;
    saved_right = node1->right;
    BPLib_RBT_ConnectLeftSafeImpl(node1, node2->left);
    BPLib_RBT_ConnectRightSafeImpl(node1, node2->right);
    BPLib_RBT_ConnectLeftSafeImpl(node2, saved_left);
    BPLib_RBT_ConnectRightSafeImpl(node2, saved_right);
}

/*--------------------------------------------------------------------------------------
 * Internal implementation routine, see declaration for detail
 *--------------------------------------------------------------------------------------*/
BPLib_Status_t BPLib_RBT_InsertAsLeafImpl(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *new_node,
                                          BPLib_RBT_CompareFunc_t compare_func, void *compare_arg)
{
    BPLib_RBT_Link_t  *curr_ptr;
    BPLib_RBT_Link_t **ref_ptr;
    BPLib_RBT_Link_t  *parent_ptr;
    BPLib_Val_t        insert_key_value;
    BPLib_Status_t     status;
    int                compare_result;

    status           = BPLIB_ERROR;
    parent_ptr       = NULL;
    insert_key_value = BPLib_RBT_GetKeyValueImpl(new_node);
    ref_ptr          = &tree->root;
    while (true)
    {
        curr_ptr = *ref_ptr;
        if (curr_ptr == NULL)
        {
            /* got to a leaf without a duplicate - so insert it here */
            new_node->parent = parent_ptr;
            *ref_ptr         = new_node;
            status           = BPLIB_SUCCESS;
            break;
        }

        compare_result = BPLib_RBT_CompareKeyImpl(insert_key_value, BPLib_RBT_GetKeyValueImpl(curr_ptr));
        if (compare_result == 0 && compare_func != NULL)
        {
            compare_result = compare_func(curr_ptr, compare_arg);
        }

        if (compare_result == 0)
        {
            /* duplicate entries are not allowed */
            status = BPLIB_RBT_DUPLICATE;
            break;
        }

        parent_ptr = curr_ptr;
        if (compare_result > 0)
        {
            ref_ptr = &curr_ptr->right;
        }
        else
        {
            ref_ptr = &curr_ptr->left;
        }
    }

    return status;
}

/*--------------------------------------------------------------------------------------
 * Internal implementation routine, see declaration for detail
 *--------------------------------------------------------------------------------------*/
void BPLib_RBT_InsertRebalanceImpl(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *initial_subject)
{
    BPLib_RBT_Link_t *grandparent;
    BPLib_RBT_Link_t *parent;
    BPLib_RBT_Link_t *uncle;
    BPLib_RBT_Link_t *node;
    bool              node_is_left_side;
    bool              parent_is_left_side;
    enum
    {
        insert_case_undefined,        /**< invalid/undefined/unknown tree state */
        insert_case_no_action_needed, /**< No red-red violation exists, tree constraints are satisfied  */

        /*
         * For all cases here in this group, a red-red (red node, red parent) violation exists.
         * The sub-case determines the appropriate course of action - either repainting or rotating.
         */
        insert_case_subject_with_red_parent_at_root, /**< parent is red and is the root, no grandparent or uncle exists
                                                      */
        insert_case_subject_with_red_parent_and_uncle, /**< parent is red and the uncle is also red */
        insert_case_subject_is_inner_grandchild, /**< parent is red, uncle is black, node is the inner grandchild */
        insert_case_subject_is_outer_grandchild, /**< parent is red, uncle is black, node is the outer grandchild */

    } rebalance_case;

    rebalance_case = insert_case_undefined;
    parent         = NULL;
    grandparent    = NULL;
    uncle          = NULL;

    /* On entry, the inserted node must always be a leaf node, and is the first rebalance subject */
    /* As the rebalance loop runs, this may change as the tree gets rotated around */
    node = initial_subject;
    BPLIB_RBT_REQUIRED_CONDITION(node->left == NULL);
    BPLIB_RBT_REQUIRED_CONDITION(node->right == NULL);

    while (rebalance_case != insert_case_no_action_needed)
    {
        /* At the start of this loop, the reference node (subject) must always be red */
        BPLIB_RBT_REQUIRED_CONDITION(BPLib_RBT_IsRedImpl(node));

        /*
         * If not yet known, determine what node arrangement we have,
         * categorize into one of the rebalance cases above.
         */
        parent_is_left_side = false;
        parent              = node->parent;
        grandparent         = NULL;
        uncle               = NULL;
        if (BPLib_RBT_IsBlackImpl(parent))
        {
            /* parent is black, this is simple as the addition will not violate any rules */
            /* note this also catches the case where the node is the first one added, and is the root */
            rebalance_case = insert_case_no_action_needed;
        }
        else
        {
            node_is_left_side = (parent->left == node);

            /* parent is known to be red, which means we have a red-red constraint violation to resolve */
            grandparent = parent->parent;
            if (grandparent == NULL)
            {
                rebalance_case = insert_case_subject_with_red_parent_at_root;
                uncle          = NULL;
            }
            else
            {
                /* the uncle is the opposite sibling from the parent */
                parent_is_left_side = (grandparent->left == parent);
                if (parent_is_left_side)
                {
                    uncle = grandparent->right;
                }
                else
                {
                    uncle = grandparent->left;
                }

                if (BPLib_RBT_IsRedImpl(uncle))
                {
                    rebalance_case = insert_case_subject_with_red_parent_and_uncle;
                }
                else if (node_is_left_side == parent_is_left_side)
                {
                    /* both parent and subject node are no the same side,
                     * this  makes it an outer grandchild */
                    rebalance_case = insert_case_subject_is_outer_grandchild;
                }
                else
                {
                    rebalance_case = insert_case_subject_is_inner_grandchild;
                }
            }
        }

        /* now that we know R-B problem currently exists, do any necessary work to rearrange nodes */
        if (rebalance_case == insert_case_subject_with_red_parent_at_root)
        {
            /* this scenario just requires the parent to be painted black, so rules will be satisfied. */
            BPLib_RBT_SetBlackImpl(parent);
            rebalance_case = insert_case_no_action_needed;
            ++tree->black_height;
        }
        else if (rebalance_case == insert_case_subject_with_red_parent_and_uncle)
        {
            /* Both parent and uncle should be made black. */
            BPLib_RBT_SetBlackImpl(parent);
            BPLib_RBT_SetBlackImpl(uncle);
            BPLib_RBT_SetRedImpl(grandparent);

            /* Rebalancing continues at the grandparent - because it was made red,
             * need to confirm it is not violating the R-R rule.  This repeats the
             * entire rebalancing algorithm at the grandparent level.  */
            node           = grandparent;
            rebalance_case = insert_case_undefined;
        }
        else if (rebalance_case == insert_case_subject_is_inner_grandchild)
        {
            if (parent_is_left_side)
            {
                /*
                 * Node is the "inner" grandchild on left, needs initial left rotation
                 *
                 *   Before:     16(Gb)          After:     16(Gb)
                 *              /      \                    /     \
                 *            8(Pr)     24(Ub)         12(N'r)    24(Ub)
                 *          /   \        /  \         /    \      /  \
                 *        4      12(Nr) x    x      8(P'r)  x    x    x
                 *       / \    /  \               /   \
                 *      x   x  x    x             4     x
                 *                               / \
                 *                              x   x
                 *
                 * This code attaches nodes from the "bottom up" in the above illustration.
                 */
                BPLib_RBT_ConnectRightSafeImpl(parent, node->left);
                BPLib_RBT_ConnectLeftNonNullImpl(node, parent);
                BPLib_RBT_ConnectLeftNonNullImpl(grandparent, node);
            }
            else
            {
                /*
                 * Node is the "inner" grandchild on right, needs initial right rotation
                 *
                 *   Before:     16(Gb)         After:      16(Gb)
                 *              /     \                    /     \
                 *            8(Ub)     24(Pr)          8(Ub)      20(N'r)
                 *                    /    \                     /    \
                 *                   20(Nr)  28                 18     24(P'r)
                 *                  /  \    /  \                      /   \
                 *                 18  22  26  30                   22     28
                 *                                                        /  \
                 *                                                       26  30
                 *
                 * This code attaches nodes from the "bottom up" in the above illustration.
                 */
                BPLib_RBT_ConnectLeftSafeImpl(parent, node->right);
                BPLib_RBT_ConnectRightNonNullImpl(node, parent);
                BPLib_RBT_ConnectRightNonNullImpl(grandparent, node);
            }

            /* for both cases, node and parent are now switched, so it has become an outer grandchild */
            rebalance_case = insert_case_undefined;
            node           = parent;
        }
        else if (rebalance_case == insert_case_subject_is_outer_grandchild)
        {
            /* in both these scenarios, the parent becomes the grandparent, and
             * links must be adjusted accordingly at that level (unless GP is the root) */
            *(BPLib_RBT_FindParentRefImpl(tree, grandparent)) = parent;
            parent->parent                                    = grandparent->parent;

            if (parent_is_left_side)
            {
                /*
                 * Node is the "outer" grandchild on left, needs right rotation
                 *
                 * Example:
                 *
                 *   Before:       16(Gb)            After :       12(P'b)
                 *               /     \                         /     \
                 *           12(Pr)     24(Ub)                8(N'r)     16(G'r)
                 *          /    \                          /    \     /    \
                 *         8(Nr)  14                       4      10  14     24(U'b)
                 *       /   \                           /  \
                 *     4      10                        2    6
                 *    / \
                 *   2   6
                 */
                BPLib_RBT_ConnectLeftSafeImpl(grandparent, parent->right);
                BPLib_RBT_ConnectRightNonNullImpl(parent, grandparent);
            }
            else
            {
                /*
                 * Node is the "outer" grandchild on right, needs left rotation
                 *
                 * Example:
                 *
                 *   Before:     16(Gb)           After:            20(P'b)
                 *             /     \                            /     \
                 *          8(Ub)      20(Pr)                   16(G'r)   24(N'r)
                 *                   /    \                    /   \     /   \
                 *                  18     24(Nr)            8(U'b) 18  22    28
                 *                        /   \                              /  \
                 *                      22     28                           26  30
                 *                            /  \
                 *                           26  30
                 */
                BPLib_RBT_ConnectRightSafeImpl(grandparent, parent->left);
                BPLib_RBT_ConnectLeftNonNullImpl(parent, grandparent);
            }

            /* also in both cases, the parent becomes black, and the grandparent becomes red */
            BPLib_RBT_SwapColorImpl(parent, grandparent);
            rebalance_case = insert_case_no_action_needed;
        }
    }
}

/*--------------------------------------------------------------------------------------
 * Internal implementation routine, see declaration for detail
 *--------------------------------------------------------------------------------------*/
void BPLib_RBT_DeleteMakeLeafImpl(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *node)
{
    BPLib_RBT_Link_t *target;

    /*
     * Only a leaf node can be directly deleted, so if this is not a leaf node,
     * then we must find suitable node(s) to swap places with.  This may require
     * 2 iterations of this loop (and 2 swaps) to arrive at a leaf.
     *
     * Swapping nodes like this will NOT trigger a violation of the R-B
     * color constraints, because the color stays in the same spot (that is, the
     * swapped nodes also swap colors, keeping color arrangement the same, and
     * thereby maintaining all constraints).
     */
    while (true)
    {
        if (node->left != NULL)
        {
            /*
             * Find the successor to replace this node
             * Descendant is the local max of the left sub-tree.
             */
            target = node->left;
            while (target->right != NULL)
            {
                target = target->right;
            }
        }
        else if (node->right != NULL)
        {
            /*
             * Find the successor to replace this node
             * Descendant is the local min of the right sub-tree.
             *
             * NOTE: In this case the right is the only child, and thus must be red,
             * and furthermore it must not have any children, as that would violate
             * constraints.  Therefore a while() loop here is not necessary - the right
             * child is always a leaf node when left is null.
             */
            target = node->right;
        }
        else
        {
            /* arrived at a leaf node */
            break;
        }

        /*
         * NOTE: Regarding non-leaf nodes with only 1 child -
         * The insertion constraints guarantee that if we have a node with only
         * one non-NULL child, that node must be black, and the child must be red.
         *
         *  - No node (of any color) may have a single black non-NULL child, because that non-NULL
         *     side would  have a greater black height in its subtree than the NULL side would have,
         *     and this would be a violation of RB equal black height constraint.  So the "only-child"
         *     of any node must always be red.
         *
         *  - If the node itself were red, it cannot have a red child (other constraint of RB tree)
         *
         * So the target in this case is assumed to be red, and the subject/removed node
         * is assumed to be black.  The target is taking the place of the subject,
         * and it becomes black.  As a result, only a red node was effectively removed,
         * and thus the black depth remains the same.
         *
         * Additionally, that red child node must be a leaf, because if it had any children,
         * they must be black, and that would violate the black depth constraint.
         */

        /*
         * If one of these nodes is the direct parent/child of the other,
         * that represents a slightly different case for swapping
         */
        if (target->parent == node)
        {
            /* node1 is the direct parent of node2 */
            BPLib_RBT_SwapParentChildImpl(tree, node, target);
        }
        else
        {
            /* general swap, this should work for any other nodes that are not parent/child of each other */
            BPLib_RBT_SwapDistantImpl(tree, node, target);
        }

        /*
         * The swap performed above will have also swapped colors, because the entire nodes were switched,
         * and the node color was part of that.
         *
         * This additional color swap puts the original colors back into the original locations, meaning
         * that none of the R-B constraints will have been affected by this swap in the end.
         */
        BPLib_RBT_SwapColorImpl(node, target);
    }
}

/*--------------------------------------------------------------------------------------
 * Internal implementation routine, see declaration for detail
 *--------------------------------------------------------------------------------------*/
void BPLib_RBT_DeleteRebalanceImpl(BPLib_RBT_Root_t *tree, BPLib_RBT_Link_t *initial_subject)
{
    BPLib_RBT_Link_t *sibling;
    BPLib_RBT_Link_t *close_nephew;
    BPLib_RBT_Link_t *distant_nephew;
    BPLib_RBT_Link_t *parent;
    BPLib_RBT_Link_t *node;
    bool              node_is_left_side;
    enum
    {
        delete_case_undefined,        /**< invalid/undefined/unknown tree state */
        delete_case_no_action_needed, /**< No constraint violation exists, r-b conditions are satisfied  */

        /*
         * Below are the various possible sub-cases of black leaf deletion
         */
        delete_case_black_subject_is_root,
        delete_case_black_subject_with_red_sibling,
        delete_case_black_subject_with_red_distant_nephew,
        delete_case_black_subject_with_red_close_nephew,
        delete_case_black_subject_with_red_parent,
        delete_case_black_subject_with_all_black_relatives

    } rebalance_case;

    node              = initial_subject;
    node_is_left_side = false; /* set for real later */

    /* initial subject node must always be a leaf */
    BPLIB_RBT_REQUIRED_CONDITION(node->left == NULL);
    BPLIB_RBT_REQUIRED_CONDITION(node->right == NULL);

    if (BPLib_RBT_IsRedImpl(node))
    {
        /* red leaf - delete is trivial, does not affect black depth */
        rebalance_case = delete_case_no_action_needed;
    }
    else
    {
        /* black leaf - need to run the loop below to figure out what to do */
        rebalance_case = delete_case_undefined;
    }

    while (rebalance_case != delete_case_no_action_needed)
    {
        parent         = node->parent;
        sibling        = NULL;
        close_nephew   = NULL;
        distant_nephew = NULL;

        if (parent == NULL)
        {
            /* arrived at root, so black height balance is restored */
            rebalance_case = delete_case_black_subject_is_root;
        }
        else
        {
            /* get the sibling - if this was the left sibling, then get the right, or vice versa */
            node_is_left_side = (parent->left == node);

            if (node_is_left_side)
            {
                sibling        = parent->right;
                close_nephew   = sibling->left;
                distant_nephew = sibling->right;
            }
            else
            {
                sibling        = parent->left;
                close_nephew   = sibling->right;
                distant_nephew = sibling->left;
            }

            if (BPLib_RBT_IsRedImpl(sibling))
            {
                rebalance_case = delete_case_black_subject_with_red_sibling;
            }
            else if (BPLib_RBT_IsRedImpl(close_nephew))
            {
                rebalance_case = delete_case_black_subject_with_red_close_nephew;
            }
            else if (BPLib_RBT_IsRedImpl(distant_nephew))
            {
                rebalance_case = delete_case_black_subject_with_red_distant_nephew;
            }
            else if (BPLib_RBT_IsRedImpl(parent))
            {
                rebalance_case = delete_case_black_subject_with_red_parent;
            }
            else
            {
                rebalance_case = delete_case_black_subject_with_all_black_relatives;
            }
        }

        if (rebalance_case == delete_case_black_subject_is_root)
        {
            /*
             * this means we have traversed all the way from a leaf starting point to the
             * root, via the "black_leaf_with_all_black_relatives" case.  This means
             * the overall black height of the tree has been reduced.
             */
            --tree->black_height;
            rebalance_case = delete_case_no_action_needed;
        }
        else if (rebalance_case == delete_case_black_subject_with_all_black_relatives)
        {
            /*
             * black height of this subtree has been decreased, so this local subtree is OK,
             * but must move up to the parent and repeat the process, to check the other paths
             * which are _not_ through this subtree.
             */
            BPLib_RBT_SetRedImpl(sibling);
            node = parent; /* move up one level and repeat */
        }
        else if (rebalance_case == delete_case_black_subject_with_red_sibling ||
                 rebalance_case == delete_case_black_subject_with_red_distant_nephew)
        {
            /* In both of these cases a rotation is performed where sibling becomes parent.
             * The only difference is in how the nodes are repainted, and also in the case of
             * red sibling, the process needs to repeat because there still may be a violation. */
            *(BPLib_RBT_FindParentRefImpl(tree, parent)) = sibling;
            sibling->parent                              = parent->parent;

            if (node_is_left_side)
            {
                /*
                 * Node is the left child, needs initial left rotation
                 *
                 *   Before:     16(Pb)         After:          24(S'b)
                 *              /      \                      /        \
                 *            8(Nb)    24(Sr)             16(P'r)      28(D'b)
                 *           /  \      /    \             /     \      /    \
                 *          x    x   20(Cb) 28(Db)     8(N'b) 20(C'b) x      x
                 *                   /  \   /  \       /  \     /  \
                 *                  x    x x    x     x    x   x    x
                 *
                 * -OR-
                 *
                 *   Before:       16(P?)                  After:        24(S'?)
                 *                /     \                               /     \
                 *             8(Nb)      24(Sb)                    16(P'b)     28(D'b)
                 *            /  \       /   \                     /    \       /   \
                 *           x    x     x    28(Dr)           8(N'b)     x     x     x
                 *                            /   \           /  \
                 *                           x     x         x    x
                 */
                BPLib_RBT_ConnectRightSafeImpl(parent, close_nephew);
                BPLib_RBT_ConnectLeftNonNullImpl(sibling, parent);
            }
            else
            {
                /*
                 * Node is the right child, needs initial right rotation
                 *
                 *   Before:     16(Pb)            After:    8(S'b)
                 *              /      \                   /      \
                 *            8(Sr)    24(Nb)            4(D'b)   16(P'r)
                 *           /  \       /  \             /  \      /    \
                 *        4(Db) 12(Cb) x    x           x    x  12(C'b) 24(N'b)
                 *       /  \   /  \                            /  \    /  \
                 *      x    x x    x                          x    x  x    x
                 *
                 * -OR-
                 *
                 *    Before:      16(P?)          After:     8(S'?)
                 *                /     \                    /     \
                 *            8(Sb)     24(Nb)           4(D'b)      16(P'b)
                 *           /    \      /  \            /  \       /     \
                 *         4(Dr)   x    x    x          x    x     x      24(N'b)
                 *        /   \                                           /  \
                 *       x     x                                         x    x
                 */
                BPLib_RBT_ConnectLeftSafeImpl(parent, close_nephew);
                BPLib_RBT_ConnectRightNonNullImpl(sibling, parent);
            }
            BPLib_RBT_SwapColorImpl(parent, sibling);
            if (rebalance_case == delete_case_black_subject_with_red_distant_nephew)
            {
                BPLib_RBT_SetBlackImpl(distant_nephew);
                rebalance_case = delete_case_no_action_needed; /* once node is removed, this will be OK */
            }
        }
        else if (rebalance_case == delete_case_black_subject_with_red_parent)
        {
            BPLib_RBT_SetBlackImpl(parent);
            BPLib_RBT_SetRedImpl(sibling);
            rebalance_case = delete_case_no_action_needed; /* once node is removed, this will be OK */
        }
        else if (rebalance_case == delete_case_black_subject_with_red_close_nephew)
        {
            if (node_is_left_side)
            {
                /*
                 *   Before:    16(P?)             After:        16(P?)
                 *             /     \                          /     \
                 *          8(Nb)      20(Sb)                8(Nb)      18(C'b)
                 *         /  \       /     \               /  \       /   \
                 *        x    x   18(Cr)    24(Db)        x    x     x     20(S'r)
                 *                 /  \      /  \                           /   \
                 *                x    x    x    x                         x    24(D'b)
                 *                                                              /  \
                 *                                                             x    x
                 */
                BPLib_RBT_ConnectLeftSafeImpl(sibling, close_nephew->right);
                BPLib_RBT_ConnectRightNonNullImpl(close_nephew, sibling);
                BPLib_RBT_ConnectRightNonNullImpl(parent, close_nephew);
            }
            else
            {
                /*
                 * Before:       16(P?)               After:       16(P?)
                 *              /     \                           /     \
                 *           8(Sb)      20(Nb)               12(C'b)     20(Nb)
                 *         /    \       /   \                /    \      /  \
                 *     4(Db)    12(Cr) x     x             8(S'r)  x    x    x
                 *    /  \      /  \                      /   \
                 *   x    x    x    x                  4(D'b)  x
                 *                                    /  \
                 *                                   x    x
                 */
                BPLib_RBT_ConnectRightSafeImpl(sibling, close_nephew->left);
                BPLib_RBT_ConnectLeftNonNullImpl(close_nephew, sibling);
                BPLib_RBT_ConnectLeftNonNullImpl(parent, close_nephew);
            }
            BPLib_RBT_SwapColorImpl(sibling, close_nephew);
        }
    }
}
