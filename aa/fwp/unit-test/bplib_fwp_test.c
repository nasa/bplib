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

/**
 * \file
 *  Unit tests for bplib_fwp.c
 */

/*
** Include Files
*/

#include "bplib_fwp_test_utils.h"
#include "bplib_time.h"
#include "bplib_em.h"
#include "bplib_nc_payloads.h"
#include "bpa_fwp_stubs.h"  /* For stub definitions of callbacks */


/*
** Function Definitions
*/

/* Test nominal FWP initialization */
void Test_BPLib_FWP_Init_Nominal(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    Callbacks.BPA_TIMEP_GetHostClockState         = BPA_TIMEP_GetHostClockState;
    Callbacks.BPA_TIMEP_GetHostEpoch              = BPA_TIMEP_GetHostEpoch;
    Callbacks.BPA_TIMEP_GetHostTime               = BPA_TIMEP_GetHostTime;
    Callbacks.BPA_TIMEP_GetMonotonicTime          = BPA_TIMEP_GetMonotonicTime;
    Callbacks.BPA_PERFLOGP_Entry                  = BPA_PERFLOGP_Entry;
    Callbacks.BPA_PERFLOGP_Exit                   = BPA_PERFLOGP_Exit;
    Callbacks.BPA_TABLEP_SingleTableUpdate        = BPA_TABLEP_SingleTableUpdate;
    Callbacks.BPA_EVP_Init                        = BPA_EVP_Init;
    Callbacks.BPA_EVP_SendEvent                   = BPA_EVP_SendEvent;
    Callbacks.BPA_PERFLOGP_Entry                  = BPA_PERFLOGP_Entry;
    Callbacks.BPA_PERFLOGP_Exit                   = BPA_PERFLOGP_Exit;
    Callbacks.BPA_ADUP_AddApplication             = BPA_ADUP_AddApplication;
    Callbacks.BPA_ADUP_StartApplication           = BPA_ADUP_StartApplication;
    Callbacks.BPA_ADUP_StopApplication            = BPA_ADUP_StopApplication;
    Callbacks.BPA_ADUP_RemoveApplication          = BPA_ADUP_RemoveApplication;
    Callbacks.BPA_TLMP_SendChannelContactPkt      = BPA_TLMP_SendChannelContactPkt;
    Callbacks.BPA_TLMP_SendNodeMibConfigPkt       = BPA_TLMP_SendNodeMibConfigPkt;
    Callbacks.BPA_TLMP_SendNodeMibCounterPkt      = BPA_TLMP_SendNodeMibCounterPkt;
    Callbacks.BPA_TLMP_SendPerSourceMibConfigPkt  = BPA_TLMP_SendPerSourceMibConfigPkt;
    Callbacks.BPA_TLMP_SendPerSourceMibCounterPkt = BPA_TLMP_SendPerSourceMibCounterPkt;
    Callbacks.BPA_TLMP_SendStoragePkt             = BPA_TLMP_SendStoragePkt;
    
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_SUCCESS);

    UtAssert_True(Callbacks.BPA_TIMEP_GetHostClockState == BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostClockState,
                    "Same BPA_TIMEP_GetHostClockState functions");
    UtAssert_True(Callbacks.BPA_TIMEP_GetHostEpoch == BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostEpoch,
                    "Same BPA_TIMEP_GetHostEpoch functions");
    UtAssert_True(Callbacks.BPA_TIMEP_GetHostTime == BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostTime,
                    "Same BPA_TIMEP_GetHostTime functions");
    UtAssert_True(Callbacks.BPA_TIMEP_GetMonotonicTime == BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime,
                    "Same BPA_TIMEP_GetMonotonicTime functions");
    UtAssert_True(Callbacks.BPA_PERFLOGP_Entry == BPLib_FWP_ProxyCallbacks.BPA_PERFLOGP_Entry,
                    "Same BPA_PERFLOGP_Entry functions");
    UtAssert_True(Callbacks.BPA_PERFLOGP_Exit == BPLib_FWP_ProxyCallbacks.BPA_PERFLOGP_Exit,
                    "Same BPA_PERFLOGP_Exit functions");
    UtAssert_True(Callbacks.BPA_TABLEP_SingleTableUpdate == BPLib_FWP_ProxyCallbacks.BPA_TABLEP_SingleTableUpdate,
                    "Same BPA_TABLEP_SingleTableUpdate functions");
    UtAssert_True(Callbacks.BPA_EVP_Init == BPLib_FWP_ProxyCallbacks.BPA_EVP_Init,
                    "Same BPA_EVP_Init function");
    UtAssert_True(Callbacks.BPA_EVP_SendEvent == BPLib_FWP_ProxyCallbacks.BPA_EVP_SendEvent,
                    "Same BPA_EVP_SendEvent function");
    UtAssert_True(Callbacks.BPA_PERFLOGP_Entry == BPLib_FWP_ProxyCallbacks.BPA_PERFLOGP_Entry, 
                    "Same BPA_PERFLOGP_Entry functions");
    UtAssert_True(Callbacks.BPA_PERFLOGP_Exit == BPLib_FWP_ProxyCallbacks.BPA_PERFLOGP_Exit, 
                    "Same BPA_PERFLOGP_Exit functions");
    UtAssert_True(Callbacks.BPA_ADUP_AddApplication == BPLib_FWP_ProxyCallbacks.BPA_ADUP_AddApplication,
                    "Same BPA_ADUP_AddApplication functions");
    UtAssert_True(Callbacks.BPA_ADUP_StartApplication == BPLib_FWP_ProxyCallbacks.BPA_ADUP_StartApplication,
                    "Same BPA_ADUP_StartApplication functions");
    UtAssert_True(Callbacks.BPA_ADUP_StopApplication == BPLib_FWP_ProxyCallbacks.BPA_ADUP_StopApplication,
                    "Same BPA_ADUP_StopApplication functions");
    UtAssert_True(Callbacks.BPA_ADUP_RemoveApplication == BPLib_FWP_ProxyCallbacks.BPA_ADUP_RemoveApplication,
                    "Same BPA_ADUP_RemoveApplication functions");
    UtAssert_True(Callbacks.BPA_TLMP_SendChannelContactPkt == BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendChannelContactPkt, 
                    "Same BPA_TLMP_SendChannelContactPkt functions");
    UtAssert_True(Callbacks.BPA_TLMP_SendNodeMibConfigPkt == BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibConfigPkt, 
                    "Same BPA_TLMP_SendNodeMibConfigPkt functions");
    UtAssert_True(Callbacks.BPA_TLMP_SendNodeMibCounterPkt == BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibCounterPkt, 
                    "Same BPA_TLMP_SendNodeMibCounterPkt functions");
    UtAssert_True(Callbacks.BPA_TLMP_SendPerSourceMibConfigPkt == BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibConfigPkt, 
                    "Same BPA_TLMP_SendPerSourceMibConfigPkt functions");
    UtAssert_True(Callbacks.BPA_TLMP_SendPerSourceMibCounterPkt == BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibCounterPkt, 
                    "Same BPA_TLMP_SendPerSourceMibCounterPkt functions");
    UtAssert_True(Callbacks.BPA_TLMP_SendStoragePkt == BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendStoragePkt, 
                    "Same BPA_TLMP_SendStoragePkt functions");
    
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_GetHostClockStateNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_TIMEP_GetHostClockState = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_GetHostEpochNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_TIMEP_GetHostEpoch = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_GetHostTimeNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_TIMEP_GetHostTime = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_GetMonotonicTimeNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_TIMEP_GetMonotonicTime = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_SingleTableUpdateNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_TABLEP_SingleTableUpdate = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

