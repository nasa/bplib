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
 * Auto-Generated stub implementations for functions defined in bplib_qm_waitqueue header
 */

#include "bplib_qm_waitqueue.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QM_WaitQueueDestroy()
 * ----------------------------------------------------
 */
void BPLib_QM_WaitQueueDestroy(BPLib_QM_WaitQueue_t *q)
{
    UT_GenStub_AddParam(BPLib_QM_WaitQueueDestroy, BPLib_QM_WaitQueue_t *, q);

    UT_GenStub_Execute(BPLib_QM_WaitQueueDestroy, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QM_WaitQueueInit()
 * ----------------------------------------------------
 */
bool BPLib_QM_WaitQueueInit(BPLib_QM_WaitQueue_t *q, void *storage, size_t el_size, size_t capacity)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QM_WaitQueueInit, bool);

    UT_GenStub_AddParam(BPLib_QM_WaitQueueInit, BPLib_QM_WaitQueue_t *, q);
    UT_GenStub_AddParam(BPLib_QM_WaitQueueInit, void *, storage);
    UT_GenStub_AddParam(BPLib_QM_WaitQueueInit, size_t, el_size);
    UT_GenStub_AddParam(BPLib_QM_WaitQueueInit, size_t, capacity);

    UT_GenStub_Execute(BPLib_QM_WaitQueueInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QM_WaitQueueInit, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QM_WaitQueueTryPull()
 * ----------------------------------------------------
 */
bool BPLib_QM_WaitQueueTryPull(BPLib_QM_WaitQueue_t *q, void *ret_item, int timeout_ms)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QM_WaitQueueTryPull, bool);

    UT_GenStub_AddParam(BPLib_QM_WaitQueueTryPull, BPLib_QM_WaitQueue_t *, q);
    UT_GenStub_AddParam(BPLib_QM_WaitQueueTryPull, void *, ret_item);
    UT_GenStub_AddParam(BPLib_QM_WaitQueueTryPull, int, timeout_ms);

    UT_GenStub_Execute(BPLib_QM_WaitQueueTryPull, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QM_WaitQueueTryPull, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QM_WaitQueueTryPush()
 * ----------------------------------------------------
 */
bool BPLib_QM_WaitQueueTryPush(BPLib_QM_WaitQueue_t *q, void *item, int timeout_ms)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QM_WaitQueueTryPush, bool);

    UT_GenStub_AddParam(BPLib_QM_WaitQueueTryPush, BPLib_QM_WaitQueue_t *, q);
    UT_GenStub_AddParam(BPLib_QM_WaitQueueTryPush, void *, item);
    UT_GenStub_AddParam(BPLib_QM_WaitQueueTryPush, int, timeout_ms);

    UT_GenStub_Execute(BPLib_QM_WaitQueueTryPush, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QM_WaitQueueTryPush, bool);
}
