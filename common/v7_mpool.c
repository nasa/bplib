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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <string.h>
#include <assert.h>

#include "bplib.h"
#include "bplib_os.h"
#include "v7_types.h"
#include "v7_mpool.h"

#define MPOOL_CACHE_CBOR_DATA_SIGNATURE  0x6b243e33

typedef struct bplib_mpool_refcount_link
{
    bplib_mpool_block_t link;
    size_t refcount;
} bplib_mpool_refcount_link_t;

typedef struct bplib_mpool_primary_block_node
{
    bplib_mpool_refcount_link_t refcount_link;
    bplib_mpool_primary_block_t pblock;
} bplib_mpool_primary_block_node_t;

typedef struct bplib_mpool_flow_node
{
    bplib_mpool_block_t   subif_link;
    bplib_mpool_block_t   processable_link;
    bplib_mpool_flow_t fblock;
} bplib_mpool_flow_node_t;

typedef struct bplib_mpool_baseintf_node
{
    bplib_mpool_flow_node_t flow; /* keep first! */
    bplib_mpool_baseintf_t    biblock;
} bplib_mpool_baseintf_node_t;

typedef struct bplib_mpool_dataservice_node
{
    bplib_mpool_flow_node_t flow; /* keep first! */
    bplib_mpool_dataservice_t fblock;
} bplib_mpool_dataservice_node_t;

typedef struct bplib_mpool_generic_data_node
{
    bplib_mpool_block_t        chunk_link;
    bplib_mpool_generic_data_t gdblock; /* variably sized, must be last */
} bplib_mpool_generic_data_node_t;

typedef struct bplib_mpool_canonical_block_node
{
    bplib_mpool_refcount_link_t refcount_link;
    bplib_mpool_canonical_block_t cblock;
} bplib_mpool_canonical_block_node_t;

typedef struct bplib_mpool_block_ref_node
{
    bplib_mpool_block_t         std_link;
    bplib_rbt_link_t      rbt_link; /* secondary link for storage in RB tree */
    bplib_mpool_refptr_t       *pref_target;
    bplib_action_func_t         notify_on_discard;
    void *notify_arg;
} bplib_mpool_block_ref_node_t;

typedef union bplib_mpool_any_buffer
{
    bplib_mpool_block_t                    std_link;
    bplib_mpool_refcount_link_t            refcount_link;
    bplib_mpool_block_ref_node_t           ref;
    bplib_mpool_primary_block_node_t       primary;
    bplib_mpool_canonical_block_node_t     canonical;
    bplib_mpool_generic_data_node_t        generic_data;

    bplib_mpool_flow_node_t        flow;
    bplib_mpool_baseintf_node_t    baseintf;
    bplib_mpool_dataservice_node_t dataservice;

    uint8_t                                max_content[BP_MPOOL_MAX_ENCODED_CHUNK_SIZE];
} bplib_mpool_any_buffer_t;

struct bplib_mpool_refptr
{
    bplib_mpool_refcount_link_t link;
};

struct mpool
{
    size_t                   num_bufs;
    size_t                   buffer_size;
    size_t                   alloc_count;
    size_t                   recycled_count;

    bplib_mpool_block_t      free_blocks;
    bplib_mpool_block_t      active_bundles;
    bplib_mpool_block_t      recycle_blocks;
    bplib_mpool_block_t      active_flow_list;

    bplib_mpool_any_buffer_t first_buffer;
};


#define MPOOL_ENCODE_CHUNK_CAPACITY \
    (sizeof(bplib_mpool_any_buffer_t) - offsetof(bplib_mpool_generic_data_node_t, gdblock.data_start))

#define MPOOL_CONTAINER_OF(link_ptr, container_member) \
    ((bplib_mpool_any_buffer_t*)((uint8_t*)link_ptr - offsetof(bplib_mpool_any_buffer_t, container_member)))



size_t bplib_mpool_get_generic_data_capacity(const bplib_mpool_block_t *cb)
{
    if (!bplib_mpool_is_generic_data_block(cb))
    {
        return 0;
    }
    return MPOOL_ENCODE_CHUNK_CAPACITY;
}

void bplib_mpool_link_reset(bplib_mpool_block_t *link, bplib_mpool_blocktype_t type)
{
    link->type = type;
    link->next = link;
    link->prev = link;
}

void bplib_mpool_insert_after(bplib_mpool_block_t *list, bplib_mpool_block_t *node)
{
    node->next       = list->next;
    node->prev       = list;
    list->next       = node;
    node->next->prev = node;
}

void bplib_mpool_insert_before(bplib_mpool_block_t *list, bplib_mpool_block_t *node)
{
    node->prev       = list->prev;
    node->next       = list;
    list->prev       = node;
    node->prev->next = node;
}

