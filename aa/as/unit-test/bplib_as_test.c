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

    BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected = TestValue;
    // BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected = TestValue;

    /* Run function under test */
    BPLib_AS_Increment(SourceEid, BUNDLE_COUNT_CUSTODY_REJECTED, 1);
    TestValue += 1;

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected);
    // UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected);
}

void Test_BPLib_AS_Increment_Error(void)
{
    int16_t SourceEid;

    /* === Invalid EID test === */

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleCountCustodyRequest = 0;
    SourceEid = BPLIB_MAX_NUM_SOURCE_EID;

    /* Run the function under test */
    BPLib_AS_Increment(SourceEid, BUNDLE_COUNT_CUSTODY_REQUEST, 1);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, 0, BPLib_AS_NodeCountersPayload.BundleCountCustodyRequest);

    /* Verify expected event was issued */
    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_INCREMENT_ERR_EID,
                                "Could not get counter %d with source EID %d to increment by %d, RC = %d");

    /* === Unknown counter test === */

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleCountCustodyRequest = 0;
    SourceEid = 2;

    /* Run the function under test */
    BPLib_AS_Increment(SourceEid, BPLIB_AS_NUM_CNTRS, 5);

    /* Verify expected event was issued */
    BPLib_AS_Test_Verify_Event(1, BPLIB_AS_INCREMENT_ERR_EID,
                                "Could not get counter %d with source EID %d to increment by %d, RC = %d");
}

void Test_BPLib_AS_Decrement_Nominal(void)
{
    int16_t  SourceEid;
    uint32_t TestValue;

    SourceEid = 2;
    TestValue = 6;

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleCountCustodyTransferred = TestValue;
    // BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyTransferred = TestValue;

    /* Run function under test */
    BPLib_AS_Decrement(SourceEid, BUNDLE_COUNT_CUSTODY_TRANSFERRED, 4);
    TestValue -= 4;

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.BundleCountCustodyTransferred);
    // UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyTransferred);
}

void Test_BPLib_AS_Decrement_Error(void)
{
    int16_t SourceEid;

    /* === Invalid EID test === */

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleCountDeleted = 1;
    SourceEid = BPLIB_MAX_NUM_SOURCE_EID;

    /* Run the function under test */
    BPLib_AS_Decrement(SourceEid, BUNDLE_COUNT_DELETED, 10);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, 1, BPLib_AS_NodeCountersPayload.BundleCountDeleted);

    /* Verify expected event was issued */
    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_DECREMENT_ERR_EID,
                                "Could not get counter %d with source EID %d to decrement by %d, RC = %d");

    /* === Unknown counter test === */

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleCountDeleted = 1;
    SourceEid = 2;

    /* Run the function under test */
    BPLib_AS_Decrement(SourceEid, BPLIB_AS_NUM_CNTRS, 5);

    /* Verify expected event was issued */
    BPLib_AS_Test_Verify_Event(1, BPLIB_AS_DECREMENT_ERR_EID,
                                "Could not get counter %d with source EID %d to decrement by %d, RC = %d");
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

    /* Verify expected event was issued */
    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_RESET_SRC_INVAL_EID_ERR_EID,
                                "Could not reset source counters due to an invalid source EID (%d)");
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

    /* Verify expected event was issued */
    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_RESET_BNDL_INVAL_EID_ERR_EID,
                                "Could not reset bundle counters due to invalid source EID (%d)");
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

    /* Verify expected event was issued */
    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_RESET_ERR_INVAL_EID_ERR_EID,
                                "Could not reset error counters due to invalid source EID (%d)");
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

void TestBplibAs_Register(void)
{
    ADD_TEST(Test_BPLib_AS_Init_Nominal);
    ADD_TEST(Test_BPLib_AS_Init_Error);
    ADD_TEST(Test_BPLib_AS_Increment_Nominal);
    ADD_TEST(Test_BPLib_AS_Increment_Error);
    ADD_TEST(Test_BPLib_AS_Decrement_Nominal);
    ADD_TEST(Test_BPLib_AS_Decrement_Error);
    ADD_TEST(Test_BPLib_AS_ResetSourceCounters_Nominal);
    ADD_TEST(Test_BPLib_AS_ResetSourceCounters_Error);
    ADD_TEST(Test_BPLib_AS_ResetBundleCounters_Nominal);
    ADD_TEST(Test_BPLib_AS_ResetBundleCounters_Error);
    ADD_TEST(Test_BPLib_AS_ResetErrorCounters_Nominal);
    ADD_TEST(Test_BPLib_AS_ResetErrorCounters_Error);
    ADD_TEST(Test_BPLib_AS_ResetAllCounters_Nominal);
}
