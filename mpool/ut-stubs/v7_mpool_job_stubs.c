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
 * Auto-Generated stub implementations for functions defined in v7_mpool_job header
 */

#include "v7_mpool_job.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_job_cast()
 * ----------------------------------------------------
 */
bplib_mpool_job_t *bplib_mpool_job_cast(bplib_mpool_block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_job_cast, bplib_mpool_job_t *);

    UT_GenStub_AddParam(bplib_mpool_job_cast, bplib_mpool_block_t *, cb);

    UT_GenStub_Execute(bplib_mpool_job_cast, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_job_cast, bplib_mpool_job_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_job_get_next_active()
 * ----------------------------------------------------
 */
bplib_mpool_job_t *bplib_mpool_job_get_next_active(bplib_mpool_t *pool)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_job_get_next_active, bplib_mpool_job_t *);

    UT_GenStub_AddParam(bplib_mpool_job_get_next_active, bplib_mpool_t *, pool);

    UT_GenStub_Execute(bplib_mpool_job_get_next_active, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_job_get_next_active, bplib_mpool_job_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_job_init()
 * ----------------------------------------------------
 */
void bplib_mpool_job_init(bplib_mpool_block_t *base_block, bplib_mpool_job_t *jblk)
{
    UT_GenStub_AddParam(bplib_mpool_job_init, bplib_mpool_block_t *, base_block);
    UT_GenStub_AddParam(bplib_mpool_job_init, bplib_mpool_job_t *, jblk);

    UT_GenStub_Execute(bplib_mpool_job_init, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_job_mark_active()
 * ----------------------------------------------------
 */
void bplib_mpool_job_mark_active(bplib_mpool_job_t *job)
{
    UT_GenStub_AddParam(bplib_mpool_job_mark_active, bplib_mpool_job_t *, job);

    UT_GenStub_Execute(bplib_mpool_job_mark_active, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_job_run_all()
 * ----------------------------------------------------
 */
void bplib_mpool_job_run_all(bplib_mpool_t *pool, void *arg)
{
    UT_GenStub_AddParam(bplib_mpool_job_run_all, bplib_mpool_t *, pool);
    UT_GenStub_AddParam(bplib_mpool_job_run_all, void *, arg);

    UT_GenStub_Execute(bplib_mpool_job_run_all, Basic, NULL);
}