void bplib_mpool_extract_node(bplib_mpool_block_t *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next       = node;
    node->prev       = node;
}

void bplib_mpool_merge_listx(bplib_mpool_block_t *dest, bplib_mpool_block_t *src)
{
    bplib_mpool_block_t *dlast = dest->prev; /* last node in dest list */
    bplib_mpool_block_t *slast = src->prev;  /* last node in dest list */

    /* nominally combine the two lists.
     * NOTE: This (temporarily) yields a list with two head nodes. */
    dlast->next = src;
    slast->next = dest;
    dest->prev  = slast;
    src->prev   = dlast;

    /* extract the "src" which separates them back into two lists.
     * the src list will now be empty */
    // x bplib_mpool_extract_node(src);
}

bplib_mpool_primary_block_t *bplib_mpool_cast_primary(bplib_mpool_block_t *cb)
{
    if (cb != NULL)
    {
        if (cb->type == bplib_mpool_blocktype_primary_ref)
        {
            cb = bplib_mpool_get_reference_target(((bplib_mpool_block_ref_node_t *)cb)->pref_target);
        }
        if (cb->type == bplib_mpool_blocktype_primary)
        {
            return &((bplib_mpool_primary_block_node_t *)cb)->pblock;
        }
    }
    return NULL;
}

bplib_mpool_canonical_block_t *bplib_mpool_cast_canonical(bplib_mpool_block_t *cb)
{
    if (cb != NULL)
    {
        if (cb->type == bplib_mpool_blocktype_canonical_ref)
        {
            cb = bplib_mpool_get_reference_target(((bplib_mpool_block_ref_node_t *)cb)->pref_target);
        }
        if (cb->type == bplib_mpool_blocktype_canonical)
        {
            return &((bplib_mpool_canonical_block_node_t *)cb)->cblock;
        }
    }
    return NULL;
}

bplib_mpool_refcount_link_t *bplib_mpool_get_refcount_link_header(bplib_mpool_block_t *cb)
{
    if (cb != NULL)
    {
        if (cb->type == bplib_mpool_blocktype_primary_ref || cb->type == bplib_mpool_blocktype_canonical_ref)
        {
            return &((bplib_mpool_block_ref_node_t *)cb)->pref_target->link;
        }
        if (cb->type == bplib_mpool_blocktype_primary || cb->type == bplib_mpool_blocktype_canonical)
        {
            return (bplib_mpool_refcount_link_t *)cb;
        }
    }
    return NULL;
}

size_t bplib_mpool_get_read_refcount(bplib_mpool_block_t *cb)
{
    bplib_mpool_refcount_link_t *reflink;

    reflink = bplib_mpool_get_refcount_link_header(cb);
    if (reflink == NULL)
    {
        return 0;
    }
    return reflink->refcount;
}


bplib_mpool_generic_data_t *bplib_mpool_cast_generic_data(bplib_mpool_block_t *cb, uint32_t required_magic)
{
    bplib_mpool_generic_data_node_t *cgd;

    if (cb != NULL && bplib_mpool_is_generic_data_block(cb))
    {
        cgd = (bplib_mpool_generic_data_node_t *)cb;
        if (cgd->gdblock.magic_number == required_magic)
        {
            return &cgd->gdblock;
        }
    }
    return NULL;
}

bplib_mpool_generic_data_t    *bplib_mpool_cast_cbor_data(bplib_mpool_block_t *cb)
{
    return bplib_mpool_cast_generic_data(cb, MPOOL_CACHE_CBOR_DATA_SIGNATURE);
}


bplib_mpool_dataservice_t   *bplib_mpool_cast_dataservice(bplib_mpool_block_t *cb)
{
    if (cb != NULL)
    {
        if (cb->type == bplib_mpool_blocktype_flow_active_link)
        {
            cb = &MPOOL_CONTAINER_OF(cb, flow.processable_link)->std_link;
        }
        if (cb->type == bplib_mpool_blocktype_dataservice)
        {
            return &((bplib_mpool_dataservice_node_t *)cb)->fblock;
        }
    }
    return NULL;
}

bplib_mpool_baseintf_t   *bplib_mpool_cast_baseintf(bplib_mpool_block_t *cb)
{
    if (cb != NULL)
    {
        if (cb->type == bplib_mpool_blocktype_flow_active_link)
        {
            cb = &MPOOL_CONTAINER_OF(cb, flow.processable_link)->std_link;
        }
        if (cb->type == bplib_mpool_blocktype_baseintf)
        {
            return &((bplib_mpool_baseintf_node_t *)cb)->biblock;
        }
    }
    return NULL;
}

