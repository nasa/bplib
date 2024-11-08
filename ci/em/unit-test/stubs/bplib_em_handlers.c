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
 * @file
 *
 * Handlers for EM function stubs
 */

#include "bplib_em_handlers.h"


BPLib_EM_SendEvent_context_t context_BPLib_EM_SendEvent[UT_MAX_SENDEVENT_DEPTH];


void UT_Handler_BPLib_EM_SendEvent(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint16 CallCount;
    uint16 idx;

    CallCount = UT_GetStubCount(UT_KEY(BPLib_EM_SendEvent));

    if (CallCount > (sizeof(context_BPLib_EM_SendEvent) / sizeof(context_BPLib_EM_SendEvent[0])))
    {
        UtAssert_Failed("BPLib_EM_SendEvent UT depth %u exceeded: %u, increase UT_MAX_SENDEVENT_DEPTH",
                        UT_MAX_SENDEVENT_DEPTH, CallCount);
    }
    else
    {
        idx                                      = CallCount - 1;
        context_BPLib_EM_SendEvent[idx].EventID   = UT_Hook_GetArgValueByName(Context, "EventID", uint16);
        context_BPLib_EM_SendEvent[idx].EventType = UT_Hook_GetArgValueByName(Context, "EventType", BPLib_EM_EventType_t);

        strncpy(context_BPLib_EM_SendEvent[idx].Spec, UT_Hook_GetArgValueByName(Context, "Spec", const char *),
                BPLIB_EM_EXPANDED_EVENT_SIZE);
        context_BPLib_EM_SendEvent[idx].Spec[BPLIB_EM_EXPANDED_EVENT_SIZE - 1] = '\0';
    }
}
