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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <assert.h>

#include "bplib.h"
#include "bplib_os.h"
#include "bplib_mem_rbtree.h"

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
#define BPLIB_RBT_COLOR_MASK (~((bp_val_t)BPLIB_RBT_RED_FLAG))

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * set_black -
 *
 * node: A ptr to an BPLib_MEM_RBT_Link_t to set the color to black. [OUTPUT]
 *-------------------------------------------------------------------------------------*/
static inline void set_black(BPLib_MEM_RBT_Link_t *node)
{
    node->key_value_and_color &= BPLIB_RBT_COLOR_MASK;
}

/*--------------------------------------------------------------------------------------
 * set_red -
 *
 * node: A ptr to an BPLib_MEM_RBT_Link_t to set the color to red. [OUTPUT]
 *-------------------------------------------------------------------------------------*/
static inline void set_red(BPLib_MEM_RBT_Link_t *node)
{
    node->key_value_and_color |= BPLIB_RBT_RED_FLAG;
}

/*--------------------------------------------------------------------------------------
 * swap_colors - node2 gets the color of node1, and node1 gets the color of node2
 *-------------------------------------------------------------------------------------*/
static inline void swap_colors(BPLib_MEM_RBT_Link_t *node1, BPLib_MEM_RBT_Link_t *node2)
{
    node1->key_value_and_color ^= node2->key_value_and_color & BPLIB_RBT_RED_FLAG;
    node2->key_value_and_color ^= node1->key_value_and_color & BPLIB_RBT_RED_FLAG;
    node1->key_value_and_color ^= node2->key_value_and_color & BPLIB_RBT_RED_FLAG;
}

/*--------------------------------------------------------------------------------------
 * initialize_node_value - initially configure the node with the given value
 *
 * Color will be set to RED (default for all new nodes)
 *-------------------------------------------------------------------------------------*/
static inline void initialize_node_value(BPLib_MEM_RBT_Link_t *node, bp_val_t value)
{
    node->key_value_and_color = BPLIB_RBT_RED_FLAG | (value << 1);
}

/*--------------------------------------------------------------------------------------
 * get_key_value - gets the key value from the given node
 *-------------------------------------------------------------------------------------*/
static inline bp_val_t get_key_value(const BPLib_MEM_RBT_Link_t *node)
{
    return (node->key_value_and_color >> 1);
}

/**
 * @brief Checks if a node is red
 *
 * This is intended to be used where node is already known to be non-null
 *
 * @param node Pointer to the node to check
 * @return true if node is red
 * @return false if node is black
 */
static inline bool is_red_nonnull(const BPLib_MEM_RBT_Link_t *node)
{
    return (node->key_value_and_color & BPLIB_RBT_RED_FLAG) != 0;
}

/**
 * @brief Checks if a node is black
 *
 * This is intended to be used where node is already known to be non-null
 *
 * @param node Pointer to the node to check
 * @return true if node is black
 * @return false if node is red
 */
static inline bool is_black_nonnull(const BPLib_MEM_RBT_Link_t *node)
{
    return (node->key_value_and_color & BPLIB_RBT_RED_FLAG) == 0;
}

/*--------------------------------------------------------------------------------------
 * is_black -
 *
 * this checks the color for logical/algorithmic purposes, where a NIL/NULL node
 * may be passed in, and is considered black.
 *
 * node: A BPLib_MEM_RBT_Link_t to check its color. [INPUT]
 * returns: Whether or not the provided BPLib_MEM_RBT_Link_t is logically black.
 *-------------------------------------------------------------------------------------*/
static inline bool is_black(const BPLib_MEM_RBT_Link_t *node)
{
    return node == NULL || is_black_nonnull(node);
}

/*--------------------------------------------------------------------------------------
 * is_red -
 *
 * this checks the color for logical/algorithmic purposes, where a NIL/NULL node
 * may be passed in, and is considered black.
 *
 * node: A BPLib_MEM_RBT_Link_t to check its color. [INPUT]
 * returns: Whether or not the provided BPLib_MEM_RBT_Link_t is red.
 *-------------------------------------------------------------------------------------*/