bplib_rbt_link_t   *bplib_mpool_cast_ref_to_rbt(bplib_mpool_block_t *rblk)
{
    if (rblk != NULL)
    {
        if (bplib_mpool_is_indirect_block(rblk))
        {
            return &((bplib_mpool_block_ref_node_t *)rblk)->rbt_link;
        }
    }

    return NULL;
}

bplib_mpool_block_t      *bplib_mpool_cast_rbt_to_block(bplib_rbt_link_t *rbtlink)
{
    bplib_mpool_block_t      *rblk;

    if (rbtlink != NULL)
    {
        rblk = &MPOOL_CONTAINER_OF(rbtlink, ref.rbt_link)->std_link;

        /* sanity check: only refs have this item */
        if (!bplib_mpool_is_indirect_block(rblk))
        {
            rblk = NULL;
        }
    }
    else
    {
        rblk = NULL;
    }

    return rblk;
}

bplib_mpool_block_t    *bplib_mpool_get_generic_block_from_pointer(void *ptr, uint32_t required_magic)
{
    bplib_mpool_generic_data_node_t *gd;

    if (ptr != NULL)
    {
        gd = &MPOOL_CONTAINER_OF(ptr, generic_data.gdblock.data_start)->generic_data;

        /* sanity check: magic number should match */
        if (gd->chunk_link.type == bplib_mpool_blocktype_service_specific_data && gd->gdblock.magic_number == required_magic)
        {
            return &gd->chunk_link;
        }
    }

    return NULL;
}



bplib_mpool_flow_t   *bplib_mpool_cast_flow(bplib_mpool_block_t *cb)
{
    if (cb != NULL)
    {
        if (cb->type == bplib_mpool_blocktype_flow_active_link)
        {
            cb = &MPOOL_CONTAINER_OF(cb, flow.processable_link)->std_link;
        }
        if (cb->type == bplib_mpool_blocktype_baseintf || cb->type == bplib_mpool_blocktype_dataservice)
        {
            return &((bplib_mpool_flow_node_t *)cb)->fblock;
        }
    }

    return NULL;
}

bplib_mpool_block_t *bplib_mpool_shift_subq_bundle(bplib_mpool_subq_t *subq)
{
    bplib_mpool_block_t *node = subq->block_list.next;

    /* All nodes in the incoming list should have a linktype of primary.
     * If not, then this means the list is empty */
    if (!bplib_mpool_is_pri_block(node))
    {
        return NULL;
    }

    bplib_mpool_extract_node(node);

    return node;
}

void bplib_mpool_mark_flow_active(bplib_mpool_t *pool, bplib_mpool_flow_t *flow)
{
    bplib_mpool_flow_node_t *node;

    node = &MPOOL_CONTAINER_OF(flow, flow.fblock)->flow;
    bplib_mpool_extract_node(&node->processable_link);
    bplib_mpool_insert_before(&pool->active_flow_list, &node->processable_link);
}

void bplib_mpool_init_flow_block(bplib_mpool_flow_node_t *node)
{
    /* clear the basic block, which zeros all stats */
    memset(&node->fblock, 0, sizeof(node->fblock));

    /* now init the link structs */
    bplib_mpool_link_reset(&node->processable_link, bplib_mpool_blocktype_flow_active_link);
    bplib_mpool_link_reset(&node->fblock.input.block_list, bplib_mpool_blocktype_head);
    bplib_mpool_link_reset(&node->fblock.output.block_list, bplib_mpool_blocktype_head);
}

