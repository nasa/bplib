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

void Test_BPLib_AS_Increment_NodeCounter_Nominal(void)
{
    uint32_t TestValue;

    /* Force BPLib_AS_Increment to see the given EID as valid and for a node counter */
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsMatch), true);

    /* Set values to test against */
    TestValue = 50;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DISCARDED] = TestValue;

    /* Run function under test */
    BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DISCARDED, 5);
    TestValue -= 5;

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DISCARDED]);

    /* Show that counters were locked and unlocked */
    UtAssert_STUB_COUNT(BPLib_AS_LockCounters, 1);
    UtAssert_STUB_COUNT(BPLib_AS_UnlockCounters, 1);
}

void Test_BPLib_AS_Increment_SourceCounter_Nominal(void)
{
    int8_t  MibIndex;
    uint32_t TestValue;

    /* Force BPLib_AS_Increment to see the given EID as valid and for a source counter */
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsMatch), false);

    /* Set values to test against */
    MibIndex = 3;
    TestValue = 17;
    BPLib_AS_SourceCountersPayload.MibArray[MibIndex].SourceCounters[BUNDLE_COUNT_FORWARDED] = TestValue;

    /* Run function under test */
    BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_FORWARDED, 7);
    TestValue -= 7;

    // /* Verify that counter is the expected value */
    // UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.MibArray[MibIndex].SourceCounters[BUNDLE_COUNT_FORWARDED]);

    /* Show that counters were locked and unlocked */
    UtAssert_STUB_COUNT(BPLib_AS_LockCounters, 1);
    UtAssert_STUB_COUNT(BPLib_AS_UnlockCounters, 1);
}

void Test_BPLib_AS_Increment_InvalidEID_Error(void)
{
    uint32_t TestValue;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), false);

    /* Set values to test against */
    TestValue = 45;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FORWARDED_FAILED] = TestValue;

    /* Run the function under test */
    BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_FORWARDED_FAILED, 10);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FORWARDED_FAILED]);

    /* Show that counters were not locked or unlocked */
    UtAssert_STUB_COUNT(BPLib_AS_LockCounters, 0);
    UtAssert_STUB_COUNT(BPLib_AS_UnlockCounters, 0);

    /* Demonstrate that the correct error event was issued */
    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_SET_CTR_ERR_EID,
                                "Could not increment node counter %d to %d, RC = %d");
}

void Test_BPLib_AS_Increment_UnkNodeCtr_Error(void)
{
    uint32_t TestValue;

    /* Force BPLib_AS_Increment to see the given EID as valid and for a node counter */
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsMatch), true);

    /* Set values to test against */
    TestValue = 6;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENTED] = TestValue;

    /* Run the function under test */
    BPLib_AS_Increment(BPLIB_EID_INSTANCE, BPLIB_AS_NUM_NODE_CNTRS, 8);

    /* Verify that counter was set to the expected value */
    UtAssert_STUB_COUNT(BPLib_AS_SetCounter, 1);
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENTED]);

    /* Show that counters were locked and unlocked */
    UtAssert_STUB_COUNT(BPLib_AS_LockCounters, 1);
    UtAssert_STUB_COUNT(BPLib_AS_UnlockCounters, 1);

    /* Demonstrate that the correct error event was issued */
    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_SET_CTR_ERR_EID,
                                "Could not increment node counter %d to %d, RC = %d");
}

void Test_BPLib_AS_Increment_UnkSrcCtr_Error(void)
{
    uint32_t TestValue;
    uint32_t MibEntry;

    /* Force BPLib_AS_Increment to see the given EID as valid and for a source counter */
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsMatch), false);

    /* Set values to test against */
    TestValue = 66;
    MibEntry  = 10;

    BPLib_AS_SourceCountersPayload.MibArray[MibEntry].SourceCounters[BUNDLE_COUNT_FRAGMENT_ERROR] = TestValue;

    /* Run the function under test */
    BPLib_AS_Increment(BPLIB_EID_INSTANCE, BPLIB_AS_NUM_SOURCE_CNTRS, 6);

    /* Verify that counter was set to the expected value */
    // UtAssert_STUB_COUNT(BPLib_AS_SetCounter, 1);
    // UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.MibArray[MibEntry].SourceCounters[BUNDLE_COUNT_FRAGMENT_ERROR]);

    /* Show that counters were locked and unlocked */
    UtAssert_STUB_COUNT(BPLib_AS_LockCounters, 1);
    UtAssert_STUB_COUNT(BPLib_AS_UnlockCounters, 1);

    /* Demonstrate that the correct error event was issued */
    // BPLib_AS_Test_Verify_Event(0, BPLIB_AS_SET_CTR_ERR_EID,
    //                             "Could not increment source counter %d to %d, RC = %d");
}

