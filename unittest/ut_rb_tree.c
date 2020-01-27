/************************************************************************
 * File: ut_crc.c
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *  All Other Rights Reserved.
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be
 *  used, distributed and modified only pursuant to the terms of that
 *  agreement.
 *
 * Maintainer(s):
 *  Alexander Meade, Code 582 NASA GSFC
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdlib.h>
#include <time.h>

#include "bplib.h"
#include "rb_tree.h"
#include "ut_assert.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define MAX_TREE_SIZE ((BP_MAX_ENCODED_VALUE  / 2) + 1)
#define RED   true  /* Boolean representing a red rb_node_t. */
#define BLACK false /* Boolean representing a black rb_node_t, */

/******************************************************************************
 TEST AND DEBUGGING HELPER FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * is_black -
 *-------------------------------------------------------------------------------------*/
static bool is_black(rb_node_t* node)
{
    return node == NULL || node->color == BLACK;
}

/*--------------------------------------------------------------------------------------
 * are_consecutive -
 *-------------------------------------------------------------------------------------*/
static bool are_consecutive(bp_val_t value_1, bp_val_t value_2)
{
    return (value_1 != BP_MAX_ENCODED_VALUE) && (value_1 + 1 == value_2);
}

/*--------------------------------------------------------------------------------------
 * apply_inorder - Applies a function in order to nodes in a rb_tree.
 *
 * node: A ptr to a rb_node to apply a function to and recurse its subtrees. [INPUT]
 * func. A function pointer accepting a ptr to an rb_node. [INPUT]
 *--------------------------------------------------------------------------------------*/
static void apply_inorder(rb_node_t* node, void (*func)(rb_node_t*))
{
    if (node->left != NULL)
    {
        /* Apply function to left subtree. */
        apply_inorder(node->left, func);
    }

    /* Apply function to the current node. */
    func(node);

    if (node->right != NULL)
    {
        /* Apply function to right subtree. */
        apply_inorder(node->right, func);
    }
}

/*--------------------------------------------------------------------------------------
 * assert_inorder_values_are - Asserts that the nodes in tree match the provided values.
 *
 * node: A ptr to a rb_node_t to recursively check if its values match the values array.
 * nodes: An array of ptr to rb_node_ts whose values, offsets and colors will be
 *      compared to the nodes in the tree tree inorder.
 * length: The length of the nodes array.
 * index: A ptr to an integer index into the nodes array which is incremented within recursive
 *      calls to this function.
 * returns: The number of times the current index was incremented by its subtrees
 *--------------------------------------------------------------------------------------*/
static uint32_t assert_inorder_nodes_are(rb_node_t* node,
                                    rb_node_t** nodes,
                                    uint32_t length,
                                    uint32_t index)
{
    uint32_t index_offset = 0;

    if (node == NULL)
    {
        /* In the case where root is empty we should have no nodes.  */
        ut_check(length == 0);
        return 0;
    }

    if (node->left != NULL)
    {
        /* Check left subtree values. */
        index_offset += assert_inorder_nodes_are(node->left, nodes, length, index);
    }

    /* Check that there are not more nodes than expected. */
    ut_check(index < length);

    /* Check that the nodes matches those provided in the list. */
    ut_check(node->range.value == nodes[index + index_offset]->range.value);
    ut_check(node->range.offset == nodes[index + index_offset]->range.offset);
    ut_check(node->color == nodes[index + index_offset]->color);

    index_offset += 1;

    if (node->right != NULL)
    {
        /* Check right subtre values. */
        index_offset += assert_inorder_nodes_are(node->right, nodes, length,
                                                 index + index_offset);
    }

    return index_offset;
}

/*--------------------------------------------------------------------------------------
 * assert_node_has_no_adjacent_red - Checks the condition that a given node has no adjacent
 *      red nodes among its children.
 *
 * node: A ptr to check rb_node_t to check it and its subtrees for validity. [INPUT]
 *--------------------------------------------------------------------------------------*/
static void assert_node_has_no_adjacent_red(rb_node_t* node)
{
    if (is_black(node))
    {
        /* Return early since the node is black and cannot have an adajcent red. */
        return;
    }
    if (node->left != NULL)
    {
        /* Assert left child is black since current node is red. */
        ut_check(is_black(node->left));
        assert_node_has_no_adjacent_red(node->left);
    }
    if (node->right != NULL)
    {
        /* Assert right child is black since current node is red. */
        ut_check(is_black(node->right));
        assert_node_has_no_adjacent_red(node->right);
    }
}

/*--------------------------------------------------------------------------------------
 * count_is_black -
 *
 * node: A ptr to an rb_node_t to check its color. [INPUT]
 * returns: 1 if the provided node is black or null, else 0.
 *--------------------------------------------------------------------------------------*/
static uint32_t count_is_black(rb_node_t* node)
{
    return is_black(node) ? 1 : 0;
}

/*--------------------------------------------------------------------------------------
 * assert_tree_pathes_have_equal_black_depths - Asserts the validity condition that
 *      all pathes from a given node in a red black tree have equal black depths.
 *
 * node: A ptr to a node to check its subtrees for equal black depths. [INPUT]
 * returns: The black depth in indicating the number of black nodes along a path to a
 * leaf starting at node.
 *--------------------------------------------------------------------------------------*/
static uint32_t assert_tree_pathes_have_equal_black_depths(rb_node_t* node)
{
    if (node == NULL)
    {
        /* If the node is NULL leaf or NULL root return early. */
        return 0;
    }

    uint32_t left_count = 0;
    uint32_t right_count = 0;
    if (node->left != NULL)
    {
        /* Node has a left child. Check that the black depths are
           equal down the child subtree. */
        left_count = assert_tree_pathes_have_equal_black_depths(node->left);
    }
    else {
        /* If node has no left child the left tree has a black
           depth of 1 since leaves are black. */
        left_count += 1;
    }
    if (node->right != NULL)
    {
        /* Node has a left child. Check that the black depths are equal
           down the child subtree. */
        right_count = assert_tree_pathes_have_equal_black_depths(node->right);
    }
    else
    {
        /* Node has no right child and so the right subtree black depth
           count is 1 since leaves are black. */
        right_count += 1;
    }

    ut_check (left_count == right_count);
    /* Need to only test one path since they are equal. */
    return count_is_black(node) + left_count;
}

