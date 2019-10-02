/************************************************************************
 * File: rb_tree.h
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

#ifndef __BPLIB_RB_TREE_H__
#define __BPLIB_RB_TREE_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_os.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* A wrapper for a range from [value, value + offset). */
typedef struct rb_range {
    bp_val_t    value;              /* The starting value in the range. */
    bp_val_t    offset;             /* The offset from the value. */
} rb_range_t;

/* A node in the red black tree. */
typedef struct rb_node {
    rb_range_t  range;              /* The range of values represented by the node. */
    bool        color;              /* The color of the node where RED (True) and BLACK (False).  */
    bool        traversal_state;    /* Tracks when a node is visited in a tree traversal. */    
    /* The respective ancestors of the current node within the red black tree.
     * Child nodes that are null are assumed to be black in color.  The root node 
     * of the red black tree has a null parent. */
    struct rb_node* left;
    struct rb_node* right;
    struct rb_node* parent;
} rb_node_t;

/* A wrapper around a red black tree's nodes with some additional metadata. */
typedef struct rb_tree {
    bp_val_t    size;           /* The number of rb_nodes within the tree. */
    bp_val_t    max_size;       /* The maximum number of rb_nodes within the tree. */
    rb_node_t*  root;           /* The root of the tree. When root is null size is also 0. */
    rb_node_t*  free_node_head; /* The memory location of the first unallocated rb_node. */
    rb_node_t*  free_node_tail; /* The memory location of the last unallocated rb_node. */
    /* The starting memory address of the allocated memory for rb_nodes. 
     * This value is tracked so that the nodes can be deallocated in a single call to free. */
    rb_node_t*  node_block; 
} rb_tree_t;

/* A status reflecting potential outcomes of a call to rb_tree_insert. */
typedef enum rb_tree_status {
    RB_SUCCESS,                  /* Success. */
    RB_FAIL_INSERT_DUPLICATE,    /* Value was not inserted because a duplicate existed. */
    RB_FAIL_TREE_FULL,           /* Value was not inserted because the rb_tree was full. */
    RB_FAIL_SIZE_ZERO,           /* Maximum size of the rb_tree_t was set to zero. */
    RB_FAIL_EXCEEDED_MAX_SIZE,   /* Exceeded the maximum allocatable size of the tree. */
    RB_FAIL_NULL_TREE,           /* The provided rb_tree_t was NULL. */
    RB_FAIL_MEM_ERR,             /* No memory was allocated. */
    RB_FAIL_NULL_NODE,           /* The provided rb_node_t was NULL. */ 
    RB_FAIL_NULL_RANGE,          /* The provided rb_range_t was NULL. */
    RB_FAIL_VALUE_NOT_FOUND      /* The provided value did not exist in the tree. */
} rb_tree_status_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Red Black Tree API 
 *
 * NOTE: The rb_tree_t is limited by its maximum size data type, in this case a bp_val_t.
 *       Since ranges are being stored no range will ever exceed BP_MAX_ENCODED_VALUE nor will the tree be able
 *       to allocate more than BP_MAX_ENCODED_VALUE nodes even if the memory is available.
 * 
 */

rb_tree_status_t    rb_tree_create      (bp_val_t max_size, rb_tree_t* tree);   /* Creates am empty rb_tree. */
rb_tree_status_t    rb_tree_clear       (rb_tree_t* tree);                      /* Clears the nodes in a rb_tree without deallocating any memory. */
bool                rb_tree_is_empty    (rb_tree_t* tree);                      /* Checks whether a rb_tree is empty. */
bool                rb_tree_is_full     (rb_tree_t* tree);                      /* Checks whether a rb_tree is full. */
rb_tree_status_t    rb_tree_insert      (bp_val_t value, rb_tree_t* tree);      /* Inserts value into a red black tree. Duplicates will not be inserted. */
rb_tree_status_t    rb_tree_delete      (bp_val_t value, rb_tree_t* tree);      /* Deletes a value from a rb_tree_t and may lead to split nodes. */
rb_tree_status_t    rb_tree_destroy     (rb_tree_t* tree);                      /* Frees all memory allocated for a rb_tree and recursively frees its nodes. */
rb_tree_status_t    rb_tree_get_first   (rb_tree_t* tree, rb_node_t** iter);    /* Gets the node of lowest value in the tree to serve as an iterator to calls of get next. */
rb_tree_status_t    rb_tree_get_next    (rb_tree_t* tree, rb_node_t** iter, rb_range_t* range, bool should_pop, bool should_rebalance); /* Gets the next range inorder in the rb_tree_t and increments the iterator. */

#endif  /* __BPLIB_RB_TREE_H__ */
