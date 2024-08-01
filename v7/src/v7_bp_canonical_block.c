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
#include "v7_encode_internal.h"

typedef enum bp_canonical_field
{
    bp_canonical_field_undef,
    bp_canonical_field_blocktype,
    bp_canonical_field_blocknum,
    bp_canonical_field_flags,
    bp_canonical_field_crctype,
    bp_canonical_field_info_content,
    bp_canonical_field_crcvalue,
    bp_canonical_field_done
} bp_canonical_field_t;

static const v7_bitmap_table_t V7_BLOCK_PROCESSING_FLAGS_BITMAP_TABLE[] = {
    {offsetof(bp_block_processing_flags_t, must_remove), 0x10},
    {offsetof(bp_block_processing_flags_t, must_delete), 0x04},
    {offsetof(bp_block_processing_flags_t, xmit_status), 0x02},
    {offsetof(bp_block_processing_flags_t, must_replicate), 0x01},
    {0, 0} /* end of table marker, keep last */
};

/*
 * -----------------------------------------------------------------------------------
 * IMPLEMENTATION
 * Helpers for encoding/decoding of individual protocol elements
 * -----------------------------------------------------------------------------------
 */
void v7_encode_bp_block_processing_flags(v7_encode_state_t *enc, const bp_block_processing_flags_t *v)
{
    v7_encode_bitmap(enc, (const uint8_t *)v, V7_BLOCK_PROCESSING_FLAGS_BITMAP_TABLE);
}

void v7_encode_bp_canonical_info(v7_encode_state_t *enc, const v7_canonical_block_info_t *info)
{
    /*
     * The content is wrapped as a CBOR byte string.
     * The data may itself be CBOR-encoded (so this may result as CBOR-in-CBOR)
     *
     * This encodes the data and snapshots the position of the END of that CBOR byte
     * string.
     */
    UsefulBufC bytes_buf;
    bytes_buf.ptr = info->content_vptr;
    bytes_buf.len = info->content_size;
    QCBOREncode_AddBytes(enc->cbor, bytes_buf);

}

void v7_encode_bp_canonical_bundle_block(v7_encode_state_t *enc, const bp_canonical_bundle_block_t *v,
                                         const v7_canonical_block_info_t *info)
{
    bp_canonical_field_t field_id = bp_canonical_field_undef;

    bp_blocktype_t encode_blocktype;

    /*
     * multiple different block types may get labeled as the "payload block"
     * because RFC9171 insists that something must be labeled as such.
     */
    switch (v->blockType)
    {
        case bp_blocktype_adminRecordPayloadBlock:
        case bp_blocktype_ciphertextPayloadBlock:
        case bp_blocktype_custodyAcceptPayloadBlock:
            /* This special block is fulfilling the RFC9171 "payload block" requirement -
             * there must be an extension block elsewhere in the bundle that helps the
             * decoder on the other side figure out what it really is */
            encode_blocktype = bp_blocktype_payloadBlock;
            break;
        default:
            encode_blocktype = v->blockType;
            break;
    }

    if (encode_blocktype < bp_blocktype_SPECIAL_BLOCKS_START)
    {
        while (field_id < bp_canonical_field_done && !enc->error)
        {
            switch (field_id)
            {
                case bp_canonical_field_blocktype:
                    v7_encode_bp_blocktype(enc, &encode_blocktype);
                    break;
                case bp_canonical_field_blocknum:
                    v7_encode_bp_blocknum(enc, &v->blockNum);
                    break;
                case bp_canonical_field_flags:
                    v7_encode_bp_block_processing_flags(enc, &v->processingControlFlags);
                    break;
                case bp_canonical_field_crctype:
                    v7_encode_bp_crctype(enc, &v->crctype);
                    break;
                case bp_canonical_field_info_content:
                    v7_encode_bp_canonical_info(enc, info);
                    break;
                case bp_canonical_field_crcvalue:
                    /* Attach the CRC if requested. */
                    if (v->crctype != bp_crctype_none)
                    {
                        v7_encode_crc(enc);
                    }
                    break;
                default:
                    break;
            }

            ++field_id;
        }
    }
    else
    {
        /* could not identify what type of RFC9171 block type to use */
        enc->error = true;
    }
}

