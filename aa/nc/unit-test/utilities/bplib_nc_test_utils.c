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
    /* Confirm the issuing function was called */
    UtAssert_STUB_COUNT(BPLib_EM_SendEvent, EventNum + 1);

    /* Check the string */
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[EventNum].EventID, EventID);
    UtAssert_STRINGBUF_EQ(EventText, BPLIB_EM_EXPANDED_EVENT_SIZE,
                            context_BPLib_EM_SendEvent[EventNum].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
}

void Test_BPLib_NC_VerifyIncrement(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Amount, int16_t CallNum)
{
    UtAssert_STUB_COUNT(BPLib_AS_Increment, CallNum);

    if (SourceEid != -1)
    {
        UtAssert_EQ(int16_t, SourceEid, Context_BPLib_AS_Increment[CallNum - 1].SourceEid);
    }

    if (Counter != -1)
    {
        UtAssert_EQ(BPLib_AS_Counter_t, Counter, Context_BPLib_AS_Increment[CallNum - 1].Counter);
    }

    if (Amount != 0xFFFFFFFF)
    {
        UtAssert_EQ(uint32_t, Amount, Context_BPLib_AS_Increment[CallNum - 1].Amount);
    }
}

void Test_BPLib_NC_VerifyDecrement(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Amount, int16_t CallNum)
{
    UtAssert_STUB_COUNT(BPLib_AS_Decrement, CallNum);

    if (SourceEid != -1)
    {
        UtAssert_EQ(uint16_t, SourceEid, Context_BPLib_AS_Decrement[CallNum - 1].SourceEid);
    }

    if (Counter != -1)
    {
        UtAssert_EQ(BPLib_AS_Counter_t, Counter, Context_BPLib_AS_Decrement[CallNum - 1].Counter);
    }

    if (Amount != 0xFFFFFFFF)
    {
        UtAssert_EQ(uint32_t, Amount, Context_BPLib_AS_Decrement[CallNum - 1].Amount);
    }
}

void BPLib_NC_Test_Setup(void)
{
    /* Initialize test environment to default state for every test */
    UT_ResetState(0);

    BPLib_FWP_ProxyCallbacks.BPA_ADUP_AddApplication             = BPA_ADUP_AddApplication;
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_RemoveApplication          = BPA_ADUP_RemoveApplication;
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StartApplication           = BPA_ADUP_StartApplication;
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StopApplication            = BPA_ADUP_StopApplication;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibConfigPkt       = BPA_TLMP_SendNodeMibConfigPkt;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibConfigPkt  = BPA_TLMP_SendPerSourceMibConfigPkt;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendChannelContactPkt      = BPA_TLMP_SendChannelContactPkt;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendStoragePkt             = BPA_TLMP_SendStoragePkt;

    UT_SetHandlerFunction(UT_KEY(BPLib_EM_SendEvent), UT_Handler_BPLib_EM_SendEvent, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_AS_Increment), UT_Handler_BPLib_AS_Increment, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_AS_Decrement), UT_Handler_BPLib_AS_Decrement, NULL);
}

void BPLib_NC_Test_Teardown(void)
{
    /* Clean up test environment */
    int8_t ContextNum;

    for (ContextNum = 0; ContextNum < UT_MAX_SENDEVENT_DEPTH; ContextNum++)
    {
        context_BPLib_EM_SendEvent[ContextNum].EventID   = 0;
        context_BPLib_EM_SendEvent[ContextNum].EventType = BPLib_EM_EventType_UNKNOWN;
        memset(&context_BPLib_EM_SendEvent[ContextNum].Spec, 0, sizeof(context_BPLib_EM_SendEvent[ContextNum].Spec));
    }
}

void UtTest_Setup(void)
{
    TestBplibNc_Register();
}
