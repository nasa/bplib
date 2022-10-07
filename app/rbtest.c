/************************************************************************
 *
 *  Test program for Red-Black tree implementation
 *
 *  This is intended to confirm the operation of the basic R-B algorithm
 *
 *************************************************************************/

/*************************************************************************
 * Includes
 *************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

#include "bplib_api_types.h"
#include "v7_rbtree.h"

#define rbtree_assert(cond) rbtree_assert_impl(#cond, cond)

#define TEST_SIZE 2150

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

void rbtree_assert_impl(const char *text, bool condition)
{
    if (!condition)
    {
        fprintf(stderr, "Assertion failed: %s\n", text);
        abort();
    }
}

/* Confirm the subtree confirms to the R-B tree constriants */
int bplib_rbt_check_subtree(int node_print_depth, bplib_rbt_link_t *node)
{
    char           my_color;
    int            my_black_height;
    int            left_black_height;
    int            right_black_height;
    rbtest_node_t *rbtn_ptr;

    left_black_height  = 0;
    right_black_height = 0;
    my_black_height    = 0;

    if (bplib_rbt_node_is_red(node))
    {
        /* the current node is red - the parent must not also be red.
         * this is one of the core constraints of the R-B tree algorithm */
        rbtree_assert(!bplib_rbt_node_is_red(node->parent));
        my_color = 'R';
    }
    else
    {
        ++my_black_height;
        my_color = 'B';
    }

    if (node != NULL)
    {
        if (node_print_depth > 0)
        {
            ++node_print_depth;
        }
        right_black_height = bplib_rbt_check_subtree(node_print_depth, node->right);
        my_black_height += right_black_height;
        if (node_print_depth > 0)
        {
            rbtn_ptr = (rbtest_node_t *)node;
            printf("%*s%-4lu[%-4d](%c/%d)\n", (node_print_depth - 1) * 3, "", bplib_rbt_get_key_value(node),
                   rbtn_ptr->internal_value, my_color, my_black_height);
        }

        left_black_height = bplib_rbt_check_subtree(node_print_depth, node->left);

        /* Confirm the other constraint of an R-B tree, which is that the left side
         * and right side should have an equal black height. */
        rbtree_assert(left_black_height == right_black_height);
    }

    return my_black_height;
}

/*
 * Test routine to print the contents of an R-B tree
 * The output is intended for debugging and validation
 * It also checks that the basic R-B constraints are being met
 */
void bplib_rbt_debug_print(bplib_rbt_root_t *tree)
{
    int black_height;

    printf("\nTree content:\n");
    black_height = bplib_rbt_check_subtree(1, tree->root);

    /* Check that the calculated/verified black height matches the expected */
    /* note that by definition an empty tree still has a black height of 1, but
     * the "black_height" counter was cleared to 0 via memset().  Therefore
     * the value here will always be one less than the correct value */
    rbtree_assert(black_height == (1 + tree->black_height));
}

