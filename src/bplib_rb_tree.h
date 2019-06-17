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

// A node in the red black tree.
typedef struct rb_node {
    uint32_t  value;   // The value retained by the node.
    uint32_t  offset;  // Offset of value representing the range [value, value + offset).
    bool      color;   // The color of the node where RED (True) and BLACK (False). 

    /* The respective ancestors of the current node within the red black tree.
       Child nodes that are null are assumed to be black in color.  The root node 
       of the red black tree has a null parent. */
    struct rb_node* left;
    struct rb_node* right;
    struct rb_node* parent;
} rb_node;

// A wrapper around a red black trees nodes with some additional metadata.
typedef struct rb_tree {
    uint32_t size; // The number of rb_nodes within the tree.
    uint32_t max_size; // The maximum number of rb_nodes within the tree.
    rb_node* root; // The root of the tree. When root is null size is also 0.
    rb_node* free_node_head; // The memory location of the first unallocated rb_node.
    rb_node* free_node_tail; // The memory location of the last unallocated rb_node.
} rb_tree;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Red Black Tree API */

// Creates am empty rb_tree.
struct rb_tree* create_rb_tree(uint32_t max_size);
// Checks whether a rb_tree is empty.
bool is_empty(struct rb_tree* tree); 
// Checks whether a rb_tree is full.
bool is_full(struct rb_tree* tree);
// Inserts value into a red black tree. Duplicates will not be inserted.
bool insert(uint32_t value, struct rb_tree* tree);
#endif  /* __BPLIB_RB_TREE_H__ */
