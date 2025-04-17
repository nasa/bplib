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

    /* Store and flush bundle */
    UtAssert_INT32_EQ(BPLib_STOR_StoreBundle(&BplibInst, &Bundle), BPLIB_SUCCESS);
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

    /* Store and flush bundle */
    BplibInst.BundleStorage.db = NULL;
    UtAssert_INT32_EQ(BPLib_STOR_StoreBundle(&BplibInst, &Bundle), BPLIB_SUCCESS);
    UtAssert_INT32_EQ(BPLib_STOR_FlushPending(&BplibInst), BPLIB_STOR_SQL_STORAGE_ERR);

    /* Ensure the bundle was still freed despite the SQL error */
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 1);

    /* Look for event message */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_STOR_SQL_STORE_ERR_EID);

}

/*******************************************************************************
** GarbageCollect Tests
*/

/* Test STOR_GarbageCollect guards against Null Params */
void Test_BPLib_STOR_GarbageCollect_NullParams(void)
{
    size_t NumDiscarded;

    UtAssert_INT32_EQ(BPLib_STOR_GarbageCollect(NULL, &NumDiscarded), BPLIB_NULL_PTR_ERROR);
    UtAssert_INT32_EQ(BPLib_STOR_GarbageCollect(&BplibInst, NULL), BPLIB_NULL_PTR_ERROR);
}

/* Test STOR_GarbageCollect discards a bundle. */
void Test_BPLib_STOR_GarbageCollect_Nominal(void)
{
    size_t NumDiscarded;

    /* Nothing should be discarded if current time is before bundle expiration time */
    UT_SetDeferredRetcode(UT_KEY(BPA_TIMEP_GetHostTime), 1, BPLIB_STOR_EPOCHOFFSET + 1);
    UtAssert_INT32_EQ(BPLib_STOR_GarbageCollect(&BplibInst, &NumDiscarded), BPLIB_SUCCESS);
    UtAssert_UINT32_EQ(NumDiscarded, 0);

    /* Storage should be discarded because bundle timestamp is expired */
    UT_SetDeferredRetcode(UT_KEY(BPA_TIMEP_GetHostTime), 1, INT64_MAX);
    UtAssert_INT32_EQ(BPLib_STOR_GarbageCollect(&BplibInst, &NumDiscarded), BPLIB_SUCCESS);
    UtAssert_UINT32_EQ(NumDiscarded, 1);
}

/* Test STOR_GarbageCollect handles a SQL Failure */
void Test_BPLib_STOR_GarbageCollect_SQLFail(void)
{
    size_t NumDiscarded;

    /* Force sql statements to fail by passing a NULL instance */
    BplibInst.BundleStorage.db = NULL;
    UtAssert_INT32_EQ(BPLib_STOR_GarbageCollect(&BplibInst, &NumDiscarded), BPLIB_STOR_SQL_DISCARD_ERR);

    /* Look for event message */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_STOR_SQL_GC_ERR_EID);
}

/*******************************************************************************
** EgressForDestEID Tests
*/

/* Test STOR_EgressForDestEID guards against Null Params */
void Test_BPLib_STOR_EgressForDestEid_NullParams(void)
{
    BPLib_EID_Pattern_t DestEID;
    size_t NumEgressed;

    /* Null Params */
    UtAssert_INT32_EQ(BPLib_STOR_EgressForDestEID(NULL, 0, true, &DestEID, 1, &NumEgressed), BPLIB_NULL_PTR_ERROR);
    UtAssert_INT32_EQ(BPLib_STOR_EgressForDestEID(&BplibInst, 0, true, NULL, 1, &NumEgressed), BPLIB_NULL_PTR_ERROR);
    UtAssert_INT32_EQ(BPLib_STOR_EgressForDestEID(&BplibInst, 0, true, &DestEID, 1, NULL), BPLIB_NULL_PTR_ERROR);

    /* Bad MaxBundles Value */
    UtAssert_INT32_EQ(BPLib_STOR_EgressForDestEID(&BplibInst, 0, true, &DestEID, 0, &NumEgressed), BPLIB_STOR_PARAM_ERR);
}

