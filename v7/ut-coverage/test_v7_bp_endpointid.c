/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/*
 * Includes
 */
#include "test_bplib_v7.h"

void test_v7_encode_bp_endpointid_scheme(void)
{
    /* Test function for:
     * void v7_encode_bp_endpointid_scheme(v7_encode_state_t *enc, const bp_endpointid_scheme_t *v)
     */
    v7_encode_state_t      enc;
    bp_endpointid_scheme_t v;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&v, 0, sizeof(bp_endpointid_scheme_t));

    enc.error = true;
    UtAssert_VOIDCALL(v7_encode_bp_endpointid_scheme(&enc, &v));
}

void test_v7_encode_bp_ipn_nodenumber(void)
{
    /* Test function for:
     * void v7_encode_bp_ipn_nodenumber(v7_encode_state_t *enc, const bp_ipn_nodenumber_t *v)
     */
    v7_encode_state_t   enc;
    bp_ipn_nodenumber_t v;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&v, 0, sizeof(bp_ipn_nodenumber_t));

    enc.error = true;
    UtAssert_VOIDCALL(v7_encode_bp_ipn_nodenumber(&enc, &v));
}

void test_v7_encode_bp_ipn_servicenumber(void)
{
    /* Test function for:
     * void v7_encode_bp_ipn_servicenumber(v7_encode_state_t *enc, const bp_ipn_servicenumber_t *v)
     */
    v7_encode_state_t      enc;
    bp_ipn_servicenumber_t v;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&v, 0, sizeof(bp_ipn_servicenumber_t));

    enc.error = true;
    UtAssert_VOIDCALL(v7_encode_bp_ipn_servicenumber(&enc, &v));
}

void test_v7_encode_bp_ipn_uri_ssp_impl(void)
{
    /* Test function for:
     * void v7_encode_bp_ipn_uri_ssp_impl(v7_encode_state_t *enc, const void *arg)
     */
    v7_encode_state_t enc;
    bp_ipn_uri_ssp_t  arg;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&arg, 0, sizeof(bp_ipn_uri_ssp_t));

    enc.error = true;
    UtAssert_VOIDCALL(v7_encode_bp_ipn_uri_ssp_impl(&enc, &arg));
}

void test_v7_encode_bp_ipn_uri_ssp(void)
{
    /* Test function for:
     * void v7_encode_bp_ipn_uri_ssp(v7_encode_state_t *enc, const bp_ipn_uri_ssp_t *v)
     */
    v7_encode_state_t enc;
    bp_ipn_uri_ssp_t  v;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&v, 0, sizeof(bp_ipn_uri_ssp_t));

    enc.error = true;
    UtAssert_VOIDCALL(v7_encode_bp_ipn_uri_ssp(&enc, &v));
}

void test_v7_encode_bp_endpointid_buffer_impl(void)
{
    /* Test function for:
     * void v7_encode_bp_endpointid_buffer_impl(v7_encode_state_t *enc, const void *arg)
     */
    v7_encode_state_t      enc;
    bp_endpointid_buffer_t v;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&v, 0, sizeof(bp_endpointid_buffer_t));

    v.scheme = bp_endpointid_scheme_ipn;
    UtAssert_VOIDCALL(v7_encode_bp_endpointid_buffer_impl(&enc, &v));

    v.scheme = bp_endpointid_scheme_undefined;
    UtAssert_VOIDCALL(v7_encode_bp_endpointid_buffer_impl(&enc, &v));
}

void test_v7_decode_bp_endpointid_scheme(void)
{
    /* Test function for:
     * void v7_decode_bp_endpointid_scheme(v7_decode_state_t *dec, bp_endpointid_scheme_t *v)
     */
    v7_decode_state_t      dec;
    bp_endpointid_scheme_t v;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_endpointid_scheme_t));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_endpointid_scheme(&dec, &v));
}

void test_v7_decode_bp_ipn_nodenumber(void)
{
    /* Test function for:
     * void v7_decode_bp_ipn_nodenumber(v7_decode_state_t *dec, bp_ipn_nodenumber_t *v)
     */
    v7_decode_state_t   dec;
    bp_ipn_nodenumber_t v;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_ipn_nodenumber_t));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_ipn_nodenumber(&dec, &v));
}

void test_v7_decode_bp_ipn_servicenumber(void)
{
    /* Test function for:
     * void v7_decode_bp_ipn_servicenumber(v7_decode_state_t *dec, bp_ipn_servicenumber_t *v)
     */
    v7_decode_state_t      dec;
    bp_ipn_servicenumber_t v;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_ipn_servicenumber_t));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_ipn_servicenumber(&dec, &v));
}

