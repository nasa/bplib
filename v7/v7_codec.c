/************************************************************************
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *  All Other Rights Reserved.
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be
 *  used, distributed and modified only pursuant to the terms of that
 *  agreement.
 *
 *************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"

#include "v7_types.h"
#include "v7_codec.h"
#include "cbor.h"

typedef struct
{
    const bp_canonical_bundle_block_t *encode_block;
    bp_canonical_bundle_block_t       *decode_block;
    const void                        *content_ptr;
    size_t                             content_size;
    size_t                             num_fields;

} v7_canonical_block_info_t;

typedef struct v7_bitmap_table
{
    size_t       offset;
    bp_integer_t mask;
} v7_bitmap_table_t;

typedef struct v7_encode_state
{
    bool           error;
    CborEncoder   *cbor;
    mpool_stream_t mps;

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
                                                const void *content_ptr, size_t content_length);
static void v7_decode_bp_canonical_bundle_block(v7_decode_state_t *dec, bp_canonical_bundle_block_t *v,
                                                const void **content_ptr, size_t *content_length);
static void v7_encode_bp_previous_node_block(v7_encode_state_t *enc, const bp_previous_node_block_t *v);
static void v7_decode_bp_previous_node_block(v7_decode_state_t *dec, bp_previous_node_block_t *v);
static void v7_encode_bp_bundle_age_block(v7_encode_state_t *enc, const bp_bundle_age_block_t *v);
static void v7_decode_bp_bundle_age_block(v7_decode_state_t *dec, bp_bundle_age_block_t *v);
static void v7_encode_bp_hop_count_block(v7_encode_state_t *enc, const bp_hop_count_block_t *v);
static void v7_decode_bp_hop_count_block(v7_decode_state_t *dec, bp_hop_count_block_t *v);
static void v7_encode_bp_canonical_block_buffer(v7_encode_state_t *enc, const bp_canonical_block_buffer_t *v,
                                                const void *content_ptr, size_t content_length);
static void v7_decode_bp_canonical_block_buffer(v7_decode_state_t *dec, bp_canonical_block_buffer_t *v,
                                                const void **content_ptr, size_t *content_length);

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
static size_t v7_save_and_verify_block(mpool_t *pool, mpool_cache_block_t *head, const uint8_t *block_base,
                                       size_t block_size, bp_crctype_t crc_type, bp_crcval_t crc_check);

/*
 * -----------------------------------------------------------------------------------
 * IMPLEMENTATION
 * Helpers for encoding/decoding of individual protocol elements
 * -----------------------------------------------------------------------------------
 */

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
    static const uint8_t    CBOR_BYTESTRING = 0x40;
    bplib_crc_parameters_t *crc_params;
    size_t                  crc_len;
    size_t                  i;
    bp_crcval_t             crc_val;
    uint8_t                 cbor_temp_encode[1 + sizeof(crc_val)];
    uint8_t                *pb;

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

    crc_params = mpool_stream_get_crc_params(&enc->mps);
    crc_len    = bplib_crc_get_width(crc_params) / 8;

    if ((1 + crc_len) > sizeof(cbor_temp_encode))
    {
        enc->error = true;
    }
    else
    {
        /* Predict what the enclosing CBOR octet will be */
        cbor_temp_encode[0] = CBOR_BYTESTRING + crc_len;
        memset(&cbor_temp_encode[1], 0, crc_len);

        /* First need to inject zero bytes in place of the CRC */
        crc_val = mpool_stream_get_intermediate_crc(&enc->mps);
        crc_val = bplib_crc_update(crc_params, crc_val, &cbor_temp_encode, 1 + crc_len);
        crc_val = bplib_crc_finalize(crc_params, crc_val);

        /*
         * shift out the CRC bytes, from low to high, and fill the buffer
         * with the real crc value instead of zeros
         */
        pb = &cbor_temp_encode[1 + crc_len];
        for (i = 0; i < crc_len; ++i)
        {
            --pb;
            *pb = crc_val & 0xFF;
            crc_val >>= 8;
        }

        /*
         * Encode only the CRC bytes, NOT the bytestring major type octet,
         * because TinyCBOR will add that on its own.
         */
        if (cbor_encode_byte_string(enc->cbor, pb, crc_len) != CborNoError)
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
    v7_encode_bp_endpointid_buffer(enc, &v->sourceID);
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
    v7_decode_bp_endpointid_buffer(dec, &v->sourceID);
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

void v7_encode_bp_hop_count_block(v7_encode_state_t *enc, const bp_hop_count_block_t *v)
{
    v7_encode_bp_integer(enc, &v->hopLimit);
    v7_encode_bp_integer(enc, &v->hopCount);
}

void v7_decode_bp_hop_count_block(v7_decode_state_t *dec, bp_hop_count_block_t *v)
{
    v7_decode_bp_integer(dec, &v->hopLimit);
    v7_decode_bp_integer(dec, &v->hopCount);
}

void v7_encode_bp_canonical_bundle_block(v7_encode_state_t *enc, const bp_canonical_bundle_block_t *v,
                                         const void *content_ptr, size_t content_length)
{
    v7_encode_bp_blocktype(enc, &v->blockType);
    v7_encode_bp_blocknum(enc, &v->blockNum);
    v7_encode_bp_block_processing_flags(enc, &v->processingControlFlags);
    v7_encode_bp_crctype(enc, &v->crctype);

    /*
     * The content is wrapped as a CBOR byte string.
     * The data may itself be CBOR-encoded (so this may result as CBOR-in-CBOR)
     */
    cbor_encode_byte_string(enc->cbor, content_ptr, content_length);

    /* Attach the CRC if requested. */
    if (v->crctype != bp_crctype_none)
    {
        v7_encode_crc(enc);
    }
}

void v7_decode_bp_canonical_bundle_block(v7_decode_state_t *dec, bp_canonical_bundle_block_t *v,
                                         const void **content_ptr, size_t *content_length)
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
    cbor_content_start_ptr = NULL;
    cbor_content_length    = 0;
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
            }
            else
            {
                cbor_content_start_ptr = NULL;
                cbor_content_length    = 0;
            }
        }
    }

    /* Export the pointer and length of the content part (may be NULL/0) */
    *content_ptr    = cbor_content_start_ptr;
    *content_length = cbor_content_length;

    if (v->crctype != bp_crctype_none)
    {
        v7_decode_crc(dec, &v->crcval);
    }
}

