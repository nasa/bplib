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

void Test_BPLib_AS_SetCounter_Nominal(void)
{
    int16_t  SourceEid;
    uint32_t TestValue;

    SourceEid = 2;
    TestValue = 6;

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED] = TestValue - 2;
    // BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED] = TestValue - 2;

    /* Run function under test */
    BPLib_AS_SetCounter(SourceEid, BUNDLE_COUNT_CUSTODY_TRANSFERRED, TestValue);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]);
    // UtAssert_EQ(uint32_t, TestValue, BPLib_AS_SourceCountersPayload.MibArray[SourceEid].SourceCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]);
}

void Test_BPLib_AS_SetCounter_Error(void)
{
    int16_t  SourceEid;
    uint32_t TestValue;

    /* === Invalid EID test === */

    /* Set values to test against */
    SourceEid = BPLIB_MAX_NUM_SOURCE_EID;
    TestValue = 1;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED] = TestValue;

    /* Run the function under test */
    BPLib_AS_SetCounter(SourceEid, BUNDLE_COUNT_DELETED, TestValue + 10);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]);

    /* === Unknown node counter test === */

    /* Set values to test against */
    SourceEid = 2;
    TestValue = 3;

    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED] = TestValue;

    /* Run the function under test */
    BPLib_AS_SetCounter(SourceEid, BPLIB_AS_NUM_NODE_CNTRS, TestValue + 2);

    /* Verify that counter is the expected value */
    UtAssert_EQ(uint32_t, TestValue, BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]);
}

void Test_BPLib_AS_InitMutex_Nominal(void)
{
    BPLib_Status_t Status;

    UT_SetDefaultReturnValue(UT_KEY(OS_MutSemCreate), OS_SUCCESS);

    Status = BPLib_AS_InitMutex();

    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
}

void Test_BPLib_AS_InitMutex_Error(void)
{
    BPLib_Status_t Status;

    UT_SetDefaultReturnValue(UT_KEY(OS_MutSemCreate), OS_INVALID_POINTER);

    Status = BPLib_AS_InitMutex();

    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INIT_MUTEX_ERR, Status);
}

void Test_BPLib_AS_LockUnlockCounters_Nominal(void)
{
    /* TODO: Add tasks and verify that the counters can't be modified */

    UT_SetDefaultReturnValue(UT_KEY(OS_MutSemTake), OS_SUCCESS);

    /* === Lock counters test === */
    BPLib_AS_LockCounters();

    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);

    /* === Unlock counters test === */
    BPLib_AS_UnlockCounters();

    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);
}

void Test_BPLib_AS_LockCounters_Error(void)
{
    UT_SetDefaultReturnValue(UT_KEY(OS_MutSemTake), OS_ERR_INVALID_ID);

    BPLib_AS_LockCounters();

    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_TAKE_MUTEX_ERR_EID, "Failed to take from the counter mutex, RC = %d");
}

void Test_BPLib_AS_UnlockCounters_Error(void)
{
    UT_SetDefaultReturnValue(UT_KEY(OS_MutSemGive), OS_ERR_INVALID_ID);

    BPLib_AS_UnlockCounters();

    BPLib_AS_Test_Verify_Event(0, BPLIB_AS_GIVE_MUTEX_ERR_EID, "Failed to give to the counter mutex, RC = %d");
}

void Test_BPLib_AS_GetMibArrayIndex_Nominal(void)
{
    BPLib_Status_t Status;
    uint8_t Index;
    uint8_t ExpectedIndex;
    BPLib_EID_t EID_Test;
    BPLib_EID_Pattern_t EID_Pattern;

    memset((void*) &EID_Pattern, 0, sizeof(BPLib_EID_Pattern_t));
    Index         = 20;
    ExpectedIndex = 2;

    EID_Test.Scheme       = BPLIB_EID_SCHEME_IPN;
    EID_Test.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    EID_Test.Allocator    = 1;
    EID_Test.Node         = 2;
    EID_Test.Service      = 3;

    EID_Pattern.Scheme       = BPLIB_EID_SCHEME_IPN;
    EID_Pattern.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    EID_Pattern.MaxAllocator = 1;
    EID_Pattern.MinAllocator = 0;
    EID_Pattern.MaxNode      = 2;
    EID_Pattern.MinNode      = 0;
    EID_Pattern.MaxService   = 3;
    EID_Pattern.MinService   = 0;

    BPLib_AS_SourceCountersPayload.MibArray[ExpectedIndex].SourceEIDs[1] = EID_Pattern;

    Status = BPLib_AS_GetMibArrayIndex(EID_Test, &Index);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_UINT8_EQ(Index, ExpectedIndex);
}

void Test_BPLib_AS_GetMibArrayIndex_NoMatch_Error(void)
{
    BPLib_Status_t Status;
    uint8_t Index;
    BPLib_EID_t EID_Test;
    BPLib_EID_Pattern_t EID_Pattern;

    memset((void*) &EID_Pattern, 0, sizeof(BPLib_EID_Pattern_t));
    Index = 20;

    EID_Test.Scheme       = BPLIB_EID_SCHEME_IPN;
    EID_Test.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    EID_Test.Allocator    = 4;
    EID_Test.Node         = 5;
    EID_Test.Service      = 6;

    EID_Pattern.Scheme       = BPLIB_EID_SCHEME_IPN;
    EID_Pattern.IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    EID_Pattern.MaxAllocator = 4;
    EID_Pattern.MinAllocator = 0;
    EID_Pattern.MaxNode      = 5;
    EID_Pattern.MinNode      = 0;
    EID_Pattern.MaxService   = 1;
    EID_Pattern.MinService   = 0;

    BPLib_AS_SourceCountersPayload.MibArray[1].SourceEIDs[1] = EID_Pattern;

    Status = BPLib_AS_GetMibArrayIndex(EID_Test, &Index);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_AS_UNKNOWN_MIB_ARRAY_EID);
    UtAssert_UINT8_EQ(Index, 20);
}

void TestBplibAsInternal_Register(void)
{
    ADD_TEST(Test_BPLib_AS_EidIsValid_Nominal);
    ADD_TEST(Test_BPLib_AS_EidIsValid_Error);
    ADD_TEST(Test_BPLib_AS_SetCounter_Nominal);
    ADD_TEST(Test_BPLib_AS_SetCounter_Error);
    ADD_TEST(Test_BPLib_AS_InitMutex_Nominal);
    ADD_TEST(Test_BPLib_AS_InitMutex_Error);
    ADD_TEST(Test_BPLib_AS_LockUnlockCounters_Nominal);
    ADD_TEST(Test_BPLib_AS_LockCounters_Error);
    ADD_TEST(Test_BPLib_AS_UnlockCounters_Error);
}