/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
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


void Test_BPLib_CBOR_EncodeBundle_OutputBufLen1(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[2];
    size_t OutputSize;

    ReturnStatus = BPLib_CBOR_EncodeBundle(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_CBOR_ENC_BUNDLE_OUTPUT_BUF_LEN_1_ERR);
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

    /* Set up for encoded primary length error check */
    InputBundle.blocks.PrimaryBlock.RequiresEncode = false;
    InputBundle.blocks.PrimaryBlock.BlockOffsetStart = 1;
    InputBundle.blocks.PrimaryBlock.BlockOffsetEnd = sizeof(OutputBuffer) + 1;


    ReturnStatus = BPLib_CBOR_EncodeBundle(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_EQ(BPLib_Status_t, ReturnStatus, BPLIB_CBOR_ENC_PRIM_COPY_SIZE_GT_OUTPUT_ERR);
    UtAssert_EQ(size_t, OutputSize, 0);
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
    InputBundle.blob->used_len = sizeof(primary_and_payload_with_aa_x_20);


    InputBundle.blocks.PrimaryBlock.BlockOffsetStart = 1;
    InputBundle.blocks.PrimaryBlock.BlockOffsetEnd = 41;

    InputBundle.blocks.PayloadHeader.BlockOffsetStart = 0;
    InputBundle.blocks.PayloadHeader.BlockOffsetEnd = sizeof(OutputBuffer) + 1;

    ReturnStatus = BPLib_CBOR_EncodeBundle(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_EQ(BPLib_Status_t, ReturnStatus, BPLIB_CBOR_ENC_PAYL_COPY_SIZE_GT_OUTPUT_ERR);
    UtAssert_EQ(size_t, OutputSize, 0);

    /*
    ** BPLib_MEM_CopyOutFromOffset should be called once:
    ** 1. during primary block data copy
    ** 2. (payload copy should be skipped, due to error)
    */
    UtAssert_STUB_COUNT(BPLib_MEM_CopyOutFromOffset, 1);
}


void Test_BPLib_CBOR_EncodeBundle_EncodePrimaryCopyPayload(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[1024];
    size_t OutputSize = 0xdeadbeef;
    BPLib_MEM_Block_t FirstBlock;

    memset(&InputBundle, 0, sizeof(InputBundle));
    memset(&FirstBlock, 0, sizeof(FirstBlock));
    InputBundle.blob = &FirstBlock;

    /* copy valid bundle into our block */
    memcpy(&InputBundle.blob->user_data.raw_bytes,
        primary_and_payload_with_aa_x_20,
        sizeof(primary_and_payload_with_aa_x_20));
    InputBundle.blob->used_len = sizeof(primary_and_payload_with_aa_x_20);

    /* Header Info */
    InputBundle.blocks.PrimaryBlock.BundleProcFlags = 0;
    InputBundle.blocks.PrimaryBlock.CrcType = BPLib_CRC_Type_CRC16;
    /* Dest EID */
    InputBundle.blocks.PrimaryBlock.DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
    InputBundle.blocks.PrimaryBlock.DestEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    InputBundle.blocks.PrimaryBlock.DestEID.Allocator = 0;
    InputBundle.blocks.PrimaryBlock.DestEID.Node = 200;
    InputBundle.blocks.PrimaryBlock.DestEID.Service = 2;
    /* Src EID */
    InputBundle.blocks.PrimaryBlock.SrcEID.Scheme = BPLIB_EID_SCHEME_IPN;
    InputBundle.blocks.PrimaryBlock.SrcEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    InputBundle.blocks.PrimaryBlock.SrcEID.Allocator = 0;
    InputBundle.blocks.PrimaryBlock.SrcEID.Node = 300;
    InputBundle.blocks.PrimaryBlock.SrcEID.Service = 3;
    /* Report-To EID */
    InputBundle.blocks.PrimaryBlock.ReportToEID.Scheme = BPLIB_EID_SCHEME_IPN;
    InputBundle.blocks.PrimaryBlock.ReportToEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    InputBundle.blocks.PrimaryBlock.ReportToEID.Allocator = 0;
    InputBundle.blocks.PrimaryBlock.ReportToEID.Node = 400;
    InputBundle.blocks.PrimaryBlock.ReportToEID.Service = 4;
    /* Other Header Info */
    InputBundle.blocks.PrimaryBlock.Timestamp.CreateTime = 12;
    InputBundle.blocks.PrimaryBlock.Timestamp.SequenceNumber = 34;
    InputBundle.blocks.PrimaryBlock.Lifetime = 0;
    InputBundle.blocks.PrimaryBlock.FragmentOffset = 0;
    InputBundle.blocks.PrimaryBlock.TotalAduLength = 0;
    InputBundle.blocks.PrimaryBlock.CrcVal = 0xdead;
    /* Primary Metadata */
    InputBundle.blocks.PrimaryBlock.RequiresEncode = true;
    InputBundle.blocks.PrimaryBlock.BlockOffsetStart = 1;
    InputBundle.blocks.PrimaryBlock.BlockOffsetEnd = 41;
    /* Payload Metadata */
    InputBundle.blocks.PayloadHeader.RequiresEncode = false;
    InputBundle.blocks.PayloadHeader.BlockOffsetStart = 42;
    InputBundle.blocks.PayloadHeader.BlockOffsetEnd = InputBundle.blocks.PayloadHeader.BlockOffsetStart + 29;


    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_CopyOutFromOffset), BPLIB_SUCCESS);

    ReturnStatus = BPLib_CBOR_EncodeBundle(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_EQ(BPLib_Status_t, ReturnStatus, BPLIB_SUCCESS);

    // UtAssert_EQ(size_t, OutputSize, sizeof(primary_and_payload_with_aa_x_20));

    /*
    ** BPLib_MEM_CopyOutFromOffset should be called once:
    **    (skip primary block data copy, since it was encoded)
    ** 1. during payload block data copy
    */
    UtAssert_STUB_COUNT(BPLib_MEM_CopyOutFromOffset, 1);
}



void Test_BPLib_CBOR_EncodeBundle_EncodePrimaryAndPayload(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[1024];
    size_t OutputSize = 0xdeadbeef;
    BPLib_MEM_Block_t FirstBlock;

    memset(&InputBundle, 0, sizeof(InputBundle));
    memset(&FirstBlock, 0, sizeof(FirstBlock));
    InputBundle.blob = &FirstBlock;

    memcpy(&InputBundle.blob->user_data.raw_bytes,
        primary_and_payload_with_aa_x_20,
        sizeof(primary_and_payload_with_aa_x_20));
    InputBundle.blob->used_len = sizeof(primary_and_payload_with_aa_x_20);


    /* Header Info */
    InputBundle.blocks.PrimaryBlock.BundleProcFlags = 0;
    InputBundle.blocks.PrimaryBlock.CrcType = BPLib_CRC_Type_CRC16;
    /* Dest EID */
    InputBundle.blocks.PrimaryBlock.DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
    InputBundle.blocks.PrimaryBlock.DestEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    InputBundle.blocks.PrimaryBlock.DestEID.Allocator = 0;
    InputBundle.blocks.PrimaryBlock.DestEID.Node = 200;
    InputBundle.blocks.PrimaryBlock.DestEID.Service = 2;
    /* Src EID */
    InputBundle.blocks.PrimaryBlock.SrcEID.Scheme = BPLIB_EID_SCHEME_IPN;
    InputBundle.blocks.PrimaryBlock.SrcEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    InputBundle.blocks.PrimaryBlock.SrcEID.Allocator = 0;
    InputBundle.blocks.PrimaryBlock.SrcEID.Node = 300;
    InputBundle.blocks.PrimaryBlock.SrcEID.Service = 3;
    /* Report-To EID */
    InputBundle.blocks.PrimaryBlock.ReportToEID.Scheme = BPLIB_EID_SCHEME_IPN;
    InputBundle.blocks.PrimaryBlock.ReportToEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    InputBundle.blocks.PrimaryBlock.ReportToEID.Allocator = 0;
    InputBundle.blocks.PrimaryBlock.ReportToEID.Node = 400;
    InputBundle.blocks.PrimaryBlock.ReportToEID.Service = 4;
    /* Other Header Info */
    InputBundle.blocks.PrimaryBlock.Timestamp.CreateTime = 12;
    InputBundle.blocks.PrimaryBlock.Timestamp.SequenceNumber = 34;
    InputBundle.blocks.PrimaryBlock.Lifetime = 0;
    InputBundle.blocks.PrimaryBlock.FragmentOffset = 0;
    InputBundle.blocks.PrimaryBlock.TotalAduLength = 0;
    InputBundle.blocks.PrimaryBlock.CrcVal = 0xdead;
    /* Primary Metadata */
    InputBundle.blocks.PrimaryBlock.RequiresEncode = true;
    InputBundle.blocks.PrimaryBlock.BlockOffsetStart = 1;
    InputBundle.blocks.PrimaryBlock.BlockOffsetEnd = 41;
    /* Payload Metadata */
    InputBundle.blocks.PayloadHeader.RequiresEncode = true;
    InputBundle.blocks.PayloadHeader.DataOffsetStart = 42;
    InputBundle.blocks.PayloadHeader.DataSize = 29;


    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_CopyOutFromOffset), BPLIB_SUCCESS);

    ReturnStatus = BPLib_CBOR_EncodeBundle(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_EQ(BPLib_Status_t, ReturnStatus, BPLIB_SUCCESS);
    // UtAssert_EQ(size_t, OutputSize, sizeof(primary_and_payload_with_aa_x_20));

    /*
    ** BPLib_MEM_CopyOutFromOffset should be called once:
    ** 1. during payload data copy
    */
    UtAssert_STUB_COUNT(BPLib_MEM_CopyOutFromOffset, 1);
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

    memcpy(&InputBundle.blob->user_data.raw_bytes,
        primary_and_payload_with_aa_x_20,
        sizeof(primary_and_payload_with_aa_x_20));
    InputBundle.blob->used_len = sizeof(primary_and_payload_with_aa_x_20);


    /* Header Info */
    InputBundle.blocks.PrimaryBlock.BundleProcFlags = 0;
    InputBundle.blocks.PrimaryBlock.CrcType = BPLib_CRC_Type_CRC16;
    /* Dest EID */
    InputBundle.blocks.PrimaryBlock.DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
    InputBundle.blocks.PrimaryBlock.DestEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    InputBundle.blocks.PrimaryBlock.DestEID.Allocator = 0;
    InputBundle.blocks.PrimaryBlock.DestEID.Node = 200;
    InputBundle.blocks.PrimaryBlock.DestEID.Service = 2;
    /* Src EID */
    InputBundle.blocks.PrimaryBlock.SrcEID.Scheme = BPLIB_EID_SCHEME_IPN;
    InputBundle.blocks.PrimaryBlock.SrcEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    InputBundle.blocks.PrimaryBlock.SrcEID.Allocator = 0;
    InputBundle.blocks.PrimaryBlock.SrcEID.Node = 300;
    InputBundle.blocks.PrimaryBlock.SrcEID.Service = 3;
    /* Report-To EID */
    InputBundle.blocks.PrimaryBlock.ReportToEID.Scheme = BPLIB_EID_SCHEME_IPN;
    InputBundle.blocks.PrimaryBlock.ReportToEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    InputBundle.blocks.PrimaryBlock.ReportToEID.Allocator = 0;
    InputBundle.blocks.PrimaryBlock.ReportToEID.Node = 400;
    InputBundle.blocks.PrimaryBlock.ReportToEID.Service = 4;
    /* Other Header Info */
    InputBundle.blocks.PrimaryBlock.Timestamp.CreateTime = 12;
    InputBundle.blocks.PrimaryBlock.Timestamp.SequenceNumber = 34;
    InputBundle.blocks.PrimaryBlock.Lifetime = 0;
    InputBundle.blocks.PrimaryBlock.FragmentOffset = 0;
    InputBundle.blocks.PrimaryBlock.TotalAduLength = 0;
    InputBundle.blocks.PrimaryBlock.CrcVal = 0xdead;
    /* Primary Metadata */
    InputBundle.blocks.PrimaryBlock.RequiresEncode = false;
    InputBundle.blocks.PrimaryBlock.BlockOffsetStart = 1;
    InputBundle.blocks.PrimaryBlock.BlockOffsetEnd = 41;
    /* Payload Metadata */
    InputBundle.blocks.PayloadHeader.RequiresEncode = false;
    InputBundle.blocks.PayloadHeader.BlockOffsetStart = 42;
    InputBundle.blocks.PayloadHeader.BlockOffsetEnd = InputBundle.blocks.PayloadHeader.BlockOffsetStart + 29;


    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_CopyOutFromOffset), BPLIB_SUCCESS);

    ReturnStatus = BPLib_CBOR_EncodeBundle(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_EQ(BPLib_Status_t, ReturnStatus, BPLIB_SUCCESS);
    // UtAssert_EQ(size_t, OutputSize, sizeof(primary_and_payload_with_aa_x_20));

    /*
    ** BPLib_MEM_CopyOutFromOffset should be called twice:
    ** 1. during primary block data copy
    ** 2. during payload block data copy
    */
    UtAssert_STUB_COUNT(BPLib_MEM_CopyOutFromOffset, 2);
}

