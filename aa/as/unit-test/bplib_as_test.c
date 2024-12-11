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

void Test_BPLib_AS_Increment_Nominal(void)
{
    int16_t  SourceEid;
    uint32_t TestValue;

    /* Set values to test against */
    SourceEid = 2;
    TestValue = 15;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REJECTED] = TestValue;
    // BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_REJECTED] = TestValue;

    /* Run function under test */
    BPLib_AS_Increment(SourceEid, BUNDLE_COUNT_CUSTODY_REJECTED, 1);
    TestValue += 1;

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REJECTED]);
    // UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].BundleCountCustodyRejected);
}

void Test_BPLib_AS_Increment_Error(void)
{
    int16_t  SourceEid;
    uint32_t TestValue;

    /* === Invalid EID test === */

    /* Set values to test against */
    SourceEid = BPLIB_MAX_NUM_SOURCE_EID;
    TestValue = 10;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REQUEST] = TestValue;

    /* Run the function under test */
    BPLib_AS_Increment(SourceEid, BUNDLE_COUNT_CUSTODY_REQUEST, 1);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REQUEST]);

    /* === Unknown node counter test === */

    /* Set values to test against */
    SourceEid = 2;
    TestValue = 6;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REQUEST] = TestValue;

    /* Run the function under test */
    BPLib_AS_Increment(SourceEid, BPLIB_AS_NUM_NODE_CNTRS, 5);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REQUEST]);

    /* === Unknown source counter test === */
    // TODO
}

void Test_BPLib_AS_Decrement_Nominal(void)
{
    int16_t  SourceEid;
    uint32_t TestValue;

    SourceEid = 2;
    TestValue = 6;

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED] = TestValue;
    // BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED] = TestValue;

    /* Run function under test */
    BPLib_AS_Decrement(SourceEid, BUNDLE_COUNT_CUSTODY_TRANSFERRED, 4);
    TestValue -= 4;

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]);
    // UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]);
}

void Test_BPLib_AS_Decrement_Error(void)
{
    int16_t  SourceEid;
    uint32_t TestValue;

    /* === Invalid EID test === */

    /* Set values to test against */
    SourceEid = BPLIB_MAX_NUM_SOURCE_EID;
    TestValue = 1;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED] = TestValue;

    /* Run the function under test */
    BPLib_AS_Decrement(SourceEid, BUNDLE_COUNT_DELETED, 10);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]);

    /* === Unknown node counter test === */

    /* Set values to test against */
    SourceEid = 2;
    TestValue = 3;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED] = TestValue;

    /* Run the function under test */
    BPLib_AS_Decrement(SourceEid, BPLIB_AS_NUM_NODE_CNTRS, 5);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]);

    /* === Unknown source counter test === */
    // TODO
}

void Test_BPLib_AS_ResetCounter_Nominal(void)
{
    int16_t  SourceEid;
    uint32_t TestValue;

    SourceEid = 2;
    TestValue = 6;

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED] = TestValue;
    // BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED] = TestValue;

    /* Run function under test */
    BPLib_AS_ResetCounter(SourceEid, BUNDLE_COUNT_CUSTODY_TRANSFERRED);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, 0, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]);
    // UtAssert_EQ(uint32_t, 0, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]);
}

void Test_BPLib_AS_ResetCounter_Error(void)
{
    int16_t  SourceEid;
    uint32_t TestValue;

    /* === Invalid EID test === */

    /* Set values to test against */
    SourceEid = BPLIB_MAX_NUM_SOURCE_EID;
    TestValue = 1;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED] = TestValue;

    /* Run the function under test */
    BPLib_AS_ResetCounter(SourceEid, BUNDLE_COUNT_DELETED);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]);

    /* === Unknown node counter test === */

    /* Set values to test against */
    SourceEid = 2;
    TestValue = 3;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED] = TestValue;

    /* Run the function under test */
    BPLib_AS_ResetCounter(SourceEid, BPLIB_AS_NUM_NODE_CNTRS);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]);

    /* === Unknown source counter test === */
    // TODO
}