void basic_test_unique(void)
{
    int               i;
    int               status;
    bplib_rbt_link_t *node_ptr;

    /* Case 1, insert a value and then remove it */
    status = bplib_rbt_insert_value_unique(100, &rbtree, &node_array[0].link);
    rbtree_assert(status == BP_SUCCESS);
    bplib_rbt_debug_print(&rbtree);

    status = bplib_rbt_insert_value_unique(100, &rbtree, &node_array[1].link);
    rbtree_assert(status == BP_DUPLICATE);

    node_ptr = bplib_rbt_search_unique(99, &rbtree);
    rbtree_assert(node_ptr == NULL);

    node_ptr = bplib_rbt_search_unique(100, &rbtree);
    rbtree_assert(node_ptr == &node_array[0].link);

    status = bplib_rbt_extract_node(&rbtree, node_ptr);
    rbtree_assert(status == BP_SUCCESS);

    node_ptr = bplib_rbt_search_unique(100, &rbtree);
    rbtree_assert(node_ptr == NULL);

    /* add some nodes in incrementing order, this should utimately fill up to a tree depth of 3 */
    /* all these nodes (beyond the 1st) are resolved via the "right side outer grandchild" rebalance case */
    for (i = 0; i < 14; i += 2)
    {
        status = bplib_rbt_insert_value_unique(100 * i, &rbtree, &node_array[i].link);
        rbtree_assert(status == BP_SUCCESS);

        printf("\n\nTree after insert %d\n", i);
        bplib_rbt_debug_print(&rbtree);
    }

    /* add more nodes in decrementing order */
    for (i = 28; i >= 14; i -= 2)
    {
        status = bplib_rbt_insert_value_unique(100 * i, &rbtree, &node_array[i].link);
        rbtree_assert(status == BP_SUCCESS);
    }

    printf("\n\nTree after set 2\n");
    bplib_rbt_debug_print(&rbtree);

    /* fill in the gaps.  This should exercise more of the rebalance cases. */
    for (i = 1; i < 28; i += 2)
    {
        status = bplib_rbt_insert_value_unique(100 * i, &rbtree, &node_array[i].link);
        rbtree_assert(status == BP_SUCCESS);
    }

    printf("\n\nTree after set 3\n");
    bplib_rbt_debug_print(&rbtree);

    /* do some removals */
    node_ptr = bplib_rbt_search_unique(0, &rbtree);
    rbtree_assert(node_ptr == &node_array[0].link);

    status = bplib_rbt_extract_node(&rbtree, node_ptr);
    rbtree_assert(status == BP_SUCCESS);

    printf("\n\nTree after remove 0\n");
    bplib_rbt_debug_print(&rbtree);

    /* 1200 is likely the root node */
    node_ptr = bplib_rbt_search_unique(1200, &rbtree);
    rbtree_assert(node_ptr == &node_array[12].link);

    status = bplib_rbt_extract_node(&rbtree, node_ptr);
    rbtree_assert(status == BP_SUCCESS);

    printf("\n\nTree after remove 12\n");
    bplib_rbt_debug_print(&rbtree);

    /* 500 is a red leaf node (easy) */
    node_ptr = bplib_rbt_search_unique(500, &rbtree);
    rbtree_assert(node_ptr == &node_array[5].link);

    status = bplib_rbt_extract_node(&rbtree, node_ptr);
    rbtree_assert(status == BP_SUCCESS);

    printf("\n\nTree after remove 5\n");
    bplib_rbt_debug_print(&rbtree);

    /* 400 is a black node but swapped into red leaf position */
    node_ptr = bplib_rbt_search_unique(400, &rbtree);
    rbtree_assert(node_ptr == &node_array[4].link);

    status = bplib_rbt_extract_node(&rbtree, node_ptr);
    rbtree_assert(status == BP_SUCCESS);

    printf("\n\nTree after remove 4\n");
    bplib_rbt_debug_print(&rbtree);

    /* 300 should be more complex now */
    node_ptr = bplib_rbt_search_unique(300, &rbtree);
    rbtree_assert(node_ptr == &node_array[3].link);

    status = bplib_rbt_extract_node(&rbtree, node_ptr);
    rbtree_assert(status == BP_SUCCESS);

    printf("\n\nTree after remove 3\n");
    bplib_rbt_debug_print(&rbtree);

    for (i = 0; i <= 28; ++i)
    {
        node_ptr = bplib_rbt_search_unique(100 * i, &rbtree);
        if (i != 0 && !(i >= 3 && i <= 5) && i != 12 && i != 5)
        {
            rbtree_assert(node_ptr == &node_array[i].link);

            status = bplib_rbt_extract_node(&rbtree, node_ptr);
            rbtree_assert(status == BP_SUCCESS);

            printf("\n\nTree after remove %d\n", i);
            bplib_rbt_debug_print(&rbtree);
        }
        else
        {
            rbtree_assert(node_ptr == NULL);
        }
    }
}

int rbtest_comparator(const bplib_rbt_link_t *link, void *arg)
{
    rbtest_node_t *node;
    int            ref_val;

    node    = (rbtest_node_t *)link;
    ref_val = *((int *)arg);

    return (ref_val - node->internal_value);
}