/* Test STOR_EgressForDestEID Nominal Channel Behavior */
void Test_BPLib_STOR_EgressForDestEid_NominalChan(void)
{
    BPLib_EID_Pattern_t DestEID;
    size_t NumEgressed;
    BPLib_Bundle_t *LoadedBundle;
    int16_t EgressID = 4;
    size_t MaxBundles = 1;

    /* Select a filter for the DestEID range matching the test bundle */
    DestEID.MaxNode = 100;
    DestEID.MinNode = 100;
    DestEID.MaxService = 1;
    DestEID.MinService = 1;

    /* Verify Egress Success and Egress Count */
    UtAssert_INT32_EQ(BPLib_STOR_EgressForDestEID(&BplibInst, EgressID, true, &DestEID,
        MaxBundles, &NumEgressed), BPLIB_SUCCESS);
    UtAssert_INT32_EQ(NumEgressed, 1);

    /* Ensure Job Added for Channel */
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 1);
    UtAssert_INT32_EQ(Context_BPLib_QM_CreateJob[0].State, CHANNEL_OUT_STOR_TO_CT);

    /* Check Bundle Contents */
    LoadedBundle = Context_BPLib_QM_CreateJob[0].Bundle;
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

}

/* Test STOR_EgressForDestEID Nomminal Contact Behavior */
void Test_BPLib_STOR_EgressForDestEid_NominalCont(void)
{
    BPLib_EID_Pattern_t DestEID;
    size_t NumEgressed;
    BPLib_Bundle_t *LoadedBundle;
    int EgressID = 4;
    size_t MaxBundles = 1;

    /* Select a filter for the DestEID range matching the test bundle */
    DestEID.MaxNode = 100;
    DestEID.MinNode = 100;
    DestEID.MaxService = 1;
    DestEID.MinService = 1;

    /* Verify Egress Success and Egress Count */
    UtAssert_INT32_EQ(BPLib_STOR_EgressForDestEID(&BplibInst, EgressID, false, &DestEID,
        MaxBundles, &NumEgressed), BPLIB_SUCCESS);
    UtAssert_INT32_EQ(NumEgressed, 1);

    /* Ensure Job Added for Channel */
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 1);
    UtAssert_INT32_EQ(Context_BPLib_QM_CreateJob[0].State, CONTACT_OUT_STOR_TO_CT);

    /* Check Bundle Contents */
    LoadedBundle = Context_BPLib_QM_CreateJob[0].Bundle;
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
}

/* Test STOR_EgressForDestEid handles a SQL Failure */
void Test_BPLib_STOR_EgressForDestEid_SQLFail(void)
{
    size_t NumEgressed;
    BPLib_EID_Pattern_t DestEID;
    size_t MaxBundles = 1;

    /* Force sql statements to fail by passing a NULL instance */
    BplibInst.BundleStorage.db = NULL;
    UtAssert_INT32_EQ(BPLib_STOR_EgressForDestEID(&BplibInst, 0, true, &DestEID,
        MaxBundles, &NumEgressed), BPLIB_STOR_SQL_LOAD_ERR);

    /* Look for event message */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_STOR_SQL_LOAD_ERR_EID);
}

/* Test STOR_EgressForDestEID frees Bundles if jobs can't be created */
void Test_BPLib_STOR_EgressForDestEid_AddJobFail(void)
{
    BPLib_EID_Pattern_t DestEID;
    size_t NumEgressed;

    /* Select a filter for the DestEID range matching the test bundle */
    size_t MaxBundles = 1;
    DestEID.MaxNode = 100;
    DestEID.MinNode = 100;
    DestEID.MaxService = 1;
    DestEID.MinService = 1;

    /* Make sure error is propagated up to callee */
    UT_SetDeferredRetcode(UT_KEY(BPLib_QM_CreateJob), 1, BPLIB_ERROR);
    UtAssert_INT32_EQ(BPLib_STOR_EgressForDestEID(&BplibInst, 0, false, &DestEID,
        MaxBundles, &NumEgressed), BPLIB_ERROR);
    UtAssert_INT32_EQ(NumEgressed, 0);

    /* Ensure loaded bundle was feed (this should be 2 because the setup calls it once during StoreBundle) */
    UtAssert_STUB_COUNT(BPLib_MEM_BundleFree, 2);
}