void Test_BPLib_AS_ResetSourceCounters_Nominal(void)
{
    BPLib_Status_t Status;
    int32_t SourceEid;
    uint32_t TestValue;

    SourceEid = 1;
    TestValue = 42;

    /* Set all source counters to a non-zero value */
    Test_BPLib_AS_SetSourceCounterValues(SourceEid, TestValue);

    /* Run the function under test */
    Status = BPLib_AS_ResetSourceCounters(SourceEid);

    /* Check that BPLib_AS_ResetSourceCounters() ran successfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);

    /* Check that source counters are set to 0 */
    Test_BPLib_AS_SourceCountersValueTest(SourceEid, 0);
}

void Test_BPLib_AS_ResetSourceCounters_Error(void)
{
    BPLib_Status_t Status;
    int32_t SourceEid;

    SourceEid = -10;

    /* ================ */
    /* Invalid EID test */
    /* ================ */

    /* Run the function under test */
    Status = BPLib_AS_ResetSourceCounters(SourceEid);

    /* Check that BPLib_AS_ResetSourceCounters() ran successfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_EID, Status);
}

void Test_BPLib_AS_ResetBundleCounters_Nominal(void)
{
    BPLib_Status_t Status;
    int32_t SourceEid;
    uint32_t TestValue;

    SourceEid = 2;
    TestValue = 144;

    /* Set the values to be tested */
    Test_BPLib_AS_SetBundleCounterValues(SourceEid, TestValue);

    /* Run the function under test */
    Status = BPLib_AS_ResetBundleCounters(SourceEid);

    /* Assert that BPLib_AS_ResetBundleCounters() ran successfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);

    /* Assert that each modified bundle counter was reset */
    Test_BPLib_AS_BundleCountersValueTest(SourceEid, 0);
}

void Test_BPLib_AS_ResetBundleCounters_Error(void)
{
    BPLib_Status_t Status;
    int32_t SourceEid;

    SourceEid = -15;

    /* Run the function under test */
    Status = BPLib_AS_ResetBundleCounters(SourceEid);

    /* Assert that BPLib_AS_ResetBundleCounters() failed due to an invalid source EID */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_EID, Status);
}

void Test_BPLib_AS_ResetErrorCounters_Nominal(void)
{
    BPLib_Status_t Status;
    int32_t SourceEid;
    uint32_t TestValue;

    SourceEid = 3;
    TestValue = 5;

    /* Set the values to be tested */
    Test_BPLib_AS_SetErrorCounterValues(SourceEid, TestValue);

    /* Run the function under test */
    Status = BPLib_AS_ResetErrorCounters(SourceEid);

    /* Assert that BPLib_AS_ResetErrorCounters() ran successfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);

    /* Assert that each modified error counter was reset */
    Test_BPLib_AS_ErrorCountersValueTest(SourceEid, 0);
}

void Test_BPLib_AS_ResetErrorCounters_Error(void)
{
    BPLib_Status_t Status;
    int32_t SourceEid;

    SourceEid = -15;

    /* Run the function under test */
    Status = BPLib_AS_ResetErrorCounters(SourceEid);

    /* Assert that BPLib_AS_ResetErrorCounters() failed due to an invalid source EID */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_EID, Status);
}

void Test_BPLib_AS_ResetAllCounters_Nominal(void)
{
    int32_t SourceEids[3];
    uint32_t TestValue;

    /* Modify counter values for the first source EID */
    SourceEids[0] = 6;
    TestValue = 1000;

    /* Set node counters to a non-zero value */
    Test_BPLib_AS_SetNodeCounterValues(TestValue);

    /* Set all counters to a non-zero value */
    Test_BPLib_AS_SetSourceCounterValues(SourceEids[0], TestValue);
    Test_BPLib_AS_SetBundleCounterValues(SourceEids[0], TestValue);
    Test_BPLib_AS_SetErrorCounterValues(SourceEids[0], TestValue);

    /* Modify counter values for a second source EID */
    SourceEids[1] = 1;
    TestValue = 500;

    /* Set all counters to a non-zero value */
    Test_BPLib_AS_SetSourceCounterValues(SourceEids[1], TestValue);
    Test_BPLib_AS_SetBundleCounterValues(SourceEids[1], TestValue);
    Test_BPLib_AS_SetErrorCounterValues(SourceEids[1], TestValue);

    /* Modify counter values for a third source EID */
    SourceEids[2] = 2;
    TestValue = 250;

    /* Set all counters to a non-zero value */
    Test_BPLib_AS_SetSourceCounterValues(SourceEids[2], TestValue);
    Test_BPLib_AS_SetBundleCounterValues(SourceEids[2], TestValue);
    Test_BPLib_AS_SetErrorCounterValues(SourceEids[2], TestValue);

    /* Run the function under test */
    BPLib_AS_ResetAllCounters();

    /* Check that all counters are set to 0 */
    Test_BPLib_AS_NodeCountersValueTest(0);

    Test_BPLib_AS_SourceCountersValueTest(SourceEids[0], 0);
    Test_BPLib_AS_BundleCountersValueTest(SourceEids[0], 0);
    Test_BPLib_AS_ErrorCountersValueTest(SourceEids[0], 0);

    Test_BPLib_AS_SourceCountersValueTest(SourceEids[1], 0);
    Test_BPLib_AS_BundleCountersValueTest(SourceEids[1], 0);
    Test_BPLib_AS_ErrorCountersValueTest(SourceEids[1], 0);

    Test_BPLib_AS_SourceCountersValueTest(SourceEids[2], 0);
    Test_BPLib_AS_BundleCountersValueTest(SourceEids[2], 0);
    Test_BPLib_AS_ErrorCountersValueTest(SourceEids[2], 0);
}