static inline bool is_red(const BPLib_MEM_RBT_Link_t *node)
{
    return node != NULL && is_red_nonnull(node);
}

/**
 * @brief Connects a parent and child node
 *
 * Node c becomes the left child of p
 * This is intended to be used where both c and p are already known to be non-null
 *
 * @param p The parent node, must not be NULL
 * @param c The child node, must not be NULL
 */
static inline void connect_left_child_nonnull(BPLib_MEM_RBT_Link_t *p, BPLib_MEM_RBT_Link_t *c)
{
    p->left   = c;
    c->parent = p;
}

/**
 * @brief Connects a parent and child node
 *
 * Node c becomes the right child of p
 * This is intended to be used where both c and p are already known to be non-null
 *
 * @param p The parent node, must not be NULL
 * @param c The child node, must not be NULL
 */
static inline void connect_right_child_nonnull(BPLib_MEM_RBT_Link_t *p, BPLib_MEM_RBT_Link_t *c)
{
    p->right  = c;
    c->parent = p;
}

/**
 * @brief Connects a parent and child node
 *
 * Node c becomes the left child of p
 * This is intended to be used where c has not been checked and might be NULL
 *
 * @param p The parent node, must not be NULL
 * @param c The child node, may be NULL
 */
static inline void connect_left_child_maybe_null(BPLib_MEM_RBT_Link_t *p, BPLib_MEM_RBT_Link_t *c)
{
    p->left = c;
    if (c)
    {
        c->parent = p;
    }
}

/**
 * @brief Connects a parent and child node
 *
 * Node c becomes the right child of p
 * This is intended to be used where c has not been checked and might be NULL
 *
 * @param p The parent node, must not be NULL
 * @param c The child node, may be NULL
 */
static inline void connect_right_child_maybe_null(BPLib_MEM_RBT_Link_t *p, BPLib_MEM_RBT_Link_t *c)
{
    p->right = c;
    if (c)
    {
        c->parent = p;
    }
}

/*--------------------------------------------------------------------------------------
 * node_is_attached - Checks if the given node is attached in a tree
 *
 * NOTE - this is not quite sufficient of a check, as a tree with a single node
 * will also have NULL pointers.  As a minimum one should also check if the node is
 * the root of the tree in addition to this test.
 *
 * node: Memory block to test
 * returns: true if the node is attached to a tree, false otherwise
 *--------------------------------------------------------------------------------------*/
static inline bool node_is_attached(const BPLib_MEM_RBT_Link_t *node)
{
    /* if any pointer is set, this is in a tree */
    return (node->parent != NULL || node->left != NULL || node->right != NULL);
}

/**
 * @brief Finds the pointer in the parent node that refers back to this node
 *
 * Either the left or right pointer of the parent should point back to this node.
 * This returns a pointer to that self-reference.  In the event that the node is
 * the root node, then a ref to the tree root is returned.  As such, this never
 * returns a NULL ref.
 *
 * @param node The node to find the parent reference to
 * @return Pointer to the reference in the parent pointing to node
 */
BP_LOCAL_SCOPE BPLib_MEM_RBT_Link_t **find_parent_ref(BPLib_MEM_RBT_Root_t *tree, BPLib_MEM_RBT_Link_t *node)
{
    BPLib_MEM_RBT_Link_t **ref;

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
    assert(*ref == node);
    return ref;
}

/**
 * @brief Swap two nodes that are known to have a parent/child relationship
 *
 * This may change the root location of the tree, if the parent is the current root
 *
 * @param tree   Pointer to the entire tree
 * @param parent Pointer to the current parent which should become child
 * @param child  Pointer to the current child which should become parent
 */
