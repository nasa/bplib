/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
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
    TestValue += 5;

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DISCARDED]);
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
    TestValue += 7;

    // /* Verify that counter is the expected value */
    // UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.MibArray[MibIndex].SourceCounters[BUNDLE_COUNT_FORWARDED]);
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
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENTED]);

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
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]);

    /* Demonstrate that the correct error event was issued */
    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_SET_CTR_ERR_EID,
                                "Could not decrement node counter %d to %d, RC = %d");
}

void Test_BPLib_AS_Decrement_UnkSrcCtr_Error(void)
{
    uint32_t TestValue;
    uint32_t MibEntry;

    /* Force BPLib_AS_Decrement to see the given EID as valid and for a source counter */
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

    /* Demonstrate that the correct error event was issued */
    // BPLib_AS_Test_Verify_Event(0, BPLIB_AS_SET_CTR_ERR_EID,
    //                             "Could not decrement source counter %d to %d, RC = %d");
}

void Test_BPLib_AS_ResetCounter_NodeCtr_Nominal(void)
{
    int16_t  SourceEid;
    uint32_t TestValue;
    BPLib_Status_t Status;

    SourceEid = BPLIB_AS_NODE_CNTR_INDICATOR;
    TestValue = 6;

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED] = TestValue;

    /* Run function under test */
    Status = BPLib_AS_ResetCounter(SourceEid, BUNDLE_COUNT_CUSTODY_TRANSFERRED);

    /* Verify the return status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, 0, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]);
}

void Test_BPLib_AS_ResetCounter_SrcCtr_Nominal(void)
{
    int16_t  SourceEid;
    uint32_t TestValue;
    BPLib_Status_t Status;

    SourceEid = 2;
    TestValue = 6;

    /* Set values to test against */
    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED] = TestValue;

    /* Run function under test */
    Status = BPLib_AS_ResetCounter(SourceEid, BUNDLE_COUNT_CUSTODY_TRANSFERRED);

    /* Verify the return status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, 0, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]);
}

void Test_BPLib_AS_ResetCounter_InvalidIndex_Error(void)
{
    int16_t  SourceEid;
    uint32_t TestValue;
    BPLib_Status_t Status;

    /* Set values to test against */
    SourceEid = BPLIB_MAX_NUM_MIB_SETS + 1;
    TestValue = 1;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED] = TestValue;

    /* Run the function under test */
    Status = BPLib_AS_ResetCounter(SourceEid, BUNDLE_COUNT_DELETED);

    /* Verify the return status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_AS_INVALID_MIB_INDEX);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]);
}

void Test_BPLib_AS_ResetCounter_UnkNodeCtr_Error(void)
{
    int16_t  SourceEid;
    uint32_t TestValue;
    BPLib_Status_t Status;

    /* Set values to test against */
    SourceEid = BPLIB_AS_NODE_CNTR_INDICATOR;
    TestValue = 13;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED] = TestValue;

    /* Run the function under test */
    Status = BPLib_AS_ResetCounter(SourceEid, BPLIB_AS_NUM_NODE_CNTRS);

    /* Verify the return status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_AS_UNKNOWN_NODE_CNTR);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]);
}

void Test_BPLib_AS_ResetCounter_UnkSrcCtr_Error(void)
{
    int16_t  SourceEid;
    uint32_t TestValue;
    BPLib_Status_t Status;

    /* Set values to test against */
    SourceEid = 5;
    TestValue = 48;

    BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_RECEIVED_ADMIN_RECORD] = TestValue;

    /* Run the function under test */
    Status = BPLib_AS_ResetCounter(SourceEid, BPLIB_AS_NUM_SOURCE_CNTRS);

    /* Verify the return status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_AS_UNKNOWN_SRC_CNTR);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_RECEIVED_ADMIN_RECORD]);
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

    /* Run the function under test */
    Status = BPLib_AS_ResetSourceCounters(SourceEid);

    /* Check that BPLib_AS_ResetSourceCounters() ran successfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_MIB_INDEX, Status);
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

void Test_BPLib_AS_SendNodeMibReportsHk_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_Instance_t Inst;

    Status = BPLib_AS_SendNodeMibReportsHk(&Inst);

    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
}

void Test_BPLib_AS_SendNodeMibReportsHk_Null(void)
{
    BPLib_Status_t Status;

    Status = BPLib_AS_SendNodeMibReportsHk(NULL);

    UtAssert_EQ(BPLib_Status_t, BPLIB_NULL_PTR_ERROR, Status);
}

void Test_BPLib_AS_AddMibArrayKey_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_EID_Pattern_t EID_Patterns[BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET];
    uint64_t MaxVal;
    uint64_t MinVal;

    MaxVal = 100;
    MinVal = 90;
    memset((void*) EID_Patterns, 0, sizeof(BPLib_EID_Pattern_t) * BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET);

    /* Verify that the function gets past the valid pattern check */
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_PatternIsValid), true);

    /* Create MIB key array pattern */
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].Scheme       = BPLIB_EID_SCHEME_DTN;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxAllocator = MaxVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinAllocator = MinVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxNode      = MaxVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinNode      = MinVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxService   = MaxVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinService   = MinVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].ActiveKeys                  = 1;

    /* Set input pattern's scheme and IPN SSP format to values not used in present keys */
    EID_Patterns[0].Scheme       = BPLIB_EID_SCHEME_IPN;
    EID_Patterns[0].IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_THREE_DIGIT;

    /* ============================================ */
    /* ============================================ */

    /* Input ranges are less than the present key ranges */
    EID_Patterns[0].MaxAllocator = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinAllocator - 1;
    EID_Patterns[0].MinAllocator = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinAllocator - 5;
    EID_Patterns[0].MaxNode      = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinNode - 6;
    EID_Patterns[0].MinNode      = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinNode - 10;
    EID_Patterns[0].MaxService   = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinService - 11;
    EID_Patterns[0].MinService   = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinService - 20;

    /* Run function under test */
    Status = BPLib_AS_AddMibArrayKey(EID_Patterns);

    /* Verify status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* Show that the input keys were copied into the MIB array */
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[1].Scheme,       EID_Patterns[0].Scheme);
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[1].IpnSspFormat, EID_Patterns[0].IpnSspFormat);
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[1].MaxAllocator, EID_Patterns[0].MaxAllocator);
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[1].MinAllocator, EID_Patterns[0].MinAllocator);
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[1].MaxNode,      EID_Patterns[0].MaxNode);
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[1].MinNode,      EID_Patterns[0].MinNode);
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[1].MaxService,   EID_Patterns[0].MaxService);
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[1].MinService,   EID_Patterns[0].MinService);

    /* Verify that the number of keys in use is the correct value */
    UtAssert_UINT8_EQ(2, BPLib_AS_SourceCountersPayload.MibArray[0].ActiveKeys);

    /* ============================================ */
    /* ============================================ */

    /* Input ranges are greater than the present key ranges */
    EID_Patterns[0].MaxAllocator = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxAllocator + 5;
    EID_Patterns[0].MinAllocator = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxAllocator + 1;
    EID_Patterns[0].MaxNode      = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxNode + 10;
    EID_Patterns[0].MinNode      = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxNode + 6;
    EID_Patterns[0].MaxService   = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxService + 20;
    EID_Patterns[0].MinService   = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxService + 11;

    /* Run function under test */
    Status = BPLib_AS_AddMibArrayKey(EID_Patterns);

    /* Verify status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* Show that the input keys were copied into the MIB array */
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[2].Scheme,       EID_Patterns[0].Scheme);
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[2].IpnSspFormat, EID_Patterns[0].IpnSspFormat);
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[2].MaxAllocator, EID_Patterns[0].MaxAllocator);
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[2].MinAllocator, EID_Patterns[0].MinAllocator);
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[2].MaxNode,      EID_Patterns[0].MaxNode);
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[2].MinNode,      EID_Patterns[0].MinNode);
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[2].MaxService,   EID_Patterns[0].MaxService);
    UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[2].MinService,   EID_Patterns[0].MinService);

    /* Verify that the number of keys in use is the correct value */
    UtAssert_UINT8_EQ(3, BPLib_AS_SourceCountersPayload.MibArray[0].ActiveKeys);
}

