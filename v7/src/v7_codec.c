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

#include "bplib.h"
#include "bplib_os.h"

#include "v7_types.h"
#include "v7_codec.h"
#include "v7_mpstream.h"
#include "v7_mpool.h"
#include "v7_mpool_bblocks.h"
#include "cbor.h"

typedef struct
{
    const bp_canonical_bundle_block_t *encode_block;
    bp_canonical_bundle_block_t       *decode_block;
    const void                        *content_vptr;
    size_t                            *content_offset_out;
    size_t                             content_size;

} v7_canonical_block_info_t;

typedef struct
{
    bp_adminrectype_t                encode_rectype;
    const bp_canonical_block_data_t *payload_data;

} v7_admin_rec_payload_encode_info_t;

typedef struct
{
    bp_adminrectype_t          decode_rectype;
    bp_canonical_block_data_t *payload_data;

} v7_admin_rec_payload_decode_info_t;

typedef struct v7_bitmap_table
{
    size_t       offset;
    bp_integer_t mask;
} v7_bitmap_table_t;

typedef struct v7_encode_state
{
    bool error;

    bool                    crc_flag;
    bp_crcval_t             crc_val;
    bplib_crc_parameters_t *crc_params;

    CborEncoder *cbor;

    size_t total_bytes_encoded;

    v7_chunk_writer_func_t next_writer;
    void                  *next_writer_arg;

    // bplib_mpool_stream_t mps;

} v7_encode_state_t;

typedef struct v7_decode_state
{
    bool           error;
    const uint8_t *base;
    CborValue     *cbor;
} v7_decode_state_t;

static const v7_bitmap_table_t V7_BUNDLE_CONTROL_FLAGS_BITMAP_TABLE[] = {
    {offsetof(bp_bundle_processing_control_flags_t, deletion_status_req), 0x40000},
    {offsetof(bp_bundle_processing_control_flags_t, delivery_status_req), 0x20000},
    {offsetof(bp_bundle_processing_control_flags_t, forwarding_status_req), 0x10000},
    {offsetof(bp_bundle_processing_control_flags_t, reception_status_req), 0x04000},
    {offsetof(bp_bundle_processing_control_flags_t, statusTimeRequested), 0x40},
    {offsetof(bp_bundle_processing_control_flags_t, acknowledgementRequested), 0x20},
    {offsetof(bp_bundle_processing_control_flags_t, mustNotFragment), 0x04},
    {offsetof(bp_bundle_processing_control_flags_t, isAdminRecord), 0x02},
    {offsetof(bp_bundle_processing_control_flags_t, isFragment), 0x01},
    {0, 0} /* end of table marker, keep last */
};

static const v7_bitmap_table_t V7_BLOCK_PROCESSING_FLAGS_BITMAP_TABLE[] = {
    {offsetof(bp_block_processing_flags_t, must_remove), 0x10},
    {offsetof(bp_block_processing_flags_t, must_delete), 0x04},
    {offsetof(bp_block_processing_flags_t, xmit_status), 0x02},
    {offsetof(bp_block_processing_flags_t, must_replicate), 0x01},
    {0, 0} /* end of table marker, keep last */
};

/*
 * Generic encode/decode container (aka CBOR array) helpers.
 * This handles the general process of encoding and decoding a container, specifically:
 *  - capture the parent state
 *  - create a new sub-state for the container
 *  - call the function with that new state
 *  - close out the sub-state after function returns
 *  - resume in the parent.
 */
typedef void (*v7_decode_func_t)(v7_decode_state_t *dec, void *arg);
typedef void (*v7_encode_func_t)(v7_encode_state_t *enc, const void *arg);

static void v7_decode_container(v7_decode_state_t *dec, size_t entries, v7_decode_func_t func, void *arg);
static void v7_encode_container(v7_encode_state_t *enc, size_t entries, v7_encode_func_t func, const void *arg);

/*
 * -----------------------------------------------------------------------------------
 * Helpers for encoding/decoding of individual protocol elements
 *
 * Each type used in the bpv7 protocol structures should get a separate encode/decode
 * routine here.  However these are not expected to be called externally, because one
 * must know what structure should be occuring at this specific position in the CBOR
 * stream.  The only externally-callable routines should operate at the block level.
 *
 * If a failure occurs, these set the "error" flag within the state struct.  Such
 * an error probably means any further decoding/encoding is not possible because
 * it may throw off the position in the CBOR stream.
 * -----------------------------------------------------------------------------------
 */

static void v7_encode_small_int(v7_encode_state_t *enc, int val);
static int  v7_decode_small_int(v7_decode_state_t *dec);

