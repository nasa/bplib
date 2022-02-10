/************************************************************************
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
 *************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <string.h>
#include <assert.h>

#include "bplib.h"
#include "bplib_os.h"
#include "v7_types.h"
#include "v7_mpool.h"

typedef struct mpool_cache_refcount
{
    size_t count;
} mpool_cache_refcount_t;

typedef struct mpool_cache_primary_block_node
{
    mpool_cache_block_t         bundle_link;
    mpool_cache_refcount_t      ref;
    mpool_cache_primary_block_t pblock;
} mpool_cache_primary_block_node_t;

typedef struct mpool_cache_encode_block_node
{
    mpool_cache_block_t        chunk_link;
    mpool_cache_encode_block_t eblock; /* variably sized, must be last */
} mpool_cache_encode_block_node_t;

typedef struct mpool_cache_canonical_block_node
{
    mpool_cache_block_t           cblock_link;
    mpool_cache_refcount_t        ref;
    mpool_cache_canonical_block_t cblock;
} mpool_cache_canonical_block_node_t;

typedef struct mpool_cache_primary_block_ref_node_t
{
    mpool_cache_block_t               bundle_link;
    mpool_cache_primary_block_node_t *pblocknode;
} mpool_cache_primary_block_ref_node_t;

typedef struct mpool_cache_canonical_block_ref_node_t
{
    mpool_cache_block_t                 cblock_link;
    mpool_cache_canonical_block_node_t *cblocknode;
} mpool_cache_canonical_block_ref_node_t;

typedef union mpool_cache_any_buffer
{
    mpool_cache_block_t                    link;
    mpool_cache_primary_block_node_t       primary;
    mpool_cache_primary_block_ref_node_t   primary_ref;
    mpool_cache_canonical_block_node_t     canonical;
    mpool_cache_canonical_block_ref_node_t canonical_ref;
    mpool_cache_encode_block_node_t        encoded;
    uint8_t                                max_content[BP_MPOOL_MAX_ENCODED_CHUNK_SIZE];
} mpool_cache_any_buffer_t;

struct mpool
{
    size_t                   num_bufs;
    size_t                   buffer_size;
    mpool_cache_block_t      free_blocks;
    mpool_cache_block_t      common_blocks;
    mpool_cache_block_t      outgoing_bundles;
    mpool_cache_block_t      incoming_bundles;
    mpool_cache_any_buffer_t first_buffer;
};

static void mpool_decr_refcount(mpool_t *pool, mpool_cache_block_t *cb);

#define MPOOL_ENCODE_CHUNK_CAPACITY \
    (sizeof(mpool_cache_any_buffer_t) - offsetof(mpool_cache_encode_block_node_t, eblock.content_start))

size_t mpool_get_encode_data_capacity(const mpool_cache_encode_block_t *ceb)
{
    return MPOOL_ENCODE_CHUNK_CAPACITY;
}

void mpool_link_reset(mpool_cache_block_t *link, mpool_cache_blocktype_t type)
{
    link->type = type;
    link->next = link;
    link->prev = link;
}

void mpool_insert_after(mpool_cache_block_t *list, mpool_cache_block_t *node)
{
    node->next       = list->next;
    node->prev       = list;
    list->next       = node;
    node->next->prev = node;
}

void mpool_insert_before(mpool_cache_block_t *list, mpool_cache_block_t *node)
{
    node->prev       = list->prev;
    node->next       = list;
    list->prev       = node;
    node->prev->next = node;
}

void mpool_extract_node(mpool_cache_block_t *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next       = node;
    node->prev       = node;
}

void mpool_merge_listx(mpool_cache_block_t *dest, mpool_cache_block_t *src)
{
    mpool_cache_block_t *dlast = dest->prev; /* last node in dest list */
    mpool_cache_block_t *slast = src->prev;  /* last node in dest list */

    /* nominally combine the two lists.
     * NOTE: This (temporarily) yields a list with two head nodes. */
    dlast->next = src;
    slast->next = dest;
    dest->prev  = slast;
    src->prev   = dlast;

    /* extract the "src" which separates them back into two lists.
     * the src list will now be empty */
    // x mpool_extract_node(src);
}