void Test_BPLib_AS_AddMibArrayKey_AllocatorOverlap_Error(void)
{
    BPLib_Status_t Status;
    BPLib_EID_Pattern_t EID_Patterns[BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET];
    uint64_t MaxVal;
    uint64_t MinVal;

    MaxVal = 10;
    MinVal = 1;
    memset((void*) EID_Patterns, 0, sizeof(BPLib_EID_Pattern_t) * BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET);

    /* Verify that the function gets past the valid pattern check */
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_PatternIsValid), true);

    /* Create MIB key array pattern */
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].Scheme       = BPLIB_EID_SCHEME_DTN;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxAllocator = MaxVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinAllocator = MinVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxNode      = MaxVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinNode      = MinVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxService   = MaxVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinService   = MinVal;

    /* Set input pattern's scheme and IPN SSP format to values not used in present keys */
    EID_Patterns[0].Scheme       = BPLIB_EID_SCHEME_IPN;
    EID_Patterns[0].IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_THREE_DIGIT;

    /* Preset all other input pattern fields to guarantee nothing else overlaps */
    EID_Patterns[0].MaxNode    = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxNode + MaxVal;
    EID_Patterns[0].MinNode    = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinNode + MinVal;
    EID_Patterns[0].MaxService = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxService + MaxVal;
    EID_Patterns[0].MinService = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinService + MinVal;

    /* ============================================ */
    /* ============================================ */

    /* Create subset overlap */
    EID_Patterns[0].MaxAllocator = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxAllocator - 1;
    EID_Patterns[0].MinAllocator = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinAllocator + 1;

    /* Run function under test */
    Status = BPLib_AS_AddMibArrayKey(EID_Patterns);

    /* Verify status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_AS_MIB_KEYS_OVERLAP);

    /* Show that the input keys were not copied into the MIB array */
    UtAssert_EQ(uint64_t, BPLIB_EID_SCHEME_DTN,               BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].Scheme);
    UtAssert_EQ(uint64_t, BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].IpnSspFormat);
    UtAssert_EQ(uint64_t, MaxVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxAllocator);
    UtAssert_EQ(uint64_t, MinVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinAllocator);

    /* Verify that the number of keys in use is the correct value */
    UtAssert_UINT8_EQ(0, BPLib_AS_SourceCountersPayload.MibArray[0].ActiveKeys);

    /* Verify debug event was issued */
    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_ADD_MIB_ARRAY_KEY_DBG_EID,
                                "MIB array key overlap found between input #%d and MIB #%d, active key #%d");

    /* ============================================ */
    /* ============================================ */

    /* Create left-skewed overlap */
    EID_Patterns[0].MaxAllocator = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxAllocator - 1;
    EID_Patterns[0].MinAllocator = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinAllocator - 1;

    /* Run function under test */
    Status = BPLib_AS_AddMibArrayKey(EID_Patterns);

    /* Verify status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_AS_MIB_KEYS_OVERLAP);

    /* Show that the input keys were not copied into the MIB array */
    UtAssert_EQ(uint64_t, BPLIB_EID_SCHEME_DTN,               BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].Scheme);
    UtAssert_EQ(uint64_t, BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].IpnSspFormat);
    UtAssert_EQ(uint64_t, MaxVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxAllocator);
    UtAssert_EQ(uint64_t, MinVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinAllocator);

    /* Verify that the number of keys in use is the correct value */
    UtAssert_UINT8_EQ(0, BPLib_AS_SourceCountersPayload.MibArray[0].ActiveKeys);

    /* Verify debug event was issued */
    BPLib_AS_Test_Verify_Event(1, BPLIB_AS_ADD_MIB_ARRAY_KEY_DBG_EID,
                                "MIB array key overlap found between input #%d and MIB #%d, active key #%d");

    /* ============================================ */
    /* ============================================ */

    /* Create right-skewed overlap */
    EID_Patterns[0].MaxAllocator = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxAllocator + 1;
    EID_Patterns[0].MinAllocator = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinAllocator + 1;

    /* Run function under test */
    Status = BPLib_AS_AddMibArrayKey(EID_Patterns);

    /* Verify status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_AS_MIB_KEYS_OVERLAP);

    /* Show that the input keys were not copied into the MIB array */
    UtAssert_EQ(uint64_t, BPLIB_EID_SCHEME_DTN,               BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].Scheme);
    UtAssert_EQ(uint64_t, BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].IpnSspFormat);
    UtAssert_EQ(uint64_t, MaxVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxAllocator);
    UtAssert_EQ(uint64_t, MinVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinAllocator);

    /* Verify that the number of keys in use is the correct value */
    UtAssert_UINT8_EQ(0, BPLib_AS_SourceCountersPayload.MibArray[0].ActiveKeys);

    /* Verify debug event was issued */
    BPLib_AS_Test_Verify_Event(2, BPLIB_AS_ADD_MIB_ARRAY_KEY_DBG_EID,
                                "MIB array key overlap found between input #%d and MIB #%d, active key #%d");
}

