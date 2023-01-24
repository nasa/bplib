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
 * IMPLEMENTATION
 * Helpers for encoding/decoding of individual protocol elements
 * -----------------------------------------------------------------------------------
 */
void v7_encode_bp_adminrec_payload_impl(v7_encode_state_t *enc, const void *arg)
{
    const v7_admin_rec_payload_encode_info_t *admin_rec_payload;

    admin_rec_payload = arg;
    v7_encode_small_int(enc, admin_rec_payload->encode_rectype);

    if (!enc->error)
    {
        switch (admin_rec_payload->encode_rectype)
        {
            case bp_adminrectype_custodyAcknowledgement:
                v7_encode_bp_custody_acknowledement_record(
                    enc, &admin_rec_payload->payload_data->custody_accept_payload_block);
                break;
            default:
                /* missing implementation */
                enc->error = true;
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

void v7_decode_bp_adminrec_payload_impl(v7_decode_state_t *dec, void *arg)
{
    v7_admin_rec_payload_decode_info_t *admin_rec_payload;

    admin_rec_payload                 = arg;
    admin_rec_payload->decode_rectype = v7_decode_small_int(dec);

    if (!dec->error)
    {
        switch (admin_rec_payload->decode_rectype)
        {
            case bp_adminrectype_custodyAcknowledgement:
                v7_decode_bp_custody_acknowledement_record(
                    dec, &admin_rec_payload->payload_data->custody_accept_payload_block);
                break;
            default:
                /* missing implementation */
                dec->error = true;
                break;
        }
    }
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
