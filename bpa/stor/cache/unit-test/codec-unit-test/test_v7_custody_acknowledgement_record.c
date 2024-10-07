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

void test_v7_encode_bp_custody_acceptance_seqlist_impl(void)
{
    /* Test function for:
     * void v7_encode_bp_custody_acceptance_seqlist_impl(v7_encode_state_t *enc, const void *arg)
     */
    v7_encode_state_t                 enc;
    BPLIB_CT_AcceptPayloadBlock_t arg;

    memset(&enc, 0, sizeof(v7_encode_state_t));
    memset(&arg, 0, sizeof(BPLIB_CT_AcceptPayloadBlock_t));

    enc.error       = false;
    arg.num_entries = 1;
    UtAssert_VOIDCALL(v7_encode_bp_custody_acceptance_seqlist_impl(&enc, &arg));
}

void test_v7_decode_bp_custody_acceptance_seqlist_impl(void)
{
    /* Test function for:
     * void v7_decode_bp_custody_acceptance_seqlist_impl(v7_decode_state_t *dec, void *arg)
     */
    v7_decode_state_t                 dec;
    BPLIB_CT_AcceptPayloadBlock_t arg;
    CborValue                         cval;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&arg, 0, sizeof(BPLIB_CT_AcceptPayloadBlock_t));
    memset(&cval, 0, sizeof(CborValue));

    arg.num_entries = 10;
    dec.cbor        = &cval;
    cval.remaining  = 10;
    dec.error       = true;
    UtAssert_VOIDCALL(v7_decode_bp_custody_acceptance_seqlist_impl(&dec, &arg));
    dec.error = false;
    UtAssert_VOIDCALL(v7_decode_bp_custody_acceptance_seqlist_impl(&dec, &arg));
}

void test_v7_decode_bp_custody_acknowledement_record_impl(void)
{
    /* Test function for:
     * void v7_decode_bp_custody_acknowledement_record_impl(v7_decode_state_t *dec, void *arg)
     */
    v7_decode_state_t                 dec;
    BPLIB_CT_AcceptPayloadBlock_t arg;
    CborValue                         cval;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&arg, 0, sizeof(BPLIB_CT_AcceptPayloadBlock_t));
    memset(&cval, 0, sizeof(CborValue));

    dec.cbor = &cval;
    UtAssert_VOIDCALL(v7_decode_bp_custody_acknowledement_record_impl(&dec, &arg));
}

void test_v7_decode_bp_custody_acknowledement_record(void)
{
    /* Test function for:
     * void v7_decode_bp_custody_acknowledement_record(v7_decode_state_t *dec, BPLIB_CT_AcceptPayloadBlock_t *v)
     */
    v7_decode_state_t                 dec;
    BPLIB_CT_AcceptPayloadBlock_t arg;
    CborValue                         cval;

    memset(&dec, 0, sizeof(v7_decode_state_t));
    memset(&arg, 0, sizeof(BPLIB_CT_AcceptPayloadBlock_t));
    memset(&cval, 0, sizeof(CborValue));

    dec.cbor = &cval;
    UtAssert_VOIDCALL(v7_decode_bp_custody_acknowledement_record(&dec, &arg));
}

void TestV7CustodyAcknowledgementRecord_Rgister(void)
{
    UtTest_Add(test_v7_encode_bp_custody_acceptance_seqlist_impl, NULL, NULL,
               "Test v7_encode_bp_custody_acceptance_seqlist_impl");
    UtTest_Add(test_v7_decode_bp_custody_acceptance_seqlist_impl, NULL, NULL,
               "Test v7_decode_bp_custody_acceptance_seqlist_impl");
    UtTest_Add(test_v7_decode_bp_custody_acknowledement_record_impl, NULL, NULL,
               "Test v7_decode_bp_custody_acknowledement_record_impl");
    UtTest_Add(test_v7_decode_bp_custody_acknowledement_record, NULL, NULL,
               "Test v7_decode_bp_custody_acknowledement_record");
}
