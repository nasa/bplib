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

#include "v7_encode_internal.h"

/*
 * -----------------------------------------------------------------------------------
 * IMPLEMENTATION
 * Helpers for encoding/decoding of individual protocol elements
 * -----------------------------------------------------------------------------------
 */

int v7_encoder_mpstream_write(void *arg, const void *ptr, size_t sz)
{
    if (bplib_mpool_stream_write(arg, ptr, sz) < sz)
    {
        return BP_ERROR;
    }

    return BP_SUCCESS;
}

int v7_encoder_write_crc(v7_encode_state_t *enc)
{
    uint8_t     crc_data[1 + sizeof(bp_crcval_t)];
    uint8_t     i;
    uint8_t     crc_len;
    bp_crcval_t crc_final;
    uint8_t    *pb;

    crc_len   = bplib_crc_get_width(enc->crc_params) / 8;
    crc_final = bplib_crc_finalize(enc->crc_params, enc->crc_val);
    pb        = &crc_data[crc_len];

    for (i = 0; i < crc_len; ++i)
    {
        --pb;
        *pb = crc_final & 0xFF;
        crc_final >>= 8;
    }

    return enc->next_writer(enc->next_writer_arg, crc_data, crc_len);
}

int v7_encoder_write_wrapper(void *arg, const void *ptr, size_t sz)
{
    v7_encode_state_t *enc = arg;
    int                write_result;
    size_t             crc_len;

    crc_len = bplib_crc_get_width(enc->crc_params) / 8;
    if (enc->crc_params)
    {
        enc->crc_val = bplib_crc_update(enc->crc_params, enc->crc_val, ptr, sz);
    }

    write_result = enc->next_writer(enc->next_writer_arg, ptr, sz - crc_len);
    if (enc->crc_flag)
    {
        /* write the actual crc value, instead of writing the passed-in string (which is 0-padded) */
        enc->crc_flag = false;
        write_result  = v7_encoder_write_crc(enc);
    }

    if (write_result != BP_SUCCESS)
    {
        return write_result;
    }

    enc->total_bytes_encoded += sz;

    return 0;
}

static void v7_encode_setup(v7_encode_state_t *enc, QCBOREncodeContext *top_level_cbor, bp_crctype_t crctype,
                            v7_chunk_writer_func_t next_writer, void *next_writer_arg)
{
    memset(enc, 0, sizeof(*enc));
    
    enc->cbor = top_level_cbor;

    enc->crc_params = v7_codec_get_crc_algorithm(crctype);
    enc->crc_val    = bplib_crc_initial_value(enc->crc_params);

    enc->next_writer     = next_writer;
    enc->next_writer_arg = next_writer_arg;
}

int v7_block_encode_pri(bplib_mpool_bblock_primary_t *cpb)
{
    v7_encode_state_t         v7_state;
    bplib_mpool_stream_t      mps;
    QCBOREncodeContext        top_level_enc;
    const bp_primary_block_t *pri;
    MakeUsefulBufOnStack(scratch_buf, BPLIB_STANDARD_BUF_LENGTH);
    UsefulBufC                encodedCBOR;
    QCBORError                err;

    /* If there is any existing encoded data, return it to the pool */
    bplib_mpool_bblock_primary_drop_encode(cpb);

    pri = bplib_mpool_bblock_primary_get_logical(cpb);

    bplib_mpool_start_stream_init(&mps, bplib_mpool_get_parent_pool_from_link(&cpb->chunk_list),
                                  bplib_mpool_stream_dir_write);
    
    QCBOREncode_Init(&top_level_enc, scratch_buf);
    
    v7_encode_setup(&v7_state, &top_level_enc, pri->crctype, v7_encoder_mpstream_write, &mps);

    v7_encode_bp_primary_block(&v7_state, pri);
    
    err = QCBOREncode_Finish(&top_level_enc, &encodedCBOR);
    if (err != QCBOR_SUCCESS)
    {
        v7_state.error = true;
    }
    
    if (v7_state.crc_flag == true)
    {
        v7_encoder_write_wrapper(&v7_state, encodedCBOR.ptr, encodedCBOR.len);
    }
    
    if (!v7_state.error)
    {
        cpb->block_encode_size_cache = bplib_mpool_stream_tell(&mps);
        bplib_mpool_stream_attach(&mps, bplib_mpool_bblock_primary_get_encoded_chunks(cpb));
    }

    bplib_mpool_stream_close(&mps);

    if (v7_state.error)
    {
        return -1;
    }
    return 0;
}

int v7_block_encode_pay(bplib_mpool_bblock_canonical_t *ccb, const void *data_ptr, size_t data_size)
{
    v7_encode_state_t                  v7_state;
    bplib_mpool_stream_t               mps;
    QCBOREncodeContext                 top_level_enc;
    const bp_canonical_block_buffer_t *pay;
    size_t                             data_encoded_offset;
    bplib_mpool_t                     *ppool;
    MakeUsefulBufOnStack(scratch_buf, BPLIB_STANDARD_BUF_LENGTH);
    UsefulBufC encodedCBOR;
    QCBORError err;

    /* If there is any existing encoded data, return it to the pool */
    bplib_mpool_bblock_canonical_drop_encode(ccb);

    ppool = bplib_mpool_get_parent_pool_from_link(&ccb->chunk_list);

    pay = bplib_mpool_bblock_canonical_get_logical(ccb);
    bplib_mpool_start_stream_init(&mps, ppool, bplib_mpool_stream_dir_write);
    
    QCBOREncode_Init(&top_level_enc, scratch_buf);
    
    v7_encode_setup(&v7_state, &top_level_enc, pay->canonical_block.crctype, v7_encoder_mpstream_write, &mps);

    v7_encode_bp_canonical_block_buffer(&v7_state, pay, data_ptr, data_size, &data_encoded_offset);
    err = QCBOREncode_Finish(&top_level_enc, &encodedCBOR);
    if (err != QCBOR_SUCCESS)
    {
        v7_state.error = true;
    }
    
    if (v7_state.crc_flag == true)
    {
        v7_encoder_write_wrapper(&v7_state, encodedCBOR.ptr, encodedCBOR.len);
    }    

    if (!v7_state.error)
    {
        bplib_mpool_bblock_canonical_set_content_position(ccb, data_encoded_offset, data_size);
        ccb->block_encode_size_cache = bplib_mpool_stream_tell(&mps);
        bplib_mpool_stream_attach(&mps, bplib_mpool_bblock_canonical_get_encoded_chunks(ccb));
    }

    bplib_mpool_stream_close(&mps);

    if (v7_state.error)
    {
        return -1;
    }
    return 0;
}