void Test_BPLib_AS_AddMibArrayKey_NodeOverlap_Error(void)
{
    BPLib_Status_t Status;
    BPLib_EID_Pattern_t EID_Patterns[BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET];
    uint64_t MaxVal;
    uint64_t MinVal;

    MaxVal = 20;
    MinVal = 11;
    memset((void*) EID_Patterns, 0, sizeof(BPLib_EID_Pattern_t) * BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET);

    /* Verify that the function gets past the valid pattern check */
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_PatternIsValid), true);

    /* Create MIB key array pattern */
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].Scheme       = BPLIB_EID_SCHEME_DTN;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxAllocator = MaxVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinAllocator = MinVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxNode      = MaxVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinNode      = MinVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxService   = MaxVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinService   = MinVal;

    /* Set input pattern's scheme and IPN SSP format to values not used in present keys */
    EID_Patterns[0].Scheme       = BPLIB_EID_SCHEME_IPN;
    EID_Patterns[0].IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_THREE_DIGIT;

    /* Preset all other input pattern fields to guarantee nothing else overlaps */
    EID_Patterns[0].MaxAllocator = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxAllocator + MaxVal;
    EID_Patterns[0].MinAllocator = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinAllocator + MinVal;
    EID_Patterns[0].MaxService   = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxService + MaxVal;
    EID_Patterns[0].MinService   = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinService + MinVal;

    /* ============================================ */
    /* ============================================ */

    /* Create subset overlap */
    EID_Patterns[0].MaxNode = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxNode - 1;
    EID_Patterns[0].MinNode = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinNode + 1;

    /* Run function under test */
    Status = BPLib_AS_AddMibArrayKey(EID_Patterns);

    /* Verify status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_AS_MIB_KEYS_OVERLAP);

    /* Show that the input keys were not copied into the MIB array */
    UtAssert_EQ(uint64_t, BPLIB_EID_SCHEME_DTN,               BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].Scheme);
    UtAssert_EQ(uint64_t, BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].IpnSspFormat);
    UtAssert_EQ(uint64_t, MaxVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxNode);
    UtAssert_EQ(uint64_t, MinVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinNode);

    /* Verify that the number of keys in use is the correct value */
    UtAssert_UINT8_EQ(0, BPLib_AS_SourceCountersPayload.MibArray[0].ActiveKeys);
    
    /* Verify debug event was issued */
    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_ADD_MIB_ARRAY_KEY_DBG_EID,
                                "MIB array key overlap found between input #%d and MIB #%d, active key #%d");

    /* ============================================ */
    /* ============================================ */

    /* Create left-skewed overlap */
    EID_Patterns[0].MaxNode = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxNode - 1;
    EID_Patterns[0].MinNode = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinNode - 1;

    /* Run function under test */
    Status = BPLib_AS_AddMibArrayKey(EID_Patterns);

    /* Verify status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_AS_MIB_KEYS_OVERLAP);

    /* Show that the input keys were not copied into the MIB array */
    UtAssert_EQ(uint64_t, BPLIB_EID_SCHEME_DTN,               BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].Scheme);
    UtAssert_EQ(uint64_t, BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].IpnSspFormat);
    UtAssert_EQ(uint64_t, MaxVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxNode);
    UtAssert_EQ(uint64_t, MinVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinNode);

    /* Verify that the number of keys in use is the correct value */
    UtAssert_UINT8_EQ(0, BPLib_AS_SourceCountersPayload.MibArray[0].ActiveKeys);

    /* Verify debug event was issued */
    BPLib_AS_Test_Verify_Event(1, BPLIB_AS_ADD_MIB_ARRAY_KEY_DBG_EID,
                                "MIB array key overlap found between input #%d and MIB #%d, active key #%d");

    /* ============================================ */
    /* ============================================ */

    /* Create right-skewed overlap */
    EID_Patterns[0].MaxNode = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxNode + 1;
    EID_Patterns[0].MinNode = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinNode + 1;

    /* Run function under test */
    Status = BPLib_AS_AddMibArrayKey(EID_Patterns);

    /* Verify status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_AS_MIB_KEYS_OVERLAP);

    /* Show that the input keys were not copied into the MIB array */
    UtAssert_EQ(uint64_t, BPLIB_EID_SCHEME_DTN,               BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].Scheme);
    UtAssert_EQ(uint64_t, BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].IpnSspFormat);
    UtAssert_EQ(uint64_t, MaxVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxNode);
    UtAssert_EQ(uint64_t, MinVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinNode);

    /* Verify that the number of keys in use is the correct value */
    UtAssert_UINT8_EQ(0, BPLib_AS_SourceCountersPayload.MibArray[0].ActiveKeys);

    /* Verify debug event was issued */
    BPLib_AS_Test_Verify_Event(2, BPLIB_AS_ADD_MIB_ARRAY_KEY_DBG_EID,
                                "MIB array key overlap found between input #%d and MIB #%d, active key #%d");
}

