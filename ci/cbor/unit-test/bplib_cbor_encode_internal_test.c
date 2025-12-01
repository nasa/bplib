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
** BPLib_CBOR_EncodePrimary Tests
*/

void Test_BPLib_CBOR_EncodePrimary_NullInputErrors(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t StoredBundleIn;
    char OutputBuffer[512];
    size_t OutputBufferSize = sizeof(OutputBuffer);
    size_t NumBytesCopied = 0;

    /* all null */
    ReturnStatus = BPLib_CBOR_EncodePrimary(NULL, NULL, 0, NULL);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);

    /* StoredBundleIn NULL */
    ReturnStatus = BPLib_CBOR_EncodePrimary(NULL, OutputBuffer, OutputBufferSize, &NumBytesCopied);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);

    /* OutputBuffer NULL */
    ReturnStatus = BPLib_CBOR_EncodePrimary(&StoredBundleIn, NULL, 0, &NumBytesCopied);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);

    /* NumBytesCopied NULL */
    ReturnStatus = BPLib_CBOR_EncodePrimary(&StoredBundleIn, OutputBuffer, OutputBufferSize, NULL);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
}


void Test_BPLib_CBOR_EncodePrimary_Crc16(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t StoredBundleIn;
    char OutputBuffer[512];
    size_t OutputBufferSize = sizeof(OutputBuffer);
    size_t NumBytesCopied = 0;

    /* Setup nominal inputs */
    memset(&StoredBundleIn, 0, sizeof(StoredBundleIn));

    StoredBundleIn.blocks.PrimaryBlock.BundleProcFlags = 0;
    StoredBundleIn.blocks.PrimaryBlock.CrcType = BPLib_CRC_Type_CRC16;

    StoredBundleIn.blocks.PrimaryBlock.DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
    StoredBundleIn.blocks.PrimaryBlock.DestEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    StoredBundleIn.blocks.PrimaryBlock.DestEID.Allocator = 0;
    StoredBundleIn.blocks.PrimaryBlock.DestEID.Node = 200;
    StoredBundleIn.blocks.PrimaryBlock.DestEID.Service = 2;

    StoredBundleIn.blocks.PrimaryBlock.SrcEID.Scheme = BPLIB_EID_SCHEME_IPN;
    StoredBundleIn.blocks.PrimaryBlock.SrcEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    StoredBundleIn.blocks.PrimaryBlock.SrcEID.Allocator = 0;
    StoredBundleIn.blocks.PrimaryBlock.SrcEID.Node = 300;
    StoredBundleIn.blocks.PrimaryBlock.SrcEID.Service = 3;

    StoredBundleIn.blocks.PrimaryBlock.ReportToEID.Scheme = BPLIB_EID_SCHEME_IPN;
    StoredBundleIn.blocks.PrimaryBlock.ReportToEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    StoredBundleIn.blocks.PrimaryBlock.ReportToEID.Allocator = 0;
    StoredBundleIn.blocks.PrimaryBlock.ReportToEID.Node = 400;
    StoredBundleIn.blocks.PrimaryBlock.ReportToEID.Service = 4;

    StoredBundleIn.blocks.PrimaryBlock.Timestamp.CreateTime = 12;
    StoredBundleIn.blocks.PrimaryBlock.Timestamp.SequenceNumber = 34;

    StoredBundleIn.blocks.PrimaryBlock.Lifetime = 0;
    StoredBundleIn.blocks.PrimaryBlock.FragmentOffset = 0;
    StoredBundleIn.blocks.PrimaryBlock.TotalAduLength = 0;

    StoredBundleIn.blocks.PrimaryBlock.CrcVal = 0xdead;

    /* Call UUT and check status */
    ReturnStatus = BPLib_CBOR_EncodePrimary(&StoredBundleIn, OutputBuffer, OutputBufferSize, &NumBytesCopied);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
    UtAssert_EQ(size_t, NumBytesCopied, 32);
}