mpool_cache_primary_block_t *mpool_cast_primary(mpool_cache_block_t *cb)
{
    if (cb != NULL)
    {
        if (cb->type == mpool_cache_blocktype_primary)
        {
            return &((mpool_cache_primary_block_node_t *)cb)->pblock;
        }
        if (cb->type == mpool_cache_blocktype_primary_ref)
        {
            return &((mpool_cache_primary_block_ref_node_t *)cb)->pblocknode->pblock;
        }
    }
    return NULL;
}

mpool_cache_refcount_t *mpool_get_refcount(mpool_cache_block_t *cb)
{
    mpool_cache_any_buffer_t *cab;

    if (cb != NULL)
    {
        cab = (mpool_cache_any_buffer_t *)cb;
        if (cb->type == mpool_cache_blocktype_primary)
        {
            return &cab->primary.ref;
        }
        if (cb->type == mpool_cache_blocktype_primary_ref)
        {
            return &cab->primary_ref.pblocknode->ref;
        }
        if (cb->type == mpool_cache_blocktype_canonical)
        {
            return &cab->canonical.ref;
        }
        if (cb->type == mpool_cache_blocktype_canonical_ref)
        {
            return &cab->canonical_ref.cblocknode->ref;
        }
    }
    return NULL;
}

mpool_cache_canonical_block_t *mpool_cast_canonical(mpool_cache_block_t *cb)
{
    mpool_cache_any_buffer_t *cab;

    if (cb != NULL)
    {
        cab = (mpool_cache_any_buffer_t *)cb;

        if (cb->type == mpool_cache_blocktype_canonical)
        {
            return &cab->canonical.cblock;
        }
        if (cb->type == mpool_cache_blocktype_canonical_ref)
        {
            return &cab->canonical_ref.cblocknode->cblock;
        }
    }
    return NULL;
}

mpool_cache_encode_block_t *mpool_cast_encode_block(mpool_cache_block_t *cb)
{
    if (cb != NULL && cb->type == mpool_cache_blocktype_encoded_chunk)
    {
        return &((mpool_cache_encode_block_node_t *)cb)->eblock;
    }
    return NULL;
}

mpool_cache_block_t *mpool_get_next_outgoing_bundle(mpool_t *pool)
{
    mpool_cache_block_t *node = pool->outgoing_bundles.next;

    /* All nodes in the outgoing list should have a linktype of primary.
     * If not, then this means the list is empty */
    if (node->type != mpool_cache_blocktype_primary)
    {
        return NULL;
    }

    return node;
}

mpool_cache_block_t *mpool_get_next_incoming_bundle(mpool_t *pool)
{
    mpool_cache_block_t *node = pool->incoming_bundles.next;

    /* All nodes in the incoming list should have a linktype of primary.
     * If not, then this means the list is empty */
    if (node->type != mpool_cache_blocktype_primary)
    {
        return NULL;
    }

    return node;
}

mpool_cache_block_t *mpool_alloc_primary_block(mpool_t *pool)
{
    mpool_cache_block_t              *node = pool->free_blocks.next;
    mpool_cache_primary_block_node_t *cpbn;

    /* All nodes in the free list should have a linktype of undefined.
     * If not, then this means the list is empty */
    if (node->type != mpool_cache_blocktype_undefined)
    {
        return NULL;
    }

    /* extract the node from the free list, and reconfigure it as a pri block */
    mpool_extract_node(node);

    node->type = mpool_cache_blocktype_primary;
    cpbn       = (mpool_cache_primary_block_node_t *)node;

    mpool_link_reset(&cpbn->pblock.cblock_list, mpool_cache_blocktype_head);
    mpool_link_reset(&cpbn->pblock.chunk_list, mpool_cache_blocktype_head);

    /* wipe the logical content to be sure no stale data exists */
    memset(&cpbn->pblock.content_start, 0, sizeof(cpbn->pblock.content_start));

    return node;
}

