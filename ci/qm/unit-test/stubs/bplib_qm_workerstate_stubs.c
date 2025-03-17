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
 * Auto-Generated stub implementations for functions defined in bplib_qm_workerstate header
 */

#include "bplib_qm_workerstate.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QM_WorkerState_GiveNewJob()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_QM_WorkerState_GiveNewJob(BPLib_QM_WorkerState_t *WorkerState, BPLib_QM_Job_t *Job)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QM_WorkerState_GiveNewJob, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_QM_WorkerState_GiveNewJob, BPLib_QM_WorkerState_t *, WorkerState);
    UT_GenStub_AddParam(BPLib_QM_WorkerState_GiveNewJob, BPLib_QM_Job_t *, Job);

    UT_GenStub_Execute(BPLib_QM_WorkerState_GiveNewJob, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QM_WorkerState_GiveNewJob, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QM_WorkerState_Init()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_QM_WorkerState_Init(BPLib_QM_WorkerState_t *WorkerState, int WorkerID,
                                         BPLib_QM_WaitQueue_t *FreeWorkerQueue)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QM_WorkerState_Init, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_QM_WorkerState_Init, BPLib_QM_WorkerState_t *, WorkerState);
    UT_GenStub_AddParam(BPLib_QM_WorkerState_Init, int, WorkerID);
    UT_GenStub_AddParam(BPLib_QM_WorkerState_Init, BPLib_QM_WaitQueue_t *, FreeWorkerQueue);

    UT_GenStub_Execute(BPLib_QM_WorkerState_Init, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QM_WorkerState_Init, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QM_WorkerState_MarkJobDone()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_QM_WorkerState_MarkJobDone(BPLib_QM_WorkerState_t *WorkerState, BPLib_QM_JobState_t JobResult)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QM_WorkerState_MarkJobDone, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_QM_WorkerState_MarkJobDone, BPLib_QM_WorkerState_t *, WorkerState);
    UT_GenStub_AddParam(BPLib_QM_WorkerState_MarkJobDone, BPLib_QM_JobState_t, JobResult);

    UT_GenStub_Execute(BPLib_QM_WorkerState_MarkJobDone, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QM_WorkerState_MarkJobDone, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QM_WorkerState_WaitForNewJob()
 * ----------------------------------------------------
 */
bool BPLib_QM_WorkerState_WaitForNewJob(BPLib_QM_WorkerState_t *WorkerState, int TimeoutMs)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QM_WorkerState_WaitForNewJob, bool);

    UT_GenStub_AddParam(BPLib_QM_WorkerState_WaitForNewJob, BPLib_QM_WorkerState_t *, WorkerState);
    UT_GenStub_AddParam(BPLib_QM_WorkerState_WaitForNewJob, int, TimeoutMs);

    UT_GenStub_Execute(BPLib_QM_WorkerState_WaitForNewJob, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QM_WorkerState_WaitForNewJob, bool);
}