void Test_BPLib_AS_Decrement_NodeCounter_Nominal(void)
{
    uint32_t TestValue;

    /* Force BPLib_AS_Decrement to see the given EID as valid and for a node counter */
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsMatch), true);

    /* Set values to test against */
    TestValue = 6;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED] = TestValue;

    /* Run function under test */
    BPLib_AS_Decrement(BPLIB_EID_INSTANCE, BUNDLE_COUNT_CUSTODY_TRANSFERRED, 4);
    TestValue -= 4;

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]);

    /* Show that counters were locked and unlocked */
    UtAssert_STUB_COUNT(BPLib_AS_LockCounters, 1);
    UtAssert_STUB_COUNT(BPLib_AS_UnlockCounters, 1);
}

void Test_BPLib_AS_Decrement_SourceCounter_Nominal(void)
{
    int8_t  MibIndex;
    uint32_t TestValue;

    /* Force BPLib_AS_Decrement to see the given EID as valid and for a source counter */
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsMatch), false);

    /* Set values to test against */
    MibIndex = 2;
    TestValue = 6;
    BPLib_AS_SourceCountersPayload.MibArray[MibIndex].SourceCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED] = TestValue;

    /* Run function under test */
    BPLib_AS_Decrement(BPLIB_EID_INSTANCE, BUNDLE_COUNT_CUSTODY_TRANSFERRED, 4);
    TestValue -= 4;

    // /* Verify that counter is the expected value */
    // UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.MibArray[MibIndex].SourceCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]);

    /* Show that counters were locked and unlocked */
    UtAssert_STUB_COUNT(BPLib_AS_LockCounters, 1);
    UtAssert_STUB_COUNT(BPLib_AS_UnlockCounters, 1);
}

void Test_BPLib_AS_Decrement_InvalidEID_Error(void)
{
    uint32_t TestValue;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), false);

    /* Set values to test against */
    TestValue = 1;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED] = TestValue;

    /* Run the function under test */
    BPLib_AS_Decrement(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED, 10);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]);

    /* Show that counters were not locked or unlocked */
    UtAssert_STUB_COUNT(BPLib_AS_LockCounters, 0);
    UtAssert_STUB_COUNT(BPLib_AS_UnlockCounters, 0);

    /* Demonstrate that the correct error event was issued */
    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_SET_CTR_ERR_EID,
                                "Could not decrement node counter %d to %d, RC = %d");
}

void Test_BPLib_AS_Decrement_UnkNodeCtr_Error(void)
{
    uint32_t TestValue;

    /* Force BPLib_AS_Decrement to see the given EID as valid and for a node counter */
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsMatch), true);

    /* Set values to test against */
    TestValue = 3;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED] = TestValue;

    /* Run the function under test */
    BPLib_AS_Decrement(BPLIB_EID_INSTANCE, BPLIB_AS_NUM_NODE_CNTRS, 5);

    /* Verify that counter was set to the expected value */
    UtAssert_STUB_COUNT(BPLib_AS_SetCounter, 1);
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]);

    /* Show that counters were locked and unlocked */
    UtAssert_STUB_COUNT(BPLib_AS_LockCounters, 1);
    UtAssert_STUB_COUNT(BPLib_AS_UnlockCounters, 1);

    /* Demonstrate that the correct error event was issued */
    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_SET_CTR_ERR_EID,
                                "Could not decrement node counter %d to %d, RC = %d");
}

void Test_BPLib_AS_Decrement_UnkSrcCtr_Error(void)
{
    uint32_t TestValue;
    uint32_t MibEntry;

    /* Force BPLib_AS_Decrement to see the given EID as valid and for a source counter */
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsMatch), false);

    /* Set values to test against */
    TestValue = 5;
    MibEntry  = 1;

    BPLib_AS_SourceCountersPayload.MibArray[MibEntry].SourceCounters[BUNDLE_COUNT_DELIVERED] = TestValue;

    /* Run the function under test */
    BPLib_AS_Decrement(BPLIB_EID_INSTANCE, BPLIB_AS_NUM_SOURCE_CNTRS, 5);

    /* Verify that counter was set to the expected value */
    // UtAssert_STUB_COUNT(BPLib_AS_SetCounter, 1);
    // UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.MibArray[MibEntry].SourceCounters[BUNDLE_COUNT_DELIVERED]);

    /* Show that counters were locked and unlocked */
    UtAssert_STUB_COUNT(BPLib_AS_LockCounters, 1);
    UtAssert_STUB_COUNT(BPLib_AS_UnlockCounters, 1);

    /* Demonstrate that the correct error event was issued */
    // BPLib_AS_Test_Verify_Event(0, BPLIB_AS_SET_CTR_ERR_EID,
    //                             "Could not decrement source counter %d to %d, RC = %d");
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
    UtAssert_EQ(BPLib_Status_t, BPLIB_INVALID_EID, Status);
}