mpool_cache_block_t *mpool_alloc_canonical_block(mpool_t *pool)
{
    mpool_cache_block_t                *node = pool->free_blocks.next;
    mpool_cache_canonical_block_node_t *ccb;

    /* All nodes in the free list should have a linktype of undefined.
     * If not, then this means the list is empty */
    if (node->type != mpool_cache_blocktype_undefined)
    {
        return NULL;
    }

    /* extract the node from the free list, and reconfigure it as a canonical block */
    mpool_extract_node(node);

    node->type = mpool_cache_blocktype_canonical;
    ccb        = (mpool_cache_canonical_block_node_t *)node;

    mpool_link_reset(&ccb->cblock.chunk_list, mpool_cache_blocktype_head);

    /* wipe the logical content to be sure no stale data exists */
    memset(&ccb->cblock.content_start, 0, sizeof(ccb->cblock.content_start));
    mpool_set_canonical_block_encoded_content_detail(&ccb->cblock, 0, 0);

    return node;
}

mpool_cache_block_t *mpool_alloc_encode_block(mpool_t *pool)
{
    mpool_cache_block_t             *node = pool->free_blocks.next;
    mpool_cache_encode_block_node_t *ceb;

    /* All nodes in the free list should have a linktype of undefined.
     * If not, then this means the list is empty */
    if (node->type != mpool_cache_blocktype_undefined)
    {
        return NULL;
    }

    /* extract the node from the free list, and reconfigure it as a canonical block */
    mpool_extract_node(node);

    node->type = mpool_cache_blocktype_encoded_chunk;
    ceb        = (mpool_cache_encode_block_node_t *)node;

    ceb->eblock.encoded_length = 0;

    return node;
}

void mpool_append_encode_block(mpool_cache_block_t *head, mpool_cache_block_t *blk)
{
    assert(blk->type == mpool_cache_blocktype_encoded_chunk);
    mpool_insert_before(head, blk);
}

void mpool_return_single_encode_block(mpool_t *pool, mpool_cache_block_t *blk)
{
    assert(blk->type == mpool_cache_blocktype_encoded_chunk);
    mpool_extract_node(blk);
    blk->type = mpool_cache_blocktype_undefined;
    mpool_insert_before(&pool->free_blocks, blk);
}

void mpool_return_all_encode_blocks(mpool_t *pool, mpool_cache_block_t *list)
{
    mpool_cache_block_t *ptr;

    assert(list->type == mpool_cache_blocktype_head);

    ptr = list->next;
    while (ptr->type != mpool_cache_blocktype_head)
    {
        /*
         * all nodes should be of the encoded_chunk type here ---
         * other block types may contain other refs, so its important to ONLY use this
         * routine for encoded chunks.  Use mpool_decr_refcount() to collect ANY type of block.
         */
        assert(ptr->type == mpool_cache_blocktype_encoded_chunk);
        ptr->type = mpool_cache_blocktype_undefined;
        ptr       = ptr->next;
    }

    mpool_merge_listx(&pool->free_blocks, list);
    mpool_extract_node(list);
}

void mpool_return_all_blocks_in_list(mpool_t *pool, mpool_cache_block_t *list)
{
    mpool_cache_block_t *ptr;

    assert(list->type == mpool_cache_blocktype_head);

    ptr = list->next;
    while (ptr->type != mpool_cache_blocktype_head)
    {
        mpool_decr_refcount(pool, ptr);

        /* whether it was a ref or direct block, this block can get freed */
        ptr->type = mpool_cache_blocktype_undefined;
        ptr       = ptr->next;
    }

    mpool_merge_listx(&pool->free_blocks, list);
    mpool_extract_node(list);
}

/*
 * Decrement the canonical block refcount
 * The refcount allows a single representation in memory to be virtually
 * included in many different bundles at once (useful for e.g. previous hop)
 *
 * This has no effect if called on a non-canonical block
 */