void basic_test_nonunique(void)
{
    // int               i;
    bplib_rbt_iter_t  it;
    bp_val_t          curr_val;
    bp_val_t          last_val;
    rbtest_node_t    *rbtn_ptr;
    int               status;
    int               count;
    bplib_rbt_link_t *node_ptr;
    int               ref_val;

    /* Case 1, insert a value and then remove it */
    ref_val = 0;
    status  = bplib_rbt_insert_value_generic(100, &rbtree, &node_array[0].link, rbtest_comparator, &ref_val);
    rbtree_assert(status == BP_SUCCESS);
    bplib_rbt_debug_print(&rbtree);

    ref_val = 0;
    status  = bplib_rbt_insert_value_generic(100, &rbtree, &node_array[1].link, rbtest_comparator, &ref_val);
    rbtree_assert(status == BP_DUPLICATE);

    ref_val = 1;
    status  = bplib_rbt_insert_value_generic(100, &rbtree, &node_array[1].link, rbtest_comparator, &ref_val);
    rbtree_assert(status == BP_SUCCESS);

    ref_val  = 1;
    node_ptr = bplib_rbt_search_generic(99, &rbtree, rbtest_comparator, &ref_val);
    rbtree_assert(node_ptr == NULL);

    ref_val  = 0;
    node_ptr = bplib_rbt_search_generic(100, &rbtree, rbtest_comparator, &ref_val);
    rbtree_assert(node_ptr == &node_array[0].link);

    status = bplib_rbt_extract_node(&rbtree, node_ptr);
    rbtree_assert(status == BP_SUCCESS);

    ref_val  = 0;
    node_ptr = bplib_rbt_search_generic(100, &rbtree, rbtest_comparator, &ref_val);
    rbtree_assert(node_ptr == NULL);

    ref_val  = 1;
    node_ptr = bplib_rbt_search_generic(100, &rbtree, rbtest_comparator, &ref_val);
    rbtree_assert(node_ptr == &node_array[1].link);

    status = bplib_rbt_extract_node(&rbtree, node_ptr);
    rbtree_assert(status == BP_SUCCESS);

    /* part 2, create a case where the tree has duplicate keys but they are not next to each other */

    status = bplib_rbt_insert_value_unique(150, &rbtree, &node_array[150].link);
    rbtree_assert(status == BP_SUCCESS);
    status = bplib_rbt_insert_value_unique(160, &rbtree, &node_array[160].link);
    rbtree_assert(status == BP_SUCCESS);
    status = bplib_rbt_insert_value_unique(170, &rbtree, &node_array[170].link);
    rbtree_assert(status == BP_SUCCESS);
    status = bplib_rbt_insert_value_unique(180, &rbtree, &node_array[180].link);
    rbtree_assert(status == BP_SUCCESS);

    bplib_rbt_debug_print(&rbtree);

    ref_val = 165;
    status  = bplib_rbt_insert_value_generic(160, &rbtree, &node_array[165].link, rbtest_comparator, &ref_val);
    rbtree_assert(status == BP_SUCCESS);
    bplib_rbt_debug_print(&rbtree);

    ref_val = 155;
    status  = bplib_rbt_insert_value_generic(160, &rbtree, &node_array[155].link, rbtest_comparator, &ref_val);
    rbtree_assert(status == BP_SUCCESS);
    bplib_rbt_debug_print(&rbtree);

    /* iterate only items with key 160 */
    last_val = 160;
    count    = 0;
    status   = bplib_rbt_iter_goto_min(last_val, &rbtree, &it);
    while (status == BP_SUCCESS)
    {
        curr_val = bplib_rbt_get_key_value(it.position);
        if (curr_val != last_val)
        {
            break;
        }
        rbtn_ptr = (rbtest_node_t *)it.position;
        printf("iter node: %lu (%d)\n", curr_val, rbtn_ptr->internal_value);
        status = bplib_rbt_iter_next(&it);
        ++count;
    }

    rbtree_assert(count == 3);

    /* iterate all items except 180 in descending order */
    last_val = 175;
    count    = 0;
    status   = bplib_rbt_iter_goto_max(last_val, &rbtree, &it);
    while (status == BP_SUCCESS)
    {
        curr_val = bplib_rbt_get_key_value(it.position);
        rbtn_ptr = (rbtest_node_t *)it.position;
        printf("iter node: %lu (%d)\n", curr_val, rbtn_ptr->internal_value);
        rbtree_assert(curr_val <= last_val);
        status = bplib_rbt_iter_prev(&it);
        ++count;
    }

    rbtree_assert(count == 5);

    status = bplib_rbt_extract_node(&rbtree, &node_array[150].link);
    rbtree_assert(status == BP_SUCCESS);
    status = bplib_rbt_extract_node(&rbtree, &node_array[155].link);
    rbtree_assert(status == BP_SUCCESS);
    status = bplib_rbt_extract_node(&rbtree, &node_array[160].link);
    rbtree_assert(status == BP_SUCCESS);
    status = bplib_rbt_extract_node(&rbtree, &node_array[165].link);
    rbtree_assert(status == BP_SUCCESS);
    status = bplib_rbt_extract_node(&rbtree, &node_array[170].link);
    rbtree_assert(status == BP_SUCCESS);
    status = bplib_rbt_extract_node(&rbtree, &node_array[180].link);
    rbtree_assert(status == BP_SUCCESS);
}

