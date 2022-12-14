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

/*
 * -----------------------------------------------------------------------------------
 * SMALL INT - helpers to encode/decode simple values and enums that are expected to
 * fit within the native "int" type
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

/*
 * -----------------------------------------------------------------------------------
 * BP_INTEGER - encode/decode generic instances of the "bp_integer_t" type
 * This value may exceed the range of the default int type
 * -----------------------------------------------------------------------------------
 */
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

/*
 * -----------------------------------------------------------------------------------
 * BP_BLOCKNUM - encode/decode instances of the "bp_blocknum_t" type
 * These are simple integers, not expected to be large values
 * -----------------------------------------------------------------------------------
 */
void v7_encode_bp_blocknum(v7_encode_state_t *enc, const bp_blocknum_t *v)
{
    v7_encode_small_int(enc, *v);
}

void v7_decode_bp_blocknum(v7_decode_state_t *dec, bp_blocknum_t *v)
{
    *v = (bp_blocknum_t)v7_decode_small_int(dec);
}

/*
 * -----------------------------------------------------------------------------------
 * BP_BLOCKTYPE - encode/decode instances of the "bp_blocktype_t" type
 * This is an enumeration, and thus not expected to be large values
 * -----------------------------------------------------------------------------------
 */
void v7_encode_bp_blocktype(v7_encode_state_t *enc, const bp_blocktype_t *v)
{
    v7_encode_small_int(enc, *v);
}

void v7_decode_bp_blocktype(v7_decode_state_t *dec, bp_blocktype_t *v)
{
    *v = (bp_blocktype_t)v7_decode_small_int(dec);
}

/*
 * -----------------------------------------------------------------------------------
 * BP_CRCTYPE - encode/decode instances of the "bp_crctype_t" type
 * This is an enumeration, and thus not expected to be large values
 * -----------------------------------------------------------------------------------
 */
void v7_encode_bp_crctype(v7_encode_state_t *enc, const bp_crctype_t *v)
{
    v7_encode_small_int(enc, *v);
}

void v7_decode_bp_crctype(v7_decode_state_t *dec, bp_crctype_t *v)
{
    *v = (bp_crctype_t)v7_decode_small_int(dec);
}

/*
 * -----------------------------------------------------------------------------------
 * BP_DTNTIME - encode/decode instances of the "bp_dtntime_t" type
 * This is a large integer value, potentially up to 64 bits in size.
 * -----------------------------------------------------------------------------------
 */
void v7_encode_bp_dtntime(v7_encode_state_t *enc, const bp_dtntime_t *v)
{
    v7_encode_bp_integer(enc, v);
}

void v7_decode_bp_dtntime(v7_decode_state_t *dec, bp_dtntime_t *v)
{
    v7_decode_bp_integer(dec, v);
}
