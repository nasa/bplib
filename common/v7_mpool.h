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

#include "bplib_api_types.h"
#include "v7_types.h"
#include "v7_rbtree.h"
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
typedef struct bplib_mpool_primary_block       bplib_mpool_primary_block_t;
typedef struct bplib_mpool_canonical_block     bplib_mpool_canonical_block_t;
typedef struct bplib_mpool_generic_data        bplib_mpool_generic_data_t;
typedef struct bplib_mpool_baseintf            bplib_mpool_baseintf_t;
typedef struct bplib_mpool_dataservice         bplib_mpool_dataservice_t;
typedef struct bplib_mpool_refptr              bplib_mpool_refptr_t;

typedef struct bplib_mpool_subq                bplib_mpool_subq_t;
typedef struct bplib_mpool_flow                bplib_mpool_flow_t;

typedef enum bplib_mpool_blocktype
{
    bplib_mpool_blocktype_undefined,
    bplib_mpool_blocktype_head,
    bplib_mpool_blocktype_cbor_data,
    bplib_mpool_blocktype_service_specific_data,
    bplib_mpool_blocktype_primary,
    bplib_mpool_blocktype_canonical,
    bplib_mpool_blocktype_baseintf,
    bplib_mpool_blocktype_dataservice,
    bplib_mpool_blocktype_flow_active_link,

    /*
     * The ref block types are all identical, they are just assigned different
     * types to get a hint as to what is being referred to (TBD if this is
     * useful or a nuisance).
     */
    bplib_mpool_blocktype_primary_ref,
    bplib_mpool_blocktype_canonical_ref,

    /*
     * An opaque block type that this code should not attempt to interpret at all
     */
    bplib_mpool_blocktype_secondary_link,

    bplib_mpool_blocktype_max /* end marker, keep last */
} bplib_mpool_blocktype_t;

struct bplib_mpool_block
{
    bplib_mpool_blocktype_t   type;
    struct bplib_mpool_block *next;
    struct bplib_mpool_block *prev;
};

typedef void (*bplib_mpool_callback_func_t)(void *, bplib_mpool_block_t *);

typedef struct bplib_mpool_delivery_data
{
    bplib_policy_delivery_t  delivery_policy;
    bp_handle_t      ingress_intf_id;
    bp_handle_t      egress_intf_id;
    bp_handle_t      storage_intf_id;
    bp_sid_t         committed_storage_id;
    uint64_t         local_retx_interval;
    bp_dtntime_t     ingress_time;
    bp_dtntime_t     egress_time;
} bplib_mpool_delivery_data_t;

struct bplib_mpool_primary_block
{
    bplib_mpool_block_t cblock_list;
    bplib_mpool_block_t chunk_list;
    size_t              block_encode_size_cache;
    size_t              bundle_encode_size_cache;
    bp_primary_block_t  pri_logical_data;
    bplib_mpool_delivery_data_t delivery_data;
};

struct bplib_mpool_canonical_block
{
    bplib_mpool_block_t         chunk_list;
    bplib_mpool_primary_block_t *bundle_ref;
    size_t                      block_encode_size_cache;
    size_t                      encoded_content_offset;
    size_t                      encoded_content_length;
    bp_canonical_block_buffer_t canonical_logical_data;
};

/*
 * This union is just to make sure that the "content_start" field
 * in a generic data block is suitably aligned to hold any data type, mainly
 * the max-sized integers, max-sized floating point values, or a pointer.
 */
typedef union bplib_mpool_aligned_data
{
    uint8_t first_octet;
    uintmax_t align_int;
    void *align_ptr;
    long double align_float;
} bplib_mpool_aligned_data_t;

struct bplib_mpool_generic_data
{
    uint32_t data_length;           /* actual length of content */
    uint32_t magic_number;          /* a "signature" (sanity check) value for identifying the data */
    bplib_mpool_aligned_data_t data_start;  /* variably sized, not using C99 flexible array member due to restrictions it causes */
};

struct bplib_mpool_subq
{
    bplib_mpool_block_t block_list;
    bplib_q_stats_t     stats;
};

struct bplib_mpool_flow
{
    bplib_mpool_subq_t input;
    bplib_mpool_subq_t output;
    bplib_routetbl_t *parent_rtbl;
    bp_handle_t parent_intf_id;
    bp_handle_t self_intf_id;
};

struct bplib_mpool_dataservice
{
    bplib_connection_t params;
};

struct bplib_mpool_baseintf
{
    bplib_mpool_block_t subflow_list;
    bplib_mpool_block_t *service_specific_block;
    bp_ipn_t node_num;
};

static inline bplib_mpool_block_t *bplib_mpool_get_reference_target(bplib_mpool_refptr_t *refptr)
{
    return (bplib_mpool_block_t *)refptr;
}