static void v7_encode_bp_canonical_block_buffer_impl(v7_encode_state_t *enc, const void *arg)
{
    const v7_canonical_block_info_t *info = arg;
    v7_encode_bp_canonical_bundle_block(enc, info->encode_block, info->content_ptr, info->content_size);
}

void v7_encode_bp_canonical_block_buffer(v7_encode_state_t *enc, const bp_canonical_block_buffer_t *v,
                                         const void *content_ptr, size_t content_length)
{
    v7_canonical_block_info_t info;

    memset(&info, 0, sizeof(info));

    /* this also needs to predict the number of fields */
    info.num_fields = 5; /* fixed/required fields */
    if (v->canonical_block.crctype != bp_crctype_none)
    {
        ++info.num_fields; /* one more for CRC */
    }

    info.encode_block = &v->canonical_block;
    info.content_ptr  = content_ptr;
    info.content_size = content_length;

    v7_encode_container(enc, info.num_fields, v7_encode_bp_canonical_block_buffer_impl, &info);
}

static void v7_decode_bp_canonical_block_buffer_impl(v7_decode_state_t *dec, void *arg)
{
    v7_canonical_block_info_t *info = arg;
    v7_decode_bp_canonical_bundle_block(dec, info->decode_block, &info->content_ptr, &info->content_size);
}

void v7_decode_bp_canonical_block_buffer(v7_decode_state_t *dec, bp_canonical_block_buffer_t *v,
                                         const void **content_ptr, size_t *content_length)
{
    v7_canonical_block_info_t info;

    memset(&info, 0, sizeof(info));

    info.decode_block = &v->canonical_block;

    v7_decode_container(dec, CborIndefiniteLength, v7_decode_bp_canonical_block_buffer_impl, &info);

    *content_ptr    = info.content_ptr;
    *content_length = info.content_size;
}

static CborError v7_encoder_write(void *arg, const void *ptr, size_t sz, CborEncoderAppendType at)
{
    v7_encode_state_t *v7_state = arg;
    if (mpool_stream_write(&v7_state->mps, ptr, sz) < sz)
    {
        return CborErrorOutOfMemory;
    }

    return CborNoError;
}