int v7_block_encode_canonical(bplib_mpool_bblock_canonical_t *ccb)
{
    v7_encode_state_t                  v7_state;
    bplib_mpool_stream_t               mps;
    QCBOREncodeContext                 top_level_enc;
    QCBOREncodeContext                 second_level_enc;
    const bp_canonical_block_buffer_t *logical;
    MakeUsefulBufOnStack(scratch_area, BPLIB_STANDARD_BUF_LENGTH);
    MakeUsefulBufOnStack(scratch_area2, BPLIB_STANDARD_BUF_LENGTH);
    size_t                             scratch_size;
    size_t                             content_encoded_offset;
    bplib_mpool_t                     *ppool;
    UsefulBufC                         encodedCBOR;
    UsefulBufC                         encodedCBOR2;
    QCBORError                   err;

    /* If there is any existing encoded data, return it to the pool */
    bplib_mpool_bblock_canonical_drop_encode(ccb);

    ppool = bplib_mpool_get_parent_pool_from_link(&ccb->chunk_list);

    /*
     * Encoding of a V7 extension block is a two-part affair, it must
     * first encode the block-specific information into CBOR, then wrap
     * those encoded octets as a CBOR byte string within the extension block.
     * This effectively makes it CBOR-in-CBOR.
     */
    logical = bplib_mpool_bblock_canonical_get_logical(ccb);

    memset(&v7_state, 0, sizeof(v7_state));
    QCBOREncode_Init(&second_level_enc, scratch_area2);

    v7_state.cbor = &second_level_enc;

    switch (logical->canonical_block.blockType)
    {
        case bp_blocktype_payloadBlock:
            break;
        case bp_blocktype_bundleAuthenicationBlock:
            break;
        case bp_blocktype_payloadIntegrityBlock:
            break;
        case bp_blocktype_payloadConfidentialityBlock:
            break;
        case bp_blocktype_previousHopInsertionBlock:
            break;
        case bp_blocktype_previousNode:
            v7_encode_bp_previous_node_block(&v7_state, &logical->data.previous_node_block);
            break;
        case bp_blocktype_bundleAge:
            v7_encode_bp_bundle_age_block(&v7_state, &logical->data.age_block);
            break;
        case bp_blocktype_metadataExtensionBlock:
            break;
        case bp_blocktype_extensionSecurityBlock:
            break;
        case bp_blocktype_hopCount:
            v7_encode_bp_hop_count_block(&v7_state, &logical->data.hop_count_block);
            break;
        case bp_blocktype_custodyTrackingBlock:
            v7_encode_bp_custody_tracking_block(&v7_state, &logical->data.custody_tracking_block);
            break;
        case bp_blocktype_adminRecordPayloadBlock:
        case bp_blocktype_custodyAcceptPayloadBlock:
            v7_encode_bp_admin_record_payload(&v7_state, logical);
            break;
        default:
            /* do nothing */
            break;
    }
    scratch_size = second_level_enc.OutBuf.data_len;
    
    QCBOREncode_Init(&top_level_enc, scratch_area);
    v7_state.cbor = &top_level_enc;
    if (!v7_state.error)
    {
        if (scratch_size > 0)
        {
            /*
             * Do second-stage encode - take the scratch buffer and use it as the content of the extension block
             */
            bplib_mpool_start_stream_init(&mps, ppool, bplib_mpool_stream_dir_write);
            v7_encode_setup(&v7_state, &top_level_enc, logical->canonical_block.crctype, v7_encoder_mpstream_write,
                            &mps);

            v7_encode_bp_canonical_block_buffer(&v7_state, logical, second_level_enc.OutBuf.UB.ptr, scratch_size,
                                                &content_encoded_offset);
            
            err = QCBOREncode_Finish(&second_level_enc, &encodedCBOR2);
            if ( err!= QCBOR_SUCCESS)
            {
                v7_state.error = true;
            }
            
            err = QCBOREncode_Finish(&top_level_enc, &encodedCBOR);
            if ( err!= QCBOR_SUCCESS)
            {
                v7_state.error = true;
            }
    
            if (v7_state.crc_flag == true)
            {
                v7_encoder_write_wrapper(&v7_state, encodedCBOR.ptr, encodedCBOR.len);
            }
            
            if (!v7_state.error)
            {
                bplib_mpool_bblock_canonical_set_content_position(ccb, content_encoded_offset, scratch_size);
                ccb->block_encode_size_cache = bplib_mpool_stream_tell(&mps);
                bplib_mpool_stream_attach(&mps, bplib_mpool_bblock_canonical_get_encoded_chunks(ccb));
            }

            bplib_mpool_stream_close(&mps);
        }
    }
    
    if (v7_state.error)
    {
        return -1;
    }
    return 0;
}