void Test_BPLib_AS_AddMibArrayKey_ServiceOverlap_Error(void)
{
    BPLib_Status_t Status;
    BPLib_EID_Pattern_t EID_Patterns[BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET];
    uint64_t MaxVal;
    uint64_t MinVal;

    MaxVal = 30;
    MinVal = 21;
    memset((void*) EID_Patterns, 0, sizeof(BPLib_EID_Pattern_t) * BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET);

    /* Verify that the function gets past the valid pattern check */
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_PatternIsValid), true);

    /* Create MIB key array pattern */
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].Scheme       = BPLIB_EID_SCHEME_DTN;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxAllocator = MaxVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinAllocator = MinVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxNode      = MaxVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinNode      = MinVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxService   = MaxVal;
    BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinService   = MinVal;

    /* Set input pattern's scheme and IPN SSP format to values not used in present keys */
    EID_Patterns[0].Scheme       = BPLIB_EID_SCHEME_IPN;
    EID_Patterns[0].IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_THREE_DIGIT;

    /* Preset all other input pattern fields to guarantee nothing else overlaps */
    EID_Patterns[0].MaxAllocator = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxAllocator + MaxVal;
    EID_Patterns[0].MinAllocator = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinAllocator + MinVal;
    EID_Patterns[0].MaxNode      = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxNode + MaxVal;
    EID_Patterns[0].MinNode      = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinNode + MinVal;

    /* ============================================ */
    /* ============================================ */

    /* Create subset overlap */
    EID_Patterns[0].MaxService = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxService - 1;
    EID_Patterns[0].MinService = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinService + 1;

    /* Run function under test */
    Status = BPLib_AS_AddMibArrayKey(EID_Patterns);

    /* Verify status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_AS_MIB_KEYS_OVERLAP);

    /* Show that the input keys were not copied into the MIB array */
    UtAssert_EQ(uint64_t, BPLIB_EID_SCHEME_DTN,               BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].Scheme);
    UtAssert_EQ(uint64_t, BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].IpnSspFormat);
    UtAssert_EQ(uint64_t, MaxVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxService);
    UtAssert_EQ(uint64_t, MinVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinService);

    /* Verify that the number of keys in use is the correct value */
    UtAssert_UINT8_EQ(0, BPLib_AS_SourceCountersPayload.MibArray[0].ActiveKeys);

    /* Verify debug event was issued */
    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_ADD_MIB_ARRAY_KEY_DBG_EID,
                                "MIB array key overlap found between input #%d and MIB #%d, active key #%d");

    /* ============================================ */
    /* ============================================ */

    /* Create left-skewed overlap */
    EID_Patterns[0].MaxService = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxService - 1;
    EID_Patterns[0].MinService = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinService - 1;

    /* Run function under test */
    Status = BPLib_AS_AddMibArrayKey(EID_Patterns);

    /* Verify status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_AS_MIB_KEYS_OVERLAP);

    /* Show that the input keys were not copied into the MIB array */
    UtAssert_EQ(uint64_t, BPLIB_EID_SCHEME_DTN,               BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].Scheme);
    UtAssert_EQ(uint64_t, BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].IpnSspFormat);
    UtAssert_EQ(uint64_t, MaxVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxService);
    UtAssert_EQ(uint64_t, MinVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinService);

    /* Verify that the number of keys in use is the correct value */
    UtAssert_UINT8_EQ(0, BPLib_AS_SourceCountersPayload.MibArray[0].ActiveKeys);

    /* Verify debug event was issued */
    BPLib_AS_Test_Verify_Event(1, BPLIB_AS_ADD_MIB_ARRAY_KEY_DBG_EID,
                                "MIB array key overlap found between input #%d and MIB #%d, active key #%d");

    /* ============================================ */
    /* ============================================ */

    /* Create right-skewed overlap */
    EID_Patterns[0].MaxService = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxService + 1;
    EID_Patterns[0].MinService = BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinService + 1;

    /* Run function under test */
    Status = BPLib_AS_AddMibArrayKey(EID_Patterns);

    /* Verify status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_AS_MIB_KEYS_OVERLAP);

    /* Show that the input keys were not copied into the MIB array */
    UtAssert_EQ(uint64_t, BPLIB_EID_SCHEME_DTN,               BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].Scheme);
    UtAssert_EQ(uint64_t, BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].IpnSspFormat);
    UtAssert_EQ(uint64_t, MaxVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MaxService);
    UtAssert_EQ(uint64_t, MinVal,                             BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[0].MinService);

    /* Verify that the number of keys in use is the correct value */
    UtAssert_UINT8_EQ(0, BPLib_AS_SourceCountersPayload.MibArray[0].ActiveKeys);

    /* Verify debug event was issued */
    BPLib_AS_Test_Verify_Event(2, BPLIB_AS_ADD_MIB_ARRAY_KEY_DBG_EID,
                                "MIB array key overlap found between input #%d and MIB #%d, active key #%d");
}

