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
#include <string.h>
#include <ctype.h>

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "osapi.h"

#include "bplib_api_types.h"
#include "bplib_rbtree.h"

#define TEST_SIZE 2150

typedef struct rbtest_node
{
    BPLib_STOR_MEM_RBT_Link_t    link;
    int                 internal_value;
    struct rbtest_node *next_seq;
} rbtest_node_t;

BPLib_STOR_MEM_RBT_Root_t rbtree;

rbtest_node_t *node_array;

rbtest_node_t *head_free;
rbtest_node_t *tail_free;
rbtest_node_t *head_used;
rbtest_node_t *tail_used;

osal_id_t rand_fd;

/* Confirm the subtree confirms to the R-B tree constriants */
int BPLib_STOR_MEM_RBT_CheckSubtree(int node_print_depth, BPLib_STOR_MEM_RBT_Link_t *node)
{
    char           my_color;
    int            my_black_height;
    int            left_black_height;
    int            right_black_height;
    rbtest_node_t *rbtn_ptr;

    left_black_height  = 0;
    right_black_height = 0;
    my_black_height    = 0;

    if (BPLib_STOR_MEM_RBT_NodeIsRed(node))
    {
        /* the current node is red - the parent must not also be red.
         * this is one of the core constraints of the R-B tree algorithm */

        /* Note this only prints the assertion if not true, to avoid cluttering the log */
        if (BPLib_STOR_MEM_RBT_NodeIsRed(node->parent))
        {
            UtAssert_Failed("Red-Red constraint violated");
        }
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
        right_black_height = BPLib_STOR_MEM_RBT_CheckSubtree(node_print_depth, node->right);
        my_black_height += right_black_height;
        if (node_print_depth > 0)
        {
            rbtn_ptr = (rbtest_node_t *)node;
            UtPrintf("%*s%-4lu[%-4d](%c/%d)\n", (node_print_depth - 1) * 3, "", BPLib_STOR_MEM_RBT_GetKeyValue(node),
                     rbtn_ptr->internal_value, my_color, my_black_height);
        }

        left_black_height = BPLib_STOR_MEM_RBT_CheckSubtree(node_print_depth, node->left);

        /* Confirm the other constraint of an R-B tree, which is that the left side
         * and right side should have an equal black height. */
        if (left_black_height != right_black_height)
        {
            UtAssert_Failed("Black height constraint violated");
        }
    }

    return my_black_height;
}

int rbtest_comparator(const BPLib_STOR_MEM_RBT_Link_t *link, void *arg)
{
    rbtest_node_t *node;
    int            ref_val;

    node    = (rbtest_node_t *)link;
    ref_val = *((int *)arg);

    return (ref_val - node->internal_value);
}

void bplib_rbtree_fuzz_setup(void)
{
    UtAssert_INT32_EQ(OS_OpenCreate(&rand_fd, "/dev/urandom", 0, OS_READ_ONLY), OS_SUCCESS);
}

void bplib_rbtree_fuzz_teardown(void)
{
    UtAssert_INT32_EQ(OS_close(rand_fd), OS_SUCCESS);
}

