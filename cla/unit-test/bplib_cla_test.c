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

BPLib_Status_t BPLib_BI_RecvFullBundleIn(const void *BundleInPtr, size_t Size) 
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_BI_SendFullBundleOut(void *BundleOutPtr, size_t *Size) 
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_BI_RecvCtrlMsg(BPLib_CLA_CtrlMsg_t* MsgPtr)
{
    return BPLIB_SUCCESS;
}

/*
** Test function for
** int BPLib_CLA_Init()
*/
void Test_BPLib_CLA_Init(void)
{
    UtAssert_INT32_EQ(BPLib_CLA_Init(), BPLIB_SUCCESS);
}

void Test_BPLib_CLA_Ingress_Nominal(void)
{
    uint32_t ContId = 0;
    uint8_t Bundle[5]= {0x9F, 0x0, 0x0, 0x0, 0x0};
    size_t Size = 10;
    uint32_t Timeout = 0;
    
    UtAssert_INT32_EQ(BPLib_CLA_Ingress(ContId, Bundle, Size, Timeout), BPLIB_SUCCESS);
}

void Test_BPLib_CLA_Egress_Nominal(void)
{
    uint32_t ContId = 0;
    uint8_t Bundle[]= {0x9F, 0x0, 0x0, 0x0, 0x0};
    size_t Size;
    uint32_t Timeout = 0;
    UtAssert_INT32_EQ(BPLib_CLA_Egress(ContId, Bundle, &Size, Timeout), BPLIB_SUCCESS);
}

void Test_BPLib_CLA_Ingress_MsgData(void)
{
    uint32_t ContId = 0;
    char Bundle[8]= {"BPNMSG"};
    size_t Size = 10;
    uint32_t Timeout = 0;
    UtAssert_INT32_EQ(BPLib_CLA_Ingress(ContId, Bundle, Size, Timeout), BPLIB_SUCCESS);
}

void Test_BPLib_CLA_Ingress_NonControlMsg(void)
{
    uint32_t ContId = 0;
    uint8_t Bundle[5]= {1, 0, 0, 0, 0};
    size_t Size = 10;
    uint32_t Timeout = 0;
    UT_SetDefaultReturnValue(UT_KEY(BPLib_CLA_IsAControlMsg), 0);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_CLA_ProcessControlMessage), BPLIB_ERROR);    
    UtAssert_INT32_EQ(BPLib_CLA_Ingress(ContId, Bundle, Size, Timeout), BPLIB_SUCCESS);
}

void Test_BPLib_CLA_Ingress_NULLBundle(void)
{
    uint32_t ContId = 0;
    uint8_t* Bundle = NULL;
    size_t Size = 10;
    uint32_t Timeout = 0;
    UtAssert_INT32_EQ(BPLib_CLA_Ingress(ContId, Bundle, Size, Timeout), BPLIB_SUCCESS);
}

void Test_BPLib_CLA_Egress_NULLBundle(void)
{
    uint32_t ContId = 0;
    uint8_t* Bundle = NULL;
    size_t Size;
    uint32_t Timeout = 0;
    UtAssert_INT32_EQ(BPLib_CLA_Egress(ContId, Bundle, &Size, Timeout), BPLIB_SUCCESS);
}

void TestBplibCla_Register(void)
{
    UtTest_Add(Test_BPLib_CLA_Init, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Init");
    UtTest_Add(Test_BPLib_CLA_Ingress_Nominal, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_Nominal");
    UtTest_Add(Test_BPLib_CLA_Egress_Nominal, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Egress_Nominal");
    UtTest_Add(Test_BPLib_CLA_Ingress_MsgData, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_MsgData");
    UtTest_Add(Test_BPLib_CLA_Ingress_NonControlMsg, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_NonControlMsg");
    UtTest_Add(Test_BPLib_CLA_Ingress_NULLBundle, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Ingress_NULLBundle");
    UtTest_Add(Test_BPLib_CLA_Egress_NULLBundle, BPLib_CLA_Test_Setup, BPLib_CLA_Test_Teardown, "Test_BPLib_CLA_Egress_NULLBundle");
}
