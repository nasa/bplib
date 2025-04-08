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
#include "bplib_nc_test_utils.h"

/* Test nominal initialization */
void Test_BPLib_NC_RWLock_Init(void)
{
    UtAssert_INT32_EQ(BPLib_NC_RWLock_Init(&RWLock), BPLIB_SUCCESS);
}

/* Test for NULL pointer check */
void Test_BPLib_NC_RWLock_InitNull(void)
{
    UtAssert_INT32_EQ(BPLib_NC_RWLock_Init(NULL), BPLIB_NULL_PTR_ERROR);
}

/* Destroy destroy runs gracefully */
void Test_BPLib_NC_RWLock_Destroy(void)
{
    BPLib_NC_RWLock_Destroy(&RWLock);
    BPLib_NC_RWLock_Destroy(NULL);
}

/* Test RLock */
void Test_BPLib_NC_RWLock_RLock(void)
{
    RWLock.WriterCnt = 0;

    BPLib_NC_RWLock_RLock(&RWLock);
    UtAssert_INT32_EQ(RWLock.ReaderCnt, 1);
}

/* Test RUnlock */
void Test_BPLib_NC_RWLock_RUnlock(void)
{
    RWLock.ReaderCnt = 10;

    BPLib_NC_RWLock_RUnlock(&RWLock);
    UtAssert_INT32_EQ(RWLock.ReaderCnt, 9);
}

/* Test WLock */
void Test_BPLib_NC_RWLock_WLock(void)
{
    RWLock.WriterCnt = 0;
    RWLock.ReaderCnt = 0;

    BPLib_NC_RWLock_WLock(&RWLock);
    UtAssert_INT32_EQ(RWLock.WriterCnt, 1);
}

/* Test WUnlock */
void Test_BPLib_NC_RWLock_WUnlock(void)
{
    RWLock.WriterCnt = 1;

    BPLib_NC_RWLock_WUnlock(&RWLock);
    UtAssert_INT32_EQ(RWLock.WriterCnt, 0);
}


void TestBplibNcRwlock_Register()
{
    /* Init/Destroy */
    UtTest_Add(Test_BPLib_NC_RWLock_Init, BPLib_NC_Test_Setup, BPLib_NC_Test_Teardown, "Test_BPLib_NC_RWLock_Init");
    UtTest_Add(Test_BPLib_NC_RWLock_InitNull, BPLib_NC_Test_Setup, BPLib_NC_Test_Teardown, "Test_BPLib_NC_RWLock_InitNull");
    UtTest_Add(Test_BPLib_NC_RWLock_Destroy, BPLib_NC_Test_Setup, BPLib_NC_Test_Teardown, "Test_BPLib_NC_RWLock_Destroy");

    /* Lock/Unlock tests */
    UtTest_Add(Test_BPLib_NC_RWLock_RLock, BPLib_NC_Test_Setup_RWLock, BPLib_NC_Test_Teardown_RWLock, "Test_BPLib_NC_RWLock_RLock");
    UtTest_Add(Test_BPLib_NC_RWLock_RUnlock, BPLib_NC_Test_Setup_RWLock, BPLib_NC_Test_Teardown_RWLock, "Test_BPLib_NC_RWLock_RUnlock");
    UtTest_Add(Test_BPLib_NC_RWLock_WLock, BPLib_NC_Test_Setup_RWLock, BPLib_NC_Test_Teardown_RWLock, "Test_BPLib_NC_RWLock_WLock");
    UtTest_Add(Test_BPLib_NC_RWLock_WUnlock, BPLib_NC_Test_Setup_RWLock, BPLib_NC_Test_Teardown_RWLock, "Test_BPLib_NC_RWLock_WUnlock");
}
