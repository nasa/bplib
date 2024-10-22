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

#include <stdio.h>
#include "bplib_api_types.h"
#include <assert.h>

#include "bplib_mem.h"
#include "bplib_time.h"

#include "bplib_stor_cache.h"
#include "bplib_stor_cache_block.h"
#include "bplib_stor_cache_internal.h"

#include "bplib_stor_qm_encode.h"
#include "bplib_stor_qm_decode.h"

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockPrimaryCast
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_BblockPrimary_t *BPLib_STOR_CACHE_BblockPrimaryCast(BPLib_STOR_CACHE_Block_t *cb)
{
        BPLib_STOR_CACHE_BlockContent_t *content;

    content = BPLib_STOR_CACHE_BlockDereferenceContent(cb);
    if (content != NULL && content->header.base_link.type == BPLib_STOR_CACHE_BlocktypePrimary)
    {
        return &content->u.primary.pblock;
    }
    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockCanonicalCast
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_BblockCanonical_t *BPLib_STOR_CACHE_BblockCanonicalCast(BPLib_STOR_CACHE_Block_t *cb)
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
void *BPLib_STOR_CACHE_BblockCborCast(BPLib_STOR_CACHE_Block_t *cb)
{
    /* CBOR data blocks are nothing more than generic blocks with a different sig */
    return BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)cb, BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockCborSetSize
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_BblockCborSetSize(BPLib_STOR_CACHE_Block_t *cb, size_t user_content_size)
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
void BPLib_STOR_CACHE_BblockPrimaryInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_BblockPrimary_t *pblk)
{
    BPLib_STOR_CACHE_InitListHead(base_block, &pblk->cblock_list);
    BPLib_STOR_CACHE_InitListHead(base_block, &pblk->chunk_list);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockCanonicalInit
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_BblockCanonicalInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_BblockCanonical_t *cblk)
{
    BPLib_STOR_CACHE_InitListHead(base_block, &cblk->chunk_list);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockPrimaryAlloc
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BblockPrimaryAlloc(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number,
                                                              void *init_arg, uint8_t priority, BPLib_TIME_MonotonicTime_t timeout)
{
    BPLib_STOR_CACHE_BlockContent_t *result;
    BPLib_MEM_Lock_t                *lock;
    bool                             within_timeout;

    lock           = BPLib_MEM_LockResource(pool);
    within_timeout = true;
    while (true)
    {
        result = BPLib_STOR_CACHE_AllocBlockInternal(pool, BPLib_STOR_CACHE_BlocktypePrimary, magic_number, init_arg, priority);
        if (result != NULL || !within_timeout)
        {
            break;
        }

        within_timeout = BPLib_MEM_LockWait(lock, timeout);
    }
    BPLib_MEM_LockRelease(lock);
    return (BPLib_STOR_CACHE_Block_t *)result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockCanonicalAlloc
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BblockCanonicalAlloc(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number, void *init_arg)
{
    BPLib_STOR_CACHE_BlockContent_t *result;
    BPLib_MEM_Lock_t          *lock;

    lock   = BPLib_MEM_LockResource(pool);
    result = BPLib_STOR_CACHE_AllocBlockInternal(pool, BPLib_STOR_CACHE_BlocktypeCanonical, magic_number, init_arg,
                                              BPLIB_MEM_ALLOC_PRI_MED);
    BPLib_MEM_LockRelease(lock);

    return (BPLib_STOR_CACHE_Block_t *)result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockCborAlloc
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BblockCborAlloc(BPLib_STOR_CACHE_Pool_t *pool)
{
    BPLib_STOR_CACHE_BlockContent_t *result;
    BPLib_MEM_Lock_t          *lock;

    lock   = BPLib_MEM_LockResource(pool);
    result = BPLib_STOR_CACHE_AllocBlockInternal(pool, BPLib_STOR_CACHE_BlocktypeGeneric, BPLIB_MEM_CACHE_CBOR_DATA_SIGNATURE,
                                              NULL, BPLIB_MEM_ALLOC_PRI_MED);
    BPLib_MEM_LockRelease(lock);

    return (BPLib_STOR_CACHE_Block_t *)result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_BblockCborAppend
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_BblockCborAppend(BPLib_STOR_CACHE_Block_t *head, BPLib_STOR_CACHE_Block_t *blk)
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
    BPLib_STOR_CACHE_Block_t *elist;

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
    BPLib_STOR_CACHE_Block_t *elist;

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
size_t BPLib_STOR_CACHE_BblockCborExport(BPLib_STOR_CACHE_Block_t *list, void *out_ptr, size_t max_out_size, size_t seek_start,
                                      size_t max_count)
{
    BPLib_STOR_CACHE_Block_t *blk;
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
void BPLib_STOR_CACHE_BblockPrimaryAppend(BPLib_STOR_CACHE_BblockPrimary_t *cpb, BPLib_STOR_CACHE_Block_t *blk)
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

BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_BblockPrimaryLocateCanonical(BPLib_STOR_CACHE_BblockPrimary_t *cpb,
                                                                 BPLib_STOR_CACHE_Blocktype_t                block_type)
{
    BPLib_STOR_CACHE_Block_t            *cblk;
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

size_t BPLib_STOR_CACHE_SumPrencodedSize(BPLib_STOR_CACHE_Block_t *list)
{
    BPLib_STOR_CACHE_Block_t *blk;
    size_t               size_sum;

    size_sum = 0;
    blk      = list;
    while (true)
    {
        blk = BPLib_STOR_CACHE_GetNextBlock(blk);
        if (BPLib_STOR_CACHE_IsListHead(blk))
        {
            break;
        }
        size_sum += BPLib_STOR_CACHE_GetUserContentSize(blk);
    }

    return size_sum;
}

size_t BPLib_STOR_CACHE_ComputeFullBundleSize(BPLib_STOR_CACHE_BblockPrimary_t *cpb)
{
    BPLib_STOR_CACHE_Block_t            *blk;
    BPLib_STOR_CACHE_BblockCanonical_t *ccb;
    int                             last_encode;
    size_t                          sum_result;

    if (cpb->bundle_encode_size_cache == 0)
    {
        /*
         * For any block which is not already encoded, this needs to encode it now.
         * For most blocks all the information is in the logical data, so it is possible
         * to do this on the fly.
         *
         * However the payload block MUST be pre-encoded because the information is
         * not available in the logical data.  The payload content only exists in encoded
         * form.
         */
        if (cpb->block_encode_size_cache == 0)
        {
            last_encode = block_encode_pri(cpb);
        }
        else
        {
            last_encode = 0;
        }
        sum_result = cpb->block_encode_size_cache;
        blk        = BPLib_STOR_CACHE_BblockPrimaryGetCanonicalList(cpb);
        while (last_encode == 0)
        {
            blk = BPLib_STOR_CACHE_GetNextBlock (blk);
            ccb = BPLib_STOR_CACHE_BblockCanonicalCast(blk);
            if (ccb == NULL)
            {
                /*
                 * no more blocks... this is the normal stop condition.
                 *
                 * This adds 2 extra bytes here because the complete bundle is supposed to be wrapped
                 * inside a CBOR indefinite-length array, which adds one octet at the beginning
                 * and one octet at the end.
                 */
                cpb->bundle_encode_size_cache = 2 + sum_result;
                break;
            }

            if (ccb->block_encode_size_cache == 0)
            {
                last_encode = block_encode_canonical(ccb);
            }

            sum_result += ccb->block_encode_size_cache;
        }
    }

    return cpb->bundle_encode_size_cache;
}

size_t BPLib_STOR_CACHE_CopyFullBundleOut(BPLib_STOR_CACHE_BblockPrimary_t *cpb, void *buffer, size_t buf_sz)
{
    size_t                              remain_sz;
    size_t                              chunk_sz;
    uint8_t                            *out_p;
    BPLib_STOR_CACHE_Block_t           *cblk;
    BPLib_STOR_CACHE_BblockCanonical_t *ccb;

    /*
     * two bytes is just the overhead added by this routine.  It is definitely not enough
     * for a real bundle, this just avoids buffer bounds violation in here.
     */
    if (buf_sz < 2)
    {
        return 0; // Returned bundle count of zero for too small buffer size.
    }

    out_p  = buffer;
    *out_p = 0x9F; /* Start CBOR indefinite-length array */
    ++out_p;

    remain_sz = buf_sz - 2;

    chunk_sz =
        BPLib_STOR_CACHE_BblockCborExport(BPLib_STOR_CACHE_BblockPrimaryGetEncodedChunks(cpb), out_p, remain_sz, 0, -1);
    out_p += chunk_sz;
    remain_sz -= chunk_sz;
    cblk = BPLib_STOR_CACHE_BblockPrimaryGetCanonicalList(cpb);
    while (remain_sz > 0)
    {
        cblk = BPLib_STOR_CACHE_GetNextBlock(cblk);
        ccb  = BPLib_STOR_CACHE_BblockCanonicalCast(cblk);
        if (ccb == NULL)
        {
            break;
        }
        chunk_sz = BPLib_STOR_CACHE_BblockCborExport(BPLib_STOR_CACHE_BblockCanonicalGetEncodedChunks(ccb), out_p,
                                                  remain_sz, 0, -1);
        out_p += chunk_sz;
        remain_sz -= chunk_sz;
    }

    /* there should always be enough space for this, because it was accounted for at the beginning */
    *out_p = 0xFF; /* End CBOR indefinite-length array (break code) */
    ++out_p;

    return (out_p - (uint8_t *)buffer);
}

size_t BPLib_STOR_CACHE_CopyFullBundleIn(BPLib_STOR_CACHE_BblockPrimary_t *cpb, const void *buffer, size_t buf_sz)
{
    size_t         remain_sz;
    size_t         chunk_sz;
    const uint8_t *in_p;
    BPLib_STOR_CACHE_Blocktype_t payload_block_hint;
    BPLib_STOR_CACHE_Pool_t *ppool;

    BPLib_STOR_CACHE_Block_t           *cblk;
    BPLib_STOR_CACHE_BblockCanonical_t *ccb;

    /* get the parent pool (will be needed for block allocs later) */
    ppool = BPLib_STOR_CACHE_GetParentPoolFromLink(&cpb->chunk_list);

    /* In case the bundle had any data with it, drop it now */
    /* note this sets cpb->block_encode_size_cache to 0 */
    BPLib_STOR_CACHE_BblockPrimaryDropEncode(cpb);

    /* also drop any existing canonical blocks */
    if (BPLib_STOR_CACHE_IsNonemptyListHead(&cpb->cblock_list))
    {
        BPLib_STOR_CACHE_RecycleAllBlocksInList(NULL, &cpb->cblock_list);
    }

    /*
     * two bytes is just the overhead added by this routine.  It is definitely not enough
     * for a real bundle, this just avoids buffer bounds violation in here.
     */
    if (buf_sz < 2)
    {
        return 0; // Bundle count is zero on error.
    }

    in_p = buffer;
    if (*in_p != 0x9F) /* CBOR indefinite-length array */
    {
        /* not well formed BP */
        return 0; // Bundle count is zero on error.
    }

    ++in_p;
    remain_sz = buf_sz - 2;

    /*
     * This is to "undo" / invert of the encoding side logic where
     * fixed-content/known blocks need to be labeled as the generic "payload"
     * block in order to satisfy RFC9171 requirement that all bundles must
     * have a block that is of type 1 (payload).
     *
     * In this case, the presence of certain extension blocks will indicate
     * how the payload block should really be interpreted.
     */
    payload_block_hint = BPLib_STOR_CACHE_BlocktypeUndefined;

    /*
     * From this point forward, any allocated blocks will need to
     * be returned if the process fails, in order to not be leaked.
     */
    do
    {
        if (cpb->block_encode_size_cache == 0)
        {
            /* First block is always a primary block */
            /* Decode Primary Block */
            if (BPLib_STOR_QM_DecodePrimary(cpb, in_p, remain_sz) < 0)
            {
                /* fail to decode */
                break;
            }

            chunk_sz = cpb->block_encode_size_cache;

            /* if the block is an admin record, this determines how to interpret the payload */
            if (cpb->data.logical.controlFlags.isAdminRecord)
            {
                payload_block_hint = BPLib_STOR_CACHE_BlocktypeAdminRecordPayloadBlock;
            }
        }
        else
        {
            /* Anything beyond first block is always a canonical block */

            /* Allocate Blocks */
            cblk = BPLib_STOR_CACHE_BblockCanonicalAlloc(ppool, 0, NULL);
            ccb  = BPLib_STOR_CACHE_BblockCanonicalCast(cblk);
            if (ccb == NULL)
            {
                /* no mem */
                break;
            }

            /* Preemptively store it; the whole chain will be discarded if decode fails */
            BPLib_STOR_CACHE_BblockPrimaryAppend(cpb, cblk);

            /* Decode Canonical/Payload Block */
            if (BPLib_STOR_QM_DecodeCanonical(ccb, in_p, remain_sz, payload_block_hint) < 0)
            {
                /* fail to decode */
                break;
            }

            chunk_sz = ccb->block_encode_size_cache;

            /* check for certain special/known extension blocks that indicate how to interpret
             * the payload.  The presence (or not) of these blocks changes gives a hint as
             * to what the payload should be.  Since the payload block is last by definition,
             * the identifying extension block should always be found first.
             *
             * The challenge comes if more than one of these blocks exists in the same bundle,
             * it gets fuzzy how this should work.
             */
            switch (ccb->canonical_logical_data.canonical_block.blockType)
            {
                case BPLib_STOR_CACHE_BlocktypePayloadConfidentialityBlock:
                    /* bpsec not implemented yet, but this is the idea */
                    if (payload_block_hint == BPLib_STOR_CACHE_BlocktypeUndefined)
                    {
                        payload_block_hint = BPLib_STOR_CACHE_BlocktypeCiphertextPayloadBlock;
                    }
                    break;

                case BPLib_STOR_CACHE_BlocktypeCustodyTrackingBlock:
                    /* if this block is present it requests full custody tracking */
                    cpb->data.delivery.delivery_policy = BPLib_STOR_CACHE_PolicyDeliveryCustodyTracking;
                    break;

                default:
                    /* nothing to do */
                    break;
            }
        }

        in_p += chunk_sz;
        remain_sz -= chunk_sz;
    } while (remain_sz > 0 && *in_p != 0xFF); /* not CBOR break code */

    /*
     * This process should typically have consumed the entire bundle buffer.
     * If remain_sz != 0 at this point, it means there was some mismatch
     * between what the CLA saw as a bundle, verses what CBOR decoding
     * saw as a bundle.  This may or may not be an issue, may depend on
     * context.  So the size of zero is returned, so the caller can decide.
     */
    if (cpb == NULL || *in_p != 0xFF) /* CBOR break code */
    {
        return 0; // Bundle count is zero on error.
    }

    ++in_p;
    cpb->bundle_encode_size_cache = (in_p - (uint8_t *)buffer);

    return cpb->bundle_encode_size_cache;
}
