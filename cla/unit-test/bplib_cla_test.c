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

#include "bplib_cla_test_utils.h"

#include "bplib_bi.h"
#include "bplib_qm_waitqueue.h"
#include "bplib_mem.h"


void Test_BPLib_CLA_Ingress_NullInstPtrError(void)
{
    BPLib_Status_t ReturnStatus;
    uint32_t ContId = 0;
    uint8_t InputBundleBuffer[30];
    uint8_t* Bundle = NULL;
    size_t InputBundleSize = sizeof(InputBundleBuffer);
    uint32_t Timeout = 0;

    ReturnStatus = BPLib_CLA_Ingress(NULL,
                                     ContId,
                                     InputBundleBuffer,
                                     InputBundleSize,
                                     Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_ERROR);
}


void Test_BPLib_CLA_Ingress_NullInputBundleError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t InputInstance;
    uint32_t ContId = 0;
    uint8_t InputBundleBuffer[30];
    uint8_t* Bundle = NULL;
    size_t InputBundleSize = sizeof(InputBundleBuffer);
    uint32_t Timeout = 0;

    ReturnStatus = BPLib_CLA_Ingress(&InputInstance,
                                     ContId,
                                     NULL,
                                     InputBundleSize,
                                     Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_ERROR);
}


// void Test_BPLib_CLA_Ingress_Nominal(void)
// {
//     uint32_t ContId = 0;
//     uint8_t Bundle[5]= {0x9F, 0x0, 0x0, 0x0, 0x0};
//     size_t Size = 10;
//     uint32_t Timeout = 0;
    
//     UtAssert_INT32_EQ(BPLib_CLA_Ingress(ContId, Bundle, Size, Timeout), BPLIB_SUCCESS);
// }

// void Test_BPLib_CLA_Ingress_MsgData(void)
// {
//     uint32_t ContId = 0;
//     char Bundle[8]= {"BPNMSG"};
//     size_t Size = 10;
//     uint32_t Timeout = 0;
//     UtAssert_INT32_EQ(BPLib_CLA_Ingress(ContId, Bundle, Size, Timeout), BPLIB_SUCCESS);
// }

// void Test_BPLib_CLA_Ingress_NonControlMsg(void)
// {
//     uint32_t ContId = 0;
//     uint8_t Bundle[5]= {1, 0, 0, 0, 0};
//     size_t Size = 10;
//     uint32_t Timeout = 0;
//     UT_SetDefaultReturnValue(UT_KEY(BPLib_CLA_IsAControlMsg), 0);
//     UT_SetDefaultReturnValue(UT_KEY(BPLib_CLA_ProcessControlMessage), BPLIB_ERROR);    
//     UtAssert_INT32_EQ(BPLib_CLA_Ingress(ContId, Bundle, Size, Timeout), BPLIB_SUCCESS);
// }

void Test_BPLib_CLA_Egress_NullInstanceInputError(void)
{
    BPLib_Status_t ReturnStatus;
    uint32_t ContId = 0;
    uint8_t OutputBundleBuffer[30];
    size_t NumBytesCopiedToOutputBuf;
    size_t OutputBundleBufferLength = sizeof(OutputBundleBuffer);
    uint32_t Timeout = 0;

    ReturnStatus = BPLib_CLA_Egress(NULL,
                                    ContId,
                                    OutputBundleBuffer,
                                    &NumBytesCopiedToOutputBuf,
                                    OutputBundleBufferLength,
                                    Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
    UtAssert_STUB_COUNT(BPLib_QM_WaitQueueTryPull, 0);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 0);
}


void Test_BPLib_CLA_Egress_NullBundleOutBufferError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    uint32_t ContId = 0;
    uint8_t OutputBundleBuffer[30];
    size_t NumBytesCopiedToOutputBuf;
    size_t OutputBundleBufferLength = sizeof(OutputBundleBuffer);
    uint32_t Timeout = 0;

    ReturnStatus = BPLib_CLA_Egress(&Instance,
                                    ContId,
                                    NULL,
                                    &NumBytesCopiedToOutputBuf,
                                    OutputBundleBufferLength,
                                    Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
    UtAssert_STUB_COUNT(BPLib_QM_WaitQueueTryPull, 0);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 0);
}


void Test_BPLib_CLA_Egress_NullSizeBufferError(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    uint32_t ContId = 0;
    uint8_t OutputBundleBuffer[30];
    // size_t NumBytesCopiedToOutputBuf;
    size_t OutputBundleBufferLength = sizeof(OutputBundleBuffer);
    uint32_t Timeout = 0;

    ReturnStatus = BPLib_CLA_Egress(&Instance,
                                    ContId,
                                    OutputBundleBuffer,
                                    NULL,
                                    OutputBundleBufferLength,
                                    Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_NULL_PTR_ERROR);
    UtAssert_STUB_COUNT(BPLib_QM_WaitQueueTryPull, 0);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 0);
}


void Test_BPLib_CLA_Egress_QueuePullTimeout(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    uint32_t ContId = 0;
    uint8_t OutputBundleBuffer[30];
    size_t NumBytesCopiedToOutputBuf;
    size_t OutputBundleBufferLength = sizeof(OutputBundleBuffer);
    uint32_t Timeout = 0;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_QM_WaitQueueTryPull), false);

    ReturnStatus = BPLib_CLA_Egress(&Instance,
                                    ContId,
                                    OutputBundleBuffer,
                                    &NumBytesCopiedToOutputBuf,
                                    OutputBundleBufferLength,
                                    Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_CLA_TIMEOUT);
    UtAssert_STUB_COUNT(BPLib_QM_WaitQueueTryPull, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 0);
}


