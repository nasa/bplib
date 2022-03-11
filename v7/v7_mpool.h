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

#ifndef V7_MPOOL_H
#define V7_MPOOL_H

#include <string.h>

#include "bplib.h"
#include "v7_types.h"
#include "crc.h"

/*
 * A note about encoded chunk size -
 * All data chunks, including logical (non-encoded) primary and canonical block info,
 * as well as encoded blobs, get stored in the same size record.  These are then chained
 * together to hold larger items.
 */
#define BP_MPOOL_MAX_ENCODED_CHUNK_SIZE 256

/*
 * The 3 basic types of blocks which are cacheable in the mpool
 */
typedef struct mpool_cache_primary_block       mpool_cache_primary_block_t;
typedef struct mpool_cache_canonical_block     mpool_cache_canonical_block_t;
typedef struct mpool_cache_canonical_ref_block mpool_cache_canonical_ref_block_t;
typedef struct mpool_cache_encode_block        mpool_cache_encode_block_t;

typedef struct mpool_cache_block mpool_cache_block_t;
typedef struct mpool             mpool_t;

typedef enum mpool_cache_blocktype
{
    mpool_cache_blocktype_undefined,
    mpool_cache_blocktype_head,
    mpool_cache_blocktype_primary,
    mpool_cache_blocktype_primary_ref,
    mpool_cache_blocktype_canonical,
    mpool_cache_blocktype_canonical_ref,
    mpool_cache_blocktype_encoded_chunk
} mpool_cache_blocktype_t;

struct mpool_cache_block
{
    mpool_cache_blocktype_t   type;
    struct mpool_cache_block *next;
    struct mpool_cache_block *prev;
};

typedef enum mpool_stream_dir
{
    mpool_stream_dir_undefined,
    mpool_stream_dir_write,
    mpool_stream_dir_read
} mpool_stream_dir_t;

typedef struct mpool_stream
{
    mpool_stream_dir_t      dir;
    mpool_t                *pool;
    mpool_cache_block_t    *last_eblk;
    mpool_cache_block_t     head;
    size_t                  curr_pos;
    size_t                  curr_limit;
    size_t                  stream_position;
    bplib_crc_parameters_t *crc_params;
    bp_crcval_t             crcval;
} mpool_stream_t;

/*
 * Note that all cache nodes must begin with a
 * mpool_cache_block_t structure.  There may be
 * additional mpool_cache_block_t's as well,
 * but the first is assumed to always be there,
 * to matter which type of link it is.
 *
 * They also must have a member named "content_start" which
 * reflects the actual structure/pointer returned to the user.
 */

//#define mpool_container_of(p, type) (type*)(((uint8_t*)p) - offsetof(type, content_start))

typedef struct mpool_cache_primary_block
{
    mpool_cache_block_t cblock_list;
    mpool_cache_block_t chunk_list;
    bp_primary_block_t  content_start;
} mpool_cache_primary_block_t;

typedef struct mpool_cache_canonical_block
{
    mpool_cache_block_t         chunk_list;
    size_t                      encoded_content_offset;
    size_t                      encoded_content_length;
    bp_canonical_block_buffer_t content_start;
} mpool_cache_canonical_block_t;

typedef struct mpool_cache_encode_block
{
    size_t  encoded_length; /* actual length of content */
    uint8_t content_start;  /* variably sized, not using C99 flexible array member due to restrictions it causes */
} mpool_cache_encode_block_t;

static inline bp_primary_block_t *mpool_get_pri_block_logical(mpool_cache_primary_block_t *cpb)
{
    return &cpb->content_start;
}

static inline mpool_cache_block_t *mpool_get_pri_block_encoded_chunks(mpool_cache_primary_block_t *cpb)
{
    return &cpb->chunk_list;
}

static inline mpool_cache_block_t *mpool_get_canonical_block_list(mpool_cache_primary_block_t *cpb)
{
    return &cpb->cblock_list;
}

static inline bp_canonical_block_buffer_t *mpool_get_canonical_block_logical(mpool_cache_canonical_block_t *ccb)
{
    return &ccb->content_start;
}

static inline mpool_cache_block_t *mpool_get_canonical_block_encoded_chunks(mpool_cache_canonical_block_t *ccb)
{
    return &ccb->chunk_list;
}

static inline void mpool_set_canonical_block_encoded_content_detail(mpool_cache_canonical_block_t *ccb, size_t offset,
                                                                    size_t length)
{
    ccb->encoded_content_offset = offset;
    ccb->encoded_content_length = length;
}

static inline size_t mpool_get_canonical_block_encoded_content_length(const mpool_cache_canonical_block_t *ccb)
{
    return ccb->encoded_content_length;
}

static inline size_t mpool_get_canonical_block_encoded_content_offset(const mpool_cache_canonical_block_t *ccb)
{
    return ccb->encoded_content_offset;
}

