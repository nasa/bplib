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

void v7_encode_bp_endpointid_buffer_impl(v7_encode_state_t *enc, const void *arg)
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

void v7_decode_bp_endpointid_buffer_impl(v7_decode_state_t *dec, void *arg)
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
