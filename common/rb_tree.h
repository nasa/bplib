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

#ifndef _rb_tree_h_
#define _rb_tree_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* A wrapper for a range from [cid, cid + offset). */
typedef struct rb_range {
    bp_val_t    value;              /* The starting number in the range. */
    bp_val_t    offset;             /* The offset from the cid. */
} rb_range_t;

/* A node in the red black tree. */
typedef struct rb_node {
    rb_range_t  range;              /* The range of cids represented by the node. */
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
    rb_node_t*  iterator;       /* The current node when using iterator functions */
    /* The starting memory address of the allocated memory for rb_nodes. 
     * This value is tracked so that the nodes can be deallocated in a single call to free. */
    rb_node_t*  node_block; 
} rb_tree_t;

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

int     rb_tree_create      (bp_val_t max_size, rb_tree_t* tree);   /* Creates am empty rb_tree. */
int     rb_tree_clear       (rb_tree_t* tree);                      /* Clears the nodes in a rb_tree without deallocating any memory. */
bool    rb_tree_is_empty    (rb_tree_t* tree);                      /* Checks whether a rb_tree is empty. */
bool    rb_tree_is_full     (rb_tree_t* tree);                      /* Checks whether a rb_tree is full. */
int     rb_tree_insert      (bp_val_t value, rb_tree_t* tree);      /* Inserts number into a red black tree. Duplicates will not be inserted. */
int     rb_tree_delete      (bp_val_t value, rb_tree_t* tree);      /* Deletes a number from a rb_tree_t and may lead to split nodes. */
int     rb_tree_destroy     (rb_tree_t* tree);                      /* Frees all memory allocated for a rb_tree and recursively frees its nodes. */
int     rb_tree_goto_first  (rb_tree_t* tree);                      /* Gets the node of lowest cid in the tree to serve as an iterator to calls of get next. */
int     rb_tree_get_next    (rb_tree_t* tree, rb_range_t* range, bool should_pop, bool should_rebalance); /* Gets the next range in order in the rb_tree_t and increments the iterator. */

#endif  /* _rb_tree_h_ */
