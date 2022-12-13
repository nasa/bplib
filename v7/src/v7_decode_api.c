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

#include "v7_decode_internal.h"

/*
 * Decoding helper to save and validate/verify a complete block after decode.
 *
 * Initial decoding of a block will de-construct the CBOR stream into its logical elements, including the CRC,
 * as well as determining the encoded size of the block.  Note the CLA it will only know the size of the
 * bundle, not the blocks inside of it. In order to determine the latter, the CBOR must be decoded.  In
 * addition to the size, the CRC type, its value, and furthermore its position in the encoded stream
 * will also be known.
 *
 * Once this information is known, the block needs to be saved to the storage service.
 *
 * This is the best time to validate the CRC, too - because CRC can't be validated until it is found in the
 * CBOR stream, but after decoding it is too late to check it because it needs to be zero-filled for the check.
 * (hence a sort of circular dependency).
 *
 * This function will save the encoded CBOR block data to the storage service, and validate the
 * CRC of the data in the process of doing so.
 *
 * Returns the actual size saved to the storage service.  If the CRC fails to validate, this returns 0,
 * and nothing is saved to the storage service.
 */
static size_t v7_save_and_verify_block(bplib_mpool_block_t *head, const uint8_t *block_base, size_t block_size,
                                       bp_crctype_t crc_type, bp_crcval_t crc_check);

/*
 * -----------------------------------------------------------------------------------
 * IMPLEMENTATION
 * Helpers for encoding/decoding of individual protocol elements
 * -----------------------------------------------------------------------------------
 */

size_t v7_save_and_verify_block(bplib_mpool_block_t *head, const uint8_t *block_base, size_t block_size,
                                bp_crctype_t crc_type, bp_crcval_t crc_check)
{
    static const uint8_t    ZERO_BYTES[4] = {0};
    size_t                  crc_len;
    bplib_crc_parameters_t *crc_params;
    bp_crcval_t             crc_val;
    bplib_mpool_stream_t    mps;
    size_t                  result;

    result = 0;
    bplib_mpool_start_stream_init(&mps, bplib_mpool_get_parent_pool_from_link(head), bplib_mpool_stream_dir_write);
    crc_params = v7_codec_get_crc_algorithm(crc_type);
    crc_len    = bplib_crc_get_width(crc_params) / 8;
    crc_val    = bplib_crc_initial_value(crc_params);
    if (crc_len < block_size && crc_len <= sizeof(ZERO_BYTES))
    {
        /* copy the entire block including original (still unverified) CRC to the buffer */
        if (bplib_mpool_stream_write(&mps, block_base, block_size) == block_size)
        {
            /* calculate the CRC value locally */
            crc_val = bplib_crc_update(crc_params, crc_val, block_base, block_size - crc_len);

            /* need to pump in zero bytes for CRC width */
            crc_val = bplib_crc_update(crc_params, crc_val, ZERO_BYTES, crc_len);
            crc_val = bplib_crc_finalize(crc_params, crc_val);

            if (crc_val == crc_check)
            {
                result = block_size;
                bplib_mpool_stream_attach(&mps, head);
            }
        }
    }

    bplib_mpool_stream_close(&mps);

    return result;
}

int v7_block_decode_pri(bplib_mpool_bblock_primary_t *cpb, const void *data_ptr, size_t data_size)
{
    v7_decode_state_t   v7_state;
    CborValue           origin;
    bp_primary_block_t *pri;
    size_t              block_size;
    CborParser          parser;

    /* If there is any existing encoded data, return it to the pool */
    bplib_mpool_bblock_primary_drop_encode(cpb);

    pri = bplib_mpool_bblock_primary_get_logical(cpb);
    memset(&v7_state, 0, sizeof(v7_state));

    if (cbor_parser_init(data_ptr, data_size, 0, &parser, &origin) != CborNoError)
    {
        v7_state.error = true;
    }
    else
    {
        v7_state.base = data_ptr;
        v7_state.cbor = &origin;

        v7_decode_bp_primary_block(&v7_state, pri);
    }

    if (!v7_state.error)
    {
        block_size                   = cbor_value_get_next_byte(&origin) - v7_state.base;
        cpb->block_encode_size_cache = v7_save_and_verify_block(bplib_mpool_bblock_primary_get_encoded_chunks(cpb),
                                                                v7_state.base, block_size, pri->crctype, pri->crcval);

        if (cpb->block_encode_size_cache != block_size)
        {
            /* something went wrong in validation */
            v7_state.error = true;
        }
    }

    if (v7_state.error)
    {
        return -1;
    }
    return 0;
}