void Test_BPLib_CBOR_EncodePrimary_Crc32(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t StoredBundleIn;
    char OutputBuffer[512];
    size_t OutputBufferSize = sizeof(OutputBuffer);
    size_t NumBytesCopied = 0;

    /* Setup nominal inputs */
    memset(&StoredBundleIn, 0, sizeof(StoredBundleIn));

    StoredBundleIn.blocks.PrimaryBlock.BundleProcFlags = 0;
    StoredBundleIn.blocks.PrimaryBlock.CrcType = BPLib_CRC_Type_CRC32C;

    StoredBundleIn.blocks.PrimaryBlock.DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
    StoredBundleIn.blocks.PrimaryBlock.DestEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    StoredBundleIn.blocks.PrimaryBlock.DestEID.Allocator = 0;
    StoredBundleIn.blocks.PrimaryBlock.DestEID.Node = 200;
    StoredBundleIn.blocks.PrimaryBlock.DestEID.Service = 2;

    StoredBundleIn.blocks.PrimaryBlock.SrcEID.Scheme = BPLIB_EID_SCHEME_IPN;
    StoredBundleIn.blocks.PrimaryBlock.SrcEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    StoredBundleIn.blocks.PrimaryBlock.SrcEID.Allocator = 0;
    StoredBundleIn.blocks.PrimaryBlock.SrcEID.Node = 300;
    StoredBundleIn.blocks.PrimaryBlock.SrcEID.Service = 3;

    StoredBundleIn.blocks.PrimaryBlock.ReportToEID.Scheme = BPLIB_EID_SCHEME_IPN;
    StoredBundleIn.blocks.PrimaryBlock.ReportToEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    StoredBundleIn.blocks.PrimaryBlock.ReportToEID.Allocator = 0;
    StoredBundleIn.blocks.PrimaryBlock.ReportToEID.Node = 400;
    StoredBundleIn.blocks.PrimaryBlock.ReportToEID.Service = 4;

    StoredBundleIn.blocks.PrimaryBlock.Timestamp.CreateTime = 12;
    StoredBundleIn.blocks.PrimaryBlock.Timestamp.SequenceNumber = 34;

    StoredBundleIn.blocks.PrimaryBlock.Lifetime = 0;
    StoredBundleIn.blocks.PrimaryBlock.FragmentOffset = 0;
    StoredBundleIn.blocks.PrimaryBlock.TotalAduLength = 0;

    StoredBundleIn.blocks.PrimaryBlock.CrcVal = 0xdeadbeef;

    /* Call UUT and check status */
    ReturnStatus = BPLib_CBOR_EncodePrimary(&StoredBundleIn, OutputBuffer, OutputBufferSize, &NumBytesCopied);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
    UtAssert_EQ(size_t, NumBytesCopied, 34);
}


void Test_BPLib_CBOR_EncodePrimary_CrcNone(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t StoredBundleIn;
    char OutputBuffer[512];
    size_t OutputBufferSize = sizeof(OutputBuffer);
    size_t NumBytesCopied = 0;

    /* Setup nominal inputs */
    memset(&StoredBundleIn, 0, sizeof(StoredBundleIn));

    StoredBundleIn.blocks.PrimaryBlock.BundleProcFlags = 0;
    StoredBundleIn.blocks.PrimaryBlock.CrcType = BPLib_CRC_Type_None;

    StoredBundleIn.blocks.PrimaryBlock.DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
    StoredBundleIn.blocks.PrimaryBlock.DestEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    StoredBundleIn.blocks.PrimaryBlock.DestEID.Allocator = 0;
    StoredBundleIn.blocks.PrimaryBlock.DestEID.Node = 200;
    StoredBundleIn.blocks.PrimaryBlock.DestEID.Service = 2;

    StoredBundleIn.blocks.PrimaryBlock.SrcEID.Scheme = BPLIB_EID_SCHEME_IPN;
    StoredBundleIn.blocks.PrimaryBlock.SrcEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    StoredBundleIn.blocks.PrimaryBlock.SrcEID.Allocator = 0;
    StoredBundleIn.blocks.PrimaryBlock.SrcEID.Node = 300;
    StoredBundleIn.blocks.PrimaryBlock.SrcEID.Service = 3;

    StoredBundleIn.blocks.PrimaryBlock.ReportToEID.Scheme = BPLIB_EID_SCHEME_IPN;
    StoredBundleIn.blocks.PrimaryBlock.ReportToEID.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    StoredBundleIn.blocks.PrimaryBlock.ReportToEID.Allocator = 0;
    StoredBundleIn.blocks.PrimaryBlock.ReportToEID.Node = 400;
    StoredBundleIn.blocks.PrimaryBlock.ReportToEID.Service = 4;

    StoredBundleIn.blocks.PrimaryBlock.Timestamp.CreateTime = 12;
    StoredBundleIn.blocks.PrimaryBlock.Timestamp.SequenceNumber = 34;

    StoredBundleIn.blocks.PrimaryBlock.Lifetime = 0;
    StoredBundleIn.blocks.PrimaryBlock.FragmentOffset = 0;
    StoredBundleIn.blocks.PrimaryBlock.TotalAduLength = 0;

    StoredBundleIn.blocks.PrimaryBlock.CrcVal = 0;

    /* Call UUT and check status */
    ReturnStatus = BPLib_CBOR_EncodePrimary(&StoredBundleIn, OutputBuffer, OutputBufferSize, &NumBytesCopied);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
    UtAssert_EQ(size_t, NumBytesCopied, 29);
}



