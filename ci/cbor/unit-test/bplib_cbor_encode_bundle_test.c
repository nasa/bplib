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
** Primary Block: 
**         CRC Type: 1
**         Flags: 4
**         Dest EID (scheme.node.service): 2.200.1
**         Source EID (scheme.node.service): 2.100.1
**         Report-To EID (scheme.node.service): 2.100.1
**         Timestamp (created, seq): 755533838904, 0
**         Lifetime: 3600000
**         CRC Value: 0xB19
** Canonical Block [0]: 
**         Block Type: 1
**         Block Number: 1
**         Flags: 0
**         CRC Type: 1
**         Offset Into Encoded Bundle: 42
*/
unsigned char const primary_and_payload_with_aa_x_20[] = {
    0x9f, 0x89, 0x07, 0x04, 0x01, 0x82, 0x02, 0x82, 
    0x18, 0xc8, 0x01, 0x82, 0x02, 0x82, 0x18, 0x64, 
    0x01, 0x82, 0x02, 0x82, 0x18, 0x64, 0x01, 0x82, 
    0x1b, 0x00, 0x00, 0x00, 0xaf, 0xe9, 0x53, 0x7a, 
    0x38, 0x00, 0x1a, 0x00, 0x36, 0xee, 0x80, 0x42, 
    0x0b, 0x19, 0x86, 0x01, 0x01, 0x00, 0x01, 0x54, 
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
    0xaa, 0xaa, 0xaa, 0xaa, 0x42, 0xc6, 0x8f, 0xff,
};


void Test_BPLib_CBOR_EncodeBundle_InputBundleNullError(void)
{
    BPLib_Status_t ReturnStatus;
    // BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[2048];
    size_t OutputSize;

    ReturnStatus = BPLib_CBOR_EncodeBundle(NULL,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
}

void Test_BPLib_CBOR_EncodeBundle_InputBundleBlobNullError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[2048];
    size_t OutputSize;

    memset(&InputBundle, 0, sizeof(InputBundle));
    InputBundle.blob = NULL;

    ReturnStatus = BPLib_CBOR_EncodeBundle(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
}


void Test_BPLib_CBOR_EncodeBundle_OutputBundleBufNullError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[2048];
    size_t OutputSize;

    ReturnStatus = BPLib_CBOR_EncodeBundle(&InputBundle,
                                        NULL,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
}


void Test_BPLib_CBOR_EncodeBundle_OutputSizeBufNullError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[2048];
    // size_t OutputSize;

    ReturnStatus = BPLib_CBOR_EncodeBundle(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        NULL);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
}


void Test_BPLib_CBOR_EncodeBundle_PrimaryCopySizeGtOutputError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[1024];
    size_t OutputSize = 0xdeadbeef;
    BPLib_MEM_Block_t FirstBlock;

    memset(&InputBundle, 0, sizeof(InputBundle));
    memset(&FirstBlock, 0, sizeof(FirstBlock));
    InputBundle.blob = &FirstBlock;

    /* Set up for first `EncodedSize` error check */
    InputBundle.blocks.PrimaryBlock.RequiresEncode = false;
    InputBundle.blocks.PrimaryBlock.EncodedSize = sizeof(OutputBuffer) + 1;


    ReturnStatus = BPLib_CBOR_EncodeBundle(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_EQ(BPLib_Status_t, ReturnStatus, BPLIB_BI_COPY_PRIME_ENC_SIZE_GT_OUTPUT_ERR);
    // TODO: Update OutputSize verification
    // UtAssert_EQ(size_t, OutputSize, (size_t) 0);
    UtAssert_STUB_COUNT(BPLib_MEM_CopyOutFromOffset, 0);
}


void Test_BPLib_CBOR_EncodeBundle_PrimaryCopySizeGtUserLenError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[1024];
    size_t OutputSize = 0xdeadbeef;
    BPLib_MEM_Block_t FirstBlock;

    memset(&InputBundle, 0, sizeof(InputBundle));
    memset(&FirstBlock, 0, sizeof(FirstBlock));
    InputBundle.blob = &FirstBlock;

    /* Set up for second `EncodedSize` error check (EncodedSize > used_len) */
    InputBundle.blocks.PrimaryBlock.RequiresEncode = false;
    InputBundle.blocks.PrimaryBlock.EncodedSize = 10;
    InputBundle.blob->used_len = InputBundle.blocks.PrimaryBlock.EncodedSize - 1;

    ReturnStatus = BPLib_CBOR_EncodeBundle(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_EQ(BPLib_Status_t, ReturnStatus, BPLIB_BI_COPY_PRIME_ENC_SIZE_GT_USER_DATA_ERR);
    // TODO: Update OutputSize verification
    // UtAssert_EQ(size_t, OutputSize, (size_t) 0);
    UtAssert_STUB_COUNT(BPLib_MEM_CopyOutFromOffset, 0);
}


