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

/*******************************************************************************
** EgressForID Tests
*/

/* Test STOR_EgressForID guards against Null Params and Bad Params */
void Test_BPLib_STOR_EgressForID_NullParams(void)
{
    size_t NumEgressed;

    /* Null Params */
    UtAssert_INT32_EQ(BPLib_STOR_EgressForID(NULL, 0, true, &NumEgressed), BPLIB_NULL_PTR_ERROR);
    UtAssert_INT32_EQ(BPLib_STOR_EgressForID(&BplibInst, 0, true, NULL), BPLIB_NULL_PTR_ERROR);

    /* Bad MAX_CHANNELS and MAX_CONTACTS Value */
    UtAssert_INT32_EQ(BPLib_STOR_EgressForID(&BplibInst, BPLIB_MAX_NUM_CHANNELS + 1, true, &NumEgressed), BPLIB_STOR_PARAM_ERR);
    UtAssert_INT32_EQ(BPLib_STOR_EgressForID(&BplibInst, BPLIB_MAX_NUM_CONTACTS + 1, false, &NumEgressed), BPLIB_STOR_PARAM_ERR);
}

/* Test STOR_EgressForID Nominal Contact Behavior */
void Test_BPLib_STOR_EgressForID_NominalChan(void)
{
    size_t NumEgressed = 0;
    BPLib_Bundle_t *LoadedBundle;
    uint32_t EgressID = 0;

    /** Step 1: Load all available bundles into an egress batch **/

    /* Select a filter for the DestEID range matching the test bundle */
    BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[EgressID].LocalServiceNumber = 100;
    BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[EgressID].LocalServiceNumber = 1;
    
    /* Verify Egress Success and Egress Count remains 0 */
    UtAssert_INT32_EQ(BPLib_STOR_EgressForID(&BplibInst, EgressID, true, &NumEgressed), BPLIB_SUCCESS);
    UtAssert_INT32_EQ(NumEgressed, 0);
    UtAssert_STUB_COUNT(BPLib_QM_WaitQueueTryPush, 0);

    /* Verify bundle is in the batch */
    UtAssert_INT32_EQ(BplibInst.BundleStorage.ChannelLoadBatches[EgressID].Size, 1);

    /** Step 2: Push the egress batch into QM **/

    /* Call Egress Again, this time it should be egressed because there was a batch loaded */
    UtAssert_INT32_EQ(BPLib_STOR_EgressForID(&BplibInst, EgressID, true, &NumEgressed), BPLIB_SUCCESS);
    UtAssert_INT32_EQ(NumEgressed, 1);

    /* Ensure bundle egressed to channel */
    UtAssert_STUB_COUNT(BPLib_QM_WaitQueueTryPush, 1);

    /* Check Bundle Contents */
    LoadedBundle = Context_BPLib_QM_WaitQueueTryPush[0].Bundle;
    UtAssert_NOT_NULL(LoadedBundle);
    UtAssert_INT32_EQ(LoadedBundle->Meta.EgressID, EgressID);
    UtAssert_UINT32_EQ(LoadedBundle->blocks.PrimaryBlock.Timestamp.CreateTime, 797186475264);
    UtAssert_UINT32_EQ(LoadedBundle->blocks.PrimaryBlock.Lifetime, 5000);
    UtAssert_UINT32_EQ(LoadedBundle->blocks.PrimaryBlock.DestEID.Node, 100);
    UtAssert_UINT32_EQ(LoadedBundle->blocks.PrimaryBlock.DestEID.Service, 1);
    UtAssert_UINT32_EQ(((BPLib_MEM_Block_t *)(LoadedBundle))->used_len, sizeof(BPLib_BBlocks_t));

    /* Check blob contents */
    UtAssert_StrCmp((char*)LoadedBundle->blob->user_data.raw_bytes, "CBOR Blob", "Blob Comparison: %s == %s",
        (char*)LoadedBundle->blob->user_data.raw_bytes, "CBOR Blob");

    /** Step 3: Verify batch was reset as a result of being consumed **/
    UtAssert_INT32_EQ(BPLib_STOR_EgressForID(&BplibInst, EgressID, true, &NumEgressed), BPLIB_SUCCESS);
    UtAssert_INT32_EQ(NumEgressed, 0);
    /* This check ensures LoadBatch_Reset was called */
    UtAssert_INT32_EQ(BplibInst.BundleStorage.ChannelLoadBatches[EgressID].Size, 0);
}