static void v7_encode_crc(v7_encode_state_t *enc);
static void v7_decode_crc(v7_decode_state_t *dec, bp_crcval_t *v);
static void v7_encode_bp_integer(v7_encode_state_t *enc, const bp_integer_t *v);
static void v7_decode_bp_integer(v7_decode_state_t *dec, bp_integer_t *v);
static void v7_encode_bp_blocknum(v7_encode_state_t *enc, const bp_blocknum_t *v);
static void v7_decode_bp_blocknum(v7_decode_state_t *dec, bp_blocknum_t *v);
static void v7_encode_bp_blocktype(v7_encode_state_t *enc, const bp_blocktype_t *v);
static void v7_decode_bp_blocktype(v7_decode_state_t *dec, bp_blocktype_t *v);
static void v7_encode_bp_dtntime(v7_encode_state_t *enc, const bp_dtntime_t *v);
static void v7_decode_bp_dtntime(v7_decode_state_t *dec, bp_dtntime_t *v);
static void v7_encode_bp_sequencenumber(v7_encode_state_t *enc, const bp_sequencenumber_t *v);
static void v7_decode_bp_sequencenumber(v7_decode_state_t *dec, bp_sequencenumber_t *v);
static void v7_encode_bp_endpointid_scheme(v7_encode_state_t *enc, const bp_endpointid_scheme_t *v);
static void v7_decode_bp_endpointid_scheme(v7_decode_state_t *dec, bp_endpointid_scheme_t *v);
static void v7_encode_bp_ipn_nodenumber(v7_encode_state_t *enc, const bp_ipn_nodenumber_t *v);
static void v7_decode_bp_ipn_nodenumber(v7_decode_state_t *dec, bp_ipn_nodenumber_t *v);
static void v7_encode_bp_ipn_servicenumber(v7_encode_state_t *enc, const bp_ipn_servicenumber_t *v);
static void v7_decode_bp_ipn_servicenumber(v7_decode_state_t *dec, bp_ipn_servicenumber_t *v);
static void v7_encode_bp_ipn_uri_ssp(v7_encode_state_t *enc, const bp_ipn_uri_ssp_t *v);
static void v7_decode_bp_ipn_uri_ssp(v7_decode_state_t *dec, bp_ipn_uri_ssp_t *v);
static void v7_encode_bp_creation_timestamp(v7_encode_state_t *enc, const bp_creation_timestamp_t *v);
static void v7_decode_bp_creation_timestamp(v7_decode_state_t *dec, bp_creation_timestamp_t *v);
static void v7_encode_bp_crctype(v7_encode_state_t *enc, const bp_crctype_t *v);
static void v7_decode_bp_crctype(v7_decode_state_t *dec, bp_crctype_t *v);
static void v7_encode_bp_bundle_processing_control_flags(v7_encode_state_t                          *enc,
                                                         const bp_bundle_processing_control_flags_t *v);
static void v7_decode_bp_bundle_processing_control_flags(v7_decode_state_t                    *dec,
                                                         bp_bundle_processing_control_flags_t *v);
static void v7_encode_bp_block_processing_flags(v7_encode_state_t *enc, const bp_block_processing_flags_t *v);
static void v7_decode_bp_block_processing_flags(v7_decode_state_t *dec, bp_block_processing_flags_t *v);
static void v7_encode_bp_lifetime(v7_encode_state_t *enc, const bp_lifetime_t *v);
static void v7_decode_bp_lifetime(v7_decode_state_t *dec, bp_lifetime_t *v);
static void v7_encode_bp_adu_length(v7_encode_state_t *enc, const bp_adu_length_t *v);
static void v7_decode_bp_adu_length(v7_decode_state_t *dec, bp_adu_length_t *v);
static void v7_encode_bp_endpointid_buffer(v7_encode_state_t *enc, const bp_endpointid_buffer_t *v);
static void v7_decode_bp_endpointid_buffer(v7_decode_state_t *dec, bp_endpointid_buffer_t *v);
static void v7_encode_bp_primary_block(v7_encode_state_t *enc, const bp_primary_block_t *v);
static void v7_decode_bp_primary_block(v7_decode_state_t *dec, bp_primary_block_t *v);
static void v7_encode_bp_canonical_bundle_block(v7_encode_state_t *enc, const bp_canonical_bundle_block_t *v,
                                                const v7_canonical_block_info_t *info);
static void v7_decode_bp_canonical_bundle_block(v7_decode_state_t *dec, bp_canonical_bundle_block_t *v,
                                                v7_canonical_block_info_t *info);
static void v7_encode_bp_previous_node_block(v7_encode_state_t *enc, const bp_previous_node_block_t *v);
static void v7_decode_bp_previous_node_block(v7_decode_state_t *dec, bp_previous_node_block_t *v);
static void v7_encode_bp_bundle_age_block(v7_encode_state_t *enc, const bp_bundle_age_block_t *v);
static void v7_decode_bp_bundle_age_block(v7_decode_state_t *dec, bp_bundle_age_block_t *v);
static void v7_encode_bp_hop_count_block(v7_encode_state_t *enc, const bp_hop_count_block_t *v);
static void v7_decode_bp_hop_count_block(v7_decode_state_t *dec, bp_hop_count_block_t *v);

static void v7_encode_bp_custody_tracking_block(v7_encode_state_t *enc, const bp_custody_tracking_block_t *v);
static void v7_decode_bp_custody_tracking_block(v7_decode_state_t *dec, bp_custody_tracking_block_t *v);

static void v7_encode_bp_custody_acceptance_block(v7_encode_state_t *enc, const bp_custody_accept_payload_block_t *v);
static void v7_decode_bp_custody_acceptance_block(v7_decode_state_t *dec, bp_custody_accept_payload_block_t *v);

static void v7_decode_bp_canonical_block_buffer(v7_decode_state_t *dec, bp_canonical_block_buffer_t *v,
                                                size_t *content_encoded_offset, size_t *content_length);
static void v7_encode_bp_canonical_block_buffer(v7_encode_state_t *enc, const bp_canonical_block_buffer_t *v,
                                                const void *content_ptr, size_t content_length,
                                                size_t *content_encoded_offset);

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

