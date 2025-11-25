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

/**
 * @file
 *
 * Handlers for AS function stubs
 */

/* ======== */
/* Includes */
/* ======== */

#include "bplib_as_handlers.h"

/* ======= */
/* Globals */
/* ======= */

BPLib_AS_IncrementDecrementContext_t Context_BPLib_AS_Increment[UT_MAX_INCDEC_DEPTH];
BPLib_AS_IncrementDecrementContext_t Context_BPLib_AS_Decrement[UT_MAX_INCDEC_DEPTH];

/* ==================== */
/* Function Definitions */
/* ==================== */

void UT_Handler_BPLib_AS_Increment(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint16 CallCount;
    uint16 CallNum;

    CallCount = UT_GetStubCount(UT_KEY(BPLib_AS_Increment));

    if (CallCount > UT_MAX_INCDEC_DEPTH)
    {
        UtAssert_Failed("BPLib_AS_Increment call history depth exceeded. Called: %u, Max: %u",
                        CallCount,
                        UT_MAX_INCDEC_DEPTH);
    }
    else
    {
        CallNum = CallCount - 1;

        Context_BPLib_AS_Increment[CallNum].EID     = UT_Hook_GetArgValueByName(Context, "EID",     BPLib_EID_t);
        Context_BPLib_AS_Increment[CallNum].Counter = UT_Hook_GetArgValueByName(Context, "Counter", BPLib_AS_Counter_t);
        Context_BPLib_AS_Increment[CallNum].Amount  = UT_Hook_GetArgValueByName(Context, "Amount",  uint32_t);
    }
}

void UT_Handler_BPLib_AS_Decrement(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint16 CallCount;
    uint16 CallNum;

    CallCount = UT_GetStubCount(UT_KEY(BPLib_AS_Decrement));

    if (CallCount > UT_MAX_INCDEC_DEPTH)
    {
        UtAssert_Failed("BPLib_AS_Decrement call history depth exceeded. Called: %u, Max: %u",
                        CallCount,
                        UT_MAX_INCDEC_DEPTH);
    }
    else
    {
        CallNum = CallCount - 1;

        Context_BPLib_AS_Decrement[CallNum].EID     = UT_Hook_GetArgValueByName(Context, "EID",     BPLib_EID_t);
        Context_BPLib_AS_Decrement[CallNum].Counter = UT_Hook_GetArgValueByName(Context, "Counter", BPLib_AS_Counter_t);
        Context_BPLib_AS_Decrement[CallNum].Amount  = UT_Hook_GetArgValueByName(Context, "Amount",  uint32_t);
    }
}