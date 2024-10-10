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
 * Auto-Generated stub implementations for functions defined in bplib_stor_qm_job header
 */

#include "bplib_stor_qm_job.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_JobCast()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Job_t *BPLib_STOR_CACHE_JobCast(BPLib_STOR_CACHE_Block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_JobCast, BPLib_STOR_CACHE_Job_t *);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_JobCast, BPLib_STOR_CACHE_Block_t *, cb);

    UT_GenStub_Execute(BPLib_STOR_CACHE_JobCast, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_JobCast, BPLib_STOR_CACHE_Job_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_JobGetNextActive()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Job_t *BPLib_STOR_CACHE_JobGetNextActive(BPLib_STOR_CACHE_Pool_t *pool)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_JobGetNextActive, BPLib_STOR_CACHE_Job_t *);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_JobGetNextActive, BPLib_STOR_CACHE_Pool_t *, pool);

    UT_GenStub_Execute(BPLib_STOR_CACHE_JobGetNextActive, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_JobGetNextActive, BPLib_STOR_CACHE_Job_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_JobInit()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_JobInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Job_t *jblk)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_JobInit, BPLib_STOR_CACHE_Block_t *, base_block);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_JobInit, BPLib_STOR_CACHE_Job_t *, jblk);

    UT_GenStub_Execute(BPLib_STOR_CACHE_JobInit, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_JobMarkActive()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_JobMarkActive(BPLib_STOR_CACHE_Job_t *job)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_JobMarkActive, BPLib_STOR_CACHE_Job_t *, job);

    UT_GenStub_Execute(BPLib_STOR_CACHE_JobMarkActive, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_JobRunAll()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_JobRunAll(BPLib_STOR_CACHE_Pool_t *pool, void *arg)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_JobRunAll, BPLib_STOR_CACHE_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_JobRunAll, void *, arg);

    UT_GenStub_Execute(BPLib_STOR_CACHE_JobRunAll, Basic, NULL);
}