/* Test STOR_EgressForID Nominal Contact Behavior */
void Test_BPLib_STOR_EgressForID_NominalCont(void)
{
    size_t NumEgressed = 0;
    BPLib_Bundle_t *LoadedBundle;
    uint32_t EgressID = 0;

    /** Step 1: Load all available bundles into an egress batch **/

    /* Select a filter for the DestEID range matching the test bundle */
    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MaxNode = 100;
    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MinNode = 100;
    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MaxService = 1;
    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MinService = 1;
    
    /* Verify Egress Success and Egress Count remains 0 */
    UtAssert_INT32_EQ(BPLib_STOR_EgressForID(&BplibInst, EgressID, false, &NumEgressed), BPLIB_SUCCESS);
    UtAssert_INT32_EQ(NumEgressed, 0);
    UtAssert_STUB_COUNT(BPLib_QM_WaitQueueTryPush, 0);

    /* Verify bundle is in the batch */
    UtAssert_INT32_EQ(BplibInst.BundleStorage.ContactLoadBatches[EgressID].Size, 1);

    /** Step 2: Push the egress batch into QM **/

    /* Call Egress Again, this time it should be egressed because there was a batch loaded */
    UtAssert_INT32_EQ(BPLib_STOR_EgressForID(&BplibInst, EgressID, false, &NumEgressed), BPLIB_SUCCESS);
    UtAssert_INT32_EQ(NumEgressed, 1);

    /* Ensure bundle egressed to channel */
    UtAssert_STUB_COUNT(BPLib_QM_WaitQueueTryPush, 1);

    /* Check Bundle Contents */
    LoadedBundle = Context_BPLib_QM_WaitQueueTryPush[0].Bundle;
    UtAssert_NOT_NULL(LoadedBundle);
    UtAssert_INT32_EQ(LoadedBundle->Meta.EgressID, EgressID);
    UtAssert_UINT32_EQ(LoadedBundle->blocks.PrimaryBlock.Timestamp.CreateTime, 797186475264);
    UtAssert_UINT32_EQ(LoadedBundle->blocks.PrimaryBlock.Lifetime, 5000);
    UtAssert_UINT32_EQ(LoadedBundle->blocks.PrimaryBlock.DestEID.Node, 100);
    UtAssert_UINT32_EQ(LoadedBundle->blocks.PrimaryBlock.DestEID.Service, 1);
    UtAssert_UINT32_EQ(((BPLib_MEM_Block_t *)(LoadedBundle))->used_len, sizeof(BPLib_BBlocks_t));

    /* Check blob contents */
    UtAssert_StrCmp((char*)LoadedBundle->blob->user_data.raw_bytes, "CBOR Blob", "Blob Comparison: %s == %s",
        (char*)LoadedBundle->blob->user_data.raw_bytes, "CBOR Blob");

    /** Step 3: Verify batch was reset as a result of being consumed **/
    UtAssert_INT32_EQ(BPLib_STOR_EgressForID(&BplibInst, EgressID, false, &NumEgressed), BPLIB_SUCCESS);
    UtAssert_INT32_EQ(NumEgressed, 0);
    /* This check ensures LoadBatch_Reset was called */
    UtAssert_INT32_EQ(BplibInst.BundleStorage.ChannelLoadBatches[EgressID].Size, 0);
}

