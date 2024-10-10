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

#include <stdio.h>  // For fprintf to stderr in BPLib_STOR_CACHE_Create.

#include <assert.h>

#include "bplib_api_types.h"

#include "bplib_mem.h"

#include "bplib_stor_qm_ducts.h"

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"
#include "bplib_stor_cache_internal.h"
#include "bplib_stor_cache_ref.h"

// TODO BPLIB_TIMEOUT should be in bplib_api_types.h.
#define BPLIB_TIMEOUT                 ((BPLib_Status_t) -2)  // TODO In bplib_api_types this is BPLIB_UNIMPLEMENTED.

/**
 * @brief Maxmimum number of blocks to be collected in a single maintenace cycle
 */
#define BPLIB_CACHE_MAINTENCE_COLLECT_LIMIT 20

#define BPLIB_CACHE_NUM_LOCKS 1 /* for now */

BPLib_MEM_Lock_t BPLIB_CACHE_LOCK_SET[BPLIB_CACHE_NUM_LOCKS];

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_LinkReset
 *
 *-----------------------------------------------------------------*/
static inline void BPLib_STOR_CACHE_LinkReset(BPLib_STOR_CACHE_Block_t *link, BPLib_STOR_CACHE_Blocktype_t type,
                                          uint32_t parent_offset)
{
    link->type          = type;
    link->parent_offset = parent_offset;
    link->next          = link;
    link->prev          = link;
}

#ifdef BPLIB_STOR_CACHE_OWN_LOCK

