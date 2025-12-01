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

/*******************************************************************************
** FlushPending Tests
*/
void Test_BPLib_STOR_FlushPending_NullParams(void)
{
    UtAssert_INT32_EQ(BPLib_STOR_FlushPending(NULL), BPLIB_NULL_PTR_ERROR);
}

void Test_BPLib_STOR_FlushPending_NoBundles(void)
{
    BplibInst.BundleStorage.InsertBatchSize = 0;

    /* Flush to disk */
    UtAssert_INT32_EQ(BPLib_STOR_FlushPending(&BplibInst), BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);

    /* Ensure no events */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);
}

void Test_BPLib_STOR_FlushPending_Nominal(void)
{
    BPLib_Bundle_t Bundle;
    BPLib_STOR_Test_CreateTestBundle(&Bundle);

    /* Place bundle in storage batch */
    BplibInst.BundleStorage.InsertBatch[0] = &Bundle;
    BplibInst.BundleStorage.InsertBatchSize = 1;

    /* Flush to disk */
    UtAssert_INT32_EQ(BPLib_STOR_FlushPending(&BplibInst), BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);

    /* Ensure the bundle was freed from memory after being stored */
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);

    /* Ensure no events */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);

    /* Ensure batch size was reset */
    UtAssert_INT32_EQ(BplibInst.BundleStorage.InsertBatchSize, 0);

    /* Free the test bundle (for most test cases this is done in utils.c) */
    BPLib_STOR_Test_FreeTestBundle(&Bundle);
}

void Test_BPLib_STOR_FlushPending_SQLFail(void)
{
    BPLib_Bundle_t Bundle;

    /* Place bundle in storage batch */
    BplibInst.BundleStorage.InsertBatch[0] = &Bundle;
    BplibInst.BundleStorage.InsertBatchSize = 1;

    /* Break the SQL layer by setting the sql instance to null */
    BplibInst.BundleStorage.db = NULL;
    UtAssert_INT32_EQ(BPLib_STOR_FlushPending(&BplibInst), BPLIB_STOR_SQL_STORAGE_ERR);

    /* Ensure the bundle was still freed despite the SQL error */
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);

    /* Look for event message */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_STOR_SQL_STORE_ERR_EID);
}

/*******************************************************************************
** StoreBundle Tests
*/

/* Test StoreBundle Guards against Null Params */
void Test_BPLib_STOR_StoreBundle_NullParams(void)
{
    BPLib_Bundle_t Bundle;

    UtAssert_INT32_EQ(BPLib_STOR_StoreBundle(NULL, &Bundle), BPLIB_NULL_PTR_ERROR);
    UtAssert_INT32_EQ(BPLib_STOR_StoreBundle(&BplibInst, NULL), BPLIB_NULL_PTR_ERROR);
}

/* Test StoreBundle succeeds for valid bundle */
void Test_BPLib_STOR_StoreBundle_Nominal(void)
{
    BPLib_Bundle_t Bundle;
    BPLib_STOR_Test_CreateTestBundle(&Bundle);

    /* Store a bundle */
    UtAssert_INT32_EQ(BPLib_STOR_StoreBundle(&BplibInst, &Bundle), BPLIB_SUCCESS);

    /* At this point, batch size should be 1, and nothing should have been flushed*/
    UtAssert_INT32_EQ(BplibInst.BundleStorage.InsertBatchSize, 1);

    /* Flush to disk */
    UtAssert_INT32_EQ(BPLib_STOR_FlushPending(&BplibInst), BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);

    /* Ensure the bundle was freed from memory after being stored */
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);

    /* Ensure batch size was reset */
    UtAssert_INT32_EQ(BplibInst.BundleStorage.InsertBatchSize, 0);

    /* Free the test bundle (for most test cases this is done in utils.c) */
    BPLib_STOR_Test_FreeTestBundle(&Bundle);
}

/* Test StoreBundle frees bundles if SQL fails. Note: because there are no SQL
** stubs, the only way I could force this to fail is by setting the db to NULL,
** which triggers some error logic in SQL implementation
*/
void Test_BPLib_STOR_StoreBundle_SQLFail(void)
{
    BPLib_Bundle_t Bundle;

    BPLib_STOR_Test_CreateTestBundle(&Bundle);
    
    /* Store and flush bundle */
    BplibInst.BundleStorage.db = NULL;
    UtAssert_INT32_EQ(BPLib_STOR_StoreBundle(&BplibInst, &Bundle), BPLIB_SUCCESS);
    UtAssert_INT32_EQ(BPLib_STOR_FlushPending(&BplibInst), BPLIB_STOR_SQL_STORAGE_ERR);

    /* Ensure the bundle was still freed despite the SQL error */
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);

    /* Look for event message */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_STOR_SQL_STORE_ERR_EID);

    /* Ensure batch size was reset */
    UtAssert_INT32_EQ(BplibInst.BundleStorage.InsertBatchSize, 0);
}

/* Test that storing a batch worth of bundles places the bundles in persistent storage */
void Test_BPLib_STOR_StoreBundle_StoreBatch(void)
{
    BPLib_Bundle_t Bundle;
    int i;
    BPLib_STOR_Test_CreateTestBundle(&Bundle);

    /* Store a batch worth of bundles */
    for (i = 0; i < BPLIB_STOR_INSERTBATCHSIZE - 1; i++)
    {
        UtAssert_INT32_EQ(BPLib_STOR_StoreBundle(&BplibInst, &Bundle), BPLIB_SUCCESS);
        Bundle.blocks.PrimaryBlock.Timestamp.CreateTime += 1;
    }

    /* At this point, batch size should be INSERTBATCHSIZE - 1, and nothing should have been flushed */
    UtAssert_INT32_EQ(BplibInst.BundleStorage.InsertBatchSize, BPLIB_STOR_INSERTBATCHSIZE - 1);

    /* Store the bundle that triggers the batch */
    UtAssert_INT32_EQ(BPLib_STOR_StoreBundle(&BplibInst, &Bundle), BPLIB_SUCCESS);
    UtAssert_INT32_EQ(BplibInst.BundleStorage.InsertBatchSize, 0);

    /* No events */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);

    /* Ensure the bundle was freed from memory after being stored */
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, BPLIB_STOR_INSERTBATCHSIZE);

    /* Ensure batch size was reset */
    UtAssert_INT32_EQ(BplibInst.BundleStorage.InsertBatchSize, 0);

    /* Free the test bundle (for most test cases this is done in utils.c) */
    BPLib_STOR_Test_FreeTestBundle(&Bundle);
}

void TestBplib_STOR_Store_Register(void)
{
    /* Store Tests */
    UtTest_Add(Test_BPLib_STOR_StoreBundle_NullParams, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_StoreBundle_NullParams");
    UtTest_Add(Test_BPLib_STOR_StoreBundle_Nominal, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPlib_STOR_StoreBundleNominal");
    UtTest_Add(Test_BPLib_STOR_StoreBundle_SQLFail, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_StoreBundle_SQLFail");
    UtTest_Add(Test_BPLib_STOR_StoreBundle_StoreBatch, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_StoreBundle_StoreBatch");

    /* FlushPending Tests */
    UtTest_Add(Test_BPLib_STOR_FlushPending_NullParams, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_FlushPending_NullParams");
    UtTest_Add(Test_BPLib_STOR_FlushPending_NoBundles, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_FlushPending_NoBundles");
    UtTest_Add(Test_BPLib_STOR_FlushPending_Nominal, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_FlushPending_Nominal");
    UtTest_Add(Test_BPLib_STOR_FlushPending_SQLFail, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_FlushPending_SQLFail");
}