void mpool_decr_refcount(mpool_t *pool, mpool_cache_block_t *cb)
{
    mpool_cache_canonical_block_node_t *ccbn;
    mpool_cache_primary_block_node_t   *cpbn;
    mpool_cache_any_buffer_t           *cab;
    mpool_cache_refcount_t             *pref;
    mpool_cache_block_t                *pcontent;

    cab  = (mpool_cache_any_buffer_t *)cb;
    ccbn = NULL;
    cpbn = NULL;

    /* Dereference the node if indirect */
    switch (cb->type)
    {
        case mpool_cache_blocktype_canonical:
        {
            ccbn = &cab->canonical;
            break;
        }
        case mpool_cache_blocktype_primary:
        {
            cpbn = &cab->primary;
            break;
        }
        case mpool_cache_blocktype_canonical_ref:
        {
            ccbn                          = cab->canonical_ref.cblocknode;
            cab->canonical_ref.cblocknode = NULL; /* this ref is going away */
            break;
        }
        case mpool_cache_blocktype_primary_ref:
        {
            cpbn                        = cab->primary_ref.pblocknode;
            cab->primary_ref.pblocknode = NULL; /* this ref is going away */
            break;
        }
        default:
        {
            break;
        }
    }

    if (cpbn)
    {
        pref     = &cpbn->ref;
        pcontent = &cpbn->bundle_link;
    }
    else if (ccbn)
    {
        pref     = &ccbn->ref;
        pcontent = &ccbn->cblock_link;
    }
    else
    {
        pref     = NULL;
        pcontent = cb;
    }

    if (pref != NULL && pref->count > 0)
    {
        --pref->count;
    }

    /* If not refcounted OR if the refcount got to zero, then clean up the object  */
    /* TBD: part of this block recovery could also be deferred to a garbage collector */
    if (pref == NULL || pref->count == 0)
    {
        /* Collect the content chunks too */
        if (cpbn != NULL)
        {
            mpool_return_all_blocks_in_list(pool, &cpbn->pblock.cblock_list);
            mpool_return_all_encode_blocks(pool, &cpbn->pblock.chunk_list);
        }
        if (ccbn != NULL)
        {
            mpool_return_all_encode_blocks(pool, &ccbn->cblock.chunk_list);
        }

        /* ONLY if this was a ref node, delete the content here */
        /* The ref itself will be removed by the caller */
        if (pcontent != cb)
        {
            /* now reset this node and return it to the free block list */
            mpool_extract_node(pcontent);
            pcontent->type = mpool_cache_blocktype_undefined;
            mpool_insert_before(&pool->free_blocks, pcontent);
        }
    }
}

void mpool_return_block(mpool_t *pool, mpool_cache_block_t *cb)
{
    mpool_decr_refcount(pool, cb);
}

size_t mpool_sum_encoded_size(mpool_cache_block_t *list)
{
    mpool_cache_block_t        *blk;
    mpool_cache_encode_block_t *ceb;
    size_t                      size_sum;

    size_sum = 0;
    blk      = list;
    while (true)
    {
        blk = mpool_get_next_block(blk);
        ceb = mpool_cast_encode_block(blk);
        if (ceb == NULL)
        {
            break;
        }
        size_sum += ceb->encoded_length;
    }

    return size_sum;
}

size_t mpool_sum_full_bundle_size(mpool_cache_primary_block_t *cpb)
{
    mpool_cache_block_t           *blk;
    mpool_cache_canonical_block_t *ccb;
    size_t                         size_sum;

    size_sum = mpool_sum_encoded_size(&cpb->chunk_list);
    blk      = &cpb->cblock_list;
    while (true)
    {
        blk = mpool_get_next_block(blk);
        ccb = mpool_cast_canonical(blk);
        if (ccb == NULL)
        {
            break;
        }
        size_sum += mpool_sum_encoded_size(&ccb->chunk_list);
    }

    return size_sum;
}

