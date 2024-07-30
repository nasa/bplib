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

#include "v7_codec_internal.h"

bplib_crc_parameters_t *v7_codec_get_crc_algorithm(bp_crctype_t crctype)
{
    bplib_crc_parameters_t *result;

    switch (crctype)
    {
        case bp_crctype_CRC16:
            result = &BPLIB_CRC16_X25;
            break;
        case bp_crctype_CRC32C:
            result = &BPLIB_CRC32_CASTAGNOLI;
            break;
        default:
            result = &BPLIB_CRC_NONE;
            break;
    }

    return result;
}

size_t v7_sum_preencoded_size(bplib_mpool_block_t *list)
{
    bplib_mpool_block_t *blk;
    size_t               size_sum;

    size_sum = 0;
    blk      = list;
    while (true)
    {
        blk = bplib_mpool_get_next_block(blk);
        if (bplib_mpool_is_list_head(blk))
        {
            break;
        }
        size_sum += bplib_mpool_get_user_content_size(blk);
    }

    return size_sum;
}

size_t v7_compute_full_bundle_size(bplib_mpool_bblock_primary_t *cpb)
{
    bplib_mpool_block_t            *blk;
    bplib_mpool_bblock_canonical_t *ccb;
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
            last_encode = v7_block_encode_pri(cpb);
        }
        else
        {
            last_encode = 0;
        }
        sum_result = cpb->block_encode_size_cache;
        blk        = bplib_mpool_bblock_primary_get_canonical_list(cpb);
        while (last_encode == 0 && blk)
        {
            blk = bplib_mpool_get_next_block(blk);
            ccb = bplib_mpool_bblock_canonical_cast(blk);
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
                last_encode = v7_block_encode_canonical(ccb);
            }

            sum_result += ccb->block_encode_size_cache;
        }
    }

    return cpb->bundle_encode_size_cache;
}

size_t v7_copy_full_bundle_out(bplib_mpool_bblock_primary_t *cpb, void *buffer, size_t buf_sz)
{
    size_t                          remain_sz;
    size_t                          chunk_sz;
    uint8_t                        *out_p;
    bplib_mpool_block_t            *cblk;
    bplib_mpool_bblock_canonical_t *ccb;

    /*
     * two bytes is just the overhead added by this routine.  It is definitely not enough
     * for a real bundle, this just avoids buffer bounds violation in here.
     */
    if (buf_sz < 2)
    {
        return 0;
    }

    out_p  = buffer;
    *out_p = 0x9F; /* Start CBOR indefinite-length array */
    ++out_p;
    
    remain_sz = buf_sz - 2;

    chunk_sz =
        bplib_mpool_bblock_cbor_export(bplib_mpool_bblock_primary_get_encoded_chunks(cpb), out_p, remain_sz, 0, -1);
    out_p += chunk_sz;
    remain_sz -= chunk_sz;
    cblk = bplib_mpool_bblock_primary_get_canonical_list(cpb);
    while (remain_sz > 0)
    {
        cblk = bplib_mpool_get_next_block(cblk);
        ccb  = bplib_mpool_bblock_canonical_cast(cblk);
        if (ccb == NULL)
        {
            break;
        }
        chunk_sz = bplib_mpool_bblock_cbor_export(bplib_mpool_bblock_canonical_get_encoded_chunks(ccb), out_p,
                                                  remain_sz, 0, -1);
        out_p += chunk_sz;
        remain_sz -= chunk_sz;
    }

    /* there should always be enough space for this, because it was accounted for at the beginning */
    *out_p = 0xFF; /* End CBOR indefinite-length array (break code) */
    ++out_p;

    return (out_p - (uint8_t *)buffer);
}

size_t v7_copy_full_bundle_in(bplib_mpool_bblock_primary_t *cpb, const void *buffer, size_t buf_sz)
{
    size_t         remain_sz;
    size_t         chunk_sz;
    const uint8_t *in_p;
    bp_blocktype_t payload_block_hint;
    bplib_mpool_t *ppool;

    bplib_mpool_block_t            *cblk;
    bplib_mpool_bblock_canonical_t *ccb;

    /* get the parent pool (will be needed for block allocs later) */
    ppool = bplib_mpool_get_parent_pool_from_link(&cpb->chunk_list);

    /* In case the bundle had any data with it, drop it now */
    /* note this sets cpb->block_encode_size_cache to 0 */
    bplib_mpool_bblock_primary_drop_encode(cpb);

    /* also drop any existing canonical blocks */
    if (bplib_mpool_is_nonempty_list_head(&cpb->cblock_list))
    {
        bplib_mpool_recycle_all_blocks_in_list(NULL, &cpb->cblock_list);
    }

    /*
     * two bytes is just the overhead added by this routine.  It is definitely not enough
     * for a real bundle, this just avoids buffer bounds violation in here.
     */
    if (buf_sz < 2)
    {
        return 0;
    }

    in_p = buffer;
    if (*in_p != 0x9F) /* CBOR indefinite-length array */
    {
        /* not well formed BP */
        return 0;
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
    payload_block_hint = bp_blocktype_undefined;

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
            if (v7_block_decode_pri(cpb, in_p, remain_sz) < 0)
            {
                /* fail to decode */
                break;
            }

            chunk_sz = cpb->block_encode_size_cache;

            /* if the block is an admin record, this determines how to interpret the payload */
            if (cpb->data.logical.controlFlags.isAdminRecord)
            {
                payload_block_hint = bp_blocktype_adminRecordPayloadBlock;
            }
        }
        else
        {
            /* Anything beyond first block is always a canonical block */

            /* Allocate Blocks */
            cblk = bplib_mpool_bblock_canonical_alloc(ppool, 0, NULL);
            ccb  = bplib_mpool_bblock_canonical_cast(cblk);
            if (ccb == NULL)
            {
                /* no mem */
                break;
            }

            /* Preemptively store it; the whole chain will be discarded if decode fails */
            bplib_mpool_bblock_primary_append(cpb, cblk);

            /* Decode Canonical/Payload Block */
            if (v7_block_decode_canonical(ccb, in_p, remain_sz, payload_block_hint) < 0)
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
                case bp_blocktype_payloadConfidentialityBlock:
                    /* bpsec not implemented yet, but this is the idea */
                    if (payload_block_hint == bp_blocktype_undefined)
                    {
                        payload_block_hint = bp_blocktype_ciphertextPayloadBlock;
                    }
                    break;

                case bp_blocktype_custodyTrackingBlock:
                    /* if this block is present it requests full custody tracking */
                    cpb->data.delivery.delivery_policy = bplib_policy_delivery_custody_tracking;
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
     * context.  So the size is returned, so the caller can decide.
     */
    if (cpb == NULL || *in_p != 0xFF) /* CBOR break code */
    {
        return 0;
    }

    ++in_p;
    cpb->bundle_encode_size_cache = (in_p - (uint8_t *)buffer);

    return cpb->bundle_encode_size_cache;
}