bplib_mpool_block_t *bplib_mpool_alloc_block_internal(bplib_mpool_t *pool, bplib_mpool_blocktype_t blocktype)
{
    bplib_mpool_block_t             *node = pool->free_blocks.next;
    bplib_mpool_any_buffer_t        *cab;

    /* All nodes in the free list should have a linktype of undefined.
     * If not, then this means the list is empty */
    if (node->type != bplib_mpool_blocktype_undefined)
    {
        return NULL;
    }

    /* extract the node from the free list, and reconfigure it as the requested block type */
    bplib_mpool_extract_node(node);
    ++pool->alloc_count;
    cab = (bplib_mpool_any_buffer_t *)node;

    switch(blocktype)
    {
        case bplib_mpool_blocktype_primary:
            bplib_mpool_link_reset(&cab->primary.pblock.cblock_list, bplib_mpool_blocktype_head);
            bplib_mpool_link_reset(&cab->primary.pblock.chunk_list, bplib_mpool_blocktype_head);

            /* wipe the logical content to be sure no stale data exists */
            memset(&cab->primary.pblock.delivery_data, 0, sizeof(cab->primary.pblock.delivery_data));
            memset(&cab->primary.pblock.pri_logical_data, 0, sizeof(cab->primary.pblock.pri_logical_data));
            cab->primary.pblock.bundle_encode_size_cache = 0;
            cab->primary.pblock.block_encode_size_cache = 0;
            break;
        case bplib_mpool_blocktype_canonical:
            bplib_mpool_link_reset(&cab->canonical.cblock.chunk_list, bplib_mpool_blocktype_head);

            /* wipe the logical content to be sure no stale data exists */
            memset(&cab->canonical.cblock.canonical_logical_data, 0, sizeof(cab->canonical.cblock.canonical_logical_data));
            bplib_mpool_set_canonical_block_encoded_content_detail(&cab->canonical.cblock, 0, 0);
            cab->canonical.cblock.block_encode_size_cache = 0;
            break;
        case bplib_mpool_blocktype_cbor_data:
            /*
             * for cbor, should only need to set size to 0 (skips wiping the block mem)
             * because this happens frequently, it may have an effect on performance.
             */
            cab->generic_data.gdblock.data_length = 0;
            break;
        case bplib_mpool_blocktype_service_specific_data:
            /*
             * for service specific data, wipe the whole block.
             * this should not happen that frequently - only when services are added/removed -
             * and as such the value of ensuring initial state is predictable is greater
             * than the performance cost of (possibly) initializing more than what is necessary
             */
            memset(&cab->generic_data.gdblock, 0, sizeof(cab->generic_data.gdblock));
            break;
        case bplib_mpool_blocktype_baseintf:
            bplib_mpool_init_flow_block(&cab->baseintf.flow);
            bplib_mpool_link_reset(&cab->baseintf.biblock.subflow_list, bplib_mpool_blocktype_head);
            break;
        case bplib_mpool_blocktype_dataservice:
            bplib_mpool_init_flow_block(&cab->dataservice.flow);
            memset(&cab->dataservice.fblock, 0, sizeof(cab->dataservice.fblock));
            break;
        case bplib_mpool_blocktype_primary_ref:
        case bplib_mpool_blocktype_canonical_ref:
            cab->ref.pref_target = NULL;
            break;

        default:
            /* as a catch-all, clear the entire block (somewhat heavy-handed). */
            memset(cab, 0, sizeof(*cab));
            break;
    }

    node->type = blocktype;

    return node;
}

bplib_mpool_block_t *bplib_mpool_alloc_block(bplib_mpool_t *pool, bplib_mpool_blocktype_t blocktype)
{
    bplib_mpool_block_t *result;

    /*
     * external code is only allowed to directly allocate certain types of nodes
     * via this general API.  For other node types there is a separate API to allocate
     * (possibly because there is some different setup or tracking to do for that type)
     */
    switch(blocktype)
    {
        case bplib_mpool_blocktype_service_specific_data:
        case bplib_mpool_blocktype_baseintf:
        case bplib_mpool_blocktype_dataservice:
            result = bplib_mpool_alloc_block_internal(pool, blocktype);
            break;
        default:
            /* nodes of other types are not user-allocatable via this API */
            result = NULL;
            break;
    }

    return result;
}

bplib_mpool_block_t *bplib_mpool_alloc_primary_block(bplib_mpool_t *pool)
{
    return bplib_mpool_alloc_block_internal(pool, bplib_mpool_blocktype_primary);
}

bplib_mpool_block_t *bplib_mpool_alloc_canonical_block(bplib_mpool_t *pool)
{
    return bplib_mpool_alloc_block_internal(pool, bplib_mpool_blocktype_canonical);
}

bplib_mpool_block_t *bplib_mpool_alloc_generic_block(bplib_mpool_t *pool, uint32_t magic_number, size_t req_capacity)
{
    bplib_mpool_block_t *blk;
    bplib_mpool_generic_data_node_t *cgd;

    /* for now all blocks are fixed size, so if it doesn't fit, it's broken */
    assert(req_capacity <= MPOOL_ENCODE_CHUNK_CAPACITY);
    blk = bplib_mpool_alloc_block_internal(pool, bplib_mpool_blocktype_service_specific_data);
    if (blk)
    {
        /* need to set the internal fields to track */
        cgd = (bplib_mpool_generic_data_node_t *)blk;
        cgd->gdblock.magic_number = magic_number;
        cgd->gdblock.data_length = req_capacity;
    }
    return blk;
}

bplib_mpool_block_t *bplib_mpool_alloc_cbor_data_block(bplib_mpool_t *pool)
{
    bplib_mpool_block_t *blk;
    bplib_mpool_generic_data_node_t *cgd;

    blk = bplib_mpool_alloc_block_internal(pool, bplib_mpool_blocktype_cbor_data);
    if (blk)
    {
        /* need to set the internal fields to track */
        cgd = (bplib_mpool_generic_data_node_t *)blk;
        cgd->gdblock.magic_number = MPOOL_CACHE_CBOR_DATA_SIGNATURE;
    }
    return blk;
}

