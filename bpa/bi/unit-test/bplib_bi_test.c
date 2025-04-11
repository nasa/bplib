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
    UtAssert_INT32_EQ(BPLib_BI_RecvFullBundleIn(NULL, NULL, Size), BPLIB_NULL_PTR_ERROR);
    UtAssert_INT32_EQ(BPLib_BI_RecvFullBundleIn(NULL, BundleIn, Size), BPLIB_NULL_PTR_ERROR);
    UtAssert_INT32_EQ(BPLib_BI_RecvFullBundleIn(&instance, NULL, Size), BPLIB_NULL_PTR_ERROR);

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

    ReturnStatus = BPLib_BI_RecvFullBundleIn(&Instance, BundleIn, Size);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleAlloc, 1);
    UtAssert_STUB_COUNT(BPLib_CBOR_DecodeBundle, 0);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 0);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 0);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 0);
}


void Test_BPLib_BI_RecvFullBundleIn_CborDecodeError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    BPLib_Bundle_t AllocatedBundleMem;
    char BundleIn[32];
    size_t Size = 0;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_BundleAlloc), (UT_IntReturn_t) &AllocatedBundleMem);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_CBOR_DecodeBundle), BPLIB_CBOR_DEC_BUNDLE_TOO_SHORT_ERR);

    ReturnStatus = BPLib_BI_RecvFullBundleIn(&Instance, BundleIn, Size);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_CBOR_DEC_BUNDLE_TOO_SHORT_ERR);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleAlloc, 1);
    UtAssert_STUB_COUNT(BPLib_CBOR_DecodeBundle, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 2);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 0);
}


void Test_BPLib_BI_RecvFullBundleIn_Nominal(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    BPLib_Bundle_t AllocatedBundleMem;
    char BundleIn[32];
    size_t Size = 0;

    memset(&AllocatedBundleMem, 0, sizeof(AllocatedBundleMem));
    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_BundleAlloc), (UT_IntReturn_t) &AllocatedBundleMem);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_CBOR_DecodeBundle), BPLIB_SUCCESS);

    ReturnStatus = BPLib_BI_RecvFullBundleIn(&Instance, BundleIn, Size);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleAlloc, 1);
    UtAssert_STUB_COUNT(BPLib_CBOR_DecodeBundle, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 0);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 0);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 1);
}


void Test_BPLib_BI_RecvCtrlMsg_Nominal(void)
{
    BPLib_CLA_CtrlMsg_t* MsgPtr = NULL;
    UtAssert_INT32_EQ(BPLib_BI_RecvCtrlMsg(MsgPtr), BPLIB_SUCCESS);    
}

void Test_BPLib_BI_ValidateBundle_Nominal(void)
{
    UtAssert_INT32_EQ(BPLib_BI_ValidateBundle(NULL), BPLIB_SUCCESS);        
}


void Test_BPLib_BI_BlobCopyOut_InputBundleNullError(void)
{
    BPLib_Status_t ReturnStatus;
    // BPLib_Bundle_t InputBundle;
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
    // size_t OutputSize;

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



/*
** TODO: Finish BPLib_BI_BlobCopyOut unit tests
*/

void TestBplibBi_Register(void)
{
    UtTest_Add(Test_BPLib_BI_RecvFullBundleIn_NullInputErrors, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvFullBundleIn_NullInputErrors");
    UtTest_Add(Test_BPLib_BI_RecvFullBundleIn_MemAllocError, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvFullBundleIn_MemAllocError");
    UtTest_Add(Test_BPLib_BI_RecvFullBundleIn_CborDecodeError, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvFullBundleIn_CborDecodeError");
    UtTest_Add(Test_BPLib_BI_RecvFullBundleIn_Nominal, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvFullBundleIn_Nominal");

    UtTest_Add(Test_BPLib_BI_RecvCtrlMsg_Nominal, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_RecvCtrlMsg_Nominal");
    UtTest_Add(Test_BPLib_BI_ValidateBundle_Nominal, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_ValidateBundle_Nominal");

    UtTest_Add(Test_BPLib_BI_BlobCopyOut_InputBundleNullError, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_BlobCopyOut_InputBundleNullError");
    UtTest_Add(Test_BPLib_BI_BlobCopyOut_InputBundleBlobNullError, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_BlobCopyOut_InputBundleBlobNullError");
    UtTest_Add(Test_BPLib_BI_BlobCopyOut_OutputBundleBufNullError, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_BlobCopyOut_OutputBundleBufNullError");
    UtTest_Add(Test_BPLib_BI_BlobCopyOut_OutputSizeBufNullError, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_BlobCopyOut_OutputSizeBufNullError");

    UtTest_Add(Test_BPLib_BI_BlobCopyOut_Nominal, BPLib_BI_Test_Setup, BPLib_BI_Test_Teardown, "Test_BPLib_BI_BlobCopyOut_Nominal");
}