/*--------------------------------------------------------------------------------------
 * assert_node_values_in_between_child - Asserts the red black tree validity condition that
 *      a given nodes left child has a value less than node and the right child has a value
 *      greater than node.
 *
 * node: A ptr to an rb_node_t to compare its value to its children and also check the children's subtrees. [INPUT]
 *--------------------------------------------------------------------------------------*/
static void assert_node_value_in_between_children(rb_node_t* node)
{
    if (node == NULL)
    {
        return;
    }

    if (node->left != NULL)
    {
        ut_check((node->range.value) > (node->left->range.value + node->left->range.offset));
        assert_node_value_in_between_children(node->left);
    }
    if (node->right != NULL)
    {
        ut_check((node->range.value + node->range.offset) < node->right->range.value);
        assert_node_value_in_between_children(node->right);
    }
}

/*--------------------------------------------------------------------------------------
 * assert_rb_tree_is_valid - Checks all the validity conditions for a valid red black tree.
 *
 * tree: A ptr to an rb_tree_t to check for validity. [INPUT]
 *--------------------------------------------------------------------------------------*/
static void assert_rb_tree_is_valid(rb_tree_t* tree)
{
    ut_check(is_black(tree->root));
    assert_node_has_no_adjacent_red(tree->root);
    assert_tree_pathes_have_equal_black_depths(tree->root);
    assert_node_value_in_between_children(tree->root);
}

/*--------------------------------------------------------------------------------------
 * shuffle - Shuffles an array of integers randomly.
 *
 * array: A ptr to the array to shuffle. [OUTPUT]
 * length: The length of the uint32_t array to shuffle. [INPUT]
 *--------------------------------------------------------------------------------------*/
static void shuffle(uint32_t* array, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        uint32_t j = i + rand() / (RAND_MAX / (length - i) + 1);
        uint32_t temp = array[j];
        array[j] = array[i];
        array[i] = temp;
    }
}

/******************************************************************************
 RB_TREE TESTS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * test_new_tree_empty -
 *--------------------------------------------------------------------------------------*/