/*******************************************************************************
** ScanCache Tests
*/

/* Test ScanCache Detects Null Instance Pointer */
void Test_BPLib_STOR_ScanCache_NullInstError(void)
{
    uint32_t BundlesToScan = 0;
    UtAssert_INT32_EQ(BPLib_STOR_ScanCache(NULL, BundlesToScan), BPLIB_NULL_PTR_ERROR);
}

/* Test that function exits gracefully when storage is empty */
void Test_BPLib_STOR_ScanCache_EmptyStorage(void)
{
    uint32_t BundlesToScan = 1;


    UtAssert_INT32_EQ(BPLib_STOR_ScanCache(&BplibInst, BundlesToScan), BPLIB_SUCCESS);

    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 0);
}

/* Test that one bundle gets set to egress to contact 0 */
void Test_BPLib_STOR_ScanCache_OneContactEgress(void)
{
    uint32_t BundlesToScan = 1;

    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MaxNode = 100;
    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MinNode = 100;
    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MaxService = 1;
    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MinService = 1;

    UtAssert_INT32_EQ(BPLib_STOR_ScanCache(&BplibInst, BundlesToScan), BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 1);
}

/* Test that a failed addition of an unsorted job is reported correctly */
void Test_BPLib_STOR_ScanCache_ErrAddJob(void)
{
    uint32_t BundlesToScan = 2;
    BPLib_Bundle_t Bundle;
    BPLib_Bundle_t *BundlePtr = &Bundle;

    Bundle.Meta.EgressID = BPLIB_UNKNOWN_ROUTE_ID;
    
    UT_SetDeferredRetcode(UT_KEY(BPLib_QM_WaitQueueTryPull), 1, true);
    UT_SetDataBuffer(UT_KEY(BPLib_QM_WaitQueueTryPull), &BundlePtr, sizeof(BPLib_Bundle_t *), false);
    UT_SetDeferredRetcode(UT_KEY(BPLib_EID_PatternIsMatch), 1, true);
    UT_SetDeferredRetcode(UT_KEY(BPLib_QM_CreateJob), 1, BPLIB_ERROR);

    UtAssert_INT32_EQ(BPLib_STOR_ScanCache(&BplibInst, BundlesToScan), BPLIB_ERROR);
    UtAssert_UINT16_EQ(Bundle.Meta.EgressID, 0);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_STOR_SCAN_CACHE_ADD_JOB_ERR_EID);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 1);
}

/* Test that one bundle gets set to egress to channel 1 */
void Test_BPLib_STOR_ScanCache_OneChannelEgress(void)
{
    uint32_t BundlesToScan = 1;
    uint16_t ChanID = BPLIB_MAX_NUM_CHANNELS - 1;

    BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[ChanID].LocalServiceNumber = 1;

    UT_SetDeferredRetcode(UT_KEY(BPLib_NC_GetAppState), ChanID + 1, BPLIB_NC_APP_STATE_STARTED);

    UtAssert_INT32_EQ(BPLib_STOR_ScanCache(&BplibInst, BundlesToScan), BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 1);
}

/* Test that one bundle without an available channel path does not get egressed */
void Test_BPLib_STOR_ScanCache_NoChannel(void)
{
    uint32_t BundlesToScan = 1;
    uint16_t ChanID = BPLIB_MAX_NUM_CHANNELS - 1;
    int16_t InvalidServ = 3;

    BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[ChanID].LocalServiceNumber = InvalidServ;

    UT_SetDeferredRetcode(UT_KEY(BPLib_NC_GetAppState), ChanID + 1, BPLIB_NC_APP_STATE_STARTED);

    UtAssert_INT32_EQ(BPLib_STOR_ScanCache(&BplibInst, BundlesToScan), BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 0);
}