static int v7_encoder_mpstream_write(void *arg, const void *ptr, size_t sz)
{
    if (bplib_mpool_stream_write(arg, ptr, sz) < sz)
    {
        return BP_ERROR;
    }

    return BP_SUCCESS;
}

bplib_crc_parameters_t *v7_encode_get_algorithm(bp_crctype_t crctype)
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

static int v7_encoder_write_crc(v7_encode_state_t *enc)
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

static CborError v7_encoder_write_wrapper(void *arg, const void *ptr, size_t sz, CborEncoderAppendType at)
{
    v7_encode_state_t *enc = arg;
    int                write_result;

    if (enc->crc_params)
    {
        enc->crc_val = bplib_crc_update(enc->crc_params, enc->crc_val, ptr, sz);
    }

    if (enc->crc_flag && at == CborEncoderAppendStringData)
    {
        /* write the actual crc value, instead of writing the passed-in string (which is 0-padded) */
        enc->crc_flag = false;
        write_result  = v7_encoder_write_crc(enc);
    }
    else
    {
        write_result = enc->next_writer(enc->next_writer_arg, ptr, sz);
    }

    if (write_result != BP_SUCCESS)
    {
        return CborErrorIO;
    }

    enc->total_bytes_encoded += sz;

    return CborNoError;
}

static void v7_encode_setup(v7_encode_state_t *enc, CborEncoder *top_level_cbor, bp_crctype_t crctype,
                            v7_chunk_writer_func_t next_writer, void *next_writer_arg)
{
    memset(enc, 0, sizeof(*enc));

    cbor_encoder_init_writer(top_level_cbor, v7_encoder_write_wrapper, enc);
    enc->cbor = top_level_cbor;

    enc->crc_params = v7_encode_get_algorithm(crctype);
    enc->crc_val    = bplib_crc_initial_value(enc->crc_params);

    enc->next_writer     = next_writer;
    enc->next_writer_arg = next_writer_arg;
}

void v7_encode_small_int(v7_encode_state_t *enc, int val)
{
    if (!enc->error)
    {
        if (cbor_encode_int(enc->cbor, val) != CborNoError)
        {
            enc->error = true;
        }
    }
}

int v7_decode_small_int(v7_decode_state_t *dec)
{
    int val;

    val = 0;
    if (!dec->error)
    {
        if (cbor_value_at_end(dec->cbor) || cbor_value_get_type(dec->cbor) != CborIntegerType)
        {
            dec->error = true;
        }
        else
        {
            if (cbor_value_get_int(dec->cbor, &val) != CborNoError)
            {
                dec->error = true;
            }
            else if (cbor_value_advance_fixed(dec->cbor) != CborNoError)
            {
                dec->error = true;
            }
        }
    }

    return val;
}

void v7_encode_crc(v7_encode_state_t *enc)
{
    size_t               crc_len;
    static const uint8_t CRC_PAD_BYTES[sizeof(bp_crcval_t)] = {0};

    /*
     * NOTE: Unlike other encode functions, this does not accept a value passed in, as the
     * caller would have no way of knowing it - the CRC reflects the encoded data, but this
     * is called in the process of creating that encoded data.  So the value is pulled
     * from the state of the stream writer.
     *
     * Another bit of circular dependency here:
     *
     * to encode the CRC requires the value, but it has to be encoded in a byte string
     * first because the CRC includes/covers those CBOR encoding bits too.
     *
     * but the byte string cannot be encoded until the value is known....
     *
     * Note that TinyCBOR is being used in a streaming mode, thus we cannot write
     * a string and then "back up" - it may not be possible to reverse the stream.
     *
     * To get around this circular dependency for now, we can predict what the CBOR
     * encoding bytes _should_ be (always a byte string of the CRC length) and update
     * the CRC accordingly, then actually write the CRC.
     */

    crc_len = bplib_crc_get_width(enc->crc_params) / 8;

    if (crc_len > sizeof(CRC_PAD_BYTES))
    {
        enc->error = true;
    }
    else
    {
        enc->crc_flag = true;

        if (cbor_encode_byte_string(enc->cbor, CRC_PAD_BYTES, crc_len) != CborNoError)
        {
            enc->error = true;
        }
    }
}

void v7_decode_crc(v7_decode_state_t *dec, bp_crcval_t *v)
{
    bp_crcval_t crc_val;
    uint8_t     bytes[1 + sizeof(crc_val)];
    size_t      len;
    size_t      i;

    crc_val = 0;

    if (!dec->error)
    {
        if (cbor_value_at_end(dec->cbor) || cbor_value_get_type(dec->cbor) != CborByteStringType)
        {
            dec->error = true;
        }
        else
        {
            len = sizeof(bytes);
            if (cbor_value_copy_byte_string(dec->cbor, bytes, &len, NULL) != CborNoError)
            {
                dec->error = true;
            }
            else if (cbor_value_advance(dec->cbor) != CborNoError)
            {
                dec->error = true;
            }
            else
            {
                /* Interpret the bytestring value as an integer */
                for (i = 0; i < len; ++i)
                {
                    crc_val <<= 8;
                    crc_val |= bytes[i];
                }
            }
        }
    }

    *v = crc_val;
}

