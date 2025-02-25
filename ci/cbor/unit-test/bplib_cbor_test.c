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

/*
 * Include
 */
#include "bplib_cbor_test_utils.h"

/*
** Test function for
** int BPLib_CBOR_Init()
*/
void Test_BPLib_CBOR_Init(void)
{
    UtAssert_INT32_EQ(BPLib_CBOR_Init(), BPLIB_SUCCESS);
}

void Test_BPLib_CBOR_DecodeBundle(void)
{
    // TODO Include bpnode config here or what? (For BPNODE_MEM_POOL_LEN)
    /**
     * \brief Size of BPLib's Memory Pool, in bytes
     */
    #define BPNODE_MEM_POOL_LEN               (16834u)

    // Good Bundle Data
    // Created with `xxd -i good-bundle.bin > good.c`
    unsigned char good_bundle_bin[] = {
    0x9f, 0x89, 0x07, 0x04, 0x01, 0x82, 0x02, 0x82, 0x18, 0xc8, 0x01, 0x82,
    0x02, 0x82, 0x18, 0x64, 0x01, 0x82, 0x02, 0x82, 0x18, 0x64, 0x01, 0x82,
    0x1b, 0x00, 0x00, 0x00, 0xaf, 0xe9, 0x53, 0x7a, 0x38, 0x00, 0x1a, 0x00,
    0x36, 0xee, 0x80, 0x42, 0x0b, 0x19, 0x86, 0x06, 0x06, 0x00, 0x01, 0x47,
    0x82, 0x02, 0x82, 0x19, 0x01, 0x2c, 0x02, 0x42, 0x25, 0xd4, 0x86, 0x07,
    0x02, 0x05, 0x01, 0x45, 0x1a, 0x00, 0x01, 0xa5, 0xe0, 0x42, 0x3a, 0xed,
    0x86, 0x0a, 0x03, 0x01, 0x01, 0x43, 0x82, 0x0f, 0x03, 0x42, 0xf8, 0x13,
    0x86, 0x0f, 0x04, 0x02, 0x01, 0x4a, 0x83, 0x0a, 0x02, 0x82, 0x02, 0x82,
    0x19, 0x01, 0x2f, 0x01, 0x42, 0x25, 0xc7, 0x86, 0x10, 0x05, 0x00, 0x01,
    0x52, 0x85, 0x01, 0x04, 0x00, 0x82, 0x02, 0x82, 0x19, 0x01, 0x2f, 0x01,
    0x82, 0x02, 0x82, 0x19, 0x01, 0x31, 0x02, 0x42, 0x66, 0xce, 0x86, 0x01,
    0x01, 0x00, 0x01, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
    0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x0a,
    0x42, 0x7a, 0x2f, 0xff
    };
    unsigned int good_bundle_bin_len = 160;

    uint8_t *bundle_blob = good_bundle_bin;
    size_t CandidateBundleLen = good_bundle_bin_len;
    BPLib_Bundle_t *CandidateBundle;
    BPLib_Bundle_t *bundle;  // The decoded bundle

    // TODO Instance init should move to bplib_cbor_test_utils.c
    BPLib_Instance_t inst;
    memset(&inst, 0, sizeof(BPLib_Instance_t));
    uint8_t *pool_mem;

    UtAssert_INT32_EQ(BPLib_MEM_PoolInit(&inst.pool, &pool_mem, (size_t)BPNODE_MEM_POOL_LEN), BPLIB_SUCCESS);

    CandidateBundle = BPLib_MEM_BundleAlloc(&inst.pool, bundle_blob, CandidateBundleLen);
    UtAssert_NOT_NULL(CandidateBundle);

    if (CandidateBundleLen == 0)
    {
        UtPrintf("BPLib_MEM_BundleAlloc set CandidateBundleLen to zero.");
        CandidateBundleLen = good_bundle_bin_len;
    }

    /* Decode Nominally */
    UtAssert_INT32_EQ(BPLib_CBOR_DecodeBundle(CandidateBundle, CandidateBundleLen, bundle), BPLIB_SUCCESS);

    /* Decode with error */
    UtAssert_INT32_EQ(BPLib_CBOR_DecodeBundle(CandidateBundle, 0, bundle), BPLIB_CBOR_DEC_ERR);
}

void TestBplibCbor_Register(void)
{
    UtTest_Add(Test_BPLib_CBOR_Init, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_Init");
    UtTest_Add(Test_BPLib_CBOR_DecodeBundle, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_TryBundleDecode");
}