void iterator_test(void)
{
    int               i;
    int               count;
    int               status;
    bp_val_t          curr_val;
    bp_val_t          last_val;
    bplib_rbt_link_t *node_ptr;
    bplib_rbt_iter_t  it;

    /* goal is to exercise the iterator APIs */
    /* Start by adding a bunch of nodes to a tree */
    for (i = 10; i < 90; i += 2)
    {
        status = bplib_rbt_insert_value_unique(i, &rbtree, &node_array[i].link);
        rbtree_assert(status == BP_SUCCESS);
    }

    /* iterate the entire tree, by supplying 0 as the minimum */
    last_val = 0;
    count    = 0;
    status   = bplib_rbt_iter_goto_min(last_val, &rbtree, &it);
    rbtree_assert(status == BP_SUCCESS);
    rbtree_assert(bplib_rbt_get_key_value(it.position) == 10);
    while (status == BP_SUCCESS)
    {
        ++count;
        curr_val = bplib_rbt_get_key_value(it.position);
        printf("iter node: %lu\n", curr_val);
        rbtree_assert(curr_val > last_val);
        last_val = curr_val;
        status   = bplib_rbt_iter_next(&it);
    }

    rbtree_assert(count == 40);

    /* iterate the entire tree, by supplying 100 as the maximum */
    last_val = 100;
    count    = 0;
    status   = bplib_rbt_iter_goto_max(last_val, &rbtree, &it);
    rbtree_assert(status == BP_SUCCESS);
    rbtree_assert(bplib_rbt_get_key_value(it.position) == 88);
    while (status == BP_SUCCESS)
    {
        ++count;
        curr_val = bplib_rbt_get_key_value(it.position);
        printf("iter node: %lu\n", curr_val);
        rbtree_assert(curr_val < last_val);
        last_val = curr_val;
        status   = bplib_rbt_iter_prev(&it);
    }

    rbtree_assert(count == 40);

    /* iterate partial tree, by supplying 50 as the minimum */
    last_val = 50;
    count    = 0;
    status   = bplib_rbt_iter_goto_min(last_val, &rbtree, &it);
    rbtree_assert(status == BP_SUCCESS);
    rbtree_assert(bplib_rbt_get_key_value(it.position) == 50);
    while (true)
    {
        ++count;
        if (bplib_rbt_iter_next(&it) != BP_SUCCESS)
        {
            break;
        }
        curr_val = bplib_rbt_get_key_value(it.position);
        printf("iter node: %lu\n", curr_val);
        rbtree_assert(curr_val > last_val);
        last_val = curr_val;
    }

    rbtree_assert(count == 20);

    /* iterate partial tree, by supplying 50 as the maximum */
    last_val = 50;
    count    = 0;
    status   = bplib_rbt_iter_goto_max(last_val, &rbtree, &it);
    rbtree_assert(status == BP_SUCCESS);
    rbtree_assert(bplib_rbt_get_key_value(it.position) == 50);
    while (true)
    {
        ++count;
        if (bplib_rbt_iter_prev(&it) != BP_SUCCESS)
        {
            break;
        }
        curr_val = bplib_rbt_get_key_value(it.position);
        printf("iter node: %lu\n", curr_val);
        rbtree_assert(curr_val < last_val);
        last_val = curr_val;
    }

    /* this gets one extra, because both 50 and 10 should be included */
    rbtree_assert(count == 21);

    /* check some other cases */
    last_val = 33;
    status   = bplib_rbt_iter_goto_max(last_val, &rbtree, &it);
    rbtree_assert(status == BP_SUCCESS);
    rbtree_assert(bplib_rbt_get_key_value(it.position) == 32);
    last_val = 59;
    status   = bplib_rbt_iter_goto_min(last_val, &rbtree, &it);
    rbtree_assert(status == BP_SUCCESS);
    rbtree_assert(bplib_rbt_get_key_value(it.position) == 60);

    last_val = 95;
    status   = bplib_rbt_iter_goto_min(last_val, &rbtree, &it);
    rbtree_assert(status != BP_SUCCESS);
    last_val = 5;
    status   = bplib_rbt_iter_goto_max(last_val, &rbtree, &it);
    rbtree_assert(status != BP_SUCCESS);

    for (i = 10; i < 90; i += 2)
    {
        node_ptr = bplib_rbt_search_unique(i, &rbtree);
        rbtree_assert(node_ptr != NULL);

        status = bplib_rbt_extract_node(&rbtree, node_ptr);
        rbtree_assert(status == BP_SUCCESS);
    }

    status = bplib_rbt_iter_goto_min(50, &rbtree, &it);
    rbtree_assert(status != BP_SUCCESS);
    status = bplib_rbt_iter_goto_max(50, &rbtree, &it);
    rbtree_assert(status != BP_SUCCESS);
}