size_t v7_save_and_verify_block(mpool_t *pool, mpool_cache_block_t *head, const uint8_t *block_base, size_t block_size,
                                bp_crctype_t crc_type, bp_crcval_t crc_check)
{
    static const uint8_t    ZERO_BYTES[4] = {0};
    size_t                  data_len;
    size_t                  crc_len;
    bplib_crc_parameters_t *crc_params;
    bp_crcval_t             crc_val;
    mpool_stream_t          mps;
    size_t                  result;

    result = 0;
    mpool_start_stream_init(&mps, pool, mpool_stream_dir_write, crc_type);
    crc_params = mpool_stream_get_crc_params(&mps);
    crc_len    = bplib_crc_get_width(crc_params) / 8;
    if (crc_len < block_size && crc_len <= sizeof(ZERO_BYTES))
    {
        data_len = block_size - crc_len;
        /* first copy only the data part */
        if (mpool_stream_write(&mps, block_base, data_len) == data_len)
        {
            /* snapshot the CRC intermediate value now */
            crc_val = mpool_stream_get_intermediate_crc(&mps);

            /* now copy the original (still unverified) CRC to the buffer */
            if (mpool_stream_write(&mps, block_base + data_len, crc_len) == crc_len)
            {
                /* now verify the CRC - need to pump in zero bytes for CRC width */
                crc_val = bplib_crc_update(crc_params, crc_val, ZERO_BYTES, crc_len);
                crc_val = bplib_crc_finalize(crc_params, crc_val);

                if (crc_val == crc_check)
                {
                    result = mpool_stream_tell(&mps);
                    mpool_stream_attach(&mps, head);
                }
            }
        }
    }

    mpool_stream_close(&mps);

    return result;
}

int v7_block_decode_pri(mpool_t *pool, mpool_cache_primary_block_t *cpb, const void *data_ptr, size_t data_size)
{
    v7_decode_state_t   v7_state;
    CborValue           origin;
    bp_primary_block_t *pri;
    size_t              block_size;
    CborParser          parser;
    int                 result;

    pri = mpool_get_pri_block_logical(cpb);
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

    if (v7_state.error)
    {
        result = -1;
    }
    else
    {
        block_size = cbor_value_get_next_byte(&origin) - v7_state.base;
        block_size = v7_save_and_verify_block(pool, mpool_get_pri_block_encoded_chunks(cpb), v7_state.base, block_size,
                                              pri->crctype, pri->crcval);

        if (block_size > 0)
        {
            result = block_size;
        }
        else
        {
            result = -1;
        }
    }

    return result;
}

int v7_block_decode_canonical(mpool_t *pool, mpool_cache_canonical_block_t *ccb, const void *data_ptr, size_t data_size)
{
    v7_decode_state_t            v7_state;
    CborValue                    origin;
    bp_canonical_block_buffer_t *logical;
    size_t                       block_size;
    CborParser                   parser;
    const void                  *content_ptr;
    size_t                       content_offset;
    size_t                       content_size;
    int                          result;

    logical = mpool_get_canonical_block_logical(ccb);
    memset(&v7_state, 0, sizeof(v7_state));

    if (cbor_parser_init(data_ptr, data_size, 0, &parser, &origin) != CborNoError)
    {
        v7_state.error = true;
    }
    else
    {
        v7_state.base = data_ptr;
        v7_state.cbor = &origin;

        v7_decode_bp_canonical_block_buffer(&v7_state, logical, &content_ptr, &content_size);
    }

    if (v7_state.error)
    {
        result = -1;
    }
    else
    {
        content_offset = (const uint8_t *)content_ptr - v7_state.base;
        block_size     = cbor_value_get_next_byte(&origin) - v7_state.base;
        block_size =
            v7_save_and_verify_block(pool, mpool_get_canonical_block_encoded_chunks(ccb), v7_state.base, block_size,
                                     logical->canonical_block.crctype, logical->canonical_block.crcval);

        if (block_size > 0)
        {
            mpool_set_canonical_block_encoded_content_detail(ccb, content_offset, content_size);
            result = block_size;

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
                    default:
                        /* do nothing */
                        break;
                }
            }

            /* if second stage decode didn't work, then flag the whole block as bad */
            if (v7_state.error)
            {
                result = -1;
            }
        }
        else
        {
            result = -1;
        }
    }

    return result;
}

