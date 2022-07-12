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

/**
 * @file
 *
 * @brief Contains the memory pool implementation for blocks representing
 * bundles.  This includes primary blocks, canonical blocks, and the
 * CBOR-encoded blocks representing the primary/canonical block data.
 */

#include <string.h>
#include <assert.h>

#include "bplib.h"
#include "bplib_os.h"
#include "v7_types.h"
#include "v7_mpool_internal.h"

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_bblock_primary_cast
 *
 *-----------------------------------------------------------------*/
bplib_mpool_bblock_primary_t *bplib_mpool_bblock_primary_cast(bplib_mpool_block_t *cb)
{
    bplib_mpool_block_content_t *content;

    content = bplib_mpool_block_dereference_content(cb);
    if (content != NULL && content->header.base_link.type == bplib_mpool_blocktype_primary)
    {
        return &content->u.primary.pblock;
    }
    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_bblock_canonical_cast
 *
 *-----------------------------------------------------------------*/
bplib_mpool_bblock_canonical_t *bplib_mpool_bblock_canonical_cast(bplib_mpool_block_t *cb)
{
    bplib_mpool_block_content_t *content;

    content = bplib_mpool_block_dereference_content(cb);
    if (content != NULL && content->header.base_link.type == bplib_mpool_blocktype_canonical)
    {
        return &content->u.canonical.cblock;
    }
    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_bblock_cbor_cast
 *
 *-----------------------------------------------------------------*/
void *bplib_mpool_bblock_cbor_cast(bplib_mpool_block_t *cb)
{
    /* CBOR data blocks are nothing more than generic blocks with a different sig */
    return bplib_mpool_generic_data_cast(cb, MPOOL_CACHE_CBOR_DATA_SIGNATURE);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_bblock_cbor_set_size
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_bblock_cbor_set_size(bplib_mpool_block_t *cb, size_t user_content_size)
{
    bplib_mpool_block_content_t *content;

    content = bplib_mpool_block_dereference_content(cb);
    if (content != NULL && content->header.base_link.type == bplib_mpool_blocktype_generic &&
        content->header.content_type_signature == MPOOL_CACHE_CBOR_DATA_SIGNATURE)
    {
        content->header.user_content_length = user_content_size;
    }
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_bblock_primary_init
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_bblock_primary_init(bplib_mpool_block_t *base_block, bplib_mpool_bblock_primary_t *pblk)
{
    bplib_mpool_init_list_head(base_block, &pblk->cblock_list);
    bplib_mpool_init_list_head(base_block, &pblk->chunk_list);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_bblock_canonical_init
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_bblock_canonical_init(bplib_mpool_block_t *base_block, bplib_mpool_bblock_canonical_t *cblk)
{
    bplib_mpool_init_list_head(base_block, &cblk->chunk_list);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_bblock_primary_alloc
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_t *bplib_mpool_bblock_primary_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg)
{
    bplib_mpool_block_content_t *result;
    bplib_mpool_lock_t          *lock;

    lock   = bplib_mpool_lock_resource(pool);
    result = bplib_mpool_alloc_block_internal(pool, bplib_mpool_blocktype_primary, magic_number, init_arg);
    bplib_mpool_lock_release(lock);

    return (bplib_mpool_block_t *)result;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_bblock_canonical_alloc
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_t *bplib_mpool_bblock_canonical_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg)
{
    bplib_mpool_block_content_t *result;
    bplib_mpool_lock_t          *lock;

    lock   = bplib_mpool_lock_resource(pool);
    result = bplib_mpool_alloc_block_internal(pool, bplib_mpool_blocktype_canonical, magic_number, init_arg);
    bplib_mpool_lock_release(lock);

    return (bplib_mpool_block_t *)result;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_bblock_cbor_alloc
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_t *bplib_mpool_bblock_cbor_alloc(bplib_mpool_t *pool)
{
    bplib_mpool_block_content_t *result;
    bplib_mpool_lock_t          *lock;

    lock = bplib_mpool_lock_resource(pool);
    result =
        bplib_mpool_alloc_block_internal(pool, bplib_mpool_blocktype_generic, MPOOL_CACHE_CBOR_DATA_SIGNATURE, NULL);
    bplib_mpool_lock_release(lock);

    return (bplib_mpool_block_t *)result;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_bblock_cbor_append
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_bblock_cbor_append(bplib_mpool_block_t *head, bplib_mpool_block_t *blk)
{
    /* this just confirms it actually is a CBOR data block */
    assert(bplib_mpool_bblock_cbor_cast(blk) != NULL);
    bplib_mpool_insert_before(head, blk);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_bblock_primary_drop_encode
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_bblock_primary_drop_encode(bplib_mpool_bblock_primary_t *cpb)
{
    bplib_mpool_block_t *elist;

    /* If there is any existing encoded data, return it to the pool */
    elist = bplib_mpool_bblock_primary_get_encoded_chunks(cpb);
    if (bplib_mpool_is_nonempty_list_head(elist))
    {
        bplib_mpool_recycle_all_blocks_in_list(NULL, elist);
    }
    cpb->block_encode_size_cache  = 0;
    cpb->bundle_encode_size_cache = 0;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_bblock_canonical_drop_encode
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_bblock_canonical_drop_encode(bplib_mpool_bblock_canonical_t *ccb)
{
    bplib_mpool_block_t *elist;

    /* If there is any existing encoded data, return it to the pool */
    elist = bplib_mpool_bblock_canonical_get_encoded_chunks(ccb);
    if (bplib_mpool_is_nonempty_list_head(elist))
    {
        bplib_mpool_recycle_all_blocks_in_list(NULL, elist);
    }
    ccb->block_encode_size_cache = 0;

    /* this also invalidates the size of the parent bundle, if it was in one */
    if (ccb->bundle_ref)
    {
        ccb->bundle_ref->bundle_encode_size_cache = 0;
    }
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_bblock_cbor_export
 *
 *-----------------------------------------------------------------*/
size_t bplib_mpool_bblock_cbor_export(bplib_mpool_block_t *list, void *out_ptr, size_t max_out_size, size_t seek_start,
                                      size_t max_count)
{
    bplib_mpool_block_t *blk;
    uint8_t             *curr_ptr;
    const uint8_t       *src_ptr;
    size_t               remain_sz;
    size_t               chunk_sz;
    size_t               seek_left;
    size_t               data_left;

    remain_sz = max_out_size;
    curr_ptr  = out_ptr;
    seek_left = seek_start;
    data_left = max_count;
    blk       = list;
    while (data_left > 0)
    {
        blk     = bplib_mpool_get_next_block(blk);
        src_ptr = bplib_mpool_bblock_cbor_cast(blk);
        if (src_ptr == NULL)
        {
            break;
        }
        chunk_sz = bplib_mpool_get_user_content_size(blk);
        if (seek_left > chunk_sz)
        {
            seek_left -= chunk_sz;
        }
        else
        {
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

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_bblock_primary_append
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_bblock_primary_append(bplib_mpool_bblock_primary_t *cpb, bplib_mpool_block_t *blk)
{
    bplib_mpool_bblock_canonical_t *ccb;

    /* this should only be invoked w/canonical blocks.  Anything else is a bug. */
    ccb = bplib_mpool_bblock_canonical_cast(blk);
    assert(ccb != NULL);
    assert(ccb->bundle_ref == NULL);

    /*
     * TBD on block ordering requirements.  The BPv7 spec says the payload should be last,
     * but other blocks may appear in any order.  It is not clear if there is any reason
     * to be more specific about block order.
     *
     * For now, if the block being inserted has block num 1, put it last, otherwise put it first.
     */
    if (bplib_mpool_bblock_canonical_get_logical(ccb)->canonical_block.blockNum == 1)
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
    ccb->bundle_ref               = cpb;
}

bplib_mpool_block_t *bplib_mpool_bblock_primary_locate_canonical(bplib_mpool_bblock_primary_t *cpb,
                                                                 bp_blocktype_t                block_type)
{
    bplib_mpool_block_t            *cblk;
    bplib_mpool_bblock_canonical_t *ccb;

    cblk = bplib_mpool_bblock_primary_get_canonical_list(cpb);
    while (true)
    {
        /* note this searches in reverse order, anticipating this will often be searching for payload,
         * which is supposed to be the last block in the bundle.  If the block is there it will be found
         * regardless of the search order, but this speeds up the more frequent case */
        cblk = bplib_mpool_get_prev_block(cblk);
        if (bplib_mpool_is_list_head(cblk))
        {
            /* end of list, not found */
            cblk = NULL;
            break;
        }
        ccb = bplib_mpool_bblock_canonical_cast(cblk);
        if (ccb != NULL && ccb->canonical_logical_data.canonical_block.blockType == block_type)
        {
            /* found it */
            break;
        }
    }

    return cblk;
}
