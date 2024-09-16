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

#include <stdlib.h>

#include "test_bplib_mem_common.h"
#include "bplib_rbtree.h"

/************************************************************************
 *
 *  Test program for Red-Black tree implementation
 *
 *  This is intended to confirm the operation of the basic R-B algorithm
 *
 *************************************************************************/

#define TEST_SIZE 200

typedef struct rbtest_node
{
    bplib_rbt_link_t    link;
    int                 internal_value;
    struct rbtest_node *next_seq;
} rbtest_node_t;

bplib_rbt_root_t rbtree;

rbtest_node_t *node_array;

rbtest_node_t *head_free;
rbtest_node_t *tail_free;
rbtest_node_t *head_used;
rbtest_node_t *tail_used;

static int rbtest_comparator(const bplib_rbt_link_t *link, void *arg)
{
    rbtest_node_t *node;
    int            ref_val;

    node    = (rbtest_node_t *)link;
    ref_val = *((int *)arg);

    return (ref_val - node->internal_value);
}

void TestBplibMemCommon_RBT_Setup(void)
{
    int i;

    UtPrintf("Allocating memory for %d nodes\n", TEST_SIZE);
    node_array = malloc(sizeof(*node_array) * TEST_SIZE);
    if (!UtAssert_NOT_NULL(node_array))
    {
        UtAssert_Abort("malloc failure");
    }

    memset(node_array, 0, sizeof(*node_array) * TEST_SIZE);

    node_array[0].next_seq = NULL;
    for (i = 1; i < TEST_SIZE; ++i)
    {
        node_array[i].internal_value = i;
        node_array[i].next_seq       = &node_array[i - 1];
    }

    tail_free = &node_array[0];
    head_free = &node_array[TEST_SIZE - 1];
    head_used = NULL;
    tail_used = NULL;
}

void TestBplibMemCommon_RBT_Basics(void)
{
    bplib_rbt_root_t tree;

    memset(&tree, 0xEE, sizeof(tree));
    UtAssert_VOIDCALL(bplib_rbt_init_root(&tree));
    UtAssert_ZERO(tree.black_height);
    UtAssert_NULL(tree.root);
    UtAssert_BOOL_TRUE(bplib_rbt_tree_is_empty(&tree));
    UtAssert_BOOL_FALSE(bplib_rbt_node_is_member(&tree, &node_array[0].link));
    UtAssert_BOOL_FALSE(bplib_rbt_node_is_member(&tree, &node_array[1].link));

    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(0, &tree, &node_array[0].link), BP_SUCCESS);

    UtAssert_UINT32_EQ(bplib_rbt_get_key_value(&node_array[0].link), 0);
    UtAssert_BOOL_FALSE(bplib_rbt_tree_is_empty(&tree));
    UtAssert_BOOL_TRUE(bplib_rbt_node_is_member(&tree, &node_array[0].link));
    UtAssert_BOOL_TRUE(bplib_rbt_node_is_red(&node_array[0].link));
    UtAssert_BOOL_FALSE(bplib_rbt_node_is_member(&tree, &node_array[1].link));

    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(1, &tree, &node_array[1].link), BP_SUCCESS);
    UtAssert_UINT32_EQ(bplib_rbt_get_key_value(&node_array[1].link), 1);
    UtAssert_BOOL_FALSE(bplib_rbt_tree_is_empty(&tree));
    UtAssert_BOOL_FALSE(bplib_rbt_node_is_red(&node_array[0].link));
    UtAssert_BOOL_TRUE(bplib_rbt_node_is_red(&node_array[1].link));
    UtAssert_BOOL_TRUE(bplib_rbt_node_is_member(&tree, &node_array[0].link));
    UtAssert_BOOL_TRUE(bplib_rbt_node_is_member(&tree, &node_array[1].link));

    UtAssert_INT32_EQ(bplib_rbt_extract_node(&tree, &node_array[0].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&tree, &node_array[1].link), BP_SUCCESS);
    UtAssert_BOOL_TRUE(bplib_rbt_tree_is_empty(&tree));
    UtAssert_BOOL_FALSE(bplib_rbt_node_is_member(&tree, &node_array[0].link));
    UtAssert_BOOL_FALSE(bplib_rbt_node_is_member(&tree, &node_array[1].link));
}

