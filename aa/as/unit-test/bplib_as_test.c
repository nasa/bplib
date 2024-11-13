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

void Test_BPLib_AS_Get_Nominal(void)
{
    BPLib_Status_t Status;
    uint32_t TestValue;
    int32_t SourceEid;

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected = 5;
    // BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected = 12;
    TestValue = 0;
    SourceEid = 1;

    /* Run function under test */
    Status = BPLib_AS_Get(SourceEid, BUNDLE_CNT_CUSTODY_REJ, &TestValue);

    /* Assert that TestValue was updated and BPLib_AS_Get() was run successfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
    UtAssert_EQ(uint32_t, TestValue, 5);

    /* TODO: Event checking */
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

    // TODO: Event checking

    /* === Unknown node counter test === */

    /* Set values to test against */
    TestValue = 56;
    SourceEid = 1;

    /* Run the function under test */
    Status = BPLib_AS_Get(SourceEid, BPLIB_AS_NUM_CNTRS, &TestValue);

    /* Assert that TestValue wasn't updated and BPLib_AS_Get() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_NODE_CNTR, Status);
    UtAssert_EQ(uint32_t, TestValue, 56);

    // TODO: Event checking

    /* === Unknown source counter test ===

    // Set values to test against
    TestValue = 20;
    SourceEid = 2;

    // Run the function under test
    Status = BPLib_AS_Get(SourceEid, BPLIB_AS_NUM_CNTRS, &TestValue);

    // Assert that TestValue wasn't updated and BPLib_AS_Get() was run unsuccessfully
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_SRC_CNTR, Status);
    UtAssert_EQ(uint32_t, TestValue, 20);

    // TODO: Event checking
    */
}

void Test_BPLib_AS_Set_Nominal(void)
{
    BPLib_Status_t Status;
    uint32_t TestValue;
    int32_t SourceEid;

    /* === Single counter test === */

    /* Set values to test against */
    TestValue = 10;
    SourceEid = 2;
    BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived = 0;
    // BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodySignalReceived = 0

    /* Run function under test */
    Status = BPLib_AS_Set(SourceEid, TestValue, 1, BUNDLE_CNT_CS_RECV);

    /* Assert that the counter was updated and BPLib_AS_Set() was run successfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived);
    // UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodySignalReceived);

    /* === Multiple counter test === */

    /* Set values to test against */
    TestValue = 20;
    SourceEid = 2;

    BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected = 0;
    BPLib_AS_NodeCountersPayload.BundleCountDeletedUnsupportedBlock = 0;
    BPLib_AS_NodeCountersPayload.BundleCountForwarded = 0;

    // BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected = 0;
    // BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnsupportedBlock = 0;
    // BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwarded = 0;


    /* Run function under test */
    Status = BPLib_AS_Set(SourceEid, TestValue, 3, BUNDLE_CNT_CUSTODY_REJ,
                                                   BUNDLE_CNT_DEL_UNSUPPORT_BLK,
                                                   BUNDLE_CNT_FORW);

    /* Assert that the counter was modified and BPLib_AS_Set() was run successfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);

    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected);
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedUnsupportedBlock);
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.BundleCountForwarded);

    // UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected);
    // UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnsupportedBlock);
    // UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwarded);

    // TODO: Event checking
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

    // TODO: Event checking

    /* === Unknown node counter test === */

    /* Set values to test against */
    SourceEid = 1; /* Indicates that a node counter is desired */

    /* Run the function under test */
    Status = BPLib_AS_Set(SourceEid, BPLIB_AS_NUM_CNTRS, TestValue);

    /* Assert that BPLib_AS_Set() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_NODE_CNTR, Status);

    // TODO: Event checking

    /* === Unknown source counter test ===

    // Set values to test against
    SourceEid = 2;

    // Run the function under test
    Status = BPLib_AS_Set(SourceEid, BPLIB_AS_NUM_CNTRS, TestValue);

    // Assert that BPLib_AS_Set() was run unsuccessfully
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_SRC_CNTR, Status);

    // TODO: Event checking
    */
}

