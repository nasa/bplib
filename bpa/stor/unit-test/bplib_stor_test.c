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

/*
** Test function for
** int BPLib_STOR_Init()
*/
void Test_BPLib_STOR_Init(void)
{
    UtAssert_INT32_EQ(BPLib_STOR_Init(), BPLIB_SUCCESS);
}

void Test_BPLib_STOR_StorageTblValidateFunc_Nominal(void)
{
    BPLib_STOR_StorageTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));
    UtAssert_INT32_EQ((int32) BPLib_STOR_StorageTblValidateFunc(&TestTblData), (int32) BPLIB_SUCCESS);     
}

void Test_BPLib_STOR_ScanCache_NullInstError(void)
{
    uint32_t BundlesToScan = 0;
    UtAssert_INT32_EQ(BPLib_STOR_ScanCache(NULL, BundlesToScan), BPLIB_NULL_PTR_ERROR);
}

/* Test that a null bundle was pulled from the queue */
void Test_BPLib_STOR_ScanCache_NullQueue(void)
{
    uint32_t BundlesToScan = 1;
    BPLib_Bundle_t *BundlePtr = NULL;
    
    UT_SetDeferredRetcode(UT_KEY(BPLib_NC_GetAppState), 1, BPLIB_NC_APP_STATE_STARTED);
    UT_SetDeferredRetcode(UT_KEY(BPLib_QM_WaitQueueTryPull), 1, true);
    UT_SetDataBuffer(UT_KEY(BPLib_QM_WaitQueueTryPull), &BundlePtr, sizeof(BundlePtr), false);

    UtAssert_INT32_EQ(BPLib_STOR_ScanCache(&BplibInst, BundlesToScan), BPLIB_NULL_PTR_ERROR);

    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_STOR_SCAN_CACHE_GOT_NULL_BUNDLE_WARN_EID);
}

/* Test that function exits gracefully when the queue is empty */
void Test_BPLib_STOR_ScanCache_EmptyQueue(void)
{
    uint32_t BundlesToScan = 1;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_QM_WaitQueueTryPull), false);

    UtAssert_INT32_EQ(BPLib_STOR_ScanCache(&BplibInst, BundlesToScan), BPLIB_SUCCESS);

    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);
    UtAssert_STUB_COUNT(BPLib_QM_AddUnsortedJob, 0);
}

/* Test that one bundle gets set to egress to contact 0 */
void Test_BPLib_STOR_ScanCache_OneContactEgress(void)
{
    uint32_t BundlesToScan = 1;
    BPLib_Bundle_t Bundle;
    BPLib_Bundle_t *BundlePtr = &Bundle;

    Bundle.Meta.EgressID = BPLIB_UNKNOWN_ROUTE_ID;
    
    UT_SetDeferredRetcode(UT_KEY(BPLib_QM_WaitQueueTryPull), 1, true);
    UT_SetDataBuffer(UT_KEY(BPLib_QM_WaitQueueTryPull), &BundlePtr, sizeof(BPLib_Bundle_t *), false);
    UT_SetDeferredRetcode(UT_KEY(BPLib_EID_PatternIsMatch), 1, true);

    UtAssert_INT32_EQ(BPLib_STOR_ScanCache(&BplibInst, BundlesToScan), BPLIB_SUCCESS);
    UtAssert_UINT16_EQ(Bundle.Meta.EgressID, 0);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);
    UtAssert_STUB_COUNT(BPLib_QM_AddUnsortedJob, 1);
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
    UT_SetDeferredRetcode(UT_KEY(BPLib_QM_AddUnsortedJob), 1, BPLIB_ERROR);

    UtAssert_INT32_EQ(BPLib_STOR_ScanCache(&BplibInst, BundlesToScan), BPLIB_ERROR);
    UtAssert_UINT16_EQ(Bundle.Meta.EgressID, 0);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 1);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_STOR_SCAN_CACHE_ADD_JOB_ERR_EID);
    UtAssert_STUB_COUNT(BPLib_QM_AddUnsortedJob, 1);
}

/* Test that one bundle gets set to egress to channel 1 */
void Test_BPLib_STOR_ScanCache_OneChannelEgress(void)
{
    uint32_t BundlesToScan = 1;
    BPLib_Bundle_t Bundle;
    BPLib_Bundle_t *BundlePtr = &Bundle;
    uint16_t ChanID = BPLIB_MAX_NUM_CHANNELS - 1;

    Bundle.Meta.EgressID = BPLIB_UNKNOWN_ROUTE_ID;
    Bundle.blocks.PrimaryBlock.DestEID.Service = 10;
    BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[ChanID].LocalServiceNumber = 10;
    
    UT_SetDeferredRetcode(UT_KEY(BPLib_NC_GetAppState), ChanID + 1, BPLIB_NC_APP_STATE_STARTED);
    UT_SetDeferredRetcode(UT_KEY(BPLib_QM_WaitQueueTryPull), 1, true);
    UT_SetDataBuffer(UT_KEY(BPLib_QM_WaitQueueTryPull), &BundlePtr, sizeof(BPLib_Bundle_t *), false);
    UT_SetDeferredRetcode(UT_KEY(BPLib_EID_NodeIsMatch), 1, true);

    UtAssert_INT32_EQ(BPLib_STOR_ScanCache(&BplibInst, BundlesToScan), BPLIB_SUCCESS);
    UtAssert_UINT16_EQ(Bundle.Meta.EgressID, ChanID);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);
    UtAssert_STUB_COUNT(BPLib_QM_AddUnsortedJob, 1);
}

