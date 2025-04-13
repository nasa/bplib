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
 * Auto-Generated stub implementations for functions defined in bplib_qm header
 */

#include "bplib_qm.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QM_CreateJob()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_QM_CreateJob(BPLib_Instance_t *inst, BPLib_Bundle_t *bundle, BPLib_QM_JobState_t state,
                                  BPLib_QM_Priority_t priority, int TimeoutMs)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QM_CreateJob, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_QM_CreateJob, BPLib_Instance_t *, inst);
    UT_GenStub_AddParam(BPLib_QM_CreateJob, BPLib_Bundle_t *, bundle);
    UT_GenStub_AddParam(BPLib_QM_CreateJob, BPLib_QM_JobState_t, state);
    UT_GenStub_AddParam(BPLib_QM_CreateJob, BPLib_QM_Priority_t, priority);
    UT_GenStub_AddParam(BPLib_QM_CreateJob, int, TimeoutMs);

    UT_GenStub_Execute(BPLib_QM_CreateJob, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QM_CreateJob, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QM_QueueTableDestroy()
 * ----------------------------------------------------
 */
void BPLib_QM_QueueTableDestroy(BPLib_Instance_t *inst)
{
    UT_GenStub_AddParam(BPLib_QM_QueueTableDestroy, BPLib_Instance_t *, inst);

    UT_GenStub_Execute(BPLib_QM_QueueTableDestroy, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QM_QueueTableInit()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_QM_QueueTableInit(BPLib_Instance_t *inst, size_t MaxJobs)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QM_QueueTableInit, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_QM_QueueTableInit, BPLib_Instance_t *, inst);
    UT_GenStub_AddParam(BPLib_QM_QueueTableInit, size_t, MaxJobs);

    UT_GenStub_Execute(BPLib_QM_QueueTableInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QM_QueueTableInit, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QM_RegisterWorker()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_QM_RegisterWorker(BPLib_Instance_t *inst, int *WorkerID)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QM_RegisterWorker, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_QM_RegisterWorker, BPLib_Instance_t *, inst);
    UT_GenStub_AddParam(BPLib_QM_RegisterWorker, int *, WorkerID);

    UT_GenStub_Execute(BPLib_QM_RegisterWorker, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QM_RegisterWorker, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QM_WorkerRunJob()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_QM_WorkerRunJob(BPLib_Instance_t *inst, int WorkerID, int TimeoutMs)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QM_WorkerRunJob, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_QM_WorkerRunJob, BPLib_Instance_t *, inst);
    UT_GenStub_AddParam(BPLib_QM_WorkerRunJob, int, WorkerID);
    UT_GenStub_AddParam(BPLib_QM_WorkerRunJob, int, TimeoutMs);

    UT_GenStub_Execute(BPLib_QM_WorkerRunJob, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QM_WorkerRunJob, BPLib_Status_t);
}
