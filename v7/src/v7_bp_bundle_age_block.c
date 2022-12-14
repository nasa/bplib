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

void v7_encode_bp_bundle_age_block(v7_encode_state_t *enc, const bp_bundle_age_block_t *v)
{
    v7_encode_bp_dtntime(enc, &v->age);
}

void v7_decode_bp_bundle_age_block(v7_decode_state_t *dec, bp_bundle_age_block_t *v)
{
    v7_decode_bp_dtntime(dec, &v->age);
}