/* Display the contents of the RB tree as a series of debug messages */
static uint32 Test_RBT_DebugDisplaySubtree(uint32 node_print_depth, bplib_rbt_link_t *node)
{
    char           my_color;
    uint32         my_black_height;
    rbtest_node_t *rbtn_ptr;

    my_black_height = 0;

    if (bplib_rbt_node_is_red(node))
    {
        my_color = 'R';
    }
    else
    {
        ++my_black_height;
        my_color = 'B';
    }

    if (node != NULL)
    {
        ++node_print_depth;
        my_black_height += Test_RBT_DebugDisplaySubtree(node_print_depth, node->right);

        rbtn_ptr = (rbtest_node_t *)node;
        UtDebug("%*s%-4lu[%-3d](%c/%d)\n", (node_print_depth - 1) * 3, "", bplib_rbt_get_key_value(node),
                rbtn_ptr->internal_value, my_color, my_black_height);

        Test_RBT_DebugDisplaySubtree(node_print_depth, node->left);
    }

    return my_black_height;
}

/* Confirm the subtree confirms to the R-B tree constriants */
static uint32 Test_RBT_CheckSubtree(bplib_rbt_link_t *node)
{
    uint32 my_black_height;
    uint32 left_black_height;
    uint32 right_black_height;

    left_black_height  = 0;
    right_black_height = 0;
    my_black_height    = 0;

    if (bplib_rbt_node_is_red(node))
    {
        /* the current node is red - the parent must not also be red.
         * this is one of the core constraints of the R-B tree algorithm */
        UtAssert_BOOL_FALSE(bplib_rbt_node_is_red(node->parent));
    }
    else
    {
        ++my_black_height;
    }

    if (node != NULL)
    {
        right_black_height = Test_RBT_CheckSubtree(node->right);
        left_black_height  = Test_RBT_CheckSubtree(node->left);

        /* Confirm the other constraint of an R-B tree, which is that the left side
         * and right side should have an equal black height. */
        UtAssert_UINT32_EQ(left_black_height, right_black_height);
        my_black_height += right_black_height;
    }

    return my_black_height;
}

/*
 * Test routine to print the contents of an R-B tree
 * The output is intended for debugging and validation
 * It also checks that the basic R-B constraints are being met
 */
static void Test_RBT_CheckTree(bplib_rbt_root_t *tree)
{
    /* Display the content of the tree for debug (this will only show if "-d" is set) */
    Test_RBT_DebugDisplaySubtree(1, tree->root);

    /* Check that the calculated/verified black height matches the expected */
    /* note that by definition an empty tree still has a black height of 1, but
     * the "black_height" counter was cleared to 0 via memset().  Therefore
     * the value here will always be one less than the correct value */
    UtDebug("Start tree validation");
    UtAssert_UINT32_EQ(Test_RBT_CheckSubtree(tree->root), (1 + tree->black_height));
    UtDebug("End tree validation");
}