void BPLib_STOR_CACHE_LockInit(void)
{
    uint32_t            i;
    BPLib_STOR_CACHE_Lock_t *lock;

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

BPLib_STOR_CACHE_Lock_t *BPLib_STOR_CACHE_LockPrepare(void *resource_addr)
{
    /*
     * for now, this always uses the same lock (coarse-grained locking) but in the future
     * it might become a striped or finer-grained bucketed lock for more concurrency
     */
    return &BPLIB_CACHE_LOCK_SET[0];
}

BPLib_STOR_CACHE_Lock_t *BPLib_STOR_CACHE_LockResource(void *resource_addr)
{
    BPLib_STOR_CACHE_Lock_t *selected_lock;

    /*
     * for now, this always uses the same lock (coarse-grained locking) but in the future
     * it might become a striped or finer-grained bucketed lock for more concurrency
     */
    selected_lock = BPLib_STOR_CACHE_LockPrepare(resource_addr);
    BPLib_STOR_CACHE_LockAcquire(selected_lock);

    return selected_lock;
}

bool BPLib_STOR_CACHE_LockWait(BPLib_STOR_CACHE_Lock_t *lock, BPLib_TIME_MonotonicTime_t until_time)
{
        bool within_timeout;
    int  status;

    uint64_t BPLib_STOR_CACHE_GetDtnTime();
    uint64_t ms;
    BPLib_TIME_GetMonotonicTime(&ms);
    BPLib_TIME_GetDtnTime(&ms);
    within_timeout = (BPLIB_TIME_TO_INT(until_time) > ms);
    if (within_timeout)
    {
        status = bplib_os_wait_until_ms(lock->lock_id, BPLIB_TIME_TO_INT(until_time));
        if (status == BPLIB_SUCCESS) //BPLIB_TIMEOUT)
        {
            /* if timeout was returned, then assume that enough time has elapsed
             * such that the current time is beyond BPLIB_TIME_TO_INT(until_time) now.  note that
             * the caller should still check for whatever condition was being waited
             * on. */
            within_timeout = false;
        }
    }

    return within_timeout;
}

#endif // BPLIB_STOR_CACHE_OWN_LOCK


BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BlockFromExternalId(BPLib_STOR_CACHE_Pool_t *pool, bp_handle_t handle)
{
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;
    BPLib_STOR_CACHE_BlockContent_t      *blk;
    int                                   serial;

    admin  = BPLib_STOR_CACHE_GetAdmin(pool);
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
 * Function: BPLib_STOR_CACHE_GetBlockFromLink
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_GetBlockFromLink(BPLib_STOR_CACHE_Block_t *lblk)
{
    BPLib_STOR_CACHE_Block_t *bblk;

    bblk = lblk;

    /* Check if this is a secondary index, and if so, jump to the actual block base */
    /* this check of the type is not strictly needed, as the offset should be set to 0 for main blocks */
    while (bblk != NULL && BPLib_STOR_CACHE_IsSecondaryIndexNode(bblk) && bblk->parent_offset != 0)
    {
        /* the parent_offset field indicates this block position within the parent */
        bblk = (BPLib_STOR_CACHE_Block_t *)(void *)((uint8_t *)bblk - bblk->parent_offset);
    }

    return bblk;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_GetBlockContent
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_BlockContent_t *BPLib_STOR_CACHE_GetBlockContent(BPLib_STOR_CACHE_Block_t *cb)
{
    if (cb != NULL && BPLib_STOR_CACHE_IsAnyContentNode(cb))
    {
        return (BPLib_STOR_CACHE_BlockContent_t *)(void *)cb;
    }
    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_GetBlockContentConst
 *
 *-----------------------------------------------------------------*/
const BPLib_STOR_CACHE_BlockContent_t *BPLib_STOR_CACHE_GetBlockContentConst(const BPLib_STOR_CACHE_Block_t *cb)
{
    if (cb != NULL && BPLib_STOR_CACHE_IsAnyContentNode(cb))
    {
        return (const BPLib_STOR_CACHE_BlockContent_t *)(const void *)cb;
    }
    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BlockDereferenceContent
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_BlockContent_t *BPLib_STOR_CACHE_BlockDereferenceContent(BPLib_STOR_CACHE_Block_t *cb)
{
    BPLib_STOR_CACHE_BlockContent_t *block_ptr;

    block_ptr = BPLib_STOR_CACHE_GetBlockContent(cb);

    if (block_ptr != NULL)
    {
        /* Additionally, if this block is a ref, then also dereference it to get to the real block */
        /* In theory this could be a chain of refs, so this is a while() but in reality it should be just one */
        while (block_ptr->header.base_link.type == BPLib_STOR_CACHE_BlocktypeRef)
        {
            assert(block_ptr->u.ref.pref_target != NULL);
            block_ptr = block_ptr->u.ref.pref_target;
            /* this should have always arrived at an actual content block */
            assert(BPLib_STOR_CACHE_IsAnyContentNode(&block_ptr->header.base_link));
        }

        return block_ptr;
    }

    return NULL;
}

#define BPLIB_STOR_CACHE_GET_BUFFER_USER_START_OFFSET(m) (offsetof(BPLib_STOR_CACHE_BlockBuffer_t, m.user_data_start))

#define BPLIB_STOR_CACHE_GET_BLOCK_USER_CAPACITY(m) (sizeof(BPLib_STOR_CACHE_BlockBuffer_t) - MPOOL_GET_BUFFER_USER_START_OFFSET(m))


/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_GetUserDataOffsetByBlocktype
 *
 *-----------------------------------------------------------------*/
size_t BPLib_STOR_CACHE_GetUserDataOffsetByBlocktype(BPLib_STOR_CACHE_Blocktype_t bt)
{
    static const size_t USER_DATA_START_OFFSET[BPLib_STOR_CACHE_BlocktypeMax] = {
        [BPLib_STOR_CACHE_BlocktypeUndefined] = SIZE_MAX,
        [BPLib_STOR_CACHE_BlocktypeApi]       = BPLIB_STOR_CACHE_GET_BUFFER_USER_START_OFFSET(api),
        [BPLib_STOR_CACHE_BlocktypeGeneric]   = BPLIB_STOR_CACHE_GET_BUFFER_USER_START_OFFSET(generic_data),
        [BPLib_STOR_CACHE_BlocktypePrimary]   = BPLIB_STOR_CACHE_GET_BUFFER_USER_START_OFFSET(primary),
        [BPLib_STOR_CACHE_BlocktypeCanonical] = BPLIB_STOR_CACHE_GET_BUFFER_USER_START_OFFSET(canonical),
        [BPLib_STOR_CACHE_BlocktypeDuct]      = BPLIB_STOR_CACHE_GET_BUFFER_USER_START_OFFSET(duct),
        [BPLib_STOR_CACHE_BlocktypeRef]       = BPLIB_STOR_CACHE_GET_BUFFER_USER_START_OFFSET(ref)};

    if (bt >= BPLib_STOR_CACHE_BlocktypeMax)
    {
        return SIZE_MAX;
    }

    return USER_DATA_START_OFFSET[bt];
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_GetGenericDataCapacity
 *
 *-----------------------------------------------------------------*/
size_t BPLib_STOR_CACHE_GetGenericDataCapacity(const BPLib_STOR_CACHE_Block_t *cb)
{
    size_t data_offset;

    data_offset = BPLib_STOR_CACHE_GetUserDataOffsetByBlocktype(cb->type);
    if (data_offset > sizeof(BPLib_STOR_CACHE_BlockBuffer_t))
    {
        return 0;
    }

    return sizeof(BPLib_STOR_CACHE_BlockBuffer_t) - data_offset;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_InitSecondaryLink
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_InitSecondaryLink(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Block_t *secondary_link,
                                     BPLib_STOR_CACHE_Blocktype_t block_type)
{
    size_t offset;

    if (base_block == NULL)
    {
        offset = 0;
    }
    else
    {
        offset = (uint8_t *)secondary_link - (uint8_t *)base_block;
        assert(offset > 0 && offset < sizeof(BPLib_STOR_CACHE_BlockContent_t));
    }

    BPLib_STOR_CACHE_LinkReset(secondary_link, block_type, offset);

    assert(BPLib_STOR_CACHE_IsSecondaryIndexNode(secondary_link));
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_InitListHead
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_InitListHead(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Block_t *list_head)
{
    BPLib_STOR_CACHE_InitSecondaryLink(base_block, list_head, BPLib_STOR_CACHE_BlocktypeListHead);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_SubqInit
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_SubqInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_SubqBase_t *qblk)
{
    /* init the link structs */
    BPLib_STOR_CACHE_InitListHead(base_block, &qblk->block_list);
    qblk->pull_count = 0;
    qblk->push_count = 0;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_InsertAfter
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_InsertAfter(BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Block_t *node)
{
    /* node being inserted should always be a singleton */
    assert(BPLib_STOR_CACHE_IsLinkUnattached(node));

    node->next       = list->next;
    node->prev       = list;
    list->next       = node;
    node->next->prev = node;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_InsertBefore
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_InsertBefore(BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_Block_t *node)
{
    /* node being inserted should always be a singleton */
    assert(BPLib_STOR_CACHE_IsLinkUnattached(node));

    node->prev       = list->prev;
    node->next       = list;
    list->prev       = node;
    node->prev->next = node;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_ExtractNode
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_ExtractNode(BPLib_STOR_CACHE_Block_t *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next       = node;
    node->prev       = node;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_MergeList
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_MergeList(BPLib_STOR_CACHE_Block_t *dest, BPLib_STOR_CACHE_Block_t *src)
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
 * Function: BPLib_STOR_CACHE_GetUserContentSize
 *
 *-----------------------------------------------------------------*/
size_t BPLib_STOR_CACHE_GetUserContentSize(const BPLib_STOR_CACHE_Block_t *cb)
{
    const BPLib_STOR_CACHE_BlockContent_t *block;

    block = BPLib_STOR_CACHE_GetBlockContentConst(cb);
    if (block != NULL)
    {
        return block->header.user_content_length;
    }
    return 0;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_ReadRefcount
 *
 *-----------------------------------------------------------------*/
size_t BPLib_STOR_CACHE_ReadRefcount(const BPLib_STOR_CACHE_Block_t *cb)
{
    const BPLib_STOR_CACHE_BlockContent_t *block;

    block = BPLib_STOR_CACHE_GetBlockContentConst(cb);
    if (block != NULL)
    {
        return block->header.refcount;
    }
    return 0;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_GetParentPoolFromLink
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Pool_t *BPLib_STOR_CACHE_GetParentPoolFromLink(BPLib_STOR_CACHE_Block_t *cb)
{
    BPLib_STOR_CACHE_BlockContent_t *block;

    block = BPLib_STOR_CACHE_GetBlockContent(BPLib_STOR_CACHE_GetBlockFromLink(cb));
    if (block != NULL)
    {
        /* the "parent_offset" should provide a map back to the parent pool.
         * in this context the units are blocks, not bytes (this extends the
         * representable range for large pools, and simplifies this arithmetic) */
        block -= block->header.base_link.parent_offset;

        /* this should have always arrived at the admin block, which is the first block */
        assert(block->header.base_link.type == BPLib_STOR_CACHE_BlocktypeAdmin);
    }
    else
    {
        block = NULL;
    }

    return (BPLib_STOR_CACHE_Pool_t *)block;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_GenericDataCast
 *
 *-----------------------------------------------------------------*/
void *BPLib_STOR_CACHE_GenericDataCast(BPLib_STOR_CACHE_Block_t *cb, uint32_t required_magic)
{
    BPLib_STOR_CACHE_BlockContent_t *block;
    size_t                       data_offset;
    void                        *result;

    /* note that any block such as refs, ducts, bundle blocks, etc. may have generic data
     * associated with it, not just a generic_data block.  The difference is that the generic
     * data block _only_ has the generic data, whereas the other block types can have both. */
    result = NULL;
    block  = BPLib_STOR_CACHE_GetBlockContent(cb);
    while (block != NULL && BPLib_STOR_CACHE_IsAnyContentNode(&block->header.base_link))
    {
        if (block->header.content_type_signature == required_magic)
        {
            data_offset = BPLib_STOR_CACHE_GetUserDataOffsetByBlocktype(block->header.base_link.type);
            if (data_offset < sizeof(BPLib_STOR_CACHE_BlockBuffer_t))
            {
                result = &block->u.content_bytes[data_offset];
            }
            break;
        }

        if (!BPLib_STOR_CACHE_IsIndirectBlock(&block->header.base_link))
        {
            break;
        }

        block = block->u.ref.pref_target;
    }

    return result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_GenericDataUncast
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_GenericDataUncast(void *blk, BPLib_STOR_CACHE_Blocktype_t parent_bt,
                                                             uint32_t required_magic)
{
    BPLib_STOR_CACHE_BlockContent_t *block;
    size_t                           data_offset;

    data_offset = BPLib_STOR_CACHE_GetUserDataOffsetByBlocktype(parent_bt);
    printf("%s:%d data_offset is %ld\n", __FILE__, __LINE__, data_offset);

    if (data_offset > sizeof(BPLib_STOR_CACHE_BlockBuffer_t))
    {
        printf("%s:%d data_offset too large. Returned null.\n", __FILE__, __LINE__);
        return NULL;
    }

    data_offset += offsetof(BPLib_STOR_CACHE_BlockContent_t, u);
    block = (BPLib_STOR_CACHE_BlockContent_t *)(void *)((uint8_t *)blk - data_offset);
    if (block->header.base_link.type != parent_bt || block->header.content_type_signature != required_magic)
    {
        printf("%s:%d base_link.type or magic wrong, %d %d %d %d\n", __FILE__, __LINE__, 
            block->header.base_link.type, parent_bt, block->header.content_type_signature, required_magic);
        return NULL;
    }

    return &block->header.base_link;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_InitBaseObject
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_InitBaseObject(BPLib_STOR_CACHE_BlockHeader_t *block_hdr, uint16_t user_content_length,
                                  uint32_t content_type_signature)
{
    block_hdr->user_content_length    = user_content_length;
    block_hdr->content_type_signature = content_type_signature;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_AllocBlockInternal
 *
 * NOTE: this must be invoked with the lock already held
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_BlockContent_t *BPLib_STOR_CACHE_AllocBlockInternal(BPLib_STOR_CACHE_Pool_t *pool,
    BPLib_STOR_CACHE_Blocktype_t blocktype, uint32_t content_type_signature, void *init_arg, uint8_t priority)
{
    BPLib_STOR_CACHE_Block_t         *node;
    BPLib_STOR_CACHE_BlockContent_t *block;
    BPLib_STOR_CACHE_ModuleApiContent_t   *api_block;
    size_t                       data_offset;
    uint32_t                     alloc_threshold;
    uint32_t                     block_count;

    BPLib_STOR_CACHE_BlockAdminContent_t *admin;

    printf("%s:%d blocktype is %d\n", __FILE__, __LINE__, blocktype);

    admin = BPLib_STOR_CACHE_GetAdmin(pool);

    printf("%s:%d admin is %ld\n", __FILE__, __LINE__, (uint64_t)admin);

    /* Only real blocks are allocated here - not secondary links nor head nodes,
     * as those are embedded within the blocks themselves. */
    if (blocktype == BPLib_STOR_CACHE_BlocktypeUndefined || blocktype >= BPLib_STOR_CACHE_BlocktypeMax)
    {
        printf("%s:%d Bad blocktype. Return null.\n", __FILE__, __LINE__);
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

    block_count = BPLib_STOR_CACHE_SubqGetDepth(&admin->free_blocks);

    if (block_count <= (admin->bblock_alloc_threshold - alloc_threshold))
    {
        printf("%s:%d No free blocks. Return null.\n", __FILE__, __LINE__);
        BPLib_STOR_CACHE_PrintTrace();
        /* no free blocks available for the requested type */
        return NULL;
    }

    /* Determine how to initialize this block by looking up the content type */
    api_block = (BPLib_STOR_CACHE_ModuleApiContent_t *)(void *)BPLib_MEM_RBT_SearchUnique(content_type_signature,
                                                                                   &admin->blocktype_registry);
    if (api_block == NULL)
    {
        printf("%s:%d No constructor. Return null.\n", __FILE__, __LINE__);
        BPLib_STOR_CACHE_PrintTrace();
        /* no constructor, cannot create the block! */
        return NULL;
    }

    /* sanity check that the user content will fit in the block */
    data_offset = BPLib_STOR_CACHE_GetUserDataOffsetByBlocktype(blocktype);
    if (data_offset > sizeof(BPLib_STOR_CACHE_BlockBuffer_t) ||
        (data_offset + api_block->user_content_size) > sizeof(BPLib_STOR_CACHE_BlockBuffer_t))
    {
        printf("%s:%d User content won't fit. Return null.\n", __FILE__, __LINE__);
        BPLib_STOR_CACHE_PrintTrace();
        /* User content will not fit in the block - cannot create an instance of this type combo */
        return NULL;
    }

    /* get a block */
    node = BPLib_STOR_CACHE_SubqPullSingle(&admin->free_blocks);
    if (node == NULL)
    {
        /* this should never happen, because depth was already checked */
        return NULL;
    }

    printf("%s:%d Got block.\n", __FILE__, __LINE__);
    BPLib_STOR_CACHE_PrintTrace();

    /*
     * Convert from blocks free to blocks used, and update high watermark if necessary.
     * This is +1 to include the block that was just pulled (that is, a call to
     * BPLib_STOR_CACHE_SubqGetDepth() on the free list now will return 1 fewer than it
     * did earlier in this function).
     */
    block_count = 1 + admin->num_bufs_total - block_count;
    if (block_count > admin->max_alloc_watermark)
    {
        admin->max_alloc_watermark = block_count;
    }

    node->type = blocktype;
    block      = BPLib_STOR_CACHE_GetBlockContent(node);

    /*
     * zero fill the content part first, this ensures that this is always done,
     * and avoids the need for the module to supply a dedicated constructor just to zero it
     */
    memset(&block->u, 0, data_offset + api_block->user_content_size);

    BPLib_STOR_CACHE_InitBaseObject(&block->header, api_block->user_content_size, content_type_signature);

    switch (blocktype)
    {
        case BPLib_STOR_CACHE_BlocktypePrimary:
            BPLib_STOR_CACHE_BblockPrimaryInit(node, &block->u.primary.pblock);
            break;
        case BPLib_STOR_CACHE_BlocktypeCanonical:
            BPLib_STOR_CACHE_BblockCanonicalInit(node, &block->u.canonical.cblock);
            break;
        case BPLib_STOR_CACHE_BlocktypeDuct:
            BPLib_STOR_QM_DuctInit(node, &block->u.duct.dblock);
            break;
                    default:
            /* nothing more for this node type (this catches cbor_data)  */
            break;
    }

    /* If the module did supply a constructor, invoke it now */
    if (api_block->api.construct != NULL)
    {
        /* A constructor really should never fail nominally, if it does there is probably a bug */
        if (api_block->api.construct(init_arg, node) != BPLIB_SUCCESS)
        {
            // TODO Use bplog?
            // bplog(NULL, BP_FLAG_DIAGNOSTIC, "Constructor failed for block type %d, signature %lx\n", blocktype,
            //      (unsigned long)content_type_signature);
        }
    }

    return block;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_GenericDataAlloc
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_GenericDataAlloc(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number, void *init_arg)
{
    BPLib_STOR_CACHE_BlockContent_t *result;
    BPLib_MEM_Lock_t          *lock;

    lock   = BPLib_MEM_LockResource(pool);
    result = BPLib_STOR_CACHE_AllocBlockInternal(pool, BPLib_STOR_CACHE_BlocktypeGeneric, magic_number, init_arg,
                                                 BPLIB_MPOOL_ALLOC_PRI_MLO);
    BPLib_MEM_LockRelease(lock);

    return (BPLib_STOR_CACHE_Block_t *)result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_RecycleBlockInternal
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_RecycleBlockInternal(BPLib_STOR_CACHE_Pool_t *pool, BPLib_STOR_CACHE_Block_t *blk)
{
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;

    admin = BPLib_STOR_CACHE_GetAdmin(pool);

    BPLib_STOR_CACHE_ExtractNode(blk);
    BPLib_STOR_CACHE_SubqPushSingle(&admin->recycle_blocks, blk);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_RecycleAllBlocksInList
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_RecycleAllBlocksInList(BPLib_STOR_CACHE_Pool_t *pool, BPLib_STOR_CACHE_Block_t *list)
{
    BPLib_MEM_Lock_t                *lock;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;

    /*
     * If the pool was not specified, then attempt to deduce from the list pointer.
     *
     * List objects embedded within pool objects have breadcrumbs such that the parent pool
     * pointer can be reconstituted.  However, if the list is NOT part of a pool object
     * (e.g. a temporary list created on the stack, for instance) then this does not work.
     */
    if (pool == NULL)
    {
        pool = BPLib_STOR_CACHE_GetParentPoolFromLink(list);
    }

    admin = BPLib_STOR_CACHE_GetAdmin(pool);

    assert(BPLib_STOR_CACHE_IsListHead(list));
    lock = BPLib_MEM_LockResource(pool);
    BPLib_STOR_CACHE_SubqMergeList(&admin->recycle_blocks, list);
    BPLib_MEM_LockRelease(lock);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_RecycleBlock
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_RecycleBlock(BPLib_STOR_CACHE_Block_t *blk)
{
    BPLib_MEM_Lock_t *lock;
    BPLib_STOR_CACHE_Pool_t      *pool;

    /* only real content blocks should be recycled.  No secondary links or components/members. */
    assert(BPLib_STOR_CACHE_IsAnyContentNode(blk));

    pool = BPLib_STOR_CACHE_GetParentPoolFromLink(blk);

    lock = BPLib_MEM_LockResource(pool);
    BPLib_STOR_CACHE_RecycleBlockInternal(pool, blk);
    BPLib_MEM_LockRelease(lock);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_ListIterGotoFirst
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_ListIterGotoFirst(const BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_ListIter_t *iter)
{
    if (!BPLib_STOR_CACHE_IsListHead(list))
    {
        return BPLIB_ERROR;
    }

    iter->pending_entry = list->next;

    return BPLib_STOR_CACHE_ListIterForward(iter);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_ListIterGotoLast
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_ListIterGotoLast(const BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_ListIter_t *iter)
{
    if (!BPLib_STOR_CACHE_IsListHead(list))
    {
        return BPLIB_ERROR;
    }

    iter->pending_entry = list->prev;

    return BPLib_STOR_CACHE_ListIterReverse(iter);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_ListIterForward
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_ListIterForward(BPLib_STOR_CACHE_ListIter_t *iter)
{
    if (iter->pending_entry == NULL || BPLib_STOR_CACHE_IsListHead(iter->pending_entry))
    {
        iter->position = NULL;
        return BPLIB_ERROR;
    }

    iter->position      = iter->pending_entry;
    iter->pending_entry = iter->position->next;
    return BPLIB_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_ListIterReverse
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_ListIterReverse(BPLib_STOR_CACHE_ListIter_t *iter)
{
    if (iter->pending_entry == NULL || BPLib_STOR_CACHE_IsListHead(iter->pending_entry))
    {
        iter->position = NULL;
        return BPLIB_ERROR;
    }

    iter->position      = iter->pending_entry;
    iter->pending_entry = iter->position->prev;
    return BPLIB_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_ForeachItemInList
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_ForeachItemInList(BPLib_STOR_CACHE_Block_t *list, bool always_remove,
BPLib_STOR_CACHE_CallbackFunc_t callback_fn, void *callback_arg)
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
            BPLib_STOR_CACHE_ExtractNode(curr_node);
        }

        callback_fn(callback_arg, curr_node);
        ++count;
    }

    return count;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_SearchList
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_SearchList(
    const BPLib_STOR_CACHE_Block_t *list, BPLib_STOR_CACHE_CallbackFunc_t match_fn, void *match_arg)
{
    int                     status;
    BPLib_STOR_CACHE_ListIter_t iter;

    memset(&iter, 0, sizeof(iter));

    status = BPLib_STOR_CACHE_ListIterGotoFirst(list, &iter);
    while (status == BPLIB_SUCCESS)
    {
        /* this calls the match function with the actual content block, as that is where
         * the real information lies (this is typically a list full of secondary links) */
        if (match_fn(match_arg, BPLib_STOR_CACHE_GetBlockFromLink(iter.position)) == 0)
        {
            break;
        }
        status = BPLib_STOR_CACHE_ListIterForward(&iter);
    }

    /* the iterator sets position to NULL if end of list was reached */
    return iter.position;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_RegisterBlocktypeInternal
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_RegisterBlocktypeInternal(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number,
                                            const BPLib_STOR_CACHE_BlocktypeApi_t *api, size_t user_content_size)
{
    BPLib_STOR_CACHE_BlockContent_t       *ablk;
    BPLib_STOR_CACHE_ModuleApiContent_t         *api_block;
    int                                status;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;

    admin = BPLib_STOR_CACHE_GetAdmin(pool);

    /* before doing anything, check if this is a duplicate.  If so, ignore it.
     * This permits "lazy binding" of apis where the blocktype is registered at the time of first use */
    if (BPLib_MEM_RBT_SearchUnique(magic_number, &admin->blocktype_registry) != NULL)
    {
        return BPLIB_MEM_RBT_DUPLICATE;
    }

    ablk = BPLib_STOR_CACHE_AllocBlockInternal(pool, BPLib_STOR_CACHE_BlocktypeApi, 0, NULL, BPLIB_MPOOL_ALLOC_PRI_LO);
    if (ablk == NULL)
    {
        return BPLIB_ERROR;
    }

    api_block = &ablk->u.api;

    if (api != NULL)
    {
        api_block->api.construct = api->construct;
        api_block->api.destruct  = api->destruct;
    }
    api_block->user_content_size = user_content_size;

    status = BPLib_MEM_RBT_InsertValueUnique(magic_number, &admin->blocktype_registry, &api_block->rbt_link);

    /* due to the pre-check above this should always have been successful, but just in case, return the block if error
     */
    if (status != BPLIB_SUCCESS)
    {
        BPLib_STOR_CACHE_RecycleBlockInternal(pool, &ablk->header.base_link);
    }

    return status;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_RegisterBlocktype
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_RegisterBlocktype(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number, const BPLib_STOR_CACHE_BlocktypeApi_t *api,
                                   size_t user_content_size)
{
    BPLib_MEM_Lock_t *lock;
    int                 result;

    lock   = BPLib_MEM_LockResource(pool);
    result = BPLib_STOR_CACHE_RegisterBlocktypeInternal(pool, magic_number, api, user_content_size);
    BPLib_MEM_LockRelease(lock);
    return result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_CollectBlocks
 *
 *-----------------------------------------------------------------*/
uint32_t BPLib_STOR_CACHE_CollectBlocks(BPLib_STOR_CACHE_Pool_t *pool, uint32_t limit)
{
    BPLib_STOR_CACHE_Block_t               *rblk;
    BPLib_STOR_CACHE_ModuleApiContent_t          *api_block;
    BPLib_STOR_CACHE_BlockContent_t        *content;
    BPLib_STOR_CACHE_CallbackFunc_t         destruct;
    uint32_t                                count;
    BPLib_MEM_Lock_t                       *lock;
    BPLib_STOR_CACHE_BlockAdminContent_t   *admin;

    admin = BPLib_STOR_CACHE_GetAdmin(pool);

    count = 0;
    lock  = BPLib_MEM_LockResource(pool);
    while (count < limit)
    {
        rblk = BPLib_STOR_CACHE_SubqPullSingle(&admin->recycle_blocks);
        if (rblk == NULL)
        {
            break;
        }

        /* recycled blocks must all be "real" blocks (not secondary refs or head nodes, etc) and
         * have refcount of 0, or else bad things might happen */
        content = BPLib_STOR_CACHE_GetBlockContent(rblk);
        assert(content != NULL);
        assert(content->header.refcount == 0);

        /* figure out how to de-initialize the user content by looking up the content type */
        api_block = (BPLib_STOR_CACHE_ModuleApiContent_t *)
                        (void *)BPLib_MEM_RBT_SearchUnique(
                            content->header.content_type_signature,
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
        BPLib_MEM_LockRelease(lock);

        /* note that, like in C++, one cannot pass an arg to the destructor here.  It
         * uses the same API/function pointer type, the arg will always be NULL. */
        if (destruct != NULL)
        {
            destruct(NULL, rblk);
        }

        /* now de-initialize the base content */
        switch (rblk->type)
        {
            case BPLib_STOR_CACHE_BlocktypeCanonical:
            {
                BPLib_MEM_LockAcquire(lock);
                BPLib_STOR_CACHE_SubqMergeList(&admin->recycle_blocks, &content->u.canonical.cblock.chunk_list);
                BPLib_MEM_LockRelease(lock);
                break;
            }
            case BPLib_STOR_CACHE_BlocktypePrimary:
            {
                BPLib_MEM_LockAcquire(lock);
                BPLib_STOR_CACHE_SubqMergeList(&admin->recycle_blocks, &content->u.primary.pblock.cblock_list);
                BPLib_STOR_CACHE_SubqMergeList(&admin->recycle_blocks, &content->u.primary.pblock.chunk_list);
                BPLib_MEM_LockRelease(lock);
                break;
            }
            case BPLib_STOR_CACHE_BlocktypeDuct:
            {
                BPLib_MEM_LockAcquire(lock);
                                BPLib_STOR_CACHE_SubqMoveAll(&admin->recycle_blocks, &content->u.duct.dblock.ingress.base_subq);
                BPLib_STOR_CACHE_SubqMoveAll(&admin->recycle_blocks, &content->u.duct.dblock.egress.base_subq);
                                BPLib_MEM_LockRelease(lock);
                break;
            }
            case BPLib_STOR_CACHE_BlocktypeRef:
            {
                BPLib_STOR_CACHE_RefRelease(content->u.ref.pref_target);
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
        rblk->type = BPLib_STOR_CACHE_BlocktypeUndefined;
        BPLib_STOR_CACHE_InitBaseObject(&content->header, 0, 0);

        BPLib_MEM_LockAcquire(lock);
        BPLib_STOR_CACHE_SubqPushSingle(&admin->free_blocks, rblk);
    }

    BPLib_MEM_LockRelease(lock);

    return count;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Maintain
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_Maintain(BPLib_STOR_CACHE_Pool_t *pool)
{
    /* the check for non-empty list can be done unlocked, as it
     * involves counter values which should be testable in an atomic fashion.
     * note this isn't final - Subq will be re-checked after locking, if this is true */
        if (BPLib_STOR_CACHE_SubqGetDepth(&BPLib_STOR_CACHE_GetAdmin(pool)->recycle_blocks) != 0)
    {
        BPLib_STOR_CACHE_CollectBlocks(pool, BPLIB_CACHE_MAINTENCE_COLLECT_LIMIT);
    }
    }

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_QueryMemCurrentUse
 *
 *-----------------------------------------------------------------*/
size_t BPLib_STOR_CACHE_QueryMemCurrentUse(BPLib_STOR_CACHE_Pool_t *pool)
{
    BPLib_STOR_CACHE_BlockAdminContent_t   *admin;

    admin = BPLib_STOR_CACHE_GetAdmin(pool);

    return (BPLib_STOR_CACHE_SubqGetDepth(&admin->free_blocks) * (size_t)admin->buffer_size);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_QueryMemMaxUse
 *
 *-----------------------------------------------------------------*/
size_t BPLib_STOR_CACHE_QueryMemMaxUse(BPLib_STOR_CACHE_Pool_t *pool)
{
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;

    admin = BPLib_STOR_CACHE_GetAdmin(pool);

    return (admin->max_alloc_watermark * (size_t)admin->buffer_size);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_DebugPrintListStats
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_DebugPrintListStats(BPLib_STOR_CACHE_Block_t *list, const char *label)
{
    BPLib_STOR_CACHE_Block_t         *blk;
    BPLib_STOR_CACHE_BlockContent_t *content;
    size_t                       depth;

    blk   = list;
    depth = 0;
    while (true)
    {
        blk = BPLib_STOR_CACHE_GetNextBlock(blk);
        if (BPLib_STOR_CACHE_IsListHead(blk))
        {
            /* as a sanity check, this should be the same head node as where it started,
             * there should be one (and only one) head node in a list */
            assert(blk == list);
            break;
        }

        content = BPLib_STOR_CACHE_GetBlockContent(blk);
        if (content != NULL)
        {
            printf("DEBUG: %s(): block addr=%lx type=%d refcount=%u\n", __func__, (unsigned long)blk,
                   content->header.base_link.type, content->header.refcount);

            if (blk->type == BPLib_STOR_CACHE_BlocktypeCanonical)
            {
                printf("DEBUG: %s():  --> canonical block type %d\n", __func__,
                       (int)content->u.canonical.cblock.canonical_logical_data.canonical_block.blockType);
            }
            else if (blk->type == BPLib_STOR_CACHE_BlocktypePrimary)
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
 * Function: BPLib_STOR_CACHE_DebugScanPool
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_DebugScanPool(BPLib_STOR_CACHE_Pool_t *pool)
{
    
    size_t                             i;
    BPLib_STOR_CACHE_BlockContent_t       *pchunk;
    uint32_t                           count_by_type[BPLib_STOR_CACHE_BlocktypeMax];
    uint32_t                           count_invalid;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;

    admin = BPLib_STOR_CACHE_GetAdmin(pool);

    printf("DEBUG: %s(): total blocks=%u, buffer_size=%zu, free=%u, recycled=%u\n", __func__,
           (unsigned int)admin->num_bufs_total, admin->buffer_size,
           (unsigned int)BPLib_STOR_CACHE_SubqGetDepth(&admin->free_blocks),
           (unsigned int)BPLib_STOR_CACHE_SubqGetDepth(&admin->recycle_blocks));

    BPLib_STOR_CACHE_DebugPrintListStats(&admin->free_blocks.block_list, "free_blocks");
    BPLib_STOR_CACHE_DebugPrintListStats(&admin->recycle_blocks.block_list, "recycle_blocks");
    BPLib_STOR_CACHE_DebugPrintListStats(&admin->active_list, "active_list");

    memset(count_by_type, 0, sizeof(count_by_type));
    count_invalid = 0;
    pchunk        = &pool->admin_block;

    /* Note that num_bufs_total does not include the admin block (0) */
    for (i = 0; i <= admin->num_bufs_total; ++i)
    {
        if (i == 0)
        {
            assert(pchunk->header.base_link.type == BPLib_STOR_CACHE_BlocktypeAdmin);
        }
        else if (pchunk->header.base_link.type < BPLib_STOR_CACHE_BlocktypeMax)
        {
            ++count_by_type[pchunk->header.base_link.type];
        }
        else
        {
            ++count_invalid;
        }
        ++pchunk;
    }

    for (i = 0; i < BPLib_STOR_CACHE_BlocktypeMax; ++i)
    {
        printf("DEBUG: %s(): block type=%zu count=%lu\n", __func__, i, (unsigned long)count_by_type[i]);
    }
    printf("DEBUG: %s(): invalid count=%lu\n", __func__, (unsigned long)count_invalid);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Create
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Pool_t *BPLib_STOR_CACHE_Create(void *pool_mem, size_t pool_size)
{
    BPLib_STOR_CACHE_Pool_t              *pool;
    size_t                                remain;
    BPLib_STOR_CACHE_BlockContent_t      *pchunk;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;

    /* this is just a sanity check, a pool that has only 1 block will not
     * be useful for anything, but it can at least be created */
    if (pool_mem == NULL || pool_size < sizeof(BPLib_STOR_CACHE_Pool_t))
    {
        /* pool memory too small */
        return NULL;
    }

    /* initialize the lock table - OK to call this multiple times,
     * subsequent calls shouldn't do anything */
    // TODO Verify MEM does this. BPLib_MEM_LockInit();

    /* wiping the entire memory might be overkill, but it is only done once
     * at start up, and this may also help verify that the memory "works" */
    memset(pool_mem, 0, pool_size);

    pool = pool_mem;

    BPLib_STOR_CACHE_LinkReset(&pool->admin_block.header.base_link, BPLib_STOR_CACHE_BlocktypeAdmin, 0);
    admin = BPLib_STOR_CACHE_GetAdmin(pool);

    /* the block lists are circular, as this reduces
     * complexity of operations (never a null pointer) */
    admin->buffer_size = sizeof(BPLib_STOR_CACHE_BlockContent_t);
    BPLib_STOR_CACHE_SubqInit(&pool->admin_block.header.base_link, &admin->free_blocks);
    BPLib_STOR_CACHE_SubqInit(&pool->admin_block.header.base_link, &admin->recycle_blocks);
    BPLib_STOR_CACHE_InitListHead(&pool->admin_block.header.base_link, &admin->active_list);
    BPLib_MEM_RBT_InitRoot(&admin->blocktype_registry);

    /* start at the _next_ buffer, which is the first usable buffer (first is the admin block) */
    pchunk = &pool->admin_block + 1;
    remain = pool_size - sizeof(BPLib_STOR_CACHE_BlockContent_t);

    /* register the first API type, which is 0.
     * Notably this prevents other modules from actually registering something at 0. */
    BPLib_MEM_RBT_InsertValueUnique(0, &admin->blocktype_registry, &admin->blocktype_basic.rbt_link);
    BPLib_MEM_RBT_InsertValueUnique(BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE, &admin->blocktype_registry,
                                  &admin->blocktype_cbor.rbt_link);

    while (remain >= sizeof(BPLib_STOR_CACHE_BlockContent_t))
    {
        BPLib_STOR_CACHE_LinkReset(&pchunk->header.base_link, BPLib_STOR_CACHE_BlocktypeUndefined, pchunk - &pool->admin_block);
        BPLib_STOR_CACHE_SubqPushSingle(&admin->free_blocks, &pchunk->header.base_link);
        remain -= sizeof(BPLib_STOR_CACHE_BlockContent_t);
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

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_SubqPushSingle
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_SubqPushSingle(BPLib_STOR_CACHE_SubqBase_t *subq, BPLib_STOR_CACHE_Block_t *cpb)
{
    BPLib_STOR_CACHE_InsertBefore(&subq->block_list, cpb);
    ++subq->push_count;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_SubqPullSingle
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_SubqPullSingle(BPLib_STOR_CACHE_SubqBase_t *subq)
{
    BPLib_STOR_CACHE_Block_t *node;

    /* if the head is reached here, then the list is empty */
    node = subq->block_list.next;
    if (BPLib_STOR_CACHE_IsListHead(node))
    {
        return NULL;
    }

    BPLib_STOR_CACHE_ExtractNode(node);
    ++subq->pull_count;

    return node;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_ListCountBlocks
 *
 *-----------------------------------------------------------------*/
uint32_t BPLib_STOR_CACHE_ListCountBlocks(BPLib_STOR_CACHE_Block_t *list)
{
    BPLib_STOR_CACHE_Block_t *node;
    uint32_t           count;

    node  = list->next;
    count = 0;
    while (node != list)
    {
        ++count;
        node = node->next;
    }

    return count;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_SubqMergeList
 *
 *-----------------------------------------------------------------*/
uint32_t BPLib_STOR_CACHE_SubqMergeList(BPLib_STOR_CACHE_SubqBase_t *subq_dst, BPLib_STOR_CACHE_Block_t *list)
{
    uint32_t block_count;

    /* for record-keeping, must count the blocks to know how many are being moved here */
    block_count = BPLib_STOR_CACHE_ListCountBlocks(list);

    BPLib_STOR_CACHE_MergeList(&subq_dst->block_list, list);
    BPLib_STOR_CACHE_ExtractNode(list);
    subq_dst->push_count += block_count;

    return block_count;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_SubqMoveAll
 *
 *-----------------------------------------------------------------*/
uint32_t BPLib_STOR_CACHE_SubqMoveAll(BPLib_STOR_CACHE_SubqBase_t *subq_dst, BPLib_STOR_CACHE_SubqBase_t *subq_src)
{
    uint32_t queue_depth;

    /* appends the entire subq to the destination */
    queue_depth = BPLib_STOR_CACHE_SubqGetDepth(subq_src);
    if (queue_depth > 0)
    {
        BPLib_STOR_CACHE_MergeList(&subq_dst->block_list, &subq_src->block_list);
        BPLib_STOR_CACHE_ExtractNode(&subq_src->block_list);
        subq_src->pull_count += queue_depth;
        subq_dst->push_count += queue_depth;
    }
    return queue_depth;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_SubqDropAll
 *
 *-----------------------------------------------------------------*/
uint32_t BPLib_STOR_CACHE_SubqDropAll(BPLib_STOR_CACHE_Pool_t *pool, BPLib_STOR_CACHE_SubqBase_t *subq)
{
    uint32_t queue_depth;

    /* appends the entire subq to the destination */
    queue_depth = BPLib_STOR_CACHE_SubqGetDepth(subq);
    if (queue_depth > 0)
    {
        BPLib_STOR_CACHE_RecycleAllBlocksInList(pool, &subq->block_list);
        subq->pull_count += queue_depth;
    }
    return queue_depth;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_DuctCast
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_QM_Duct_t *BPLib_STOR_QM_DuctCast(BPLib_STOR_CACHE_Block_t *cb)
{
    BPLib_STOR_CACHE_BlockContent_t *content;

    content = BPLib_STOR_CACHE_BlockDereferenceContent(cb);
    if (content != NULL && content->header.base_link.type == BPLib_STOR_CACHE_BlocktypeDuct)
    {
        return &content->u.duct.dblock;
    }

    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_SubqWorkitemWaitForSpace
 *
 * Internal function, lock must be held when invoked
 *
 *-----------------------------------------------------------------*/
bool BPLib_STOR_CACHE_SubqWorkitemWaitForSpace(BPLib_MEM_Lock_t *lock, BPLib_STOR_CACHE_SubqWorkitem_t *subq,
                                              uint32_t quantity, BPLib_TIME_MonotonicTime_t abs_timeout)
{
    uint32_t next_depth;
    bool     within_timeout;

    /* future depth after adding given quantity */
    next_depth     = BPLib_STOR_CACHE_SubqGetDepth(&subq->base_subq) + quantity;
    within_timeout = BPLIB_TIME_IS_VALID(abs_timeout);
    while (next_depth > subq->current_depth_limit && within_timeout)
    {
        /* adding given quantity would overfill, wait for something else to pull */
        within_timeout = BPLib_MEM_LockWait(lock, abs_timeout);
        next_depth     = BPLib_STOR_CACHE_SubqGetDepth(&subq->base_subq) + quantity;
    }

    return (next_depth <= subq->current_depth_limit);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_SubqWorkitemWaitForFill
 *
 * Internal function, lock must be held when invoked
 *
 *-----------------------------------------------------------------*/
bool BPLib_STOR_CACHE_SubqWorkitemWaitForFill(BPLib_MEM_Lock_t *lock, BPLib_STOR_CACHE_SubqWorkitem_t *subq,
                                             uint32_t quantity, BPLib_TIME_MonotonicTime_t abs_timeout)
{
    uint32_t curr_depth;
    bool     within_timeout;

    curr_depth     = BPLib_STOR_CACHE_SubqGetDepth(&subq->base_subq);
    within_timeout = BPLIB_TIME_IS_VALID(abs_timeout);
    while (curr_depth < quantity && within_timeout)
    {
        within_timeout = BPLib_MEM_LockWait(lock, abs_timeout);
        curr_depth     = BPLib_STOR_CACHE_SubqGetDepth(&subq->base_subq);
    }

    return (curr_depth >= quantity);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_QM_DuctTryPush
 *
 *-----------------------------------------------------------------*/
bool BPLib_STOR_QM_DuctTryPush(BPLib_STOR_CACHE_SubqWorkitem_t *subq_dst, BPLib_STOR_CACHE_Block_t *qblk, BPLib_TIME_MonotonicTime_t abs_timeout)
{
    BPLib_MEM_Lock_t                *lock;
    bool                               got_space;
    #ifdef QM
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;
    #endif // QM
    BPLib_STOR_CACHE_Pool_t                     *pool;

    pool  = BPLib_STOR_CACHE_GetParentPoolFromLink(&subq_dst->job_header.link);
    #ifdef QM
    admin = BPLib_STOR_CACHE_GetAdmin(pool);
    #endif // QM
    lock  = BPLib_MEM_LockResource(pool);

    got_space = BPLib_STOR_CACHE_SubqWorkitemWaitForSpace(lock, subq_dst, 1, abs_timeout);
    if (got_space)
    {
        /* this does not fail, but must be done under lock to keep things consistent */
        BPLib_STOR_CACHE_SubqPushSingle(&subq_dst->base_subq, qblk);

        /* mark the duct as "active" - done directly here while the lock is still held */
        #ifdef QM
        BPLib_STOR_CACHE_JobMarkActiveInternal(&admin->active_list, &subq_dst->job_header);
        #endif // QM
        /* in case any threads were waiting on a non-empty queue */
        // TODO OSAL BPLib_MEM_LockBroadcastSignal(lock);
    }

    BPLib_MEM_LockRelease(lock);

    return got_space;
}