void test_v7_decode_bp_ipn_uri_ssp(void)
{
    /* Test function for:
     * void v7_decode_bp_ipn_uri_ssp(v7_decode_state_t *dec, bp_ipn_uri_ssp_t *v)
     */
    v7_decode_state_t dec;
    bp_ipn_uri_ssp_t  v;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_ipn_uri_ssp_t));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_ipn_uri_ssp(&dec, &v));
}

void test_v7_decode_bp_ipn_uri_ssp_impl(void)
{
    /* Test function for:
     * void v7_decode_bp_ipn_uri_ssp_impl(v7_decode_state_t *dec, void *arg)
     */
    v7_decode_state_t   dec;
    bp_ipn_uri_ssp_t    arg;
    QCBORDecodeContext  cval;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&arg, 0, sizeof(bp_ipn_uri_ssp_t));
    memset(&cval, 0, sizeof(QCBORDecodeContext));

    dec.cbor = &cval;
    UtAssert_VOIDCALL(v7_decode_bp_ipn_uri_ssp_impl(&dec, &arg));
}

void test_v7_decode_bp_endpointid_buffer_impl(void)
{
    /* Test function for:
     * void v7_decode_bp_endpointid_buffer_impl(v7_decode_state_t *dec, void *arg)
     */
    v7_decode_state_t      dec;
    bp_endpointid_buffer_t arg;
    QCBORDecodeContext     cval;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&arg, 0, sizeof(bp_endpointid_buffer_t));
    memset(&cval, 0, sizeof(QCBORDecodeContext));

    dec.error = false;
    dec.cbor  = &cval;
    UtAssert_VOIDCALL(v7_decode_bp_endpointid_buffer_impl(&dec, &arg));

    dec.error      = false;
    UtAssert_VOIDCALL(v7_decode_bp_endpointid_buffer_impl(&dec, &arg));
}

void test_v7_decode_bp_endpointid_buffer(void)
{
    /* Test function for:
     * void v7_decode_bp_endpointid_buffer(v7_decode_state_t *dec, bp_endpointid_buffer_t *v)
     */
    v7_decode_state_t      dec;
    bp_endpointid_buffer_t v;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&v, 0, sizeof(bp_endpointid_buffer_t));

    dec.error = true;
    UtAssert_VOIDCALL(v7_decode_bp_endpointid_buffer(&dec, &v));
}

void TestV7BpEndpointid_Rgister(void)
{
    UtTest_Add(test_v7_encode_bp_endpointid_scheme, NULL, NULL, "Test v7_encode_bp_endpointid_scheme");
    UtTest_Add(test_v7_encode_bp_ipn_nodenumber, NULL, NULL, "Test v7_encode_bp_ipn_nodenumber");
    UtTest_Add(test_v7_encode_bp_ipn_servicenumber, NULL, NULL, "Test v7_encode_bp_ipn_servicenumber");
    UtTest_Add(test_v7_encode_bp_ipn_uri_ssp_impl, NULL, NULL, "Test v7_encode_bp_ipn_uri_ssp_impl");
    UtTest_Add(test_v7_encode_bp_ipn_uri_ssp, NULL, NULL, "Test v7_encode_bp_ipn_uri_ssp");
    UtTest_Add(test_v7_encode_bp_endpointid_buffer_impl, NULL, NULL, "Test v7_encode_bp_endpointid_buffer_impl");
    UtTest_Add(test_v7_decode_bp_endpointid_scheme, NULL, NULL, "Test v7_decode_bp_endpointid_scheme");
    UtTest_Add(test_v7_decode_bp_ipn_nodenumber, NULL, NULL, "Test v7_decode_bp_ipn_nodenumber");
    UtTest_Add(test_v7_decode_bp_ipn_servicenumber, NULL, NULL, "Test v7_decode_bp_ipn_servicenumber");
    UtTest_Add(test_v7_decode_bp_ipn_uri_ssp, NULL, NULL, "Test v7_decode_bp_ipn_uri_ssp");
    UtTest_Add(test_v7_decode_bp_ipn_uri_ssp_impl, NULL, NULL, "Test v7_decode_bp_ipn_uri_ssp_impl");
    UtTest_Add(test_v7_decode_bp_endpointid_buffer_impl, NULL, NULL, "Test v7_decode_bp_endpointid_buffer_impl");
    UtTest_Add(test_v7_decode_bp_endpointid_buffer, NULL, NULL, "Test v7_decode_bp_endpointid_buffer");
}