int v7_block_encode_pri(mpool_t *pool, mpool_cache_primary_block_t *cpb)
{
    v7_encode_state_t         v7_state;
    CborEncoder               origin;
    const bp_primary_block_t *pri;
    int                       result;

    pri = mpool_get_pri_block_logical(cpb);
    memset(&v7_state, 0, sizeof(v7_state));

    mpool_start_stream_init(&v7_state.mps, pool, mpool_stream_dir_write, pri->crctype);
    cbor_encoder_init_writer(&origin, v7_encoder_write, &v7_state);
    v7_state.cbor = &origin;

    v7_encode_bp_primary_block(&v7_state, pri);

    if (!v7_state.error)
    {
        result = mpool_stream_tell(&v7_state.mps);
        mpool_stream_attach(&v7_state.mps, mpool_get_pri_block_encoded_chunks(cpb));
    }
    else
    {
        result = -1;
    }

    mpool_stream_close(&v7_state.mps);
    return result;
}

int v7_block_encode_pay(mpool_t *pool, mpool_cache_canonical_block_t *ccb, const void *data_ptr, size_t data_size)
{
    v7_encode_state_t                  v7_state;
    CborEncoder                        origin;
    const bp_canonical_block_buffer_t *pay;
    int                                result;

    pay = mpool_get_canonical_block_logical(ccb);
    memset(&v7_state, 0, sizeof(v7_state));
    mpool_start_stream_init(&v7_state.mps, pool, mpool_stream_dir_write, pay->canonical_block.crctype);
    cbor_encoder_init_writer(&origin, v7_encoder_write, &v7_state);
    v7_state.cbor = &origin;

    v7_encode_bp_canonical_block_buffer(&v7_state, pay, data_ptr, data_size);

    if (!v7_state.error)
    {
        result = mpool_stream_tell(&v7_state.mps);
        mpool_stream_attach(&v7_state.mps, mpool_get_canonical_block_encoded_chunks(ccb));
    }
    else
    {
        result = -1;
    }

    mpool_stream_close(&v7_state.mps);
    return result;
}

int v7_block_encode_canonical(mpool_t *pool, mpool_cache_canonical_block_t *ccb)
{
    v7_encode_state_t                  v7_state;
    CborEncoder                        origin;
    const bp_canonical_block_buffer_t *logical;
    int                                result;
    uint8_t                            scratch_area[128];
    size_t                             scratch_size;

    /*
     * Encoding of a V7 extension block is a two-part affair, it must
     * first encode the block-specific information into CBOR, then wrap
     * those encoded octets as a CBOR byte string within the extension block.
     * This effectively makes it CBOR-in-CBOR.
     */
    logical = mpool_get_canonical_block_logical(ccb);

    memset(&v7_state, 0, sizeof(v7_state));
    mpool_start_stream_init(&v7_state.mps, pool, mpool_stream_dir_write, logical->canonical_block.crctype);
    cbor_encoder_init(&origin, scratch_area, sizeof(scratch_area), 0);

    v7_state.cbor = &origin;

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
        default:
            /* do nothing */
            break;
    }

    if (!v7_state.error)
    {
        scratch_size = cbor_encoder_get_buffer_size(&origin, scratch_area);
        if (scratch_size == 0)
        {
            result = -1;
        }
        else
        {
            /*
             * Do second-stage encode - take the scratch buffer and use it as the content of the extension block
             */
            mpool_start_stream_init(&v7_state.mps, pool, mpool_stream_dir_write, logical->canonical_block.crctype);
            cbor_encoder_init_writer(&origin, v7_encoder_write, &v7_state);
            v7_state.cbor = &origin;

            v7_encode_bp_canonical_block_buffer(&v7_state, logical, scratch_area, scratch_size);

            if (!v7_state.error)
            {
                result = mpool_stream_tell(&v7_state.mps);
                mpool_stream_attach(&v7_state.mps, mpool_get_canonical_block_encoded_chunks(ccb));
            }
            else
            {
                result = -1;
            }

            mpool_stream_close(&v7_state.mps);
        }
    }

    return result;
}