BP_LOCAL_SCOPE void swap_parent_and_child(BPLib_MEM_RBT_Root_t *tree, BPLib_MEM_RBT_Link_t *parent, BPLib_MEM_RBT_Link_t *child)
{
    BPLib_MEM_RBT_Link_t **grandparent_ref;
    BPLib_MEM_RBT_Link_t  *saved_left;
    BPLib_MEM_RBT_Link_t  *saved_right;

    /* temporarily save off the left/right from the original parent (one of these is the subject child) */
    saved_left  = parent->left;
    saved_right = parent->right;

    /* The grandparent should now refer to the child */
    grandparent_ref  = find_parent_ref(tree, parent);
    child->parent    = parent->parent;
    *grandparent_ref = child;

    /* both of the original grandchildren (if present) will now become the parent's immediate children */
    connect_left_child_maybe_null(parent, child->left);
    connect_right_child_maybe_null(parent, child->right);

    if (saved_left == child)
    {
        /* original child was on the left side of original parent */
        /* reattach original sibling now as right child of new parent */
        connect_right_child_maybe_null(child, saved_right);
        connect_left_child_nonnull(child, parent);
    }
    else
    {
        /* original child was on the right side of original parent */
        /* reattach original sibling now as left child of new parent */
        connect_left_child_maybe_null(child, saved_left);
        connect_right_child_nonnull(child, parent);
    }
}

/**
 * @brief Swap two nodes that are known not to have a parent/child relationship
 *
 * This may change the root location of the tree, if one of the nodes is the current root
 *
 * @param tree   Pointer to the entire tree
 * @param node1  Pointer to the current parent which should become child
 * @param node2  Pointer to the current child which should become parent
 */
BP_LOCAL_SCOPE void swap_distant_nodes(BPLib_MEM_RBT_Root_t *tree, BPLib_MEM_RBT_Link_t *node1, BPLib_MEM_RBT_Link_t *node2)
{
    BPLib_MEM_RBT_Link_t *saved_left;
    BPLib_MEM_RBT_Link_t *saved_right;
    BPLib_MEM_RBT_Link_t *saved_parent;

    /* switch parent pointers */
    *(find_parent_ref(tree, node1)) = node2;
    *(find_parent_ref(tree, node2)) = node1;
    saved_parent                    = node1->parent;
    node1->parent                   = node2->parent;
    node2->parent                   = saved_parent;

    /* temporarily save off the left/right from the original nodes */
    saved_left  = node1->left;
    saved_right = node1->right;
    connect_left_child_maybe_null(node1, node2->left);
    connect_right_child_maybe_null(node1, node2->right);
    connect_left_child_maybe_null(node2, saved_left);
    connect_right_child_maybe_null(node2, saved_right);
}

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_RBT_Search - Searches a rb_tree for a node containing a given value.
 *
 * tree: A ptr to a BPLib_MEM_RBT_Root_t to search. [INPUT]
 * value: The value to search for within the BPLib_MEM_RBT_Root_t. [INPUT]
 * returns: A ptr to a BPLib_MEM_RBT_Link_t to populate with the identified node. This is set to NULL
 *      if no node is found.
 *--------------------------------------------------------------------------------------*/
