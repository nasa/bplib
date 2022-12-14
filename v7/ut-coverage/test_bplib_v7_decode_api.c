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
#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"
#include "v7_codec.h"
#include "v7_mpool_bblocks.h"
#include "v7_mpool_ref.h"
#include "v7_types.h"
#include "crc.h"
#include <stdlib.h>

#include "test_bplib_v7.h"

#define BPCAT_BUNDLE_BUFFER_SIZE 16384

void Test_v7_block_decode_pri(void)
{
    size_t                       remain_sz = 10;
    bplib_mpool_bblock_primary_t cpb;
    uint8_t                      in_p[3][3] = {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}};
    cpb.data.logical.version                = 7;
    cpb.data.logical.crctype                = bp_crctype_CRC32C;
    cpb.data.logical.destinationEID         = (bp_endpointid_buffer_t) {.scheme = bp_endpointid_scheme_ipn};
    cpb.data.logical.sourceEID              = (bp_endpointid_buffer_t) {.scheme = bp_endpointid_scheme_ipn};
    cpb.data.logical.reportEID              = (bp_endpointid_buffer_t) {.scheme = bp_endpointid_scheme_ipn};
    UtAssert_INT32_NEQ(v7_block_decode_pri(&cpb, &in_p, remain_sz), 0);
}

void Test_v7_block_decode_canonical(void)
{
    bplib_mpool_bblock_canonical_t ccb;
    bp_blocktype_t                 payload_block_hint = bp_blocktype_payloadBlock;
    size_t                         remain_sz          = 10;
    uint8_t                        in_p[3][3]         = {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}};
    remain_sz                                         = sizeof(in_p);
    UtAssert_INT32_NEQ(v7_block_decode_canonical(&ccb, &in_p, remain_sz, payload_block_hint), 0);
}

void TestBplibV7Decode_Register(void)
{
    UtTest_Add(Test_v7_block_decode_pri, NULL, NULL, "BPLIB v7 block_decode_pri");
    UtTest_Add(Test_v7_block_decode_canonical, NULL, NULL, "BPLIB V7 block_decode_canonical");
}