void v7_encode_container(v7_encode_state_t *enc, size_t entries, v7_encode_func_t func, const void *arg)
{
    CborEncoder  content;
    CborEncoder *parent;

    if (!enc->error)
    {
        /* Save the parent encode state */
        parent = enc->cbor;

        if (cbor_encoder_create_array(parent, &content, entries) != CborNoError)
        {
            enc->error = true;
        }
        else
        {
            /* go into container */
            enc->cbor = &content;

            /* call the handler impl */
            func(enc, arg);

            /* return to parent */
            if (cbor_encoder_close_container(parent, &content) != CborNoError)
            {
                enc->error = true;
            }

            enc->cbor = parent;
        }
    }
}

void v7_decode_container(v7_decode_state_t *dec, size_t entries, v7_decode_func_t func, void *arg)
{
    CborValue  content;
    CborValue *parent;

    if (!dec->error)
    {
        /* Save the parent decode state */
        parent = dec->cbor;

        if (cbor_value_at_end(parent) || cbor_value_get_type(parent) != CborArrayType)
        {
            dec->error = true;
        }
        else
        {
            if (cbor_value_enter_container(parent, &content) != CborNoError)
            {
                dec->error = true;
            }
            else
            {
                /* go into container */
                dec->cbor = &content;

                /* call the handler impl */
                func(dec, arg);

                /* This should have consumed every item */
                if (!cbor_value_at_end(&content))
                {
                    dec->error = true;
                }

                /* return to parent scope */
                if (cbor_value_leave_container(parent, &content) != CborNoError)
                {
                    dec->error = true;
                }

                dec->cbor = parent;
            }
        }
    }
}

void v7_encode_bp_integer(v7_encode_state_t *enc, const bp_integer_t *v)
{
    if (!enc->error)
    {
        if (cbor_encode_uint(enc->cbor, *v) != CborNoError)
        {
            enc->error = true;
        }
    }
}

void v7_decode_bp_integer(v7_decode_state_t *dec, bp_integer_t *v)
{
    uint64_t val;

    val = 0;
    if (!dec->error)
    {
        if (cbor_value_at_end(dec->cbor) || cbor_value_get_type(dec->cbor) != CborIntegerType)
        {
            dec->error = true;
        }
        else
        {
            if (cbor_value_get_uint64(dec->cbor, &val) != CborNoError)
            {
                dec->error = true;
            }
            else if (cbor_value_advance_fixed(dec->cbor) != CborNoError)
            {
                dec->error = true;
            }
        }
    }

    *v = val;
}

void v7_encode_bp_blocknum(v7_encode_state_t *enc, const bp_blocknum_t *v)
{
    v7_encode_small_int(enc, *v);
}

void v7_decode_bp_blocknum(v7_decode_state_t *dec, bp_blocknum_t *v)
{
    *v = (bp_blocknum_t)v7_decode_small_int(dec);
}

void v7_encode_bp_blocktype(v7_encode_state_t *enc, const bp_blocktype_t *v)
{
    v7_encode_small_int(enc, *v);
}

void v7_decode_bp_blocktype(v7_decode_state_t *dec, bp_blocktype_t *v)
{
    *v = (bp_blocktype_t)v7_decode_small_int(dec);
}

void v7_encode_bp_dtntime(v7_encode_state_t *enc, const bp_dtntime_t *v)
{
    v7_encode_bp_integer(enc, v);
}

void v7_decode_bp_dtntime(v7_decode_state_t *dec, bp_dtntime_t *v)
{
    v7_decode_bp_integer(dec, v);
}

void v7_encode_bp_sequencenumber(v7_encode_state_t *enc, const bp_sequencenumber_t *v)
{
    v7_encode_bp_integer(enc, v);
}

void v7_decode_bp_sequencenumber(v7_decode_state_t *dec, bp_sequencenumber_t *v)
{
    v7_decode_bp_integer(dec, v);
}

void v7_encode_bp_endpointid_scheme(v7_encode_state_t *enc, const bp_endpointid_scheme_t *v)
{
    v7_encode_small_int(enc, *v);
}

void v7_decode_bp_endpointid_scheme(v7_decode_state_t *dec, bp_endpointid_scheme_t *v)
{
    *v = v7_decode_small_int(dec);
}

void v7_encode_bp_ipn_nodenumber(v7_encode_state_t *enc, const bp_ipn_nodenumber_t *v)
{
    v7_encode_bp_integer(enc, v);
}

void v7_decode_bp_ipn_nodenumber(v7_decode_state_t *dec, bp_ipn_nodenumber_t *v)
{
    v7_decode_bp_integer(dec, v);
}

void v7_encode_bp_ipn_servicenumber(v7_encode_state_t *enc, const bp_ipn_servicenumber_t *v)
{
    v7_encode_bp_integer(enc, v);
}

void v7_decode_bp_ipn_servicenumber(v7_decode_state_t *dec, bp_ipn_servicenumber_t *v)
{
    v7_decode_bp_integer(dec, v);
}

void v7_encode_bp_ipn_uri_ssp_impl(v7_encode_state_t *enc, const void *arg)
{
    const bp_ipn_uri_ssp_t *v = arg;

    v7_encode_bp_ipn_nodenumber(enc, &v->node_number);
    v7_encode_bp_ipn_servicenumber(enc, &v->service_number);
}

void v7_encode_bp_ipn_uri_ssp(v7_encode_state_t *enc, const bp_ipn_uri_ssp_t *v)
{
    v7_encode_container(enc, 2, v7_encode_bp_ipn_uri_ssp_impl, v);
}

