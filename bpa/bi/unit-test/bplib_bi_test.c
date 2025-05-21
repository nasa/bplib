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

#include "bplib_bi_test_utils.h"

#include "bplib_cbor.h"

void Test_BPLib_BI_RecvFullBundleIn_NullInputErrors(void)
{
    BPLib_Instance_t instance;
    char BundleIn[32];
    size_t Size = 0;

    // Test the NULL pointer checks.
    UtAssert_INT32_EQ(BPLib_BI_RecvFullBundleIn(NULL, NULL, Size, 0), BPLIB_NULL_PTR_ERROR);
    UtAssert_INT32_EQ(BPLib_BI_RecvFullBundleIn(NULL, BundleIn, Size, 0), BPLIB_NULL_PTR_ERROR);
    UtAssert_INT32_EQ(BPLib_BI_RecvFullBundleIn(&instance, NULL, Size, 0), BPLIB_NULL_PTR_ERROR);

    // Make sure we didn't get past the null pointer checks
    UtAssert_STUB_COUNT(BPLib_MEM_BundleAlloc, 0);
    UtAssert_STUB_COUNT(BPLib_CBOR_DecodeBundle, 0);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 0);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 0);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 0);
}

void Test_BPLib_BI_RecvFullBundleIn_MemAllocError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    char BundleIn[32];
    size_t Size = 0;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_BundleAlloc), (UT_IntReturn_t) NULL);

    ReturnStatus = BPLib_BI_RecvFullBundleIn(&Instance, BundleIn, Size, 0);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleAlloc, 1);
    UtAssert_STUB_COUNT(BPLib_CBOR_DecodeBundle, 0);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 0);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 0);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 0);
}

void Test_BPLib_BI_RecvFullBundleIn_LenErr(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    BPLib_Bundle_t AllocatedBundleMem;
    char BundleIn[32];
    size_t Size = 0;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_BundleAlloc), (UT_IntReturn_t) &AllocatedBundleMem);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_CBOR_DecodeBundle), BPLIB_CBOR_DEC_BUNDLE_TOO_LONG_DEC_ERR);

    ReturnStatus = BPLib_BI_RecvFullBundleIn(&Instance, BundleIn, Size, 0);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_CBOR_DEC_BUNDLE_TOO_LONG_DEC_ERR);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleAlloc, 1);
    UtAssert_STUB_COUNT(BPLib_CBOR_DecodeBundle, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DELETED_TOO_LONG, 
                                                Context_BPLib_AS_Increment[0].Counter);
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 3);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 0);
}

void Test_BPLib_BI_RecvFullBundleIn_HopErr(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    BPLib_Bundle_t AllocatedBundleMem;
    char BundleIn[32];
    size_t Size = 0;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_BundleAlloc), (UT_IntReturn_t) &AllocatedBundleMem);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_CBOR_DecodeBundle), BPLIB_CBOR_DEC_HOP_BLOCK_EXCEEDED_ERR);

    ReturnStatus = BPLib_BI_RecvFullBundleIn(&Instance, BundleIn, Size, 0);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_CBOR_DEC_HOP_BLOCK_EXCEEDED_ERR);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleAlloc, 1);
    UtAssert_STUB_COUNT(BPLib_CBOR_DecodeBundle, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DELETED_HOP_EXCEEDED, 
                                                Context_BPLib_AS_Increment[0].Counter);
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 3);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 0);
}


void Test_BPLib_BI_RecvFullBundleIn_BlkErr(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    BPLib_Bundle_t AllocatedBundleMem;
    char BundleIn[32];
    size_t Size = 0;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_BundleAlloc), (UT_IntReturn_t) &AllocatedBundleMem);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_CBOR_DecodeBundle), BPLIB_CBOR_DEC_UNKNOWN_BLOCK_DEC_ERR);

    ReturnStatus = BPLib_BI_RecvFullBundleIn(&Instance, BundleIn, Size, 0);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_CBOR_DEC_UNKNOWN_BLOCK_DEC_ERR);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleAlloc, 1);
    UtAssert_STUB_COUNT(BPLib_CBOR_DecodeBundle, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK, 
                                                Context_BPLib_AS_Increment[0].Counter);
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 3);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 0);
}


