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

typedef enum bp_pri_field
{
    bp_pri_field_undef,
    bp_pri_field_version,
    bp_pri_field_flags,
    bp_pri_field_crctype,
    bp_pri_field_destid,
    bp_pri_field_sourceid,
    bp_pri_field_reportid,
    bp_pri_field_timestamp,
    bp_pri_field_lifetime,
    bp_pri_field_fragmentoffset,
    bp_pri_field_totallength,
    bp_pri_field_crcvalue,
    bp_pri_field_done
} bp_pri_field_t;

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

/*
 * -----------------------------------------------------------------------------------
 * IMPLEMENTATION
 * Helpers for encoding/decoding of individual protocol elements
 * -----------------------------------------------------------------------------------
 */

void v7_encode_bp_bundle_processing_control_flags(v7_encode_state_t *enc, const bp_bundle_processing_control_flags_t *v)
{
    v7_encode_bitmap(enc, (const uint8_t *)v, V7_BUNDLE_CONTROL_FLAGS_BITMAP_TABLE);
}

void v7_encode_bp_sequencenumber(v7_encode_state_t *enc, const bp_sequencenumber_t *v)
{
    v7_encode_bp_integer(enc, v);
}

void v7_encode_bp_creation_timestamp_impl(v7_encode_state_t *enc, const void *arg)
{
    const bp_creation_timestamp_t *v = arg;

    v7_encode_bp_dtntime(enc, &v->time);
    v7_encode_bp_sequencenumber(enc, &v->sequence_num);
}

void v7_encode_bp_creation_timestamp(v7_encode_state_t *enc, const bp_creation_timestamp_t *v)
{
    v7_encode_container(enc, 2, v7_encode_bp_creation_timestamp_impl, v);
}

void v7_encode_bp_lifetime(v7_encode_state_t *enc, const bp_lifetime_t *v)
{
    v7_encode_bp_integer(enc, v);
}

void v7_encode_bp_adu_length(v7_encode_state_t *enc, const bp_adu_length_t *v)
{
    v7_encode_bp_integer(enc, v);
}