void Test_BPLib_AS_AddMibArrayKey_InvalidEID_Error(void)
{
    BPLib_Status_t Status;
    uint8_t PatternIndex;
    BPLib_EID_Pattern_t EID_Patterns[BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET];

    memset((void*) EID_Patterns, 0, sizeof(BPLib_EID_Pattern_t) * BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET);

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_PatternIsValid), false);

    /* Create an input key to compare against */
    EID_Patterns[0].Scheme       = BPLIB_EID_SCHEME_IPN;
    EID_Patterns[0].IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_THREE_DIGIT;
    EID_Patterns[0].MaxAllocator = 10;
    EID_Patterns[0].MinAllocator = 1;
    EID_Patterns[0].MaxNode      = 20;
    EID_Patterns[0].MinNode      = 11;
    EID_Patterns[0].MaxService   = 30;
    EID_Patterns[0].MinService   = 21;

    /* Run function under test */
    Status = BPLib_AS_AddMibArrayKey(EID_Patterns);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_INVALID_EID_PATTERN);

    /* Show that the keys were not copied into the MIB array */
    for (PatternIndex = 0; PatternIndex < BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET; PatternIndex++)
    {
        UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].Scheme,       0);
        UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].IpnSspFormat, 0);
        UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MaxAllocator, 0);
        UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MinAllocator, 0);
        UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MaxNode,      0);
        UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MinNode,      0);
        UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MaxService,   0);
        UtAssert_EQ(uint64_t, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MinService,   0);
    }

    /* Verify that the number of keys in use is the correct value */
    UtAssert_UINT8_EQ(0, BPLib_AS_SourceCountersPayload.MibArray[0].ActiveKeys);

    /* Verify correct event was issued */
    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_ADD_MIB_ARRAY_KEY_DBG_EID,
                                "Input key #%d is an invalid EID pattern");
}

