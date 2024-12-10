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

#include "bplib_nc_test_utils.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

void BPLib_NC_Test_Verify_Event(uint16_t EventNum, int32_t EventID, const char* EventText)
{
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[EventNum].EventID, EventID);
    UtAssert_STRINGBUF_EQ(EventText, BPLIB_EM_EXPANDED_EVENT_SIZE,
                            context_BPLib_EM_SendEvent[EventNum].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
}

void BPLib_NC_Test_Setup(void)
{
    /* Initialize test environment to default state for every test */
    UT_ResetState(0);

    BPLib_FWP_ProxyCallbacks.BPA_ADUP_AddApplication             = BPA_ADUP_AddApplication;
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StartApplication           = BPA_ADUP_StartApplication;
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StopApplication            = BPA_ADUP_StopApplication;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibConfigPkt       = BPA_TLMP_SendNodeMibConfigPkt;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibConfigPkt  = BPA_TLMP_SendPerSourceMibConfigPkt;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendStoragePkt             = BPA_TLMP_SendStoragePkt;

    UT_SetHandlerFunction(UT_KEY(BPLib_EM_SendEvent), UT_Handler_BPLib_EM_SendEvent, NULL);
}

void BPLib_NC_Test_Teardown(void)
{
    /* Clean up test environment */
}

void UtTest_Setup(void)
{
    TestBplibNc_Register();
}