static void v7_decode_bp_ipn_uri_ssp_impl(v7_decode_state_t *dec, void *arg)
{
    bp_ipn_uri_ssp_t *v = arg;

    v7_decode_bp_ipn_nodenumber(dec, &v->node_number);
    v7_decode_bp_ipn_servicenumber(dec, &v->service_number);
}

void v7_decode_bp_ipn_uri_ssp(v7_decode_state_t *dec, bp_ipn_uri_ssp_t *v)
{
    v7_decode_container(dec, 2, v7_decode_bp_ipn_uri_ssp_impl, v);
}

static void v7_encode_bp_creation_timestamp_impl(v7_encode_state_t *enc, const void *arg)
{
    const bp_creation_timestamp_t *v = arg;

    v7_encode_bp_dtntime(enc, &v->time);
    v7_encode_bp_sequencenumber(enc, &v->sequence_num);
}

void v7_encode_bp_creation_timestamp(v7_encode_state_t *enc, const bp_creation_timestamp_t *v)
{
    v7_encode_container(enc, 2, v7_encode_bp_creation_timestamp_impl, v);
}

static void v7_decode_bp_creation_timestamp_impl(v7_decode_state_t *dec, void *arg)
{
    bp_creation_timestamp_t *v = arg;

    v7_decode_bp_dtntime(dec, &v->time);
    v7_decode_bp_sequencenumber(dec, &v->sequence_num);
}

void v7_decode_bp_creation_timestamp(v7_decode_state_t *dec, bp_creation_timestamp_t *v)
{
    v7_decode_container(dec, 2, v7_decode_bp_creation_timestamp_impl, v);
}

void v7_encode_bp_crctype(v7_encode_state_t *enc, const bp_crctype_t *v)
{
    v7_encode_small_int(enc, *v);
}

void v7_decode_bp_crctype(v7_decode_state_t *dec, bp_crctype_t *v)
{
    *v = (bp_crctype_t)v7_decode_small_int(dec);
}

void v7_encode_bitmap(v7_encode_state_t *enc, const uint8_t *v, const v7_bitmap_table_t *ptbl)
{
    bp_integer_t value;

    value = 0;
    while (ptbl->mask != 0)
    {
        if (v[ptbl->offset])
        {
            value |= ptbl->mask;
        }
        ++ptbl;
    }

    v7_encode_bp_integer(enc, &value);
}

void v7_decode_bitmap(v7_decode_state_t *dec, uint8_t *v, const v7_bitmap_table_t *ptbl)
{
    bp_integer_t value;

    v7_decode_bp_integer(dec, &value);
    while (ptbl->mask != 0)
    {
        v[ptbl->offset] = (value & ptbl->mask) != 0;
        ++ptbl;
    }
}

void v7_encode_bp_bundle_processing_control_flags(v7_encode_state_t *enc, const bp_bundle_processing_control_flags_t *v)
{
    v7_encode_bitmap(enc, (const uint8_t *)v, V7_BUNDLE_CONTROL_FLAGS_BITMAP_TABLE);
}

void v7_decode_bp_bundle_processing_control_flags(v7_decode_state_t *dec, bp_bundle_processing_control_flags_t *v)
{
    v7_decode_bitmap(dec, (uint8_t *)v, V7_BUNDLE_CONTROL_FLAGS_BITMAP_TABLE);
}

void v7_encode_bp_block_processing_flags(v7_encode_state_t *enc, const bp_block_processing_flags_t *v)
{
    v7_encode_bitmap(enc, (const uint8_t *)v, V7_BLOCK_PROCESSING_FLAGS_BITMAP_TABLE);
}

void v7_decode_bp_block_processing_flags(v7_decode_state_t *dec, bp_block_processing_flags_t *v)
{
    v7_decode_bitmap(dec, (uint8_t *)v, V7_BLOCK_PROCESSING_FLAGS_BITMAP_TABLE);
}

void v7_encode_bp_lifetime(v7_encode_state_t *enc, const bp_lifetime_t *v)
{
    v7_encode_bp_integer(enc, v);
}

void v7_decode_bp_lifetime(v7_decode_state_t *dec, bp_lifetime_t *v)
{
    v7_decode_bp_integer(dec, v);
}

void v7_encode_bp_adu_length(v7_encode_state_t *enc, const bp_adu_length_t *v)
{
    v7_encode_bp_integer(enc, v);
}

void v7_decode_bp_adu_length(v7_decode_state_t *dec, bp_adu_length_t *v)
{
    v7_decode_bp_integer(dec, v);
}

static void v7_encode_bp_endpointid_buffer_impl(v7_encode_state_t *enc, const void *arg)
{
    const bp_endpointid_buffer_t *v = arg;

    v7_encode_bp_endpointid_scheme(enc, &v->scheme); /* always present, indicates which other field is valid */

    switch (v->scheme)
    {
        case bp_endpointid_scheme_ipn:
            v7_encode_bp_ipn_uri_ssp(enc, &v->ssp.ipn);
            break;

        default:
            /* do not know how to decode */
            enc->error = true;
            break;
    }
}

void v7_encode_bp_endpointid_buffer(v7_encode_state_t *enc, const bp_endpointid_buffer_t *v)
{
    v7_encode_container(enc, 2, v7_encode_bp_endpointid_buffer_impl, v);
}