static inline bp_primary_block_t *bplib_mpool_get_pri_block_logical(bplib_mpool_primary_block_t *cpb)
{
    return &cpb->pri_logical_data;
}

static inline bplib_mpool_block_t *bplib_mpool_get_pri_block_encoded_chunks(bplib_mpool_primary_block_t *cpb)
{
    return &cpb->chunk_list;
}

static inline bplib_mpool_block_t *bplib_mpool_get_canonical_block_list(bplib_mpool_primary_block_t *cpb)
{
    return &cpb->cblock_list;
}

static inline bp_canonical_block_buffer_t *bplib_mpool_get_canonical_block_logical(bplib_mpool_canonical_block_t *ccb)
{
    return &ccb->canonical_logical_data;
}

static inline bplib_mpool_block_t *bplib_mpool_get_canonical_block_encoded_chunks(bplib_mpool_canonical_block_t *ccb)
{
    return &ccb->chunk_list;
}

static inline void bplib_mpool_set_canonical_block_encoded_content_detail(bplib_mpool_canonical_block_t *ccb, size_t offset,
                                                                    size_t length)
{
    ccb->encoded_content_offset = offset;
    ccb->encoded_content_length = length;
}

static inline size_t bplib_mpool_get_canonical_block_encoded_content_length(const bplib_mpool_canonical_block_t *ccb)
{
    return ccb->encoded_content_length;
}

static inline size_t bplib_mpool_get_canonical_block_encoded_content_offset(const bplib_mpool_canonical_block_t *ccb)
{
    return ccb->encoded_content_offset;
}

static inline bplib_mpool_block_t *bplib_mpool_get_next_block(bplib_mpool_block_t *cb)
{
    return cb->next;
}

static inline bplib_mpool_block_t *bplib_mpool_get_prev_block(bplib_mpool_block_t *cb)
{
    return cb->prev;
}

static inline bool bplib_mpool_is_link_attached(const bplib_mpool_block_t *list)
{
    return (list->next != list);
}

static inline bool bplib_mpool_is_link_unattached(const bplib_mpool_block_t *list)
{
    return (list->next == list);
}

static inline bool bplib_mpool_is_empty_list_head(const bplib_mpool_block_t *list)
{
    return (list->type == bplib_mpool_blocktype_head && bplib_mpool_is_link_unattached(list));
}

static inline bool bplib_mpool_is_generic_data_block(const bplib_mpool_block_t *cb)
{
    return (cb->type == bplib_mpool_blocktype_cbor_data || cb->type == bplib_mpool_blocktype_service_specific_data);
}

static inline bool bplib_mpool_is_pri_block(const bplib_mpool_block_t *cb)
{
    return (cb->type == bplib_mpool_blocktype_primary || cb->type == bplib_mpool_blocktype_primary_ref);
}

static inline bool bplib_mpool_is_canonical_block(const bplib_mpool_block_t *cb)
{
    return (cb->type == bplib_mpool_blocktype_canonical || cb->type == bplib_mpool_blocktype_canonical_ref);
}

static inline bool bplib_mpool_is_sublist_block(const bplib_mpool_block_t *cb)
{
    return (cb->type == bplib_mpool_blocktype_baseintf);
}

static inline bool bplib_mpool_is_indirect_block(const bplib_mpool_block_t *cb)
{
    return (cb->type == bplib_mpool_blocktype_primary_ref || cb->type == bplib_mpool_blocktype_canonical_ref);
}

static inline bool bplib_mpool_is_flow_block(const bplib_mpool_block_t *cb)
{
    return (cb->type == bplib_mpool_blocktype_baseintf || cb->type == bplib_mpool_blocktype_dataservice || cb->type == bplib_mpool_blocktype_flow_active_link);
}


static inline bool bplib_mpool_is_any_content_node(const bplib_mpool_block_t *cb)
{
    return (cb->type > bplib_mpool_blocktype_head);
}

static inline void *bplib_mpool_get_generic_data_ptr(bplib_mpool_generic_data_t *ceb)
{
    return &ceb->data_start;
}

static inline size_t bplib_mpool_get_generic_data_size(const bplib_mpool_generic_data_t *ceb)
{
    return ceb->data_length;
}

static inline void bplib_mpool_set_generic_data_size(bplib_mpool_generic_data_t *ceb, size_t length)
{
    ceb->data_length = length;
}

size_t bplib_mpool_get_generic_data_capacity(const bplib_mpool_block_t *cb);

/* basic list ops */
void bplib_mpool_link_reset(bplib_mpool_block_t *link, bplib_mpool_blocktype_t type);
void bplib_mpool_insert_after(bplib_mpool_block_t *list, bplib_mpool_block_t *node);
void bplib_mpool_insert_before(bplib_mpool_block_t *list, bplib_mpool_block_t *node);
void bplib_mpool_extract_node(bplib_mpool_block_t *node);
void bplib_mpool_merge_listx(bplib_mpool_block_t *dest, bplib_mpool_block_t *src);
void bplib_mpool_extract_node(bplib_mpool_block_t *node);


