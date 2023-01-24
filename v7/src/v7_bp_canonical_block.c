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

void v7_encode_bp_canonical_bundle_block(v7_encode_state_t *enc, const bp_canonical_bundle_block_t *v,
                                         const v7_canonical_block_info_t *info)
{
    size_t         content_offset_cbor_end;
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

    if (encode_blocktype < bp_blocktype_MAX_NORMAL)
    {
        v7_encode_bp_blocktype(enc, &encode_blocktype);
        v7_encode_bp_blocknum(enc, &v->blockNum);
        v7_encode_bp_block_processing_flags(enc, &v->processingControlFlags);
        v7_encode_bp_crctype(enc, &v->crctype);

        /*
         * The content is wrapped as a CBOR byte string.
         * The data may itself be CBOR-encoded (so this may result as CBOR-in-CBOR)
         *
         * This encodes the data and snapshots the position of the END of that CBOR byte
         * string.
         */
        cbor_encode_byte_string(enc->cbor, info->content_vptr, info->content_size);
        content_offset_cbor_end = enc->total_bytes_encoded;
        if (content_offset_cbor_end < info->content_size)
        {
            /* this should never happen */
            enc->error = true;
        }

        /* Attach the CRC if requested. */
        if (v->crctype != bp_crctype_none)
        {
            v7_encode_crc(enc);
        }

        /*
         * Export the location where the encoded data actually appeared in the stream.
         *
         * CBOR markup occurs at the beginning of each record, so by knowing
         * where it ends it is easy to find the beginning of actual data, as the CBOR
         * markup itself is variable size, but the content is a known size here.
         */
        if (!enc->error && info->content_offset_out != NULL)
        {
            *info->content_offset_out = content_offset_cbor_end - info->content_size;
        }
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

void v7_decode_bp_canonical_bundle_block(v7_decode_state_t *dec, bp_canonical_bundle_block_t *v,
                                         v7_canonical_block_info_t *info)
{
    const uint8_t *cbor_content_start_ptr;
    size_t         cbor_content_length;
    size_t         cbor_major_size;

    v7_decode_bp_blocktype(dec, &v->blockType);
    v7_decode_bp_blocknum(dec, &v->blockNum);
    v7_decode_bp_block_processing_flags(dec, &v->processingControlFlags);
    v7_decode_bp_crctype(dec, &v->crctype);

    /* The block content should be encoded as a byte string, which internally may
     * contain more CBOR encoding, but that will be done as a separate decode.
     * For now just grab the pointers. */
    if (!dec->error)
    {
        if (cbor_value_at_end(dec->cbor) || cbor_value_get_type(dec->cbor) != CborByteStringType)
        {
            dec->error = true;
        }
        else
        {
            cbor_content_start_ptr = cbor_value_get_next_byte(dec->cbor);
            if (cbor_value_advance(dec->cbor) != CborNoError)
            {
                dec->error = true;
            }

            /*
             * This calculated length reflects the start of this CBOR value to the
             * start of the next CBOR value.  Notably this includes the CBOR overhead/markup
             * for this value, which will need to be removed.  TinyCBOR will want to
             * copy the value, so we go around it for this value and decode the major
             * type locally.
             */
            cbor_content_length = cbor_value_get_next_byte(dec->cbor) - cbor_content_start_ptr;

            /* Advance the pointer according to the CBOR length to get to the real data. */
            cbor_major_size = *cbor_content_start_ptr & 0x1F;
            if (cbor_major_size < 24)
            {
                /* no extra bytes beyond the major type */
                cbor_major_size = 0;
            }
            else if (cbor_major_size < 28)
            {
                /* 1, 2, 4, or 8 additional bytes beyond the major type */
                cbor_major_size = 1 << (cbor_major_size - 24);
            }
            else
            {
                /* Value not well formed, or indefinite length (not supported here) */
                cbor_major_size = cbor_content_length;
            }

            ++cbor_major_size; /* Account for the CBOR major type octet itself (always there) */
            if (cbor_major_size <= cbor_content_length)
            {
                cbor_content_start_ptr += cbor_major_size;
                cbor_content_length -= cbor_major_size;

                /* Export the position of the now-located content information */
                *info->content_offset_out = cbor_content_start_ptr - dec->base;
                info->content_vptr        = cbor_content_start_ptr;
                info->content_size        = cbor_content_length;
            }
            else
            {
                /* This should not happen */
                dec->error = true;
            }
        }
    }

    if (v->crctype != bp_crctype_none)
    {
        v7_decode_crc(dec, &v->crcval);
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

    v7_decode_container(dec, CborIndefiniteLength, v7_decode_bp_canonical_block_buffer_impl, &info);

    *content_length = info.content_size;
}
