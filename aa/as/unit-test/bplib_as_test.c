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

/* ======== */
/* Includes */
/* ======== */

#include "bplib_as_test_utils.h"

/*
** Test function for
** int BPLib_AS_Init()
*/
void Test_BPLib_AS_Init(void)
{
    UtAssert_INT32_EQ(BPLib_AS_Init(), BPLIB_SUCCESS);
}

void Test_BPLib_AS_Get_Node_Nominal()
{
    uint32_t* ValuePtr;

    /* Get the default values of the bundle number of custody signals generated */
    ValuePtr = (uint32_t*) BPLib_AS_Get(NODE_COUNTER, BUNDLE_CNT_GEN_CUSTODY);

    UtAssert_EQ(uint32_t, *ValuePtr, 0);

    /* Set a value and verify that it was received */
    BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived = 5;

    ValuePtr = (uint32_t*) BPLib_AS_Get(NODE_COUNTER, BUNDLE_CNT_GEN_CUSTODY);

    UtAssert_EQ(uint32_t, *ValuePtr, 5);
}

void Test_BPLib_AS_Get_Node_Error()
{

}

void Test_BPLib_AS_Get_Source_Nominal()
{

}

void Test_BPLib_AS_Get_Source_Error()
{

}

void Test_BPLib_AS_Set_Nominal()
{

}

void Test_BPLib_AS_Set_Error()
{

}

void Test_BPLib_AS_Increment_Nominal()
{

}

void Test_BPLib_AS_Increment_Error()
{

}

void Test_BPLib_AS_Decrement_Nominal()
{

}

void Test_BPLib_AS_Decrement_Error()
{

}

void Test_BPLib_AS_SetAllZero_Nominal()
{

}

void Test_BPLib_AS_SetAllZero_Error()
{

}

void TestBplibAs_Register(void)
{
    ADD_TEST(Test_BPLib_AS_Init);
    ADD_TEST(Test_BPLib_AS_Get_Node_Nominal);
    ADD_TEST(Test_BPLib_AS_Get_Node_Error);
    ADD_TEST(Test_BPLib_AS_Set_Nominal);
    ADD_TEST(Test_BPLib_AS_Set_Error);
    ADD_TEST(Test_BPLib_AS_Increment_Nominal);
    ADD_TEST(Test_BPLib_AS_Increment_Error);
    ADD_TEST(Test_BPLib_AS_Decrement_Nominal);
    ADD_TEST(Test_BPLib_AS_Decrement_Error);
    ADD_TEST(Test_BPLib_AS_SetAllZero_Nominal);
    ADD_TEST(Test_BPLib_AS_SetAllZero_Error);
}