void Test_BPLib_FWP_Init_EVP_InitNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_EVP_Init = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

void Test_BPLib_FWP_Init_EVP_SendEventNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_EVP_SendEvent = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_PERFLOGP_EntryNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_PERFLOGP_Entry = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_PERFLOGP_ExitNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_PERFLOGP_Exit = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

void Test_BPLib_FWP_Init_ADUP_AddApplicationNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_ADUP_AddApplication = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

void Test_BPLib_FWP_Init_ADUP_StartApplicationNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_ADUP_StartApplication = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

void Test_BPLib_FWP_Init_ADUP_StopApplicationNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_ADUP_StopApplication = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

void Test_BPLib_FWP_Init_ADUP_RemoveApplicationNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_ADUP_RemoveApplication = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_SendChannelContactPktNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_TLMP_SendChannelContactPkt = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_SendNodeMibConfigPktNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_TLMP_SendNodeMibConfigPkt = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_SendNodeMibCounterPktNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_TLMP_SendNodeMibCounterPkt = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_SendPerSourceMibConfigPktNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_TLMP_SendPerSourceMibConfigPkt = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_SendPerSourceMibCounterPktNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_TLMP_SendPerSourceMibCounterPkt = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_SendStoragePktNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_TLMP_SendStoragePkt = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

