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
#include <stdio.h> // TODO Change to compile-time conditional DEBUG

#include "bplib.h"
#include "bplib_api_types.h"
#include "bplib_mem_internal.h"
#include "bplib_mem_bblocks.h"

/**
 * @brief Maxmimum number of blocks to be collected in a single maintenace cycle
 */
#define BPLIB_MPOOL_MAINTENCE_COLLECT_LIMIT 20

#define BPLIB_MPOOL_NUM_LOCKS 1 /* for now */

bplib_mpool_lock_t BPLIB_MPOOL_LOCK_SET[BPLIB_MPOOL_NUM_LOCKS];

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_link_reset
 *
 *-----------------------------------------------------------------*/
static inline void bplib_mpool_link_reset(bplib_mpool_block_t *link, bplib_mpool_blocktype_t type,
                                          uint32_t parent_offset)
{
    link->type          = type;
    link->parent_offset = parent_offset;
    link->next          = link;
    link->prev          = link;
}

void bplib_mpool_lock_init(void)
{
    uint32_t            i;
    bplib_mpool_lock_t *lock;

    /* note - this relies on the BSS section being properly zero'ed out at start */
    for (i = 0; i < BPLIB_MPOOL_NUM_LOCKS; ++i)
    {
        lock = &BPLIB_MPOOL_LOCK_SET[i];
        if (!bp_handle_is_valid(lock->lock_id))
        {
            lock->lock_id = bplib_os_createlock();
        }
    }
}

bplib_mpool_lock_t *bplib_mpool_lock_prepare(void *resource_addr)
{
    /*
     * for now, this always uses the same lock (coarse-grained locking) but in the future
     * it might become a striped or finer-grained bucketed lock for more concurrency
     */
    return &BPLIB_MPOOL_LOCK_SET[0];
}

bplib_mpool_lock_t *bplib_mpool_lock_resource(void *resource_addr)
{
    bplib_mpool_lock_t *selected_lock;

    /*
     * for now, this always uses the same lock (coarse-grained locking) but in the future
     * it might become a striped or finer-grained bucketed lock for more concurrency
     */
    selected_lock = bplib_mpool_lock_prepare(resource_addr);
    bplib_mpool_lock_acquire(selected_lock);

    return selected_lock;
}

bool bplib_mpool_lock_wait(bplib_mpool_lock_t *lock, uint64_t until_dtntime)
{
    bool within_timeout;
    int  status;

    within_timeout = (until_dtntime > bplib_os_get_dtntime_ms());
    if (within_timeout)
    {
        status = bplib_os_wait_until_ms(lock->lock_id, until_dtntime);
        if (status == BP_TIMEOUT)
        {
            /* if timeout was returned, then assume that enough time has elapsed
             * such that the current time is beyond until_dtntime now.  note that
             * the caller should still check for whatever condition was being waited
             * on. */
            within_timeout = false;
        }
    }

    return within_timeout;
}