void Test_BPLib_CBOR_EncodeBundle_DtnNone(void)
{
    /*
    Primary Block: 
            CRC Type: 1
            Flags: 4
            Dest EID (scheme.node.service): 2.200.1
            Source EID (scheme.node.service): 2.100.1
            Report-To EID (scheme.ssp): 1.0 (dtn:none)
            Timestamp (created, seq): 755533838904, 0
            Lifetime: 3600000
            CRC Value: 0xB19
    Canonical Block [0]: 
            Block Type: 1
            Block Number: 1
            Flags: 0
            CRC Type: 1
            CRC Value: 0xc68f
            Offset Into Encoded Bundle: 42
    */
   unsigned char bundle_primary_and_payload_with_dtn_none[] = {
        0x9f, 0x89, 0x07, 0x04, 0x01, 0x82, 0x02, 0x82, 
        0x18, 0xc8, 0x01, 0x82, 0x02, 0x82, 0x18, 0x64, 
        0x01, 0x82, 0x01, 0x00, 0x82, 
        0x1b, 0x00, 0x00, 0x00, 0xaf, 0xe9, 0x53, 0x7a, 
        0x38, 0x00, 0x1a, 0x00, 0x36, 0xee, 0x80, 0x42, 
        0x0b, 0x19, 0x86, 0x01, 0x01, 0x00, 0x01, 0x54, 
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
        0xaa, 0xaa, 0xaa, 0xaa, 0x42, 0xc6, 0x8f, 0xff,
    };

    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[1024];
    size_t OutputSize = 0xdeadbeef;
    BPLib_MEM_Block_t FirstBlock;
    uint8_t i;

    memset(&InputBundle, 0, sizeof(InputBundle));
    memset(&FirstBlock, 0, sizeof(FirstBlock));
    InputBundle.blob = &FirstBlock;

    memcpy(&InputBundle.blob->user_data.raw_bytes,
        bundle_primary_and_payload_with_dtn_none,
        sizeof(bundle_primary_and_payload_with_dtn_none));
    InputBundle.blob->used_len = sizeof(bundle_primary_and_payload_with_dtn_none);

    UT_SetDeferredRetcode(UT_KEY(BPLib_CRC_Calculate), 1, 0xb19);

    /* Header Info */
    InputBundle.blocks.PrimaryBlock.BundleProcFlags = 4;
    InputBundle.blocks.PrimaryBlock.CrcType = BPLib_CRC_Type_CRC16;
    /* Dest EID */
    InputBundle.blocks.PrimaryBlock.DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
    InputBundle.blocks.PrimaryBlock.DestEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    InputBundle.blocks.PrimaryBlock.DestEID.Node = 200;
    InputBundle.blocks.PrimaryBlock.DestEID.Service = 1;
    /* Src EID */
    InputBundle.blocks.PrimaryBlock.SrcEID.Scheme = BPLIB_EID_SCHEME_IPN;
    InputBundle.blocks.PrimaryBlock.SrcEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    InputBundle.blocks.PrimaryBlock.SrcEID.Node = 100;
    InputBundle.blocks.PrimaryBlock.SrcEID.Service = 1;
    /* Report-To EID */
    InputBundle.blocks.PrimaryBlock.ReportToEID.Scheme = BPLIB_EID_SCHEME_DTN;
    InputBundle.blocks.PrimaryBlock.ReportToEID.Node = 0;
    InputBundle.blocks.PrimaryBlock.ReportToEID.Service = 0;
    /* Other Header Info */
    InputBundle.blocks.PrimaryBlock.Timestamp.CreateTime = 755533838904;
    InputBundle.blocks.PrimaryBlock.Timestamp.SequenceNumber = 0;
    InputBundle.blocks.PrimaryBlock.Lifetime = 3600000;
    InputBundle.blocks.PrimaryBlock.FragmentOffset = 0;
    InputBundle.blocks.PrimaryBlock.TotalAduLength = 0;
    InputBundle.blocks.PrimaryBlock.CrcVal = 0xb19;
    /* Primary Metadata */
    InputBundle.blocks.PrimaryBlock.RequiresEncode = true;
    InputBundle.blocks.PrimaryBlock.BlockOffsetStart = 1;
    InputBundle.blocks.PrimaryBlock.BlockOffsetEnd = 38;
    /* Payload Metadata */
    InputBundle.blocks.PayloadHeader.RequiresEncode = true;
    InputBundle.blocks.PayloadHeader.BlockOffsetStart = 39;
    InputBundle.blocks.PayloadHeader.BlockOffsetEnd = InputBundle.blocks.PayloadHeader.BlockOffsetStart + 29;


    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_CopyOutFromOffset), BPLIB_SUCCESS);

    ReturnStatus = BPLib_CBOR_EncodeBundle(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_EQ(BPLib_Status_t, ReturnStatus, BPLIB_SUCCESS);

    // Just validate the primary block for this test
    for (i = 0; i <= InputBundle.blocks.PrimaryBlock.BlockOffsetEnd; i++)
    {
        UtAssert_EQ(uint8_t, OutputBuffer[i], bundle_primary_and_payload_with_dtn_none[i]);
    }

    /*
    ** BPLib_MEM_CopyOutFromOffset should be called once:
    ** 1. during payload data copy
    */
   UtAssert_STUB_COUNT(BPLib_MEM_CopyOutFromOffset, 1);

}

void TestBplibCborEncode_Register(void)
{
    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_InputBundleNullError, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_InputBundleNullError");
    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_InputBundleBlobNullError, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_InputBundleBlobNullError");
    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_OutputBundleBufNullError, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_OutputBundleBufNullError");
    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_OutputSizeBufNullError, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_OutputSizeBufNullError");

    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_OutputBufLen1, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_OutputBufLen1");

    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_PrimaryCopySizeGtOutputError, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_PrimaryCopySizeGtOutputError");

    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_PayloadCopyLenError, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_PayloadCopyLenError");

    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_EncodePrimaryCopyPayload, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_EncodePrimaryCopyPayload");
    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_EncodePrimaryAndPayload, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_EncodePrimaryAndPayload");
    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_Nominal, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_Nominal");
    UtTest_Add(Test_BPLib_CBOR_EncodeBundle_DtnNone, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeBundle_DtnNone");
}
