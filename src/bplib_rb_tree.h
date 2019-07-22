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

/* A node in the red black tree. */
typedef struct rb_node {
    uint32_t  value;   /* The value retained by the node. */
    uint32_t  offset;  /* Offset of value representing the range [value, value + offset). */
    bool      color;   /* The color of the node where RED (True) and BLACK (False).  */

    /* The respective ancestors of the current node within the red black tree.
       Child nodes that are null are assumed to be black in color.  The root node 
       of the red black tree has a null parent. */
    struct rb_node* left;
    struct rb_node* right;
    struct rb_node* parent;
} rb_node;

/* A wrapper around a red black trees nodes with some additional metadata. */
typedef struct rb_tree {
    int size; /* The number of rb_nodes within the tree. */
    int max_size; /* The maximum number of rb_nodes within the tree. */
    rb_node* root; /* The root of the tree. When root is null size is also 0. */
    rb_node* free_node_head; /* The memory location of the first unallocated rb_node. */
    rb_node* free_node_tail; /* The memory location of the last unallocated rb_node. */
    /* The starting memory address of the allocated memory for rb_nodes. 
       This value is tracked so that the nodes can be deallocated in a single call to free. */
    rb_node* node_block; 
} rb_tree;

/* A status reflecting potential outcomes of a call to rb_tree_insert. */
enum rb_tree_status
{
    RB_SUCCESS_INSERT,  /* Value was sucessfully inserted into the rb_tree. */
    RB_SUCCESS_MERGE,   /* Value was sucessfully merged into the tree. */
    RB_FAIL_DUPLICATE,  /* Value was not inserted because a duplicate existed. */
    RB_FAIL_FULL,       /* Value was not inserted because the rb_tree was full. */
    RB_UNKNOWN_ERROR    /* An unknown error occured. */
};

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Red Black Tree API */

/* Creates am empty rb_tree. */
struct rb_tree* rb_tree_create(int max_size);
/* Clears all nodes in an rb_tree. This does not deallocate any memory. */
void rb_tree_clear(struct rb_tree* tree);
/* Checks whether a rb_tree is empty. */
bool rb_tree_is_empty(struct rb_tree* tree); 
/* Checks whether a rb_tree is full. */
bool rb_tree_is_full(struct rb_tree* tree);
/* Inserts value into a red black tree. Duplicates will not be inserted. */
enum rb_tree_status rb_tree_insert(uint32_t value, struct rb_tree* tree);
/* Frees all memory allocated for a rb_tree and recursively frees its nodes. */
void rb_tree_delete(struct rb_tree* tree);
/* Deletes a node and removes references to the node from its parent. 
   This should only be called on leaf nodes and does NOT result in any deletion rebalancing. 
   This function should only be called when converting the rb_tree to dacs. No memory is
   deallocated when using this function. */
void rb_node_delete_without_rebalancing(struct rb_tree* tree, struct rb_node* node);
#endif  /* __BPLIB_RB_TREE_H__ */
