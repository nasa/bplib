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
#include "BPLib_STOR_CACHE_Module_internal.h"
#include "BPLib_STOR_CACHE_Module_bblocks.h"

/**
 * @brief Maxmimum number of blocks to be collected in a single maintenace cycle
 */
#define BPLIB_CACHE_MAINTENCE_COLLECT_LIMIT 20

#define BPLIB_CACHE_NUM_LOCKS 1 /* for now */

BPLib_STOR_CACHE_Module_lock_t BPLIB_CACHE_LOCK_SET[BPLIB_CACHE_NUM_LOCKS];

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_link_reset
 *
 *-----------------------------------------------------------------*/
static inline void BPLib_STOR_CACHE_Module_link_reset(BPLib_STOR_CACHE_Block_t *link, BPLib_STOR_CACHE_Module_blocktype_t type,
                                          uint32_t parent_offset)
{
    link->type          = type;
    link->parent_offset = parent_offset;
    link->next          = link;
    link->prev          = link;
}

void BPLib_STOR_CACHE_Module_lock_init(void)
{
    uint32_t            i;
    BPLib_STOR_CACHE_Module_lock_t *lock;

    /* note - this relies on the BSS section being properly zero'ed out at start */
    for (i = 0; i < BPLIB_CACHE_NUM_LOCKS; ++i)
    {
        lock = &BPLIB_CACHE_LOCK_SET[i];
        if (!bp_handle_is_valid(lock->lock_id))
        {
            // STOR lock->lock_id = bplib_os_createlock();
        }
    }
}

BPLib_STOR_CACHE_Module_lock_t *BPLib_STOR_CACHE_Module_lock_prepare(void *resource_addr)
{
    /*
     * for now, this always uses the same lock (coarse-grained locking) but in the future
     * it might become a striped or finer-grained bucketed lock for more concurrency
     */
    return &BPLIB_CACHE_LOCK_SET[0];
}

BPLib_STOR_CACHE_Module_lock_t *BPLib_STOR_CACHE_Module_lock_resource(void *resource_addr)
{
    BPLib_STOR_CACHE_Module_lock_t *selected_lock;

    /*
     * for now, this always uses the same lock (coarse-grained locking) but in the future
     * it might become a striped or finer-grained bucketed lock for more concurrency
     */
    selected_lock = BPLib_STOR_CACHE_Module_lock_prepare(resource_addr);
    BPLib_STOR_CACHE_Module_lock_acquire(selected_lock);

    return selected_lock;
}

bool BPLib_STOR_CACHE_Module_lock_wait(BPLib_STOR_CACHE_Module_lock_t *lock, uint64_t until_dtntime)
{
    #ifdef STOR // os lock time
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
    #else // STOR
    return true;
    #endif // STOR
}

BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_Block_from_external_id(BPLib_STOR_CACHE_Module_t *pool, bp_handle_t handle)
{
    BPLib_STOR_CACHE_Block_admin_content_t *admin;
    BPLib_STOR_CACHE_Block_content_t       *blk;
    int                                serial;

    admin  = BPLib_STOR_CACHE_Module_get_admin(pool);
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

    return &blk->header.base_link;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_get_block_from_link
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_Module_get_block_from_link(BPLib_STOR_CACHE_Block_t *lblk)
{
    BPLib_STOR_CACHE_Block_t *bblk;

    bblk = lblk;

    /* Check if this is a secondary index, and if so, jump to the actual block base */
    /* this check of the type is not strictly needed, as the offset should be set to 0 for main blocks */
    while (bblk != NULL && BPLib_STOR_CACHE_Module_is_secondary_index_node(bblk) && bblk->parent_offset != 0)
    {
        /* the parent_offset field indicates this block position within the parent */
        bblk = (BPLib_STOR_CACHE_Block_t *)(void *)((uint8_t *)bblk - bblk->parent_offset);
    }

    return bblk;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_get_block_content
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_content_t *BPLib_STOR_CACHE_Module_get_block_content(BPLib_STOR_CACHE_Block_t *cb)
{
    if (cb != NULL && BPLib_STOR_CACHE_Module_is_any_content_node(cb))
    {
        return (BPLib_STOR_CACHE_Block_content_t *)(void *)cb;
    }
    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_get_block_content_const
 *
 *-----------------------------------------------------------------*/
const BPLib_STOR_CACHE_Block_content_t *BPLib_STOR_CACHE_Module_get_block_content_const(const BPLib_STOR_CACHE_Block_t *cb)
{
    if (cb != NULL && BPLib_STOR_CACHE_Module_is_any_content_node(cb))
    {
        return (const BPLib_STOR_CACHE_Block_content_t *)(const void *)cb;
    }
    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Block_dereference_content
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_content_t *BPLib_STOR_CACHE_Block_dereference_content(BPLib_STOR_CACHE_Block_t *cb)
{
    BPLib_STOR_CACHE_Block_content_t *block_ptr;

    block_ptr = BPLib_STOR_CACHE_Module_get_block_content(cb);

    if (block_ptr != NULL)
    {
        /* Additionally, if this block is a ref, then also dereference it to get to the real block */
        /* In theory this could be a chain of refs, so this is a while() but in reality it should be just one */
        while (block_ptr->header.base_link.type == BPLib_STOR_CACHE_Module_blocktype_ref)
        {
            assert(block_ptr->u.ref.pref_target != NULL);
            block_ptr = block_ptr->u.ref.pref_target;
            /* this should have always arrived at an actual content block */
            assert(BPLib_STOR_CACHE_Module_is_any_content_node(&block_ptr->header.base_link)); // TODO Is base_link correct as pointer?
        }

        return block_ptr;
    }

    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_get_user_data_offset_by_blocktype
 *
 *-----------------------------------------------------------------*/
size_t BPLib_STOR_CACHE_Module_get_user_data_offset_by_blocktype(BPLib_STOR_CACHE_Module_blocktype_t bt)
{
    static const size_t USER_DATA_START_OFFSET[BPLib_STOR_CACHE_Module_blocktype_max] = {
        [BPLib_STOR_CACHE_Module_blocktype_undefined] = SIZE_MAX,
        [BPLib_STOR_CACHE_Module_blocktype_api]       = BPLIB_MEM_GET_BUFFER_USER_START_OFFSET(api),
        [BPLib_STOR_CACHE_Module_blocktype_generic]   = BPLIB_MEM_GET_BUFFER_USER_START_OFFSET(generic_data),
        [BPLib_STOR_CACHE_Module_blocktype_primary]   = BPLIB_MEM_GET_BUFFER_USER_START_OFFSET(primary),
        [BPLib_STOR_CACHE_Module_blocktype_canonical] = BPLIB_MEM_GET_BUFFER_USER_START_OFFSET(canonical),
        // STOR [BPLib_STOR_CACHE_Module_blocktype_flow]      = BPLIB_MEM_GET_BUFFER_USER_START_OFFSET(flow),
        [BPLib_STOR_CACHE_Module_blocktype_ref]       = BPLIB_MEM_GET_BUFFER_USER_START_OFFSET(ref)};

    if (bt >= BPLib_STOR_CACHE_Module_blocktype_max)
    {
        return SIZE_MAX;
    }

    return USER_DATA_START_OFFSET[bt];
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_get_generic_data_capacity
 *
 *-----------------------------------------------------------------*/
size_t BPLib_STOR_CACHE_Module_get_generic_data_capacity(const BPLib_STOR_CACHE_Block_t *cb)
{
    size_t data_offset;

    data_offset = BPLib_STOR_CACHE_Module_get_user_data_offset_by_blocktype(cb->type);
    if (data_offset > sizeof(BPLib_STOR_CACHE_Block_buffer_t))
    {
        return 0;
    }

    return sizeof(BPLib_STOR_CACHE_Block_buffer_t) - data_offset;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_init_secondary_link
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_Module_init_secondary_link(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Block_t *secondary_link,
                                     BPLib_STOR_CACHE_Module_blocktype_t block_type)
{
    size_t offset;

    if (base_block == NULL)
    {
        offset = 0;
    }
    else
    {
        offset = (uint8_t *)secondary_link - (uint8_t *)base_block;
        assert(offset > 0 && offset < sizeof(BPLib_STOR_CACHE_Block_content_t));
    }

    BPLib_STOR_CACHE_Module_link_reset(secondary_link, block_type, offset);

    assert(BPLib_STOR_CACHE_Module_is_secondary_index_node(secondary_link));
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_init_list_head
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_Module_init_list_head(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Block_t *list_head)
{
    BPLib_STOR_CACHE_Module_init_secondary_link(base_block, list_head, BPLib_STOR_CACHE_Module_blocktype_list_head);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_insert_after
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_Module_insert_after(BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Block_t *node)
{
    /* node being inserted should always be a singleton */
    assert(BPLib_STOR_CACHE_Module_is_link_unattached(node));

    node->next       = list->next;
    node->prev       = list;
    list->next       = node;
    node->next->prev = node;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_insert_before
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_Module_insert_before(BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Block_t *node)
{
    /* node being inserted should always be a singleton */
    assert(BPLib_STOR_CACHE_Module_is_link_unattached(node));

    node->prev       = list->prev;
    node->next       = list;
    list->prev       = node;
    node->prev->next = node;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_extract_node
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_Module_extract_node(BPLib_STOR_CACHE_Block_t *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next       = node;
    node->prev       = node;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_merge_list
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_Module_merge_list(BPLib_STOR_CACHE_Block_t *dest, BPLib_STOR_CACHE_Block_t *src)
{
    BPLib_STOR_CACHE_Block_t *dlast = dest->prev; /* last node in dest list */
    BPLib_STOR_CACHE_Block_t *slast = src->prev;  /* last node in src list */

    /* nominally combine the two lists.
     * NOTE: This (temporarily) yields a list with two head nodes. */
    dlast->next = src;
    slast->next = dest;
    dest->prev  = slast;
    src->prev   = dlast;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_get_user_content_size
 *
 *-----------------------------------------------------------------*/
size_t BPLib_STOR_CACHE_Module_get_user_content_size(const BPLib_STOR_CACHE_Block_t *cb)
{
    const BPLib_STOR_CACHE_Block_content_t *block;

    block = BPLib_STOR_CACHE_Module_get_block_content_const(cb);
    if (block != NULL)
    {
        return block->header.user_content_length;
    }
    return 0;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_read_refcount
 *
 *-----------------------------------------------------------------*/
size_t BPLib_STOR_CACHE_Module_read_refcount(const BPLib_STOR_CACHE_Block_t *cb)
{
    const BPLib_STOR_CACHE_Block_content_t *block;

    block = BPLib_STOR_CACHE_Module_get_block_content_const(cb);
    if (block != NULL)
    {
        return block->header.refcount;
    }
    return 0;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_get_parent_pool_from_link
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Module_t *BPLib_STOR_CACHE_Module_get_parent_pool_from_link(BPLib_STOR_CACHE_Block_t *cb)
{
    BPLib_STOR_CACHE_Block_content_t *block;

    block = BPLib_STOR_CACHE_Module_get_block_content(BPLib_STOR_CACHE_Module_get_block_from_link(cb));
    if (block != NULL)
    {
        /* the "parent_offset" should provide a map back to the parent pool.
         * in this context the units are blocks, not bytes (this extends the
         * representable range for large pools, and simplifies this arithmetic) */
        block -= block->header.base_link.parent_offset;

        /* this should have always arrived at the admin block, which is the first block */
        assert(block->header.base_link.type == BPLib_STOR_CACHE_Module_blocktype_admin);
    }
    else
    {
        block = NULL;
    }

    return (BPLib_STOR_CACHE_Module_t *)block;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_generic_data_cast
 *
 *-----------------------------------------------------------------*/
void *BPLib_STOR_CACHE_Module_generic_data_cast(BPLib_STOR_CACHE_Block_t *cb, uint32_t required_magic)
{
    BPLib_STOR_CACHE_Block_content_t *block;
    size_t                       data_offset;
    void                        *result;

    /* note that any block such as refs, flows, bundle blocks, etc. may have generic data
     * associated with it, not just a generic_data block.  The difference is that the generic
     * data block _only_ has the generic data, whereas the other block types can have both. */
    result = NULL;
    block  = BPLib_STOR_CACHE_Module_get_block_content(cb);
    while (block != NULL && BPLib_STOR_CACHE_Module_is_any_content_node(&block->header.base_link))
    {
        if (block->header.content_type_signature == required_magic)
        {
            data_offset = BPLib_STOR_CACHE_Module_get_user_data_offset_by_blocktype(block->header.base_link.type);
            if (data_offset < sizeof(BPLib_STOR_CACHE_Block_buffer_t))
            {
                result = &block->u.content_bytes[data_offset];
            }
            break;
        }

        if (!BPLib_STOR_CACHE_Module_is_indirect_block(&block->header.base_link))
        {
            break;
        }

        block = block->u.ref.pref_target;
    }

    return result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_generic_data_uncast
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_Module_generic_data_uncast(void *blk, BPLib_STOR_CACHE_Module_blocktype_t parent_bt,
                                                     uint32_t required_magic)
{
    BPLib_STOR_CACHE_Block_content_t *block;
    size_t                       data_offset;

    data_offset = BPLib_STOR_CACHE_Module_get_user_data_offset_by_blocktype(parent_bt);
    if (data_offset > sizeof(BPLib_STOR_CACHE_Block_buffer_t))
    {
        return NULL;
    }

    data_offset += offsetof(BPLib_STOR_CACHE_Block_content_t, u);
    block = (BPLib_STOR_CACHE_Block_content_t *)(void *)((uint8_t *)blk - data_offset);
    if (block->header.base_link.type != parent_bt || block->header.content_type_signature != required_magic)
    {
        return NULL;
    }

    return &block->header.base_link;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_init_base_object
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_Module_init_base_object(BPLib_STOR_CACHE_Block_header_t *block_hdr, uint16_t user_content_length,
                                  uint32_t content_type_signature)
{
    block_hdr->user_content_length    = user_content_length;
    block_hdr->content_type_signature = content_type_signature;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_alloc_block_internal
 *
 * NOTE: this must be invoked with the lock already held
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_content_t *BPLib_STOR_CACHE_Module_alloc_block_internal(BPLib_STOR_CACHE_Module_t *pool, BPLib_STOR_CACHE_Module_blocktype_t blocktype,
                                                              uint32_t content_type_signature, void *init_arg,
                                                              uint8_t priority)
{
    BPLib_STOR_CACHE_Block_t         *node;
    BPLib_STOR_CACHE_Block_content_t *block;
    BPLib_STOR_CACHE_Module_api_content_t   *api_block;
    size_t                       data_offset;
    // STOR uint32_t                     alloc_threshold;
    uint32_t                     block_count;

    BPLib_STOR_CACHE_Block_admin_content_t *admin;

    admin = BPLib_STOR_CACHE_Module_get_admin(pool);

    /* Only real blocks are allocated here - not secondary links nor head nodes,
     * as those are embedded within the blocks themselves. */
    if (blocktype == BPLib_STOR_CACHE_Module_blocktype_undefined || blocktype >= BPLib_STOR_CACHE_Module_blocktype_max)
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
    // STOR alloc_threshold = (admin->bblock_alloc_threshold * priority) / 255;

    #ifdef STOR // subq
    block_count = BPLib_STOR_CACHE_Module_subq_get_depth(admin->free_blocks);
    if (block_count <= (admin->bblock_alloc_threshold - alloc_threshold))
    {
        /* no free blocks available for the requested type */
        return NULL;
    }
    #else // STOR
    block_count = 1024;
    #endif // STOR

    /* figure out how to initialize this block by looking up the content type */
    api_block = (BPLib_STOR_CACHE_Module_api_content_t *)(void *)BPLib_STOR_CACHE_RBT_SearchUnique(content_type_signature,
                                                                             &admin->blocktype_registry);
    if (api_block == NULL)
    {
        /* no constructor, cannot create the block! */
        return NULL;
    }

    /* sanity check that the user content will fit in the block */
    data_offset = BPLib_STOR_CACHE_Module_get_user_data_offset_by_blocktype(blocktype);
    if (data_offset > sizeof(BPLib_STOR_CACHE_Block_buffer_t) ||
        (data_offset + api_block->user_content_size) > sizeof(BPLib_STOR_CACHE_Block_buffer_t))
    {
        /* User content will not fit in the block - cannot create an instance of this type combo */
        return NULL;
    }

    /* get a block */
    node = BPLib_STOR_CACHE_Module_subq_pull_single(admin->free_blocks);
    if (node == NULL)
    {
        /* this should never happen, because depth was already checked */
        return NULL;
    }

    /*
     * Convert from blocks free to blocks used, and update high watermark if necessary.
     * This is +1 to include the block that was just pulled (that is, a call to
     * BPLib_STOR_CACHE_Module_subq_get_depth() on the free list now will return 1 fewer than it
     * did earlier in this function).
     */
    block_count = 1 + admin->num_bufs_total - block_count;
    if (block_count > admin->max_alloc_watermark)
    {
        admin->max_alloc_watermark = block_count;
    }

    node->type = blocktype;
    block      = BPLib_STOR_CACHE_Module_get_block_content(node);

    /*
     * zero fill the content part first, this ensures that this is always done,
     * and avoids the need for the module to supply a dedicated constructor just to zero it
     */
    memset(&block->u, 0, data_offset + api_block->user_content_size);

    BPLib_STOR_CACHE_Module_init_base_object(&block->header, api_block->user_content_size, content_type_signature);

    switch (blocktype)
    {
        case BPLib_STOR_CACHE_Module_blocktype_primary:
            BPLib_STOR_CACHE_Module_bblock_primary_init(node, &block->u.primary.pblock);
            break;
        case BPLib_STOR_CACHE_Module_blocktype_canonical:
            BPLib_STOR_CACHE_Module_bblock_canonical_init(node, &block->u.canonical.cblock);
            break;
        case BPLib_STOR_CACHE_Module_blocktype_flow:
            #ifdef STOR // TODO Change flow to duct
            BPLib_STOR_CACHE_Module_flow_init(node, block->u.flow.fblock);
            break;
            #endif // STOR
        default:
            /* nothing more for this node type (this catches cbor_data)  */
            break;
    }

    /* If the module did supply a constructor, invoke it now */
    if (api_block->api.construct != NULL)
    {
        /* A constructor really should never fail nominally, if it does there is probably a bug */
        if (api_block->api.construct(init_arg, node) != BP_SUCCESS)
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
 * Function: BPLib_STOR_CACHE_Module_generic_data_alloc
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_Module_generic_data_alloc(BPLib_STOR_CACHE_Module_t *pool, uint32_t magic_number, void *init_arg)
{
    BPLib_STOR_CACHE_Block_content_t *result;
    BPLib_STOR_CACHE_Module_lock_t          *lock;

    lock   = BPLib_STOR_CACHE_Module_lock_resource(pool);
    result = BPLib_STOR_CACHE_Module_alloc_block_internal(pool, BPLib_STOR_CACHE_Module_blocktype_generic, magic_number, init_arg,
                                              BPLIB_CACHE_ALLOC_PRI_MLO);
    BPLib_STOR_CACHE_Module_lock_release(lock);

    return (BPLib_STOR_CACHE_Block_t *)result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_recycle_block_internal
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_Module_recycle_block_internal(BPLib_STOR_CACHE_Module_t *pool, BPLib_STOR_CACHE_Block_t *blk)
{
    BPLib_STOR_CACHE_Block_admin_content_t *admin;

    admin = BPLib_STOR_CACHE_Module_get_admin(pool);

    BPLib_STOR_CACHE_Module_extract_node(blk);
    BPLib_STOR_CACHE_Module_subq_push_single(admin->recycle_blocks, blk);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_recycle_all_blocks_in_list
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_Module_recycle_all_blocks_in_list(BPLib_STOR_CACHE_Module_t *pool, BPLib_STOR_CACHE_Block_t *list)
{
    BPLib_STOR_CACHE_Module_lock_t                *lock;
    BPLib_STOR_CACHE_Block_admin_content_t *admin;

    /*
     * If the pool was not specified, then attempt to deduce from the list pointer.
     *
     * List objects embedded within pool objects have breadcrumbs such that the parent pool
     * pointer can be reconstituted.  However, if the list is NOT part of a pool object
     * (e.g. a temporary list created on the stack, for instance) then this does not work.
     */
    if (pool == NULL)
    {
        pool = BPLib_STOR_CACHE_Module_get_parent_pool_from_link(list);
    }

    admin = BPLib_STOR_CACHE_Module_get_admin(pool);

    assert(BPLib_STOR_CACHE_Module_is_list_head(list));
    lock = BPLib_STOR_CACHE_Module_lock_resource(pool);
    BPLib_STOR_CACHE_Module_subq_merge_list(admin->recycle_blocks, list);
    BPLib_STOR_CACHE_Module_lock_release(lock);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_recycle_block
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_Module_recycle_block(BPLib_STOR_CACHE_Block_t *blk)
{
    BPLib_STOR_CACHE_Module_lock_t *lock;
    BPLib_STOR_CACHE_Module_t      *pool;

    /* only real content blocks should be recycled.  No secondary links or components/members. */
    assert(BPLib_STOR_CACHE_Module_is_any_content_node(blk));

    pool = BPLib_STOR_CACHE_Module_get_parent_pool_from_link(blk);

    lock = BPLib_STOR_CACHE_Module_lock_resource(pool);
    BPLib_STOR_CACHE_Module_recycle_block_internal(pool, blk);
    BPLib_STOR_CACHE_Module_lock_release(lock);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_list_iter_goto_first
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_Module_list_iter_goto_first(const BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Module_list_iter_t *iter)
{
    if (!BPLib_STOR_CACHE_Module_is_list_head(list))
    {
        return BP_ERROR;
    }

    iter->pending_entry = list->next;

    return BPLib_STOR_CACHE_Module_list_iter_forward(iter);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_list_iter_goto_last
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_Module_list_iter_goto_last(const BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Module_list_iter_t *iter)
{
    if (!BPLib_STOR_CACHE_Module_is_list_head(list))
    {
        return BP_ERROR;
    }

    iter->pending_entry = list->prev;

    return BPLib_STOR_CACHE_Module_list_iter_reverse(iter);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_list_iter_forward
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_Module_list_iter_forward(BPLib_STOR_CACHE_Module_list_iter_t *iter)
{
    if (iter->pending_entry == NULL || BPLib_STOR_CACHE_Module_is_list_head(iter->pending_entry))
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
 * Function: BPLib_STOR_CACHE_Module_list_iter_reverse
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_Module_list_iter_reverse(BPLib_STOR_CACHE_Module_list_iter_t *iter)
{
    if (iter->pending_entry == NULL || BPLib_STOR_CACHE_Module_is_list_head(iter->pending_entry))
    {
        iter->position = NULL;
        return BP_ERROR;
    }

    iter->position      = iter->pending_entry;
    iter->pending_entry = iter->position->prev;
    return BPLIB_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_foreach_item_in_list
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_Module_foreach_item_in_list(BPLib_STOR_CACHE_Block_t *list, bool always_remove,
                                     BPLib_STOR_CACHE_Module_callback_func_t callback_fn, void *callback_arg)
{
    BPLib_STOR_CACHE_Block_t *curr_node;
    BPLib_STOR_CACHE_Block_t *next_node;
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
            BPLib_STOR_CACHE_Module_extract_node(curr_node);
        }

        callback_fn(callback_arg, curr_node);
        ++count;
    }

    return count;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_search_list
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_Module_search_list(const BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Module_callback_func_t match_fn,
                                             void *match_arg)
{
    int                     status;
    BPLib_STOR_CACHE_Module_list_iter_t iter;

    memset(&iter, 0, sizeof(iter));

    status = BPLib_STOR_CACHE_Module_list_iter_goto_first(list, &iter);
    while (status == BP_SUCCESS)
    {
        /* this calls the match function with the actual content block, as that is where
         * the real information lies (this is typically a list full of secondary links) */
        if (match_fn(match_arg, BPLib_STOR_CACHE_Module_get_block_from_link(iter.position)) == 0)
        {
            break;
        }
        status = BPLib_STOR_CACHE_Module_list_iter_forward(&iter);
    }

    /* the iterator sets position to NULL if end of list was reached */
    return iter.position;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_register_blocktype_internal
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_Module_register_blocktype_internal(BPLib_STOR_CACHE_Module_t *pool, uint32_t magic_number,
                                            const BPLib_STOR_CACHE_Module_blocktype_api_t *api, size_t user_content_size)
{
    BPLib_STOR_CACHE_Block_content_t       *ablk;
    BPLib_STOR_CACHE_Module_api_content_t         *api_block;
    int                                status;
    BPLib_STOR_CACHE_Block_admin_content_t *admin;

    admin = BPLib_STOR_CACHE_Module_get_admin(pool);

    /* before doing anything, check if this is a duplicate.  If so, ignore it.
     * This permits "lazy binding" of apis where the blocktype is registered at the time of first use */
    if (BPLib_STOR_CACHE_RBT_SearchUnique(magic_number, &admin->blocktype_registry) != NULL)
    {
        return BP_DUPLICATE;
    }

    ablk = BPLib_STOR_CACHE_Module_alloc_block_internal(pool, BPLib_STOR_CACHE_Module_blocktype_api, 0, NULL, BPLIB_CACHE_ALLOC_PRI_LO);
    if (ablk == NULL)
    {
        return BP_ERROR;
    }

    api_block = &ablk->u.api;

    if (api != NULL)
    {
        api_block->api.construct = api->construct;
        api_block->api.destruct  = api->destruct;
    }
    api_block->user_content_size = user_content_size;

    status = BPLib_STOR_CACHE_RBT_InsertValueUnique(magic_number, &admin->blocktype_registry, &api_block->rbt_link);

    /* due to the pre-check above this should always have been successful, but just in case, return the block if error
     */
    if (status != BP_SUCCESS)
    {
        BPLib_STOR_CACHE_Module_recycle_block_internal(pool, &ablk->header.base_link);
    }

    return status;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_register_blocktype
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_Module_register_blocktype(BPLib_STOR_CACHE_Module_t *pool, uint32_t magic_number, const BPLib_STOR_CACHE_Module_blocktype_api_t *api,
                                   size_t user_content_size)
{
    BPLib_STOR_CACHE_Module_lock_t *lock;
    int                 result;

    lock   = BPLib_STOR_CACHE_Module_lock_resource(pool);
    result = BPLib_STOR_CACHE_Module_register_blocktype_internal(pool, magic_number, api, user_content_size);
    BPLib_STOR_CACHE_Module_lock_release(lock);
    return result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_collect_blocks
 *
 *-----------------------------------------------------------------*/
uint32_t BPLib_STOR_CACHE_Module_collect_blocks(BPLib_STOR_CACHE_Module_t *pool, uint32_t limit)
{
    BPLib_STOR_CACHE_Block_t               *rblk;
    BPLib_STOR_CACHE_Module_api_content_t         *api_block;
    BPLib_STOR_CACHE_Block_content_t       *content;
    BPLib_STOR_CACHE_Module_callback_func_t        destruct;
    uint32_t                           count;
    BPLib_STOR_CACHE_Module_lock_t                *lock;
    BPLib_STOR_CACHE_Block_admin_content_t *admin;

    admin = BPLib_STOR_CACHE_Module_get_admin(pool);

    count = 0;
    lock  = BPLib_STOR_CACHE_Module_lock_resource(pool);
    while (count < limit)
    {
        rblk = BPLib_STOR_CACHE_Module_subq_pull_single(admin->recycle_blocks);
        if (rblk == NULL)
        {
            break;
        }

        /* recycled blocks must all be "real" blocks (not secondary refs or head nodes, etc) and
         * have refcount of 0, or else bad things might happen */
        content = BPLib_STOR_CACHE_Module_get_block_content(rblk);
        assert(content != NULL);
        assert(content->header.refcount == 0);

        /* figure out how to de-initialize the user content by looking up the content type */
        api_block = (BPLib_STOR_CACHE_Module_api_content_t *)(void *)BPLib_STOR_CACHE_RBT_SearchUnique(content->header.content_type_signature,
                                                                                 &admin->blocktype_registry);

        if (api_block != NULL)
        {
            destruct = api_block->api.destruct;
        }
        else
        {
            destruct = NULL;
        }

        /* pool should be UN-locked when invoking destructor */
        BPLib_STOR_CACHE_Module_lock_release(lock);

        /* note that, like in C++, one cannot pass an arg to the destructor here.  It
         * uses the same API/function pointer type, the arg will always be NULL. */
        if (destruct != NULL)
        {
            destruct(NULL, rblk);
        }

        /* now de-initialize the base content */
        switch (rblk->type)
        {
            case BPLib_STOR_CACHE_Module_blocktype_canonical:
            {
                BPLib_STOR_CACHE_Module_lock_acquire(lock);
                BPLib_STOR_CACHE_Module_subq_merge_list(admin->recycle_blocks, &content->u.canonical.cblock.chunk_list);
                BPLib_STOR_CACHE_Module_lock_release(lock);
                break;
            }
            case BPLib_STOR_CACHE_Module_blocktype_primary:
            {
                BPLib_STOR_CACHE_Module_lock_acquire(lock);
                BPLib_STOR_CACHE_Module_subq_merge_list(admin->recycle_blocks, &content->u.primary.pblock.cblock_list);
                BPLib_STOR_CACHE_Module_subq_merge_list(admin->recycle_blocks, &content->u.primary.pblock.chunk_list);
                BPLib_STOR_CACHE_Module_lock_release(lock);
                break;
            }
            case BPLib_STOR_CACHE_Module_blocktype_flow:
            {
                BPLib_STOR_CACHE_Module_lock_acquire(lock);
                #ifdef STOR // duct
                BPLib_STOR_CACHE_Module_subq_move_all(&admin->recycle_blocks, &content->u.flow.fblock.ingress.base_subq);
                BPLib_STOR_CACHE_Module_subq_move_all(&admin->recycle_blocks, &content->u.flow.fblock.egress.base_subq);
                #endif // STOR
                BPLib_STOR_CACHE_Module_lock_release(lock);
                break;
            }
            case BPLib_STOR_CACHE_Module_blocktype_ref:
            {
                BPLib_STOR_CACHE_Module_ref_release(content->u.ref.pref_target);
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
        rblk->type = BPLib_STOR_CACHE_Module_blocktype_undefined;
        BPLib_STOR_CACHE_Module_init_base_object(&content->header, 0, 0);

        BPLib_STOR_CACHE_Module_lock_acquire(lock);
        BPLib_STOR_CACHE_Module_subq_push_single(admin->free_blocks, rblk);
    }

    BPLib_STOR_CACHE_Module_lock_release(lock);

    return count;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_maintain
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_Module_maintain(BPLib_STOR_CACHE_Module_t *pool)
{
    /* the check for non-empty list can be done unlocked, as it
     * involves counter values which should be testable in an atomic fashion.
     * note this isn't final - Subq will be re-checked after locking, if this is true */
    #ifdef STOR // subq
    if (BPLib_STOR_CACHE_Module_subq_get_depth(BPLib_STOR_CACHE_Module_get_admin(pool)->recycle_blocks) != 0)
    {
        BPLib_STOR_CACHE_Module_collect_blocks(pool, BPLIB_CACHE_MAINTENCE_COLLECT_LIMIT);
    }
    #endif // STOR
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_query_mem_current_use
 *
 *-----------------------------------------------------------------*/
size_t BPLib_STOR_CACHE_Module_query_mem_current_use(BPLib_STOR_CACHE_Module_t *pool)
{
    #ifdef STOR // subq and should be fixed in MEM.     BPLib_STOR_CACHE_Block_admin_content_t *admin;

    admin = BPLib_STOR_CACHE_Module_get_admin(pool);

    return (BPLib_STOR_CACHE_Module_subq_get_depth(admin->free_blocks) * (size_t)admin->buffer_size);
    #endif // STOR

    return 32767;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_query_mem_max_use
 *
 *-----------------------------------------------------------------*/
size_t BPLib_STOR_CACHE_Module_query_mem_max_use(BPLib_STOR_CACHE_Module_t *pool)
{
    BPLib_STOR_CACHE_Block_admin_content_t *admin;

    admin = BPLib_STOR_CACHE_Module_get_admin(pool);

    return (admin->max_alloc_watermark * (size_t)admin->buffer_size);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_debug_print_list_stats
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_Module_debug_print_list_stats(BPLib_STOR_CACHE_Block_t *list, const char *label)
{
    BPLib_STOR_CACHE_Block_t         *blk;
    BPLib_STOR_CACHE_Block_content_t *content;
    size_t                       depth;

    blk   = list;
    depth = 0;
    while (true)
    {
        blk = BPLib_STOR_CACHE_Module_get_next_block(blk);
        if (BPLib_STOR_CACHE_Module_is_list_head(blk))
        {
            /* as a sanity check, this should be the same head node as where it started,
             * there should be one (and only one) head node in a list */
            assert(blk == list);
            break;
        }

        content = BPLib_STOR_CACHE_Module_get_block_content(blk);
        if (content != NULL)
        {
            printf("DEBUG: %s(): block addr=%lx type=%d refcount=%u\n", __func__, (unsigned long)blk,
                   content->header.base_link.type, content->header.refcount);

            if (blk->type == BPLib_STOR_CACHE_Module_blocktype_canonical)
            {
                printf("DEBUG: %s():  --> canonical block type %d\n", __func__,
                       (int)content->u.canonical.cblock.canonical_logical_data.canonical_block.blockType);
            }
            else if (blk->type == BPLib_STOR_CACHE_Module_blocktype_primary)
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
 * Function: BPLib_STOR_CACHE_Module_debug_scan
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_Module_debug_scan(BPLib_STOR_CACHE_Module_t *pool)
{
    #ifdef STOR // mem debug

    size_t                             i;
    BPLib_STOR_CACHE_Block_content_t       *pchunk;
    uint32_t                           count_by_type[BPLib_STOR_CACHE_Module_blocktype_max];
    uint32_t                           count_invalid;
    BPLib_STOR_CACHE_Block_admin_content_t *admin;

    admin = BPLib_STOR_CACHE_Module_get_admin(pool);

    printf("DEBUG: %s(): total blocks=%u, buffer_size=%zu, free=%u, recycled=%u\n", __func__,
           (unsigned int)admin->num_bufs_total, admin->buffer_size,
           (unsigned int)BPLib_STOR_CACHE_Module_subq_get_depth(admin->free_blocks),
           (unsigned int)BPLib_STOR_CACHE_Module_subq_get_depth(admin->recycle_blocks));

    BPLib_STOR_CACHE_Module_debug_print_list_stats(&admin->free_blocks->block_list, "free_blocks");
    BPLib_STOR_CACHE_Module_debug_print_list_stats(&admin->recycle_blocks->block_list, "recycle_blocks");
    BPLib_STOR_CACHE_Module_debug_print_list_stats(admin->active_list, "active_list");

    memset(count_by_type, 0, sizeof(count_by_type));
    count_invalid = 0;
    pchunk        = &pool->admin_block;

    /* Note that num_bufs_total does not include the admin block (0) */
    for (i = 0; i <= admin->num_bufs_total; ++i)
    {
        if (i == 0)
        {
            assert(pchunk->header.base_link.type == BPLib_STOR_CACHE_Module_blocktype_admin);
        }
        else if (pchunk->header.base_link.type < BPLib_STOR_CACHE_Module_blocktype_max)
        {
            ++count_by_type[pchunk->header.base_link.type];
        }
        else
        {
            ++count_invalid;
        }
        ++pchunk;
    }

    for (i = 0; i < BPLib_STOR_CACHE_Module_blocktype_max; ++i)
    {
        printf("DEBUG: %s(): block type=%zu count=%lu\n", __func__, i, (unsigned long)count_by_type[i]);
    }
    printf("DEBUG: %s(): invalid count=%lu\n", __func__, (unsigned long)count_invalid);
    #else // STOR
    printf("DEBUG: %s(): BPLib_STOR_CACHE_Module_debug_scan not implemented.", __func__);
    #endif
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Module_create
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Module_t *BPLib_STOR_CACHE_Module_create(void *pool_mem, size_t pool_size)
{
    BPLib_STOR_CACHE_Module_t                     *pool;
    size_t                             remain;
    BPLib_STOR_CACHE_Block_content_t       *pchunk;
    BPLib_STOR_CACHE_Block_admin_content_t *admin;

    /* this is just a sanity check, a pool that has only 1 block will not
     * be useful for anything, but it can at least be created */
    if (pool_mem == NULL || pool_size < sizeof(BPLib_STOR_CACHE_Module_t))
    {
        /* pool memory too small */
        return NULL;
    }

    /* initialize the lock table - OK to call this multiple times,
     * subsequent calls shouldn't do anything */
    BPLib_STOR_CACHE_Module_lock_init();

    /* wiping the entire memory might be overkill, but it is only done once
     * at start up, and this may also help verify that the memory "works" */
    memset(pool_mem, 0, pool_size);

    pool = pool_mem;

    BPLib_STOR_CACHE_Module_link_reset(&pool->admin_block.header.base_link, BPLib_STOR_CACHE_Module_blocktype_admin, 0);
    admin = BPLib_STOR_CACHE_Module_get_admin(pool);

    /* the block lists are circular, as this reduces
     * complexity of operations (never a null pointer) */
    admin->buffer_size = sizeof(BPLib_STOR_CACHE_Block_content_t);
    BPLib_STOR_CACHE_Module_subq_init(&pool->admin_block.header.base_link, admin->free_blocks);
    BPLib_STOR_CACHE_Module_subq_init(&pool->admin_block.header.base_link, admin->recycle_blocks);
    BPLib_STOR_CACHE_Module_init_list_head(&pool->admin_block.header.base_link, admin->active_list);
    BPLib_STOR_CACHE_RBT_InitRoot(&admin->blocktype_registry);

    /* start at the _next_ buffer, which is the first usable buffer (first is the admin block) */
    pchunk = &pool->admin_block + 1;
    remain = pool_size - sizeof(BPLib_STOR_CACHE_Block_content_t);

    /* register the first API type, which is 0.
     * Notably this prevents other modules from actually registering something at 0. */
    BPLib_STOR_CACHE_RBT_InsertValueUnique(0, &admin->blocktype_registry, &admin->blocktype_basic.rbt_link);
    BPLib_STOR_CACHE_RBT_InsertValueUnique(BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE, &admin->blocktype_registry,
                                  &admin->blocktype_cbor.rbt_link);

    while (remain >= sizeof(BPLib_STOR_CACHE_Block_content_t))
    {
        BPLib_STOR_CACHE_Module_link_reset(&pchunk->header.base_link, BPLib_STOR_CACHE_Module_blocktype_undefined, pchunk - &pool->admin_block);
        BPLib_STOR_CACHE_Module_subq_push_single(admin->free_blocks, &pchunk->header.base_link);
        remain -= sizeof(BPLib_STOR_CACHE_Block_content_t);
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