void Test_BPLib_AS_AddMibArrayKey_FullArray_Error(void)
{
    BPLib_Status_t Status;
    uint8_t PatternIndex;
    BPLib_EID_Pattern_t EID_Patterns[BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET];

    memset((void*) EID_Patterns, 0, sizeof(BPLib_EID_Pattern_t) * BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET);

    /* Verify that the function gets past the valid pattern check */
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_PatternIsValid), true);

    /* Indicate that all keys are in use */
    for (PatternIndex = 0; PatternIndex < BPLIB_MAX_NUM_MIB_SETS; PatternIndex++)
    {
        BPLib_AS_SourceCountersPayload.MibArray[PatternIndex].ActiveKeys = BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET;
    }

    /* Have something that you attempt to write to the array */
    EID_Patterns[0].Scheme       = BPLIB_EID_SCHEME_DTN;
    EID_Patterns[0].IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    EID_Patterns[0].MaxAllocator = 1000;
    EID_Patterns[0].MinAllocator = 500;
    EID_Patterns[0].MaxNode      = 1000;
    EID_Patterns[0].MinNode      = 500;
    EID_Patterns[0].MaxService   = 1000;
    EID_Patterns[0].MinService   = 500;

    /* Run function under test */
    Status = BPLib_AS_AddMibArrayKey(EID_Patterns);

    /* Verify status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_AS_MIB_KEY_ARRAY_FULL);

    /* Show that the keys were not copied into the MIB array */
    for (PatternIndex = 0; PatternIndex < BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET; PatternIndex++)
    {
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].Scheme);
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].IpnSspFormat);
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MaxAllocator);
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MinAllocator);
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MaxNode);
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MinNode);
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MaxService);
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MinService);
    }

    /* Verify the expected event */
    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_ADD_MIB_ARRAY_KEY_DBG_EID,
                                "EID key array is full");
}

