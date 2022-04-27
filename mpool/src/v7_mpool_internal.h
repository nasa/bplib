/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef V7_MPOOL_INTERNAL_H
#define V7_MPOOL_INTERNAL_H

#include <string.h>

#include "bplib_api_types.h"
#include "v7_rbtree.h"
#include "v7_mpool.h"
#include "v7_mpool_bblocks.h"
#include "v7_mpool_flows.h"
#include "v7_mpool_ref.h"

/*
 * Minimum size of a generic data block
 */
#define BP_MPOOL_MIN_USER_BLOCK_SIZE 320

typedef struct bplib_mpool_block_header
{
    bplib_mpool_block_t base_link; /* must be first - this is the pointer used in the application */

    uint32_t content_type_signature; /* a "signature" (sanity check) value for identifying the data */
    uint16_t user_content_length;    /* actual length of user content (does not include fixed fields) */
    uint16_t refcount;               /* number of active references to the object */

} bplib_mpool_block_header_t;

/*
 * This union is just to make sure that the "content_start" field
 * in a generic data block is suitably aligned to hold any data type, mainly
 * the max-sized integers, max-sized floating point values, or a pointer.
 */
typedef union bplib_mpool_aligned_data
{
    uint8_t     first_octet;
    uintmax_t   align_int;
    void       *align_ptr;
    long double align_float;
} bplib_mpool_aligned_data_t;

typedef struct bplib_mpool_api_content
{
    bplib_rbt_link_t            rbt_link;
    bplib_mpool_blocktype_api_t api;
    size_t                      user_content_size;
    bplib_mpool_aligned_data_t  user_data_start;
} bplib_mpool_api_content_t;

typedef struct bplib_mpool_generic_data_content
{
    bplib_mpool_aligned_data_t user_data_start;
} bplib_mpool_generic_data_content_t;

typedef struct bplib_mpool_bblock_primary_content
{
    bplib_mpool_bblock_primary_t pblock;
    bplib_mpool_aligned_data_t   user_data_start;
} bplib_mpool_bblock_primary_content_t;

typedef struct bplib_mpool_bblock_canonical_content
{
    bplib_mpool_bblock_canonical_t cblock;
    bplib_mpool_aligned_data_t     user_data_start;
} bplib_mpool_bblock_canonical_content_t;

typedef struct bplib_mpool_flow_content
{
    bplib_mpool_block_t        processable_link;
    bplib_mpool_flow_t         fblock;
    bplib_mpool_aligned_data_t user_data_start;
} bplib_mpool_flow_content_t;

typedef struct bplib_mpool_block_ref_content
{
    bplib_mpool_ref_t          pref_target;
    bplib_mpool_aligned_data_t user_data_start;
} bplib_mpool_block_ref_content_t;

typedef union bplib_mpool_block_buffer
{
    bplib_mpool_generic_data_content_t     generic_data;
    bplib_mpool_api_content_t              api;
    bplib_mpool_bblock_primary_content_t   primary;
    bplib_mpool_bblock_canonical_content_t canonical;
    bplib_mpool_flow_content_t             flow;
    bplib_mpool_block_ref_content_t        ref;

    /* guarantees a minimum size of the generic data blocks, also determines the amount
     * of extra space available for user objects in other types of blocks. */
    uint8_t content_bytes[BP_MPOOL_MIN_USER_BLOCK_SIZE];
} bplib_mpool_block_buffer_t;

typedef struct bplib_mpool_block_content
{
    bplib_mpool_block_header_t header; /* must be first */
    bplib_mpool_block_buffer_t u;
} bplib_mpool_block_content_t;

struct mpool
{
    size_t   buffer_size;
    uint32_t num_bufs_total;
    uint32_t alloc_threshold;
    uint32_t alloc_count;
    uint32_t recycled_count;

    bplib_mpool_api_content_t blocktype_basic;
    bplib_rbt_root_t          xblocktype_registry;

    bplib_mpool_block_t free_blocks;
    bplib_mpool_block_t ref_managed_blocks;
    bplib_mpool_block_t recycle_blocks;
    bplib_mpool_block_t active_flow_list;

    bplib_mpool_block_content_t first_buffer;
};

#define MPOOL_GET_BUFFER_USER_START_OFFSET(m) (offsetof(bplib_mpool_block_buffer_t, m.user_data_start))

#define MPOOL_GET_BLOCK_USER_CAPACITY(m) (sizeof(bplib_mpool_block_buffer_t) - MPOOL_GET_BUFFER_USER_START_OFFSET(m))

void bplib_mpool_bblock_primary_init(bplib_mpool_bblock_primary_t *pblk);
void bplib_mpool_bblock_canonical_init(bplib_mpool_bblock_canonical_t *cblk);
void bplib_mpool_subq_init(bplib_mpool_subq_t *qblk);
void bplib_mpool_flow_init(bplib_mpool_flow_t *fblk);

bplib_mpool_block_content_t *bplib_mpool_get_block_content(bplib_mpool_block_t *cb);
bplib_mpool_block_content_t *bplib_mpool_alloc_block_internal(bplib_mpool_t *pool, bplib_mpool_blocktype_t blocktype,
                                                              uint32_t content_type_signature, void *init_arg);

#endif /* V7_MPOOL_INTERNAL_H */