void v7_encode_bp_primary_block_impl(v7_encode_state_t *enc, const void *arg)
{
    const bp_primary_block_t *v        = arg;
    bp_pri_field_t            field_id = bp_pri_field_undef;

    while (field_id < bp_pri_field_done && !enc->error)
    {
        switch (field_id)
        {
            case bp_pri_field_version:
                v7_encode_small_int(enc, v->version);
                if (v->version != 7)
                {
                    /* don't know how to encode the rest if not v7 */
                    enc->error = true;
                }
                break;
            case bp_pri_field_flags:
                v7_encode_bp_bundle_processing_control_flags(enc, &v->controlFlags);
                break;
            case bp_pri_field_crctype:
                v7_encode_bp_crctype(enc, &v->crctype);
                break;
            case bp_pri_field_destid:
                v7_encode_bp_endpointid_buffer(enc, &v->destinationEID);
                break;
            case bp_pri_field_sourceid:
                v7_encode_bp_endpointid_buffer(enc, &v->sourceEID);
                break;
            case bp_pri_field_reportid:
                v7_encode_bp_endpointid_buffer(enc, &v->reportEID);
                break;
            case bp_pri_field_timestamp:
                v7_encode_bp_creation_timestamp(enc, &v->creationTimeStamp);
                break;
            case bp_pri_field_lifetime:
                v7_encode_bp_lifetime(enc, &v->lifetime);
                break;
            case bp_pri_field_fragmentoffset:
                /* depends on whether the flag is set */
                if (v->controlFlags.isFragment)
                {
                    v7_encode_bp_adu_length(enc, &v->fragmentOffset);
                }
                break;
            case bp_pri_field_totallength:
                /* depends on whether the flag is set */
                if (v->controlFlags.isFragment)
                {
                    v7_encode_bp_adu_length(enc, &v->totalADUlength);
                }
                break;
            case bp_pri_field_crcvalue:
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
    enc->crc_params = v7_codec_get_crc_algorithm(v->crctype);
    enc->crc_val    = bplib_crc_initial_value(enc->crc_params);

    v7_encode_container(enc, num_fields, v7_encode_bp_primary_block_impl, v);
}

void v7_decode_bp_sequencenumber(v7_decode_state_t *dec, bp_sequencenumber_t *v)
{
    v7_decode_bp_integer(dec, v);
}

void v7_decode_bp_creation_timestamp_impl(v7_decode_state_t *dec, void *arg)
{
    bp_creation_timestamp_t *v = arg;

    v7_decode_bp_dtntime(dec, &v->time);
    v7_decode_bp_sequencenumber(dec, &v->sequence_num);
}

void v7_decode_bp_creation_timestamp(v7_decode_state_t *dec, bp_creation_timestamp_t *v)
{
    v7_decode_container(dec, 2, v7_decode_bp_creation_timestamp_impl, v);
}

void v7_decode_bp_bundle_processing_control_flags(v7_decode_state_t *dec, bp_bundle_processing_control_flags_t *v)
{
    v7_decode_bitmap(dec, (uint8_t *)v, V7_BUNDLE_CONTROL_FLAGS_BITMAP_TABLE);
}

void v7_decode_bp_lifetime(v7_decode_state_t *dec, bp_lifetime_t *v)
{
    v7_decode_bp_integer(dec, v);
}

void v7_decode_bp_adu_length(v7_decode_state_t *dec, bp_adu_length_t *v)
{
    v7_decode_bp_integer(dec, v);
}

void v7_decode_bp_primary_block_impl(v7_decode_state_t *dec, void *arg)
{
    bp_primary_block_t *v        = arg;
    bp_pri_field_t      field_id = bp_pri_field_undef;

    while (field_id < bp_pri_field_done && !dec->error && !cbor_value_at_end(dec->cbor))
    {
        switch (field_id)
        {
            case bp_pri_field_version:
                v->version = (uint8_t)v7_decode_small_int(dec);
                if (v->version != 7)
                {
                    /* don't know how to decode the rest if not v7 */
                    dec->error = true;
                }
                break;
            case bp_pri_field_flags:
                v7_decode_bp_bundle_processing_control_flags(dec, &v->controlFlags);
                break;
            case bp_pri_field_crctype:
                v7_decode_bp_crctype(dec, &v->crctype);
                break;
            case bp_pri_field_destid:
                v7_decode_bp_endpointid_buffer(dec, &v->destinationEID);
                break;
            case bp_pri_field_sourceid:
                v7_decode_bp_endpointid_buffer(dec, &v->sourceEID);
                break;
            case bp_pri_field_reportid:
                v7_decode_bp_endpointid_buffer(dec, &v->reportEID);
                break;
            case bp_pri_field_timestamp:
                v7_decode_bp_creation_timestamp(dec, &v->creationTimeStamp);
                break;
            case bp_pri_field_lifetime:
                v7_decode_bp_lifetime(dec, &v->lifetime);
                break;
            case bp_pri_field_fragmentoffset:
                /* depends on whether the flag is set */
                if (v->controlFlags.isFragment)
                {
                    v7_decode_bp_adu_length(dec, &v->fragmentOffset);
                }
                else
                {
                    v->fragmentOffset = 0;
                }
                break;
            case bp_pri_field_totallength:
                /* depends on whether the flag is set */
                if (v->controlFlags.isFragment)
                {
                    v7_decode_bp_adu_length(dec, &v->totalADUlength);
                }
                else
                {
                    v->totalADUlength = 0;
                }
                break;
            case bp_pri_field_crcvalue:
                /* decode the CRC if it has one. */
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

void v7_decode_bp_primary_block(v7_decode_state_t *dec, bp_primary_block_t *v)
{
    v7_decode_container(dec, CborIndefiniteLength, v7_decode_bp_primary_block_impl, v);
}