void TestBplibMemCommon_RBT_LeafNodeInsertDelete(void)
{
    /*
     * Exercise all of the R-B tree rebalancing cases for insert and delete.
     * All operations in this test sequence are performed on leaf nodes only
     *
     * IMPORTANT: DO NOT REORDER THESE TEST CASES OR CHANGE THE VALUES WITHOUT UNDERSTANDING THIS TEST.
     * The numbers and order of operations here are critical, as they reproduce specific R-B tree
     * shapes and conditions after each step.
     */

    /* "parent is black" insert case, as an empty tree */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(16, &rbtree, &node_array[16].link), BP_SUCCESS);

    /* "red parent at root" case */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(24, &rbtree, &node_array[24].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "parent is black" insert case, as a non-empty tree */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(8, &rbtree, &node_array[8].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "red parent and uncle" insert case, with uncle at grandparent->right */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(4, &rbtree, &node_array[4].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "is_outer_grandchild" insert case, with parent_is_left_side == true */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(0, &rbtree, &node_array[0].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "red parent and uncle" insert case, with uncle at grandparent->left */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(12, &rbtree, &node_array[12].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "parent is black" insert case, as a non-empty tree */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(28, &rbtree, &node_array[28].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "is_outer_grandchild" insert case, with parent_is_left_side == false */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(36, &rbtree, &node_array[36].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "red parent and uncle" insert case, with uncle at grandparent->right */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(26, &rbtree, &node_array[26].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "is_inner_grandchild" insert case, with parent_is_left_side == false, node_is_left_side == true */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(25, &rbtree, &node_array[25].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "parent is black" insert case, as a non-empty tree */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(20, &rbtree, &node_array[20].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "is_inner_grandchild" insert case, with parent_is_left_side == true, node_is_left_side == false */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(22, &rbtree, &node_array[22].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* DELETE CASES */

    /* delete red leaf node */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[20].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "black_subject_with_red_sibling" delete case, node_is_left_side == false (turns parent red) */
    /* Then will invoke a second pass as "black_subject_with_red_parent" (turns parent black) */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[36].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "black_subject_with_red_distant_nephew" delete case, node_is_left_side == true */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[0].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* delete red leaf node (sets up for next case) */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[26].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "black_subject_with_red_close_nephew" delete case, node_is_left_side == false */
    /* Then will invoke a second pass as "black_subject_with_red_distant_nephew" */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[28].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "black_subject_with_all_black_relatives" delete case, node_is_left_side == false (moves to parent) */
    /* Then will invoke a second pass at parent as "black_subject_with_red_parent" */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[25].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "black_subject_with_red_parent" delete case, node_is_left_side == true */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[4].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* delete red leaf node (sets up for next case) */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[12].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "black_subject_with_red_close_nephew" delete case, node_is_left_side == true */
    /* Second pass as "black_subject_with_red_distant_nephew" */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[8].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "black_subject_with_all_black_relatives" delete case, node_is_left_side == true (moves to parent) */
    /* Then will invoke a second pass at parent as "black_subject_is_root" */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[16].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* delete red leaf node (sets up for next case) */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[24].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* "black_subject_is_root" delete case, (last/only node) */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[22].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);
}

void TestBplibMemCommon_RBT_NonLeafDelete(void)
{
    uint32 i;

    /* Exercise case for non-leaf delete cases */
    /* First build up a tree again -- adding nodes in order like this will yield a tree
     * where the left half is all black, and the right half has a mixture of red and black */
    for (i = 0; i < 30; ++i)
    {
        UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(i, &rbtree, &node_array[i].link), BP_SUCCESS);
    }
    Test_RBT_CheckTree(&rbtree);

    /* delete 28 (black) node, swaps with red leaf at 29 */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[28].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* delete 27 (red) node, swaps with black leaf at 26 */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[27].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* Delete a node that requires a more distant swap (not a child) */
    /* delete 15 (red) node, swaps with black leaf at 14 */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[15].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* delete 14 (black) node, swaps with 13 but that is not a leaf, so 13 swaps with 12. */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[14].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* Now just delete the rest of the nodes */
    for (i = 0; i < 30; ++i)
    {
        if (i != 14 && i != 15 && i != 27 && i != 28)
        {
            UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[i].link), BP_SUCCESS);
        }
    }
}

void TestBplibMemCommon_RBT_Unique(void)
{
    int              i;
    bplib_rbt_root_t alt_root;

    bplib_rbt_init_root(&alt_root);

    /* Case 1, insert a value and then remove it */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(100, &rbtree, &node_array[0].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(100, &rbtree, &node_array[1].link), BP_DUPLICATE);

    /* Inserting a second time should be recognized and return an error */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(100, &rbtree, &node_array[0].link), BP_ERROR);

    UtAssert_NULL(bplib_rbt_search_unique(99, &rbtree));
    UtAssert_ADDRESS_EQ(bplib_rbt_search_unique(100, &rbtree), &node_array[0].link);

    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[0].link), BP_SUCCESS);

    /* Extracting a second time should be recognized and return an error */
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[0].link), BP_ERROR);

    UtAssert_NULL(bplib_rbt_search_unique(100, &rbtree));

    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(300, &rbtree, &node_array[3].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(200, &rbtree, &node_array[2].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(0, &alt_root, &node_array[0].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(100, &alt_root, &node_array[1].link), BP_SUCCESS);
    Test_RBT_CheckTree(&alt_root);

    /* Attempt to add an already-connected node to a different root - this is not valid */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(200, &alt_root, &node_array[2].link), BP_ERROR);
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(300, &alt_root, &node_array[3].link), BP_ERROR);
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(0, &rbtree, &node_array[0].link), BP_ERROR);
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(100, &rbtree, &node_array[1].link), BP_ERROR);

    UtAssert_INT32_EQ(bplib_rbt_extract_node(&alt_root, &node_array[0].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&alt_root, &node_array[1].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[2].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[3].link), BP_SUCCESS);

    /* Inserting in this order creates a tree with leaves on the far left/right but not in the middle */
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(200, &rbtree, &node_array[2].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(400, &rbtree, &node_array[4].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(0, &rbtree, &node_array[0].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(300, &rbtree, &node_array[3].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(100, &rbtree, &node_array[1].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[2].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(200, &rbtree, &node_array[2].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[3].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[0].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[1].link), BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[4].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[2].link), BP_SUCCESS);

    /* add some nodes in incrementing order, this should utimately fill up to a tree depth of 3 */
    /* all these nodes (beyond the 1st) are resolved via the "right side outer grandchild" rebalance case */
    for (i = 0; i < 14; i += 2)
    {
        UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(100 * i, &rbtree, &node_array[i].link), BP_SUCCESS);
    }

    Test_RBT_CheckTree(&rbtree);

    /* add more nodes in decrementing order */
    for (i = 28; i >= 14; i -= 2)
    {
        UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(100 * i, &rbtree, &node_array[i].link), BP_SUCCESS);
    }

    Test_RBT_CheckTree(&rbtree);

    /* fill in the gaps.  This should exercise more of the rebalance cases. */
    for (i = 1; i < 28; i += 2)
    {
        UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(100 * i, &rbtree, &node_array[i].link), BP_SUCCESS);
    }

    Test_RBT_CheckTree(&rbtree);

    /* do some removals */
    UtAssert_ADDRESS_EQ(bplib_rbt_search_unique(0, &rbtree), &node_array[0].link);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[0].link), BP_SUCCESS);

    Test_RBT_CheckTree(&rbtree);

    /* 1200 is likely the root node */
    UtAssert_ADDRESS_EQ(bplib_rbt_search_unique(1200, &rbtree), &node_array[12].link);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[12].link), BP_SUCCESS);

    Test_RBT_CheckTree(&rbtree);

    /* 500 is a red leaf node (easy) */
    UtAssert_ADDRESS_EQ(bplib_rbt_search_unique(500, &rbtree), &node_array[5].link);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[5].link), BP_SUCCESS);

    Test_RBT_CheckTree(&rbtree);

    /* 400 is a black node but swapped into red leaf position */
    UtAssert_ADDRESS_EQ(bplib_rbt_search_unique(400, &rbtree), &node_array[4].link);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[4].link), BP_SUCCESS);

    Test_RBT_CheckTree(&rbtree);

    /* 300 should be more complex now */
    UtAssert_ADDRESS_EQ(bplib_rbt_search_unique(300, &rbtree), &node_array[3].link);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[3].link), BP_SUCCESS);

    Test_RBT_CheckTree(&rbtree);

    for (i = 0; i <= 28; ++i)
    {
        if (i != 0 && !(i >= 3 && i <= 5) && i != 12 && i != 5)
        {
            UtAssert_ADDRESS_EQ(bplib_rbt_search_unique(100 * i, &rbtree), &node_array[i].link);
            UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[i].link), BP_SUCCESS);

            Test_RBT_CheckTree(&rbtree);
        }
        else
        {
            UtAssert_NULL(bplib_rbt_search_unique(100 * i, &rbtree));
        }
    }
}

