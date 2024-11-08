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

BPA_EVP_SendEvent_context_t context_BPA_EVP_SendEvent;

void UT_Handler_BPA_EVP_SendEvent(void *UserObj, UT_EntryKey_t FuncKey, 
                                                        const UT_StubContext_t *Context)
{
    context_BPA_EVP_SendEvent.EventID   = UT_Hook_GetArgValueByName(Context, 
                                                                    "EventID", uint16_t);
    context_BPA_EVP_SendEvent.EventType = UT_Hook_GetArgValueByName(Context, 
                                                    "EventType", BPLib_EM_EventType_t);

    strncpy(context_BPA_EVP_SendEvent.Spec, UT_Hook_GetArgValueByName(Context, 
                                "EventText", char const *), BPLIB_EM_EXPANDED_EVENT_SIZE);
    context_BPA_EVP_SendEvent.Spec[BPLIB_EM_EXPANDED_EVENT_SIZE - 1] = '\0';
}

void BPLib_EM_Test_Setup(void)
{
    /* Initialize test environment to default state for every test */
    UT_ResetState(0);

    /* Set up proxy callback functions */
    BPLib_FWP_ProxyCallbacks.BPA_EVP_Init = BPA_EVP_Init;
    BPLib_FWP_ProxyCallbacks.BPA_EVP_SendEvent = BPA_EVP_SendEvent;

    UT_SetHandlerFunction(UT_KEY(BPA_EVP_SendEvent), UT_Handler_BPA_EVP_SendEvent, NULL);
}

void BPLib_EM_Test_Teardown(void)
{
    /* Clean up test environment */
}

void UtTest_Setup(void)
{
    TestBplibEm_Register();
}