static void v7_encode_bp_canonical_block_buffer_impl(v7_encode_state_t *enc, const void *arg)
{
    const v7_canonical_block_info_t *info = arg;
    v7_encode_bp_canonical_bundle_block(enc, info->encode_block, arg);
}

void v7_encode_bp_canonical_block_buffer(v7_encode_state_t *enc, const bp_canonical_block_buffer_t *v,
                                         const void *content_ptr, size_t content_length, size_t *content_encoded_offset)
{
    v7_canonical_block_info_t info;
    size_t                    num_fields;

    memset(&info, 0, sizeof(info));

    /* this also needs to predict the number of fields */
    num_fields = 5; /* fixed/required fields */
    if (v->canonical_block.crctype != bp_crctype_none)
    {
        ++num_fields; /* one more for CRC */
    }

    info.encode_block       = &v->canonical_block;
    info.content_offset_out = content_encoded_offset;
    info.content_vptr       = content_ptr;
    info.content_size       = content_length;

    v7_encode_container(enc, num_fields, v7_encode_bp_canonical_block_buffer_impl, &info);
}

void v7_decode_bp_block_processing_flags(v7_decode_state_t *dec, bp_block_processing_flags_t *v)
{
    v7_decode_bitmap(dec, (uint8_t *)v, V7_BLOCK_PROCESSING_FLAGS_BITMAP_TABLE);
}

void v7_decode_bp_canonical_info(v7_decode_state_t *dec, v7_canonical_block_info_t *info)
{
    
    QCBORItem item;
    QCBORError err;
    UsefulBufC buff;
    /*
     * The content within this context must be a CBOR byte string.
     * This byte string, in turn, _might_ have CBOR-encoded data within it,
     * but it won't be decoded right now, just grab the location and
     * make sure everything seems legit at the outer layer
     */
    err = QCBORDecode_PeekNext(dec->cbor, &item);
    if (err != QCBOR_SUCCESS || item.uDataType != QCBOR_TYPE_BYTE_STRING)
    {
        dec->error = true;
        return;
    }

    QCBORDecode_GetByteString(dec->cbor, &buff);
    /* Export the position of the now-located content information */
    *info->content_offset_out = (uint8_t*)buff.ptr - dec->base;
    info->content_vptr        = buff.ptr;
    info->content_size        = buff.len;   
}

void v7_decode_bp_canonical_bundle_block(v7_decode_state_t *dec, bp_canonical_bundle_block_t *v,
                                         v7_canonical_block_info_t *info)
{
    bp_canonical_field_t field_id = bp_canonical_field_undef;

    
    while (field_id < bp_canonical_field_done && !dec->error)
    {
        switch (field_id)
        {
            case bp_canonical_field_blocktype:
                v7_decode_bp_blocktype(dec, &v->blockType);
                break;
            case bp_canonical_field_blocknum:
                v7_decode_bp_blocknum(dec, &v->blockNum);
                break;
            case bp_canonical_field_flags:
                v7_decode_bp_block_processing_flags(dec, &v->processingControlFlags);
                break;
            case bp_canonical_field_crctype:
                v7_decode_bp_crctype(dec, &v->crctype);
                break;
            case bp_canonical_field_info_content:
                v7_decode_bp_canonical_info(dec, info);
                break;
            case bp_canonical_field_crcvalue:
                if (v->crctype != bp_crctype_none)
                {
                    v7_decode_crc(dec, &v->crcval);
                }
                break;
            default:
                break;
        }

        ++field_id;
    }
}

void v7_decode_bp_canonical_block_buffer_impl(v7_decode_state_t *dec, void *arg)
{
    v7_canonical_block_info_t *info = arg;
    v7_decode_bp_canonical_bundle_block(dec, info->decode_block, info);
}

void v7_decode_bp_canonical_block_buffer(v7_decode_state_t *dec, bp_canonical_block_buffer_t *v,
                                         size_t *content_encoded_offset, size_t *content_length)
{
    v7_canonical_block_info_t info;

    memset(&info, 0, sizeof(info));

    info.decode_block       = &v->canonical_block;
    info.content_offset_out = content_encoded_offset;

    v7_decode_container(dec, QCBOR_MAX_ITEMS_IN_ARRAY, v7_decode_bp_canonical_block_buffer_impl, &info);

    *content_length = info.content_size;
}