static inline int BPLib_MEM_RBT_CompareKey(bp_val_t key1, bp_val_t key2)
{
    if (key1 == key2)
    {
        return 0;
    }
    else if (key1 > key2)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

/*--------------------------------------------------------------------------------------
 * do_insert_as_leaf - Does a trivial insert of a new node into the rb tree.
 *
 * This traverses the tree to find the correct insertion point, which will always arrive at
 * a leaf node if no duplicates exist.  If a duplicate does exist, this is an error.
 *
 * The new node is always initially painted red, so this insert will never affect the black depth,
 * but may cause a red-red violation to exist if the parent was red.  This can be corrected later.
 *
 * insert_key_value: The value of the node to attempt to insert or merge into the red black tree. [INPUT]
 * tree: A ptr to the BPLib_MEM_RBT_Root_t to insert values into. [OUTPUT]
 * new_node: Pointer to the new node link structure
 * returns: A rb_tree status indicating the result of the insertion attempt (BP_DUPLICATE or BP_SUCCESS)
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int do_insert_as_leaf(BPLib_MEM_RBT_Root_t *tree, BPLib_MEM_RBT_Link_t *new_node,
                                     BPLib_MEM_RBT_CompareFunc_t compare_func, void *compare_arg)
{
    BPLib_MEM_RBT_Link_t  *curr_ptr;
    BPLib_MEM_RBT_Link_t **ref_ptr;
    BPLib_MEM_RBT_Link_t  *parent_ptr;
    bp_val_t           insert_key_value;
    int                status;
    int                compare_result;

    status           = BP_ERROR;
    parent_ptr       = NULL;
    insert_key_value = get_key_value(new_node);
    ref_ptr          = &tree->root;
    while (true)
    {
        curr_ptr = *ref_ptr;
        if (curr_ptr == NULL)
        {
            /* got to a leaf without a duplicate - so insert it here */
            new_node->parent = parent_ptr;
            *ref_ptr         = new_node;
            status           = BP_SUCCESS;
            break;
        }

        compare_result = BPLib_MEM_RBT_CompareKey(insert_key_value, get_key_value(curr_ptr));
        if (compare_result == 0 && compare_func != NULL)
        {
            compare_result = compare_func(curr_ptr, compare_arg);
        }

        if (compare_result == 0)
        {
            /* duplicate entries are not allowed */
            status = BP_DUPLICATE;
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
 * do_insert_rebalance - Ensure that the red black tree adheres to all constraints
 *
 * This rebalances the nodes if required.  This process should be executed after every
 * insertion to ensure constraints are always adhered to.
 *
 * NOTE: rotations may change the location of the logical root of the tree.
 *
 * tree: pointer to the tree (required because this may change the logical root)
 * initial_subject: A ptr to the last inserted node in the red black tree. Must be red (initial color for new nodes)
 *
 *--------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE void do_insert_rebalance(BPLib_MEM_RBT_Root_t *tree, BPLib_MEM_RBT_Link_t *initial_subject)
{
    BPLib_MEM_RBT_Link_t *grandparent;
    BPLib_MEM_RBT_Link_t *parent;
    BPLib_MEM_RBT_Link_t *uncle;
    BPLib_MEM_RBT_Link_t *node;
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
    assert(node->left == NULL);
    assert(node->right == NULL);

    while (rebalance_case != insert_case_no_action_needed)
    {
        /* At the start of this loop, the reference node (subject) must always be red */
        assert(is_red(node));

        /*
         * If not yet known, determine what node arrangement we have,
         * categorize into one of the rebalance cases above.
         */
        parent_is_left_side = false;
        parent              = node->parent;
        grandparent         = NULL;
        uncle               = NULL;
        if (is_black(parent))
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

                if (is_red(uncle))
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
            set_black(parent);
            rebalance_case = insert_case_no_action_needed;
            ++tree->black_height;
        }
        else if (rebalance_case == insert_case_subject_with_red_parent_and_uncle)
        {
            /* Both parent and uncle should be made black. */
            set_black(parent);
            set_black(uncle);
            set_red(grandparent);

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
                connect_right_child_maybe_null(parent, node->left);
                connect_left_child_nonnull(node, parent);
                connect_left_child_nonnull(grandparent, node);
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
                connect_left_child_maybe_null(parent, node->right);
                connect_right_child_nonnull(node, parent);
                connect_right_child_nonnull(grandparent, node);
            }

            /* for both cases, node and parent are now switched, so it has become an outer grandchild */
            rebalance_case = insert_case_undefined;
            node           = parent;
        }
        else if (rebalance_case == insert_case_subject_is_outer_grandchild)
        {
            /* in both these scenarios, the parent becomes the grandparent, and
             * links must be adjusted accordingly at that level (unless GP is the root) */
            *(find_parent_ref(tree, grandparent)) = parent;
            parent->parent                        = grandparent->parent;

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
                connect_left_child_maybe_null(grandparent, parent->right);
                connect_right_child_nonnull(parent, grandparent);
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
                connect_right_child_maybe_null(grandparent, parent->left);
                connect_left_child_nonnull(parent, grandparent);
            }

            /* also in both cases, the parent becomes black, and the grandparent becomes red */
            swap_colors(parent, grandparent);
            rebalance_case = insert_case_no_action_needed;
        }
    }
}