void Test_BPLib_AS_ResetBundleCounters_Nominal(void)
{
    uint32_t TestValue;

    TestValue = 144;

    /* Set the values to be tested */
    Test_BPLib_AS_SetBundleCounterValues(TestValue);

    /* Run the function under test */
    BPLib_AS_ResetBundleCounters();

    /* Assert that each modified bundle counter was reset */
    Test_BPLib_AS_BundleCountersValueTest(0);
}

void Test_BPLib_AS_ResetErrorCounters_Nominal(void)
{
    BPLib_Status_t Status;
    uint8_t MibArrayIndex;
    uint32_t TestValue;

    MibArrayIndex = 3;
    TestValue     = 5;

    /* Set the values to be tested */
    Test_BPLib_AS_SetErrorCounterValues(MibArrayIndex, TestValue);

    /* Run the function under test */
    Status = BPLib_AS_ResetErrorCounters(MibArrayIndex);

    /* Assert that BPLib_AS_ResetErrorCounters() ran successfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);

    /* Assert that each modified error counter was reset */
    Test_BPLib_AS_ErrorCountersValueTest(MibArrayIndex, 0);
}

void Test_BPLib_AS_ResetErrorCounters_Error(void)
{
    BPLib_Status_t Status;
    uint8_t MibArrayIndex;

    MibArrayIndex = -15;

    /* Run the function under test */
    Status = BPLib_AS_ResetErrorCounters(MibArrayIndex);

    /* Assert that BPLib_AS_ResetErrorCounters() failed due to an invalid index into source MIB counters array */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_MIB_INDEX, Status);
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

    /* Set bundle counters to a non-zero value*/
    Test_BPLib_AS_SetBundleCounterValues(TestValue);

    /* Set all counters to a non-zero value */
    Test_BPLib_AS_SetSourceCounterValues(SourceEids[0], TestValue);
    Test_BPLib_AS_SetErrorCounterValues(SourceEids[0], TestValue);

    /* Modify counter values for a second source EID */
    SourceEids[1] = 1;
    TestValue = 500;

    /* Set all counters to a non-zero value */
    Test_BPLib_AS_SetSourceCounterValues(SourceEids[1], TestValue);
    Test_BPLib_AS_SetErrorCounterValues(SourceEids[1], TestValue);

    /* Modify counter values for a third source EID */
    SourceEids[2] = 2;
    TestValue = 250;

    /* Set all counters to a non-zero value */
    Test_BPLib_AS_SetSourceCounterValues(SourceEids[2], TestValue);
    Test_BPLib_AS_SetErrorCounterValues(SourceEids[2], TestValue);

    /* Run the function under test */
    BPLib_AS_ResetAllCounters();

    /* Check that all counters are set to 0 */
    Test_BPLib_AS_NodeCountersValueTest(0);

    Test_BPLib_AS_BundleCountersValueTest(0);

    Test_BPLib_AS_SourceCountersValueTest(SourceEids[0], 0);
    Test_BPLib_AS_ErrorCountersValueTest(SourceEids[0], 0);

    Test_BPLib_AS_SourceCountersValueTest(SourceEids[1], 0);
    Test_BPLib_AS_ErrorCountersValueTest(SourceEids[1], 0);

    Test_BPLib_AS_SourceCountersValueTest(SourceEids[2], 0);
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