void bplib_mpool_append_cbor_block(bplib_mpool_block_t *head, bplib_mpool_block_t *blk)
{
    assert(blk->type == bplib_mpool_blocktype_cbor_data);
    bplib_mpool_insert_before(head, blk);
}

void bplib_mpool_recycle_all_blocks_in_list(bplib_mpool_t *pool, bplib_mpool_block_t *list)
{
    assert(list->type == bplib_mpool_blocktype_head);
    bplib_mpool_merge_listx(&pool->recycle_blocks, list);
    bplib_mpool_extract_node(list);
}

void bplib_mpool_recycle_block(bplib_mpool_t *pool, bplib_mpool_block_t *blk)
{
    bplib_mpool_extract_node(blk);
    bplib_mpool_insert_before(&pool->recycle_blocks, blk);
}

void bplib_mpool_pri_drop_encode_data(bplib_mpool_t *pool, bplib_mpool_primary_block_t *cpb)
{
    bplib_mpool_block_t *elist;

    /* If there is any existing encoded data, return it to the pool */
    elist = bplib_mpool_get_pri_block_encoded_chunks(cpb);
    if (!bplib_mpool_is_empty_list_head(elist))
    {
        bplib_mpool_recycle_all_blocks_in_list(pool, elist);
    }
    cpb->block_encode_size_cache = 0;
    cpb->bundle_encode_size_cache = 0;
}

void bplib_mpool_canonical_drop_encode_data(bplib_mpool_t *pool, bplib_mpool_canonical_block_t *ccb)
{
    bplib_mpool_block_t *elist;

    /* If there is any existing encoded data, return it to the pool */
    elist = bplib_mpool_get_canonical_block_encoded_chunks(ccb);
    if (!bplib_mpool_is_empty_list_head(elist))
    {
        bplib_mpool_recycle_all_blocks_in_list(pool, elist);
    }
    ccb->block_encode_size_cache = 0;

    /* this also invalidates the size of the parent bundle, if it was in one */
    if (ccb->bundle_ref)
    {
        ccb->bundle_ref->bundle_encode_size_cache = 0;
    }
}

size_t bplib_mpool_copy_block_chain(bplib_mpool_block_t *list, void *out_ptr, size_t max_out_size, size_t seek_start,
                              size_t max_count)
{
    bplib_mpool_block_t        *blk;
    bplib_mpool_generic_data_t *ceb;
    uint8_t                    *curr_ptr;
    const uint8_t              *src_ptr;
    size_t                      remain_sz;
    size_t                      chunk_sz;
    size_t                      seek_left;
    size_t                      data_left;

    remain_sz = max_out_size;
    curr_ptr  = out_ptr;
    seek_left = seek_start;
    data_left = max_count;
    blk       = list;
    while (data_left > 0)
    {
        blk = bplib_mpool_get_next_block(blk);
        ceb = bplib_mpool_cast_cbor_data(blk);
        if (ceb == NULL)
        {
            break;
        }
        chunk_sz = ceb->data_length;
        if (seek_left > chunk_sz)
        {
            seek_left -= chunk_sz;
        }
        else
        {
            src_ptr = &ceb->data_start.first_octet;
            if (seek_left > 0)
            {
                src_ptr += seek_left;
                chunk_sz -= seek_left;
                seek_left = 0;
            }

            if (chunk_sz > data_left)
            {
                chunk_sz = data_left;
            }

            if (remain_sz < chunk_sz)
            {
                /* will not fit */
                break;
            }
            memcpy(curr_ptr, src_ptr, chunk_sz);
            curr_ptr += chunk_sz;
            remain_sz -= chunk_sz;
            data_left -= chunk_sz;
        }
    }

    return max_out_size - remain_sz;
}

void bplib_mpool_append_subq_bundle(bplib_mpool_subq_t *subq, bplib_mpool_block_t *cpb)
{
    assert(cpb->type == bplib_mpool_blocktype_primary_ref);
    bplib_mpool_insert_before(&subq->block_list, cpb);
}

void bplib_mpool_store_canonical_block(bplib_mpool_primary_block_t *cpb, bplib_mpool_block_t *blk)
{
    bplib_mpool_canonical_block_t *ccb;

    /* this should only be invoked w/canonical blocks.  Anything else is a bug. */
    ccb = bplib_mpool_cast_canonical(blk);
    assert(ccb != NULL);
    assert(ccb->bundle_ref == NULL);

    /*
     * TBD on block ordering requirements.  The BPv7 spec says the payload should be last,
     * but other blocks may appear in any order.  It is not clear if there is any reason
     * to be more specific about block order.
     *
     * For now, if the block being inserted is payload, put it last, otherwise put it first.
     */

    if (bplib_mpool_get_canonical_block_logical(ccb)->canonical_block.blockType == bp_blocktype_payloadBlock)
    {
        /* this puts it last */
        bplib_mpool_insert_before(&cpb->cblock_list, blk);
    }
    else
    {
        /* this puts it first */
        bplib_mpool_insert_after(&cpb->cblock_list, blk);
    }

    /* this changes the size of the fully-encoded bundle, but don't recalculate now */
    cpb->bundle_encode_size_cache = 0;
    ccb->bundle_ref = cpb;
}