void Test_BPLib_CBOR_EncodeBundle_PayloadCopyLenError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[70]; // less than sizeof(primary_and_payload_with_aa_x_20)
    size_t OutputSize = 0xdeadbeef;
    BPLib_MEM_Block_t FirstBlock;

    memset(&InputBundle, 0, sizeof(InputBundle));
    memset(&FirstBlock, 0, sizeof(FirstBlock));
    InputBundle.blob = &FirstBlock;

    /* Set up for nominal case */
    InputBundle.blocks.PrimaryBlock.RequiresEncode = false;
    InputBundle.blocks.PayloadHeader.RequiresEncode = false;

    memcpy(&InputBundle.blob->user_data.raw_bytes,
        primary_and_payload_with_aa_x_20,
        sizeof(primary_and_payload_with_aa_x_20));

    InputBundle.blocks.PrimaryBlock.EncodedSize = 42; // primary should be this size
    InputBundle.blob->used_len = sizeof(primary_and_payload_with_aa_x_20);


    InputBundle.blocks.PrimaryBlock.TotalAduLength = sizeof(primary_and_payload_with_aa_x_20)
                                                   - InputBundle.blocks.PrimaryBlock.EncodedSize;

    ReturnStatus = BPLib_CBOR_EncodeBundle(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_EQ(BPLib_Status_t, ReturnStatus, BPLIB_BI_COPY_PAYLOAD_ENC_SIZE_GT_OUTPUT_ERR);
    // TODO: Update OutputSize verification
    // UtAssert_EQ(size_t, OutputSize, InputBundle.blocks.PrimaryBlock.EncodedSize);
    UtAssert_STUB_COUNT(BPLib_MEM_CopyOutFromOffset, 0);
}


void Test_BPLib_CBOR_EncodeBundle_Nominal(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[1024];
    size_t OutputSize = 0xdeadbeef;
    BPLib_MEM_Block_t FirstBlock;

    memset(&InputBundle, 0, sizeof(InputBundle));
    memset(&FirstBlock, 0, sizeof(FirstBlock));
    InputBundle.blob = &FirstBlock;

    /* Set up for nominal case */
    InputBundle.blocks.PrimaryBlock.RequiresEncode = false;
    InputBundle.blocks.PayloadHeader.RequiresEncode = false;

    memcpy(&InputBundle.blob->user_data.raw_bytes,
        primary_and_payload_with_aa_x_20,
        sizeof(primary_and_payload_with_aa_x_20));

    InputBundle.blocks.PrimaryBlock.EncodedSize = 42; // primary should be this size
    InputBundle.blob->used_len = sizeof(primary_and_payload_with_aa_x_20);


    InputBundle.blocks.PrimaryBlock.TotalAduLength = sizeof(primary_and_payload_with_aa_x_20)
                                                   - InputBundle.blocks.PrimaryBlock.EncodedSize;


    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_CopyOutFromOffset), BPLIB_SUCCESS);

    ReturnStatus = BPLib_CBOR_EncodeBundle(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_EQ(BPLib_Status_t, ReturnStatus, BPLIB_SUCCESS);
    // TODO: Update OutputSize verification
    // UtAssert_EQ(size_t, OutputSize, sizeof(primary_and_payload_with_aa_x_20));
    UtAssert_STUB_COUNT(BPLib_MEM_CopyOutFromOffset, 1);
}





void TestBplibCborEncode_Register(void)
{
    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_InputBundleNullError, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_InputBundleNullError");
    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_InputBundleBlobNullError, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_InputBundleBlobNullError");
    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_OutputBundleBufNullError, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_OutputBundleBufNullError");
    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_OutputSizeBufNullError, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_OutputSizeBufNullError");

    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_PrimaryCopySizeGtOutputError, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_PrimaryCopySizeGtOutputError");
    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_PrimaryCopySizeGtUserLenError, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_PrimaryCopySizeGtUserLenError");

    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_PayloadCopyLenError, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_PayloadCopyLenError");

    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_Nominal, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_Nominal");
}