static void test_new_tree_empty()
{
    int f = ut_failures();
    printf("test_new_empty_tree: ");

    rb_tree_t tree;
    ut_check(rb_tree_create(0, &tree) == BP_PARMERR);
    ut_check(tree.root == NULL);
    ut_check(tree.max_size == 0);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_unable_to_insert_into_empty_tree -
 *--------------------------------------------------------------------------------------*/
static void test_unable_to_insert_into_empty_tree()
{
    int f = ut_failures();
    printf("test_unable_to_insert_into_empty_tree: ");

    rb_tree_t tree;
    ut_check(rb_tree_create(0, &tree) == BP_PARMERR);
    ut_check(rb_tree_is_full(&tree));
    ut_check(tree.root == NULL);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_unable_to_insert_into_full_tree -
 *--------------------------------------------------------------------------------------*/
static void test_unable_to_insert_into_full_tree()
{
    int f = ut_failures();
    printf("test_unable_to_insert_into_full_tree: ");

    rb_tree_t tree;
    rb_tree_create(4, &tree);
    ut_check(tree.size == 0);
    ut_check(!rb_tree_is_full(&tree));
    rb_tree_insert(0, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(2, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(4, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(6, &tree);
    assert_rb_tree_is_valid(&tree);
    ut_check(tree.size == 4);
    ut_check(rb_tree_is_full(&tree));
    /* The final insert should fail when tree is full. */
    ut_check(rb_tree_insert(8, &tree) == BP_CUSTODYTREEFULL);
    assert_rb_tree_is_valid(&tree);
    rb_tree_destroy(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}


/*--------------------------------------------------------------------------------------
 * test_deletes_tree -
 *--------------------------------------------------------------------------------------*/
static void test_deletes_tree()
{
    int f = ut_failures();
    printf("test_deletes_tree: ");

    /* Tests deleting a tree with nodes. */
    rb_tree_t tree;
    rb_tree_create(5, &tree);
    rb_tree_insert(0, &tree);
    rb_tree_insert(1, &tree);
    rb_tree_insert(2, &tree);
    rb_tree_insert(3, &tree);
    rb_tree_destroy(&tree);

    /* Tests deleting an empty tree. */
    rb_tree_create(0, &tree);
    rb_tree_destroy(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_single_insertion -
 *--------------------------------------------------------------------------------------*/
static void test_insert_root()
{
    int f = ut_failures();
    printf("test_insert_root: ");

    rb_tree_t tree;
    rb_tree_create(1, &tree);
    rb_tree_insert(5, &tree);
    assert_rb_tree_is_valid(&tree);

    rb_node_t n1 = {.range = {.value = 5, .offset = 0}, .color=BLACK};
    rb_node_t* nodes[] = {&n1};
    assert_inorder_nodes_are(tree.root, nodes, 1, 0);
    rb_tree_destroy(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_insert_left_subtree -
 *--------------------------------------------------------------------------------------*/
static void test_insert_left_subtree()
{
    int f = ut_failures();
    printf("test_insert_left_subtree: ");

    /* Tests a single insertion into the left subtree from root. */
    rb_tree_t tree;
    rb_tree_create(4, &tree);
    rb_tree_insert(7, &tree);
    rb_tree_insert(5, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_node_t n1 = {.range = {.value = 7, .offset = 0}, .color = BLACK};
    rb_node_t n2 = {.range = {.value = 5, .offset = 0}, .color = RED};
    rb_node_t* nodes_1[] = {&n2, &n1};
    assert_inorder_nodes_are(tree.root, nodes_1, 2, 0);

    /* Tests inserting into a left subtree requiring a rebalancing. */
    rb_tree_insert(3, &tree);
    n1.color = RED;
    n2.color = BLACK;
    rb_node_t n3 = {.range = {.value = 3, .offset = 0}, .color = RED};
    rb_node_t* nodes_2[] = {&n3, &n2, &n1};
    assert_inorder_nodes_are(tree.root, nodes_2, 3, 0);

    /* Tests an insertion into the left subtree when the parent node is not root. */
    rb_tree_insert(1, &tree);
    n1.color = BLACK;
    n2.color = BLACK;
    n3.color = BLACK;
    rb_node_t n4 = {.range = {.value = 1, .offset = 0}, .color = RED};
    rb_node_t* nodes_3[] = {&n4, &n3, &n2, &n1};
    assert_inorder_nodes_are(tree.root, nodes_3, 4, 0);
    rb_tree_destroy(&tree);

    if(f == ut_failures()) printf("PASS\n");
     else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_insert_right_subtree -
 *--------------------------------------------------------------------------------------*/
static void test_insert_right_subtree()
{
    int f = ut_failures();
    printf("test_insert_right_subtree: ");

    /* Tests a single insertion into the right subtree from root. */
    rb_tree_t tree;
    rb_tree_create(4, &tree);
    rb_tree_insert(1, &tree);
    rb_tree_insert(3, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_node_t n1 = {.range = {.value = 1, .offset = 0}, .color = BLACK};
    rb_node_t n2 = {.range = {.value = 3, .offset = 0}, .color = RED};
    rb_node_t* nodes_1[] = {&n1, &n2};
    assert_inorder_nodes_are(tree.root, nodes_1, 2, 0);

    /* Tests inserting into a right subtree requiring a rebalancing. */
    rb_tree_insert(5, &tree);
    n1.color = RED;
    n2.color = BLACK;
    rb_node_t n3 = {.range = {.value = 5, .offset = 0}, .color = RED};
    rb_node_t* nodes_2[] = {&n1, &n2, &n3};
    assert_inorder_nodes_are(tree.root, nodes_2, 3, 0);

    /* Tests an insertion into the right subtree when the parent node is not root. */
    rb_tree_insert(7, &tree);
    n1.color = BLACK;
    n2.color = BLACK;
    n3.color = BLACK;
    rb_node_t n4 = {.range = {.value = 7, .offset = 0}, .color = RED};
    rb_node_t* nodes_3[] = {&n1, &n2, &n3, &n4};
    assert_inorder_nodes_are(tree.root, nodes_3, 4, 0);
    rb_tree_destroy(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_insert_merge_lower -
 *--------------------------------------------------------------------------------------*/
static void test_insert_merge_lower()
{
    int f = ut_failures();
    printf("test_insert_merge_lower: ");

    rb_tree_t tree;
    rb_tree_create(3, &tree);
    rb_tree_insert(5, &tree);
    rb_tree_insert(2, &tree);
    rb_tree_insert(10, &tree);

    rb_node_t n1 = {.range = {.value =  2, .offset = 0}, .color = RED};
    rb_node_t n2 = {.range = {.value =  5, .offset = 0}, .color = BLACK};
    rb_node_t n3 = {.range = {.value = 10, .offset = 0}, .color = RED};
    rb_node_t* nodes[] = {&n1, &n2, &n3};

    assert_rb_tree_is_valid(&tree);
    assert_inorder_nodes_are(tree.root, nodes, 3, 0);

    /* After inserting the following values all node values in the tree should be replaced
       with the corresponding consecutive number that is lower. The offset should also be
       incremented to indicate that each node stores multiple values. */
    rb_tree_insert(4, &tree);
    rb_tree_insert(1, &tree);
    rb_tree_insert(9, &tree);
    rb_tree_insert(8, &tree);
    rb_tree_insert(7, &tree);
    rb_tree_insert(0, &tree);

    n1.range.value = 0;
    n1.range.offset = 2;

    n2.range.value = 4;
    n2.range.offset = 1;

    n3.range.value = 7;
    n3.range.offset = 3;

    assert_rb_tree_is_valid(&tree);
    assert_inorder_nodes_are(tree.root, nodes, 3, 0);
    rb_tree_destroy(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_insert_merge_upper -
 *--------------------------------------------------------------------------------------*/
static void test_insert_merge_upper()
{
    int f = ut_failures();
    printf("test_insert_merge_upper: ");

    rb_tree_t tree;
    rb_tree_create(3, &tree);
    rb_tree_insert(5, &tree);
    rb_tree_insert(2, &tree);
    rb_tree_insert(10, &tree);

    rb_node_t n1 = {.range = {.value =  2, .offset = 0}, .color = RED};
    rb_node_t n2 = {.range = {.value =  5, .offset = 0}, .color = BLACK};
    rb_node_t n3 = {.range = {.value = 10, .offset = 0}, .color = RED};
    rb_node_t* nodes[] = {&n1, &n2, &n3};

    assert_rb_tree_is_valid(&tree);
    assert_inorder_nodes_are(tree.root, nodes, 3, 0);

    /* After inserting the following values all node values in the tree should increase
       have increased offsets reflecting the fact that the node stores a range of inserted
       values. */
    rb_tree_insert(6, &tree);
    rb_tree_insert(7, &tree);
    rb_tree_insert(3, &tree);
    rb_tree_insert(11, &tree);
    rb_tree_insert(12, &tree);
    rb_tree_insert(13, &tree);
    rb_tree_insert(14, &tree);
    rb_tree_insert(15, &tree);

    n1.range.offset = 1;
    n2.range.offset = 2;
    n3.range.offset = 5;

    assert_rb_tree_is_valid(&tree);
    assert_inorder_nodes_are(tree.root, nodes, 3, 0);
    rb_tree_destroy(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_insert_merge_lower_and_child -
 *--------------------------------------------------------------------------------------*/
static void test_insert_merge_lower_and_child()
{
    int f = ut_failures();
    printf("test_insert_merge_lower_and_child: ");

    rb_tree_t tree;
    rb_tree_create(7, &tree);
    rb_tree_insert(20, &tree);
    rb_tree_insert(15, &tree);
    rb_tree_insert(25, &tree);
    rb_tree_insert(10, &tree);
    rb_tree_insert(30, &tree);
    rb_tree_insert(5,  &tree);
    rb_tree_insert(35, &tree);

    rb_node_t n1 = {.range = {.value =  5, .offset = 0}, .color = RED};
    rb_node_t n2 = {.range = {.value = 10, .offset = 0}, .color = BLACK};
    rb_node_t n3 = {.range = {.value = 15, .offset = 0}, .color = RED};
    rb_node_t n4 = {.range = {.value = 20, .offset = 0}, .color = BLACK};
    rb_node_t n5 = {.range = {.value = 25, .offset = 0}, .color = RED};
    rb_node_t n6 = {.range = {.value = 30, .offset = 0}, .color = BLACK};
    rb_node_t n7 = {.range = {.value = 35, .offset = 0}, .color = RED};
    rb_node_t* nodes_1[] = {&n1, &n2, &n3, &n4, &n5, &n6, &n7};

    assert_rb_tree_is_valid(&tree);
    assert_inorder_nodes_are(tree.root, nodes_1, 7, 0);
    rb_tree_insert(11, &tree);
    rb_tree_insert(12, &tree);
    rb_tree_insert(13, &tree);
    rb_tree_insert(14, &tree);


    n2.range.offset = 5;
    rb_node_t* nodes_2[] = {&n1, &n2, &n4, &n5, &n6, &n7};
    assert_rb_tree_is_valid(&tree);
    assert_inorder_nodes_are(tree.root, nodes_2, 6, 0);
    rb_tree_destroy(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_insert_merge_upper_and_child -
 *--------------------------------------------------------------------------------------*/
static void test_insert_merge_upper_and_child()
{
    int f = ut_failures();
    printf("test_insert_merge_upper_and_child: ");

    rb_tree_t tree;
    rb_tree_create(4, &tree);
    rb_tree_insert(20, &tree);
    rb_tree_insert(10, &tree);
    rb_tree_insert(28, &tree);
    rb_tree_insert(30, &tree);

    rb_node_t n1 = {.range = {.value = 10, .offset = 0}, .color = BLACK};
    rb_node_t n2 = {.range = {.value = 20, .offset = 0}, .color = BLACK};
    rb_node_t n3 = {.range = {.value = 28, .offset = 0}, .color = BLACK};
    rb_node_t n4 = {.range = {.value = 30, .offset = 0}, .color = RED};
    rb_node_t* nodes_1[] = {&n1, &n2, &n3, &n4};

    assert_rb_tree_is_valid(&tree);
    assert_inorder_nodes_are(tree.root, nodes_1, 4, 0);

    rb_tree_insert(29, &tree);

    n3.range.offset = 2;
    rb_node_t* nodes_2[] = {&n1, &n2, &n3};

    assert_rb_tree_is_valid(&tree);
    assert_inorder_nodes_are(tree.root, nodes_2, 3, 0);
    rb_tree_destroy(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_merge_to_single_node -
 *--------------------------------------------------------------------------------------*/
static void test_merge_to_single_node()
{
    int f = ut_failures();
    printf("test_merge_to_single_node: ");

    rb_tree_t tree;
    rb_tree_create(10, &tree);
    rb_tree_insert(1, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(3, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(5, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(7, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(9, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(11, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(13, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(15, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(12, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(8, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(4, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(14, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(2, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(6, &tree);
    assert_rb_tree_is_valid(&tree);
    rb_tree_insert(10, &tree);

    rb_node_t final_node = {.range = {.value = 1, .offset = 14}, .color = false};
    rb_node_t* nodes[] = {&final_node};

    assert_rb_tree_is_valid(&tree);
    assert_inorder_nodes_are(tree.root, nodes, 1, 0);
    rb_tree_destroy(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_no_duplicates -
 *--------------------------------------------------------------------------------------*/
static void test_no_duplicates()
{
    int f = ut_failures();
    printf("test_no_duplicates: ");

    rb_tree_t tree;
    rb_tree_create(10, &tree);
    rb_tree_insert(5, &tree);
    rb_tree_insert(10, &tree);
    rb_tree_insert(15, &tree);
    rb_node_t n1 = {.range = {.value =  5, .offset = 0}, .color = RED};
    rb_node_t n2 = {.range = {.value = 10, .offset = 0}, .color = BLACK};
    rb_node_t n3 = {.range = {.value = 15, .offset = 0}, .color = RED};
    rb_node_t* nodes[] = {&n1, &n2, &n3};


    ut_check(tree.size == 3);
    assert_rb_tree_is_valid(&tree);
    assert_inorder_nodes_are(tree.root, nodes, 3, 0);

    ut_check(rb_tree_insert(5, &tree)  == BP_DUPLICATECID);
    ut_check(rb_tree_insert(5, &tree)  == BP_DUPLICATECID);
    ut_check(rb_tree_insert(10, &tree) == BP_DUPLICATECID);
    ut_check(rb_tree_insert(10, &tree) == BP_DUPLICATECID);
    ut_check(rb_tree_insert(15, &tree) == BP_DUPLICATECID);
    ut_check(rb_tree_insert(15, &tree) == BP_DUPLICATECID);

    ut_check(tree.size == 3);
    assert_rb_tree_is_valid(&tree);
    assert_inorder_nodes_are(tree.root, nodes, 3, 0);
    rb_tree_destroy(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_are_consecutive -
 *--------------------------------------------------------------------------------------*/
static void test_are_consecutive()
{
    int f = ut_failures();
    printf("test_are_consecutive: ");

    ut_check(are_consecutive(0, 1));
    ut_check(!are_consecutive(1, 0));
    ut_check(are_consecutive(UINT32_MAX - 1, UINT32_MAX));
    ut_check(!are_consecutive(UINT32_MAX, 0));

   if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_max_range_offset -
 *--------------------------------------------------------------------------------------*/
static void test_max_range_offset()
{
    int f = ut_failures();
    printf("test_max_range_offset: ");

    rb_tree_t tree;
    int status = rb_tree_create(MAX_TREE_SIZE, &tree);
    ut_check(status == BP_SUCCESS || status == BP_FAILEDMEM);
    if (status == BP_FAILEDMEM)
    {
        /* Return early if no memory was allocated. */
        printf("PASS\n");
        return;
    }

    for (uint32_t i = 0; i <= UINT32_MAX - 1; i++)
    {
        rb_tree_insert(i, &tree);
    }
    rb_tree_insert(UINT32_MAX, &tree);
    assert_rb_tree_is_valid(&tree);

    rb_node_t n = {.range = {.value = 0, .offset = UINT32_MAX}, .color = BLACK};
    rb_node_t* nodes[] = {&n};
    assert_inorder_nodes_are(tree.root, nodes, 1, 0);
    rb_tree_destroy(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_tree_traversed_and_deleted_inorder_with_rebalancing -
 *--------------------------------------------------------------------------------------*/
static void test_unable_to_delete_value_that_does_not_exist()
{
    int f = ut_failures();
    printf("test_unable_to_delete_value_that_does_not_exist: ");

    rb_tree_t tree;
    rb_tree_create(30, &tree);

    /* Write out 5 bundles and then skip 5 bundles. */
    for (uint32_t i = 0; i < 50; i += 10)
    {
        rb_tree_insert(i, &tree);
        rb_tree_insert(i + 1, &tree);
        rb_tree_insert(i + 2, &tree);
        rb_tree_insert(i + 3, &tree);
        rb_tree_insert(i + 4, &tree);
    }
    assert_rb_tree_is_valid(&tree);

    ut_check(tree.size == 5);
    ut_check(rb_tree_delete(6, &tree) == BP_CIDNOTFOUND);
    ut_check(rb_tree_delete(25, &tree) == BP_CIDNOTFOUND);
    ut_check(rb_tree_delete(39, &tree) == BP_CIDNOTFOUND);
    ut_check(tree.size == 5);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_delete_single_node -
 *--------------------------------------------------------------------------------------*/
static void test_delete_single_node()
{
    int f = ut_failures();
    printf("test_delete_single_node: ");

    rb_tree_t tree;
    rb_tree_create(10, &tree);
    for (uint32_t i = 0; i <= 10; i += 2)
    {
        rb_tree_insert(i, &tree);
    }
    ut_check(tree.size == 6);
    assert_rb_tree_is_valid(&tree);

    rb_node_t n1 = {.range = {.value = 0,  .offset = 0}, .color = BLACK};
    rb_node_t n2 = {.range = {.value = 2,  .offset = 0}, .color = BLACK};
    rb_node_t n3 = {.range = {.value = 4,  .offset = 0}, .color = BLACK};
    rb_node_t n4 = {.range = {.value = 6,  .offset = 0}, .color = RED};
    rb_node_t n5 = {.range = {.value = 8,  .offset = 0}, .color = BLACK};
    rb_node_t n6 = {.range = {.value = 10, .offset = 0}, .color = RED};

    rb_node_t* nodes_start[] = {&n1, &n2, &n3, &n4, &n5, &n6};
    assert_inorder_nodes_are(tree.root, nodes_start, 6, 0);

    rb_tree_delete(6, &tree);
    n5.color = RED;
    n6.color = BLACK;
    rb_node_t* nodes_end[] = {&n1, &n2, &n3, &n5, &n6};
    assert_inorder_nodes_are(tree.root, nodes_end, 5, 0);
    ut_check(tree.size == 5);
    assert_rb_tree_is_valid(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_tree_traversed_and_deleted_inorder_with_rebalancing -
 *--------------------------------------------------------------------------------------*/
static void test_deletes_node_at_start_of_range()
{
    int f = ut_failures();
    printf("test_deletes_node_at_start_of_range: ");

    rb_tree_t tree;
    rb_tree_create(10, &tree);
    rb_tree_insert(0, &tree);
    rb_tree_insert(1, &tree);
    rb_tree_insert(2, &tree);
    rb_tree_insert(5, &tree);
    rb_tree_insert(6, &tree);
    rb_tree_insert(7, &tree);
    rb_tree_insert(9, &tree);
    rb_tree_insert(10, &tree);
    rb_tree_insert(13, &tree);
    rb_tree_insert(14, &tree);
    rb_tree_insert(16, &tree);

    ut_check(tree.size == 5);

    rb_node_t n1 = {.range = {.value = 0,  .offset = 2}, .color = BLACK};
    rb_node_t n2 = {.range = {.value = 5,  .offset = 2}, .color = BLACK};
    rb_node_t n3 = {.range = {.value = 9,  .offset = 1}, .color = RED};
    rb_node_t n4 = {.range = {.value = 13, .offset = 1}, .color = BLACK};
    rb_node_t n5 = {.range = {.value = 16, .offset = 0}, .color = RED};
    rb_node_t* nodes[] = {&n1, &n2, &n3, &n4, &n5};

    assert_rb_tree_is_valid(&tree);
    assert_inorder_nodes_are(tree.root, nodes, 5, 0);

    rb_tree_delete(13, &tree);
    ut_check(tree.size == 5);
    n4.range.value = 14;
    n4.range.offset = 0;
    assert_inorder_nodes_are(tree.root, nodes, 5, 0);

    rb_tree_delete(5, &tree);
    ut_check(tree.size == 5);
    n2.range.value = 6;
    n2.range.offset = 1;
    assert_inorder_nodes_are(tree.root, nodes, 5, 0);
    assert_rb_tree_is_valid(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_deletes_node_at_end_of_range -
 *--------------------------------------------------------------------------------------*/
static void test_deletes_node_at_end_of_range()
{
    int f = ut_failures();
    printf("test_deletes_node_at_end_of_range: ");

    rb_tree_t tree;
    rb_tree_create(10, &tree);
    rb_tree_insert(0, &tree);
    rb_tree_insert(1, &tree);
    rb_tree_insert(2, &tree);
    rb_tree_insert(5, &tree);
    rb_tree_insert(6, &tree);
    rb_tree_insert(7, &tree);
    rb_tree_insert(9, &tree);
    rb_tree_insert(10, &tree);
    rb_tree_insert(13, &tree);
    rb_tree_insert(14, &tree);
    rb_tree_insert(16, &tree);

    ut_check(tree.size == 5);

    rb_node_t n1 = {.range = {.value = 0,  .offset = 2}, .color = BLACK};
    rb_node_t n2 = {.range = {.value = 5,  .offset = 2}, .color = BLACK};
    rb_node_t n3 = {.range = {.value = 9,  .offset = 1}, .color = RED};
    rb_node_t n4 = {.range = {.value = 13, .offset = 1}, .color = BLACK};
    rb_node_t n5 = {.range = {.value = 16, .offset = 0}, .color = RED};
    rb_node_t* nodes[] = {&n1, &n2, &n3, &n4, &n5};

    assert_rb_tree_is_valid(&tree);
    assert_inorder_nodes_are(tree.root, nodes, 5, 0);

    rb_tree_delete(14, &tree);
    ut_check(tree.size == 5);
    n4.range.offset = 0;
    assert_inorder_nodes_are(tree.root, nodes, 5, 0);

    rb_tree_delete(7, &tree);
    ut_check(tree.size == 5);
    n2.range.offset = 1;
    assert_inorder_nodes_are(tree.root, nodes, 5, 0);
    assert_rb_tree_is_valid(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_deletes_node_inside_of_range -
 *--------------------------------------------------------------------------------------*/
static void test_deletes_node_inside_of_range()
{
    int f = ut_failures();
    printf("test_deletes_node_inside_of_range: ");

    rb_tree_t tree;
    rb_tree_create(10, &tree);
    rb_tree_insert(0, &tree);
    rb_tree_insert(1, &tree);
    rb_tree_insert(2, &tree);
    rb_tree_insert(5, &tree);
    rb_tree_insert(6, &tree);
    rb_tree_insert(7, &tree);
    rb_tree_insert(8, &tree);
    rb_tree_insert(9, &tree);
    rb_tree_insert(13, &tree);
    rb_tree_insert(14, &tree);
    rb_tree_insert(16, &tree);
    rb_tree_insert(18, &tree);

    rb_node_t n1 = {.range = {.value = 0,  .offset = 2}, .color = BLACK};
    rb_node_t n2 = {.range = {.value = 5,  .offset = 4}, .color = BLACK};
    rb_node_t n3 = {.range = {.value = 13, .offset = 1}, .color = RED};
    rb_node_t n4 = {.range = {.value = 16, .offset = 0}, .color = BLACK};
    rb_node_t n5 = {.range = {.value = 18, .offset = 0}, .color = RED};

    rb_node_t* nodes_start[] = {&n1, &n2, &n3, &n4, &n5};

    assert_rb_tree_is_valid(&tree);
    ut_check(tree.size == 5);
    assert_inorder_nodes_are(tree.root, nodes_start, 5, 0);

    rb_tree_delete(6, &tree);

    n2.range.offset = 0;
    rb_node_t n6 = {.range = {.value = 7, .offset = 2}, .color = RED};
    rb_node_t* nodes_end[] = {&n1, &n2, &n6, &n3, &n4, &n5};
    ut_check(tree.size == 6);
    assert_inorder_nodes_are(tree.root, nodes_end, 6, 0);
    assert_rb_tree_is_valid(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_deletes_node_inside_of_small_range -
 *--------------------------------------------------------------------------------------*/
static void test_deletes_node_inside_of_small_range()
{
    int f = ut_failures();
    printf("test_deletes_node_inside_of_small_range: ");

    rb_tree_t tree;
    rb_tree_create(10, &tree);
    rb_tree_insert(0, &tree);
    rb_tree_insert(1, &tree);
    rb_tree_insert(2, &tree);

    assert_rb_tree_is_valid(&tree);

    rb_tree_delete(1, &tree);

    assert_rb_tree_is_valid(&tree);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_tree_traversed_and_deleted_inorder_with_rebalancing -
 *--------------------------------------------------------------------------------------*/
static void test_tree_traversed_and_deleted_inorder_with_rebalancing()
{
    int f = ut_failures();
    printf("test_tree_traversed_and_deleted_inorder_with_rebalancing: ");

    rb_tree_t tree;
    uint32_t max_nodes = 10;
    rb_tree_create(max_nodes, &tree);
    ut_check(rb_tree_insert(6, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(2, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(3, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(8, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(10, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(12, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(11, &tree) == BP_SUCCESS);
    assert_rb_tree_is_valid(&tree);
    ut_check(tree.size == 4);

    rb_range_t range;

    ut_check(rb_tree_goto_first(NULL) == BP_PARMERR);
    ut_check(rb_tree_goto_first(&tree) == BP_SUCCESS);
    ut_check(rb_tree_get_next(&tree, NULL, true, true) == BP_ERROR);

    ut_check(rb_tree_get_next(&tree, &range, true, true) == BP_SUCCESS);
    ut_check(range.value == 2 && range.offset == 1);
    assert_rb_tree_is_valid(&tree);

    ut_check(rb_tree_get_next(&tree, &range, true, true) == BP_SUCCESS);
    ut_check(range.value == 6 && range.offset == 0);
    assert_rb_tree_is_valid(&tree);

    ut_check(rb_tree_get_next(&tree, &range, true, true) == BP_SUCCESS);
    ut_check(range.value == 8 && range.offset == 0);
    assert_rb_tree_is_valid(&tree);

    ut_check(rb_tree_get_next(&tree, &range, true, true) == BP_SUCCESS);
    ut_check(range.value == 10 && range.offset == 2);
    assert_rb_tree_is_valid(&tree);

    ut_check(rb_tree_get_next(&tree, &range, true, true) == BP_ERROR);
    ut_check(tree.size == 0);

    if(f == ut_failures())  printf("PASS\n");
    else                    printf("FAIL\n");
}


/*--------------------------------------------------------------------------------------
 * test_tree_traversed_and_deleted_inorder_without_rebalancing -
 *--------------------------------------------------------------------------------------*/
static void test_tree_traversed_and_deleted_inorder_without_rebalancing()
{
    int f = ut_failures();
    printf("test_tree_traversed_and_deleted_inorder_without_rebalancing: ");

    rb_tree_t tree;
    uint32_t max_nodes = 10;
    rb_tree_create(max_nodes, &tree);
    ut_check(rb_tree_insert(6, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(2, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(3, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(8, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(10, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(12, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(11, &tree) == BP_SUCCESS);
    assert_rb_tree_is_valid(&tree);
    ut_check(tree.size == 4);

    rb_range_t range;

    ut_check(rb_tree_goto_first(&tree) == BP_SUCCESS);

    ut_check(rb_tree_get_next(&tree, &range, true, false) == BP_SUCCESS);
    ut_check(range.value == 2 && range.offset == 1);

    ut_check(rb_tree_get_next(&tree, &range, true, false) == BP_SUCCESS);
    ut_check(range.value == 6 && range.offset == 0);

    ut_check(rb_tree_get_next(&tree, &range, true, false) == BP_SUCCESS);
    ut_check(range.value == 8 && range.offset == 0);

    ut_check(rb_tree_get_next(&tree, &range, true, false) == BP_SUCCESS);
    ut_check(range.value == 10 && range.offset == 2);
    ut_check(rb_tree_get_next(&tree, &range, true, false) ==
        BP_ERROR);
    ut_check(tree.size == 0);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_tree_traversed_inorder_after_partial_traversal -
 *--------------------------------------------------------------------------------------*/
static void test_tree_traversed_inorder_after_partial_traversal()
{
    int f = ut_failures();
    printf("test_tree_traversed_inorder_after_partial_traversal: ");

    rb_tree_t tree;
    uint32_t max_nodes = 10;
    rb_tree_create(max_nodes, &tree);
    ut_check(rb_tree_insert(2, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(4, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(6, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(8, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(10, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(12, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(14, &tree) == BP_SUCCESS);
    ut_check(rb_tree_insert(16, &tree) == BP_SUCCESS);
    assert_rb_tree_is_valid(&tree);
    ut_check(tree.size == 8);

    rb_range_t range;

    ut_check(rb_tree_goto_first(&tree) == BP_SUCCESS);
    for (uint32_t i = 2; i <= 8; i += 2)
    {
        ut_check(rb_tree_get_next(&tree, &range, false, false) == BP_SUCCESS);
        ut_check(range.value == i && range.offset == 0);
    }

    ut_check(rb_tree_goto_first(&tree) == BP_SUCCESS);
    for (uint32_t i = 2; i <= 16; i += 2)
    {
        ut_check(rb_tree_get_next(&tree, &range, false, false) == BP_SUCCESS);
        ut_check(range.value == i && range.offset == 0);
    }

    ut_check(rb_tree_get_next(&tree, &range, true, false) ==
        BP_ERROR);
    ut_check(tree.size == 8);

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}

/*--------------------------------------------------------------------------------------
 * test_random_stress -
 *--------------------------------------------------------------------------------------*/
static void test_random_stress()
{
    int f = ut_failures();
    printf("test_random_stress: ");
    fflush(stdout);

    uint32_t number_trees = 10;
    uint32_t max_bundles = 16000;
    rb_node_t n = {.range = {.value = 0, .offset = max_bundles - 1}, .color = BLACK};
    rb_node_t* nodes[] = {&n};

    uint32_t bundle_ids[max_bundles];
    for (uint32_t i = 0; i < max_bundles; i++)
    {
        bundle_ids[i] = i;
    }

    for (uint32_t i = 0; i < number_trees; i++)
    {
        int subf = ut_failures();

        rb_tree_t tree;
        rb_tree_create(max_bundles, &tree);
        shuffle(bundle_ids, max_bundles);
        for (uint32_t j = 0; j < max_bundles; j++)
        {
            rb_tree_insert(bundle_ids[j], &tree);
            assert_rb_tree_is_valid(&tree);
        }
        assert_inorder_nodes_are(tree.root, nodes, 1, 0);
        rb_tree_destroy(&tree);

        if(subf == ut_failures()) printf(".");
        else                            printf("!");
        fflush(stdout);
    }

    if(f == ut_failures()) printf("PASS\n");
    else                   printf("FAIL\n");
}


/* Struct used to measure rb_tree stats for a given operation. */
typedef struct rb_tree_stats
{
    char* name;
    uint32_t count;
    double total;
    double max;
    double min;
    double average;
} rb_tree_stats_t;

/*--------------------------------------------------------------------------------------
 * print_stats -
 *
 * stats - A ptr to an rb_tree_stats_t struct to print. [INPUT]
 *--------------------------------------------------------------------------------------*/
static void print_stats(rb_tree_stats_t* stats)
{
    printf("%s -> ( Count: %d | Total %f | Max: %f | Min: %f | Average: %f )\n",
                stats->name,
                stats->count,
                stats->total / CLOCKS_PER_SEC,
                stats->max / CLOCKS_PER_SEC,
                stats->min / CLOCKS_PER_SEC,
                stats->average / CLOCKS_PER_SEC);
}

/*--------------------------------------------------------------------------------------
 * characterize_rb_tree_performance - Tests the worst case for insertions which is
 *      skipping every other value within a range to avoid any merge conditions
 *      thereby resulting in new nodes insertions and the largest possible tree size.
 *      This does NOT generate the worst case in terms of tree rebalancing which literature
 *      does not have a clear answer for. After creating complete tree which is merged to a
 *      single node. This function performs a series of worst case deletions until the tree
 *      is empty. These are caused when deleting a value within a range resulting in the
 *      insertion of a new node.
 *--------------------------------------------------------------------------------------*/
static int characterize_rb_tree_performance()
{
    printf("\nCharacterizing RB Tree Performance... \n");
    uint32_t max_trees = 1000;
    uint32_t max_size = 16000;
    rb_tree_t tree;
    int status = rb_tree_create(max_size, &tree);

    if (status != BP_SUCCESS)
    {
        /* If tree creation status is not successful, return early. */
        return status;
    }



    /* Define stats variables to track. */
    rb_tree_stats_t insert_stats = {.name="Insert",
                                    .count = 0,
                                    .total = 0,
                                    .max = 0,
                                    .min = UINT32_MAX,
                                    .average = 0,
                                   };

    rb_tree_stats_t delete_stats = {.name="Delete",
                                    .count = 0,
                                    .total = 0,
                                    .max = 0,
                                    .min = UINT32_MAX,
                                    .average = 0,
                                   };


    uint32_t max_val = (max_size - 1) * 2;
    ut_check(max_val >= 2);

    clock_t start, end, elapsed;

    uint32_t bundle_ids[max_val];
    for (uint32_t i = 0; i < max_val; i++)
    {
        bundle_ids[i] = i;
    }


    /* Seed random test. */
    srand(0);
    for (uint32_t j = 0; j < max_trees; j++)
    {
        shuffle(bundle_ids, max_val);
        for (uint32_t i = 0; i <= max_val; i++)
        {
            start = clock();
            rb_tree_insert(i, &tree);
            end = clock();

            elapsed = end - start;

            insert_stats.max = insert_stats.max < elapsed ? elapsed : insert_stats.max;
            insert_stats.min = insert_stats.min > elapsed ? elapsed : insert_stats.min;
            insert_stats.total += elapsed;
            insert_stats.count += 1;
        }
        ut_check(tree.size == 1);
        shuffle(bundle_ids, max_val);
        for (uint32_t i = 0; i <= max_val; i++)
        {
            start = clock();
            rb_tree_delete(i, &tree);
            end = clock();

            elapsed = end - start;

            delete_stats.max = delete_stats.max < elapsed ? elapsed : delete_stats.max;
            delete_stats.min = delete_stats.min > elapsed ? elapsed : delete_stats.min;
            delete_stats.total += elapsed;
            delete_stats.count += 1;
        }
        ut_check(rb_tree_is_empty(&tree));
     }

    insert_stats.average = insert_stats.total / insert_stats.count;
    delete_stats.average = delete_stats.total/ delete_stats.count;
    printf("Results calculated from creating and destroying %d rb_trees of size %d\n",
            max_trees,
            max_size);
    print_stats(&insert_stats);
    print_stats(&delete_stats);
    printf("Performance Characterization Complete!\n");
    return BP_SUCCESS;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * print_node -
 *
 * node: A ptr to an rb_node to print. [INPUT]
 *--------------------------------------------------------------------------------------*/
void print_node(rb_node_t* node)
{
    if (node == NULL)
    {
        printf("NULL NODE\n");
        return;
    }

    printf("[ C: %5s || N: %3d || P: %3d || L: %3d || R: %3d || O: %3lu]\n",
        node->color ? "RED" : "BLACK",
        (int) node->range.value,
        node->parent != NULL ? (int) node->parent->range.value : -1,
        node->left != NULL ? (int) node->left->range.value : -1,
        node->right != NULL ? (int) node->right->range.value : -1,
        (unsigned long)node->range.offset);
}

/*--------------------------------------------------------------------------------------
 * print_tree -
 *
 * tree: A ptr to an rb_tree to print. [INPUT]
 *--------------------------------------------------------------------------------------*/
void print_tree(rb_tree_t* tree)
{
    printf("\n##################################\n");
    printf("* Size: %lu / %lu\n", (unsigned long)tree->size, (unsigned long)tree->max_size);
    printf("**********************************\n");

    if (tree->size == 0) {
        return;
    }
    printf("* In Order Elements:               \n");
    printf("**********************************\n");
    apply_inorder(tree->root, print_node);
    printf("**********************************\n");
    fflush(stdout);
}

/*--------------------------------------------------------------------------------------
 * ut_rb_tree_perf -
 *--------------------------------------------------------------------------------------*/
int ut_rb_tree_perf (void)
{
    characterize_rb_tree_performance();

    return 0;
}

/*--------------------------------------------------------------------------------------
 * ut_rb_tree -
 *--------------------------------------------------------------------------------------*/
int ut_rb_tree (void)
{
    ut_reset();

    test_new_tree_empty();
    test_unable_to_insert_into_empty_tree();
    test_unable_to_insert_into_full_tree();
    test_deletes_tree();
    test_insert_root();
    test_insert_left_subtree();
    test_insert_right_subtree();
    test_insert_merge_lower();
    test_insert_merge_upper();
    test_insert_merge_lower_and_child();
    test_insert_merge_upper_and_child();
    test_merge_to_single_node();
    test_no_duplicates();
    test_are_consecutive();
    test_max_range_offset();
    test_unable_to_delete_value_that_does_not_exist();
    test_delete_single_node();
    test_deletes_node_at_start_of_range();
    test_deletes_node_at_end_of_range();
    test_deletes_node_inside_of_range();
    test_deletes_node_inside_of_small_range();
    test_tree_traversed_and_deleted_inorder_with_rebalancing();
    test_tree_traversed_and_deleted_inorder_without_rebalancing();
    test_tree_traversed_inorder_after_partial_traversal();
    test_random_stress();

    return ut_failures();
}