/* Test that one bundle without an available contact path does not get egressed */
void Test_BPLib_STOR_ScanCache_NoContact(void)
{
    uint32_t BundlesToScan = 1;
    int16_t InvalidServ = 3;

    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MaxNode = 100;
    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MinNode = 100;
    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MaxService = InvalidServ;
    BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[0].DestEIDs[0].MinService = InvalidServ;

    UtAssert_INT32_EQ(BPLib_STOR_ScanCache(&BplibInst, BundlesToScan), BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);
    UtAssert_STUB_COUNT(BPLib_QM_CreateJob, 0);
}

/*  Test that ScanCache locks NC before reading */
void Test_BPLib_STOR_ScanCache_NCLock(void)
{
    uint32_t BundlesToScan = 1;
    BPLib_STOR_ScanCache(&BplibInst, BundlesToScan);
    UtAssert_STUB_COUNT(BPLib_NC_ReaderLock, 1);
    UtAssert_STUB_COUNT(BPLib_NC_ReaderUnlock, 1);
}

void TestBplibStor_Register(void)
{
    /* Init/Teardown */
    UtTest_Add(Test_BPLib_STOR_Init, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_Init");
    UtTest_Add(Test_BPLib_STOR_Init, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_Init_NullInst");
    UtTest_Add(Test_BPLib_STOR_Destroy, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_Destroy");

    /* Storage Table Tests */
    UtTest_Add(Test_BPLib_STOR_StorageTblValidateFunc_Nominal, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_StorageTblValidateFunc_Nominal");

    /* Store Tests */
    UtTest_Add(Test_BPLib_STOR_StoreBundle_NullParams, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_StoreBundle_NullParams");
    UtTest_Add(Test_BPLib_STOR_StoreBundle_Nominal, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPlib_STOR_StoreBundleNominal");
    UtTest_Add(Test_BPLib_STOR_StoreBundle_SQLFail, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_StoreBundle_SQLFail");

    /* Load(Egress) Tests */
    UtTest_Add(Test_BPLib_STOR_EgressForDestEid_NullParams, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_Egress_NullParams");
    UtTest_Add(Test_BPLib_STOR_EgressForDestEid_NominalChan, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_Egress_NominalChan");
    UtTest_Add(Test_BPLib_STOR_EgressForDestEid_NominalCont, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_Egress_NominalCont");
    UtTest_Add(Test_BPLib_STOR_EgressForDestEid_SQLFail, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_Egress_SQLFail");
    UtTest_Add(Test_BPLib_STOR_EgressForDestEid_AddJobFail, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_Egress_AddJobFail");

    /* Garbage Collect Tests */
    UtTest_Add(Test_BPLib_STOR_GarbageCollect_NullParams, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_GC_NullParams");
    UtTest_Add(Test_BPLib_STOR_GarbageCollect_Nominal, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_GC_Nominal");
    UtTest_Add(Test_BPLib_STOR_GarbageCollect_SQLFail, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_GC_SQLFail");

    /* Scan Cache Tests */
    UtTest_Add(Test_BPLib_STOR_ScanCache_NullInstError, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_ScanCache_NullInstError");
    UtTest_Add(Test_BPLib_STOR_ScanCache_EmptyStorage, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_ScanCache_EmptyStorage");
    UtTest_Add(Test_BPLib_STOR_ScanCache_OneContactEgress, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_ScanCache_OneContactEgress");
    UtTest_Add(Test_BPLib_STOR_ScanCache_OneChannelEgress, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_ScanCache_OneChannelEgress");
    UtTest_Add(Test_BPLib_STOR_ScanCache_NoContact, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_ScanCache_NoContact");
    UtTest_Add(Test_BPLib_STOR_ScanCache_NoChannel, BPLib_STOR_Test_SetupOneBundleStored, BPLib_STOR_Test_TeardownOneBundleStored, "Test_BPLib_STOR_ScanCache_NoChannel");
    UtTest_Add(Test_BPLib_STOR_ScanCache_NCLock, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_ScanCache_NCLock");
}