void Test_BPLib_BI_RecvFullBundleIn_UnintellErr(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    BPLib_Bundle_t AllocatedBundleMem;
    char BundleIn[32];
    size_t Size = 0;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_BundleAlloc), (UT_IntReturn_t) &AllocatedBundleMem);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_CBOR_DecodeBundle), BPLIB_CBOR_DEC_BUNDLE_TOO_SHORT_ERR);

    ReturnStatus = BPLib_BI_RecvFullBundleIn(&Instance, BundleIn, Size, 0);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_CBOR_DEC_BUNDLE_TOO_SHORT_ERR);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleAlloc, 1);
    UtAssert_STUB_COUNT(BPLib_CBOR_DecodeBundle, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DELETED_UNINTELLIGIBLE, 
                                                Context_BPLib_AS_Increment[0].Counter);
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 3);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 0);
}

void Test_BPLib_BI_RecvFullBundleIn_ExpireErr(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    char BundleIn[32];
    size_t Size = 0;
    BPLib_TIME_MonotonicTime_t MonoTime;

    MonoTime.Time = 0;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_BundleAlloc), (UT_IntReturn_t) &DeserializedBundle);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_CBOR_DecodeBundle), BPLIB_SUCCESS);
    UT_SetDataBuffer(UT_KEY(BPLib_TIME_GetMonotonicTime), &MonoTime, sizeof(MonoTime), false);
    UT_SetDataBuffer(UT_KEY(BPLib_TIME_GetMonotonicTime), &MonoTime, sizeof(MonoTime), false);

    /* Age block should exceed lifetime */
    DeserializedBundle.blocks.ExtBlocks[1].BlockData.AgeBlockData.Age = 123456;

    ReturnStatus = BPLib_BI_RecvFullBundleIn(&Instance, BundleIn, Size, 0);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_BI_EXPIRED_BUNDLE_ERR);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleAlloc, 1);
    UtAssert_STUB_COUNT(BPLib_CBOR_DecodeBundle, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DELETED_EXPIRED, 
                                                Context_BPLib_AS_Increment[0].Counter);
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 3);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 0);
}

/* Test that bundle ingress with a well-formed bundle goes well */
void Test_BPLib_BI_RecvFullBundleIn_Nominal(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    char BundleIn[32];
    size_t Size = 0;
    BPLib_TIME_MonotonicTime_t MonoTime;

    MonoTime.Time = 0;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_BundleAlloc), (UT_IntReturn_t) &DeserializedBundle);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_CBOR_DecodeBundle), BPLIB_SUCCESS);
    UT_SetDataBuffer(UT_KEY(BPLib_TIME_GetMonotonicTime), &MonoTime, sizeof(MonoTime), false);
    UT_SetDataBuffer(UT_KEY(BPLib_TIME_GetMonotonicTime), &MonoTime, sizeof(MonoTime), false);

    ReturnStatus = BPLib_BI_RecvFullBundleIn(&Instance, BundleIn, Size, 0);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleAlloc, 1);
    UtAssert_STUB_COUNT(BPLib_CBOR_DecodeBundle, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 0);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 1);
}

/* Test that bundle ingress when the contact ID is invalid */
void Test_BPLib_BI_RecvFullBundleIn_IdErr(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    char BundleIn[32];
    size_t Size = 0;


    ReturnStatus = BPLib_BI_RecvFullBundleIn(&Instance, BundleIn, Size, BPLIB_MAX_NUM_CONTACTS);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_INVALID_CONT_ID_ERR);
}

/* Test that bundle ingress fails with an invalidly formatted bundle */
void Test_BPLib_BI_RecvFullBundleIn_Invalid(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    char BundleIn[32];
    size_t Size = 0;

    DeserializedBundle.blocks.PayloadHeader.BlockType = BPLib_BlockType_Reserved;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_BundleAlloc), (UT_IntReturn_t) &DeserializedBundle);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_CBOR_DecodeBundle), BPLIB_SUCCESS);

    ReturnStatus = BPLib_BI_RecvFullBundleIn(&Instance, BundleIn, Size, 0);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_BI_INVALID_BUNDLE_ERR);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleAlloc, 1);
    UtAssert_STUB_COUNT(BPLib_CBOR_DecodeBundle, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 3);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 0);
}

