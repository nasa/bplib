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

#include "bplib_cla_handlers.h"
#include "bplib_cla.h"

void UT_Handler_BPLib_CLA_GetContactRunState(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    BPLib_CLA_ContactRunState_t* ReturnState = UT_Hook_GetArgValueByName(Context, "ReturnState", BPLib_CLA_ContactRunState_t*);
    int32 Status;

    UT_Stub_GetInt32StatusCode(Context, &Status);

    if (Status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(BPLib_CLA_GetContactRunState), ReturnState, sizeof(BPLib_CLA_ContactRunState_t*));
    }
}

void UT_Handler_BPLib_CLA_Egress(void* UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t* Context)
{
    size_t* MsgSize = UT_Hook_GetArgValueByName(Context, "Size", size_t*);
    int32 Status;

    UT_Stub_GetInt32StatusCode(Context, &Status);
    if (Status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(BPLib_CLA_Egress), MsgSize, sizeof(size_t*));
    }
}