void bplib_rbtree_fuzz_stress_test(bool unique_only)
{
    int               status;
    uint32_t          total_duplicates;
    uint32_t          total_inserts;
    uint32_t          total_extracts;
    uint16_t          value;
    uint8_t           operation;
    uint8_t           threshold;
    bool              is_add;
    rbtest_node_t    *node_ptr;
    BPLib_STOR_MEM_RBT_Link_t *removed_link_ptr;

    /*
     * Fuzz stress test -
     *
     * The goal here is to entirely fill and entirely empty the tree several times,
     * with random intermediate add/remove ops along the way, using source data that in
     * random order.
     *
     * This should exercise thousands of tree ops, catching all the various rebalancing
     * cases that are possible.
     *
     * NOTE: UtAssert macros will add an entry on the log, even for success cases.  Because
     * this does thousands of ops, in the active mode this checks the results locally
     * and directly uses UtAssert_Failed() if not correct.
     */

    threshold        = 128;
    total_inserts    = 0;
    total_extracts   = 0;
    total_duplicates = 0;

    while (true)
    {
        if (head_used == NULL)
        {
            /* tree empty - MUST do add this time, from now on should prefer adds */
            UtAssert_UINT32_EQ(total_inserts, total_extracts);

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
            UtAssert_UINT32_EQ((total_inserts - total_extracts), TEST_SIZE);

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
            if (OS_read(rand_fd, &operation, sizeof(operation)) != sizeof(operation))
            {
                UtAssert_Abort("OS_read() failed");
            }

            /* This creates a higher chance of preferred op, lower chance of non-preferred op */
            is_add = (operation < threshold);
        }

        if (is_add)
        {
            if (OS_read(rand_fd, &value, sizeof(value)) != sizeof(value))
            {
                UtAssert_Abort("OS_read() failed");
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
                status = BPLib_STOR_MEM_RBT_InsertValueUnique(value, &rbtree, &node_ptr->link);
            }
            else
            {
                status = BPLib_STOR_MEM_RBT_InsertValueGeneric(value, &rbtree, &node_ptr->link, rbtest_comparator,
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
                if (status != BP_DUPLICATE)
                {
                    UtAssert_Failed("insert failure other than BP_DUPLICATE: %d\n", (int)status);
                }
                else if (!unique_only)
                {
                    UtAssert_Failed("insert failure on non-unique tree?");
                }

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
            value = BPLib_STOR_MEM_RBT_GetKeyValue(&node_ptr->link);
            if (unique_only)
            {
                removed_link_ptr = BPLib_STOR_MEM_RBT_SearchUnique(value, &rbtree);
            }
            else
            {
                removed_link_ptr =
                    BPLib_STOR_MEM_RBT_SearchGeneric(value, &rbtree, rbtest_comparator, &node_ptr->internal_value);
            }

            if (removed_link_ptr != &node_ptr->link)
            {
                UtAssert_Failed("Removed link pointer mismatch: %p/%p", (void *)removed_link_ptr,
                                (void *)&node_ptr->link);
            }

            status = BPLib_STOR_MEM_RBT_ExtractNode(&rbtree, removed_link_ptr);
            if (status != BP_SUCCESS)
            {
                UtAssert_Failed("BPLib_STOR_MEM_RBT_ExtractNode() failed: %d", (int)status);
            }

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
        if (BPLib_STOR_MEM_RBT_CheckSubtree(0, rbtree.root) != (1 + rbtree.black_height))
        {
            UtAssert_Failed("RB black height mismatch");
        }

        /* periodically do an intermediate status report */
        if (((total_inserts + total_extracts + total_duplicates) & 0x7FF) == 0)
        {
            UtPrintf("Fuzz test running: insert=%u, extract=%u, duplicates=%u, size=%u\n", total_inserts,
                     total_extracts, total_duplicates, total_inserts - total_extracts);
        }
    }

    /* this test stops at an empty tree, so insert == extract here (half the total) */
    UtPrintf("Fuzz test completed! Total insert+extract ops=%u, duplicates=%u\n", total_inserts + total_extracts,
             total_duplicates);
}

void prepare_test(void)
{
    int       i;
    osal_id_t fs_id;

    UtAssert_INT32_EQ(OS_API_Init(), OS_SUCCESS);

    /* Slight cheat/hack here - since this is most likely to run on Linux, until
     * OSAL gets a more proper random number facility, mapping the /dev filesystem
     * allows this application to open /dev/urandom to get the random numbers
     * needed for fuzz testing */
    UtAssert_INT32_EQ(OS_FileSysAddFixedMap(&fs_id, "/dev", "/dev"), OS_SUCCESS);

    memset(&rbtree, 0, sizeof(rbtree));

    UtPrintf("Allocating memory for %d nodes\n", TEST_SIZE);
    node_array = malloc(sizeof(*node_array) * TEST_SIZE);
    if (!UtAssert_NOT_NULL(node_array))
    {
        UtAssert_Abort("malloc() failed");
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

void bplib_rbtree_fuzz_unique(void)
{
    bplib_rbtree_fuzz_stress_test(true);
}

void bplib_rbtree_fuzz_nonunique(void)
{
    bplib_rbtree_fuzz_stress_test(false);
}

/******************************************************************************
 * Main
 ******************************************************************************/
void UtTest_Setup(void)
{
    prepare_test();

    UtTest_Add(bplib_rbtree_fuzz_unique, bplib_rbtree_fuzz_setup, bplib_rbtree_fuzz_teardown, "fuzz unique");
    UtTest_Add(bplib_rbtree_fuzz_nonunique, bplib_rbtree_fuzz_setup, bplib_rbtree_fuzz_teardown, "fuzz nonunique");
}