/* Test that bundle ingress fails when QM can't ingress a bundle */
void Test_BPLib_BI_RecvFullBundleIn_JobFail(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    char BundleIn[32];
    size_t Size = 0;
    BPLib_TIME_MonotonicTime_t MonoTime;

    MonoTime.Time = 0;

    UT_SetDataBuffer(UT_KEY(BPLib_TIME_GetMonotonicTime), &MonoTime, sizeof(MonoTime), false);
    UT_SetDataBuffer(UT_KEY(BPLib_TIME_GetMonotonicTime), &MonoTime, sizeof(MonoTime), false);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_BundleAlloc), (UT_IntReturn_t) &DeserializedBundle);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_CBOR_DecodeBundle), BPLIB_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_QM_CreateJob), BPLIB_ERROR);

    ReturnStatus = BPLib_BI_RecvFullBundleIn(&Instance, BundleIn, Size, 0);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_ERROR);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleAlloc, 1);
    UtAssert_STUB_COUNT(BPLib_CBOR_DecodeBundle, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 2);
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 2);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 1);
}

void Test_BPLib_BI_RecvCtrlMsg_Nominal(void)
{
    BPLib_CLA_CtrlMsg_t* MsgPtr = NULL;
    UtAssert_INT32_EQ(BPLib_BI_RecvCtrlMsg(MsgPtr), BPLIB_SUCCESS);    
}

/* Test that bundle validation on a null bundle fails */
void Test_BPLib_BI_ValidateBundle_Null(void)
{
    UtAssert_INT32_EQ(BPLib_BI_ValidateBundle(NULL), BPLIB_NULL_PTR_ERROR);        
}

/* Test that bundle validation fails when there's no payload */
void Test_BPLib_BI_ValidateBundle_NoPayload(void)
{
    DeserializedBundle.blocks.PayloadHeader.BlockType = BPLib_BlockType_Reserved;

    UtAssert_INT32_EQ(BPLib_BI_ValidateBundle(&DeserializedBundle), BPLIB_BI_INVALID_BUNDLE_ERR);        
}

/* Test that bundle validation fails when the hop count block is duplicated */
void Test_BPLib_BI_ValidateBundle_DuplHopCount(void)
{
    DeserializedBundle.blocks.ExtBlocks[0].Header.BlockType = BPLib_BlockType_HopCount;
    DeserializedBundle.blocks.ExtBlocks[1].Header.BlockType = BPLib_BlockType_HopCount;

    UtAssert_INT32_EQ(BPLib_BI_ValidateBundle(&DeserializedBundle), BPLIB_BI_INVALID_BUNDLE_ERR);        
}

/* Test that bundle validation fails when the age block is duplicated */
void Test_BPLib_BI_ValidateBundle_DuplAge(void)
{
    DeserializedBundle.blocks.ExtBlocks[0].Header.BlockType = BPLib_BlockType_Age;
    DeserializedBundle.blocks.ExtBlocks[1].Header.BlockType = BPLib_BlockType_Age;

    UtAssert_INT32_EQ(BPLib_BI_ValidateBundle(&DeserializedBundle), BPLIB_BI_INVALID_BUNDLE_ERR);        
}

/* Test that bundle validation fails when the previous node block is duplicated */
void Test_BPLib_BI_ValidateBundle_DuplPrevNode(void)
{
    DeserializedBundle.blocks.ExtBlocks[0].Header.BlockType = BPLib_BlockType_PrevNode;
    DeserializedBundle.blocks.ExtBlocks[1].Header.BlockType = BPLib_BlockType_PrevNode;

    UtAssert_INT32_EQ(BPLib_BI_ValidateBundle(&DeserializedBundle), BPLIB_BI_INVALID_BUNDLE_ERR);        
}