void Test_BPLib_AS_AddMibArrayKey_NoKeysGiven_Error(void)
{
    BPLib_Status_t Status;
    uint8_t PatternIndex;
    BPLib_EID_Pattern_t EID_Patterns[BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET];

    memset((void*) EID_Patterns, 0, sizeof(BPLib_EID_Pattern_t) * BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET);

    /* Verify that the function gets past the valid pattern check */
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_PatternIsValid), true);

    /* Run function under test */
    Status = BPLib_AS_AddMibArrayKey(EID_Patterns);

    /* Verify status */
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_AS_NO_KEYS_GIVEN);

    /* Show that the keys were not copied into the MIB array */
    for (PatternIndex = 0; PatternIndex < BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET; PatternIndex++)
    {
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].Scheme);
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].IpnSspFormat);
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MaxAllocator);
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MinAllocator);
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MaxNode);
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MinNode);
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MaxService);
        UtAssert_EQ(uint64_t, 0, BPLib_AS_SourceCountersPayload.MibArray[0].EidPatterns[PatternIndex].MinService);
    }
}

void Test_BPLib_AS_GetCounter_Nominal(void)
{
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsMatch), true);

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELIVERED] = 0xdead;

    UtAssert_EQ(uint32_t, BPLib_AS_GetCounter(&BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELIVERED),
                          BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELIVERED]);
}