void fuzz_stress_test(bool unique_only)
{
    int               fd;
    int               status;
    uint32_t          total_duplicates;
    uint32_t          total_inserts;
    uint32_t          total_extracts;
    uint16_t          value;
    uint8_t           operation;
    uint8_t           threshold;
    bool              is_add;
    rbtest_node_t    *node_ptr;
    bplib_rbt_link_t *removed_link_ptr;

    /*
     * Fuzz stress test -
     *
     * The goal here is to entirely fill and entirely empty the tree several times,
     * with random intermediate add/remove ops along the way, using source data that in
     * random order.
     *
     * This should exercise thousands of tree ops, catching all the various rebalancing
     * cases that are possible.
     */

    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        abort();
    }

    threshold        = 128;
    total_inserts    = 0;
    total_extracts   = 0;
    total_duplicates = 0;

    while (true)
    {
        if (head_used == NULL)
        {
            /* tree empty - MUST do add this time, from now on should prefer adds */
            rbtree_assert(total_inserts == total_extracts);

            /* Each time this returns to empty, the threshold for next pass increases */
            threshold = 263 - threshold; /* 135/256 chance of future adds (~52.7%) first time */
            is_add    = true;

            /* With an empty tree, we can end the test now, if sufficient work was done */
            if (threshold >= 170)
            {
                break;
            }
        }
        else if (head_free == NULL)
        {
            /* tree full - MUST do remove this time, from now on should prefer removes */
            rbtree_assert((total_inserts - total_extracts) == TEST_SIZE);

            threshold = 256 - threshold; /* 121/256 chance of future adds (~47.3%) first time */
            is_add    = false;
        }
        else
        {
            /*
             * normal/active range, randomly choose whether to add or remove, but weighted
             * toward one or the other based on threshold.  This should grandually fill (or empty)
             * the tree over time, but with plenty of intermediate adds/removes mixed in.
             */
            if (read(fd, &operation, sizeof(operation)) != sizeof(operation))
            {
                perror("read operation");
                abort();
            }

            /* This creates a higher chance of preferred op, lower chance of non-preferred op */
            is_add = (operation < threshold);
        }

        if (is_add)
        {
            if (read(fd, &value, sizeof(value)) != sizeof(value))
            {
                perror("read value");
                abort();
            }

            /* try to add another node to tree, get next block from free list */
            node_ptr           = head_free;
            head_free          = head_free->next_seq;
            node_ptr->next_seq = NULL;
            if (head_free == NULL)
            {
                /* last node in list */
                tail_free = NULL;
            }

            if (unique_only)
            {
                status = bplib_rbt_insert_value_unique(value, &rbtree, &node_ptr->link);
            }
            else
            {
                status = bplib_rbt_insert_value_generic(value, &rbtree, &node_ptr->link, rbtest_comparator,
                                                        &node_ptr->internal_value);
            }

            if (status == BP_SUCCESS)
            {
                ++total_inserts;

                /* append to "used" list */
                if (tail_used)
                {
                    tail_used->next_seq = node_ptr;
                }
                else
                {
                    /* first/only node in list */
                    head_used = node_ptr;
                }
                tail_used = node_ptr;
            }
            else
            {
                /* some of the random numbers will be duplicate, this is expected */
                rbtree_assert(unique_only);
                rbtree_assert(status == BP_DUPLICATE);
                ++total_duplicates;

                /* append back to "free" list */
                if (tail_free)
                {
                    tail_free->next_seq = node_ptr;
                }
                else
                {
                    /* first/only node in list */
                    head_free = node_ptr;
                }
                tail_free = node_ptr;
            }
        }
        else
        {
            /* try to remove node from tree, get next block from used list */
            node_ptr           = head_used;
            head_used          = head_used->next_seq;
            node_ptr->next_seq = NULL;
            if (head_used == NULL)
            {
                /* last node in list */
                tail_used = NULL;
            }

            /* this should always succeed, as the list only contains values that were added */
            value = bplib_rbt_get_key_value(&node_ptr->link);
            if (unique_only)
            {
                removed_link_ptr = bplib_rbt_search_unique(value, &rbtree);
            }
            else
            {
                removed_link_ptr =
                    bplib_rbt_search_generic(value, &rbtree, rbtest_comparator, &node_ptr->internal_value);
            }

            rbtree_assert(removed_link_ptr == &node_ptr->link);

            status = bplib_rbt_extract_node(&rbtree, removed_link_ptr);
            rbtree_assert(status == BP_SUCCESS);

            /* append back to "free" list */
            if (tail_free)
            {
                tail_free->next_seq = node_ptr;
            }
            else
            {
                /* first/only node in list */
                head_free = node_ptr;
            }
            tail_free = node_ptr;

            ++total_extracts;
        }

        /* after every operation, the R-B constraints should be satisfied */
        rbtree_assert(bplib_rbt_check_subtree(0, rbtree.root) == (1 + rbtree.black_height));

        /* periodically do an intermediate status report */
        if (((total_inserts + total_extracts + total_duplicates) & 0x7FF) == 0)
        {
            printf("Fuzz test running: insert=%u, extract=%u, duplicates=%u, size=%u\n", total_inserts, total_extracts,
                   total_duplicates, total_inserts - total_extracts);
        }
    }

    close(fd);

    /* this test stops at an empty tree, so insert == extract here (half the total) */
    printf("Fuzz test completed! Total insert+extract ops=%u, duplicates=%u\n", total_inserts + total_extracts,
           total_duplicates);
}

void prepare_test(void)
{
    int i;

    memset(&rbtree, 0, sizeof(rbtree));

    printf("Allocating memory for %d nodes\n", TEST_SIZE);
    node_array = malloc(sizeof(*node_array) * TEST_SIZE);
    if (node_array == NULL)
    {
        perror("malloc");
        abort();
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

/******************************************************************************
 * Main
 ******************************************************************************/
int main(int argc, char *argv[])
{
    prepare_test();
    basic_test_unique();
    basic_test_nonunique();
    iterator_test();
    fuzz_stress_test(true);
    fuzz_stress_test(false);

    return EXIT_SUCCESS;
}
