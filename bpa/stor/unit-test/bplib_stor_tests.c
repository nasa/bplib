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

/*
 * Include
 */
#include "bplib_stor_test_utils.h"
#include "bplib_nc.h"
#include "bplib_qm_handlers.h"

#include <stdlib.h>

/*
** Test function for
** int BPLib_STOR_Init()
*/
void Test_BPLib_STOR_Init(void)
{
    UtAssert_INT32_EQ(BPLib_STOR_Init(&BplibInst), BPLIB_SUCCESS);
}

void Test_BPLib_STOR_InitNullInst(void)
{
    UtAssert_INT32_EQ(BPLib_STOR_Init(NULL), BPLIB_NULL_PTR_ERROR);
}

/* Test the STOR Correctly initializes the stored bundle count */
void Test_BPLib_STOR_InitStoredCount(void)
{
    /* The fixture for this function pre-stores one bundle. therefore the check is simply that the
    ** stored count is already 1.
    */
    UtAssert_EQ(uint32_t, BplibInst.BundleStorage.BundleCountStored, 1);
}

/* Test Destroy runs without segfault */
void Test_BPLib_STOR_Destroy(void)
{
    BPLib_STOR_Destroy(&BplibInst);
    BPLib_STOR_Destroy(NULL);
}

/* Test Table Validation returns SUCCESS in Nominal case */
void Test_BPLib_STOR_StorageTblValidateFunc_Nominal(void)
{
    BPLib_STOR_StorageTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));
    UtAssert_INT32_EQ((int32) BPLib_STOR_StorageTblValidateFunc(&TestTblData), (int32) BPLIB_SUCCESS);     
}


/*******************************************************************************
** GarbageCollect Tests
*/

/* Test STOR_GarbageCollect guards against Null Params */
void Test_BPLib_STOR_GarbageCollect_NullParams(void)
{
    UtAssert_INT32_EQ(BPLib_STOR_GarbageCollect(NULL), BPLIB_NULL_PTR_ERROR);
}

/* Test STOR_GarbageCollect discards a bundle. */
void Test_BPLib_STOR_GarbageCollect_NominalExpired(void)
{
    UtAssert_EQ(uint32_t, BplibInst.BundleStorage.BundleCountStored, 1);

    /* Nothing should be discarded if current time is before bundle expiration time */
    UT_SetDeferredRetcode(UT_KEY(BPA_TIMEP_GetHostTime), 1, BPLIB_STOR_EPOCHOFFSET + 1);
    UtAssert_INT32_EQ(BPLib_STOR_GarbageCollect(&BplibInst), BPLIB_SUCCESS);

    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DELETED_EXPIRED, 
                                                Context_BPLib_AS_Increment[0].Counter);
    UtAssert_INT32_EQ(0, Context_BPLib_AS_Increment[0].Amount);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DELETED, 
                                                Context_BPLib_AS_Increment[1].Counter);
    UtAssert_INT32_EQ(0, Context_BPLib_AS_Increment[1].Amount);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DISCARDED, 
                                                Context_BPLib_AS_Increment[2].Counter);
    UtAssert_INT32_EQ(0, Context_BPLib_AS_Increment[2].Amount);
    UtAssert_EQ(uint32_t, BplibInst.BundleStorage.BundleCountStored, 1);

    /* Skip counters 3,4 which are for DiscardEgressed */

    /* Storage should be discarded because bundle timestamp is expired */
    UT_SetDeferredRetcode(UT_KEY(BPA_TIMEP_GetHostTime), 1, INT64_MAX);
    UtAssert_INT32_EQ(BPLib_STOR_GarbageCollect(&BplibInst), BPLIB_SUCCESS);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DELETED_EXPIRED, 
                                                Context_BPLib_AS_Increment[5].Counter);
    UtAssert_INT32_EQ(1, Context_BPLib_AS_Increment[5].Amount);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DELETED, 
                                                Context_BPLib_AS_Increment[6].Counter);
    UtAssert_INT32_EQ(1, Context_BPLib_AS_Increment[6].Amount);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DISCARDED, 
                                                Context_BPLib_AS_Increment[7].Counter);
    UtAssert_INT32_EQ(1, Context_BPLib_AS_Increment[7].Amount);
    UtAssert_EQ(uint32_t, BplibInst.BundleStorage.BundleCountStored, 0);
}

/* Test STOR_GarbageCollect handles a SQL Failure */
void Test_BPLib_STOR_GarbageCollect_SQLFail(void)
{
    /* Force sql statements to fail by passing a NULL instance */
    BplibInst.BundleStorage.db = NULL;
    UtAssert_INT32_EQ(BPLib_STOR_GarbageCollect(&BplibInst), BPLIB_STOR_SQL_DISCARD_ERR);

    /* Look for event message */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 2);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_STOR_SQL_GC_ERR_EID);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[1].EventID, BPLIB_STOR_SQL_GC_ERR_EID);
}

void Test_BPLib_STOR_UpdateHkPkt_Nominal(void)
{
    size_t ExpectedBytesMemInUse;
    size_t ExpectedBytesMemHighWater;
    size_t ExpectedBytesMemFree;
    size_t ExpectedKbStorageInUse;

    memset((void*) &BPLib_STOR_StoragePayload, 0, sizeof(BPLib_StorageHkTlm_Payload_t));

    BplibInst.pool.impl.num_blocks = 10;
    BplibInst.pool.impl.num_free   = 20;
    BplibInst.pool.impl.block_size = 30;

    ExpectedBytesMemInUse     = ((BplibInst.pool.impl.num_blocks - BplibInst.pool.impl.num_free) * BplibInst.pool.impl.block_size);
    ExpectedBytesMemHighWater = ExpectedBytesMemInUse;
    ExpectedBytesMemFree      = (BplibInst.pool.impl.num_free * BplibInst.pool.impl.block_size);
    ExpectedKbStorageInUse    = (BplibInst.BundleStorage.BytesStorageInUse / 1000);

    BPLib_STOR_UpdateHkPkt(&BplibInst);

    UtAssert_EQ(size_t, BPLib_STOR_StoragePayload.BytesMemInUse,     ExpectedBytesMemInUse);
    UtAssert_EQ(size_t, BPLib_STOR_StoragePayload.BytesMemHighWater, ExpectedBytesMemHighWater);
    UtAssert_EQ(size_t, BPLib_STOR_StoragePayload.BytesMemFree,      ExpectedBytesMemFree);
    UtAssert_EQ(size_t, BPLib_STOR_StoragePayload.KbStorageInUse,    ExpectedKbStorageInUse);
}


void TestBplib_STOR_Register(void)
{
    /* Init/Teardown */
    UtTest_Add(Test_BPLib_STOR_Init, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_Init");
    UtTest_Add(Test_BPLib_STOR_Init, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_Init_NullInst");
    UtTest_Add(Test_BPLib_STOR_Destroy, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_Destroy");
    UtTest_Add(Test_BPLib_STOR_InitStoredCount, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_InitStoredCount");

    /* Storage Table Tests */
    UtTest_Add(Test_BPLib_STOR_StorageTblValidateFunc_Nominal, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_StorageTblValidateFunc_Nominal");

    /* Garbage Collect Tests */
    UtTest_Add(Test_BPLib_STOR_GarbageCollect_NullParams, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_GC_NullParams");
    UtTest_Add(Test_BPLib_STOR_GarbageCollect_NominalExpired, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_GarbageCollect_NominalExpired");
    UtTest_Add(Test_BPLib_STOR_GarbageCollect_SQLFail, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_GC_SQLFail");
}