void Test_BPLib_AS_GetCounter_Null(void)
{
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELIVERED] = 0xdead;

    UtAssert_EQ(uint32_t, BPLib_AS_GetCounter(NULL, BUNDLE_COUNT_DELIVERED), 0);
}

void Test_BPLib_AS_GetCounter_InvalidEid(void)
{
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), false);

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELIVERED] = 0xdead;

    UtAssert_EQ(uint32_t, BPLib_AS_GetCounter(&BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELIVERED), 0);
}

void Test_BPLib_AS_GetCounter_InvalidCounter(void)
{
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);

    UtAssert_EQ(uint32_t, BPLib_AS_GetCounter(&BPLIB_EID_INSTANCE, BPLIB_AS_NUM_NODE_CNTRS), 0);
}

void Test_BPLib_AS_GetCounter_SrcCounter(void)
{
    BPLib_EID_t Eid;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsMatch), false);

    UtAssert_EQ(uint32_t, BPLib_AS_GetCounter(&Eid, BUNDLE_COUNT_DELIVERED), 0);
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

    ADD_TEST(Test_BPLib_AS_ResetCounter_NodeCtr_Nominal);
    ADD_TEST(Test_BPLib_AS_ResetCounter_SrcCtr_Nominal);
    ADD_TEST(Test_BPLib_AS_ResetCounter_InvalidIndex_Error);
    ADD_TEST(Test_BPLib_AS_ResetCounter_UnkNodeCtr_Error);
    ADD_TEST(Test_BPLib_AS_ResetCounter_UnkSrcCtr_Error);

    ADD_TEST(Test_BPLib_AS_ResetSourceCounters_Nominal);
    ADD_TEST(Test_BPLib_AS_ResetSourceCounters_Error);

    ADD_TEST(Test_BPLib_AS_ResetBundleCounters_Nominal);

    ADD_TEST(Test_BPLib_AS_ResetErrorCounters_Nominal);
    ADD_TEST(Test_BPLib_AS_ResetErrorCounters_Error);

    ADD_TEST(Test_BPLib_AS_ResetAllCounters_Nominal);

    ADD_TEST(Test_BPLib_AS_SendNodeMibCountersHk_Nominal);

    ADD_TEST(Test_BPLib_AS_SendSourceMibCountersHk_Nominal);

    ADD_TEST(Test_BPLib_AS_SendNodeMibReportsHk_Nominal);
    ADD_TEST(Test_BPLib_AS_SendNodeMibReportsHk_Null);
    
    ADD_TEST(Test_BPLib_AS_AddMibArrayKey_Nominal);
    ADD_TEST(Test_BPLib_AS_AddMibArrayKey_AllocatorOverlap_Error);
    ADD_TEST(Test_BPLib_AS_AddMibArrayKey_NodeOverlap_Error);
    ADD_TEST(Test_BPLib_AS_AddMibArrayKey_ServiceOverlap_Error);
    ADD_TEST(Test_BPLib_AS_AddMibArrayKey_InvalidEID_Error);
    ADD_TEST(Test_BPLib_AS_AddMibArrayKey_FullArray_Error);
    ADD_TEST(Test_BPLib_AS_AddMibArrayKey_NoKeysGiven_Error);

    ADD_TEST(Test_BPLib_AS_GetCounter_Nominal);
    ADD_TEST(Test_BPLib_AS_GetCounter_Null);
    ADD_TEST(Test_BPLib_AS_GetCounter_InvalidCounter);
    ADD_TEST(Test_BPLib_AS_GetCounter_InvalidEid);
    ADD_TEST(Test_BPLib_AS_GetCounter_SrcCounter);
}