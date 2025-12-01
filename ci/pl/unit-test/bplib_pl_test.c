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

#include "bplib_pl_test_utils.h"
#include "bplib_pl.h"
#include "bpa_fwp_stubs.h"

BPLib_FWP_ProxyCallbacks_t BPLib_FWP_ProxyCallbacks;

/*
** Test function for
** int BPLib_PL_Init()
*/
void Test_BPLib_PL_Init(void)
{
    UtAssert_INT32_EQ(BPLib_PL_Init(), BPLIB_SUCCESS);
}

void Test_BPLib_PL_PerfLogEntryNominal(void)
{
    uint32_t PerfLogID = 10;
    BPLib_FWP_ProxyCallbacks.BPA_PERFLOGP_Entry = BPA_PERFLOGP_Entry;
    UtAssert_INT32_EQ(BPLib_PL_PerfLogEntry(PerfLogID), BPLIB_SUCCESS);    
}

void Test_BPLib_PL_PerfLogExitNominal(void)
{
    uint32_t PerfLogID = 10;
    BPLib_FWP_ProxyCallbacks.BPA_PERFLOGP_Exit = BPA_PERFLOGP_Exit;
    UtAssert_INT32_EQ(BPLib_PL_PerfLogExit(PerfLogID), BPLIB_SUCCESS);    
}

void Test_BPLib_PL_PerfLogEntryNULL(void)
{
    uint32_t PerfLogID = 10;
    memset(&BPLib_FWP_ProxyCallbacks, 0, sizeof(BPLib_FWP_ProxyCallbacks_t));
    UtAssert_INT32_EQ(BPLib_PL_PerfLogEntry(PerfLogID), BPLIB_PL_NULL_CALLBACK_ERROR);    
}

void Test_BPLib_PL_PerfLogExitNULL(void)
{
    uint32_t PerfLogID = 10;
    memset(&BPLib_FWP_ProxyCallbacks, 0, sizeof(BPLib_FWP_ProxyCallbacks_t));
    UtAssert_INT32_EQ(BPLib_PL_PerfLogExit(PerfLogID), BPLIB_PL_NULL_CALLBACK_ERROR);    
}

void TestBplibPl_Register(void)
{
    UtTest_Add(Test_BPLib_PL_Init, BPLib_PL_Test_Setup, BPLib_PL_Test_Teardown, "Test_BPLib_PL_Init");
    UtTest_Add(Test_BPLib_PL_PerfLogEntryNULL, BPLib_PL_Test_Setup, BPLib_PL_Test_Teardown, "Test_BPLib_PL_PerfLogEntryNULL");
    UtTest_Add(Test_BPLib_PL_PerfLogExitNULL, BPLib_PL_Test_Setup, BPLib_PL_Test_Teardown, "Test_BPLib_PL_PerfLogExitNULL");
    UtTest_Add(Test_BPLib_PL_PerfLogEntryNominal, BPLib_PL_Test_Setup, BPLib_PL_Test_Teardown, "Test_BPLib_PL_PerfLogEntryNominal");
    UtTest_Add(Test_BPLib_PL_PerfLogExitNominal, BPLib_PL_Test_Setup, BPLib_PL_Test_Teardown, "Test_BPLib_PL_PerfLogExitNominal");
}