static void v7_decode_bp_endpointid_buffer_impl(v7_decode_state_t *dec, void *arg)
{
    bp_endpointid_buffer_t *v = arg;

    v7_decode_bp_endpointid_scheme(dec, &v->scheme); /* always present, indicates which other field is valid */

    switch (v->scheme)
    {
        case bp_endpointid_scheme_ipn:
            v7_decode_bp_ipn_uri_ssp(dec, &v->ssp.ipn);
            break;

        default:
            /* do not know how to decode */
            dec->error = true;
            break;
    }
}

void v7_decode_bp_endpointid_buffer(v7_decode_state_t *dec, bp_endpointid_buffer_t *v)
{
    v7_decode_container(dec, 2, v7_decode_bp_endpointid_buffer_impl, v);
}

void v7_encode_bp_primary_block_impl(v7_encode_state_t *enc, const void *arg)
{
    const bp_primary_block_t *v = arg;

    v7_encode_small_int(enc, v->version);
    if (v->version != 7)
    {
        /* don't know how to encode the rest if not v7 */
        enc->error = true;
        return;
    }

    /* the other 7 fixed fields defined by BPv7 */
    v7_encode_bp_bundle_processing_control_flags(enc, &v->controlFlags);
    v7_encode_bp_crctype(enc, &v->crctype);
    v7_encode_bp_endpointid_buffer(enc, &v->destinationEID);
    v7_encode_bp_endpointid_buffer(enc, &v->sourceEID);
    v7_encode_bp_endpointid_buffer(enc, &v->reportEID);
    v7_encode_bp_creation_timestamp(enc, &v->creationTimeStamp);
    v7_encode_bp_lifetime(enc, &v->lifetime);

    /* next fields depend on whether the flag is set */
    if (v->controlFlags.isFragment)
    {
        v7_encode_bp_adu_length(enc, &v->fragmentOffset);
        v7_encode_bp_adu_length(enc, &v->totalADUlength);
    }

    /* Attach the CRC if requested. */
    if (v->crctype != bp_crctype_none)
    {
        v7_encode_crc(enc);
    }
}

void v7_encode_bp_primary_block(v7_encode_state_t *enc, const bp_primary_block_t *v)
{
    size_t num_fields;

    /*
     * In order to encode the resulting array with a definite length (rather than CBOR
     * indefinite length) this must predict the number of fields that will be encoded.
     */
    num_fields = 8; /* fixed fields per BPv7 */
    if (v->controlFlags.isFragment)
    {
        num_fields += 2; /* fragments have a size + offset field */
    }
    if (v->crctype != bp_crctype_none)
    {
        ++num_fields; /* one more for CRC */
    }

    /* reset the CRC algorithm for this block */
    enc->crc_params = v7_encode_get_algorithm(v->crctype);
    enc->crc_val    = bplib_crc_initial_value(enc->crc_params);

    v7_encode_container(enc, num_fields, v7_encode_bp_primary_block_impl, v);
}

void v7_decode_bp_primary_block_impl(v7_decode_state_t *dec, void *arg)
{
    bp_primary_block_t *v = arg;

    v->version = (uint8_t)v7_decode_small_int(dec);
    if (v->version != 7)
    {
        /* don't know how to decode the rest if not v7 */
        dec->error = true;
        return;
    }

    /* the other 7 fixed fields defined by BPv7 */
    v7_decode_bp_bundle_processing_control_flags(dec, &v->controlFlags);
    v7_decode_bp_crctype(dec, &v->crctype);
    v7_decode_bp_endpointid_buffer(dec, &v->destinationEID);
    v7_decode_bp_endpointid_buffer(dec, &v->sourceEID);
    v7_decode_bp_endpointid_buffer(dec, &v->reportEID);
    v7_decode_bp_creation_timestamp(dec, &v->creationTimeStamp);
    v7_decode_bp_lifetime(dec, &v->lifetime);

    /* next fields depend on whether the flag is set */
    if (v->controlFlags.isFragment)
    {
        v7_decode_bp_adu_length(dec, &v->fragmentOffset);
        v7_decode_bp_adu_length(dec, &v->totalADUlength);
    }
    else
    {
        v->fragmentOffset = 0;
        v->totalADUlength = 0;
    }

    /* Attach the CRC if requested. */
    if (v->crctype != bp_crctype_none)
    {
        v7_decode_crc(dec, &v->crcval);
    }
}

void v7_decode_bp_primary_block(v7_decode_state_t *dec, bp_primary_block_t *v)
{
    v7_decode_container(dec, CborIndefiniteLength, v7_decode_bp_primary_block_impl, v);
}

void v7_encode_bp_previous_node_block(v7_encode_state_t *enc, const bp_previous_node_block_t *v)
{
    v7_encode_bp_endpointid_buffer(enc, &v->nodeId);
}

void v7_decode_bp_previous_node_block(v7_decode_state_t *dec, bp_previous_node_block_t *v)
{
    v7_decode_bp_endpointid_buffer(dec, &v->nodeId);
}

void v7_encode_bp_bundle_age_block(v7_encode_state_t *enc, const bp_bundle_age_block_t *v)
{
    v7_encode_bp_dtntime(enc, &v->age);
}