void bplib_mpool_store_flow(bplib_mpool_baseintf_t *ci, bplib_mpool_block_t *fblk)
{
    /* TBD: Should have a more intelligent/sorted/hashed storage for these? */
    bplib_mpool_insert_before(&ci->subflow_list, fblk);
}

int bplib_mpool_foreach_item_in_list(bplib_mpool_block_t *list, bool always_remove, bplib_mpool_callback_func_t callback_fn, void *callback_arg)
{
    bplib_mpool_block_t *curr_node;
    bplib_mpool_block_t *next_node;
    int count;

    /*
     * Note: To handle the possibility that the callback removes the node
     * from this list and puts it into another list, this needs to preemptively
     * get the next node from this list before running the callback.
     */
    next_node = list->next;
    count = 0;
    while(next_node != list)
    {
        curr_node = next_node;
        next_node = curr_node->next;

        /*
         * If always_remove is set, then take it out of this list before invoking the callback.
         * The callback function must take responsibility for inserting the item into another list
         */
        if (always_remove)
        {
            bplib_mpool_extract_node(curr_node);
        }

        callback_fn(callback_arg, curr_node);
        ++count;
    }

    return count;
}

int bplib_mpool_process_all_flows(bplib_mpool_t *pool, bplib_mpool_callback_func_t callback_fn, void *callback_arg)
{
    bplib_mpool_block_t temp_list;
    int count;

    /*
     * To address the possibility that the callback puts the item into the same
     * list it was originally in, first make a separate temp list and move all
     * the items into that.  This should ensure that this only processes
     * the contents once, even if the callback puts it back into the orginal list.
     * While that still might create an infinite loop at the next level up,
     * it won't create an infinite loop here at least.
     */
    bplib_mpool_link_reset(&temp_list, bplib_mpool_blocktype_head);
    bplib_mpool_merge_listx(&temp_list, &pool->active_flow_list);
    bplib_mpool_extract_node(&pool->active_flow_list);

    count = bplib_mpool_foreach_item_in_list(&temp_list, true, callback_fn, callback_arg);

    /* this should have moved everything - if non-empty then nodes will be leaked! */
    assert(bplib_mpool_is_empty_list_head(&temp_list));

    return count;
}

bplib_mpool_refptr_t *bplib_mpool_create_light_reference(bplib_mpool_t *pool, bplib_mpool_block_t *blk)
{
    bplib_mpool_refcount_link_t *reflink;

    reflink = bplib_mpool_get_refcount_link_header(blk);
    if (reflink == NULL)
    {
        return NULL;
    }

    /*
     * If this is the first reference/new block, insert the original into
     * the active list.  This is the base reference, and it should persist
     * until there are no more other references.
     */
    if (reflink->refcount == 0)
    {
        bplib_mpool_insert_before(&pool->active_bundles, &reflink->link);
    }

    ++reflink->refcount;

    return (bplib_mpool_refptr_t *)reflink;
}

bplib_mpool_block_t *bplib_mpool_create_block_reference(bplib_mpool_t *pool, bplib_mpool_block_t *blk, bplib_action_func_t notify_on_discard, void *notify_arg)
{
    bplib_mpool_block_ref_node_t *refnode;
    bplib_mpool_block_t *rblk;
    bplib_mpool_blocktype_t ref_type;

    switch(blk->type)
    {
        case bplib_mpool_blocktype_primary:
        case bplib_mpool_blocktype_primary_ref:
            ref_type = bplib_mpool_blocktype_primary_ref;
            break;
        case bplib_mpool_blocktype_canonical:
        case bplib_mpool_blocktype_canonical_ref:
            ref_type = bplib_mpool_blocktype_canonical_ref;
            break;
        default:
            ref_type = bplib_mpool_blocktype_undefined;
            break;
    }

    if (ref_type == bplib_mpool_blocktype_undefined)
    {
        rblk = NULL;
    }
    else
    {
        rblk = bplib_mpool_alloc_block_internal(pool, ref_type);
    }

    if (rblk == NULL)
    {
        return NULL;
    }

    refnode = (bplib_mpool_block_ref_node_t *)rblk;
    refnode->pref_target = bplib_mpool_create_light_reference(pool, blk);
    refnode->notify_on_discard = notify_on_discard;
    refnode->notify_arg = notify_arg;

    return rblk;
}

