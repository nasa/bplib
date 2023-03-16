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

#ifndef V7_MPOOL_BUNDLE_BLOCKS_H
#define V7_MPOOL_BUNDLE_BLOCKS_H

#include <string.h>

#include "v7_mpool.h"
#include "v7_types.h"

typedef struct bplib_mpool_bblock_tracking
{
    bplib_policy_delivery_t delivery_policy;
    bp_handle_t             ingress_intf_id;
    uint64_t                ingress_time;
    bp_handle_t             egress_intf_id;
    uint64_t                egress_time;
    bp_handle_t             storage_intf_id;
    bp_sid_t                committed_storage_id;

    /* JPHFIX: this is here for now, but really it belongs on the egress CLA intf based on its RTT */
    uint64_t local_retx_interval;
} bplib_mpool_bblock_tracking_t;

typedef struct bplib_mpool_bblock_primary_data
{
    bp_primary_block_t            logical;
    bplib_mpool_bblock_tracking_t delivery;
} bplib_mpool_bblock_primary_data_t;

struct bplib_mpool_bblock_primary
{
    bplib_mpool_block_t cblock_list;
    bplib_mpool_block_t chunk_list;
    size_t              block_encode_size_cache;
    size_t              bundle_encode_size_cache;

    bplib_mpool_bblock_primary_data_t data;
};

struct bplib_mpool_bblock_canonical
{
    bplib_mpool_block_t           chunk_list;
    bplib_mpool_bblock_primary_t *bundle_ref;
    size_t                        block_encode_size_cache;
    size_t                        encoded_content_offset;
    size_t                        encoded_content_length;
    bp_canonical_block_buffer_t   canonical_logical_data;
};

/**
 * @brief Gets the logical information associated with a primary block
 *
 * @param cpb
 * @return bp_primary_block_t*
 */
static inline bp_primary_block_t *bplib_mpool_bblock_primary_get_logical(bplib_mpool_bblock_primary_t *cpb)
{
    return &cpb->data.logical;
}

/**
 * @brief Gets the list of encoded chunks associated with a primary block
 *
 * @param cpb
 * @return bplib_mpool_block_t*
 */
static inline bplib_mpool_block_t *bplib_mpool_bblock_primary_get_encoded_chunks(bplib_mpool_bblock_primary_t *cpb)
{
    return &cpb->chunk_list;
}

/**
 * @brief Gets the list of canonical blocks associated with a primary block
 *
 * @param cpb
 * @return bplib_mpool_block_t*
 */
static inline bplib_mpool_block_t *bplib_mpool_bblock_primary_get_canonical_list(bplib_mpool_bblock_primary_t *cpb)
{
    return &cpb->cblock_list;
}

/**
 * @brief Gets the logical data associated with a canonical block
 *
 * @param ccb
 * @return bp_canonical_block_buffer_t*
 */
static inline bp_canonical_block_buffer_t *bplib_mpool_bblock_canonical_get_logical(bplib_mpool_bblock_canonical_t *ccb)
{
    return &ccb->canonical_logical_data;
}

/**
 * @brief Gets the list of encoded chunks associated with a canonical block
 *
 * @param ccb
 * @return bplib_mpool_block_t*
 */
static inline bplib_mpool_block_t *bplib_mpool_bblock_canonical_get_encoded_chunks(bplib_mpool_bblock_canonical_t *ccb)
{
    return &ccb->chunk_list;
}

/**
 * @brief Sets the offset and length of the content part of an encoded canonical block
 *
 * This refers to the position within the CBOR data of the actual content
 *
 * @param ccb
 * @param offset
 * @param length
 */
static inline void bplib_mpool_bblock_canonical_set_content_position(bplib_mpool_bblock_canonical_t *ccb, size_t offset,
                                                                     size_t length)
{
    ccb->encoded_content_offset = offset;
    ccb->encoded_content_length = length;
}

/**
 * @brief Gets the length of the content part of the encoded canonical block
 *
 * @param ccb
 * @return size_t
 */
static inline size_t bplib_mpool_bblock_canonical_get_content_length(const bplib_mpool_bblock_canonical_t *ccb)
{
    return ccb->encoded_content_length;
}

