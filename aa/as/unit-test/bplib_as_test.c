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
void Test_BPLib_AS_Init_Nominal(void)
{
    UtAssert_INT32_EQ(BPLib_AS_Init(), BPLIB_SUCCESS);
}

void Test_BPLib_AS_Init_Error(void)
{
    // UtAssert_INT32_EQ(BPLib_AS_Init(), SOME_ERROR);
}

void Test_BPLib_AS_Get_Nominal(void)
{
    BPLib_Status_t Status;
    uint32_t TestValue;
    int32_t SourceEid;

    /* === Node counter test === */

    /* Indicate that the desired counter is node-specific */
    SourceEid = BPLIB_AS_NODE_EID;

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived = 5;
    TestValue = 0;

    /* Run function under test */
    Status = BPLib_AS_Get(SourceEid, BUNDLE_CNT_CS_RECV, &TestValue);

    /* Assert that TestValue was updated and BPLib_AS_Get() was run successfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
    UtAssert_EQ(uint32_t, TestValue, 5);

    /* === Source counter test === */
    
    /* Indicate that the desired counter is source-specific */
    SourceEid = 2;

    /* Set values to test against */
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected = 12;
    TestValue = 0;

    /* Run function under test */
    Status = BPLib_AS_Get(SourceEid, BUNDLE_CNT_CUSTODY_REJ, &TestValue);

    /* Assert that TestValue was modified and BPLib_AS_Get() was run successfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
    UtAssert_EQ(uint32_t, 12, TestValue);
}

void Test_BPLib_AS_Get_Error(void)
{
    BPLib_Status_t Status;
    uint32_t TestValue;
    int16_t SourceEid;

    /* === Invalid EID test === */

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount = 10;
    TestValue = 0;
    SourceEid = BPLIB_MAX_NUM_SOURCE_EID;

    /* Run the function under test */
    Status = BPLib_AS_Get(SourceEid, BUNDLE_AGT_ACCPT_CNT, &TestValue);

    /* Assert that TestValue wasn't updated and BPLib_AS_Get() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_EID, Status);
    UtAssert_EQ(uint32_t, TestValue, 0);

    /* === Unknown node counter test === */

    /* Set values to test against */
    TestValue = 10;
    SourceEid = BPLIB_AS_NODE_EID; /* Indicates that a node counter is desired */

    /* Run the function under test */
    Status = BPLib_AS_Get(SourceEid, BPLIB_AS_NUM_COUNTERS, &TestValue);

    /* Assert that TestValue wasn't updated and BPLib_AS_Get() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_NODE_CNTR, Status);
    UtAssert_EQ(uint32_t, TestValue, 10);

    /* === Unknown source counter test === */

    /* Set values to test against */
    TestValue = 20;
    SourceEid = 2;

    /* Run the function under test */
    Status = BPLib_AS_Get(SourceEid, BPLIB_AS_NUM_COUNTERS, &TestValue);

    /* Assert that TestValue wasn't updated and BPLib_AS_Get() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_SRC_CNTR, Status);
    UtAssert_EQ(uint32_t, TestValue, 20);
}

void Test_BPLib_AS_Set_Nominal(void)
{
    BPLib_Status_t Status;
    uint32_t TestValue;
    int32_t SourceEid;

    /* === Node counter test === */

    /* Indicate that the desired counter is node-specific */
    SourceEid = BPLIB_AS_NODE_EID;

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived = 0;
    TestValue = 10;

    /* Run function under test */
    Status = BPLib_AS_Set(SourceEid, BUNDLE_CNT_CS_RECV, TestValue);

    /* Assert that the counter was updated and BPLib_AS_Set() was run successfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived);

    /* === Source counter test === */
    
    /* Indicate that the desired counter is source-specific */
    SourceEid = 2;

    /* Set values to test against */
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected = 0;
    TestValue = 20;

    /* Run function under test */
    Status = BPLib_AS_Set(SourceEid, BUNDLE_CNT_CUSTODY_REJ, TestValue);

    /* Assert that the counter was modified and BPLib_AS_Set() was run successfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected);
}

void Test_BPLib_AS_Set_Error(void)
{
    BPLib_Status_t Status;
    uint32_t TestValue;
    int16_t SourceEid;

    /* === Invalid EID test === */

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount = 0;
    TestValue = 10;
    SourceEid = BPLIB_MAX_NUM_SOURCE_EID;

    /* Run the function under test */
    Status = BPLib_AS_Set(SourceEid, BUNDLE_AGT_ACCPT_CNT, TestValue);

    /* Assert that the counter wasn't updated and BPLib_AS_Set() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_EID, Status);
    UtAssert_EQ(uint32_t, 0, BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount);

    /* === Unknown node counter test === */

    /* Set values to test against */
    SourceEid = BPLIB_AS_NODE_EID; /* Indicates that a node counter is desired */

    /* Run the function under test */
    Status = BPLib_AS_Set(SourceEid, BPLIB_AS_NUM_COUNTERS, TestValue);

    /* Assert that BPLib_AS_Set() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_NODE_CNTR, Status);

    /* === Unknown source counter test === */

    /* Set values to test against */
    SourceEid = 2;

    /* Run the function under test */
    Status = BPLib_AS_Set(SourceEid, BPLIB_AS_NUM_COUNTERS, TestValue);

    /* Assert that BPLib_AS_Set() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_SRC_CNTR, Status);
}

void Test_BPLib_AS_Increment_Nominal(void)
{
    BPLib_Status_t Status;
    int16_t SourceEid;

    /* === Node counter test === */

    SourceEid = BPLIB_AS_NODE_EID;

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount = 3;

    /* Run function under test*/
    Status = BPLib_AS_Increment(SourceEid, BUNDLE_AGT_ACCPT_CNT);

    /* Verify that the value was incremented */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
    UtAssert_EQ(uint32_t, 4, BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount);

    /* === Source counter test === */

    SourceEid = 2;

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted = 15;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted = 15;

    /* Run function under test */
    Status = BPLib_AS_Increment(SourceEid, BUNDLE_CNT_GEN_ACCPT);

    /* Verify that the node and source counters were incremented */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
    UtAssert_EQ(uint32_t, 16, BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted);
    UtAssert_EQ(uint32_t, 16, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted);
}