/*
** BPLib_CBOR_EncodeExtensionBlock Tests
*/


void Test_BPLib_CBOR_EncodeExtensionBlock_NullInputErrors(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t StoredBundleIn;
    char OutputBuffer[512];
    size_t OutputBufferSize = sizeof(OutputBuffer);
    size_t NumBytesCopied = 0;

    /* all null */
    ReturnStatus = BPLib_CBOR_EncodeExtensionBlock(NULL, 0, NULL, 0, NULL);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);

    /* StoredBundleIn NULL */
    ReturnStatus = BPLib_CBOR_EncodeExtensionBlock(NULL, 0, OutputBuffer, OutputBufferSize, &NumBytesCopied);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);

    /* OutputBuffer NULL */
    ReturnStatus = BPLib_CBOR_EncodeExtensionBlock(&StoredBundleIn, 0, NULL, 0, &NumBytesCopied);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);

    /* NumBytesCopied NULL */
    ReturnStatus = BPLib_CBOR_EncodeExtensionBlock(&StoredBundleIn, 0, OutputBuffer, OutputBufferSize, NULL);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
}

void Test_BPLib_CBOR_EncodeExtensionBlock_Nominal(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t StoredBundleIn;
    char OutputBuffer[512];
    size_t OutputBufferSize = sizeof(OutputBuffer);
    size_t NumBytesCopied = 0;
    BPLib_MEM_Block_t Blob;

    /* Setup nominal inputs */
    memset(&StoredBundleIn, 0, sizeof(StoredBundleIn));
    StoredBundleIn.blob = &Blob;

    /* Call UUT and check status */
    ReturnStatus = BPLib_CBOR_EncodeExtensionBlock(&StoredBundleIn, 0, OutputBuffer, OutputBufferSize, &NumBytesCopied);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
}

/* Test extension block encode when the block is to be skipped */
void Test_BPLib_CBOR_EncodeExtensionBlock_Skip(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t StoredBundleIn;
    char OutputBuffer[512];
    size_t OutputBufferSize = sizeof(OutputBuffer);
    size_t NumBytesCopied = 0;
    BPLib_MEM_Block_t Blob;

    /* Setup nominal inputs */
    memset(&StoredBundleIn, 0, sizeof(StoredBundleIn));
    StoredBundleIn.blob = &Blob;

    StoredBundleIn.blocks.ExtBlocks[0].Header.RequiresDiscard = true;

    /* Call UUT and check status */
    ReturnStatus = BPLib_CBOR_EncodeExtensionBlock(&StoredBundleIn, 0, OutputBuffer, OutputBufferSize, &NumBytesCopied);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
    UtAssert_INT32_EQ(NumBytesCopied, 0);
}

