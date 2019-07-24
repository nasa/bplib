/************************************************************************
 * File: bplib_rb_tree.h
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * Maintainer(s):
 *  Alexander Meade, Code 582 NASA GSFC
 *
 *************************************************************************/

#ifndef __BPLIB_RB_TREE_H__
#define __BPLIB_RB_TREE_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* A wrapper for a range from [value, value + offset). */
typedef struct bp_rb_range {
    uint32_t value; /* The starting value in the range. */
    uint32_t offset; /* The offset from the value. */
} bp_rb_range_t;

/* A node in the red black tree. */
typedef struct bp_rb_node {
    bp_rb_range_t range; /* The range of values represented by the node. */
    bool           color;   /* The color of the node where RED (True) and BLACK (False).  */
    bool traversal_state; /* Tracks when a node is visited in a tree traversal. */
    
    /* The respective ancestors of the current node within the red black tree.
       Child nodes that are null are assumed to be black in color.  The root node 
       of the red black tree has a null parent. */
    struct bp_rb_node* left;
    struct bp_rb_node* right;
    struct bp_rb_node* parent;
} bp_rb_node_t;

/* A wrapper around a red black trees nodes with some additional metadata. */
typedef struct bp_rb_tree {
    uint32_t size; /* The number of bp_rb_nodes within the tree. */
    uint32_t max_size; /* The maximum number of bp_rb_nodes within the tree. */
    bp_rb_node_t* root; /* The root of the tree. When root is null size is also 0. */
    bp_rb_node_t* free_node_head; /* The memory location of the first unallocated bp_rb_node. */
    bp_rb_node_t* free_node_tail; /* The memory location of the last unallocated bp_rb_node. */
    /* The starting memory address of the allocated memory for bp_rb_nodes. 
       This value is tracked so that the nodes can be deallocated in a single call to free. */
    bp_rb_node_t* node_block; 
} bp_rb_tree_t;

/* A status reflecting potential outcomes of a call to bp_rb_tree_insert. */
typedef enum bp_rb_tree_status
{
    BP_RB_SUCCESS,                  /* Success. */
    BP_RB_FAIL_INSERT_DUPLICATE,    /* Value was not inserted because a duplicate existed. */
    BP_RB_FAIL_TREE_FULL,           /* Value was not inserted because the bp_rb_tree was full. */
    BP_RB_FAIL_SIZE_ZERO,           /* Maximum size of the bp_rb_tree_t was set to zero. */
    BP_RB_FAIL_EXCEEDED_MAX_SIZE,   /* Exceeded the maximum allocatable size of the tree. */
    BP_RB_FAIL_NULL_TREE,           /* The provided bp_rb_tree_t was NULL. */
    BP_RB_FAIL_MEM_ERR,             /* No memory was allocated. */
    BP_RB_FAIL_NULL_NODE,           /* The provided bp_rb_node_t was NULL. */ 
    BP_RB_FAIL_NULL_RANGE,          /* The provided bp_rb_range_t was NULL. */
    BP_RB_FAIL_VALUE_NOT_FOUND      /* The provided value did not exist in the tree. */
} bp_rb_tree_status_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Red Black Tree API 

NOTE: The bp_rb_tree_t is limited by its maximum size data type, in this case a uint32_t.
Since ranges are being stored no range will ever exceed UINT32_MAX nor will the tree be able
to allocate more than UINT32_MAX nodes even if the memory is available.
*/

/* Creates am empty bp_rb_tree. */
bp_rb_tree_status_t bplib_rb_tree_create(uint32_t max_size, bp_rb_tree_t* tree);
/* Clears the nodes in a bp_rb_tree without deallocating any memory. */
bp_rb_tree_status_t bplib_rb_tree_clear(bp_rb_tree_t* tree); 
/* Checks whether a bp_rb_tree is empty. */
bool bplib_rb_tree_is_empty(bp_rb_tree_t* tree); 
/* Checks whether a bp_rb_tree is full. */
bool bplib_rb_tree_is_full(bp_rb_tree_t* tree);
/* Inserts value into a red black tree. Duplicates will not be inserted. */
bp_rb_tree_status_t bplib_rb_tree_insert(uint32_t value, bp_rb_tree_t* tree);
/* Frees all memory allocated for a bp_rb_tree and recursively frees its nodes. */
bp_rb_tree_status_t bplib_rb_tree_destroy(bp_rb_tree_t* tree);
/* Gets the node of lowest value in the tree to serve as an iterator to calls of get next.*/
bp_rb_tree_status_t bplib_rb_tree_get_first_rb_node(bp_rb_tree_t* tree,
                                                    bp_rb_node_t** iter)
/* Gets the next range inorder in the bp_rb_tree_t and increments the iterator. */
bp_rb_tree_status_t bplib_rb_tree_get_next_rb_node(bp_rb_tree_t* tree,
                                                   bp_rb_node_t** iter,
                                                   bp_rb_range_t* range,
                                                   bool should_pop,
                                                   bool should_rebalance);
#endif  /* __BPLIB_RB_TREE_H__ */
