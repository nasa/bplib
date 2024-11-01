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

void Test_BPLib_AS_Get_Nominal()
{
    BPLib_Status_t Status;
    uint32_t* ValuePtr;

    /* === Node counter test === */

    /* Set values to test against */
    memset((void*) ValuePtr, 0, sizeof(ValuePtr));
    BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived = 5;

    /* Run function under test */
    Status = BPLib_AS_Get(1, BUNDLE_CNT_GEN_CUSTODY, (void*) ValuePtr);

    /* Assert that ValuePtr was updated and BPLib_AS_Get() was run successfully */
    UtAssert_EQ(uint32_t, *ValuePtr, 5);
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);

    /* === Source counter test === */
    /* TODO */
}

void Test_BPLib_AS_Get_Error()
{
    BPLib_Status_t Status;
    uint32_t* ValuePtr;

    memset((void*) ValuePtr, 0, sizeof(ValuePtr));

    /* === Node counter test === */

    /* Run the function under test */
    Status = BPLib_AS_Get(1, BPLIB_AS_NUM_COUNTERS, (void*) ValuePtr);

    /* Assert that ValuePtr wasn't updated and BPLib_AS_Get() was run unsuccessfully */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_CNTR, Status);
    UtAssert_EQ(uint32_t, *ValuePtr, 0);

    /* === Source counter test ===*/
    /* TODO */
}

void Test_BPLib_AS_Set_Nominal()
{
    BPLib_Status_t Status;
    uint32_t* ValueToSet;
    int32_t SourceEid;

    /* === Node counter test === */

    SourceEid = BPLIB_AS_NODE_EID; // This is mutating a node counter only

    /* Set values to test against*/
    memset((void*) ValueToSet, 24, sizeof(ValueToSet));
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDeleted = 5;

    /* Run function under test */
    Status = BPLib_AS_Set(SourceEid, BUNDLE_CNT_GEN_CRS_DEL, (void*) ValueToSet);

    /* Test that BPLib_AS_Set() was successful and set the appropriate value */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
    UtAssert_EQ(uint32_t, BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDeleted, 24);

    /* === Source counter test === */

    SourceEid = 2; // Mutate the source with EID 2

    /* Set values to test against */
    memset((void*) ValueToSet, 12, sizeof(ValueToSet));
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyReForwarded = 7;

    /* Run function under test */
    Status = BPLib_AS_Set(SourceEid, BUNDLE_CNT_CUSTODY_RE_FORW, (void*) ValueToSet);

    /* Test that BPLib_AS_Set() was successful and set the appropriate values */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
    UtAssert_EQ(uint32_t, BPLib_AS_NodeCountersPayload.BundleCountCustodyReForwarded, 7);
    UtAssert_EQ(uint32_t, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyReForwarded, 7);
}

void Test_BPLib_AS_Set_Error()
{
    BPLib_Status_t Status;
    uint32_t* ValueToSet;
    int32_t SourceEid;

    /* === Node counter test === */

    SourceEid = BPLIB_AS_NODE_EID; // This is intended to mutate a node counter

    /* Run function under test */
    Status = BPLib_AS_Set(SourceEid, BPLIB_AS_NUM_COUNTERS, (void*) ValueToSet);

    /* Test that BPLib_AS_Set() was unsuccessful */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_SOURCE_EID_MISSING, Status);
   
    /* === Source counter test === */

    SourceEid = 2; // Mutate the source with EID 2

    /* Run function under test */
    Status = BPLib_AS_Set(SourceEid, BPLIB_AS_NUM_COUNTERS, (void*) ValueToSet);

    /* Test that BPLib_AS_Set() was unsuccessful and the counters weren't modified */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_CNTR, Status);
}

void Test_BPLib_AS_Increment_Nominal()
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

void Test_BPLib_AS_Increment_Error()
{
    BPLib_Status_t Status;
    int16_t SourceEid;

    /* === Node counter test === */

    SourceEid = BPLIB_AS_NODE_EID;

    /* Run function under test */
    Status = BPLib_AS_Increment(SourceEid, BPLIB_AS_NUM_COUNTERS);
    
    /* Assert that the increment function failed */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_SOURCE_EID_MISSING, Status);

    /* === Source counter test === */

    SourceEid = 2;

    /* Run function under test */
    Status = BPLib_AS_Increment(SourceEid, BPLIB_AS_NUM_COUNTERS);

    /* Assert that the increment function failed */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_CNTR, Status);
}

void Test_BPLib_AS_Decrement_Nominal()
{
    BPLib_Status_t Status;
    int16_t SourceEid;

    /* === Node counter test === */

    SourceEid = BPLIB_AS_NODE_EID;

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleAgentRejectedDirectiveCount = 30;

    /* Run function under test*/
    Status = BPLib_AS_Decrement(SourceEid, BUNDLE_AGT_REJ_CNT);

    /* Verify that the value was decremented */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
    UtAssert_EQ(uint32_t, 29, BPLib_AS_NodeCountersPayload.BundleAgentRejectedDirectiveCount);

    /* === Source counter test === */

    SourceEid = 2;

    /* Set values to test against */
    BPLib_AS_NodeCountersPayload.BundleCountReceived = 42;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceived = 42;

    /* Run function under test */
    Status = BPLib_AS_Decrement(SourceEid, BUNDLE_CNT_RECV);

    /* Verify that the node and source counters were decremented */
    UtAssert_EQ(BPLib_Status_t, BPLIB_SUCCESS, Status);
    UtAssert_EQ(uint32_t, 41, BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted);
    UtAssert_EQ(uint32_t, 41, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted);
}

void Test_BPLib_AS_Decrement_Error()
{
    BPLib_Status_t Status;
    int16_t SourceEid;

    /* === Node counter test === */

    SourceEid = BPLIB_AS_NODE_EID;

    /* Run function under test */
    Status = BPLib_AS_Decrement(SourceEid, BPLIB_AS_NUM_COUNTERS);
    
    /* Assert that the decrement function failed */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_SOURCE_EID_MISSING, Status);

    /* === Source counter test === */

    SourceEid = 2;

    /* Run function under test */
    Status = BPLib_AS_Decrement(SourceEid, BPLIB_AS_NUM_COUNTERS);

    /* Assert that the decrement function failed */
    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_CNTR, Status);
}

void Test_BPLib_AS_ResetAllCounters_Nominal()
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

void Test_BPLib_AS_ResetAllCounters_Error()
{
    // None
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
    ADD_TEST(Test_BPLib_AS_ResetAllCounters_Error);
}