/* Test STOR_EgressForID frees Bundles if the bundle can't be pushed */
void Test_BPLib_STOR_EgressForID_WaitQueuePushFail(void)
{
    size_t NumEgressed = 0;
    uint32_t EgressID = 0;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_QM_WaitQueueTryPush), (UT_IntReturn_t) false);

    /** Step 1: Load all available bundles into an egress batch **/

    /* Select a filter for the DestEID range matching the test bundle */
    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MaxNode = 100;
    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MinNode = 100;
    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MaxService = 1;
    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MinService = 1;
    
    /* Verify Egress Success and Egress Count remains 0 */
    UtAssert_INT32_EQ(BPLib_STOR_EgressForID(&BplibInst, EgressID, false, &NumEgressed), BPLIB_SUCCESS);
    UtAssert_INT32_EQ(NumEgressed, 0);
    UtAssert_STUB_COUNT(BPLib_QM_WaitQueueTryPush, 0);

    /* Verify bundle is in the batch */
    UtAssert_INT32_EQ(BplibInst.BundleStorage.ContactLoadBatches[EgressID].Size, 1);

    /** Step 2: Push the egress batch into QM, this should fail. **/

    /* Call Egress Again, this time it should be egressed because there was a batch loaded */
    UtAssert_INT32_EQ(BPLib_STOR_EgressForID(&BplibInst, EgressID, false, &NumEgressed), BPLIB_SUCCESS);
    UtAssert_INT32_EQ(NumEgressed, 0);

    /* Ensure bundle egressed to channel */
    UtAssert_STUB_COUNT(BPLib_QM_WaitQueueTryPush, 1);

    /* Verify bundlefree was called (Why this is 2: The test fixture calls Free once during store()) */
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1 + 1);
}

/* Test that one bundle without an available contact path does not get egressed */
void Test_BPLib_STOR_EgressForID_NoBundles(void)
{
    size_t NumEgressed = 0;
    uint32_t InvalidServ = 3;
    uint32_t EgressID = 0;

    /** Step 1: Load all available bundles into an egress batch **/

    /* Select a filter for the DestEID range matching the test bundle */
    BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[EgressID].LocalServiceNumber = 100;
    BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[EgressID].LocalServiceNumber = InvalidServ;
    
    /* Verify Egress Success and Egress Count remains 0 */
    UtAssert_INT32_EQ(BPLib_STOR_EgressForID(&BplibInst, EgressID, true, &NumEgressed), BPLIB_SUCCESS);
    UtAssert_INT32_EQ(NumEgressed, 0);
    UtAssert_STUB_COUNT(BPLib_QM_WaitQueueTryPush, 0);

    /* Verify bundle is NOT in the batch */
    UtAssert_INT32_EQ(BplibInst.BundleStorage.ContactLoadBatches[EgressID].Size, 0);
}

/* Test STOR_EgressForDestEid handles a SQL Failure */
void Test_BPLib_STOR_EgressForID_SQLFail(void)
{
    size_t NumEgressed;

    /* Force sql statements to fail by passing a NULL instance */
    BplibInst.BundleStorage.db = NULL;
    UtAssert_INT32_EQ(BPLib_STOR_EgressForID(&BplibInst, 0, true, &NumEgressed),
        BPLIB_STOR_SQL_LOAD_IDS_ERR);

    /* Look for event message */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_STOR_SQL_LOAD_ERR_EID);
}

void TestBplib_STOR_Load_Register(void)
{
    /* Load (Egress) Tests */
    UtTest_Add(Test_BPLib_STOR_EgressForID_NullParams, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_Egress_NullParams");
    UtTest_Add(Test_BPLib_STOR_EgressForID_NominalChan, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_Egress_NominalChan");
    UtTest_Add(Test_BPLib_STOR_EgressForID_NominalCont, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_Egress_NominalCont");
    UtTest_Add(Test_BPLib_STOR_EgressForID_WaitQueuePushFail, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_Egress_WaitQueuePushFail");
    UtTest_Add(Test_BPLib_STOR_EgressForID_NoBundles, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_EgressForID_NoBundles");
    UtTest_Add(Test_BPLib_STOR_EgressForID_SQLFail, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_Egress_SQLFail");
}
