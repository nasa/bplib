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
// TODO Switch to osal bplib/os? OS_ #include "bplib_os.h"
#include "bplib_api_types.h"
#include "bplib_mem.h"
#include "bplib_mem_internal.h"

#include "osapi.h"

/**
 * @brief Maxmimum number of blocks to be collected in a single maintenace cycle
 */
#define BPLIB_MEM_MAINTENCE_COLLECT_LIMIT 20

#define BPLIB_MEM_NUM_LOCKS 1 /* for now */

BPLib_MEM_Lock_t BPLIB_MEM_LOCK_SET[BPLIB_MEM_NUM_LOCKS];

int BPLib_MEM_Init(void)
{
    // TODO Add memory pool base link pointer to MEM globals.
    BPLib_MEM_OS_Init();

    return BPLIB_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_LinkReset
 *
 *-----------------------------------------------------------------*/
static inline void BPLib_MEM_LinkReset(BPLib_MEM_Block_t *link, BPLib_MEM_Blocktype_t type,
                                       uint32_t parent_offset)
{
    link->type          = type;
    link->parent_offset = parent_offset;
    link->next          = link;
    link->prev          = link;
}

void BPLib_MEM_LockInit(void)
{
    uint32_t            i;
    BPLib_MEM_Lock_t *lock;

    /* note - this relies on the BSS section being properly zero'ed out at start */
    for (i = 0; i < BPLIB_MEM_NUM_LOCKS; ++i)
    {
        lock = &BPLIB_MEM_LOCK_SET[i];
        if (!bp_handle_is_valid(lock->lock_id))
        {
            lock->lock_id = BPLib_MEM_OS_CreateLock();
        }
    }
}

BPLib_MEM_Lock_t *BPLib_MEM_LockPrepare(void *resource_addr)
{
    /*
     * for now, this always uses the same lock (coarse-grained locking) but in the future
     * it might become a striped or finer-grained bucketed lock for more concurrency
     */
    return &BPLIB_MEM_LOCK_SET[0];
}

BPLib_MEM_Lock_t *BPLib_MEM_LockResource(void *resource_addr)
{
    BPLib_MEM_Lock_t *selected_lock;

    /*
     * for now, this always uses the same lock (coarse-grained locking) but in the future
     * it might become a striped or finer-grained bucketed lock for more concurrency
     */
    selected_lock = BPLib_MEM_LockPrepare(resource_addr);
    BPLib_MEM_LockAcquire(selected_lock);

    return selected_lock;
}

bool BPLib_MEM_LockWait(BPLib_MEM_Lock_t *lock, uint64_t until_dtntime)
{
    bool within_timeout;
    int  status;

    within_timeout = (until_dtntime > BPLib_MEM_OS_GetDtnTimeMs());
    if (within_timeout)
    {
        status = BPLib_MEM_OS_WaitUntilMs(lock->lock_id, until_dtntime);
        if (status == BPLIB_MEM_TIMEOUT)
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

BPLib_MEM_Block_t *BPLib_MEM_BlockFromExternalId(BPLib_MEM_Pool_t *pool, bp_handle_t handle)
{
    BPLib_MEM_BlockAdminContent_t *admin;
    BPLib_MEM_BlockContent_t      *blk;
    int                            serial;

    admin  = BPLib_MEM_GetAdmin(pool);
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
 * Function: BPLib_MEM_GetBlockFromLink
 *
 *-----------------------------------------------------------------*/
BPLib_MEM_Block_t *BPLib_MEM_GetBlockFromLink(BPLib_MEM_Block_t *lblk)
{
    BPLib_MEM_Block_t *bblk;

    bblk = lblk;

    /* Check if this is a secondary index, and if so, jump to the actual block base */
    /* this check of the type is not strictly needed, as the offset should be set to 0 for main blocks */
    while (bblk != NULL && BPLib_MEM_IsSecondaryIndexNode(bblk) && bblk->parent_offset != 0)
    {
        /* the parent_offset field indicates this block position within the parent */
        bblk = (BPLib_MEM_Block_t *)(void *)((uint8_t *)bblk - bblk->parent_offset);
    }

    return bblk;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_GetBlockContent
 *
 *-----------------------------------------------------------------*/
BPLib_MEM_BlockContent_t *BPLib_MEM_GetBlockContent(BPLib_MEM_Block_t *cb)
{
    if (cb != NULL && BPLib_MEM_IsAnyContentNode(cb))
    {
        return (BPLib_MEM_BlockContent_t *)(void *)cb;
    }
    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_GetBlockContentConst
 *
 *-----------------------------------------------------------------*/
const BPLib_MEM_BlockContent_t *BPLib_MEM_GetBlockContentConst(const BPLib_MEM_Block_t *cb)
{
    if (cb != NULL && BPLib_MEM_IsAnyContentNode(cb))
    {
        return (const BPLib_MEM_BlockContent_t *)(const void *)cb;
    }
    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_BlockDereferenceContent
 *
 *-----------------------------------------------------------------*/
BPLib_MEM_BlockContent_t *BPLib_MEM_BlockDereferenceContent(BPLib_MEM_Block_t *cb)
{
    BPLib_MEM_BlockContent_t *block_ptr;

    block_ptr = BPLib_MEM_GetBlockContent(cb);

    if (block_ptr != NULL)
    {
        // TODO Does MEM need refs? CACHE has them.
        /* Additionally, if this block is a ref, then also dereference it to get to the real block */
        /* In theory this could be a chain of refs, so this is a while() but in reality it should be just one */
        /**
         * while (block_ptr->header.base_link.type == BPLib_MEM_BlocktypeRef)
         * {
         *     assert(block_ptr->u.ref.pref_target != NULL);
         *     block_ptr = block_ptr->u.ref.pref_target;
         *     // This should have always arrived at an actual content block
         *    assert(BPLib_MEM_IsAnyContentNode(&block_ptr->header.base_link));
         * }
         */
        return block_ptr;
    }

    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_GetUserDataOffsetByBlocktype
 *
 *-----------------------------------------------------------------*/
size_t BPLib_MEM_GetUserDataOffsetByBlocktype(BPLib_MEM_Blocktype_t bt)
{
    static const size_t USER_DATA_START_OFFSET[BPLib_MEM_BlocktypeMax] = {
        [BPLib_MEM_BlocktypeUndefined] = SIZE_MAX,
        [BPLib_MEM_BlocktypeApi]       = BPLIB_MEM_GET_BUFFER_USER_START_OFFSET(api),
        [BPLib_MEM_BlocktypeGeneric]   = BPLIB_MEM_GET_BUFFER_USER_START_OFFSET(generic_data)};

    if (bt >= BPLib_MEM_BlocktypeMax)
    {
        return SIZE_MAX;
    }

    return USER_DATA_START_OFFSET[bt];
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_GetGenericDataCapacity
 *
 *-----------------------------------------------------------------*/
size_t BPLib_MEM_GetGenericDataCapacity(const BPLib_MEM_Block_t *cb)
{
    size_t data_offset;

    data_offset = BPLib_MEM_GetUserDataOffsetByBlocktype(cb->type);
    if (data_offset > sizeof(BPLib_MEM_BlockBuffer_t))
    {
        return 0;
    }

    return sizeof(BPLib_MEM_BlockBuffer_t) - data_offset;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_InitSecondaryLink
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_InitSecondaryLink(BPLib_MEM_Block_t *base_block, BPLib_MEM_Block_t *secondary_link,
                                     BPLib_MEM_Blocktype_t block_type)
{
    size_t offset;

    if (base_block == NULL)
    {
        offset = 0;
    }
    else
    {
        offset = (uint8_t *)secondary_link - (uint8_t *)base_block;
        assert(offset > 0 && offset < sizeof(BPLib_MEM_BlockContent_t));
    }

    BPLib_MEM_LinkReset(secondary_link, block_type, offset);

    assert(BPLib_MEM_IsSecondaryIndexNode(secondary_link));
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_InitListHead
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_InitListHead(BPLib_MEM_Block_t *base_block, BPLib_MEM_Block_t *list_head)
{
    BPLib_MEM_InitSecondaryLink(base_block, list_head, BPLib_MEM_BlocktypeListHead);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_SubqInit
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_SubqInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_SubqBase_t *qblk)
{
    /* init the link structs */
    BPLib_MEM_InitListHead(base_block, &qblk->block_list);
    qblk->pull_count = 0;
    qblk->push_count = 0;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_InsertAfter
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_InsertAfter(BPLib_MEM_Block_t *list, BPLib_MEM_Block_t *node)
{
    /* node being inserted should always be a singleton */
    assert(BPLib_MEM_IsLinkUnattached(node));

    node->next       = list->next;
    node->prev       = list;
    list->next       = node;
    node->next->prev = node;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_InsertBefore
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_InsertBefore(BPLib_MEM_Block_t *list, BPLib_MEM_Block_t *node)
{
    /* node being inserted should always be a singleton */
    assert(BPLib_MEM_IsLinkUnattached(node));

    node->prev       = list->prev;
    node->next       = list;
    list->prev       = node;
    node->prev->next = node;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_ExtractNode
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_ExtractNode(BPLib_MEM_Block_t *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next       = node;
    node->prev       = node;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_MergeList
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_MergeList(BPLib_MEM_Block_t *dest, BPLib_MEM_Block_t *src)
{

    BPLib_MEM_Block_t *dlast = dest->prev; // last node in dest list
    BPLib_MEM_Block_t *slast = src->prev;  // last node in src list

    /* nominally combine the two lists.
     * NOTE: This (temporarily) yields a list with two head nodes. */

    dlast->next = src;
    slast->next = dest;
    dest->prev  = slast;
    src->prev   = dlast;

}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_GetUserContentSize
 *
 *-----------------------------------------------------------------*/
size_t BPLib_MEM_GetUserContentSize(const BPLib_MEM_Block_t *cb)
{
    const BPLib_MEM_BlockContent_t *block;

    block = BPLib_MEM_GetBlockContentConst(cb);
    if (block != NULL)
    {
        return block->header.user_content_length;
    }
    return 0;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_ReadRefcount
 *
 *-----------------------------------------------------------------*/
size_t BPLib_MEM_ReadRefcount(const BPLib_MEM_Block_t *cb)
{
    const BPLib_MEM_BlockContent_t *block;

    block = BPLib_MEM_GetBlockContentConst(cb);
    if (block != NULL)
    {
        return block->header.refcount;
    }
    return 0;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_GetParentPoolFromLink
 *
 *-----------------------------------------------------------------*/
BPLib_MEM_Pool_t *BPLib_MEM_GetParentPoolFromLink(BPLib_MEM_Block_t *cb)
{
    BPLib_MEM_BlockContent_t *block;

    block = BPLib_MEM_GetBlockContent(BPLib_MEM_GetBlockFromLink(cb));
    if (block != NULL)
    {
        /* the "parent_offset" should provide a map back to the parent pool.
         * in this context the units are blocks, not bytes (this extends the
         * representable range for large pools, and simplifies this arithmetic) */
        block -= block->header.base_link.parent_offset;

        /* this should have always arrived at the admin block, which is the first block */
        assert(block->header.base_link.type == BPLib_MEM_BlocktypeAdmin);
    }
    else
    {
        block = NULL;
    }

    return (BPLib_MEM_Pool_t *)block;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_InitBaseObject
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_InitBaseObject(BPLib_MEM_BlockHeader_t *block_hdr, uint16_t user_content_length,
                                  uint32_t content_type_signature)
{
    block_hdr->user_content_length    = user_content_length;
    block_hdr->content_type_signature = content_type_signature;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_AllocBlockInternal
 *
 * NOTE: this must be invoked with the lock already held
 *-----------------------------------------------------------------*/
BPLib_MEM_BlockContent_t *BPLib_MEM_AllocBlockInternal(BPLib_MEM_Pool_t *pool, BPLib_MEM_Blocktype_t blocktype,
                                                              uint32_t content_type_signature, void *init_arg,
                                                              uint8_t priority)
{
    BPLib_MEM_Block_t           *node;
    BPLib_MEM_BlockContent_t    *block;
    BPLib_MEM_ApiContent_t      *api_block;
    size_t                       data_offset;
    uint32_t                     alloc_threshold;
    uint32_t                     block_count;

    BPLib_MEM_BlockAdminContent_t *admin;

    admin = BPLib_MEM_GetAdmin(pool);

    /**
     * Only BlocktypeGeneric and BlocktypeApi are valid in MEM.
     */
    if (!(blocktype == BPLib_MEM_BlocktypeGeneric || blocktype == BPLib_MEM_BlocktypeApi))
    {
        return NULL;
    }

    /**
     * Check free block threshold: This check may be removed because it applies more to CACHE.
     * Note that Bundle CACHE may take additional pool blocks (refs, cbor, etc)
     * in order to forward stored bundles along when the time comes.
     *
     * Without a sufficient buffer/threshold, if the entire pool is consumed by bundles, it will
     * prevent any blocks from being forwarded (and eventually freed) as well, creating deadlock.
     *
     * This soft limit only applies for actual bundle blocks, not for refs.
     */
    // TODO Consider removing alloc_threshold from BPLib_MEM_AllocBlockInternal
    alloc_threshold = (admin->bblock_alloc_threshold * priority) / 255;

    block_count = BPLib_MEM_SubqGetDepth(&admin->free_blocks);
    if (block_count <= (admin->bblock_alloc_threshold - alloc_threshold))
    {
        /* no free blocks available for the requested type */
        return NULL;
    }

    /* Determine how to initialize this block by looking up the content type */
    api_block = (BPLib_MEM_ApiContent_t *)(void *)BPLib_MEM_RBT_SearchUnique(content_type_signature,
                                                                             &admin->blocktype_registry);
    if (api_block == NULL)
    {
        /* no constructor, cannot create the block! */
        return NULL;
    }

    /* sanity check that the user content will fit in the block */
    data_offset = BPLib_MEM_GetUserDataOffsetByBlocktype(blocktype);
    if (data_offset > sizeof(BPLib_MEM_BlockBuffer_t) ||
        (data_offset + api_block->user_content_size) > sizeof(BPLib_MEM_BlockBuffer_t))
    {
        /* User content will not fit in the block - cannot create an instance of this type combo */
        return NULL;
    }
    /* get a block */
    node = BPLib_MEM_SubqPullSingle(&admin->free_blocks);
    if (node == NULL)
    {
        /* this should never happen, because depth was already checked */
        return NULL;
    }

    /*
     * Convert from blocks free to blocks used, and update high watermark if necessary.
     * This is +1 to include the block that was just pulled (that is, a call to
     * BPLib_MEM_SubqGetDepth() on the free list now will return 1 fewer than it
     * did earlier in this function).
     */
    block_count = 1 + admin->num_bufs_total - block_count;
    if (block_count > admin->max_alloc_watermark)
    {
        admin->max_alloc_watermark = block_count;
    }

    node->type = blocktype;
    block      = BPLib_MEM_GetBlockContent(node);
    block      = BPLib_MEM_GetBlockContent(node);

    /*
     * zero fill the content part first, this ensures that this is always done,
     * and avoids the need for the module to supply a dedicated constructor just to zero it
     */
    memset(&block->u, 0, data_offset + api_block->user_content_size);

    BPLib_MEM_InitBaseObject(&block->header, api_block->user_content_size, content_type_signature);

    /* If the module did supply a constructor, invoke it now */
    if (api_block->api.construct != NULL)
    {
        /* A constructor really should never fail nominally, if it does there is probably a bug */
        if (api_block->api.construct(init_arg, node) != BPLIB_SUCCESS)
        {
            bplog(NULL, BPLIB_MEM_FLAG_DIAGNOSTIC, "Constructor failed for block type %d, signature %lx\n", blocktype,
                  (unsigned long)content_type_signature);
        }
    }

    return block;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_SubqMergeList
 *
 *-----------------------------------------------------------------*/
uint32_t BPLib_MEM_SubqMergeList(BPLib_MEM_SubqBase_t *subq_dst, BPLib_MEM_Block_t *list)
{
    #ifdef BPLIB_MEM_SUBQ_TESTS
    uint32_t block_count;

    /* for record-keeping, must count the blocks to know how many are being moved here */
    block_count = BPLib_MEM_ListCountBlocks(list);

    BPLib_MEM_MergeList(&subq_dst->block_list, list);
    BPLib_MEM_ExtractNode(list);
    subq_dst->push_count += block_count;

    return block_count;
    #else // BPLIB_MEM_SUBQ_TESTS
    return 0;
    #endif
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_SubqMoveAll
 *
 *-----------------------------------------------------------------*/
uint32_t BPLib_MEM_SubqMoveAll(BPLib_MEM_SubqBase_t *subq_dst, BPLib_MEM_SubqBase_t *subq_src)
{
    uint32_t queue_depth;

    /* appends the entire subq to the destination */
    queue_depth = BPLib_MEM_SubqGetDepth(subq_src);
    if (queue_depth > 0)
    {
        BPLib_MEM_MergeList(&subq_dst->block_list, &subq_src->block_list);
        BPLib_MEM_ExtractNode(&subq_src->block_list);
        subq_src->pull_count += queue_depth;
        subq_dst->push_count += queue_depth;
    }
    return queue_depth;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_GenericDataAlloc
 *
 *-----------------------------------------------------------------*/
BPLib_MEM_Block_t *BPLib_MEM_GenericDataAlloc(BPLib_MEM_Pool_t *pool, uint32_t magic_number, void *init_arg)
{
    BPLib_MEM_BlockContent_t *result;
    BPLib_MEM_Lock_t          *lock;

    lock   = BPLib_MEM_LockResource(pool);
    result = BPLib_MEM_AllocBlockInternal(pool, BPLib_MEM_BlocktypeGeneric, magic_number, init_arg,
                                              BPLIB_MEM_ALLOC_PRI_MLO);
    BPLib_MEM_LockRelease(lock);

    return (BPLib_MEM_Block_t *)result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_SubqPushSingle
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_SubqPushSingle(BPLib_MEM_SubqBase_t *subq, BPLib_MEM_Block_t *cpb)
{
    BPLib_MEM_InsertBefore(&subq->block_list, cpb);
    ++subq->push_count;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_SubqPullSingle
 *
 *-----------------------------------------------------------------*/
BPLib_MEM_Block_t *BPLib_MEM_SubqPullSingle(BPLib_MEM_SubqBase_t *subq)
{
    BPLib_MEM_Block_t *node;

    /* if the head is reached here, then the list is empty */
    node = subq->block_list.next;
    if (BPLib_MEM_IsListHead(node))
    {
        return NULL;
    }

    BPLib_MEM_ExtractNode(node);
    ++subq->pull_count;

    return node;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_RecycleBlockInternal
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_RecycleBlockInternal(BPLib_MEM_Pool_t *pool, BPLib_MEM_Block_t *blk)
{
    BPLib_MEM_BlockAdminContent_t *admin;

    admin = BPLib_MEM_GetAdmin(pool);

    BPLib_MEM_ExtractNode(blk);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, blk);
    BPLib_MEM_SubqPushSingle(&admin->recycle_blocks, blk);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_RecycleAllBlocksInList
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_RecycleAllBlocksInList(BPLib_MEM_Pool_t *pool, BPLib_MEM_Block_t *list)
{
    BPLib_MEM_Lock_t                *lock;
    BPLib_MEM_BlockAdminContent_t   *admin;

    /*
     * If the pool was not specified, then attempt to deduce from the list pointer.
     *
     * List objects embedded within pool objects have breadcrumbs such that the parent pool
     * pointer can be reconstituted.  However, if the list is NOT part of a pool object
     * (e.g. a temporary list created on the stack, for instance) then this does not work.
     */
    if (pool == NULL)
    {
        pool = BPLib_MEM_GetParentPoolFromLink(list);
    }

    admin = BPLib_MEM_GetAdmin(pool);

    assert(BPLib_MEM_IsListHead(list));
    lock = BPLib_MEM_LockResource(pool);
    BPLib_MEM_SubqMergeList(&admin->recycle_blocks, list);
    BPLib_MEM_LockRelease(lock);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_RecycleBlock
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_RecycleBlock(BPLib_MEM_Block_t *blk)
{
    BPLib_MEM_Lock_t *lock;
    BPLib_MEM_Pool_t      *pool;

    /* only real content blocks should be recycled.  No secondary links or components/members. */
    assert(BPLib_MEM_IsAnyContentNode(blk));

    pool = BPLib_MEM_GetParentPoolFromLink(blk);
    lock = BPLib_MEM_LockResource(pool);
    BPLib_MEM_RecycleBlockInternal(pool, blk);
    BPLib_MEM_LockRelease(lock);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_SubqDropAll
 *
 *-----------------------------------------------------------------*/
uint32_t BPLib_MEM_SubqDropAll(BPLib_MEM_Pool_t *pool, BPLib_MEM_SubqBase_t *subq)
{
    uint32_t queue_depth;

    /* appends the entire subq to the destination */
    queue_depth = BPLib_MEM_SubqGetDepth(subq);
    if (queue_depth > 0)
    {
        BPLib_MEM_RecycleAllBlocksInList(pool, &subq->block_list);
        subq->pull_count += queue_depth;
    }
    return queue_depth;
}


#ifdef BPLIB_MEM_ENABLE_LIST // STOR - Cache does blocktype lists, but MEM needs a generic list.
/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_ListIterGotoFirst
 * Function: BPLib_MEM_ListIterGotoFirst
 *
 *-----------------------------------------------------------------*/
int BPLib_MEM_ListIterGotoFirst(const BPLib_MEM_Block_t *list, BPLib_MEM_ListIter_t *iter)
{
    if (!BPLib_MEM_IsListHead(list))
    {
        return BPLIB_ERROR;
    }

    iter->pending_entry = list->next;

    return BPLib_MEM_ListIterForward(iter);
    return BPLib_MEM_ListIterForward(iter);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_ListIterGotoLast
 *
 *-----------------------------------------------------------------*/
int BPLib_MEM_ListIterGotoLast(const BPLib_MEM_Block_t *list, BPLib_MEM_ListIter_t *iter)
{
    if (!BPLib_MEM_IsListHead(list))
    {
        return BPLIB_ERROR;
    }

    iter->pending_entry = list->prev;

    return BPLib_MEM_ListIterReverse(iter);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_ListIterForward
 *
 *-----------------------------------------------------------------*/
int BPLib_MEM_ListIterForward(BPLib_MEM_ListIter_t *iter)
{
    if (iter->pending_entry == NULL || BPLib_MEM_IsListHead(iter->pending_entry))
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
 * Function: BPLib_MEM_ListIterReverse
 *
 *-----------------------------------------------------------------*/
int BPLib_MEM_ListIterReverse(BPLib_MEM_ListIter_t *iter)
{
    if (iter->pending_entry == NULL || BPLib_MEM_IsListHead(iter->pending_entry))
    if (iter->pending_entry == NULL || BPLib_MEM_IsListHead(iter->pending_entry))
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
 * Function: BPLib_MEM_ForeachItemInList
 *
 *-----------------------------------------------------------------*/
int BPLib_MEM_ForeachItemInList(BPLib_MEM_Block_t *list, bool always_remove,
                                     BPLib_MEM_CallbackFunc_t callback_fn, void *callback_arg)
{
    BPLib_MEM_Block_t *curr_node;
    BPLib_MEM_Block_t *next_node;
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
            BPLib_MEM_ExtractNode(curr_node);
        }

        callback_fn(callback_arg, curr_node);
        ++count;
    }

    return count;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_SearchList
 *
 *-----------------------------------------------------------------*/
BPLib_MEM_Block_t *BPLib_MEM_SearchList(const BPLib_MEM_Block_t *list, BPLib_MEM_CallbackFunc_t match_fn,
                                             void *match_arg)
{
    int                     status;
    BPLib_MEM_ListIter_t iter;
    BPLib_MEM_ListIter_t iter;

    memset(&iter, 0, sizeof(iter));

    status = BPLib_MEM_ListIterGotoFirst(list, &iter);
    status = BPLib_MEM_ListIterGotoFirst(list, &iter);
    while (status == BPLIB_SUCCESS)
    {
        /* this calls the match function with the actual content block, as that is where
         * the real information lies (this is typically a list full of secondary links) */
        if (match_fn(match_arg, BPLib_MEM_GetBlockFromLink(iter.position)) == 0)
        if (match_fn(match_arg, BPLib_MEM_GetBlockFromLink(iter.position)) == 0)
        {
            break;
        }
        status = BPLib_MEM_ListIterForward(&iter);
        status = BPLib_MEM_ListIterForward(&iter);
    }

    /* the iterator sets position to NULL if end of list was reached */
    return iter.position;
}

#endif // BPLIB_MEM_ENABLE_LIST

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_CollectBlocks
 *
 *-----------------------------------------------------------------*/
uint32_t BPLib_MEM_CollectBlocks(BPLib_MEM_Pool_t *pool, uint32_t limit)
{
    BPLib_MEM_Block_t              *rblk;
    BPLib_MEM_ApiContent_t         *api_block;
    BPLib_MEM_BlockContent_t       *content;
    BPLib_MEM_CallbackFunc_t        destruct;
    uint32_t                        count;
    BPLib_MEM_Lock_t               *lock;
    BPLib_MEM_BlockAdminContent_t  *admin;

    admin = BPLib_MEM_GetAdmin(pool);

    count = 0;
    lock  = BPLib_MEM_LockResource((void *)pool);
    while (count < limit)
    {
        rblk = BPLib_MEM_SubqPullSingle(&admin->recycle_blocks);
        if (rblk == NULL)
        {
            break;
        }

        /* recycled blocks must all be "real" blocks (not secondary refs or head nodes, etc) and
         * have refcount of 0, or else bad things might happen */
        content = BPLib_MEM_GetBlockContent(rblk);
        assert(content != NULL);
        assert(content->header.refcount == 0);

        /* figure out how to de-initialize the user content by looking up the content type */
        api_block = (BPLib_MEM_ApiContent_t *)(void *)BPLib_MEM_RBT_SearchUnique(content->header.content_type_signature,
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

        // printf("DEBUG: %s() recycled block type %d\n", __func__, rblk->type);
        ++count;

        /* always return _this_ node to the free pile */
        rblk->type = BPLib_MEM_BlocktypeUndefined;
        BPLib_MEM_InitBaseObject(&content->header, 0, 0);

          BPLib_MEM_LockAcquire(lock);
        BPLib_MEM_SubqPushSingle(&admin->free_blocks, rblk);
    }

    BPLib_MEM_LockRelease(lock);

    return count;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_Maintain
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_Maintain(BPLib_MEM_Pool_t *pool)
{
    /* the check for non-empty list can be done unlocked, as it
     * involves counter values which should be testable in an atomic fashion.
     * note this isn't final - Subq will be re-checked after locking, if this is true */
    if (BPLib_MEM_SubqGetDepth(&BPLib_MEM_GetAdmin(pool)->recycle_blocks) != 0)
    {
        BPLib_MEM_CollectBlocks(pool, BPLIB_MEM_MAINTENCE_COLLECT_LIMIT);
    }
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_RegisterBlocktypeInternal
 *
 *-----------------------------------------------------------------*/
int BPLib_MEM_RegisterBlocktypeInternal(BPLib_MEM_Pool_t *pool, uint32_t magic_number,
                                            const BPLib_MEM_BlocktypeApi_t *api, size_t user_content_size)
{
    BPLib_MEM_BlockContent_t       *ablk;
    BPLib_MEM_ApiContent_t         *api_block;
    int                             status;
    BPLib_MEM_BlockAdminContent_t  *admin;

    admin = BPLib_MEM_GetAdmin (pool);

    /* before doing anything, check if this is a duplicate.  If so, ignore it.
     * This permits "lazy binding" of apis where the blocktype is registered at the time of first use */
    if (BPLib_MEM_RBT_SearchUnique(magic_number, &admin->blocktype_registry) != NULL)
    {
        return BPLIB_MEM_RBT_DUPLICATE;
    }

    ablk = BPLib_MEM_AllocBlockInternal(pool, BPLib_MEM_BlocktypeApi, 0, NULL, BPLIB_MEM_ALLOC_PRI_LO);
    if (ablk == NULL)
    {
        return BPLIB_ERROR;
    }

    api_block = &ablk->u.api;

    if (api != NULL)
    {
        api_block->api = *api;
    }
    api_block->user_content_size = user_content_size;

    status = BPLib_MEM_RBT_InsertValueUnique(magic_number, &admin->blocktype_registry, &api_block->rbt_link);

    /* due to the pre-check above this should always have been successful, but just in case, return the block if error
     */
    if (status != BPLIB_SUCCESS)
    {
        BPLib_MEM_RecycleBlockInternal(pool, &ablk->header.base_link);
    }

    return status;
}


/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_RegisterBlocktype
 *
 *-----------------------------------------------------------------*/
int BPLib_MEM_RegisterBlocktype(BPLib_MEM_Pool_t *pool, uint32_t magic_number, const BPLib_MEM_BlocktypeApi_t *api,
                                size_t user_content_size)
{
    BPLib_MEM_Lock_t   *lock;
    int                 result;

    lock   = BPLib_MEM_LockResource(pool);
    result = BPLib_MEM_RegisterBlocktypeInternal(pool, magic_number, api, user_content_size);
    BPLib_MEM_LockRelease(lock);
    return result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_QueryMemCurrentUse
 *
 *-----------------------------------------------------------------*/
size_t BPLib_MEM_QueryMemCurrentUse(BPLib_MEM_Pool_t *pool)
{
    BPLib_MEM_BlockAdminContent_t *admin;

    admin = BPLib_MEM_GetAdmin(pool);

    return (BPLib_MEM_SubqGetDepth(&admin->free_blocks) * (size_t)admin->buffer_size);

    return 32767;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_QueryMemMaxUse
 *
 *-----------------------------------------------------------------*/
size_t BPLib_MEM_QueryMemMaxUse(BPLib_MEM_Pool_t *pool)
{
    BPLib_MEM_BlockAdminContent_t *admin;

    admin = BPLib_MEM_GetAdmin(pool);

    return (admin->max_alloc_watermark * (size_t)admin->buffer_size);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_DebugPrintListStats
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_DebugPrintListStats(BPLib_MEM_Block_t *list, const char *label)
{
    printf("MEM DEBUG: %s(): %s Not implemented yet.\n", __func__, label);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_DebugScan
 * Function: BPLib_MEM_DebugScan
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_DebugScan(BPLib_MEM_Pool_t *pool)
{
    size_t                             i;
    BPLib_MEM_BlockContent_t       *pchunk;
    uint32_t                           count_by_type[BPLib_MEM_BlocktypeMax];
    uint32_t                           count_invalid;
    BPLib_MEM_BlockAdminContent_t *admin;

    admin = BPLib_MEM_GetAdmin(pool);

    printf("DEBUG: %s(): total blocks=%u, buffer_size=%zu, free=%u, recycled=%u\n", __func__,
           (unsigned int)admin->num_bufs_total, admin->buffer_size,
           (unsigned int)BPLib_MEM_SubqGetDepth(&admin->free_blocks),
           (unsigned int)BPLib_MEM_SubqGetDepth(&admin->recycle_blocks));

    BPLib_MEM_DebugPrintListStats(&admin->free_blocks.block_list, "free_blocks");
    BPLib_MEM_DebugPrintListStats(&admin->recycle_blocks.block_list, "recycle_blocks");
    BPLib_MEM_DebugPrintListStats(&admin->active_list, "active_list");

    memset(count_by_type, 0, sizeof(count_by_type));
    count_invalid = 0;
    pchunk        = &pool->admin_block;

    /* Note that num_bufs_total does not include the admin block (0) */
    for (i = 0; i <= admin->num_bufs_total; ++i)
    {
        if (i == 0)
        {
            assert(pchunk->header.base_link.type == BPLib_MEM_BlocktypeAdmin);
        }
        else if (pchunk->header.base_link.type < BPLib_MEM_BlocktypeMax)
        {
            ++count_by_type[pchunk->header.base_link.type];
        }
        else
        {
            ++count_invalid;
        }
        ++pchunk;
    }

    for (i = 0; i < BPLib_MEM_BlocktypeMax; ++i)
    {
        printf("DEBUG: %s(): block type=%zu count=%lu\n", __func__, i, (unsigned long)count_by_type[i]);
    }
    printf("DEBUG: %s(): invalid count=%lu\n", __func__, (unsigned long)count_invalid);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_PoolCreate
 *
 *-----------------------------------------------------------------*/
BPLib_MEM_Pool_t *BPLib_MEM_PoolCreate(void *pool_mem, size_t pool_size)
{
    BPLib_MEM_Pool_t                     *pool;
    size_t                             remain;
    BPLib_MEM_BlockContent_t       *pchunk;
    BPLib_MEM_BlockAdminContent_t *admin;

    /* this is just a sanity check, a pool that has only 1 block will not
     * be useful for anything, but it can at least be created */
    if (pool_mem == NULL || pool_size < sizeof(BPLib_MEM_Pool_t))
    {
        /* pool memory too small */
        return NULL;
    }

    /* initialize the lock table - OK to call this multiple times,
     * subsequent calls shouldn't do anything */
    BPLib_MEM_LockInit();

    /* wiping the entire memory might be overkill, but it is only done once
     * at start up, and this may also help verify that the memory "works" */
    memset(pool_mem, 0, pool_size);

    pool = pool_mem;

    BPLib_MEM_LinkReset(&pool->admin_block.header.base_link, BPLib_MEM_BlocktypeAdmin, 0);
    admin = BPLib_MEM_GetAdmin(pool);

    /* the block lists are circular, as this reduces
     * complexity of operations (never a null pointer) */
    admin->buffer_size = sizeof(BPLib_MEM_BlockContent_t);
    BPLib_MEM_SubqInit(&pool->admin_block.header.base_link, &admin->free_blocks);
    BPLib_MEM_SubqInit(&pool->admin_block.header.base_link, &admin->recycle_blocks);
    BPLib_MEM_InitListHead(&pool->admin_block.header.base_link, &admin->active_list);
    BPLib_MEM_RBT_InitRoot(&admin->blocktype_registry);

    /* start at the _next_ buffer, which is the first usable buffer (first is the admin block) */
    pchunk = &pool->admin_block + 1;
    remain = pool_size - sizeof(BPLib_MEM_BlockContent_t);

    /* register the first API type, which is 0.
     * Notably this prevents other modules from actually registering something at 0. */
    BPLib_MEM_RBT_InsertValueUnique(BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE, &admin->blocktype_registry,
                                  &admin->blocktype_cbor.rbt_link);

    while (remain >= sizeof(BPLib_MEM_BlockContent_t))
    {
        BPLib_MEM_LinkReset(&pchunk->header.base_link, BPLib_MEM_BlocktypeUndefined, pchunk - &pool->admin_block);
        BPLib_MEM_SubqPushSingle(&admin->free_blocks, &pchunk->header.base_link);
        remain -= sizeof(BPLib_MEM_BlockContent_t);
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
