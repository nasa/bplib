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
Primary Block: 
         CRC Type: 1
         Flags: 4
         Dest EID (scheme.node.service): 2.200.1
         Source EID (scheme.node.service): 2.100.1
         Report-To EID (scheme.node.service): 2.100.1
         Timestamp (created, seq): 755533838904, 0
         Lifetime: 3600000
         CRC Value: 0xB19
*/
uint8_t CandPrimary[] = {
    0x9f, 0x89, 0x07, 0x04, 0x01, 0x82, 0x02, 0x82, 
    0x18, 0xc8, 0x01, 0x82, 0x02, 0x82, 0x18, 0x64, 
    0x01, 0x82, 0x02, 0x82, 0x18, 0x64, 0x01, 0x82, 
    0x1b, 0x00, 0x00, 0x00, 0xaf, 0xe9, 0x53, 0x7a, 
    0x38, 0x00, 0x1a, 0x00, 0x36, 0xee, 0x80, 0x42, 
    0x0b, 0x19,
};

/*
Canonical Block [0]: 
         Block Type: 1
         Block Number: 1
         Flags: 0
         CRC Type: 1
         CRC Value: 0xc68f
*/
uint8_t CandPayload[] = {
    0x86, 0x01, 0x01, 0x00, 0x01, 0x54, 
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
    0xaa, 0xaa, 0xaa, 0xaa, 0x42, 0xc6, 0x8f,
};

/* Test an invalid CRC-16 in the primary block */
void Test_BPLib_CBOR_DecodePrimary_InvalidCrc(void)
{
    BPLib_Bundle_t Bundle;
    QCBORDecodeContext ctx;
    UsefulBufC UBufC;
    QCBORItem OuterArr;

    /* Initialize QCBOR context */
    UBufC.ptr = (const void *)(CandPrimary);
    UBufC.len = sizeof(CandPrimary);
    QCBORDecode_Init(&ctx, UBufC, QCBOR_DECODE_MODE_NORMAL);
    QCBORDecode_EnterArray(&ctx, &OuterArr);    

    /* Set CRC calculation to return different CRC from what's in the primary block */
    UT_SetDeferredRetcode(UT_KEY(BPLib_CRC_Calculate), 1, 0xbeef);

    UtAssert_INT32_EQ(BPLib_CBOR_DecodePrimary(&ctx, &Bundle, CandPrimary), BPLIB_INVALID_CRC_ERROR);    
}

/* Test an invalid CRC-16 in the payload block */
void Test_BPLib_CBOR_DecodeCanonical_InvalidCrc(void)
{
    BPLib_Bundle_t Bundle;
    QCBORDecodeContext ctx;
    UsefulBufC UBufC;

    /* Initialize QCBOR context */
    UBufC.ptr = (const void*)(CandPayload);
    UBufC.len = sizeof(CandPayload);
    QCBORDecode_Init(&ctx, UBufC, QCBOR_DECODE_MODE_NORMAL);   

    /* Set CRC calculation to return different CRC from what's in the primary block */
    UT_SetDeferredRetcode(UT_KEY(BPLib_CRC_Calculate), 1, 0xbeef);

    UtAssert_INT32_EQ(BPLib_CBOR_DecodeCanonical(&ctx, &Bundle, 0, CandPayload), BPLIB_INVALID_CRC_ERROR);    
}


void TestBplibCborDecodeInternal_Register(void)
{
    UtTest_Add(Test_BPLib_CBOR_DecodePrimary_InvalidCrc, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_DecodePrimary_InvalidCrc");
    UtTest_Add(Test_BPLib_CBOR_DecodeCanonical_InvalidCrc, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_DecodeCanonical_InvalidCrc");

}
