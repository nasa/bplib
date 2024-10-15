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


/*
** Function Definitions
*/


/* 
** Stub functions for callback testing
*/

int64_t BPA_TIMEP_GetMonotonicTime(void) { return 0; }
void BPA_TIMEP_GetHostEpoch(BPLib_TIME_Epoch_t *Epoch) { return; }
BPLib_TIME_ClockState_t BPA_TIMEP_GetHostClockState(void) { return BPLIB_TIME_CLOCK_VALID; }
int64_t BPA_TIMEP_GetHostTime(void) { return 0; }
BPLib_Status_t BPA_EVP_Init(void) { return 0; }
BPLib_Status_t BPA_EVP_SendEvent(uint16_t EventID, BPLib_EM_EventType_t EventType, char const* EventText) { return 0; }
int32_t BPA_TABLEP_SingleTableUpdate(int16_t TblHandle) { return 0; }
void BPA_PERFLOGP_Entry(uint32 PerfLogID) { return; }
void BPA_PERFLOGP_Exit(uint32 PerfLogID) { return; }
BPLib_Status_t BPA_ADUP_ValidateConfigTbl(void *TblData) { return BPLIB_SUCCESS; }
BPLib_Status_t BPA_ADUP_In(void *AduPtr, uint8_t ChanId) { return BPLIB_SUCCESS; }
BPLib_Status_t BPA_ADUP_Out(void *AduPtr, uint8_t ChanId) { return BPLIB_SUCCESS; }
BPLib_Status_t BPA_ADUP_AddApplication(uint8_t ChanId) { return BPLIB_SUCCESS; }
BPLib_Status_t BPA_ADUP_StartApplication(uint8_t ChanId) { return BPLIB_SUCCESS; }
BPLib_Status_t BPA_ADUP_StopApplication(uint8_t ChanId) { return BPLIB_SUCCESS; }

/* Test nominal FWP initialization */
void Test_BPLib_FWP_Init_Nominal(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    Callbacks.BPA_TIMEP_GetHostClockState  = BPA_TIMEP_GetHostClockState;
    Callbacks.BPA_TIMEP_GetHostEpoch       = BPA_TIMEP_GetHostEpoch;
    Callbacks.BPA_TIMEP_GetHostTime        = BPA_TIMEP_GetHostTime;
    Callbacks.BPA_TIMEP_GetMonotonicTime   = BPA_TIMEP_GetMonotonicTime;
    Callbacks.BPA_TABLEP_SingleTableUpdate = BPA_TABLEP_SingleTableUpdate;
    Callbacks.BPA_EVP_Init                 = BPA_EVP_Init;
    Callbacks.BPA_EVP_SendEvent            = BPA_EVP_SendEvent;
    Callbacks.BPA_PERFLOGP_Entry           = BPA_PERFLOGP_Entry;
    Callbacks.BPA_PERFLOGP_Exit            = BPA_PERFLOGP_Exit;
    Callbacks.BPA_ADUP_ValidateConfigTbl   = BPA_ADUP_ValidateConfigTbl;
    Callbacks.BPA_ADUP_In                  = BPA_ADUP_In;
    Callbacks.BPA_ADUP_Out                 = BPA_ADUP_Out;
    Callbacks.BPA_ADUP_AddApplication      = BPA_ADUP_AddApplication;
    Callbacks.BPA_ADUP_StartApplication    = BPA_ADUP_StartApplication;
    Callbacks.BPA_ADUP_StopApplication     = BPA_ADUP_StopApplication;

    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_SUCCESS);

    UtAssert_True(Callbacks.BPA_TIMEP_GetHostClockState == BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostClockState,
                    "Same BPA_TIMEP_GetHostClockState functions");
    UtAssert_True(Callbacks.BPA_TIMEP_GetHostEpoch == BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostEpoch,
                    "Same BPA_TIMEP_GetHostEpoch functions");
    UtAssert_True(Callbacks.BPA_TIMEP_GetHostTime == BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostTime,
                    "Same BPA_TIMEP_GetHostTime functions");
    UtAssert_True(Callbacks.BPA_TIMEP_GetMonotonicTime == BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime,
                    "Same BPA_TIMEP_GetMonotonicTime functions");
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
    UtAssert_True(Callbacks.BPA_ADUP_ValidateConfigTbl == BPLib_FWP_ProxyCallbacks.BPA_ADUP_ValidateConfigTbl,
                    "Same BPA_ADUP_ValidateConfigTbl functions");
    UtAssert_True(Callbacks.BPA_ADUP_In == BPLib_FWP_ProxyCallbacks.BPA_ADUP_In,
                    "Same BPA_ADUP_In functions");
    UtAssert_True(Callbacks.BPA_ADUP_Out == BPLib_FWP_ProxyCallbacks.BPA_ADUP_Out,
                    "Same BPA_ADUP_Out functions");
    UtAssert_True(Callbacks.BPA_ADUP_AddApplication == BPLib_FWP_ProxyCallbacks.BPA_ADUP_AddApplication,
                    "Same BPA_ADUP_AddApplication functions");
    UtAssert_True(Callbacks.BPA_ADUP_StartApplication == BPLib_FWP_ProxyCallbacks.BPA_ADUP_StartApplication,
                    "Same BPA_ADUP_StartApplication functions");
    UtAssert_True(Callbacks.BPA_ADUP_StopApplication == BPLib_FWP_ProxyCallbacks.BPA_ADUP_StopApplication,
                    "Same BPA_ADUP_StopApplication functions");
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

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_ADUP_ValidateConfigTblNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_ADUP_ValidateConfigTbl = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

void Test_BPLib_FWP_Init_ADUP_InNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_ADUP_In = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

void Test_BPLib_FWP_Init_ADUP_OutNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    memset(&Callbacks, 1, sizeof(BPLib_FWP_ProxyCallbacks_t));
    Callbacks.BPA_ADUP_Out = NULL;
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
}