void Test_BPLib_AS_Increment_Nominal(void)
{
    BPLib_Status_t Status;
    int16_t SourceEid;

    /* Set values to test against */
    SourceEid = 2;
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted = 15;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted = 15;

    /* Run function under test */
    Status = BPLib_AS_Increment(SourceEid, BUNDLE_CNT_GEN_ACCPT);

    /* Verify that the node and source counters were incremented */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);

    UtAssert_EQ(uint32_t, 16, BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted);
    // UtAssert_EQ(uint32_t, 16, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted);
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

    /* === Unknown counter test === */

    /* Set values to test against */
    SourceEid = 2;

    /* Run the function under test */
    Status = BPLib_AS_Increment(SourceEid, BPLIB_AS_NUM_CNTRS);

    /* Assert that BPLib_AS_Increment() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_NODE_CNTR, Status);
}

void Test_BPLib_AS_Decrement_Nominal(void)
{
    BPLib_Status_t Status;
    int16_t SourceEid;

    SourceEid = 2;

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted = 15;
    // BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted = 15;

    /* Run function under test */
    Status = BPLib_AS_Decrement(SourceEid, BUNDLE_CNT_GEN_ACCPT);

    /* Verify that the node and source counters were decremented */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);

    UtAssert_EQ(uint32_t, 14, BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted);
    // UtAssert_EQ(uint32_t, 14, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted);

    // TODO: Event checking
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

    /* === Unknown counter test === */

    /* Set values to test against */
    SourceEid = 2;

    /* Run the function under test */
    Status = BPLib_AS_Decrement(SourceEid, BPLIB_AS_NUM_CNTRS);

    /* Assert that BPLib_AS_Decrement() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_NODE_CNTR, Status);
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

    /* TODO: Event checking */
}

void Test_BPLib_AS_ResetSourceCounters_Error(void)
{
    BPLib_Status_t Status;
    int32_t SourceEid;
    uint32_t TestValue;
    uint32_t PrevVal;

    SourceEid = -10;
    TestValue = 24;

    /* Set all source counters to a non-zero value */
    Test_BPLib_AS_SetSourceCounterValues(SourceEid, TestValue);

    /* ================ */
    /* Invalid EID test */
    /* ================ */

    /* Run the function under test */
    Status = BPLib_AS_ResetSourceCounters(SourceEid);

    /* Check that BPLib_AS_ResetSourceCounters() ran successfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_RESET_SRC_ERR, Status);

    /* Check that source counters haven't been reset */
    Test_BPLib_AS_SourceCountersValueTest(SourceEid, TestValue);

    /* TODO: Event checking */

    /* ================================ */
    /* Invalid BPLib_AS_Set() operation */
    /* ================================ */

    /* 
    ** Assign an invalid counter indicator to an entry in the array that holds all
    ** the resettable source counters to force BPLib_AS_Set() to output an error
    */

    PrevVal = ResettableSourceCounters[0];
    ResettableSourceCounters[0] = -15;

    /* Make the source EID a valid value */
    SourceEid = 1;

    /* Run the function under test */
    Status = BPLib_AS_ResetSourceCounters(SourceEid);

    /* Assert that BPLib_AS_ResetSourceCounters() failed */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_RESET_SRC_ERR, Status);

    /* Verify that the source counters were unchanged */
    Test_BPLib_AS_SourceCountersValueTest(SourceEid, TestValue);

    /* TODO: Event checking */

    /* Set the array that holds all the source counters back to its original state for future tests */
    ResettableSourceCounters[0] = PrevVal;
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

    /* TODO: Event checking */
}