bplib_mpool_primary_block_t   *bplib_mpool_cast_primary(bplib_mpool_block_t *cb);
bplib_mpool_canonical_block_t *bplib_mpool_cast_canonical(bplib_mpool_block_t *cb);
bplib_mpool_generic_data_t    *bplib_mpool_cast_cbor_data(bplib_mpool_block_t *cb);
bplib_mpool_generic_data_t    *bplib_mpool_cast_generic_data(bplib_mpool_block_t *cb, uint32_t required_magic);
bplib_mpool_baseintf_t        *bplib_mpool_cast_baseintf(bplib_mpool_block_t *cb);
bplib_mpool_dataservice_t     *bplib_mpool_cast_dataservice(bplib_mpool_block_t *cb);
bplib_mpool_flow_t            *bplib_mpool_cast_flow(bplib_mpool_block_t *cb);

bplib_mpool_block_t    *bplib_mpool_get_generic_block_from_pointer(void *ptr, uint32_t required_magic);
bplib_rbt_link_t   *bplib_mpool_cast_ref_to_rbt(bplib_mpool_block_t *rblk);
bplib_mpool_block_t      *bplib_mpool_cast_rbt_to_block(bplib_rbt_link_t *rbtlink);

size_t bplib_mpool_get_read_refcount(bplib_mpool_block_t *cb);

bplib_mpool_block_t *bplib_mpool_alloc_primary_block(bplib_mpool_t *pool);
bplib_mpool_block_t *bplib_mpool_alloc_canonical_block(bplib_mpool_t *pool);
bplib_mpool_block_t *bplib_mpool_alloc_cbor_data_block(bplib_mpool_t *pool);
bplib_mpool_block_t *bplib_mpool_alloc_generic_block(bplib_mpool_t *pool, uint32_t magic_number, size_t req_capacity);
bplib_mpool_block_t *bplib_mpool_alloc_block(bplib_mpool_t *pool, bplib_mpool_blocktype_t blocktype);

void bplib_mpool_append_cbor_block(bplib_mpool_block_t *head, bplib_mpool_block_t *blk);

void bplib_mpool_append_subq_bundle(bplib_mpool_subq_t *subq, bplib_mpool_block_t *cpb);
bplib_mpool_block_t *bplib_mpool_shift_subq_bundle(bplib_mpool_subq_t *subq);
void bplib_mpool_mark_flow_active(bplib_mpool_t *pool, bplib_mpool_flow_t *flow);

void bplib_mpool_store_canonical_block(bplib_mpool_primary_block_t *cpb, bplib_mpool_block_t *ccb);

void bplib_mpool_recycle_block(bplib_mpool_t *pool, bplib_mpool_block_t *blk);
void bplib_mpool_recycle_all_blocks_in_list(bplib_mpool_t *pool, bplib_mpool_block_t *list);

void bplib_mpool_store_flow(bplib_mpool_baseintf_t *ci, bplib_mpool_block_t *fblk);

int bplib_mpool_foreach_item_in_list(bplib_mpool_block_t *list, bool always_remove, bplib_mpool_callback_func_t callback_fn, void *callback_arg);
int bplib_mpool_process_all_flows(bplib_mpool_t *pool, bplib_mpool_callback_func_t callback_fn, void *callback_arg);


void bplib_mpool_pri_drop_encode_data(bplib_mpool_t *pool, bplib_mpool_primary_block_t *cpb);
void bplib_mpool_canonical_drop_encode_data(bplib_mpool_t *pool, bplib_mpool_canonical_block_t *ccb);

bplib_mpool_refptr_t *bplib_mpool_create_light_reference(bplib_mpool_t *pool, bplib_mpool_block_t *blk);
bplib_mpool_block_t *bplib_mpool_create_block_reference(bplib_mpool_t *pool, bplib_mpool_block_t *blk, bplib_action_func_t notify_on_discard, void *notify_arg);
void bplib_mpool_release_light_reference(bplib_mpool_t *pool, bplib_mpool_refptr_t *refptr);

size_t bplib_mpool_copy_block_chain(bplib_mpool_block_t *list, void *out_ptr, size_t max_out_size, size_t seek_start,
                              size_t max_count);

void bplib_mpool_maintain(bplib_mpool_t *pool);
bplib_mpool_t *bplib_mpool_create(void *pool_mem, size_t pool_size);

void bplib_mpool_debug_scan(bplib_mpool_t *pool);
void bplib_mpool_debug_print_queue_stats(bplib_mpool_block_t *list, const char *label);

#endif /* V7_MPOOL_H */