void v7_decode_bp_bundle_age_block(v7_decode_state_t *dec, bp_bundle_age_block_t *v)
{
    v7_decode_bp_dtntime(dec, &v->age);
}

static void v7_encode_bp_hop_count_block_impl(v7_encode_state_t *enc, const void *arg)
{
    const bp_hop_count_block_t *v = arg;

    v7_encode_bp_integer(enc, &v->hopLimit);
    v7_encode_bp_integer(enc, &v->hopCount);
}

void v7_encode_bp_hop_count_block(v7_encode_state_t *enc, const bp_hop_count_block_t *v)
{
    v7_encode_container(enc, 2, v7_encode_bp_hop_count_block_impl, v);
}

static void v7_decode_bp_hop_count_block_impl(v7_decode_state_t *dec, void *arg)
{
    bp_hop_count_block_t *v = arg;

    v7_decode_bp_integer(dec, &v->hopLimit);
    v7_decode_bp_integer(dec, &v->hopCount);
}

void v7_decode_bp_hop_count_block(v7_decode_state_t *dec, bp_hop_count_block_t *v)
{
    v7_decode_container(dec, 2, v7_decode_bp_hop_count_block_impl, v);
}

void v7_encode_bp_custody_tracking_block(v7_encode_state_t *enc, const bp_custody_tracking_block_t *v)
{
    v7_encode_bp_endpointid_buffer(enc, &v->current_custodian);
}

void v7_decode_bp_custody_tracking_block(v7_decode_state_t *dec, bp_custody_tracking_block_t *v)
{
    v7_decode_bp_endpointid_buffer(dec, &v->current_custodian);
}

static void v7_encode_bp_custody_acceptance_seqlist_impl(v7_encode_state_t *enc, const void *arg)
{
    const bp_custody_accept_payload_block_t *v = arg;
    bp_integer_t                             n;

    for (n = 0; n < v->num_entries && !enc->error; ++n)
    {
        v7_encode_bp_integer(enc, &v->sequence_nums[n]);
    }
}

static void v7_encode_bp_custody_acceptance_block_impl(v7_encode_state_t *enc, const void *arg)
{
    const bp_custody_accept_payload_block_t *v = arg;

    v7_encode_bp_endpointid_buffer(enc, &v->flow_source_eid);
    v7_encode_container(enc, v->num_entries, v7_encode_bp_custody_acceptance_seqlist_impl, v);
}

void v7_encode_bp_custody_acceptance_block(v7_encode_state_t *enc, const bp_custody_accept_payload_block_t *v)
{
    v7_encode_container(enc, 2, v7_encode_bp_custody_acceptance_block_impl, v);
}

static void v7_decode_bp_custody_acceptance_seqlist_impl(v7_decode_state_t *dec, void *arg)
{
    bp_custody_accept_payload_block_t *v = arg;

    while (!cbor_value_at_end(dec->cbor) && v->num_entries < BP_DACS_MAX_SEQ_PER_PAYLOAD)
    {
        v7_decode_bp_integer(dec, &v->sequence_nums[v->num_entries]);
        if (dec->error)
        {
            break;
        }

        ++v->num_entries;
    }
}

static void v7_decode_bp_custody_acceptance_block_impl(v7_decode_state_t *dec, void *arg)
{
    bp_custody_accept_payload_block_t *v = arg;

    v7_decode_bp_endpointid_buffer(dec, &v->flow_source_eid);
    v7_decode_container(dec, CborIndefiniteLength, v7_decode_bp_custody_acceptance_seqlist_impl, v);
}

void v7_decode_bp_custody_acceptance_block(v7_decode_state_t *dec, bp_custody_accept_payload_block_t *v)
{
    v7_decode_container(dec, 2, v7_decode_bp_custody_acceptance_block_impl, v);
}

static void v7_encode_bp_adminrec_payload_impl(v7_encode_state_t *enc, const void *arg)
{
    const v7_admin_rec_payload_encode_info_t *admin_rec_payload;

    admin_rec_payload = arg;
    v7_encode_small_int(enc, admin_rec_payload->encode_rectype);

    if (!enc->error)
    {
        switch (admin_rec_payload->encode_rectype)
        {
            case bp_adminrectype_custodyAcknowledgement:
                v7_encode_bp_custody_acceptance_block(enc,
                                                      &admin_rec_payload->payload_data->custody_accept_payload_block);
                break;
            default:
                /* missing implementation */
                enc->error = true;
                break;
        }
    }
}

static void v7_decode_bp_adminrec_payload_impl(v7_decode_state_t *dec, void *arg)
{
    v7_admin_rec_payload_decode_info_t *admin_rec_payload;

    admin_rec_payload                 = arg;
    admin_rec_payload->decode_rectype = v7_decode_small_int(dec);

    if (!dec->error)
    {
        switch (admin_rec_payload->decode_rectype)
        {
            case bp_adminrectype_custodyAcknowledgement:
                v7_decode_bp_custody_acceptance_block(dec,
                                                      &admin_rec_payload->payload_data->custody_accept_payload_block);
                break;
            default:
                /* missing implementation */
                dec->error = true;
                break;
        }
    }
}