void TestBplibMemCommon_RBT_NonUnique(void)
{
    bplib_rbt_iter_t it;
    int              status;
    int              count;
    int              ref_val;

    /* Case 1, insert a value and then remove it */
    ref_val = 0;
    UtAssert_INT32_EQ(bplib_rbt_insert_value_generic(100, &rbtree, &node_array[0].link, rbtest_comparator, &ref_val),
                      BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    ref_val = 0;
    UtAssert_INT32_EQ(bplib_rbt_insert_value_generic(100, &rbtree, &node_array[1].link, rbtest_comparator, &ref_val),
                      BP_DUPLICATE);

    ref_val = 1;
    UtAssert_INT32_EQ(bplib_rbt_insert_value_generic(100, &rbtree, &node_array[1].link, rbtest_comparator, &ref_val),
                      BP_SUCCESS);

    ref_val = 1;
    UtAssert_NULL(bplib_rbt_search_generic(99, &rbtree, rbtest_comparator, &ref_val));

    ref_val = 0;
    UtAssert_ADDRESS_EQ(bplib_rbt_search_generic(100, &rbtree, rbtest_comparator, &ref_val), &node_array[0].link);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[0].link), BP_SUCCESS);

    ref_val = 0;
    UtAssert_NULL(bplib_rbt_search_generic(100, &rbtree, rbtest_comparator, &ref_val));

    ref_val = 1;
    UtAssert_ADDRESS_EQ(bplib_rbt_search_generic(100, &rbtree, rbtest_comparator, &ref_val), &node_array[1].link);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[1].link), BP_SUCCESS);

    /* part 2, create a case where the tree has duplicate keys but they are not next to each other */

    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(150, &rbtree, &node_array[150].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(160, &rbtree, &node_array[160].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(170, &rbtree, &node_array[170].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(180, &rbtree, &node_array[180].link), BP_SUCCESS);

    Test_RBT_CheckTree(&rbtree);

    ref_val = 165;
    UtAssert_INT32_EQ(bplib_rbt_insert_value_generic(160, &rbtree, &node_array[165].link, rbtest_comparator, &ref_val),
                      BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    ref_val = 155;
    UtAssert_INT32_EQ(bplib_rbt_insert_value_generic(160, &rbtree, &node_array[155].link, rbtest_comparator, &ref_val),
                      BP_SUCCESS);
    Test_RBT_CheckTree(&rbtree);

    /* iterate only items with key 160, in forward order */
    ref_val = 160;
    count   = 0;
    UtAssert_INT32_EQ(bplib_rbt_iter_goto_min(ref_val, &rbtree, &it), BP_SUCCESS);
    do
    {
        if (bplib_rbt_get_key_value(it.position) != ref_val)
        {
            break;
        }
        status = bplib_rbt_iter_next(&it);
        ++count;
    }
    while (status == BP_SUCCESS);

    /* iterate only items with key 160, in reverse order */
    ref_val = 160;
    count   = 0;
    UtAssert_INT32_EQ(bplib_rbt_iter_goto_max(ref_val, &rbtree, &it), BP_SUCCESS);
    do
    {
        if (bplib_rbt_get_key_value(it.position) != ref_val)
        {
            break;
        }
        status = bplib_rbt_iter_prev(&it);
        ++count;
    }
    while (status == BP_SUCCESS);

    UtAssert_INT32_EQ(count, 3);

    /* iterate all items except 180 in descending order */
    ref_val = 175;
    count   = 0;
    UtAssert_INT32_EQ(bplib_rbt_iter_goto_max(ref_val, &rbtree, &it), BP_SUCCESS);
    do
    {
        UtAssert_UINT32_LTEQ(bplib_rbt_get_key_value(it.position), ref_val);
        status = bplib_rbt_iter_prev(&it);
        ++count;
    }
    while (status == BP_SUCCESS);

    UtAssert_INT32_EQ(count, 5);

    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[150].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[155].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[160].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[165].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[170].link), BP_SUCCESS);
    UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, &node_array[180].link), BP_SUCCESS);
}