bplib_mpool_block_t *bplib_mpool_block_from_external_id(bplib_mpool_t *pool, bp_handle_t handle)
{
    bplib_mpool_block_admin_content_t *admin;
    bplib_mpool_block_content_t       *blk;
    int                                serial;

    admin  = bplib_mpool_get_admin(pool);
    serial = bp_handle_to_serial(handle, BPLIB_HANDLE_MPOOL_BASE);
    if (serial < admin->num_bufs_total)
    {
        blk = &pool->admin_block;
        blk += serial;
    }
    else
    {
        blk = NULL;
    }

    return blk->header.base_link; // TODO Is this return value correct?
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_get_block_from_link
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_t *bplib_mpool_get_block_from_link(bplib_mpool_block_t *lblk)
{
    bplib_mpool_block_t *bblk;

    bblk = lblk;

    /* Check if this is a secondary index, and if so, jump to the actual block base */
    /* this check of the type is not strictly needed, as the offset should be set to 0 for main blocks */
    while (bblk != NULL && bplib_mpool_is_secondary_index_node(bblk) && bblk->parent_offset != 0)
    {
        /* the parent_offset field indicates this block position within the parent */
        bblk = (bplib_mpool_block_t *)(void *)((uint8_t *)bblk - bblk->parent_offset);
    }

    return bblk;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_get_block_content
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_content_t *bplib_mpool_get_block_content(bplib_mpool_block_t *cb)
{
    if (cb != NULL && bplib_mpool_is_any_content_node(cb))
    {
        return (bplib_mpool_block_content_t *)(void *)cb;
    }
    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_get_block_content_const
 *
 *-----------------------------------------------------------------*/
const bplib_mpool_block_content_t *bplib_mpool_get_block_content_const(const bplib_mpool_block_t *cb)
{
    if (cb != NULL && bplib_mpool_is_any_content_node(cb))
    {
        return (const bplib_mpool_block_content_t *)(const void *)cb;
    }
    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_block_dereference_content
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_content_t *bplib_mpool_block_dereference_content(bplib_mpool_block_t *cb)
{
    bplib_mpool_block_content_t *block_ptr;

    block_ptr = bplib_mpool_get_block_content(cb);

    if (block_ptr != NULL)
    {
        /* Additionally, if this block is a ref, then also dereference it to get to the real block */
        /* In theory this could be a chain of refs, so this is a while() but in reality it should be just one */
        while (block_ptr->header.base_link->type == bplib_mpool_blocktype_ref)
        {
            assert(block_ptr->u.ref.pref_target != NULL);
            block_ptr = block_ptr->u.ref.pref_target;
            /* this should have always arrived at an actual content block */
            assert(bplib_mpool_is_any_content_node(block_ptr->header.base_link)); // TODO Is base_link correct as pointer?
        }

        return block_ptr;
    }

    return NULL;
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
        [bplib_mpool_blocktype_api]       = MPOOL_GET_BUFFER_USER_START_OFFSET(api),
        [bplib_mpool_blocktype_generic]   = MPOOL_GET_BUFFER_USER_START_OFFSET(generic_data),
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
 * Function: bplib_mpool_get_generic_data_capacity
 *
 *-----------------------------------------------------------------*/
size_t bplib_mpool_get_generic_data_capacity(const bplib_mpool_block_t *cb)
{
    size_t data_offset;

    data_offset = bplib_mpool_get_user_data_offset_by_blocktype(cb->type);
    if (data_offset > sizeof(bplib_mpool_block_buffer_t))
    {
        return 0;
    }

    return sizeof(bplib_mpool_block_buffer_t) - data_offset;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_init_secondary_link
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_init_secondary_link(bplib_mpool_block_t *base_block, bplib_mpool_block_t *secondary_link,
                                     bplib_mpool_blocktype_t block_type)
{
    size_t offset;

    if (base_block == NULL)
    {
        offset = 0;
    }
    else
    {
        offset = (uint8_t *)secondary_link - (uint8_t *)base_block;
        assert(offset > 0 && offset < sizeof(bplib_mpool_block_content_t));
    }

    bplib_mpool_link_reset(secondary_link, block_type, offset);

    assert(bplib_mpool_is_secondary_index_node(secondary_link));
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_init_list_head
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_init_list_head(bplib_mpool_block_t *base_block, bplib_mpool_block_t *list_head)
{
    bplib_mpool_init_secondary_link(base_block, list_head, bplib_mpool_blocktype_list_head);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_insert_after
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_insert_after(bplib_mpool_block_t *list, bplib_mpool_block_t *node)
{
    /* node being inserted should always be a singleton */
    assert(bplib_mpool_is_link_unattached(node));

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
    /* node being inserted should always be a singleton */
    assert(bplib_mpool_is_link_unattached(node));

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

    block = bplib_mpool_get_block_content_const(cb);
    if (block != NULL)
    {
        return block->header.user_content_length;
    }
    return 0;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_read_refcount
 *
 *-----------------------------------------------------------------*/
size_t bplib_mpool_read_refcount(const bplib_mpool_block_t *cb)
{
    const bplib_mpool_block_content_t *block;

    block = bplib_mpool_get_block_content_const(cb);
    if (block != NULL)
    {
        return block->header.refcount;
    }
    return 0;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_get_parent_pool_from_link
 *
 *-----------------------------------------------------------------*/
bplib_mpool_t *bplib_mpool_get_parent_pool_from_link(bplib_mpool_block_t *cb)
{
    bplib_mpool_block_content_t *block;

    block = bplib_mpool_get_block_content(bplib_mpool_get_block_from_link(cb));
    if (block != NULL)
    {
        /* the "parent_offset" should provide a map back to the parent pool.
         * in this context the units are blocks, not bytes (this extends the
         * representable range for large pools, and simplifies this arithmetic) */
        block -= block->header.base_link->parent_offset;

        /* this should have always arrived at the admin block, which is the first block */
        assert(block->header.base_link->type == bplib_mpool_blocktype_admin);
    }
    else
    {
        block = NULL;
    }

    return (bplib_mpool_t *)block;
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
    void                        *result;

    /* note that any block such as refs, flows, bundle blocks, etc. may have generic data
     * associated with it, not just a generic_data block.  The difference is that the generic
     * data block _only_ has the generic data, whereas the other block types can have both. */
    result = NULL;
    block  = bplib_mpool_get_block_content(cb);
    while (block != NULL && bplib_mpool_is_any_content_node(block->header.base_link))
    {
        if (block->header.content_type_signature == required_magic)
        {
            data_offset = bplib_mpool_get_user_data_offset_by_blocktype(block->header.base_link->type);
            if (data_offset < sizeof(bplib_mpool_block_buffer_t))
            {
                result = &block->u.content_bytes[data_offset];
            }
            break;
        }

        if (!bplib_mpool_is_indirect_block(block->header.base_link))
        {
            break;
        }

        block = block->u.ref.pref_target;
    }

    return result;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_generic_data_uncast
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_t *bplib_mpool_generic_data_uncast(void *blk, bplib_mpool_blocktype_t parent_bt,
                                                     uint32_t required_magic)
{
    bplib_mpool_block_content_t *block;
    size_t                       data_offset;

    data_offset = bplib_mpool_get_user_data_offset_by_blocktype(parent_bt);
    if (data_offset > sizeof(bplib_mpool_block_buffer_t))
    {
        return NULL;
    }

    data_offset += offsetof(bplib_mpool_block_content_t, u);
    block = (bplib_mpool_block_content_t *)(void *)((uint8_t *)blk - data_offset);
    if (block->header.base_link->type != parent_bt || block->header.content_type_signature != required_magic)
    {
        return NULL;
    }

    return block->header.base_link;
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
 * NOTE: this must be invoked with the lock already held
 *-----------------------------------------------------------------*/
bplib_mpool_block_content_t *bplib_mpool_alloc_block_internal(bplib_mpool_t *pool, bplib_mpool_blocktype_t blocktype,
                                                              uint32_t content_type_signature, void *init_arg,
                                                              uint8_t priority)
{
    bplib_mpool_block_t         *node;
    bplib_mpool_block_content_t *block;
    bplib_mpool_api_content_t   *api_block;
    size_t                       data_offset;
    uint32_t                     alloc_threshold;
    uint32_t                     block_count;

    bplib_mpool_block_admin_content_t *admin;

    admin = bplib_mpool_get_admin(pool);

    /* Only real blocks are allocated here - not secondary links nor head nodes,
     * as those are embedded within the blocks themselves. */
    if (blocktype == bplib_mpool_blocktype_undefined || blocktype >= bplib_mpool_blocktype_max)
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
    alloc_threshold = (admin->bblock_alloc_threshold * priority) / 255;
    alloc_threshold = 1000; // TODO Reset back to threshold computation.

    #ifdef STOR

    block_count = bplib_mpool_subq_get_depth(admin->free_blocks);
    if (block_count <= (admin->bblock_alloc_threshold - alloc_threshold))
    {

        /* no free blocks available for the requested type */
        return NULL;
    }

    #endif // STOR
    /* figure out how to initialize this block by looking up the content type */
    api_block = (bplib_mpool_api_content_t *)(void *)bplib_rbt_search_unique(content_type_signature,
                                                                             &admin->blocktype_registry);
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

    /* get a block */
    node = bplib_mpool_subq_pull_single(admin->free_blocks);
    if (node == NULL)
    {
        /* this should never happen, because depth was already checked */
        return NULL;
    }

    /*
     * Convert from blocks free to blocks used, and update high watermark if necessary.
     * This is +1 to include the block that was just pulled (that is, a call to
     * bplib_mpool_subq_get_depth() on the free list now will return 1 fewer than it
     * did earlier in this function).
     */
    block_count = 1 + admin->num_bufs_total - block_count;
    if (block_count > admin->max_alloc_watermark)
    {
        admin->max_alloc_watermark = block_count;
    }

    node->type = blocktype;
    block      = bplib_mpool_get_block_content(node);

    /*
     * zero fill the content part first, this ensures that this is always done,
     * and avoids the need for the module to supply a dedicated constructor just to zero it
     */
    memset(&block->u, 0, data_offset + api_block->user_content_size);

    bplib_mpool_init_base_object(&block->header, api_block->user_content_size, content_type_signature);

    switch (blocktype)
    {
        case bplib_mpool_blocktype_primary:
            bplib_mpool_bblock_primary_init(node, block->u.primary.pblock);
            break;
        case bplib_mpool_blocktype_canonical:
            bplib_mpool_bblock_canonical_init(node, block->u.canonical.cblock);
            break;
        case bplib_mpool_blocktype_flow:
            bplib_mpool_flow_init(node, block->u.flow.fblock);
            break;
        default:
            /* nothing more for this node type (this catches cbor_data)  */
            break;
    }

    /* If the module did supply a constructor, invoke it now */
    if (api_block->api->construct != NULL)
    {
        /* A constructor really should never fail nominally, if it does there is probably a bug */
        if (api_block->api->construct(init_arg, node) != BP_SUCCESS)
        {
            // TODO why is BP_FLAG_DIAGNOSTIC undefined?
            // bplog(NULL, BP_FLAG_DIAGNOSTIC, "Constructor failed for block type %d, signature %lx\n", blocktype,
            //      (unsigned long)content_type_signature);
        }
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
    bplib_mpool_block_content_t *result;
    bplib_mpool_lock_t          *lock;

    lock   = bplib_mpool_lock_resource(pool);
    result = bplib_mpool_alloc_block_internal(pool, bplib_mpool_blocktype_generic, magic_number, init_arg,
                                              BPLIB_MPOOL_ALLOC_PRI_MLO);
    bplib_mpool_lock_release(lock);

    return (bplib_mpool_block_t *)result;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_recycle_block_internal
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_recycle_block_internal(bplib_mpool_t *pool, bplib_mpool_block_t *blk)
{
    bplib_mpool_block_admin_content_t *admin;

    admin = bplib_mpool_get_admin(pool);

    bplib_mpool_extract_node(blk);
    bplib_mpool_subq_push_single(&admin->recycle_blocks, blk);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_recycle_all_blocks_in_list
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_recycle_all_blocks_in_list(bplib_mpool_t *pool, bplib_mpool_block_t *list)
{
    bplib_mpool_lock_t                *lock;
    bplib_mpool_block_admin_content_t *admin;

    /*
     * If the pool was not specified, then attempt to deduce from the list pointer.
     *
     * List objects embedded within pool objects have breadcrumbs such that the parent pool
     * pointer can be reconstituted.  However, if the list is NOT part of a pool object
     * (e.g. a temporary list created on the stack, for instance) then this does not work.
     */
    if (pool == NULL)
    {
        pool = bplib_mpool_get_parent_pool_from_link(list);
    }

    admin = bplib_mpool_get_admin(pool);

    assert(bplib_mpool_is_list_head(list));
    lock = bplib_mpool_lock_resource(pool);
    bplib_mpool_subq_merge_list(&admin->recycle_blocks, list);
    bplib_mpool_lock_release(lock);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_recycle_block
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_recycle_block(bplib_mpool_block_t *blk)
{
    bplib_mpool_lock_t *lock;
    bplib_mpool_t      *pool;

    /* only real content blocks should be recycled.  No secondary links or components/members. */
    assert(bplib_mpool_is_any_content_node(blk));

    pool = bplib_mpool_get_parent_pool_from_link(blk);

    lock = bplib_mpool_lock_resource(pool);
    bplib_mpool_recycle_block_internal(pool, blk);
    bplib_mpool_lock_release(lock);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_list_iter_goto_first
 *
 *-----------------------------------------------------------------*/
int bplib_mpool_list_iter_goto_first(const bplib_mpool_block_t *list, bplib_mpool_list_iter_t *iter)
{
    if (!bplib_mpool_is_list_head(list))
    {
        return BP_ERROR;
    }

    iter->pending_entry = list->next;

    return bplib_mpool_list_iter_forward(iter);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_list_iter_goto_last
 *
 *-----------------------------------------------------------------*/
int bplib_mpool_list_iter_goto_last(const bplib_mpool_block_t *list, bplib_mpool_list_iter_t *iter)
{
    if (!bplib_mpool_is_list_head(list))
    {
        return BP_ERROR;
    }

    iter->pending_entry = list->prev;

    return bplib_mpool_list_iter_reverse(iter);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_list_iter_forward
 *
 *-----------------------------------------------------------------*/
int bplib_mpool_list_iter_forward(bplib_mpool_list_iter_t *iter)
{
    if (iter->pending_entry == NULL || bplib_mpool_is_list_head(iter->pending_entry))
    {
        iter->position = NULL;
        return BP_ERROR;
    }

    iter->position      = iter->pending_entry;
    iter->pending_entry = iter->position->next;
    return BP_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_list_iter_reverse
 *
 *-----------------------------------------------------------------*/
int bplib_mpool_list_iter_reverse(bplib_mpool_list_iter_t *iter)
{
    if (iter->pending_entry == NULL || bplib_mpool_is_list_head(iter->pending_entry))
    {
        iter->position = NULL;
        return BP_ERROR;
    }

    iter->position      = iter->pending_entry;
    iter->pending_entry = iter->position->prev;
    return BP_SUCCESS;
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
 * Function: bplib_mpool_search_list
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_t *bplib_mpool_search_list(const bplib_mpool_block_t *list, bplib_mpool_callback_func_t match_fn,
                                             void *match_arg)
{
    int                     status;
    bplib_mpool_list_iter_t iter;

    memset(&iter, 0, sizeof(iter));

    status = bplib_mpool_list_iter_goto_first(list, &iter);
    while (status == BP_SUCCESS)
    {
        /* this calls the match function with the actual content block, as that is where
         * the real information lies (this is typically a list full of secondary links) */
        if (match_fn(match_arg, bplib_mpool_get_block_from_link(iter.position)) == 0)
        {
            break;
        }
        status = bplib_mpool_list_iter_forward(&iter);
    }

    /* the iterator sets position to NULL if end of list was reached */
    return iter.position;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_register_blocktype_internal
 *
 *-----------------------------------------------------------------*/
int bplib_mpool_register_blocktype_internal(bplib_mpool_t *pool, uint32_t magic_number,
                                            const bplib_mpool_blocktype_api_t *api, size_t user_content_size)
{
    bplib_mpool_block_content_t       *ablk;
    bplib_mpool_api_content_t         *api_block;
    int                                status;
    bplib_mpool_block_admin_content_t *admin;

    admin = bplib_mpool_get_admin(pool);

    /* before doing anything, check if this is a duplicate.  If so, ignore it.
     * This permits "lazy binding" of apis where the blocktype is registered at the time of first use */
    if (bplib_rbt_search_unique(magic_number, &admin->blocktype_registry) != NULL)
    {
        return BP_DUPLICATE;
    }

    ablk = bplib_mpool_alloc_block_internal(pool, bplib_mpool_blocktype_api, 0, NULL, BPLIB_MPOOL_ALLOC_PRI_LO);
    if (ablk == NULL)
    {
        return BP_ERROR;
    }

    api_block = &ablk->u.api;

    if (api != NULL)
    {
        api_block->api = api;
    }
    api_block->user_content_size = user_content_size;

    status = bplib_rbt_insert_value_unique(magic_number, &admin->blocktype_registry, &api_block->rbt_link);

    /* due to the pre-check above this should always have been successful, but just in case, return the block if error
     */
    if (status != BP_SUCCESS)
    {
        bplib_mpool_recycle_block_internal(pool, &ablk->header.base_link);
    }

    return status;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_register_blocktype
 *
 *-----------------------------------------------------------------*/
int bplib_mpool_register_blocktype(bplib_mpool_t *pool, uint32_t magic_number, const bplib_mpool_blocktype_api_t *api,
                                   size_t user_content_size)
{
    bplib_mpool_lock_t *lock;
    int                 result;

    lock   = bplib_mpool_lock_resource(pool);
    result = bplib_mpool_register_blocktype_internal(pool, magic_number, api, user_content_size);
    bplib_mpool_lock_release(lock);
    return result;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_collect_blocks
 *
 *-----------------------------------------------------------------*/
uint32_t bplib_mpool_collect_blocks(bplib_mpool_t *pool, uint32_t limit)
{
    bplib_mpool_block_t               *rblk;
    bplib_mpool_api_content_t         *api_block;
    bplib_mpool_block_content_t       *content;
    bplib_mpool_callback_func_t        destruct;
    uint32_t                           count;
    bplib_mpool_lock_t                *lock;
    bplib_mpool_block_admin_content_t *admin;

    admin = bplib_mpool_get_admin(pool);

    count = 0;
    lock  = bplib_mpool_lock_resource(pool);
    while (count < limit)
    {
        rblk = bplib_mpool_subq_pull_single(&admin->recycle_blocks);
        if (rblk == NULL)
        {
            break;
        }

        /* recycled blocks must all be "real" blocks (not secondary refs or head nodes, etc) and
         * have refcount of 0, or else bad things might happen */
        content = bplib_mpool_get_block_content(rblk);
        assert(content != NULL);
        assert(content->header.refcount == 0);

        /* figure out how to de-initialize the user content by looking up the content type */
        api_block = (bplib_mpool_api_content_t *)(void *)bplib_rbt_search_unique(content->header.content_type_signature,
                                                                                 &admin->blocktype_registry);

        if (api_block != NULL)
        {
            destruct = api_block->api->destruct;
        }
        else
        {
            destruct = NULL;
        }

        /* pool should be UN-locked when invoking destructor */
        bplib_mpool_lock_release(lock);

        /* note that, like in C++, one cannot pass an arg to the destructor here.  It
         * uses the same API/function pointer type, the arg will always be NULL. */
        if (destruct != NULL)
        {
            destruct(NULL, rblk);
        }

        /* now de-initialize the base content */
        switch (rblk->type)
        {
            case bplib_mpool_blocktype_canonical:
            {
                bplib_mpool_lock_acquire(lock);
                bplib_mpool_subq_merge_list(&admin->recycle_blocks, &content->u.canonical.cblock->chunk_list);
                bplib_mpool_lock_release(lock);
                break;
            }
            case bplib_mpool_blocktype_primary:
            {
                bplib_mpool_lock_acquire(lock);
                bplib_mpool_subq_merge_list(&admin->recycle_blocks, &content->u.primary.pblock->cblock_list);
                bplib_mpool_subq_merge_list(&admin->recycle_blocks, &content->u.primary.pblock->chunk_list);
                bplib_mpool_lock_release(lock);
                break;
            }
            case bplib_mpool_blocktype_flow:
            {
                bplib_mpool_lock_acquire(lock);
                #ifdef STOR
                bplib_mpool_subq_move_all(&admin->recycle_blocks, &content->u.flow.fblock.ingress.base_subq);
                bplib_mpool_subq_move_all(&admin->recycle_blocks, &content->u.flow.fblock.egress.base_subq);
                #endif // STOR
                bplib_mpool_lock_release(lock);
                break;
            }
            case bplib_mpool_blocktype_ref:
            {
                bplib_mpool_ref_release(content->u.ref.pref_target);
                content->u.ref.pref_target = NULL; /* this ref is going away */
                break;
            }
            default:
            {
                break;
            }
        }

        // printf("DEBUG: %s() recycled block type %d\n", __func__, rblk->type);
        ++count;

        /* always return _this_ node to the free pile */
        rblk->type = bplib_mpool_blocktype_undefined;
        bplib_mpool_init_base_object(&content->header, 0, 0);

        bplib_mpool_lock_acquire(lock);
        bplib_mpool_subq_push_single(&admin->free_blocks, rblk);
    }

    bplib_mpool_lock_release(lock);

    return count;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_maintain
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_maintain(bplib_mpool_t *pool)
{
    /* the check for non-empty list can be done unlocked, as it
     * involves counter values which should be testable in an atomic fashion.
     * note this isn't final - Subq will be re-checked after locking, if this is true */
    if (bplib_mpool_subq_get_depth(&bplib_mpool_get_admin(pool)->recycle_blocks) != 0)
    {
        bplib_mpool_collect_blocks(pool, BPLIB_MPOOL_MAINTENCE_COLLECT_LIMIT);
    }
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_query_mem_current_use
 *
 *-----------------------------------------------------------------*/
size_t bplib_mpool_query_mem_current_use(bplib_mpool_t *pool)
{
    bplib_mpool_block_admin_content_t *admin;

    admin = bplib_mpool_get_admin(pool);

    return (bplib_mpool_subq_get_depth(&admin->free_blocks) * (size_t)admin->buffer_size);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_query_mem_max_use
 *
 *-----------------------------------------------------------------*/
size_t bplib_mpool_query_mem_max_use(bplib_mpool_t *pool)
{
    bplib_mpool_block_admin_content_t *admin;

    admin = bplib_mpool_get_admin(pool);

    return (admin->max_alloc_watermark * (size_t)admin->buffer_size);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_debug_print_list_stats
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_debug_print_list_stats(bplib_mpool_block_t *list, const char *label)
{
    bplib_mpool_block_t         *blk;
    bplib_mpool_block_content_t *content;
    size_t                       depth;

    blk   = list;
    depth = 0;
    while (true)
    {
        blk = bplib_mpool_get_next_block(blk);
        if (bplib_mpool_is_list_head(blk))
        {
            /* as a sanity check, this should be the same head node as where it started,
             * there should be one (and only one) head node in a list */
            assert(blk == list);
            break;
        }

        content = bplib_mpool_get_block_content(blk);
        if (content != NULL)
        {
            printf("DEBUG: %s(): block addr=%lx type=%d refcount=%u\n", __func__, (unsigned long)blk,
                   content->header.base_link->type, content->header.refcount);

            if (blk->type == bplib_mpool_blocktype_canonical)
            {
                printf("DEBUG: %s():  --> canonical block type %d\n", __func__,
                       (int)content->u.canonical.cblock.canonical_logical_data.canonical_block.blockType);
            }
            else if (blk->type == bplib_mpool_blocktype_primary)
            {
                printf("DEBUG: %s():  -->  primary dest IPN %lu\n", __func__,
                       (unsigned long)content->u.primary.pblock.data.logical.destinationEID.ssp.ipn.node_number);
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
    size_t                             i;
    bplib_mpool_block_content_t       *pchunk;
    uint32_t                           count_by_type[bplib_mpool_blocktype_max];
    uint32_t                           count_invalid;
    bplib_mpool_block_admin_content_t *admin;

    admin = bplib_mpool_get_admin(pool);

    printf("DEBUG: %s(): total blocks=%u, buffer_size=%zu, free=%u, recycled=%u\n", __func__,
           (unsigned int)admin->num_bufs_total, admin->buffer_size,
           (unsigned int)bplib_mpool_subq_get_depth(&admin->free_blocks),
           (unsigned int)bplib_mpool_subq_get_depth(&admin->recycle_blocks));

    bplib_mpool_debug_print_list_stats(&admin->free_blocks->block_list, "free_blocks");
    bplib_mpool_debug_print_list_stats(&admin->recycle_blocks->block_list, "recycle_blocks");
    bplib_mpool_debug_print_list_stats(&admin->active_list, "active_list");

    memset(count_by_type, 0, sizeof(count_by_type));
    count_invalid = 0;
    pchunk        = &pool->admin_block;

    /* Note that num_bufs_total does not include the admin block (0) */
    for (i = 0; i <= admin->num_bufs_total; ++i)
    {
        if (i == 0)
        {
            assert(pchunk->header.base_link.type == bplib_mpool_blocktype_admin);
        }
        else if (pchunk->header.base_link.type < bplib_mpool_blocktype_max)
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
    bplib_mpool_t                     *pool;
    size_t                             remain;
    bplib_mpool_block_content_t       *pchunk;
    bplib_mpool_block_admin_content_t *admin;

    /* this is just a sanity check, a pool that has only 1 block will not
     * be useful for anything, but it can at least be created */
    if (pool_mem == NULL || pool_size < sizeof(bplib_mpool_t))
    {
        /* pool memory too small */
        return NULL;
    }

    /* initialize the lock table - OK to call this multiple times,
     * subsequent calls shouldn't do anything */
    bplib_mpool_lock_init();

    /* wiping the entire memory might be overkill, but it is only done once
     * at start up, and this may also help verify that the memory "works" */
    memset(pool_mem, 0, pool_size);

    pool = pool_mem;

    bplib_mpool_link_reset(&pool->admin_block.header.base_link, bplib_mpool_blocktype_admin, 0);
    admin = bplib_mpool_get_admin(pool);

    /* the block lists are circular, as this reduces
     * complexity of operations (never a null pointer) */
    admin->buffer_size = sizeof(bplib_mpool_block_content_t);
    bplib_mpool_subq_init(&pool->admin_block.header.base_link, &admin->free_blocks);
    bplib_mpool_subq_init(&pool->admin_block.header.base_link, &admin->recycle_blocks);
    bplib_mpool_init_list_head(&pool->admin_block.header.base_link, &admin->active_list);
    bplib_rbt_init_root(&admin->blocktype_registry);

    /* start at the _next_ buffer, which is the first usable buffer (first is the admin block) */
    pchunk = &pool->admin_block + 1;
    remain = pool_size - sizeof(bplib_mpool_block_content_t);

    /* register the first API type, which is 0.
     * Notably this prevents other modules from actually registering something at 0. */
    bplib_rbt_insert_value_unique(0, &admin->blocktype_registry, &admin->blocktype_basic.rbt_link);
    bplib_rbt_insert_value_unique(MPOOL_CACHE_CBOR_DATA_SIGNATURE, &admin->blocktype_registry,
                                  &admin->blocktype_cbor.rbt_link);

    while (remain >= sizeof(bplib_mpool_block_content_t))
    {
        bplib_mpool_link_reset(&pchunk->header.base_link, bplib_mpool_blocktype_undefined, pchunk - &pool->admin_block);
        bplib_mpool_subq_push_single(&admin->free_blocks, &pchunk->header.base_link);
        remain -= sizeof(bplib_mpool_block_content_t);
        ++pchunk;
        ++admin->num_bufs_total;
    }

    /*
     * Set the bundle alloc threshold at 30% remaining (just a guess)
     * Set the internal alloc threshold at 10% remaining
     * The intent is to NOT allow the entire pool to be used by bundles,
     * there must be some buffer room for refs which transport the bundles.
     */
    admin->bblock_alloc_threshold   = (admin->num_bufs_total * 30) / 100;
    admin->internal_alloc_threshold = (admin->num_bufs_total * 10) / 100;
    fprintf(stderr, "%s(): created pool of size %zu, with %u chunks, bblock threshold = %u, internal threshold = %u\n",
            __func__, pool_size, (unsigned int)admin->num_bufs_total, (unsigned int)admin->bblock_alloc_threshold,
            (unsigned int)admin->internal_alloc_threshold);

    return pool;
}