/* Test that bundle validation succeeds when no age block is present but create time is valid */
void Test_BPLib_BI_ValidateBundle_ValidTime(void)
{
    DeserializedBundle.blocks.ExtBlocks[1].Header.BlockType = BPLib_BlockType_Reserved;
    DeserializedBundle.blocks.PrimaryBlock.Timestamp.CreateTime = 10;

    UtAssert_INT32_EQ(BPLib_BI_ValidateBundle(&DeserializedBundle), BPLIB_SUCCESS);        
}

/* Test that bundle validation fails when no age block is present and create time is invalid */
void Test_BPLib_BI_ValidateBundle_NoTime(void)
{
    DeserializedBundle.blocks.ExtBlocks[1].Header.BlockType = BPLib_BlockType_Reserved;
    DeserializedBundle.blocks.PrimaryBlock.Timestamp.CreateTime = 0;

    UtAssert_INT32_EQ(BPLib_BI_ValidateBundle(&DeserializedBundle), BPLIB_BI_INVALID_BUNDLE_ERR);        
}

/* Test that bundle validation fails the create time indicates an expired bundle */
void Test_BPLib_BI_ValidateBundle_Expired(void)
{
    DeserializedBundle.blocks.ExtBlocks[1].Header.BlockType = BPLib_BlockType_Reserved;
    DeserializedBundle.blocks.PrimaryBlock.Timestamp.CreateTime = 10;
    DeserializedBundle.blocks.PrimaryBlock.Lifetime = 12;

    UT_SetDeferredRetcode(UT_KEY(BPLib_TIME_GetCurrentDtnTime), 1, 25);

    UtAssert_INT32_EQ(BPLib_BI_ValidateBundle(&DeserializedBundle), BPLIB_BI_EXPIRED_BUNDLE_ERR);        
}

/* Test that bundle validation fails when an extension block number matches the payload number */
void Test_BPLib_BI_ValidateBundle_PayloadNumErr(void)
{
    DeserializedBundle.blocks.ExtBlocks[1].Header.BlockNum = 1;
    DeserializedBundle.blocks.PayloadHeader.BlockNum = 1;

    UtAssert_INT32_EQ(BPLib_BI_ValidateBundle(&DeserializedBundle), BPLIB_BI_INVALID_BUNDLE_ERR);        
}

/* Test that bundle validation fails when an extension block number matches another's */
void Test_BPLib_BI_ValidateBundle_BlockNumErr(void)
{
    DeserializedBundle.blocks.ExtBlocks[0].Header.BlockNum = 2;
    DeserializedBundle.blocks.ExtBlocks[1].Header.BlockNum = 2;

    UtAssert_INT32_EQ(BPLib_BI_ValidateBundle(&DeserializedBundle), BPLIB_BI_INVALID_BUNDLE_ERR);        
}

void Test_BPLib_BI_BlobCopyOut_InputBundleNullError(void)
{
    BPLib_Status_t ReturnStatus;
    uint8_t OutputBuffer[2048];
    size_t OutputSize;

    ReturnStatus = BPLib_BI_BlobCopyOut(NULL,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
}

void Test_BPLib_BI_BlobCopyOut_InputBundleBlobNullError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[2048];
    size_t OutputSize;

    memset(&InputBundle, 0, sizeof(InputBundle));
    InputBundle.blob = NULL;

    ReturnStatus = BPLib_BI_BlobCopyOut(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
}


void Test_BPLib_BI_BlobCopyOut_OutputBundleBufNullError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[2048];
    size_t OutputSize;

    ReturnStatus = BPLib_BI_BlobCopyOut(&InputBundle,
                                        NULL,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
}


void Test_BPLib_BI_BlobCopyOut_OutputSizeBufNullError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[2048];

    ReturnStatus = BPLib_BI_BlobCopyOut(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        NULL);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
}



void Test_BPLib_BI_BlobCopyOut_Nominal(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Bundle_t InputBundle;
    uint8_t OutputBuffer[1024];
    size_t OutputSize = 0xdeadbeef;
    BPLib_MEM_Block_t FirstBlock;

    memset(&InputBundle, 0, sizeof(InputBundle));
    memset(&FirstBlock, 0, sizeof(FirstBlock));
    InputBundle.blob = &FirstBlock;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_CBOR_EncodeBundle), BPLIB_SUCCESS);

    ReturnStatus = BPLib_BI_BlobCopyOut(&InputBundle,
                                        OutputBuffer,
                                        sizeof(OutputBuffer),
                                        &OutputSize);

    UtAssert_EQ(BPLib_Status_t, ReturnStatus, BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(BPLib_CBOR_EncodeBundle, 1);
}