void Test_BPLib_AS_ResetBundleCounters_Error(void)
{
    BPLib_Status_t Status;
    int32_t SourceEid;
    uint32_t TestValue;
    uint32_t PrevVal;

    SourceEid = -15;
    TestValue = 55;

    /* Set bundle counters to a non-zero value */
    Test_BPLib_AS_SetBundleCounterValues(SourceEid, TestValue);

    /* Run the function under test */
    Status = BPLib_AS_ResetBundleCounters(SourceEid);

    /* Assert that BPLib_AS_ResetBundleCounters() failed due to an invalid source EID */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_EID, Status);

    /* Verify that bundle counters were reset to 0 */
    Test_BPLib_AS_BundleCountersValueTest(SourceEid, TestValue);

    /* TODO: Event checking */

    /* ================================ */
    /* Invalid BPLib_AS_Set() operation */
    /* ================================ */

    /* 
    ** Assign an invalid counter indicator to an entry in the array that holds all
    ** the bundle counters to force BPLib_AS_Set() to output an error
    */

    PrevVal = BundleCounters[0];
    BundleCounters[0] = -20;

    /* Set source EID to a valid value */
    SourceEid = 2;

    /* Run the function under test */
    Status = BPLib_AS_ResetBundleCounters(SourceEid);

    /* Assert that BPLib_AS_ResetBundleCounters() failed */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_RESET_BNDL_ERR, Status);

    /* Verify that the bundle counters were unchanged */
    Test_BPLib_AS_BundleCountersValueTest(SourceEid, TestValue);

    /* TODO: Evnet checking */

    /* Set the array that holds all the bundle counters back to its original state for future tests */
    BundleCounters[0] = PrevVal;
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

    /* TODO: Event checking */
}

void Test_BPLib_AS_ResetErrorCounters_Error(void)
{
    BPLib_Status_t Status;
    int32_t SourceEid;
    uint32_t TestValue;
    uint32_t PrevVal;

    /* ================================ */
    /* Invalid BPLib_AS_Set() operation */
    /* ================================ */

    /* 
    ** Assign an invalid counter indicator to an entry in the array that holds all
    ** the error counters to force BPLib_AS_Set() to output an error
    */

    PrevVal = ErrorCounters[0];
    ErrorCounters[0] = -20;

    /* Set source EID to a valid value */
    SourceEid = 3;
    TestValue = 80;

    /* Run the function under test */
    Status = BPLib_AS_ResetErrorCounters(SourceEid);

    /* Assert that BPLib_AS_ResetErrorCounters() failed */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_RESET_ERR_ERR, Status);

    /* Verify that the error counters were unchanged */
    Test_BPLib_AS_ErrorCountersValueTest(SourceEid, TestValue);

    /* TODO: Evnet checking */

    /* Set the array that holds all the error counters back to its original state for future tests */
    ErrorCounters[0] = PrevVal;
}

void Test_BPLib_AS_ResetAllCounters_Nominal(void)
{
    BPLib_Status_t Status;
    int32_t SourceEids[3];
    uint32_t TestValue;

    /* Modify counter values for the first source EID */
    SourceEids[0] = 6;
    TestValue = 1000;

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
    Status = BPLib_AS_ResetAllCounters();

    /* Check that BPLib_AS_ResetAllCounters() ran successfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);

    /* Check that all counters are set to 0 */
    Test_BPLib_AS_SourceCountersValueTest(SourceEids[0], 0);
    Test_BPLib_AS_BundleCountersValueTest(SourceEids[0], 0);
    Test_BPLib_AS_ErrorCountersValueTest(SourceEids[0], 0);

    Test_BPLib_AS_SourceCountersValueTest(SourceEids[1], 0);
    Test_BPLib_AS_BundleCountersValueTest(SourceEids[1], 0);
    Test_BPLib_AS_ErrorCountersValueTest(SourceEids[1], 0);

    Test_BPLib_AS_SourceCountersValueTest(SourceEids[2], 0);
    Test_BPLib_AS_BundleCountersValueTest(SourceEids[2], 0);
    Test_BPLib_AS_ErrorCountersValueTest(SourceEids[2], 0);

    /* TODO: Event checking */
}