void Test_BPLib_AS_AddMibArrayKey_Nominal(void)
{
    BPLib_Status_t Status;
    uint8_t PatternIndex;
    BPLib_EID_Pattern_t EID_Patterns[BPLIB_MAX_MIB_ARRAY_KEYS];

    for (PatternIndex = 0; PatternIndex < BPLIB_MAX_MIB_ARRAY_KEYS; PatternIndex++)
    {
        EID_Patterns[PatternIndex].Scheme       = (PatternIndex % 2) + 1;      /*   1,   2,   1,   2 */
        EID_Patterns[PatternIndex].IpnSspFormat = (PatternIndex % 2) + 2,      /*   2,   3,   2,   3 */
        EID_Patterns[PatternIndex].MaxAllocator = 10 * (PatternIndex + 1) - 1; /*   9,  19,  29,  39 */
        EID_Patterns[PatternIndex].MinAllocator = 10 * PatternIndex;           /*   0,  10,  20,  30 */
        EID_Patterns[PatternIndex].MaxNode      = 20 * (PatternIndex + 1) - 1; /*  19,  39,  59,  79 */
        EID_Patterns[PatternIndex].MinNode      = 20 * PatternIndex;           /*   0,  20,  40,  60 */
        EID_Patterns[PatternIndex].MaxService   = 30 * (PatternIndex + 1) - 1; /*  29,  59,  89, 119 */
        EID_Patterns[PatternIndex].MinService   = 30 * PatternIndex;           /*   0,  30,  60,  90 */
    }

    Status = BPLib_AS_AddMibArrayKey(EID_Patterns);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* Show that the keys were copied into the MIB array */
    for (PatternIndex = 0; PatternIndex < BPLIB_MAX_MIB_ARRAY_KEYS; PatternIndex++)
    {
        UtAssert_EQ(uint64_t, EID_Patterns[PatternIndex].Scheme,       BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].Scheme);
        UtAssert_EQ(uint64_t, EID_Patterns[PatternIndex].IpnSspFormat, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].IpnSspFormat);
        UtAssert_EQ(uint64_t, EID_Patterns[PatternIndex].MaxAllocator, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MaxAllocator);
        UtAssert_EQ(uint64_t, EID_Patterns[PatternIndex].MinAllocator, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MinAllocator);
        UtAssert_EQ(uint64_t, EID_Patterns[PatternIndex].MaxNode,      BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MaxNode);
        UtAssert_EQ(uint64_t, EID_Patterns[PatternIndex].MinNode,      BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MinNode);
        UtAssert_EQ(uint64_t, EID_Patterns[PatternIndex].MaxService,   BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MaxService);
        UtAssert_EQ(uint64_t, EID_Patterns[PatternIndex].MinService,   BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MinService);
    }

    /* Verify that the number of keys in use is the correct value */
    UtAssert_UINT8_EQ(BPLIB_MAX_MIB_ARRAY_KEYS, BPLib_AS_SourceCountersPayload.MibArray[0].ActiveKeys);
}

void TestBplibAs_Register(void)
{
    ADD_TEST(Test_BPLib_AS_Init_Nominal);
    ADD_TEST(Test_BPLib_AS_Init_Error);
    ADD_TEST(Test_BPLib_AS_Increment_NodeCounter_Nominal);
    ADD_TEST(Test_BPLib_AS_Increment_SourceCounter_Nominal);
    ADD_TEST(Test_BPLib_AS_Increment_InvalidEID_Error);
    ADD_TEST(Test_BPLib_AS_Increment_UnkNodeCtr_Error);
    ADD_TEST(Test_BPLib_AS_Increment_UnkSrcCtr_Error);
    ADD_TEST(Test_BPLib_AS_Decrement_NodeCounter_Nominal);
    ADD_TEST(Test_BPLib_AS_Decrement_SourceCounter_Nominal);
    ADD_TEST(Test_BPLib_AS_Decrement_InvalidEID_Error);
    ADD_TEST(Test_BPLib_AS_Decrement_UnkNodeCtr_Error);
    ADD_TEST(Test_BPLib_AS_Decrement_UnkSrcCtr_Error);
    ADD_TEST(Test_BPLib_AS_ResetCounter_Nominal);
    ADD_TEST(Test_BPLib_AS_ResetCounter_Error);
    ADD_TEST(Test_BPLib_AS_ResetSourceCounters_Nominal);
    ADD_TEST(Test_BPLib_AS_ResetSourceCounters_Error);
    ADD_TEST(Test_BPLib_AS_ResetBundleCounters_Nominal);
    ADD_TEST(Test_BPLib_AS_ResetErrorCounters_Nominal);
    ADD_TEST(Test_BPLib_AS_ResetErrorCounters_Error);
    ADD_TEST(Test_BPLib_AS_ResetAllCounters_Nominal);
    ADD_TEST(Test_BPLib_AS_SendNodeMibCountersHk_Nominal);
    ADD_TEST(Test_BPLib_AS_SendSourceMibCountersHk_Nominal);
    ADD_TEST(Test_BPLib_AS_AddMibArrayKey_Nominal);
}