void Test_BPLib_AS_SendNodeMibCountersHk_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_AS_SendNodeMibCountersHk();

    UtAssert_EQ(BPLib_Status_t, 0, Status);
}

void Test_BPLib_AS_SendSourceMibCountersHk_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_AS_SendSourceMibCountersHk();

    UtAssert_EQ(BPLib_Status_t, 0, Status);
}

void Test_BPLib_AS_SendChannelContactStatHk_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_AS_SendChannelContactStatHk();

    UtAssert_EQ(BPLib_Status_t, 0, Status);
}

void Test_BPLib_AS_GetSetAppState_Nominal(void)
{
    int8_t ChanId;
    BPLib_AS_ApplicationState_t State;

    ChanId = 1;
    State  = BPLIB_AS_APP_STATE_ADDED;

    BPLib_AS_ChannelContactStatsPayload.ChannelStatus[ChanId].State = State;

    State = BPLIB_AS_APP_STATE_STARTED;
    BPLib_AS_SetAppState(ChanId, State);

    UtAssert_EQ(BPLib_AS_ApplicationState_t, State, BPLib_AS_ChannelContactStatsPayload.ChannelStatus[ChanId].State);

    /* Verify that the state has changed without modifying the payload's value before calling this function */
    State = BPLib_AS_GetAppState(ChanId);

    UtAssert_EQ(BPLib_AS_ApplicationState_t, BPLIB_AS_APP_STATE_STARTED, State);
}

void TestBplibAs_Register(void)
{
    ADD_TEST(Test_BPLib_AS_Init_Nominal);
    ADD_TEST(Test_BPLib_AS_Init_Error);
    ADD_TEST(Test_BPLib_AS_Increment_Nominal);
    ADD_TEST(Test_BPLib_AS_Increment_Error);
    ADD_TEST(Test_BPLib_AS_Decrement_Nominal);
    ADD_TEST(Test_BPLib_AS_Decrement_Error);
    ADD_TEST(Test_BPLib_AS_ResetCounter_Nominal);
    ADD_TEST(Test_BPLib_AS_ResetCounter_Error);
    ADD_TEST(Test_BPLib_AS_ResetSourceCounters_Nominal);
    ADD_TEST(Test_BPLib_AS_ResetSourceCounters_Error);
    ADD_TEST(Test_BPLib_AS_ResetBundleCounters_Nominal);
    ADD_TEST(Test_BPLib_AS_ResetBundleCounters_Error);
    ADD_TEST(Test_BPLib_AS_ResetErrorCounters_Nominal);
    ADD_TEST(Test_BPLib_AS_ResetErrorCounters_Error);
    ADD_TEST(Test_BPLib_AS_ResetAllCounters_Nominal);
    ADD_TEST(Test_BPLib_AS_SendNodeMibCountersHk_Nominal);
    ADD_TEST(Test_BPLib_AS_SendSourceMibCountersHk_Nominal);
    ADD_TEST(Test_BPLib_AS_SendChannelContactStatHk_Nominal);
    ADD_TEST(Test_BPLib_AS_GetSetAppState_Nominal);
}
