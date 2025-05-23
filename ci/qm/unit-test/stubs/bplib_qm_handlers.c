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
 * Handlers for QM function stubs
 */

#include "bplib_qm_handlers.h"

BPLib_QM_CreateJobContext_t Context_BPLib_QM_CreateJob[QM_MAX_INCDEC_DEPTH];
BPLib_QM_WaitQueuePushContext_t Context_BPLib_QM_WaitQueueTryPush[QM_MAX_INCDEC_DEPTH];

void UT_Handler_BPLib_QM_WaitQueueTryPull(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    void *ret_item = UT_Hook_GetArgValueByName(Context, "ret_item", void *);
    int32 Status;

    UT_Stub_GetInt32StatusCode(Context, &Status);

    if (Status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(BPLib_QM_WaitQueueTryPull), ret_item, sizeof(BPLib_Bundle_t *));
    }
}

void UT_Handler_BPLib_QM_DuctPull(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    BPLib_Bundle_t **RetBundle = UT_Hook_GetArgValueByName(Context, "RetBundle", BPLib_Bundle_t **);
    int32 Status;

    UT_Stub_GetInt32StatusCode(Context, &Status);

    if (Status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(BPLib_QM_DuctPull), RetBundle, sizeof(BPLib_Bundle_t **));
    }
}

void UT_Handler_BPLib_QM_WaitQueueTryPush(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint16 CallNum;
    uint16 CallCount;

    CallCount = UT_GetStubCount(UT_KEY(BPLib_QM_WaitQueueTryPush));
    if (CallCount > QM_MAX_INCDEC_DEPTH)
    {
        UtAssert_Failed("BPLib_QM_CreateJob call history depth exceeded. Called: %u, Max: %u",
                        CallCount,
                        QM_MAX_INCDEC_DEPTH);
    }
    else
    {
        CallNum = CallCount - 1;
        Context_BPLib_QM_WaitQueueTryPush[CallNum].Bundle = *UT_Hook_GetArgValueByName(Context, "item", BPLib_Bundle_t**);
    }
}

void UT_Handler_BPLib_QM_CreateJob(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint16 CallNum;
    uint16 CallCount;

    CallCount = UT_GetStubCount(UT_KEY(BPLib_QM_CreateJob));
    if (CallCount > QM_MAX_INCDEC_DEPTH)
    {
        UtAssert_Failed("BPLib_QM_CreateJob call history depth exceeded. Called: %u, Max: %u",
                        CallCount,
                        QM_MAX_INCDEC_DEPTH);
    }
    else
    {
        CallNum = CallCount - 1;
        Context_BPLib_QM_CreateJob[CallNum].State = UT_Hook_GetArgValueByName(Context, "state", BPLib_QM_JobState_t);
        Context_BPLib_QM_CreateJob[CallNum].Bundle = UT_Hook_GetArgValueByName(Context, "bundle", BPLib_Bundle_t*);
    }
}