void TestBplibBi_Register(void)
{
    UtTest_Add(Test_BPLib_BI_RecvFullBundleIn_NullInputErrors, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvFullBundleIn_NullInputErrors");
    UtTest_Add(Test_BPLib_BI_RecvFullBundleIn_MemAllocError, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvFullBundleIn_MemAllocError");
    UtTest_Add(Test_BPLib_BI_RecvFullBundleIn_UnintellErr, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvFullBundleIn_UnintellErr");
    UtTest_Add(Test_BPLib_BI_RecvFullBundleIn_LenErr, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvFullBundleIn_LenErr");
    UtTest_Add(Test_BPLib_BI_RecvFullBundleIn_HopErr, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvFullBundleIn_HopErr");
    UtTest_Add(Test_BPLib_BI_RecvFullBundleIn_BlkErr, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvFullBundleIn_BlkErr");
    UtTest_Add(Test_BPLib_BI_RecvFullBundleIn_Nominal, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvFullBundleIn_Nominal");
    UtTest_Add(Test_BPLib_BI_RecvFullBundleIn_Invalid, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvFullBundleIn_Invalid");
    UtTest_Add(Test_BPLib_BI_RecvFullBundleIn_JobFail, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvFullBundleIn_JobFail");
    UtTest_Add(Test_BPLib_BI_RecvFullBundleIn_IdErr, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvFullBundleIn_IdErr");
    UtTest_Add(Test_BPLib_BI_RecvFullBundleIn_ExpireErr, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvFullBundleIn_ExpireErr");
    UtTest_Add(Test_BPLib_BI_RecvCtrlMsg_Nominal, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvCtrlMsg_Nominal");

    UtTest_Add(Test_BPLib_BI_ValidateBundle_Null, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_ValidateBundle_Null");
    UtTest_Add(Test_BPLib_BI_ValidateBundle_NoPayload, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_ValidateBundle_NoPayload");
    UtTest_Add(Test_BPLib_BI_ValidateBundle_DuplHopCount, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_ValidateBundle_DuplHopCount");
    UtTest_Add(Test_BPLib_BI_ValidateBundle_DuplAge, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_ValidateBundle_DuplAge");
    UtTest_Add(Test_BPLib_BI_ValidateBundle_DuplPrevNode, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_ValidateBundle_DuplPrevNode");
    UtTest_Add(Test_BPLib_BI_ValidateBundle_ValidTime, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_ValidateBundle_ValidTime");
    UtTest_Add(Test_BPLib_BI_ValidateBundle_NoTime, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_ValidateBundle_NoTime");
    UtTest_Add(Test_BPLib_BI_ValidateBundle_Expired, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_ValidateBundle_Expired");
    UtTest_Add(Test_BPLib_BI_ValidateBundle_PayloadNumErr, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_ValidateBundle_PayloadNumErr");
    UtTest_Add(Test_BPLib_BI_ValidateBundle_BlockNumErr, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_ValidateBundle_BlockNumErr");

    UtTest_Add(Test_BPLib_BI_BlobCopyOut_InputBundleNullError, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_BlobCopyOut_InputBundleNullError");
    UtTest_Add(Test_BPLib_BI_BlobCopyOut_InputBundleBlobNullError, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_BlobCopyOut_InputBundleBlobNullError");
    UtTest_Add(Test_BPLib_BI_BlobCopyOut_OutputBundleBufNullError, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_BlobCopyOut_OutputBundleBufNullError");
    UtTest_Add(Test_BPLib_BI_BlobCopyOut_OutputSizeBufNullError, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_BlobCopyOut_OutputSizeBufNullError");
    UtTest_Add(Test_BPLib_BI_BlobCopyOut_Nominal, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_BlobCopyOut_Nominal");
}