void bplib_mpool_release_light_reference(bplib_mpool_t *pool, bplib_mpool_refptr_t *refptr)
{
    bplib_mpool_refcount_link_t *reflink;

    reflink = &refptr->link;
    if (reflink->refcount > 0)
    {
        --reflink->refcount;
    }
    if (reflink->refcount == 0)
    {
        bplib_mpool_recycle_block(pool, &reflink->link);
    }
}

void bplib_mpool_maintain(bplib_mpool_t *pool)
{
    bplib_mpool_block_t pending_recycle;
    bplib_mpool_block_t *rblk;
    bplib_mpool_any_buffer_t *cab;
    size_t recycled_count;

    recycled_count = 0;
    bplib_mpool_link_reset(&pending_recycle, bplib_mpool_blocktype_head);

    while(!bplib_mpool_is_empty_list_head(&pool->recycle_blocks))
    {
        rblk = &pending_recycle;

        bplib_mpool_merge_listx(rblk, &pool->recycle_blocks);
        bplib_mpool_extract_node(&pool->recycle_blocks);

        while(true)
        {
            rblk = bplib_mpool_get_next_block(rblk);
            if (rblk == &pending_recycle)
            {
                break;
            }

            cab = (bplib_mpool_any_buffer_t *)rblk;

            switch (rblk->type)
            {
                case bplib_mpool_blocktype_canonical:
                {
                    /* recycled blocks must all have refcount of 0, or else bad things might happen */
                    assert(cab->refcount_link.refcount == 0);
                    bplib_mpool_recycle_all_blocks_in_list(pool, &cab->canonical.cblock.chunk_list);
                    break;
                }
                case bplib_mpool_blocktype_primary:
                {
                    /* recycled blocks must all have refcount of 0, or else bad things might happen */
                    assert(cab->refcount_link.refcount == 0);
                    bplib_mpool_recycle_all_blocks_in_list(pool, &cab->primary.pblock.cblock_list);
                    bplib_mpool_recycle_all_blocks_in_list(pool, &cab->primary.pblock.chunk_list);
                    break;
                }
                case bplib_mpool_blocktype_baseintf:
                case bplib_mpool_blocktype_dataservice:
                {
                    bplib_mpool_recycle_all_blocks_in_list(pool, &cab->flow.fblock.input.block_list);
                    bplib_mpool_recycle_all_blocks_in_list(pool, &cab->flow.fblock.output.block_list);
                    break;
                }
                case bplib_mpool_blocktype_canonical_ref:
                case bplib_mpool_blocktype_primary_ref:
                {
                    if (cab->ref.notify_on_discard != NULL)
                    {
                        cab->ref.notify_on_discard(&cab->std_link, cab->ref.notify_arg);
                    }
                    bplib_mpool_release_light_reference(pool, cab->ref.pref_target);
                    cab->ref.pref_target = NULL; /* this ref is going away */
                    break;
                }
                default:
                {
                    break;
                }
            }

            printf("DEBUG: %s() recycled block type %d\n", __func__, rblk->type);

            /* always return _this_ node to the free pile */
            rblk->type = bplib_mpool_blocktype_undefined;
            ++recycled_count;
        }

        bplib_mpool_merge_listx(&pool->free_blocks, rblk);
        bplib_mpool_extract_node(rblk);
    }

    if (recycled_count != 0)
    {
        printf("DEBUG: %s() recycled %lu blocks\n", __func__, (unsigned long)recycled_count);
    }

    pool->recycled_count += recycled_count;
}

