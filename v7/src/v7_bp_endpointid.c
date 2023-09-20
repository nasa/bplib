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
#include "v7_decode_internal.h"

typedef enum bp_endpointid_field
{
    bp_endpointid_field_undef,
    bp_endpointid_field_scheme,
    bp_endpointid_field_ssp,
    bp_endpointid_field_done
} bp_endpointid_field_t;

/*
 * -----------------------------------------------------------------------------------
 * IMPLEMENTATION
 * Helpers for encoding/decoding of individual protocol elements
 * -----------------------------------------------------------------------------------
 */

void v7_encode_bp_endpointid_scheme(v7_encode_state_t *enc, const bp_endpointid_scheme_t *v)
{
    v7_encode_small_int(enc, *v);
}

void v7_encode_bp_ipn_nodenumber(v7_encode_state_t *enc, const bp_ipn_nodenumber_t *v)
{
    v7_encode_bp_integer(enc, v);
}

void v7_encode_bp_ipn_servicenumber(v7_encode_state_t *enc, const bp_ipn_servicenumber_t *v)
{
    v7_encode_bp_integer(enc, v);
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

void v7_encode_bp_dtn_uri_ssp_impl(v7_encode_state_t *enc, const void *arg)
{
    const bp_dtn_uri_ssp_t *v = arg;
    bp_integer_t            well_known_value;

    if (v->is_none)
    {
        well_known_value = 0;
        v7_encode_bp_integer(enc, &well_known_value);
    }
    else
    {
        /* do not know how to encode */
        enc->error = true;
    }
}

void v7_encode_bp_dtn_uri_ssp(v7_encode_state_t *enc, const bp_dtn_uri_ssp_t *v)
{
    v7_encode_container(enc, 2, v7_encode_bp_dtn_uri_ssp_impl, v);
}

void v7_encode_bp_endpointid_ssp(v7_encode_state_t *enc, bp_endpointid_scheme_t scheme, const bp_endpointid_ssp_t *v)
{
    switch (scheme)
    {
        case bp_endpointid_scheme_ipn:
            v7_encode_bp_ipn_uri_ssp(enc, &v->ipn);
            break;
        case bp_endpointid_scheme_dtn:
            v7_encode_bp_dtn_uri_ssp(enc, &v->dtn);
            break;
        default:
            /* do not know how to decode */
            enc->error = true;
            break;
    }
}

void v7_encode_bp_endpointid_buffer_impl(v7_encode_state_t *enc, const void *arg)
{
    const bp_endpointid_buffer_t *v        = arg;
    bp_endpointid_field_t         field_id = bp_endpointid_field_undef;

    while (field_id < bp_endpointid_field_done && !enc->error)
    {
        switch (field_id)
        {
            case bp_endpointid_field_scheme:
                v7_encode_bp_endpointid_scheme(enc,
                                               &v->scheme); /* always present, indicates which other field is valid */
                break;
            case bp_endpointid_field_ssp:
                v7_encode_bp_endpointid_ssp(enc, v->scheme, &v->ssp);
                break;
            default:
                break;
        }

        ++field_id;
    }
}

void v7_encode_bp_endpointid_buffer(v7_encode_state_t *enc, const bp_endpointid_buffer_t *v)
{
    v7_encode_container(enc, 2, v7_encode_bp_endpointid_buffer_impl, v);
}

void v7_decode_bp_endpointid_scheme(v7_decode_state_t *dec, bp_endpointid_scheme_t *v)
{
    *v = v7_decode_small_int(dec);
}

void v7_decode_bp_ipn_nodenumber(v7_decode_state_t *dec, bp_ipn_nodenumber_t *v)
{
    v7_decode_bp_integer(dec, v);
}

void v7_decode_bp_ipn_servicenumber(v7_decode_state_t *dec, bp_ipn_servicenumber_t *v)
{
    v7_decode_bp_integer(dec, v);
}

void v7_decode_bp_ipn_uri_ssp_impl(v7_decode_state_t *dec, void *arg)
{
    bp_ipn_uri_ssp_t *v = arg;

    v7_decode_bp_ipn_nodenumber(dec, &v->node_number);
    v7_decode_bp_ipn_servicenumber(dec, &v->service_number);
}

void v7_decode_bp_ipn_uri_ssp(v7_decode_state_t *dec, bp_ipn_uri_ssp_t *v)
{
    v7_decode_container(dec, 2, v7_decode_bp_ipn_uri_ssp_impl, v);
}

void v7_decode_bp_dtn_uri_ssp(v7_decode_state_t *dec, bp_dtn_uri_ssp_t *v)
{
    bp_integer_t well_known_value;

    /*
     * The only dtn scheme value recognized is the special value "none".
     * This is indicated by an integer value of 0.  Anything else constitutes
     * a decoding error due to an unsupported address type.
     */
    v7_decode_bp_integer(dec, &well_known_value);
    v->is_none = (!dec->error && well_known_value == 0);

    if (!v->is_none)
    {
        dec->error = true;
    }
}

void v7_decode_bp_endpointid_ssp(v7_decode_state_t *dec, bp_endpointid_scheme_t scheme, bp_endpointid_ssp_t *v)
{
    switch (scheme)
    {
        case bp_endpointid_scheme_ipn:
            v7_decode_bp_ipn_uri_ssp(dec, &v->ipn);
            break;
        case bp_endpointid_scheme_dtn:
            v7_decode_bp_dtn_uri_ssp(dec, &v->dtn);
            break;
        default:
            /* do not know how to decode */
            dec->error = true;
            break;
    }
}

void v7_decode_bp_endpointid_buffer_impl(v7_decode_state_t *dec, void *arg)
{
    bp_endpointid_buffer_t *v        = arg;
    bp_endpointid_field_t   field_id = bp_endpointid_field_undef;

    while (field_id < bp_endpointid_field_done && !dec->error && !cbor_value_at_end(dec->cbor))
    {
        switch (field_id)
        {
            case bp_endpointid_field_scheme:
                v7_decode_bp_endpointid_scheme(dec,
                                               &v->scheme); /* always present, indicates which other field is valid */
                break;
            case bp_endpointid_field_ssp:
                v7_decode_bp_endpointid_ssp(dec, v->scheme, &v->ssp);
                break;
            default:
                break;
        }

        ++field_id;
    }
}

void v7_decode_bp_endpointid_buffer(v7_decode_state_t *dec, bp_endpointid_buffer_t *v)
{
    v7_decode_container(dec, 2, v7_decode_bp_endpointid_buffer_impl, v);
}
