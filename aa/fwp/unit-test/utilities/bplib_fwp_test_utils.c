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

/* ======== */
/* Includes */
/* ======== */

#include "bplib_fwp_test_utils.h"

/* =========== */
/* Global Data */
/* =========== */

BPLib_FWP_ProxyCallbacks_t TestCallbacks;

/* ==================== */
/* Function Definitions */
/* ==================== */

void BPLib_FWP_Test_Setup(void)
{
    memset((void*) &TestCallbacks,  0, sizeof(BPLib_FWP_ProxyCallbacks_t));

    TestCallbacks.BPA_TIMEP_GetHostClockState         = BPA_TIMEP_GetHostClockState;
    TestCallbacks.BPA_TIMEP_GetHostEpoch              = BPA_TIMEP_GetHostEpoch;
    TestCallbacks.BPA_TIMEP_GetHostTime               = BPA_TIMEP_GetHostTime;
    TestCallbacks.BPA_TIMEP_GetMonotonicTime          = BPA_TIMEP_GetMonotonicTime;
    TestCallbacks.BPA_PERFLOGP_Entry                  = BPA_PERFLOGP_Entry;
    TestCallbacks.BPA_PERFLOGP_Exit                   = BPA_PERFLOGP_Exit;
    TestCallbacks.BPA_EVP_Init                        = BPA_EVP_Init;
    TestCallbacks.BPA_EVP_SendEvent                   = BPA_EVP_SendEvent;
    TestCallbacks.BPA_PERFLOGP_Entry                  = BPA_PERFLOGP_Entry;
    TestCallbacks.BPA_PERFLOGP_Exit                   = BPA_PERFLOGP_Exit;
    TestCallbacks.BPA_ADUP_AddApplication             = BPA_ADUP_AddApplication;
    TestCallbacks.BPA_ADUP_StartApplication           = BPA_ADUP_StartApplication;
    TestCallbacks.BPA_ADUP_StopApplication            = BPA_ADUP_StopApplication;
    TestCallbacks.BPA_ADUP_RemoveApplication          = BPA_ADUP_RemoveApplication;
    TestCallbacks.BPA_TLMP_SendChannelContactPkt      = BPA_TLMP_SendChannelContactPkt;
    TestCallbacks.BPA_TLMP_SendNodeMibConfigPkt       = BPA_TLMP_SendNodeMibConfigPkt;
    TestCallbacks.BPA_TLMP_SendNodeMibCounterPkt      = BPA_TLMP_SendNodeMibCounterPkt;
    TestCallbacks.BPA_TLMP_SendPerSourceMibConfigPkt  = BPA_TLMP_SendPerSourceMibConfigPkt;
    TestCallbacks.BPA_TLMP_SendPerSourceMibCounterPkt = BPA_TLMP_SendPerSourceMibCounterPkt;
    TestCallbacks.BPA_TLMP_SendStoragePkt             = BPA_TLMP_SendStoragePkt;
    TestCallbacks.BPA_TABLEP_TableUpdate              = BPA_TABLEP_TableUpdate;
    TestCallbacks.BPA_CLAP_ContactSetup               = BPA_CLAP_ContactSetup;
    TestCallbacks.BPA_CLAP_ContactStart               = BPA_CLAP_ContactStart;
    TestCallbacks.BPA_CLAP_ContactStop                = BPA_CLAP_ContactStop;
    TestCallbacks.BPA_CLAP_ContactTeardown            = BPA_CLAP_ContactTeardown;

    /* Initialize test environment to default state for every test */
    UT_ResetState(0);
}

void BPLib_FWP_Test_Teardown(void)
{
    /* Clean up test environment */
}

void UtTest_Setup(void)
{
    TestBplibFwp_Register();
}
