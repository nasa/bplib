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
#include "v7_mpool_internal.h"

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_link_reset
 *
 *-----------------------------------------------------------------*/
static inline void bplib_mpool_link_reset(bplib_mpool_block_t *link, bplib_mpool_blocktype_t type)
{
    link->type = type;
    link->next = link;
    link->prev = link;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_get_block_content
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_content_t *bplib_mpool_get_block_content(bplib_mpool_block_t *cb)
{
    size_t offset;

    if (cb != NULL)
    {
        /* Check if this is a secondary index, and if so, jump to the actual block base */
        offset = (size_t)cb->type;
        offset -= bplib_mpool_blocktype_secondary_link_base;
        if (offset > 0 && offset < sizeof(bplib_mpool_block_content_t))
        {
            cb = (bplib_mpool_block_t *)((uint8_t *)cb - offset);
        }

        /* this group of block types has a full header (notably this does NOT include list head nodes) */
        if (bplib_mpool_is_any_content_node(cb))
        {
            return (bplib_mpool_block_content_t *)cb;
        }
    }
    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_obtain_base_block
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_t *bplib_mpool_obtain_base_block(bplib_mpool_block_t *cb)
{
    bplib_mpool_block_content_t *block_ptr;

    block_ptr = bplib_mpool_get_block_content(cb);

    if (block_ptr != NULL)
    {
        /* Additionally, if this block is a ref, then also dereference it to get to the real block */
        /* In theory this could be a chain of refs, so this is a while() but in reality it should be just one */
        while (block_ptr->header.base_link.type == bplib_mpool_blocktype_ref)
        {
            block_ptr = block_ptr->u.ref.pref_target;
        }

        return &block_ptr->header.base_link;
    }

    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_get_generic_data_capacity
 *
 *-----------------------------------------------------------------*/
size_t bplib_mpool_get_generic_data_capacity(const bplib_mpool_block_t *cb)
{
    if (!bplib_mpool_is_generic_data_block(cb))
    {
        return 0;
    }
    return MPOOL_GET_BLOCK_USER_CAPACITY(generic_data);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_init_list_head
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_init_list_head(bplib_mpool_block_t *head)
{
    bplib_mpool_link_reset(head, bplib_mpool_blocktype_head);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_init_secondary_link
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_init_secondary_link(bplib_mpool_block_t *base_block, bplib_mpool_block_t *secondary_link)
{
    size_t offset;

    offset = (uint8_t *)secondary_link - (uint8_t *)base_block;
    assert(offset > 0 && offset < sizeof(bplib_mpool_block_content_t));
    bplib_mpool_link_reset(secondary_link, bplib_mpool_blocktype_secondary_link_base + offset);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_insert_after
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_insert_after(bplib_mpool_block_t *list, bplib_mpool_block_t *node)
{
    node->next       = list->next;
    node->prev       = list;
    list->next       = node;
    node->next->prev = node;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_insert_before
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_insert_before(bplib_mpool_block_t *list, bplib_mpool_block_t *node)
{
    node->prev       = list->prev;
    node->next       = list;
    list->prev       = node;
    node->prev->next = node;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_extract_node
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_extract_node(bplib_mpool_block_t *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next       = node;
    node->prev       = node;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_merge_list
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_merge_list(bplib_mpool_block_t *dest, bplib_mpool_block_t *src)
{
    bplib_mpool_block_t *dlast = dest->prev; /* last node in dest list */
    bplib_mpool_block_t *slast = src->prev;  /* last node in src list */

    /* nominally combine the two lists.
     * NOTE: This (temporarily) yields a list with two head nodes. */
    dlast->next = src;
    slast->next = dest;
    dest->prev  = slast;
    src->prev   = dlast;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_get_user_content_size
 *
 *-----------------------------------------------------------------*/
size_t bplib_mpool_get_user_content_size(const bplib_mpool_block_t *cb)
{
    const bplib_mpool_block_content_t *block;

    block = bplib_mpool_get_block_content((bplib_mpool_block_t *)cb);
    if (block == NULL)
    {
        return 0;
    }
    return block->header.user_content_length;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_read_refcount
 *
 *-----------------------------------------------------------------*/
size_t bplib_mpool_read_refcount(const bplib_mpool_block_t *cb)
{
    const bplib_mpool_block_content_t *block;

    block = bplib_mpool_get_block_content((bplib_mpool_block_t *)cb);
    if (block == NULL)
    {
        return 0;
    }
    return block->header.refcount;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_get_user_data_offset_by_blocktype
 *
 *-----------------------------------------------------------------*/
size_t bplib_mpool_get_user_data_offset_by_blocktype(bplib_mpool_blocktype_t bt)
{
    static const size_t USER_DATA_START_OFFSET[bplib_mpool_blocktype_max] = {
        [bplib_mpool_blocktype_undefined] = SIZE_MAX,
        [bplib_mpool_blocktype_head]      = SIZE_MAX,
        [bplib_mpool_blocktype_api]       = MPOOL_GET_BUFFER_USER_START_OFFSET(api),
        [bplib_mpool_blocktype_generic]   = MPOOL_GET_BUFFER_USER_START_OFFSET(generic_data),
        [bplib_mpool_blocktype_cbor_data] = MPOOL_GET_BUFFER_USER_START_OFFSET(generic_data),
        [bplib_mpool_blocktype_primary]   = MPOOL_GET_BUFFER_USER_START_OFFSET(primary),
        [bplib_mpool_blocktype_canonical] = MPOOL_GET_BUFFER_USER_START_OFFSET(canonical),
        [bplib_mpool_blocktype_flow]      = MPOOL_GET_BUFFER_USER_START_OFFSET(flow),
        [bplib_mpool_blocktype_ref]       = MPOOL_GET_BUFFER_USER_START_OFFSET(ref)};

    if (bt >= bplib_mpool_blocktype_max)
    {
        return SIZE_MAX;
    }

    return USER_DATA_START_OFFSET[bt];
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_generic_data_cast
 *
 *-----------------------------------------------------------------*/
void *bplib_mpool_generic_data_cast(bplib_mpool_block_t *cb, uint32_t required_magic)
{
    bplib_mpool_block_content_t *block;
    size_t                       data_offset;

    block = bplib_mpool_get_block_content(cb);
    if (block != NULL && block->header.content_type_signature == required_magic)
    {
        data_offset = bplib_mpool_get_user_data_offset_by_blocktype(block->header.base_link.type);
        if (data_offset < sizeof(bplib_mpool_block_buffer_t))
        {
            return &block->u.content_bytes[data_offset];
        }
    }
    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_init_base_object
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_init_base_object(bplib_mpool_block_header_t *block_hdr, uint16_t user_content_length,
                                  uint32_t content_type_signature)
{
    block_hdr->user_content_length    = user_content_length;
    block_hdr->content_type_signature = content_type_signature;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_alloc_block_internal
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_content_t *bplib_mpool_alloc_block_internal(bplib_mpool_t *pool, bplib_mpool_blocktype_t blocktype,
                                                              uint32_t content_type_signature, void *init_arg)
{
    bplib_mpool_block_t         *node;
    bplib_mpool_block_content_t *block;
    bplib_mpool_api_content_t   *api_block;
    size_t                       data_offset;

    /* Only real blocks are allocated here - not secondary links nor head nodes,
     * as those are embedded within the blocks themselves. */
    if (blocktype < bplib_mpool_blocktype_api || blocktype >= bplib_mpool_blocktype_max)
    {
        return NULL;
    }

    /*
     * Check free block threshold: Note that it may take additional pool blocks (refs, cbor, etc)
     * in order to forward stored bundles along when the time comes.
     *
     * Without a sufficient buffer/threshold, if the entire pool is consumed by bundles, it will
     * prevent any blocks from being forwarded (and eventually freed) as well, creating deadlock.
     *
     * This soft limit only applies for actual bundle blocks, not for refs.
     */
    if ((pool->alloc_count - pool->recycled_count) > pool->alloc_threshold &&
        (blocktype == bplib_mpool_blocktype_primary || blocktype == bplib_mpool_blocktype_canonical))
    {
        return NULL;
    }

    /* figure out how to initialize this block by looking up the content type */
    api_block = (bplib_mpool_api_content_t *)bplib_rbt_search(content_type_signature, &pool->xblocktype_registry);
    if (api_block == NULL)
    {
        /* no constructor, cannot create the block! */
        return NULL;
    }

    /* sanity check that the user content will fit in the block */
    data_offset = bplib_mpool_get_user_data_offset_by_blocktype(blocktype);
    if (data_offset > sizeof(bplib_mpool_block_buffer_t) ||
        (data_offset + api_block->user_content_size) > sizeof(bplib_mpool_block_buffer_t))
    {
        /* User content will not fit in the block - cannot create an instance of this type combo */
        return NULL;
    }

    /* All nodes in the free list should have a linktype of undefined.
     * If not, then this means the list is empty */
    node = pool->free_blocks.next;
    if (node->type != bplib_mpool_blocktype_undefined)
    {
        return NULL;
    }

    /* extract the node from the free list, and reconfigure it as the requested block type */
    bplib_mpool_extract_node(node);
    ++pool->alloc_count;
    block = (bplib_mpool_block_content_t *)node;

    /*
     * zero fill the content part first, this ensures that this is always done,
     * and avoids the need for the module to supply a dedicated constructor just to zero it
     */
    memset(&block->u, 0, data_offset + api_block->user_content_size);

    bplib_mpool_init_base_object(&block->header, api_block->user_content_size, content_type_signature);

    switch (blocktype)
    {
        case bplib_mpool_blocktype_primary:
            bplib_mpool_bblock_primary_init(&block->u.primary.pblock);
            break;
        case bplib_mpool_blocktype_canonical:
            bplib_mpool_bblock_canonical_init(&block->u.canonical.cblock);
            break;
        case bplib_mpool_blocktype_flow:
            bplib_mpool_init_secondary_link(node, &block->u.flow.processable_link);
            bplib_mpool_flow_init(&block->u.flow.fblock);
            break;
        default:
            /* nothing more for this node type (this catches cbor_data)  */
            break;
    }

    node->type = blocktype;

    /* If the module did supply a constructor, invoke it now */
    if (api_block->api.construct != NULL)
    {
        api_block->api.construct(init_arg, node);
    }

    return block;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_generic_data_alloc
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_t *bplib_mpool_generic_data_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg)
{
    return (bplib_mpool_block_t *)bplib_mpool_alloc_block_internal(pool, bplib_mpool_blocktype_generic, magic_number,
                                                                   init_arg);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_recycle_all_blocks_in_list
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_recycle_all_blocks_in_list(bplib_mpool_t *pool, bplib_mpool_block_t *list)
{
    assert(list->type == bplib_mpool_blocktype_head);
    bplib_mpool_merge_list(&pool->recycle_blocks, list);
    bplib_mpool_extract_node(list);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_recycle_block
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_recycle_block(bplib_mpool_t *pool, bplib_mpool_block_t *blk)
{
    bplib_mpool_extract_node(blk);
    bplib_mpool_insert_before(&pool->recycle_blocks, blk);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_foreach_item_in_list
 *
 *-----------------------------------------------------------------*/
int bplib_mpool_foreach_item_in_list(bplib_mpool_block_t *list, bool always_remove,
                                     bplib_mpool_callback_func_t callback_fn, void *callback_arg)
{
    bplib_mpool_block_t *curr_node;
    bplib_mpool_block_t *next_node;
    int                  count;

    /*
     * Note: To handle the possibility that the callback removes the node
     * from this list and puts it into another list, this needs to preemptively
     * get the next node from this list before running the callback.
     */
    next_node = list->next;
    count     = 0;
    while (next_node != list)
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

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_register_blocktype
 *
 *-----------------------------------------------------------------*/
int bplib_mpool_register_blocktype(bplib_mpool_t *pool, uint32_t magic_number, const bplib_mpool_blocktype_api_t *api,
                                   size_t user_content_size)
{
    bplib_mpool_block_content_t *ablk;
    bplib_mpool_api_content_t   *api_block;
    int                          status;

    /* before doing anything, check if this is a duplicate.  If so, ignore it.
     * This permits "lazy binding" of apis where the blocktype is registered at the time of first use */
    if (bplib_rbt_search(magic_number, &pool->xblocktype_registry) != NULL)
    {
        return BP_DUPLICATE;
    }

    ablk = bplib_mpool_alloc_block_internal(pool, bplib_mpool_blocktype_api, 0, NULL);
    if (ablk == NULL)
    {
        return BP_ERROR;
    }

    api_block = &ablk->u.api;

    if (api != NULL)
    {
        api_block->api = *api;
    }
    api_block->user_content_size = user_content_size;

    status = bplib_rbt_insert_value(magic_number, &pool->xblocktype_registry, &api_block->rbt_link);

    /* due to the pre-check above this should always have been successful, but just in case, return the block if error
     */
    if (status != BP_SUCCESS)
    {
        bplib_mpool_recycle_block(pool, &ablk->header.base_link);
    }

    return status;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_maintain
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_maintain(bplib_mpool_t *pool)
{
    bplib_mpool_block_t          pending_recycle;
    bplib_mpool_block_t         *rblk;
    bplib_mpool_api_content_t   *api_block;
    bplib_mpool_block_content_t *content;
    size_t                       recycled_count;

    recycled_count = 0;
    bplib_mpool_init_list_head(&pending_recycle);

    while (!bplib_mpool_is_empty_list_head(&pool->recycle_blocks))
    {
        rblk = &pending_recycle;

        bplib_mpool_merge_list(rblk, &pool->recycle_blocks);
        bplib_mpool_extract_node(&pool->recycle_blocks);

        while (true)
        {
            rblk = bplib_mpool_get_next_block(rblk);
            if (rblk == &pending_recycle)
            {
                break;
            }

            /* recycled blocks must all be "real" blocks (not secondary refs or head nodes, etc) and
             * have refcount of 0, or else bad things might happen */
            assert(bplib_mpool_is_any_content_node(rblk));
            content = (bplib_mpool_block_content_t *)rblk;
            assert(content->header.refcount == 0);

            /* figure out how to de-initialize the user content by looking up the content type */
            api_block = (bplib_mpool_api_content_t *)bplib_rbt_search(content->header.content_type_signature,
                                                                      &pool->xblocktype_registry);
            if (api_block != NULL && api_block->api.destruct != NULL)
            {
                /* note that, like in C++, one cannot pass an arg to the destructor here.  It
                 * uses the same API/function pointer type, the arg will always be NULL. */
                api_block->api.destruct(NULL, rblk);
            }

            /* now de-initialize the base content */
            switch (rblk->type)
            {
                case bplib_mpool_blocktype_canonical:
                {
                    bplib_mpool_recycle_all_blocks_in_list(pool, &content->u.canonical.cblock.chunk_list);
                    break;
                }
                case bplib_mpool_blocktype_primary:
                {
                    bplib_mpool_recycle_all_blocks_in_list(pool, &content->u.primary.pblock.cblock_list);
                    bplib_mpool_recycle_all_blocks_in_list(pool, &content->u.primary.pblock.chunk_list);
                    break;
                }
                case bplib_mpool_blocktype_flow:
                {
                    bplib_mpool_recycle_all_blocks_in_list(pool, &content->u.flow.fblock.ingress.block_listx);
                    bplib_mpool_recycle_all_blocks_in_list(pool, &content->u.flow.fblock.egress.block_listx);
                    break;
                }
                case bplib_mpool_blocktype_ref:
                {
                    bplib_mpool_ref_release(pool, content->u.ref.pref_target);
                    content->u.ref.pref_target = NULL; /* this ref is going away */
                    break;
                }
                default:
                {
                    break;
                }
            }

            printf("DEBUG: %s() recycled block type %d\n", __func__, rblk->type);

            /* always return _this_ node to the free pile */
            bplib_mpool_init_base_object(&content->header, 0, 0);
            rblk->type = bplib_mpool_blocktype_undefined;
            ++recycled_count;
        }

        bplib_mpool_merge_list(&pool->free_blocks, rblk);
        bplib_mpool_extract_node(rblk);
    }

    if (recycled_count != 0)
    {
        printf("DEBUG: %s() recycled %lu blocks\n", __func__, (unsigned long)recycled_count);
    }

    pool->recycled_count += recycled_count;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_debug_print_queue_stats
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_debug_print_queue_stats(bplib_mpool_block_t *list, const char *label)
{
    bplib_mpool_block_t         *blk;
    bplib_mpool_block_content_t *content;
    size_t                       depth;

    blk   = list;
    depth = 0;
    while (true)
    {
        blk = bplib_mpool_get_next_block(blk);
        if (blk->type == bplib_mpool_blocktype_head)
        {
            assert(blk == list);
            break;
        }

        if (bplib_mpool_is_any_content_node(blk))
        {
            content = (bplib_mpool_block_content_t *)blk;
            printf("DEBUG: %s(): block addr=%lx type=%d refcount=%u\n", __func__, (unsigned long)blk,
                   content->header.base_link.type, content->header.refcount);

            if (blk->type == bplib_mpool_blocktype_canonical)
            {
                printf("DEBUG: %s():  --> canonical block type %d\n", __func__,
                       (int)content->u.canonical.cblock.canonical_logical_data.canonical_block.blockType);
            }
            else if (blk->type == bplib_mpool_blocktype_primary)
            {
                printf("DEBUG: %s():  -->  primary dest IPN %lu\n", __func__,
                       (unsigned long)content->u.primary.pblock.pri_logical_data.destinationEID.ssp.ipn.node_number);
            }
        }

        ++depth;
    }
    printf("DEBUG: %s(): %s depth=%lu\n", __func__, label, (unsigned long)depth);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_debug_scan
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_debug_scan(bplib_mpool_t *pool)
{
    size_t                       i;
    bplib_mpool_block_content_t *pchunk;
    uint32_t                     count_by_type[bplib_mpool_blocktype_max];
    uint32_t                     count_invalid;

    printf("DEBUG: %s(): total blocks=%u, buffer_size=%zu, alloced=%u, recycled=%u, inuse=%u\n", __func__,
           (unsigned int)pool->num_bufs_total, pool->buffer_size, (unsigned int)pool->alloc_count,
           (unsigned int)pool->recycled_count, (unsigned int)(pool->alloc_count - pool->recycled_count));

    bplib_mpool_debug_print_queue_stats(&pool->free_blocks, "free_blocks");
    bplib_mpool_debug_print_queue_stats(&pool->ref_managed_blocks, "ref_managed_blocks");
    bplib_mpool_debug_print_queue_stats(&pool->recycle_blocks, "recycle_blocks");
    bplib_mpool_debug_print_queue_stats(&pool->active_flow_list, "active_flow_list");

    memset(count_by_type, 0, sizeof(count_by_type));
    count_invalid = 0;
    pchunk        = &pool->first_buffer;
    for (i = 0; i < pool->num_bufs_total; ++i)
    {
        if (pchunk->header.base_link.type < bplib_mpool_blocktype_max)
        {
            ++count_by_type[pchunk->header.base_link.type];
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

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_create
 *
 *-----------------------------------------------------------------*/
bplib_mpool_t *bplib_mpool_create(void *pool_mem, size_t pool_size)
{
    bplib_mpool_t               *pool;
    size_t                       remain;
    bplib_mpool_block_content_t *pchunk;

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
    pool->buffer_size = sizeof(bplib_mpool_block_content_t);
    bplib_mpool_init_list_head(&pool->free_blocks);
    bplib_mpool_init_list_head(&pool->ref_managed_blocks);
    bplib_mpool_init_list_head(&pool->recycle_blocks);
    bplib_mpool_init_list_head(&pool->active_flow_list);
    bplib_rbt_init_root(&pool->xblocktype_registry);
    pchunk = &pool->first_buffer;
    remain = pool_size - offsetof(bplib_mpool_t, first_buffer);

    /* register the first API type, which is 0.
     * Notably this prevents other modules from actually registering something at 0. */
    bplib_rbt_insert_value(0, &pool->xblocktype_registry, &pool->blocktype_basic.rbt_link);

    while (remain >= sizeof(bplib_mpool_block_content_t))
    {
        bplib_mpool_insert_before(&pool->free_blocks, &pchunk->header.base_link);
        remain -= sizeof(bplib_mpool_block_content_t);
        ++pchunk;
        ++pool->num_bufs_total;
    }

    /*
     * Set the alloc threshold at 70% of the total (just a guess)
     * The intent is to NOT allow the entire pool to be used by bundles,
     * there must be some buffer room for refs which transport the bundles.
     */
    pool->alloc_threshold = (pool->num_bufs_total * 7) / 10;
    printf("%s(): created pool of size %zu, with %u chunks, threshold = %u\n", __func__, pool_size,
           (unsigned int)pool->num_bufs_total, (unsigned int)pool->alloc_threshold);

    return pool;
}
