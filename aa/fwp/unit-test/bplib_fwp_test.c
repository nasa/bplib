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
#include "fwp_timep.h"


/*
** Function Definitions
*/

/* Test nominal FWP initialization */
void Test_BPLib_FWP_Init_Nominal(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    Callbacks.BPA_TIMEP_GetHostClockState = BPA_TIMEP_GetHostClockState;
    Callbacks.BPA_TIMEP_GetHostEpoch = BPA_TIMEP_GetHostEpoch;
    Callbacks.BPA_TIMEP_GetHostTime = BPA_TIMEP_GetHostTime;
    Callbacks.BPA_TIMEP_GetMonotonicTime = BPA_TIMEP_GetMonotonicTime;

    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_SUCCESS);

    UtAssert_ADDRESS_EQ(Callbacks.BPA_TIMEP_GetHostClockState, BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostClockState);
    UtAssert_ADDRESS_EQ(Callbacks.BPA_TIMEP_GetHostEpoch, BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostEpoch);
    UtAssert_ADDRESS_EQ(Callbacks.BPA_TIMEP_GetHostTime, BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostTime);
    UtAssert_ADDRESS_EQ(Callbacks.BPA_TIMEP_GetMonotonicTime, BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_GetHostClockStateNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    Callbacks.BPA_TIMEP_GetHostClockState = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_GetHostEpochNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    Callbacks.BPA_TIMEP_GetHostEpoch = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_GetHostTimeNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    Callbacks.BPA_TIMEP_GetHostTime = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}

/* Test FWP initialization with null function */
void Test_BPLib_FWP_Init_GetMonotonicTimeNull(void)
{
    BPLib_FWP_ProxyCallbacks_t Callbacks;

    Callbacks.BPA_TIMEP_GetMonotonicTime = NULL;
    UtAssert_INT32_EQ(BPLib_FWP_Init(Callbacks), BPLIB_FWP_CALLBACK_INIT_ERROR);
}


void TestBplibFwp_Register(void)
{
    UtTest_Add(Test_BPLib_FWP_Init_Nominal, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_Nominal");
    UtTest_Add(Test_BPLib_FWP_Init_GetHostClockStateNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_GetHostClockStateNull");
    UtTest_Add(Test_BPLib_FWP_Init_GetHostEpochNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_GetHostEpochNull");
    UtTest_Add(Test_BPLib_FWP_Init_GetHostTimeNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_GetHostTimeNull");
    UtTest_Add(Test_BPLib_FWP_Init_GetMonotonicTimeNull, BPLib_FWP_Test_Setup, BPLib_FWP_Test_Teardown, "Test_BPLib_FWP_Init_GetMonotonicTimeNull");
}
