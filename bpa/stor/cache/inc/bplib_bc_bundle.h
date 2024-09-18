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

#ifndef BPLIB_MEM_BBLOCKS_H
#define BPLIB_MEM_BBLOCKS_H

#include <string.h>

#include "bplib.h"
#include "bplib_mem.h"
#include "bplib_api_types.h"
#include "bplib_mem_bundle.h"

typedef struct BPLib_STOR_CACHE_BblockTracking
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
} BPLib_STOR_CACHE_BblockTracking_t;

typedef struct BPLib_STOR_CACHE_BblockPrimaryData
{
    bp_primary_block_t            logical;
    BPLib_STOR_CACHE_BblockTracking_t delivery;
} BPLib_STOR_CACHE_BblockPrimaryData_t;

struct BPLib_STOR_CACHE_BblockPrimary
{
    BPLib_MEM_block_t cblock_list;
    BPLib_MEM_block_t chunk_list;
    size_t              block_encode_size_cache;
    size_t              bundle_encode_size_cache;

    BPLib_STOR_CACHE_BblockPrimaryData_t data;
};

struct BPLib_STOR_CACHE_BblockCanonical
{
    BPLib_MEM_block_t           chunk_list;
    BPLib_STOR_CACHE_BblockPrimary_t *bundle_ref;
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
static inline bp_primary_block_t *BPLib_STOR_CACHE_BblockPrimaryGetLogical(BPLib_STOR_CACHE_BblockPrimary_t *cpb)
{
    return &cpb->data.logical;
}

/**
 * @brief Gets the list of encoded chunks associated with a primary block
 *
 * @param cpb
 * @return BPLib_MEM_block_t*
 */
static inline BPLib_MEM_block_t *BPLib_STOR_CACHE_BblockPrimaryGetEncodedChunks(BPLib_STOR_CACHE_BblockPrimary_t *cpb)
{
    return &cpb->chunk_list;
}

/**
 * @brief Gets the list of canonical blocks associated with a primary block
 *
 * @param cpb
 * @return BPLib_MEM_block_t*
 */
static inline BPLib_MEM_block_t *BPLib_STOR_CACHE_BblockPrimaryGetCanonicalList(BPLib_STOR_CACHE_BblockPrimary_t *cpb)
{
    return &cpb->cblock_list;
}

/**
 * @brief Gets the logical data associated with a canonical block
 *
 * @param ccb
 * @return bp_canonical_block_buffer_t*
 */
static inline bp_canonical_block_buffer_t *BPLib_STOR_CACHE_BblockCanonicalGetLogical(BPLib_STOR_CACHE_BblockCanonical_t *ccb)
{
    return &ccb->canonical_logical_data;
}

/**
 * @brief Gets the list of encoded chunks associated with a canonical block
 *
 * @param ccb
 * @return BPLib_MEM_block_t*
 */
static inline BPLib_MEM_block_t *BPLib_STOR_CACHE_BblockCanonicalGetEncodedChunks(BPLib_STOR_CACHE_BblockCanonical_t *ccb)
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
static inline void BPLib_STOR_CACHE_BblockCanonicalSetContentPosition(BPLib_STOR_CACHE_BblockCanonical_t *ccb, size_t offset,
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
static inline size_t BPLib_STOR_CACHE_BblockCanonicalGetContentLength(const BPLib_STOR_CACHE_BblockCanonical_t *ccb)
{
    return ccb->encoded_content_length;
}

/**
 * @brief Gets the offset of the content part of the encoded canonical block
 *
 * @param ccb
 * @return size_t
 */
static inline size_t BPLib_STOR_CACHE_BblockCanonicalGetContentOffset(const BPLib_STOR_CACHE_BblockCanonical_t *ccb)
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
 * @return BPLib_STOR_CACHE_BblockPrimary_t*
 */
BPLib_STOR_CACHE_BblockPrimary_t *BPLib_STOR_CACHE_BblockPrimaryCast(BPLib_MEM_block_t *cb);

/**
 * @brief Cast a block to a canonical type
 *
 * Confirms that the block is actually a canonical block (of any flavor) and returns
 * a pointer to that block, or returns NULL if not convertible to a canonical block type.
 *
 * @param cb
 * @return BPLib_STOR_CACHE_BblockCanonical_t*
 */
BPLib_STOR_CACHE_BblockCanonical_t *BPLib_STOR_CACHE_BblockCanonicalCast(BPLib_MEM_block_t *cb);

/**
 * @brief Cast a block to a CBOR data type
 *
 * Confirms that the block is a CBOR data block, and returns a pointer to that block
 *
 * @param cb
 * @return BPLib_STOR_CACHE_GenericData_t*
 */
void *BPLib_STOR_CACHE_BblockCborCast(BPLib_MEM_block_t *cb);

void BPLib_STOR_CACHE_BblockCborSetSize(BPLib_MEM_block_t *cb, size_t user_content_size);

/**
 * @brief Allocate a new primary block
 *
 * @param pool
 * @return BPLib_MEM_block_t*
 */
BPLib_MEM_block_t *BPLib_STOR_CACHE_BblockPrimaryAlloc(BPLib_MEM_t *pool, uint32_t magic_number, void *init_arg, uint8_t priority, uint64_t timeout);

/**
 * @brief Allocate a new canonical block
 *
 * @param pool
 * @return BPLib_MEM_block_t*
 */
BPLib_MEM_block_t *BPLib_STOR_CACHE_BblockCanonicalAlloc(BPLib_MEM_t *pool, uint32_t magic_number, void *init_arg);

/**
 * @brief Allocate a new CBOR data block
 *
 * @param pool
 * @return BPLib_MEM_block_t*
 */
BPLib_MEM_block_t *BPLib_STOR_CACHE_BblockCborAlloc(BPLib_MEM_t *pool);

/**
 * @brief Append CBOR data to the given list
 *
 * @param head
 * @param blk
 */
void BPLib_STOR_CACHE_BblockCborAppend(BPLib_MEM_block_t *head, BPLib_MEM_block_t *blk);

/**
 * @brief Append a canonical block to the bundle
 *
 * @param cpb
 * @param blk
 */
void BPLib_STOR_CACHE_BblockPrimaryAppend(BPLib_STOR_CACHE_BblockPrimary_t *cpb, BPLib_MEM_block_t *blk);

/**
 * @brief Find a canonical block within the bundle
 *
 * @param cpb
 * @param block_type
 */
BPLib_MEM_block_t *BPLib_STOR_CACHE_BblockPrimaryLocateCanonical(BPLib_STOR_CACHE_BblockPrimary_t *cpb,
                                                                 bp_blocktype_t                block_type);

/**
 * @brief Drop all encode data (CBOR) from a primary block
 *
 * This would be used if the logical data has been changed, necessitating re-encoding of the data.
 * The old data blocks are returned to the pool, and the updated contents can replace it.
 *
 * @param cpb
 */
void BPLib_STOR_CACHE_BblockPrimaryDropEncode(BPLib_STOR_CACHE_BblockPrimary_t *cpb);

/**
 * @brief Drop all encode data (CBOR) from a canonical block
 *
 * This would be used if the logical data has been changed, necessitating re-encoding of the data.
 * The old data blocks are returned to the pool, and the updated contents can replace it.
 *
 * @param ccb
 */
void BPLib_STOR_CACHE_BblockCanonicalDropEncode(BPLib_STOR_CACHE_BblockCanonical_t *ccb);

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
size_t BPLib_STOR_CACHE_BblockCborExport(BPLib_MEM_block_t *list, void *out_ptr, size_t max_out_size, size_t seek_start,
                                      size_t max_count);

#endif /* BPLIB_MEM_BBLOCKS_H */