int v7_block_decode_canonical(bplib_mpool_bblock_canonical_t *ccb, const void *data_ptr, size_t data_size,
                              bp_blocktype_t payload_block_hint)
{
    v7_decode_state_t            v7_state;
    CborError                    tcb_stat;
    CborValue                    origin;
    bp_canonical_block_buffer_t *logical;
    size_t                       block_size;
    CborParser                   parser;
    size_t                       content_offset;
    size_t                       content_size;

    block_size     = 0;
    content_offset = 0;
    content_size   = 0;

    /* If there is any existing encoded data, return it to the pool */
    bplib_mpool_bblock_canonical_drop_encode(ccb);

    logical = bplib_mpool_bblock_canonical_get_logical(ccb);
    memset(&v7_state, 0, sizeof(v7_state));

    tcb_stat = cbor_parser_init(data_ptr, data_size, 0, &parser, &origin);
    if (tcb_stat != CborNoError)
    {
        v7_state.error = true;
    }
    else
    {
        v7_state.base = data_ptr;
        v7_state.cbor = &origin;

        v7_decode_bp_canonical_block_buffer(&v7_state, logical, &content_offset, &content_size);
    }

    if (!v7_state.error)
    {
        /* This reflects the size of the entire CBOR blob that the caller passed in */
        block_size = cbor_value_get_next_byte(&origin) - v7_state.base;

        /* Copy it to the pool buffers, and check the CRC in the process */
        ccb->block_encode_size_cache =
            v7_save_and_verify_block(bplib_mpool_bblock_canonical_get_encoded_chunks(ccb), v7_state.base, block_size,
                                     logical->canonical_block.crctype, logical->canonical_block.crcval);

        if (ccb->block_encode_size_cache != block_size)
        {
            v7_state.error = true;
        }
        else
        {
            bplib_mpool_bblock_canonical_set_content_position(ccb, content_offset, content_size);
        }
    }

    if (!v7_state.error)
    {
        /*
         * multiple different block types may get labeled as the "payload block"
         * because RFC9171 insists that something must be labeled as such.
         * the purpose of the "payload_block_hint" is to identify how the payload
         * should really be interpreted based on other blocks/fields in the bundle
         */
        if (payload_block_hint != bp_blocktype_undefined &&
            logical->canonical_block.blockType == bp_blocktype_payloadBlock)
        {
            logical->canonical_block.blockType = payload_block_hint;
        }

        /*
         * Second stage decode - for recognized non-payload extension blocks
         * This should NOT be done for regular payloadb blocks - which may or
         * may not be CBOR-encoded - thus undefined to call "cbor_parser_init" on
         * this block.
         */
        if (logical->canonical_block.blockType != bp_blocktype_payloadBlock)
        {
            v7_state.base += content_offset;
            tcb_stat = cbor_parser_init(v7_state.base, content_size, 0, &parser, &origin);
            if (tcb_stat != CborNoError)
            {
                v7_state.error = true;
            }
            else
            {
                v7_state.cbor = &origin;

                switch (logical->canonical_block.blockType)
                {
                    case bp_blocktype_bundleAuthenicationBlock:
                        break;
                    case bp_blocktype_payloadIntegrityBlock:
                        break;
                    case bp_blocktype_payloadConfidentialityBlock:
                        break;
                    case bp_blocktype_previousHopInsertionBlock:
                        break;
                    case bp_blocktype_previousNode:
                        v7_decode_bp_previous_node_block(&v7_state, &logical->data.previous_node_block);
                        break;
                    case bp_blocktype_bundleAge:
                        v7_decode_bp_bundle_age_block(&v7_state, &logical->data.age_block);
                        break;
                    case bp_blocktype_metadataExtensionBlock:
                        break;
                    case bp_blocktype_extensionSecurityBlock:
                        break;
                    case bp_blocktype_hopCount:
                        v7_decode_bp_hop_count_block(&v7_state, &logical->data.hop_count_block);
                        break;
                    case bp_blocktype_custodyTrackingBlock:
                        v7_decode_bp_custody_tracking_block(&v7_state, &logical->data.custody_tracking_block);
                        break;
                    case bp_blocktype_adminRecordPayloadBlock:
                    case bp_blocktype_custodyAcceptPayloadBlock:
                        v7_decode_bp_admin_record_payload(&v7_state, logical);
                        break;
                    default:
                        /* do nothing */
                        break;
                }
            }
        }
    }

    if (v7_state.error)
    {
        return -1;
    }
    return 0;
}