void v7_encode_bp_admin_record_payload(v7_encode_state_t *enc, const bp_canonical_block_buffer_t *v)
{
    v7_admin_rec_payload_encode_info_t admin_rec_payload;

    memset(&admin_rec_payload, 0, sizeof(admin_rec_payload));

    admin_rec_payload.payload_data = &v->data;

    switch (v->canonical_block.blockType)
    {
        case bp_blocktype_custodyAcceptPayloadBlock:
            admin_rec_payload.encode_rectype = bp_adminrectype_custodyAcknowledgement;
            break;
        default:
            /* missing implementation */
            enc->error = true;
            break;
    }

    v7_encode_container(enc, 2, v7_encode_bp_adminrec_payload_impl, &admin_rec_payload);
}

void v7_decode_bp_admin_record_payload(v7_decode_state_t *dec, bp_canonical_block_buffer_t *v)
{
    v7_admin_rec_payload_decode_info_t admin_rec_payload;

    memset(&admin_rec_payload, 0, sizeof(admin_rec_payload));

    admin_rec_payload.payload_data = &v->data;

    v7_decode_container(dec, 2, v7_decode_bp_adminrec_payload_impl, &admin_rec_payload);

    /* if decode was successful, update blockType to the real/more specific block type */
    if (!dec->error)
    {
        switch (admin_rec_payload.decode_rectype)
        {
            case bp_adminrectype_custodyAcknowledgement:
                v->canonical_block.blockType = bp_blocktype_custodyAcceptPayloadBlock;
                break;
            default:
                /* missing implementation */
                dec->error = true;
                break;
        }
    }
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
    encode_blocktype = v->blockType;
    if (encode_blocktype >= bp_blocktype_SPECIAL_PAYLOADS_START && encode_blocktype < bp_blocktype_SPECIAL_PAYLOADS_MAX)
    {
        /* This special block is fulfilling the RFC9171 "payload block" requirement -
         * there must be an extension block elsewhere in the bundle that helps the
         * decoder on the other side figure out what it really is */
        encode_blocktype = bp_blocktype_payloadBlock;
    }

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

static void v7_decode_bp_canonical_block_buffer_impl(v7_decode_state_t *dec, void *arg)
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
    crc_params = v7_encode_get_algorithm(crc_type);
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

    if (cbor_parser_init(data_ptr, data_size, 0, &parser, &origin) != CborNoError)
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
         * Second stage decode - for recognized non-payload extension blocks
         */
        if (cbor_parser_init(v7_state.base + content_offset, content_size, 0, &parser, &origin) != CborNoError)
        {
            v7_state.error = true;
        }
        else
        {
            v7_state.base += content_offset;
            v7_state.cbor = &origin;

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

    if (v7_state.error)
    {
        return -1;
    }
    return 0;
}

int v7_block_encode_pri(bplib_mpool_bblock_primary_t *cpb)
{
    v7_encode_state_t         v7_state;
    bplib_mpool_stream_t      mps;
    CborEncoder               top_level_enc;
    const bp_primary_block_t *pri;

    /* If there is any existing encoded data, return it to the pool */
    bplib_mpool_bblock_primary_drop_encode(cpb);

    pri = bplib_mpool_bblock_primary_get_logical(cpb);

    bplib_mpool_start_stream_init(&mps, bplib_mpool_get_parent_pool_from_link(&cpb->chunk_list),
                                  bplib_mpool_stream_dir_write);
    v7_encode_setup(&v7_state, &top_level_enc, pri->crctype, v7_encoder_mpstream_write, &mps);

    v7_encode_bp_primary_block(&v7_state, pri);

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
    CborEncoder                        top_level_enc;
    const bp_canonical_block_buffer_t *pay;
    size_t                             data_encoded_offset;
    bplib_mpool_t                     *ppool;

    /* If there is any existing encoded data, return it to the pool */
    bplib_mpool_bblock_canonical_drop_encode(ccb);

    ppool = bplib_mpool_get_parent_pool_from_link(&ccb->chunk_list);

    pay = bplib_mpool_bblock_canonical_get_logical(ccb);
    bplib_mpool_start_stream_init(&mps, ppool, bplib_mpool_stream_dir_write);
    v7_encode_setup(&v7_state, &top_level_enc, pay->canonical_block.crctype, v7_encoder_mpstream_write, &mps);

    v7_encode_bp_canonical_block_buffer(&v7_state, pay, data_ptr, data_size, &data_encoded_offset);

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
    CborEncoder                        top_level_enc;
    const bp_canonical_block_buffer_t *logical;
    uint8_t                            scratch_area[256];
    size_t                             scratch_size;
    size_t                             content_encoded_offset;
    bplib_mpool_t                     *ppool;

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
    cbor_encoder_init(&top_level_enc, scratch_area, sizeof(scratch_area), 0);

    v7_state.cbor = &top_level_enc;

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

    if (!v7_state.error)
    {
        scratch_size = cbor_encoder_get_buffer_size(&top_level_enc, scratch_area);
        if (scratch_size > 0)
        {
            /*
             * Do second-stage encode - take the scratch buffer and use it as the content of the extension block
             */
            bplib_mpool_start_stream_init(&mps, ppool, bplib_mpool_stream_dir_write);
            v7_encode_setup(&v7_state, &top_level_enc, logical->canonical_block.crctype, v7_encoder_mpstream_write,
                            &mps);

            v7_encode_bp_canonical_block_buffer(&v7_state, logical, scratch_area, scratch_size,
                                                &content_encoded_offset);

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
        while (last_encode == 0)
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
    }
    while (remain_sz > 0 && *in_p != 0xFF); /* not CBOR break code */

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