/**
 * @brief Gets the offset of the content part of the encoded canonical block
 *
 * @param ccb
 * @return size_t
 */
static inline size_t bplib_mpool_bblock_canonical_get_content_offset(const bplib_mpool_bblock_canonical_t *ccb)
{
    return ccb->encoded_content_offset;
}

/**
 * @brief Cast a block to a primary type
 *
 * Confirms that the block is actually a primary block (of any flavor) and returns
 * a pointer to that block, or returns NULL if not convertible to a primary block type.
 *
 * @param cb
 * @return bplib_mpool_bblock_primary_t*
 */
bplib_mpool_bblock_primary_t *bplib_mpool_bblock_primary_cast(bplib_mpool_block_t *cb);

/**
 * @brief Cast a block to a canonical type
 *
 * Confirms that the block is actually a canonical block (of any flavor) and returns
 * a pointer to that block, or returns NULL if not convertible to a canonical block type.
 *
 * @param cb
 * @return bplib_mpool_bblock_canonical_t*
 */
bplib_mpool_bblock_canonical_t *bplib_mpool_bblock_canonical_cast(bplib_mpool_block_t *cb);

/**
 * @brief Cast a block to a CBOR data type
 *
 * Confirms that the block is a CBOR data block, and returns a pointer to that block
 *
 * @param cb
 * @return bplib_mpool_generic_data_t*
 */
void *bplib_mpool_bblock_cbor_cast(bplib_mpool_block_t *cb);

void bplib_mpool_bblock_cbor_set_size(bplib_mpool_block_t *cb, size_t user_content_size);

/**
 * @brief Allocate a new primary block
 *
 * @param pool
 * @return bplib_mpool_block_t*
 */
bplib_mpool_block_t *bplib_mpool_bblock_primary_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg, uint8_t priority, uint64_t timeout);

/**
 * @brief Allocate a new canonical block
 *
 * @param pool
 * @return bplib_mpool_block_t*
 */
bplib_mpool_block_t *bplib_mpool_bblock_canonical_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg);

/**
 * @brief Allocate a new CBOR data block
 *
 * @param pool
 * @return bplib_mpool_block_t*
 */
bplib_mpool_block_t *bplib_mpool_bblock_cbor_alloc(bplib_mpool_t *pool);

/**
 * @brief Append CBOR data to the given list
 *
 * @param head
 * @param blk
 */
void bplib_mpool_bblock_cbor_append(bplib_mpool_block_t *head, bplib_mpool_block_t *blk);

/**
 * @brief Append a canonical block to the bundle
 *
 * @param cpb
 * @param blk
 */
void bplib_mpool_bblock_primary_append(bplib_mpool_bblock_primary_t *cpb, bplib_mpool_block_t *blk);

/**
 * @brief Find a canonical block within the bundle
 *
 * @param cpb
 * @param block_type
 */
bplib_mpool_block_t *bplib_mpool_bblock_primary_locate_canonical(bplib_mpool_bblock_primary_t *cpb,
                                                                 bp_blocktype_t                block_type);

/**
 * @brief Drop all encode data (CBOR) from a primary block
 *
 * This would be used if the logical data has been changed, necessitating re-encoding of the data.
 * The old data blocks are returned to the pool, and the updated contents can replace it.
 *
 * @param cpb
 */
void bplib_mpool_bblock_primary_drop_encode(bplib_mpool_bblock_primary_t *cpb);

/**
 * @brief Drop all encode data (CBOR) from a canonical block
 *
 * This would be used if the logical data has been changed, necessitating re-encoding of the data.
 * The old data blocks are returned to the pool, and the updated contents can replace it.
 *
 * @param ccb
 */
void bplib_mpool_bblock_canonical_drop_encode(bplib_mpool_bblock_canonical_t *ccb);

/**
 * @brief Copy an entire chain of encoded blocks to a target buffer
 *
 * @param list
 * @param out_ptr
 * @param max_out_size
 * @param seek_start
 * @param max_count
 * @return size_t
 */
size_t bplib_mpool_bblock_cbor_export(bplib_mpool_block_t *list, void *out_ptr, size_t max_out_size, size_t seek_start,
                                      size_t max_count);

#endif /* V7_MPOOL_BUNDLE_BLOCKS_H */
