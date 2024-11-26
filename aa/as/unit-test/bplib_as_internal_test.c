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

void Test_BPLib_AS_EidIsValid_Nominal(void)
{
    UtAssert_BOOL_TRUE(BPLib_AS_EidIsValid(0));
    UtAssert_BOOL_TRUE(BPLib_AS_EidIsValid(5));
    UtAssert_BOOL_TRUE(BPLib_AS_EidIsValid(BPLIB_MAX_NUM_SOURCE_EID - 1));
}

void Test_BPLib_AS_EidIsValid_Error(void)
{
    UtAssert_BOOL_FALSE(BPLib_AS_EidIsValid(-1));
    UtAssert_BOOL_FALSE(BPLib_AS_EidIsValid(BPLIB_MAX_NUM_SOURCE_EID));
}

void Test_BPLib_AS_SetGet_Nominal(void)
{
    BPLib_Status_t Status;
    uint32_t TestValue;
    uint32_t GetValue;
    uint32_t CounterToTest;
    int32_t SourceEid;

    SourceEid = 1;

    for(CounterToTest = 0; CounterToTest < BPLIB_AS_NUM_NODE_CNTRS; CounterToTest++)
    {
        /*
        ** Modify the value assigned to the counters just to verify that values are held over 
        ** from a previous iteration of the test
        */
        TestValue = CounterToTest + 3;

        /* Set node values to test against */
        Test_BPLib_AS_SetNodeCounterValues(0);

        /* Set source counter values */
        // Test_BPLib_AS_SetSourceCounterValues(SourceEid, 0);
        
        /* Run function under test */
        Status = BPLib_AS_Set(SourceEid, (BPLib_AS_Counter_t) CounterToTest, TestValue);
        
        /* Assert that AS_Set() ran successfully */
        UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);

        /* Run function under test */
        Status = BPLib_AS_Get(SourceEid, (BPLib_AS_Counter_t) CounterToTest, &GetValue);

        /* Assert that TestValue was updated and BPLib_AS_Get() was run successfully */
        UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
        UtAssert_EQ(uint32_t, TestValue, GetValue);
    }
}

void Test_BPLib_AS_Get_Error(void)
{
    BPLib_Status_t Status;
    uint32_t TestValue;
    int16_t SourceEid;

    /* === Invalid EID test === */

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.AduCountReceived = 10;
    TestValue = 0;
    SourceEid = BPLIB_MAX_NUM_SOURCE_EID;

    /* Run the function under test */
    Status = BPLib_AS_Get(SourceEid, ADU_COUNT_RECEIVED, &TestValue);

    /* Assert that TestValue wasn't updated and BPLib_AS_Get() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_EID, Status);
    UtAssert_EQ(uint32_t, TestValue, 0);

    /* === Unknown node counter test === */

    /* Set values to test against */
    TestValue = 56;
    SourceEid = 1;

    /* Run the function under test */
    Status = BPLib_AS_Get(SourceEid, BPLIB_AS_NUM_NODE_CNTRS, &TestValue);

    /* Assert that TestValue wasn't updated and BPLib_AS_Get() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_NODE_CNTR, Status);
    UtAssert_EQ(uint32_t, TestValue, 56);

    /* === Unknown source counter test ===

    // Set values to test against
    TestValue = 20;
    SourceEid = 2;

    // Run the function under test
    Status = BPLib_AS_Get(SourceEid, BPLIB_AS_NUM_NODE_CNTRS, &TestValue);

    // Assert that TestValue wasn't updated and BPLib_AS_Get() was run unsuccessfully
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_SRC_CNTR, Status);
    UtAssert_EQ(uint32_t, TestValue, 20);
    */
}

void Test_BPLib_AS_Set_Error(void)
{
    BPLib_Status_t Status;
    int64_t TestValue;
    int16_t SourceEid;

    /* === Invalid EID test === */

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleCountCustodyReForwarded = 0;
    TestValue = 10;
    SourceEid = BPLIB_MAX_NUM_SOURCE_EID;

    /* Run the function under test */
    Status = BPLib_AS_Set(SourceEid, BUNDLE_COUNT_CUSTODY_RE_FORWARDED, (uint32_t) TestValue);

    /* Assert that the counter wasn't updated and BPLib_AS_Set() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_EID, Status);
    UtAssert_EQ(uint32_t, 0, BPLib_AS_NodeCountersPayload.BundleCountCustodyReForwarded);

    /* === Unknown node counter test === */

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleCountCustodyReForwarded = 0;
    TestValue = 5;
    SourceEid = 1;

    /* Run the function under test */
    Status = BPLib_AS_Set(SourceEid, BPLIB_AS_NUM_NODE_CNTRS, (uint32_t) TestValue);

    /* Assert that BPLib_AS_Set() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_NODE_CNTR, Status);

    /* === Unknown source counter test ===

    // Set values to test against
    BPLib_AS_NodeCountersPayload.BundleCountCustodyReForwarded = 0;
    TestValue = 14;
    SourceEid = 2;

    // Run the function under test
    Status = BPLib_AS_Set(SourceEid, BPLIB_AS_NUM_NODE_CNTRS, TestValue);

    // Assert that BPLib_AS_Set() was run unsuccessfully
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_SRC_CNTR, Status);
    */
}

void TestBplibAsInternal_Register(void)
{
    ADD_TEST(Test_BPLib_AS_EidIsValid_Nominal);
    ADD_TEST(Test_BPLib_AS_EidIsValid_Error);
    ADD_TEST(Test_BPLib_AS_SetGet_Nominal);
    ADD_TEST(Test_BPLib_AS_Get_Error);
    ADD_TEST(Test_BPLib_AS_Set_Error);
}