size_t mpool_copy_block_chain(mpool_cache_block_t *list, void *out_ptr, size_t max_out_size, size_t seek_start,
                              size_t max_count)
{
    mpool_cache_block_t        *blk;
    mpool_cache_encode_block_t *ceb;
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
        blk = mpool_get_next_block(blk);
        ceb = mpool_cast_encode_block(blk);
        if (ceb == NULL)
        {
            break;
        }
        chunk_sz = ceb->encoded_length;
        if (seek_left > chunk_sz)
        {
            seek_left -= chunk_sz;
        }
        else
        {
            src_ptr = &ceb->content_start;
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

#ifdef jphfix
typedef void (*mpool_canonical_block_handler_t)(mpool_cache_canonical_block_t *, void *arg);

void mpool_foreach_canonical_block(bp_primary_block_t *pri, mpool_canonical_block_handler_t handler, void *arg)
{
    mpool_cache_primary_block_t   *pri_block;
    mpool_cache_canonical_block_t *canon_block;
    mpool_cache_block_t           *ptr;

    pri_block = mpool_container_of(pri, mpool_cache_primary_block_t);
    assert(pri_block->bundle_link.type == mpool_cache_blocktype_primary);
    assert(pri_block->cblock_list.type == mpool_cache_blocktype_head);

    ptr = pri_block->cblock_list.next;
    while (ptr->type != mpool_cache_blocktype_head)
    {
        /* all nodes should be of the canonical type here */
        assert(ptr->type == mpool_cache_blocktype_canonical);
        handler((mpool_cache_canonical_block_t *)ptr, arg);
        ptr = ptr->next;
    }
}

void mpool_append_canonical_block(bp_primary_block_t *pri, bp_canonical_block_buffer_t *canon)
{
    mpool_cache_primary_block_t   *pri_block;
    mpool_cache_canonical_block_t *canon_block;

    pri_block = mpool_container_of(pri, mpool_cache_primary_block_t);
    assert(pri_block->bundle_link.type == mpool_cache_blocktype_primary);

    canon_block = mpool_container_of(canon, mpool_cache_canonical_block_t);
    assert(canon_block->cblock_link.type == mpool_cache_blocktype_canonical);

    mpool_insert_before(&pri_block->cblock_list, &canon_block->cblock_link);
}
#endif

void mpool_store_primary_block_incoming(mpool_t *pool, mpool_cache_block_t *cpb)
{
    assert(cpb->type == mpool_cache_blocktype_primary);
    mpool_insert_before(&pool->incoming_bundles, cpb);
}

void mpool_store_primary_block_outgoing(mpool_t *pool, mpool_cache_block_t *cpb)
{
    assert(cpb->type == mpool_cache_blocktype_primary);
    mpool_insert_before(&pool->outgoing_bundles, cpb);
}

void mpool_store_canonical_block(mpool_cache_primary_block_t *cpb, mpool_cache_block_t *blk)
{
    mpool_cache_canonical_block_t *ccb;

    /* this should only be invoked w/canonical blocks.  Anything else is a bug. */
    ccb = mpool_cast_canonical(blk);
    assert(ccb);

    /*
     * TBD on block ordering requirements.  The BPv7 spec says the payload should be last,
     * but other blocks may appear in any order.  It is not clear if there is any reason
     * to be more specific about block order.
     *
     * For now, if the block being inserted is payload, put it last, otherwise put it first.
     */

    if (mpool_get_canonical_block_logical(ccb)->canonical_block.blockType == bp_blocktype_payloadBlock)
    {
        /* this puts it last */
        mpool_insert_before(&cpb->cblock_list, blk);
    }
    else
    {
        /* this puts it first */
        mpool_insert_after(&cpb->cblock_list, blk);
    }
}

void mpool_start_stream_init(mpool_stream_t *mps, mpool_t *pool, mpool_stream_dir_t dir, bp_crctype_t crctype)
{
    memset(mps, 0, sizeof(*mps));

    mpool_link_reset(&mps->head, mpool_cache_blocktype_head);

    mps->dir       = dir;
    mps->pool      = pool;
    mps->last_eblk = &mps->head;

    switch (crctype)
    {
        case bp_crctype_CRC16:
            mps->crc_params = &BPLIB_CRC16_X25;
            break;
        case bp_crctype_CRC32C:
            mps->crc_params = &BPLIB_CRC32_CASTAGNOLI;
            break;
        default:
            mps->crc_params = &BPLIB_CRC_NONE;
            break;
    }

    mps->crcval = bplib_crc_initial_value(mps->crc_params);
}

size_t mpool_stream_write(mpool_stream_t *mps, const void *data, size_t size)
{
    mpool_cache_block_t        *next_block;
    mpool_cache_encode_block_t *curr_eblk;
    const uint8_t              *chunk_p;
    uint8_t                    *out_p;
    size_t                      chunk_sz;
    size_t                      remain_sz;

    if (mps->dir != mpool_stream_dir_write || size == 0)
    {
        return 0;
    }

    remain_sz = size;
    chunk_p   = data;
    while (remain_sz > 0)
    {
        /* If no block is ready, get one now */
        if (mps->curr_pos >= mps->curr_limit)
        {
            next_block = mpool_alloc_encode_block(mps->pool);
            if (next_block == NULL)
            {
                break;
            }

            mpool_append_encode_block(&mps->head, next_block);

            mps->last_eblk  = next_block;
            mps->curr_pos   = 0;
            mps->curr_limit = MPOOL_ENCODE_CHUNK_CAPACITY;
        }

        curr_eblk = mpool_cast_encode_block(mps->last_eblk);

        chunk_sz = mps->curr_limit - mps->curr_pos;
        if (chunk_sz > remain_sz)
        {
            chunk_sz = remain_sz;
        }

        out_p = &curr_eblk->content_start + mps->curr_pos;
        memcpy(out_p, chunk_p, chunk_sz);
        mps->crcval = bplib_crc_update(mps->crc_params, mps->crcval, out_p, chunk_sz);

        mps->curr_pos += chunk_sz;
        curr_eblk->encoded_length = mps->curr_pos;
        mps->stream_position += chunk_sz;
        remain_sz -= chunk_sz;
        chunk_p += chunk_sz;
    }

    return (size - remain_sz);
}

size_t mpool_stream_seek(mpool_stream_t *mps, size_t target_position)
{
    mpool_cache_block_t        *next_block;
    mpool_cache_encode_block_t *curr_eblk;
    size_t                      block_avail;
    size_t                      chunk_sz;

    /*
     * Loop to move the stream position forward
     * On a read this should just advance through the existing data
     * On a write this should generate zeros to fill the gap
     * If a CRC calculation is included, this will update the CRC accordingly
     */
    while (target_position > mps->stream_position)
    {
        chunk_sz = target_position - mps->stream_position;

        /* If no block is ready, get one now */
        if (mps->curr_pos >= mps->curr_limit)
        {
            if (mps->dir == mpool_stream_dir_read)
            {
                next_block = mpool_get_next_block(mps->last_eblk);
            }
            else if (mps->dir == mpool_stream_dir_write)
            {
                next_block = mpool_alloc_encode_block(mps->pool);
            }
            else
            {
                next_block = NULL;
            }

            curr_eblk = mpool_cast_encode_block(next_block);
            if (curr_eblk == NULL)
            {
                break;
            }

            mps->curr_pos  = 0;
            mps->last_eblk = next_block;

            if (mps->dir == mpool_stream_dir_write)
            {
                mpool_append_encode_block(&mps->head, next_block);
                mps->curr_limit = mpool_get_encode_data_capacity(curr_eblk);
            }
            else
            {
                mps->curr_limit = mpool_get_encode_data_actual_size(curr_eblk);
            }
        }
        else
        {
            curr_eblk = mpool_cast_encode_block(mps->last_eblk);
        }

        block_avail = mps->curr_limit - mps->curr_pos;

        if (block_avail < chunk_sz)
        {
            chunk_sz = block_avail;
        }

        /*
         * On a write stream this must zero-fill the data
         */
        if (mps->dir == mpool_stream_dir_write)
        {
            memset(&curr_eblk->content_start + mps->curr_pos, 0, chunk_sz);
            curr_eblk->encoded_length += chunk_sz;
        }

        mps->crcval =
            bplib_crc_update(mps->crc_params, mps->crcval, &curr_eblk->content_start + mps->curr_pos, chunk_sz);
        mps->curr_pos += chunk_sz;
        mps->stream_position += chunk_sz;
    }

    /*
     * Loop to move the stream position backwards
     * On a read this should just back up through the existing data
     * On a write this should drop/delete data
     * If a CRC calculation is being done, this will invalidate the CRC
     */
    while (target_position < mps->stream_position)
    {
        chunk_sz = mps->stream_position - target_position;

        if (mps->curr_pos == 0)
        {
            /* Need to move back a block */
            next_block = mpool_get_prev_block(mps->last_eblk);
            curr_eblk  = mpool_cast_encode_block(next_block);
            if (curr_eblk == NULL)
            {
                break;
            }

            if (mps->dir == mpool_stream_dir_write)
            {
                mpool_return_single_encode_block(mps->pool, mps->last_eblk);
                mps->curr_limit = mpool_get_encode_data_capacity(curr_eblk);
                mps->curr_pos   = mpool_get_encode_data_capacity(curr_eblk);
            }
            else
            {
                mps->curr_limit = mpool_get_encode_data_actual_size(curr_eblk);
                mps->curr_pos   = mps->curr_limit;
            }

            mps->last_eblk = next_block;
        }

        if (mps->curr_pos < chunk_sz)
        {
            chunk_sz = mps->curr_pos;
        }

        mps->curr_pos -= chunk_sz;
        mps->stream_position -= chunk_sz;
    }

    return mps->stream_position;
}

size_t mpool_stream_read(mpool_stream_t *mps, void *data, size_t size)
{
    mpool_cache_block_t        *next_block;
    mpool_cache_encode_block_t *curr_eblk;
    const uint8_t              *in_p;
    uint8_t                    *chunk_p;
    size_t                      chunk_sz;
    size_t                      remain_sz;

    if (mps->dir != mpool_stream_dir_read || size == 0)
    {
        return 0;
    }

    remain_sz = size;
    chunk_p   = data;
    while (remain_sz > 0)
    {
        /* If no block is ready, get one now */
        if (mps->curr_pos >= mps->curr_limit)
        {
            next_block = mpool_get_next_block(mps->last_eblk);
            curr_eblk  = mpool_cast_encode_block(next_block);
            if (curr_eblk == NULL)
            {
                /* end of stream */
                break;
            }

            mps->last_eblk  = next_block;
            mps->curr_limit = mpool_get_encode_data_actual_size(curr_eblk);
            mps->curr_pos   = 0;
        }
        else
        {
            curr_eblk = mpool_cast_encode_block(mps->last_eblk);
        }

        chunk_sz = mps->curr_limit - mps->curr_pos;
        if (chunk_sz > remain_sz)
        {
            chunk_sz = remain_sz;
        }

        in_p = &curr_eblk->content_start + mps->curr_pos;
        memcpy(chunk_p, in_p, chunk_sz);
        mps->crcval = bplib_crc_update(mps->crc_params, mps->crcval, in_p, chunk_sz);

        mps->curr_pos += chunk_sz;
        mps->stream_position += chunk_sz;
        remain_sz -= chunk_sz;
        chunk_p += chunk_sz;
    }

    return (size - remain_sz);
}

void mpool_stream_attach(mpool_stream_t *mps, mpool_cache_block_t *head)
{
    mpool_merge_listx(head, &mps->head);
    mpool_extract_node(&mps->head);

    mps->last_eblk       = &mps->head;
    mps->curr_limit      = 0;
    mps->curr_pos        = 0;
    mps->stream_position = 0;
    mps->crcval          = bplib_crc_initial_value(mps->crc_params);
}

uint8_t mpool_stream_get_crc_bit_size(const mpool_stream_t *mps)
{
    return bplib_crc_get_width(mps->crc_params);
}

bp_crcval_t mpool_stream_get_current_crc(const mpool_stream_t *mps)
{
    return bplib_crc_finalize(mps->crc_params, mps->crcval);
}

void mpool_stream_close(mpool_stream_t *mps)
{
    /* discard anything that wasn't saved (will be a no-op if it was saved) */
    mpool_return_all_encode_blocks(mps->pool, &mps->head);
}

mpool_t *mpool_create(void *pool_mem, size_t pool_size)
{
    mpool_t                  *pool;
    size_t                    remain;
    mpool_cache_any_buffer_t *pchunk;

    /* this is just a sanity check, a pool that has only 1 block will not
     * be useful for anything, but it can at least be created */
    if (pool_mem == NULL || pool_size < sizeof(mpool_t))
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
    pool->buffer_size = sizeof(mpool_cache_any_buffer_t);
    mpool_link_reset(&pool->free_blocks, mpool_cache_blocktype_head);
    mpool_link_reset(&pool->outgoing_bundles, mpool_cache_blocktype_head);
    mpool_link_reset(&pool->incoming_bundles, mpool_cache_blocktype_head);

    pchunk = &pool->first_buffer;
    remain = pool_size - offsetof(mpool_t, first_buffer);

    while (remain >= sizeof(mpool_cache_any_buffer_t))
    {
        mpool_insert_before(&pool->free_blocks, &pchunk->link);
        remain -= sizeof(mpool_cache_any_buffer_t);
        ++pchunk;
        ++pool->num_bufs;
    }

    printf("%s(): created pool of size %zu, with %zu chunks\n", __func__, pool_size, pool->num_bufs);

    return pool;
}