void TestBplibFwp_Register(void)
{
    UtTest_Add(Test_BPLib_FWP_Init_Nominal, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_Nominal");
    UtTest_Add(Test_BPLib_FWP_Init_GetHostClockStateNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_GetHostClockStateNull");
    UtTest_Add(Test_BPLib_FWP_Init_GetHostEpochNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_GetHostEpochNull");
    UtTest_Add(Test_BPLib_FWP_Init_GetHostTimeNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_GetHostTimeNull");
    UtTest_Add(Test_BPLib_FWP_Init_GetMonotonicTimeNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_GetMonotonicTimeNull");
    UtTest_Add(Test_BPLib_FWP_Init_SingleTableUpdateNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_SingleTableUpdateNull");
    UtTest_Add(Test_BPLib_FWP_Init_EVP_InitNull,  BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_EVP_InitNull");
    UtTest_Add(Test_BPLib_FWP_Init_EVP_SendEventNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_EVP_SendEventNull");
    UtTest_Add(Test_BPLib_FWP_Init_PERFLOGP_EntryNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_PERFLOGP_EntryNull");
    UtTest_Add(Test_BPLib_FWP_Init_PERFLOGP_ExitNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_PERFLOGP_ExitNull");
    UtTest_Add(Test_BPLib_FWP_Init_ADUP_AddApplicationNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_ADUP_AddApplicationNull");
    UtTest_Add(Test_BPLib_FWP_Init_ADUP_StartApplicationNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_ADUP_StartApplicationNull");
    UtTest_Add(Test_BPLib_FWP_Init_ADUP_StopApplicationNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_ADUP_StopApplicationNull");
    UtTest_Add(Test_BPLib_FWP_Init_ADUP_RemoveApplicationNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_ADUP_RemoveApplicationNull");
    UtTest_Add(Test_BPLib_FWP_Init_SendChannelContactPktNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_SendChannelContactPktNull");
    UtTest_Add(Test_BPLib_FWP_Init_SendNodeMibConfigPktNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_SendNodeMibConfigPktNull");
    UtTest_Add(Test_BPLib_FWP_Init_SendNodeMibCounterPktNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_SendNodeMibCounterPktNull");
    UtTest_Add(Test_BPLib_FWP_Init_SendPerSourceMibConfigPktNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_SendPerSourceMibConfigPktNull");
    UtTest_Add(Test_BPLib_FWP_Init_SendPerSourceMibCounterPktNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_SendPerSourceMibCounterPktNull");
    UtTest_Add(Test_BPLib_FWP_Init_SendStoragePktNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_SendStoragePktNull");
}