void Test_BPLib_AS_ResetAllCounters_Error(void)
{
    BPLib_Status_t Status;
    int32_t SourceEids[3];
    uint32_t TestValues[3];
    uint32_t PrevVal;

    /* Modify counter values for the first source EID */
    SourceEids[0] = 1;
    TestValues[0] = 10;

    /* Set all counters to a non-zero value */
    Test_BPLib_AS_SetSourceCounterValues(SourceEids[0], TestValues[0]);
    Test_BPLib_AS_SetBundleCounterValues(SourceEids[0], TestValues[0]);
    Test_BPLib_AS_SetErrorCounterValues(SourceEids[0], TestValues[0]);

    /* Modify counter values for a second source EID */
    SourceEids[1] = 5;
    TestValues[1] = 5;

    /* Set all counters to a non-zero value */
    Test_BPLib_AS_SetSourceCounterValues(SourceEids[1], TestValues[1]);
    Test_BPLib_AS_SetBundleCounterValues(SourceEids[1], TestValues[1]);
    Test_BPLib_AS_SetErrorCounterValues(SourceEids[1], TestValues[1]);

    /* Modify counter values for a third source EID */
    SourceEids[2] = 0;
    TestValues[2] = 25;

    /* Set all counters to a non-zero value */
    Test_BPLib_AS_SetSourceCounterValues(SourceEids[2], TestValues[2]);
    Test_BPLib_AS_SetBundleCounterValues(SourceEids[2], TestValues[2]);
    Test_BPLib_AS_SetErrorCounterValues(SourceEids[2], TestValues[2]);

    /* ================================ */
    /* Invalid BPLib_AS_Set() operation */
    /* ================================ */

    /* 
    ** Assign an invalid counter indicator to an entry in the array that holds all
    ** the resettable node counters to force BPLib_AS_Set() to output an error
    */

    PrevVal = ResettableNodeCounters[0];
    ResettableNodeCounters[0] = -20;

    /* Run the function under test */
    Status = BPLib_AS_ResetAllCounters();

    /* Assert that BPLib_AS_ResetAllCounters() failed */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_RESET_ALL_ERR, Status);

    /* Verify that the all counters were unchanged */
    Test_BPLib_AS_SourceCountersValueTest(SourceEids[0], TestValues[0]);
    Test_BPLib_AS_BundleCountersValueTest(SourceEids[0], TestValues[0]);
    Test_BPLib_AS_ErrorCountersValueTest(SourceEids[0], TestValues[0]);

    Test_BPLib_AS_SourceCountersValueTest(SourceEids[1], TestValues[1]);
    Test_BPLib_AS_BundleCountersValueTest(SourceEids[1], TestValues[1]);
    Test_BPLib_AS_ErrorCountersValueTest(SourceEids[1], TestValues[1]);

    Test_BPLib_AS_SourceCountersValueTest(SourceEids[2], TestValues[2]);
    Test_BPLib_AS_BundleCountersValueTest(SourceEids[2], TestValues[2]);
    Test_BPLib_AS_ErrorCountersValueTest(SourceEids[2], TestValues[2]);

    /* TODO: Evnet checking */

    /* Set the array that holds all the error counters back to its original state for future tests */
    ResettableNodeCounters[0] = PrevVal;
}

void TestBplibAs_Register(void)
{
    ADD_TEST(Test_BPLib_AS_Init_Nominal);
    ADD_TEST(Test_BPLib_AS_Init_Error);
    ADD_TEST(Test_BPLib_AS_EidIsValid_Nominal);
    ADD_TEST(Test_BPLib_AS_EidIsValid_Error);
    ADD_TEST(Test_BPLib_AS_Get_Nominal);
    ADD_TEST(Test_BPLib_AS_Get_Error);
    ADD_TEST(Test_BPLib_AS_Set_Nominal);
    ADD_TEST(Test_BPLib_AS_Set_Error);
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
    ADD_TEST(Test_BPLib_AS_ResetAllCounters_Error);
}
