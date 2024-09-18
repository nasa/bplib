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
#include "bplib_mem.h"
#include "bplib_mem_internal.h"

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockPrimaryCast
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_BblockPrimary_t *BPLib_STOR_CACHE_BblockPrimaryCast(BPLib_MEM_block_t *cb)
{
    #ifdef STOR // blocktype
    BPLib_STOR_CACHE_BlockContent_t *content;

    content = BPLib_STOR_CACHE_BlockDereferenceContent(cb);
    if (content != NULL && content->header.base_link.type == BPLib_STOR_CACHE_BlocktypePrimary)
    {
        return &content->u.primary.pblock;
    }
    #endif
    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockCanonicalCast
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_BblockCanonical_t *BPLib_STOR_CACHE_BblockCanonicalCast(BPLib_MEM_block_t *cb)
{
    BPLib_STOR_CACHE_BlockContent_t *content;

    content = BPLib_STOR_CACHE_BlockDereferenceContent(cb);
    if (content != NULL && content->header.base_link.type == BPLib_STOR_CACHE_BlocktypeCanonical)
    {
        return &content->u.canonical.cblock;
    }
    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockCborCast
 *
 *-----------------------------------------------------------------*/
void *BPLib_STOR_CACHE_BblockCborCast(BPLib_MEM_block_t *cb)
{
    /* CBOR data blocks are nothing more than generic blocks with a different sig */
    return BPLib_STOR_CACHE_GenericDataCast(cb, BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockCborSetSize
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_BblockCborSetSize(BPLib_MEM_block_t *cb, size_t user_content_size)
{
    BPLib_STOR_CACHE_BlockContent_t *content;

    content = BPLib_STOR_CACHE_BlockDereferenceContent(cb);
    if (content != NULL && content->header.base_link.type == BPLib_STOR_CACHE_BlocktypeGeneric &&
        content->header.content_type_signature == BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE)
    {
        content->header.user_content_length = user_content_size;
    }
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockPrimaryInit
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_BblockPrimaryInit(BPLib_MEM_block_t *base_block, BPLib_STOR_CACHE_BblockPrimary_t *pblk)
{
    BPLib_STOR_CACHE_InitListHead(base_block, &pblk->cblock_list);
    BPLib_STOR_CACHE_InitListHead(base_block, &pblk->chunk_list);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockCanonicalInit
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_BblockCanonicalInit(BPLib_MEM_block_t *base_block, BPLib_STOR_CACHE_BblockCanonical_t *cblk)
{
    BPLib_STOR_CACHE_InitListHead(base_block, &cblk->chunk_list);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockPrimaryAlloc
 *
 *-----------------------------------------------------------------*/
BPLib_MEM_block_t *BPLib_STOR_CACHE_BblockPrimaryAlloc(BPLib_MEM_t *pool, uint32_t magic_number, void *init_arg,
                                                      uint8_t priority, uint64_t timeout)
{
    BPLib_STOR_CACHE_BlockContent_t *result;
    BPLib_MEM_lock_t          *lock;
    bool                         within_timeout;

    lock           = BPLib_STOR_CACHE_LockResource(pool);
    within_timeout = true;
    while (true)
    {
        result =
            BPLib_STOR_CACHE_AllocBlockInternal(pool, BPLib_STOR_CACHE_BlocktypePrimary, magic_number, init_arg, priority);
        if (result != NULL || !within_timeout)
        {
            break;
        }

        within_timeout = BPLib_STOR_CACHE_LockWait(lock, timeout);
    }
    BPLib_STOR_CACHE_LockRelease(lock);

    return (BPLib_MEM_block_t *)result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockCanonicalAlloc
 *
 *-----------------------------------------------------------------*/
BPLib_MEM_block_t *BPLib_STOR_CACHE_BblockCanonicalAlloc(BPLib_MEM_t *pool, uint32_t magic_number, void *init_arg)
{
    BPLib_STOR_CACHE_BlockContent_t *result;
    BPLib_MEM_lock_t          *lock;

    lock   = BPLib_STOR_CACHE_LockResource(pool);
    result = BPLib_STOR_CACHE_AllocBlockInternal(pool, BPLib_STOR_CACHE_BlocktypeCanonical, magic_number, init_arg,
                                              BPLIB_MEM_ALLOC_PRI_MED);
    BPLib_STOR_CACHE_LockRelease(lock);

    return (BPLib_MEM_block_t *)result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockCborAlloc
 *
 *-----------------------------------------------------------------*/
BPLib_MEM_block_t *BPLib_STOR_CACHE_BblockCborAlloc(BPLib_MEM_t *pool)
{
    BPLib_STOR_CACHE_BlockContent_t *result;
    BPLib_MEM_lock_t          *lock;

    lock   = BPLib_STOR_CACHE_LockResource(pool);
    result = BPLib_STOR_CACHE_AllocBlockInternal(pool, BPLib_STOR_CACHE_BlocktypeGeneric, BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE,
                                              NULL, BPLIB_MEM_ALLOC_PRI_MED);
    BPLib_STOR_CACHE_LockRelease(lock);

    return (BPLib_MEM_block_t *)result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockCborAppend
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_BblockCborAppend(BPLib_MEM_block_t *head, BPLib_MEM_block_t *blk)
{
    /* this just confirms it actually is a CBOR data block */
    assert(BPLib_STOR_CACHE_BblockCborCast(blk) != NULL);
    BPLib_STOR_CACHE_InsertBefore(head, blk);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockPrimaryDropEncode
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_BblockPrimaryDropEncode(BPLib_STOR_CACHE_BblockPrimary_t *cpb)
{
    BPLib_MEM_block_t *elist;

    /* If there is any existing encoded data, return it to the pool */
    elist = BPLib_STOR_CACHE_BblockPrimaryGetEncodedChunks(cpb);
    if (BPLib_STOR_CACHE_IsNonemptyListHead(elist))
    {
        BPLib_STOR_CACHE_RecycleAllBlocksInList(NULL, elist);
    }
    cpb->block_encode_size_cache  = 0;
    cpb->bundle_encode_size_cache = 0;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockCanonicalDropEncode
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_BblockCanonicalDropEncode(BPLib_STOR_CACHE_BblockCanonical_t *ccb)
{
    BPLib_MEM_block_t *elist;

    /* If there is any existing encoded data, return it to the pool */
    elist = BPLib_STOR_CACHE_BblockCanonicalGetEncodedChunks(ccb);
    if (BPLib_STOR_CACHE_IsNonemptyListHead(elist))
    {
        BPLib_STOR_CACHE_RecycleAllBlocksInList(NULL, elist);
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
 * Function: BPLib_STOR_CACHE_BblockCborExport
 *
 *-----------------------------------------------------------------*/
size_t BPLib_STOR_CACHE_BblockCborExport(BPLib_MEM_block_t *list, void *out_ptr, size_t max_out_size, size_t seek_start,
                                      size_t max_count)
{
    BPLib_MEM_block_t *blk;
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
        blk = BPLib_STOR_CACHE_GetNextBlock(blk);
        if (blk == list)
        {
            break;
        }
        src_ptr = BPLib_STOR_CACHE_BblockCborCast(blk);
        if (src_ptr == NULL)
        {
            break;
        }
        chunk_sz = BPLib_STOR_CACHE_GetUserContentSize(blk);
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
 * Function: BPLib_STOR_CACHE_BblockPrimaryAppend
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_BblockPrimaryAppend(BPLib_STOR_CACHE_BblockPrimary_t *cpb, BPLib_MEM_block_t *blk)
{
    BPLib_STOR_CACHE_BblockCanonical_t *ccb;

    /* this should only be invoked w/canonical blocks.  Anything else is a bug. */
    ccb = BPLib_STOR_CACHE_BblockCanonicalCast(blk);
    assert(ccb != NULL);
    assert(ccb->bundle_ref == NULL);

    /*
     * TBD on block ordering requirements.  The BPv7 spec says the payload should be last,
     * but other blocks may appear in any order.  It is not clear if there is any reason
     * to be more specific about block order.
     *
     * For now, if the block being inserted has block num 1, put it last, otherwise put it first.
     */
    if (BPLib_STOR_CACHE_BblockCanonicalGetLogical(ccb)->canonical_block.blockNum == 1)
    {
        /* this puts it last */
        BPLib_STOR_CACHE_InsertBefore(&cpb->cblock_list, blk);
    }
    else
    {
        /* this puts it first */
        BPLib_STOR_CACHE_InsertAfter(&cpb->cblock_list, blk);
    }

    /* this changes the size of the fully-encoded bundle, but don't recalculate now */
    cpb->bundle_encode_size_cache = 0;
    ccb->bundle_ref               = cpb;
}

BPLib_MEM_block_t *BPLib_STOR_CACHE_BblockPrimaryLocateCanonical(BPLib_STOR_CACHE_BblockPrimary_t *cpb,
                                                                 bp_blocktype_t                block_type)
{
    BPLib_MEM_block_t            *cblk;
    BPLib_STOR_CACHE_BblockCanonical_t *ccb;

    cblk = BPLib_STOR_CACHE_BblockPrimaryGetCanonicalList(cpb);
    while (true)
    {
        /* note this searches in reverse order, anticipating this will often be searching for payload,
         * which is supposed to be the last block in the bundle.  If the block is there it will be found
         * regardless of the search order, but this speeds up the more frequent case */
        cblk = BPLib_STOR_CACHE_GetPrevBlock(cblk);
        if (BPLib_STOR_CACHE_IsListHead(cblk))
        {
            /* end of list, not found */
            cblk = NULL;
            break;
        }
        ccb = BPLib_STOR_CACHE_BblockCanonicalCast(cblk);
        if (ccb != NULL && ccb->canonical_logical_data.canonical_block.blockType == block_type)
        {
            /* found it */
            break;
        }
    }

    return cblk;
}
