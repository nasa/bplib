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

/* A wrapper for a range from [value, value + offset]. */
typedef struct bp_fill_range {
    uint32_t value; /* The starting value in the range. */
    uint32_t offset; /* The offset from the value. */
} bp_fill;

/* A node in the red black tree. */
typedef struct bp_rb_node {
    uint32_t  value;   /* The value retained by the node. */
    uint32_t  offset;  /* Offset of value representing the range [value, value + offset). */
    bool      color;   /* The color of the node where RED (True) and BLACK (False).  */

    /* The respective ancestors of the current node within the red black tree.
       Child nodes that are null are assumed to be black in color.  The root node 
       of the red black tree has a null parent. */
    struct bp_rb_node* left;
    struct bp_rb_node* right;
    struct bp_rb_node* parent;
} bp_rb_node;

/* A wrapper around a red black trees nodes with some additional metadata. */
typedef struct bp_rb_tree {
    uint32_t size; /* The number of bp_rb_nodes within the tree. */
    uint32_t max_size; /* The maximum number of bp_rb_nodes within the tree. */
    bp_rb_node* root; /* The root of the tree. When root is null size is also 0. */
    bp_rb_node* free_node_head; /* The memory location of the first unallocated bp_rb_node. */
    bp_rb_node* free_node_tail; /* The memory location of the last unallocated bp_rb_node. */
    /* The starting memory address of the allocated memory for bp_rb_nodes. 
       This value is tracked so that the nodes can be deallocated in a single call to free. */
    bp_rb_node* node_block; 
} bp_rb_tree;

/* A status reflecting potential outcomes of a call to bp_rb_tree_insert. */
enum bp_rb_tree_status
{
    BP_RB_SUCCESS_INSERT,  /* Value was sucessfully inserted into the bp_rb_tree. */
    BP_RB_SUCCESS_MERGE,   /* Value was sucessfully merged into the tree. */
    BP_RB_FAIL_DUPLICATE,  /* Value was not inserted because a duplicate existed. */
    BP_RB_FAIL_FULL,       /* Value was not inserted because the bp_rb_tree was full. */
};

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Red Black Tree API */

/* Creates am empty bp_rb_tree. */
struct bp_rb_tree* bplib_rb_tree_create(int max_size, struct bp_rb_tree* tree);
/* Clears the nodes in a bp_rb_tree without deallocating any memory. */
void bplib_rb_tree_clear(struct bp_rb_tree* tree); 
/* Checks whether a bp_rb_tree is empty. */
bool bplib_rb_tree_is_empty(struct bp_rb_tree* tree); 
/* Checks whether a bp_rb_tree is full. */
bool bplib_rb_tree_is_full(struct bp_rb_tree* tree);
/* Inserts value into a red black tree. Duplicates will not be inserted. */
enum bp_rb_tree_status bplib_rb_tree_insert(uint32_t value, struct bp_rb_tree* tree);
/* Frees all memory allocated for a bp_rb_tree and recursively frees its nodes. */
void bplib_rb_tree_delete(struct bp_rb_tree* tree);
/* Removes the next bp_rb_node from the provided tree where next is defined as the node with
   the lowest value remaining in the tree. This function does not deallocate memory nor rebalance
   the tree and so all */
struct bp_fill_range get_next_bp_rb_node(struct bp_rb_tree* tree, bool should_delete_node);
#endif  /* __BPLIB_RB_TREE_H__ */