/* Test that one bundle without an available channel path does not get egressed */
void Test_BPLib_STOR_ScanCache_NoChannel(void)
{
    uint32_t BundlesToScan = 1;
    BPLib_Bundle_t Bundle;
    BPLib_Bundle_t *BundlePtr = &Bundle;
    uint16_t ChanID = 0;

    Bundle.Meta.EgressID = BPLIB_UNKNOWN_ROUTE_ID;
    Bundle.blocks.PrimaryBlock.DestEID.Service = 10;
    BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[ChanID].LocalServiceNumber = 20;

    UT_SetDeferredRetcode(UT_KEY(BPLib_NC_GetAppState), ChanID + 1, BPLIB_NC_APP_STATE_STARTED);    
    UT_SetDeferredRetcode(UT_KEY(BPLib_QM_WaitQueueTryPull), 1, true);
    UT_SetDataBuffer(UT_KEY(BPLib_QM_WaitQueueTryPull), &BundlePtr, sizeof(BPLib_Bundle_t *), false);
    UT_SetDeferredRetcode(UT_KEY(BPLib_EID_NodeIsMatch), 1, true);

    UtAssert_INT32_EQ(BPLib_STOR_ScanCache(&BplibInst, BundlesToScan), BPLIB_SUCCESS);

    UtAssert_UINT16_EQ(Bundle.Meta.EgressID, BPLIB_UNKNOWN_ROUTE_ID);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);
    UtAssert_STUB_COUNT(BPLib_QM_AddUnsortedJob, 0);
    UtAssert_STUB_COUNT(BPLib_QM_WaitQueueTryPush, 1);
}

/* Test that one bundle without an available contact path does not get egressed */
void Test_BPLib_STOR_ScanCache_NoContact(void)
{
    uint32_t BundlesToScan = 1;
    BPLib_Bundle_t Bundle;
    BPLib_Bundle_t *BundlePtr = &Bundle;

    Bundle.Meta.EgressID = BPLIB_UNKNOWN_ROUTE_ID;
    
    UT_SetDeferredRetcode(UT_KEY(BPLib_QM_WaitQueueTryPull), 1, true);
    UT_SetDataBuffer(UT_KEY(BPLib_QM_WaitQueueTryPull), &BundlePtr, sizeof(BPLib_Bundle_t *), false);

    UtAssert_INT32_EQ(BPLib_STOR_ScanCache(&BplibInst, BundlesToScan), BPLIB_SUCCESS);

    UtAssert_UINT16_EQ(Bundle.Meta.EgressID, BPLIB_UNKNOWN_ROUTE_ID);
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, 0);
    UtAssert_STUB_COUNT(BPLib_QM_AddUnsortedJob, 0);
    UtAssert_STUB_COUNT(BPLib_QM_WaitQueueTryPush, 1);
}


void Test_BPLib_STOR_CacheBundle_Nominal(void)
{
    BPLib_Bundle_t Bundle;

    UtAssert_INT32_EQ(BPLib_STOR_CacheBundle(&BplibInst, &Bundle), BPLIB_SUCCESS);
}

/*
** More BPLib_STOR_ScanCache tests would go here, but this is very temporary code,
** so its probably not worth further unit testing.
*/

void TestBplibStor_Register(void)
{
    UtTest_Add(Test_BPLib_STOR_Init, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_Init");
    UtTest_Add(Test_BPLib_STOR_StorageTblValidateFunc_Nominal, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_StorageTblValidateFunc_Nominal");

    UtTest_Add(Test_BPLib_STOR_ScanCache_NullInstError, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_ScanCache_NullInstError");
    UtTest_Add(Test_BPLib_STOR_ScanCache_NullQueue, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_ScanCache_NullQueue");
    UtTest_Add(Test_BPLib_STOR_ScanCache_EmptyQueue, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_ScanCache_EmptyQueue");
    UtTest_Add(Test_BPLib_STOR_ScanCache_OneContactEgress, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_ScanCache_OneContactEgress");
    UtTest_Add(Test_BPLib_STOR_ScanCache_OneChannelEgress, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_ScanCache_OneChannelEgress");
    UtTest_Add(Test_BPLib_STOR_ScanCache_NoContact, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_ScanCache_NoContact");
    UtTest_Add(Test_BPLib_STOR_ScanCache_NoChannel, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_ScanCache_NoChannel");
    UtTest_Add(Test_BPLib_STOR_ScanCache_ErrAddJob, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_ScanCache_ErrAddJob");

    UtTest_Add(Test_BPLib_STOR_CacheBundle_Nominal, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_CacheBundle_Nominal");
}
