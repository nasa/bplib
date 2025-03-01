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

/*
** Test function for
** int BPLib_CLA_Init()
*/
// void Test_BPLib_CLA_Init(void)
// {
//     UtAssert_INT32_EQ(BPLib_CLA_Init(), BPLIB_SUCCESS);
// }

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

// void Test_BPLib_CLA_Ingress_NULLBundle(void)
// {
//     uint32_t ContId = 0;
//     uint8_t* Bundle = NULL;
//     size_t Size = 10;
//     uint32_t Timeout = 0;
//     UtAssert_INT32_EQ(BPLib_CLA_Ingress(ContId, Bundle, Size, Timeout), BPLIB_SUCCESS);
// }

// void Test_BPLib_CLA_ContactsTblValidateFunc_Nominal(void)
// {
//     BPLib_CLA_ContactsTable_t TestTblData;
//     memset(&TestTblData, 0, sizeof(TestTblData));
//     TestTblData.ContactSet[0].PortNum = 10;
//     UtAssert_INT32_EQ((int32) BPLib_CLA_ContactsTblValidateFunc(&TestTblData), (int32) BPLIB_SUCCESS);     
// }

// void Test_BPLib_CLA_ContactsTblValidateFunc_Invalid(void)
// {
//     BPLib_CLA_ContactsTable_t TestTblData;
//     memset(&TestTblData, 0, sizeof(TestTblData));

//     /* Error case should return BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE */
//     TestTblData.ContactSet[0].PortNum = 0;

//     UtAssert_INT32_EQ(BPLib_CLA_ContactsTblValidateFunc(&TestTblData), 
//                                                 BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE);
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

    ReturnStatus = BPLib_CLA_Egress(&Instance,
                                    ContId,
                                    OutputBundleBuffer,
                                    &NumBytesCopiedToOutputBuf,
                                    OutputBundleBufferLength,
                                    Timeout);

    UtAssert_INT32_EQ(ReturnStatus, BPLIB_SUCCESS);
}

void TestBplibCla_Register(void)
{
    // UtTest_Add(Test_BPLib_CLA_Init, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Init");
    // UtTest_Add(Test_BPLib_CLA_Ingress_Nominal, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_Nominal");
    // UtTest_Add(Test_BPLib_CLA_Ingress_MsgData, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_MsgData");
    // UtTest_Add(Test_BPLib_CLA_Ingress_NonControlMsg, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_NonControlMsg");
    // UtTest_Add(Test_BPLib_CLA_Ingress_NULLBundle, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_NULLBundle");
    // UtTest_Add(Test_BPLib_CLA_ContactsTblValidateFunc_Nominal, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_ContactsTblValidateFunc_Nominal");
    // UtTest_Add(Test_BPLib_CLA_ContactsTblValidateFunc_Invalid, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_ContactsTblValidateFunc_Invalid");
    UtTest_Add(Test_BPLib_CLA_Egress_NullInstanceInputError, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Egress_NullInstanceInputError");
    UtTest_Add(Test_BPLib_CLA_Egress_NullBundleOutBufferError, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Egress_NullBundleOutBufferError");
    UtTest_Add(Test_BPLib_CLA_Egress_NullSizeBufferError, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Egress_NullSizeBufferError");
    UtTest_Add(Test_BPLib_CLA_Egress_Nominal, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Egress_Nominal");
}