/* Test extension block encode when the block is unknown and needs to be copied out */
void Test_BPLib_CBOR_EncodeExtensionBlock_UnknownBlk(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t StoredBundleIn;
    char OutputBuffer[512];
    size_t OutputBufferSize = sizeof(OutputBuffer);
    size_t NumBytesCopied = 0;
    BPLib_MEM_Block_t Blob;

    /* Setup nominal inputs */
    memset(&StoredBundleIn, 0, sizeof(StoredBundleIn));
    StoredBundleIn.blob = &Blob;

    StoredBundleIn.blocks.ExtBlocks[0].Header.RequiresDiscard = false;
    StoredBundleIn.blocks.ExtBlocks[0].Header.BlockType = BPLib_BlockType_UNKNOWN;
    StoredBundleIn.blocks.ExtBlocks[0].Header.BlockOffsetEnd = 10;
    StoredBundleIn.blocks.ExtBlocks[0].Header.BlockOffsetStart = 1;

    /* Call UUT and check status */
    ReturnStatus = BPLib_CBOR_EncodeExtensionBlock(&StoredBundleIn, 0, OutputBuffer, OutputBufferSize, &NumBytesCopied);
    UtAssert_STUB_COUNT(BPLib_MEM_CopyOutFromOffset, 1);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
    UtAssert_INT32_EQ(NumBytesCopied, 10);
}


/*
** BPLib_CBOR_EncodePayload Tests
*/

void Test_BPLib_CBOR_EncodePayload_NullInputErrors(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t StoredBundleIn;
    char OutputBuffer[512];
    size_t OutputBufferSize = sizeof(OutputBuffer);
    size_t NumBytesCopied = 0;

    /* all null */
    ReturnStatus = BPLib_CBOR_EncodePayload(NULL, NULL, 0, NULL);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);

    /* StoredBundleIn NULL */
    ReturnStatus = BPLib_CBOR_EncodePayload(NULL, OutputBuffer, OutputBufferSize, &NumBytesCopied);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);

    /* OutputBuffer NULL */
    ReturnStatus = BPLib_CBOR_EncodePayload(&StoredBundleIn, NULL, 0, &NumBytesCopied);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);

    /* NumBytesCopied NULL */
    ReturnStatus = BPLib_CBOR_EncodePayload(&StoredBundleIn, OutputBuffer, OutputBufferSize, NULL);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
}


void Test_BPLib_CBOR_EncodePayload_Nominal(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t StoredBundleIn;
    char OutputBuffer[512];
    size_t OutputBufferSize = sizeof(OutputBuffer);
    size_t NumBytesCopied = 0;

    /* Setup nominal inputs */
    memset(&StoredBundleIn, 0, sizeof(StoredBundleIn));

    /* Call UUT and check status */
    ReturnStatus = BPLib_CBOR_EncodePayload(&StoredBundleIn, OutputBuffer, OutputBufferSize, &NumBytesCopied);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
}


void TestBplibCborEncodeInternal_Register(void)
{
    UtTest_Add(Test_BPLib_CBOR_EncodePrimary_NullInputErrors, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodePrimary_NullInputErrors");
    UtTest_Add(Test_BPLib_CBOR_EncodePrimary_Crc16, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodePrimary_Crc16");
    UtTest_Add(Test_BPLib_CBOR_EncodePrimary_Crc32, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodePrimary_Crc32");
    UtTest_Add(Test_BPLib_CBOR_EncodePrimary_CrcNone, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodePrimary_CrcNone");

    UtTest_Add(Test_BPLib_CBOR_EncodeExtensionBlock_NullInputErrors, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeExtensionBlock_NullInputErrors");
    UtTest_Add(Test_BPLib_CBOR_EncodeExtensionBlock_Nominal, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeExtensionBlock_Nominal");
    UtTest_Add(Test_BPLib_CBOR_EncodeExtensionBlock_Skip, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeExtensionBlock_Skip");
    UtTest_Add(Test_BPLib_CBOR_EncodeExtensionBlock_UnknownBlk, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodeExtensionBlock_UnknownBlk");

    UtTest_Add(Test_BPLib_CBOR_EncodePayload_NullInputErrors, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodePayload_NullInputErrors");
    UtTest_Add(Test_BPLib_CBOR_EncodePayload_Nominal, BPLib_CBOR_Test_Setup, BPLib_CBOR_Test_Teardown, "Test_BPLib_CBOR_EncodePayload_Nominal");
}