/*--------------------------------------------------------------------------------------
 * do_delete_make_leaf - Forces the specified node into a leaf position
 *
 * The given node is made to be a leaf by swapping positions with other descendant nodes,
 * until arriving at a leaf position.  This is a dependency for eventual node removal.
 *
 * NOTE: after this operation, the  specified node will no longer be in the
 * correct location in the tree, according to its key value.  As the intent is
 * to remove the node, this (temporary) invalid configuration is acceptable.
 *
 *-------------------------------------------------------------------------------------*/
void do_delete_make_leaf(BPLib_MEM_RBT_Root_t *tree, BPLib_MEM_RBT_Link_t *node)
{
    BPLib_MEM_RBT_Link_t *target;

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
            swap_parent_and_child(tree, node, target);
        }
        else
        {
            /* general swap, this should work for any other nodes that are not parent/child of each other */
            swap_distant_nodes(tree, node, target);
        }

        /*
         * The swap performed above will have also swapped colors, because the entire nodes were switched,
         * and the node color was part of that.
         *
         * This additional color swap puts the original colors back into the original locations, meaning
         * that none of the R-B constraints will have been affected by this swap in the end.
         */
        swap_colors(node, target);
    }
}

/*--------------------------------------------------------------------------------------
 * do_delete_rebalance - Rebalance the BPLib_MEM_RBT_Root_t to account for the deletion of a leaf node
 *
 * This rebalances the R-B tree to account for the deletion of a node, which must be a leaf
 * node.  The function do_delete_make_leaf() should be used first, to force the node into
 * a leaf position.
 *
 * tree: A ptr to the BPLib_MEM_RBT_Root_t to rebalance.
 * initial_subject: A ptr to the BPLib_MEM_RBT_Link_t from which to start the rebalancing operation. [INPUT]
 *
 *--------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE void do_delete_rebalance(BPLib_MEM_RBT_Root_t *tree, BPLib_MEM_RBT_Link_t *initial_subject)
{
    BPLib_MEM_RBT_Link_t *sibling;
    BPLib_MEM_RBT_Link_t *close_nephew;
    BPLib_MEM_RBT_Link_t *distant_nephew;
    BPLib_MEM_RBT_Link_t *parent;
    BPLib_MEM_RBT_Link_t *node;
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
    assert(node->left == NULL);
    assert(node->right == NULL);

    if (is_red(node))
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

            if (is_red(sibling))
            {
                rebalance_case = delete_case_black_subject_with_red_sibling;
            }
            else if (is_red(close_nephew))
            {
                rebalance_case = delete_case_black_subject_with_red_close_nephew;
            }
            else if (is_red(distant_nephew))
            {
                rebalance_case = delete_case_black_subject_with_red_distant_nephew;
            }
            else if (is_red(parent))
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
            set_red(sibling);
            node = parent; /* move up one level and repeat */
        }
        else if (rebalance_case == delete_case_black_subject_with_red_sibling ||
                 rebalance_case == delete_case_black_subject_with_red_distant_nephew)
        {
            /* In both of these cases a rotation is performed where sibling becomes parent.
             * The only difference is in how the nodes are repainted, and also in the case of
             * red sibling, the process needs to repeat because there still may be a violation. */
            *(find_parent_ref(tree, parent)) = sibling;
            sibling->parent                  = parent->parent;

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
                connect_right_child_maybe_null(parent, close_nephew);
                connect_left_child_nonnull(sibling, parent);
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
                connect_left_child_maybe_null(parent, close_nephew);
                connect_right_child_nonnull(sibling, parent);
            }
            swap_colors(parent, sibling);
            if (rebalance_case == delete_case_black_subject_with_red_distant_nephew)
            {
                set_black(distant_nephew);
                rebalance_case = delete_case_no_action_needed; /* once node is removed, this will be OK */
            }
        }
        else if (rebalance_case == delete_case_black_subject_with_red_parent)
        {
            set_black(parent);
            set_red(sibling);
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
                connect_left_child_maybe_null(sibling, close_nephew->right);
                connect_right_child_nonnull(close_nephew, sibling);
                connect_right_child_nonnull(parent, close_nephew);
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
                connect_right_child_maybe_null(sibling, close_nephew->left);
                connect_left_child_nonnull(close_nephew, sibling);
                connect_left_child_nonnull(parent, close_nephew);
            }
            swap_colors(sibling, close_nephew);
        }
    }
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

