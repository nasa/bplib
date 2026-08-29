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

void Test_BPLib_CBOR_EncodeExtensionBlock_CustodyBlock(void)
{
    BPLib_Status_t    Status;
    BPLib_Bundle_t    Bundle;
    char              OutputBuffer[512];
    size_t            NumBytesCopied;
    BPLib_MEM_Block_t Blob;

    memset(&Bundle, 0, sizeof(BPLib_Bundle_t));
    Bundle.blob = &Blob;

    Bundle.blocks.ExtBlocks[0].Header.BlockType       = BPLib_BlockType_CTEB;
    Bundle.blocks.ExtBlocks[0].Header.RequiresDiscard = false;

    Status = BPLib_CBOR_EncodeExtensionBlock(&Bundle, 0, OutputBuffer, sizeof(OutputBuffer), &NumBytesCopied);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
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
    uint8_t OutputBuffer[512];
    size_t OutputBufferSize = sizeof(OutputBuffer);
    size_t NumBytesCopied = 0;
    size_t i;
    const uint8_t ExpectedBlock[] = {
        0x85, /* five-item canonical block array (CRC type none) */
        0x00, /* block type */
        0x00, /* block number */
        0x00, /* block processing flags */
        0x00, /* CRC type none */
        0x40  /* empty payload byte string */
    };

    /* Setup nominal inputs */
    memset(&StoredBundleIn, 0, sizeof(StoredBundleIn));

    /* Call UUT and check status */
    ReturnStatus = BPLib_CBOR_EncodePayload(&StoredBundleIn, OutputBuffer, OutputBufferSize, &NumBytesCopied);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
    UtAssert_EQ(size_t, NumBytesCopied, sizeof(ExpectedBlock));

    for (i = 0; i < NumBytesCopied; i++)
    {
        UtAssert_EQ(uint8_t, OutputBuffer[i], ExpectedBlock[i]);
    }
}

void Test_BPLib_CBOR_EncodePayload_Ccs(void)
{
    BPLib_Status_t          Status;
    BPLib_ARP_AdminRecord_t AdminRecord;
    BPLib_Bundle_t          Bundle;
    uint8_t                 OutputBuffer[BPLIB_MAX_PAYLOAD_SIZE];
    size_t                  NumBytesCopied;
    BPLib_MEM_Block_t       BundleBlob;
    size_t                  SeqRangeSize;
    uint8_t                 Collection;
    size_t                  i;
    uint8_t                 ExpectedBlock[] = {
        0x86, 0x01, 0x01, 0x00, 0x02, 0x51, 0x82, 0x0d, 
        0xa1, 0x01, 0x83, 0x0a, 0x19, 0x01, 0x90, 0x83, 
        0x19, 0x03, 0xf2, 0x02, 0x19, 0x0b, 0xd6, 0x44, 
        0xde, 0xad, 0xbe, 0xef
    };

    NumBytesCopied   = 0;
    SeqRangeSize     = 0;
    
    AdminRecord.AdminRecordType                                             = BPLib_CT_CcsRecordTypeCode;
    AdminRecord.AdminRecordBody.CCS.NumBundleSeqCollections                 = 1;
    AdminRecord.AdminRecordBody.CCS.BundleSeqCollections[0].DispositionCode = BPLib_CT_CustodyAccepted;
    AdminRecord.AdminRecordBody.CCS.BundleSeqCollections[0].SeqRangeLen     = 3;
    AdminRecord.AdminRecordBody.CCS.BundleSeqCollections[0].SeqId           = 10;
    AdminRecord.AdminRecordBody.CCS.BundleSeqCollections[0].FirstSeqNum     = 400;
    AdminRecord.AdminRecordBody.CCS.BundleSeqCollections[0].SeqRange[0]     = 1010;
    AdminRecord.AdminRecordBody.CCS.BundleSeqCollections[0].SeqRange[1]     = 2;
    AdminRecord.AdminRecordBody.CCS.BundleSeqCollections[0].SeqRange[2]     = 3030;

    memset(&Bundle, 0, sizeof(BPLib_Bundle_t));
    Bundle.blocks.PayloadHeader.BlockType      = BPLib_BlockType_Payload;
    Bundle.blocks.PayloadHeader.BlockNum       = 1;
    Bundle.blocks.PayloadHeader.CrcType        = BPLib_CRC_Type_CRC32C;

    Bundle.blocks.PrimaryBlock.BundleProcFlags = BPLIB_BUNDLE_PROC_ADMIN_RECORD_FLAG;

    Bundle.blob = &BundleBlob;

    memcpy(Bundle.blob->user_data.BigData, &AdminRecord, sizeof(BPLib_ARP_AdminRecord_t));

    for (Collection = 0; Collection < AdminRecord.AdminRecordBody.CCS.NumBundleSeqCollections; Collection++)
    {
        /* ATTN: Assumes all sequence range values are <= 0x17. See CBOR RFC for details */
        SeqRangeSize += AdminRecord.AdminRecordBody.CCS.BundleSeqCollections[Collection].SeqRangeLen;
    }

    UT_SetDefaultReturnValue(UT_KEY(BPLib_CRC_Calculate), (BPLib_CRC_Val_t) 0xdeadbeef);
    
    Status = BPLib_CBOR_EncodePayload(&Bundle, OutputBuffer, sizeof(OutputBuffer), &NumBytesCopied);

    UtAssert_INT32_EQ(Status, BPLIB_SUCCESS);
    UtAssert_EQ(size_t, NumBytesCopied, sizeof(ExpectedBlock));

    for (i = 0; i < NumBytesCopied; i++)
    {
        UtAssert_EQ(uint8_t, OutputBuffer[i], ExpectedBlock[i]);
    }
}

void TestBplibCborEncodeInternal_Register(void)
{
    ADD_TEST(Test_BPLib_CBOR_EncodePrimary_NullInputErrors);
    ADD_TEST(Test_BPLib_CBOR_EncodePrimary_Crc16);
    ADD_TEST(Test_BPLib_CBOR_EncodePrimary_Crc32);
    ADD_TEST(Test_BPLib_CBOR_EncodePrimary_CrcNone);

    ADD_TEST(Test_BPLib_CBOR_EncodeExtensionBlock_NullInputErrors);
    ADD_TEST(Test_BPLib_CBOR_EncodeExtensionBlock_Nominal);
    ADD_TEST(Test_BPLib_CBOR_EncodeExtensionBlock_Skip);
    ADD_TEST(Test_BPLib_CBOR_EncodeExtensionBlock_CustodyBlock);
    ADD_TEST(Test_BPLib_CBOR_EncodeExtensionBlock_UnknownBlk);

    ADD_TEST(Test_BPLib_CBOR_EncodePayload_NullInputErrors);
    ADD_TEST(Test_BPLib_CBOR_EncodePayload_Nominal);
    ADD_TEST(Test_BPLib_CBOR_EncodePayload_Ccs);
}