void Test_BPLib_AS_Increment_Error(void)
{
    BPLib_Status_t Status;
    int16_t SourceEid;

    /* === Invalid EID test === */

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount = 0;
    SourceEid = BPLIB_MAX_NUM_SOURCE_EID;

    /* Run the function under test */
    Status = BPLib_AS_Increment(SourceEid, BUNDLE_AGT_ACCPT_CNT);

    /* Assert that the counter wasn't updated and BPLib_AS_Increment() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_EID, Status);
    UtAssert_EQ(uint32_t, 0, BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount);

    /* === Unknown node counter test === */

    /* Set values to test against */
    SourceEid = BPLIB_AS_NODE_EID; /* Indicates that a node counter is desired */

    /* Run the function under test */
    Status = BPLib_AS_Increment(SourceEid, BPLIB_AS_NUM_COUNTERS);

    /* Assert that BPLib_AS_Increment() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_NODE_CNTR, Status);

    /* === Unknown source counter test === */

    /* Set values to test against */
    SourceEid = 2;

    /* Run the function under test */
    Status = BPLib_AS_Increment(SourceEid, BPLIB_AS_NUM_COUNTERS);

    /* Assert that BPLib_AS_Increment() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_SRC_CNTR, Status);
}

void Test_BPLib_AS_Decrement_Nominal(void)
{
    BPLib_Status_t Status;
    int16_t SourceEid;

    /* === Node counter test === */

    SourceEid = BPLIB_AS_NODE_EID;

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount = 3;

    /* Run function under test*/
    Status = BPLib_AS_Decrement(SourceEid, BUNDLE_AGT_ACCPT_CNT);

    /* Verify that the value was decremented */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
    UtAssert_EQ(uint32_t, 2, BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount);

    /* === Source counter test === */

    SourceEid = 2;

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted = 15;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted = 15;

    /* Run function under test */
    Status = BPLib_AS_Decrement(SourceEid, BUNDLE_CNT_GEN_ACCPT);

    /* Verify that the node and source counters were decremented */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
    UtAssert_EQ(uint32_t, 14, BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted);
    UtAssert_EQ(uint32_t, 14, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted);
}

void Test_BPLib_AS_Decrement_Error(void)
{
    BPLib_Status_t Status;
    int16_t SourceEid;

    /* === Invalid EID test === */

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount = 1;
    SourceEid = BPLIB_MAX_NUM_SOURCE_EID;

    /* Run the function under test */
    Status = BPLib_AS_Decrement(SourceEid, BUNDLE_AGT_ACCPT_CNT);

    /* Assert that the counter wasn't updated and BPLib_AS_Decrement() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_EID, Status);
    UtAssert_EQ(uint32_t, 1, BPLib_AS_NodeCountersPayload.BundleAgentAcceptedDirectiveCount);

    /* === Unknown node counter test === */

    /* Set values to test against */
    SourceEid = BPLIB_AS_NODE_EID; /* Indicates that a node counter is desired */

    /* Run the function under test */
    Status = BPLib_AS_Decrement(SourceEid, BPLIB_AS_NUM_COUNTERS);

    /* Assert that BPLib_AS_Decrement() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_NODE_CNTR, Status);

    /* === Unknown source counter test === */

    /* Set values to test against */
    SourceEid = 2;

    /* Run the function under test */
    Status = BPLib_AS_Decrement(SourceEid, BPLIB_AS_NUM_COUNTERS);

    /* Assert that BPLib_AS_Decrement() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_SRC_CNTR, Status);
}

void Test_BPLib_AS_ResetAllCounters_Nominal(void)
{
    BPLib_Status_t Status;
    int16_t SourceEid = 5;

    /* Set the values to test against */
    BPLib_AS_NodeCountersPayload.BundleAgentRejectedDirectiveCount = 15;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRejectedCustody = 17;

    /* Run the function under test */
    Status = BPLib_AS_ResetAllCounters();

    /* Verify that modify counters were reset and that the reset all counters function was successful */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
    UtAssert_EQ(uint32_t, 0, BPLib_AS_NodeCountersPayload.BundleAgentRejectedDirectiveCount);
    UtAssert_EQ(uint32_t, 0, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRejectedCustody);
}

void TestBplibAs_Register(void)
{
    ADD_TEST(Test_BPLib_AS_Init_Nominal);
    ADD_TEST(Test_BPLib_AS_Init_Error);
    ADD_TEST(Test_BPLib_AS_Get_Nominal);
    ADD_TEST(Test_BPLib_AS_Get_Error);
    ADD_TEST(Test_BPLib_AS_Set_Nominal);
    ADD_TEST(Test_BPLib_AS_Set_Error);
    ADD_TEST(Test_BPLib_AS_Increment_Nominal);
    ADD_TEST(Test_BPLib_AS_Increment_Error);
    ADD_TEST(Test_BPLib_AS_Decrement_Nominal);
    ADD_TEST(Test_BPLib_AS_Decrement_Error);
    ADD_TEST(Test_BPLib_AS_ResetAllCounters_Nominal);
}