void Test_BPLib_CLA_Egress_BlobCopyFail(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    uint32_t ContId = 0;
    uint8_t OutputBundleBuffer[30];
    size_t NumBytesCopiedToOutputBuf;
    size_t OutputBundleBufferLength = sizeof(OutputBundleBuffer);
    uint32_t Timeout = 0;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_QM_WaitQueueTryPull), true);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_BI_BlobCopyOut), BPLIB_BUF_LEN_ERROR);

    ReturnStatus = BPLib_CLA_Egress(&Instance,
                                    ContId,
                                    OutputBundleBuffer,
                                    &NumBytesCopiedToOutputBuf,
                                    OutputBundleBufferLength,
                                    Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_BUF_LEN_ERROR);
    UtAssert_STUB_COUNT(BPLib_QM_WaitQueueTryPull, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);
}


void Test_BPLib_CLA_Egress_Nominal(void)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Instance_t Instance;
    uint32_t ContId = 0;
    uint8_t OutputBundleBuffer[30];
    size_t NumBytesCopiedToOutputBuf;
    size_t OutputBundleBufferLength = sizeof(OutputBundleBuffer);
    uint32_t Timeout = 0;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_QM_WaitQueueTryPull), true);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_BI_BlobCopyOut), BPLIB_SUCCESS);

    ReturnStatus = BPLib_CLA_Egress(&Instance,
                                    ContId,
                                    OutputBundleBuffer,
                                    &NumBytesCopiedToOutputBuf,
                                    OutputBundleBufferLength,
                                    Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(BPLib_QM_WaitQueueTryPull, 1);
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);
}


void Test_BPLib_CLA_ContactsTblValidateFunc_Nominal(void)
{
    BPLib_Status_t ReturnStatus;
    uint8_t ContactTableIndex;
    BPLib_CLA_ContactsTable_t TestTblData;

    /* Set input table to all valid values */
    memset(&TestTblData, 0, sizeof(TestTblData));
    for (ContactTableIndex = 0; ContactTableIndex < BPLIB_MAX_NUM_CONTACTS; ContactTableIndex++)
    {
        TestTblData.ContactSet[ContactTableIndex].PortNum = 10;
    }

    /* Run UUT and check results */
    ReturnStatus = BPLib_CLA_ContactsTblValidateFunc(&TestTblData);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
}

void Test_BPLib_CLA_ContactsTblValidateFunc_FirstEntryInvalid(void)
{
    BPLib_Status_t ReturnStatus;
    uint8_t ContactTableIndex;
    BPLib_CLA_ContactsTable_t TestTblData;

    /* Set input table to all valid values */
    memset(&TestTblData, 0, sizeof(TestTblData));
    for (ContactTableIndex = 0; ContactTableIndex < BPLIB_MAX_NUM_CONTACTS; ContactTableIndex++)
    {
        TestTblData.ContactSet[ContactTableIndex].PortNum = 10;
    }

    /*
    ** Inject an error in the first table entry
    */
    TestTblData.ContactSet[0].PortNum = 0;

    /* Run UUT and check results */
    ReturnStatus = BPLib_CLA_ContactsTblValidateFunc(&TestTblData);
    UtAssert_INT32_EQ(ReturnStatus, BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE);
}

void TestBplibCla_Register(void)
{
    UtTest_Add(Test_BPLib_CLA_Ingress_NullInstPtrError, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_NullInstPtrError");
    UtTest_Add(Test_BPLib_CLA_Ingress_NullInputBundleError, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_NullInputBundleError");

    // UtTest_Add(Test_BPLib_CLA_Ingress_Nominal, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_Nominal");
    // UtTest_Add(Test_BPLib_CLA_Ingress_MsgData, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_MsgData");
    // UtTest_Add(Test_BPLib_CLA_Ingress_NonControlMsg, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_NonControlMsg");

    UtTest_Add(Test_BPLib_CLA_ContactsTblValidateFunc_Nominal, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_ContactsTblValidateFunc_Nominal");
    UtTest_Add(Test_BPLib_CLA_ContactsTblValidateFunc_FirstEntryInvalid, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_ContactsTblValidateFunc_FirstEntryInvalid");

    UtTest_Add(Test_BPLib_CLA_Egress_NullInstanceInputError, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Egress_NullInstanceInputError");
    UtTest_Add(Test_BPLib_CLA_Egress_NullBundleOutBufferError, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Egress_NullBundleOutBufferError");
    UtTest_Add(Test_BPLib_CLA_Egress_NullSizeBufferError, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Egress_NullSizeBufferError");
    UtTest_Add(Test_BPLib_CLA_Egress_QueuePullTimeout, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Egress_QueuePullTimeout");
    UtTest_Add(Test_BPLib_CLA_Egress_BlobCopyFail, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Egress_BlobCopyFail");
    UtTest_Add(Test_BPLib_CLA_Egress_Nominal, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Egress_Nominal");
}
