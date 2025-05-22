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
    /* Nothing should be discarded if current time is before bundle expiration time */
    UT_SetDeferredRetcode(UT_KEY(BPA_TIMEP_GetHostTime), 1, BPLIB_STOR_EPOCHOFFSET + 1);
    UtAssert_INT32_EQ(BPLib_STOR_GarbageCollect(&BplibInst), BPLIB_SUCCESS);

    /* Expired Counters start at 2 */
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DELETED_EXPIRED, 
                                                Context_BPLib_AS_Increment[2].Counter);
    UtAssert_INT32_EQ(0, Context_BPLib_AS_Increment[2].Amount);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DELETED, 
                                                Context_BPLib_AS_Increment[3].Counter);
    UtAssert_INT32_EQ(0, Context_BPLib_AS_Increment[3].Amount);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DISCARDED, 
                                                Context_BPLib_AS_Increment[4].Counter);
    UtAssert_INT32_EQ(0, Context_BPLib_AS_Increment[4].Amount);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_STORED, 
                                                Context_BPLib_AS_Decrement[0].Counter);
    UtAssert_INT32_EQ(0, Context_BPLib_AS_Decrement[0].Amount);

    /* Skip counters 5, 6 which are for DiscardEgressed */

    /* Storage should be discarded because bundle timestamp is expired */
    UT_SetDeferredRetcode(UT_KEY(BPA_TIMEP_GetHostTime), 1, INT64_MAX);
    UtAssert_INT32_EQ(BPLib_STOR_GarbageCollect(&BplibInst), BPLIB_SUCCESS);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DELETED_EXPIRED, 
                                                Context_BPLib_AS_Increment[7].Counter);
    UtAssert_INT32_EQ(1, Context_BPLib_AS_Increment[7].Amount);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DELETED, 
                                                Context_BPLib_AS_Increment[8].Counter);
    UtAssert_INT32_EQ(1, Context_BPLib_AS_Increment[8].Amount);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_DISCARDED, 
                                                Context_BPLib_AS_Increment[9].Counter);
    UtAssert_INT32_EQ(1, Context_BPLib_AS_Increment[9].Amount);
    UtAssert_EQ(BPLib_AS_Counter_t, BUNDLE_COUNT_STORED, 
                                                Context_BPLib_AS_Decrement[2].Counter);
    UtAssert_INT32_EQ(1, Context_BPLib_AS_Decrement[2].Amount);
}

/* Test STOR_GarbageCollect handles a SQL Failure */
void Test_BPLib_STOR_GarbageCollect_SQLFail(void)
{
    size_t NumDiscarded;

    /* Force sql statements to fail by passing a NULL instance */
    BplibInst.BundleStorage.db = NULL;
    UtAssert_INT32_EQ(BPLib_STOR_GarbageCollect(&BplibInst), BPLIB_STOR_SQL_DISCARD_ERR);

    /* Look for event message */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 2);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_STOR_SQL_GC_ERR_EID);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[1].EventID, BPLIB_STOR_SQL_GC_ERR_EID);
}


void TestBplib_STOR_Register(void)
{
    /* Init/Teardown */
    UtTest_Add(Test_BPLib_STOR_Init, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_Init");
    UtTest_Add(Test_BPLib_STOR_Init, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_Init_NullInst");
    UtTest_Add(Test_BPLib_STOR_Destroy, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_Destroy");

    /* Storage Table Tests */
    UtTest_Add(Test_BPLib_STOR_StorageTblValidateFunc_Nominal, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_StorageTblValidateFunc_Nominal");

    /* Garbage Collect Tests */
    UtTest_Add(Test_BPLib_STOR_GarbageCollect_NullParams, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_GC_NullParams");
    UtTest_Add(Test_BPLib_STOR_GarbageCollect_NominalExpired, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_GarbageCollect_NominalExpired");
    UtTest_Add(Test_BPLib_STOR_GarbageCollect_SQLFail, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_GC_SQLFail");
}