void BPLib_MEM_RBT_InitRoot(BPLib_MEM_RBT_Root_t *tree)
{
    memset(tree, 0, sizeof(*tree));
}

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_RBT_TreeIsEmpty - Checks if the given tree is empty
 *
 * tree: A ptr to a BPLib_MEM_RBT_Root_t
 * returns: true if the tree is empty
 *--------------------------------------------------------------------------------------*/
bool BPLib_MEM_RBT_TreeIsEmpty(const BPLib_MEM_RBT_Root_t *tree)
{
    return (tree->root == NULL);
}

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_RBT_NodeIsMember - Checks if the given node is a member of the tree
 *
 * tree: A ptr to a BPLib_MEM_RBT_Root_t
 * node: Memory block to test
 * returns: true if the node is a member of the tree, false otherwise
 *--------------------------------------------------------------------------------------*/
bool BPLib_MEM_RBT_NodeIsMember(const BPLib_MEM_RBT_Root_t *tree, const BPLib_MEM_RBT_Link_t *node)
{
    const BPLib_MEM_RBT_Link_t *root_node;

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
 * BPLib_MEM_RBT_Search - Searches a rb_tree for a node containing a given value.
 *
 * tree: A ptr to a BPLib_MEM_RBT_Root_t to search. [INPUT]
 * value: The value to search for within the BPLib_MEM_RBT_Root_t. [INPUT]
 * returns: A ptr to a BPLib_MEM_RBT_Link_t to populate with the identified node. This is set to NULL
 *      if no node is found.
 *--------------------------------------------------------------------------------------*/
BPLib_MEM_RBT_Link_t *BPLib_MEM_RBT_SearchGeneric(bp_val_t search_key_value, const BPLib_MEM_RBT_Root_t *tree,
                                           BPLib_MEM_RBT_CompareFunc_t compare_func, void *compare_arg)
{
    BPLib_MEM_RBT_Link_t *curr_ptr;
    int               compare_result;

    curr_ptr = tree->root;
    while (curr_ptr != NULL)
    {
        compare_result = BPLib_MEM_RBT_CompareKey(search_key_value, get_key_value(curr_ptr));
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
 * BPLib_MEM_RBT_InsertValue - Inserts a value into the red black tree and rebalances it accordingly.
 *
 * value - The value to insert into the BPLib_MEM_RBT_Root_t. [INPUT]
 * tree: A ptr to a BPLib_MEM_RBT_Root_t to insert the value into. [OUTPUT]
 * new_node: Memory block for storage of the new value [INPUT]
 * returns: Status code indicating the result of the insertion.
 *--------------------------------------------------------------------------------------*/
int BPLib_MEM_RBT_InsertValueGeneric(bp_val_t insert_key_value, BPLib_MEM_RBT_Root_t *tree, BPLib_MEM_RBT_Link_t *link_block,
                                   BPLib_MEM_RBT_CompareFunc_t compare_func, void *compare_arg)
{
    int status;

    if (tree->root == link_block || node_is_attached(link_block))
    {
        return BP_ERROR;
    }

    /*
     * The new node will always be connected as a leaf, so assume its child
     * pointers are both NULL.
     *
     * This sets the key value and also paints it red, which is the initial
     * condition for insert because it does not affect the count of black nodes
     * from the root to the leaf.
     */
    initialize_node_value(link_block, insert_key_value);

    status = do_insert_as_leaf(tree, link_block, compare_func, compare_arg);
    if (status == BP_SUCCESS)
    {
        /* Correct any violations within the red black tree due to the insertion. */
        do_insert_rebalance(tree, link_block);
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
 * BPLib_MEM_RBT_ExtractNode - Removes a single node from the red black tree and rebalances it accordingly.
 *
 * tree: A ptr to a BPLib_MEM_RBT_Root_t to delete value from. [OUTPUT]
 * link_block: Memory block that previously stored the value
 * returns: Status code indicating the result of the deletion.
 *--------------------------------------------------------------------------------------*/
int BPLib_MEM_RBT_ExtractNode(BPLib_MEM_RBT_Root_t *tree, BPLib_MEM_RBT_Link_t *link_block)
{
    if (tree->root != link_block && !node_is_attached(link_block))
    {
        return BP_ERROR;
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
    do_delete_make_leaf(tree, link_block);

    /*
     * Rebalance the tree anticipating the removal of the subject leaf
     */
    do_delete_rebalance(tree, link_block);

    /*
     * Do the actual disconnect
     */
    *(find_parent_ref(tree, link_block)) = NULL;

    /* reset this entire link block - this also clears the node value and resets it to 0 */
    memset(link_block, 0, sizeof(*link_block));

    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_RBT_GetKeyValue
 *
 * returns: Value of the given node
 *--------------------------------------------------------------------------------------*/
bp_val_t BPLib_MEM_RBT_GetKeyValue(const BPLib_MEM_RBT_Link_t *node)
{
    return get_key_value(node);
}

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_RBT_NodeIsRed (externally exposed for debug/testing purposes)
 *
 * returns: True if the given node is red, false if black
 *--------------------------------------------------------------------------------------*/
bool BPLib_MEM_RBT_NodeIsRed(const BPLib_MEM_RBT_Link_t *node)
{
    return is_red(node);
}

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_RBT_IterNext
 *
 * Move to the next node in an iterator
 *--------------------------------------------------------------------------------------*/
int BPLib_MEM_RBT_IterNext(BPLib_MEM_RBT_Iter_t *iter)
{
    const BPLib_MEM_RBT_Link_t *next_pos;

    next_pos = iter->position;
    if (next_pos != NULL)
    {
        if (next_pos->right != NULL)
        {
            /* Descend the right-side tree at this time. */
            next_pos = next_pos->right;
            while (next_pos->left != NULL)
            {
                next_pos = next_pos->left;
            }
        }
        else
        {
            while (true)
            {
                /* Ascend to the parent, until arriving at an unvisited node */
                next_pos = next_pos->parent;

                /* if returning from the left-side child, visit this node now.
                 * otherwise, this is returning from the right side child, so keep going up. */
                if (next_pos == NULL || next_pos->left == iter->position)
                {
                    break;
                }
                iter->position = next_pos;
            }
        }
    }

    iter->position = next_pos;
    if (next_pos == NULL)
    {
        /* reached end of tree */
        return BP_ERROR;
    }

    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_RBT_IterPrev
 *
 * Move to the previous node in an iterator
 *--------------------------------------------------------------------------------------*/
int BPLib_MEM_RBT_IterPrev(BPLib_MEM_RBT_Iter_t *iter)
{
    const BPLib_MEM_RBT_Link_t *prev_pos;

    prev_pos = iter->position;
    if (prev_pos != NULL)
    {
        if (prev_pos->left != NULL)
        {
            /* Descend the left-side tree at this time. */
            prev_pos = prev_pos->left;
            while (prev_pos->right != NULL)
            {
                prev_pos = prev_pos->right;
            }
        }
        else
        {
            while (true)
            {
                /* Ascend to the parent, until arriving at an unvisited node */
                prev_pos = prev_pos->parent;

                /* if returning from the right-side child, visit this node now.
                 * otherwise, this is returning from the left side child, so keep going up. */
                if (prev_pos == NULL || prev_pos->right == iter->position)
                {
                    break;
                }
                iter->position = prev_pos;
            }
        }
    }

    iter->position = prev_pos;
    if (prev_pos == NULL)
    {
        /* reached end of tree */
        return BP_ERROR;
    }

    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_RBT_IterFindClosest
 *
 * Find a node that is either exactly matching the target value, or if that does not exist,
 * returns the nearest leaf node where that value would be attached.
 *--------------------------------------------------------------------------------------*/
const BPLib_MEM_RBT_Link_t *BPLib_MEM_RBT_IterFindClosest(bp_val_t target_value, const BPLib_MEM_RBT_Link_t *start_pos)
{
    const BPLib_MEM_RBT_Link_t *prev_pos;
    const BPLib_MEM_RBT_Link_t *curr_pos;
    bp_val_t                curr_val;

    prev_pos = NULL;
    curr_pos = start_pos;
    while (true)
    {
        if (curr_pos == NULL)
        {
            /* end of tree - go back to last valid node */
            curr_pos = prev_pos;
            break;
        }

        curr_val = get_key_value(curr_pos);
        if (curr_val == target_value)
        {
            /* exact match */
            break;
        }

        prev_pos = curr_pos;
        if (curr_val > target_value)
        {
            curr_pos = curr_pos->left;
        }
        else
        {
            curr_pos = curr_pos->right;
        }
    }

    return curr_pos;
}

int BPLib_MEM_RBT_IterGotoMin(bp_val_t minimum_value, const BPLib_MEM_RBT_Root_t *tree, BPLib_MEM_RBT_Iter_t *iter)
{
    bp_val_t curr_val;
    int      status;

    iter->position = BPLib_MEM_RBT_IterFindClosest(minimum_value, tree->root);
    if (iter->position == NULL)
    {
        /* the tree is empty */
        status = BP_ERROR;
    }
    else
    {
        /*
         * If duplicates are allowed in this tree, then this may not be the only match.
         * So if this is not a leaf node then go left and repeat the search until arriving
         * at a node that has no left child.
         */
        while (iter->position->left != NULL)
        {
            iter->position = BPLib_MEM_RBT_IterFindClosest(minimum_value, iter->position->left);
        }

        curr_val = get_key_value(iter->position);
        if (curr_val >= minimum_value)
        {
            status = BP_SUCCESS;
        }
        else
        {
            /* the next node in normal iteration may be greater than the minimum */
            status = BPLib_MEM_RBT_IterNext(iter);
        }
    }

    return status;
}

int BPLib_MEM_RBT_IterGotoMax(bp_val_t maximum_value, const BPLib_MEM_RBT_Root_t *tree, BPLib_MEM_RBT_Iter_t *iter)
{
    bp_val_t curr_val;
    int      status;

    iter->position = BPLib_MEM_RBT_IterFindClosest(maximum_value, tree->root);
    if (iter->position == NULL)
    {
        /* the tree is empty */
        status = BP_ERROR;
    }
    else
    {
        /*
         * If duplicates are allowed in this tree, then this may not be the only match.
         * So if this is not a leaf node then go right and repeat the search until arriving
         * at a node that has no right child.
         */
        while (iter->position->right != NULL)
        {
            iter->position = BPLib_MEM_RBT_IterFindClosest(maximum_value, iter->position->right);
        }

        curr_val = get_key_value(iter->position);
        if (curr_val <= maximum_value)
        {
            status = BP_SUCCESS;
        }
        else
        {
            /* the next node in reverse iteration may be less than the maximum */
            status = BPLib_MEM_RBT_IterPrev(iter);
        }
    }

    return status;
}