void bplib_mpool_debug_print_queue_stats(bplib_mpool_block_t *list, const char *label)
{
    bplib_mpool_block_t *blk;
    bplib_mpool_any_buffer_t *cab;
    size_t depth;

    blk = list;
    depth = 0;
    while(true)
    {
        blk = bplib_mpool_get_next_block(blk);
        if (blk->type == bplib_mpool_blocktype_head)
        {
            assert(blk == list);
            break;
        }
        cab = (bplib_mpool_any_buffer_t *)blk;
        if (blk->type == bplib_mpool_blocktype_canonical)
        {
            printf("DEBUG: %s(): block addr=%lx refcount=%zu, canonical type %d\n", __func__, (unsigned long)blk,
                cab->canonical.refcount_link.refcount, (int)cab->canonical.cblock.canonical_logical_data.canonical_block.blockType);
        }
        else if (blk->type == bplib_mpool_blocktype_primary)
        {
            printf("DEBUG: %s(): block addr=%lx refcount=%zu, primary dest IPN %lu\n", __func__, (unsigned long)blk,
                cab->primary.refcount_link.refcount, (unsigned long)cab->primary.pblock.pri_logical_data.destinationEID.ssp.ipn.node_number);
        }
        else if (blk->type == bplib_mpool_blocktype_canonical_ref)
        {
            cab = (bplib_mpool_any_buffer_t *)cab->ref.pref_target;
            assert(cab->std_link.type == bplib_mpool_blocktype_canonical);
            printf("DEBUG: %s(): block addr=%lx refcount=%zu, canonical ref type %d\n", __func__, (unsigned long)blk,
                cab->canonical.refcount_link.refcount, (int)cab->canonical.cblock.canonical_logical_data.canonical_block.blockType);
        }
        else if (blk->type == bplib_mpool_blocktype_primary_ref)
        {
            cab = (bplib_mpool_any_buffer_t *)cab->ref.pref_target;
            assert(cab->std_link.type == bplib_mpool_blocktype_primary);
            printf("DEBUG: %s(): block addr=%lx refcount=%zu, primary ref dest IPN %lu\n", __func__, (unsigned long)blk,
                cab->primary.refcount_link.refcount, (unsigned long)cab->primary.pblock.pri_logical_data.destinationEID.ssp.ipn.node_number);
        }

        ++depth;
    }
    printf("DEBUG: %s(): %s depth=%lu\n", __func__, label, (unsigned long)depth);

}

void bplib_mpool_debug_scan(bplib_mpool_t *pool)
{
    size_t                    i;
    bplib_mpool_any_buffer_t *pchunk;
    uint32_t count_by_type[bplib_mpool_blocktype_max];
    uint32_t count_invalid;

    printf("DEBUG: %s(): total blocks=%zu, buffer_size=%zu, alloced=%zu, recycled=%zu, inuse=%zu\n", __func__,
        pool->num_bufs, pool->buffer_size, pool->alloc_count, pool->recycled_count, pool->alloc_count - pool->recycled_count);

    bplib_mpool_debug_print_queue_stats(&pool->free_blocks, "free_blocks");
    bplib_mpool_debug_print_queue_stats(&pool->active_bundles, "active_bundles");
    bplib_mpool_debug_print_queue_stats(&pool->recycle_blocks, "recycle_blocks");
    bplib_mpool_debug_print_queue_stats(&pool->active_flow_list, "active_flow_list");

    memset(count_by_type, 0, sizeof(count_by_type));
    count_invalid = 0;
    pchunk = &pool->first_buffer;
    for (i=0; i < pool->num_bufs; ++i)
    {
        if (pchunk->std_link.type < bplib_mpool_blocktype_max)
        {
            ++count_by_type[pchunk->std_link.type];
        }
        else
        {
            ++count_invalid;
        }
        ++pchunk;
    }

    for (i = 0; i < bplib_mpool_blocktype_max; ++i)
    {
        printf("DEBUG: %s(): block type=%zu count=%lu\n", __func__, i, (unsigned long)count_by_type[i]);
    }
    printf("DEBUG: %s(): invalid count=%lu\n", __func__, (unsigned long)count_invalid);

}

bplib_mpool_t *bplib_mpool_create(void *pool_mem, size_t pool_size)
{
    bplib_mpool_t                  *pool;
    size_t                    remain;
    bplib_mpool_any_buffer_t *pchunk;

    /* this is just a sanity check, a pool that has only 1 block will not
     * be useful for anything, but it can at least be created */
    if (pool_mem == NULL || pool_size < sizeof(bplib_mpool_t))
    {
        /* pool memory too small */
        return NULL;
    }

    /* wiping the entire memory might be overkill, but it is only done once
     * at start up, and this may also help verify that the memory "works" */
    memset(pool_mem, 0, pool_size);

    pool = pool_mem;

    /* the block lists are circular, as this reduces
     * complexity of operations (never a null pointer) */
    pool->buffer_size = sizeof(bplib_mpool_any_buffer_t);
    bplib_mpool_link_reset(&pool->free_blocks, bplib_mpool_blocktype_head);
    bplib_mpool_link_reset(&pool->active_bundles, bplib_mpool_blocktype_head);
    bplib_mpool_link_reset(&pool->recycle_blocks, bplib_mpool_blocktype_head);
    bplib_mpool_link_reset(&pool->active_flow_list, bplib_mpool_blocktype_head);
    pchunk = &pool->first_buffer;
    remain = pool_size - offsetof(bplib_mpool_t, first_buffer);

    while (remain >= sizeof(bplib_mpool_any_buffer_t))
    {
        bplib_mpool_insert_before(&pool->free_blocks, &pchunk->std_link);
        remain -= sizeof(bplib_mpool_any_buffer_t);
        ++pchunk;
        ++pool->num_bufs;
    }

    printf("%s(): created pool of size %zu, with %zu chunks\n", __func__, pool_size, pool->num_bufs);

    return pool;
}
