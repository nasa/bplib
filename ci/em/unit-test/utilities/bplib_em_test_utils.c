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

#include "bplib_em_test_utils.h"

/* ========================= */
/* Stub Function Definitions */
/* ========================= */

BPLib_Status_t Test_BPA_EVP_Init(void)
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t Test_BPA_EVP_SendEvent(uint16_t EventID, BPLib_EM_EventType_t EventType, char const* EventText)
{
    return BPLIB_SUCCESS;
}

void BPLib_EM_Test_Setup(void)
{
    /* Initialize test environment to default state for every test */
    UT_ResetState(0);

    /* Set up proxy callback functions */
    BPLib_FWP_ProxyCallbacks.BPA_EVP_Init = Test_BPA_EVP_Init;
    BPLib_FWP_ProxyCallbacks.BPA_EVP_SendEvent = Test_BPA_EVP_SendEvent;
}

void BPLib_EM_Test_Teardown(void)
{
    /* Clean up test environment */
}

void UtTest_Setup(void)
{
    TestBplibEm_Register();
}