static inline mpool_cache_block_t *mpool_get_next_block(mpool_cache_block_t *cb)
{
    return cb->next;
}

static inline mpool_cache_block_t *mpool_get_prev_block(mpool_cache_block_t *cb)
{
    return cb->prev;
}

static inline bool mpool_is_encode_block(mpool_cache_block_t *cb)
{
    return (cb->type == mpool_cache_blocktype_encoded_chunk);
}

static inline bool mpool_is_pri_block(mpool_cache_block_t *cb)
{
    return (cb->type == mpool_cache_blocktype_primary);
}

static inline bool mpool_is_canonical_block(mpool_cache_block_t *cb)
{
    return (cb->type == mpool_cache_blocktype_canonical);
}

static inline bool mpool_is_direct_block(mpool_cache_block_t *cb)
{
    return (cb->type == mpool_cache_blocktype_primary || cb->type == mpool_cache_blocktype_canonical ||
            cb->type == mpool_cache_blocktype_encoded_chunk);
}

static inline bool mpool_is_indirect_block(mpool_cache_block_t *cb)
{
    return (cb->type == mpool_cache_blocktype_primary_ref || cb->type == mpool_cache_blocktype_canonical_ref);
}

static inline bool mpool_is_any_content_node(mpool_cache_block_t *cb)
{
    return (cb->type > mpool_cache_blocktype_head);
}

static inline void *mpool_get_encode_base_ptr(mpool_cache_encode_block_t *ceb)
{
    return &ceb->content_start;
}

static inline size_t mpool_get_encode_data_actual_size(const mpool_cache_encode_block_t *ceb)
{
    return ceb->encoded_length;
}

static inline void mpool_set_encode_data_size(mpool_cache_encode_block_t *ceb, size_t length)
{
    ceb->encoded_length = length;
}

size_t mpool_get_encode_data_capacity(const mpool_cache_encode_block_t *ceb);

static inline size_t mpool_get_encode_data_available(const mpool_cache_encode_block_t *ceb)
{
    return mpool_get_encode_data_capacity(ceb) - mpool_get_encode_data_actual_size(ceb);
}

mpool_cache_primary_block_t   *mpool_cast_primary(mpool_cache_block_t *cb);
mpool_cache_canonical_block_t *mpool_cast_canonical(mpool_cache_block_t *cb);
mpool_cache_encode_block_t    *mpool_cast_encode_block(mpool_cache_block_t *cb);

mpool_cache_block_t *mpool_get_next_outgoing_bundle(mpool_t *pool);
mpool_cache_block_t *mpool_get_next_incoming_bundle(mpool_t *pool);

mpool_cache_block_t *mpool_alloc_primary_block(mpool_t *pool);
mpool_cache_block_t *mpool_alloc_canonical_block(mpool_t *pool);
mpool_cache_block_t *mpool_alloc_encode_block(mpool_t *pool);

void   mpool_start_stream_init(mpool_stream_t *mps, mpool_t *pool, mpool_stream_dir_t dir, bp_crctype_t crctype);
size_t mpool_stream_write(mpool_stream_t *mps, const void *data, size_t size);
size_t mpool_stream_read(mpool_stream_t *mps, void *data, size_t size);
size_t mpool_stream_seek(mpool_stream_t *mps, size_t position);
void   mpool_stream_attach(mpool_stream_t *mps, mpool_cache_block_t *head);
static inline bplib_crc_parameters_t *mpool_stream_get_crc_params(const mpool_stream_t *mps)
{
    return mps->crc_params;
}
static inline bp_crcval_t mpool_stream_get_intermediate_crc(const mpool_stream_t *mps)
{
    return mps->crcval;
}
static inline size_t mpool_stream_tell(const mpool_stream_t *mps)
{
    return mps->stream_position;
}
void mpool_stream_close(mpool_stream_t *mps);

void mpool_append_encode_block(mpool_cache_block_t *head, mpool_cache_block_t *blk);

void mpool_store_primary_block_incoming(mpool_t *pool, mpool_cache_block_t *cpb);
void mpool_store_primary_block_outgoing(mpool_t *pool, mpool_cache_block_t *cpb);
void mpool_store_canonical_block(mpool_cache_primary_block_t *cpb, mpool_cache_block_t *ccb);

void mpool_return_block(mpool_t *pool, mpool_cache_block_t *blk);

size_t mpool_sum_encode_size(mpool_cache_block_t *list);
size_t mpool_sum_full_bundle_size(mpool_cache_primary_block_t *cpb);
size_t mpool_copy_block_chain(mpool_cache_block_t *list, void *out_ptr, size_t max_out_size, size_t seek_start,
                              size_t max_count);

mpool_t *mpool_create(void *pool_mem, size_t pool_size);

#endif /* V7_MPOOL_H */