void TestBplibMemCommon_RBT_Iterator(void)
{
    int               i;
    int               count;
    int               status;
    bp_val_t          last_val;
    bplib_rbt_link_t *node_ptr;
    bplib_rbt_iter_t  it;

    /* Call with an iterator that has no position */
    memset(&it, 0, sizeof(it));
    UtAssert_INT32_EQ(bplib_rbt_iter_next(&it), BP_ERROR);
    UtAssert_INT32_EQ(bplib_rbt_iter_prev(&it), BP_ERROR);

    /* goal is to exercise the iterator APIs */
    /* Start by adding a bunch of nodes to a tree */
    for (i = 10; i < 90; i += 2)
    {
        UtAssert_INT32_EQ(bplib_rbt_insert_value_unique(i, &rbtree, &node_array[i].link), BP_SUCCESS);
    }

    Test_RBT_CheckTree(&rbtree);

    /* iterate the entire tree, by supplying 0 as the minimum */
    last_val = 0;
    count    = 0;
    UtAssert_INT32_EQ(bplib_rbt_iter_goto_min(last_val, &rbtree, &it), BP_SUCCESS);
    UtAssert_UINT32_EQ(bplib_rbt_get_key_value(it.position), 10);
    do
    {
        ++count;
        UtAssert_UINT32_GT(bplib_rbt_get_key_value(it.position), last_val);
        last_val = bplib_rbt_get_key_value(it.position);
        status   = bplib_rbt_iter_next(&it);
    }
    while (status == BP_SUCCESS);

    UtAssert_INT32_EQ(count, 40);

    /* iterate the entire tree, by supplying 100 as the maximum */
    last_val = 100;
    count    = 0;
    UtAssert_INT32_EQ(bplib_rbt_iter_goto_max(last_val, &rbtree, &it), BP_SUCCESS);
    UtAssert_UINT32_EQ(bplib_rbt_get_key_value(it.position), 88);
    do
    {
        ++count;
        UtAssert_UINT32_LT(bplib_rbt_get_key_value(it.position), last_val);
        last_val = bplib_rbt_get_key_value(it.position);
        status   = bplib_rbt_iter_prev(&it);
    }
    while (status == BP_SUCCESS);

    UtAssert_INT32_EQ(count, 40);

    /* iterate partial tree, by supplying 50 as the minimum */
    last_val = 50;
    count    = 0;
    UtAssert_INT32_EQ(bplib_rbt_iter_goto_min(last_val, &rbtree, &it), BP_SUCCESS);
    UtAssert_UINT32_EQ(bplib_rbt_get_key_value(it.position), 50);
    while (true)
    {
        ++count;
        if (bplib_rbt_iter_next(&it) != BP_SUCCESS)
        {
            break;
        }
        UtAssert_UINT32_GT(bplib_rbt_get_key_value(it.position), last_val);
        last_val = bplib_rbt_get_key_value(it.position);
    }

    UtAssert_INT32_EQ(count, 20);

    /* iterate partial tree, by supplying 50 as the maximum */
    last_val = 50;
    count    = 0;
    UtAssert_INT32_EQ(bplib_rbt_iter_goto_max(last_val, &rbtree, &it), BP_SUCCESS);
    UtAssert_UINT32_EQ(bplib_rbt_get_key_value(it.position), 50);
    while (true)
    {
        ++count;
        if (bplib_rbt_iter_prev(&it) != BP_SUCCESS)
        {
            break;
        }
        UtAssert_UINT32_LT(bplib_rbt_get_key_value(it.position), last_val);
        last_val = bplib_rbt_get_key_value(it.position);
    }

    /* this gets one extra, because both 50 and 10 should be included */
    UtAssert_INT32_EQ(count, 21);

    /* check some other cases */
    last_val = 33;
    UtAssert_INT32_EQ(bplib_rbt_iter_goto_max(last_val, &rbtree, &it), BP_SUCCESS);
    UtAssert_UINT32_EQ(bplib_rbt_get_key_value(it.position), 32);
    last_val = 59;
    UtAssert_INT32_EQ(bplib_rbt_iter_goto_min(last_val, &rbtree, &it), BP_SUCCESS);
    UtAssert_UINT32_EQ(bplib_rbt_get_key_value(it.position), 60);

    last_val = 95;
    UtAssert_INT32_NEQ(bplib_rbt_iter_goto_min(last_val, &rbtree, &it), BP_SUCCESS);
    last_val = 5;
    UtAssert_INT32_NEQ(bplib_rbt_iter_goto_max(last_val, &rbtree, &it), BP_SUCCESS);

    for (i = 10; i < 90; i += 2)
    {
        UtAssert_NOT_NULL(node_ptr = bplib_rbt_search_unique(i, &rbtree));

        UtAssert_INT32_EQ(bplib_rbt_extract_node(&rbtree, node_ptr), BP_SUCCESS);
    }

    UtAssert_INT32_NEQ(bplib_rbt_iter_goto_min(50, &rbtree, &it), BP_SUCCESS);
    UtAssert_INT32_NEQ(bplib_rbt_iter_goto_max(50, &rbtree, &it), BP_SUCCESS);
}
