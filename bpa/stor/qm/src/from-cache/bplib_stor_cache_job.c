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
 * @brief Contains the memory pool implementation for blocks representing
 * flows of bundles (ingress and egress queue).
 *
 */

#include <string.h>
#include <assert.h>

#include "bplib_stor_cache_internal.h"

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Init
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_Init(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Job_t *jblk)
{
    BPLib_STOR_CACHE_InitSecondaryLink(base_block, &jblk->link, BPLib_STOR_CACHE_BlocktypeJob);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_Cast
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Job_t *BPLib_STOR_CACHE_Cast(BPLib_STOR_CACHE_Block_t *cb)
{
    if (cb != NULL && cb->type == BPLib_STOR_CACHE_BlocktypeJob)
    {
        return (BPLib_STOR_CACHE_Job_t *)cb;
    }

    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_JobCancelInternal
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_JobCancelInternal(BPLib_STOR_CACHE_Job_t *job)
{
    assert(job->link.type == BPLib_STOR_CACHE_BlocktypeJob);
    BPLib_STOR_CACHE_ExtractNode(&job->link);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_MarkActiveInternal
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_JobMarkActiveInternal(BPLib_STOR_CACHE_Block_t *active_list, BPLib_STOR_CACHE_Job_t *job)
{
    /* first cancel the job it if it was already active */
    /* this permits it to be marked as active multiple times, it will still only be in the runnable list once */
    BPLib_STOR_CACHE_JobCancelInternal(job);
    if (job->handler)
    {
        BPLib_STOR_CACHE_InsertBefore(active_list, &job->link);
    }
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_MarkActive
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_JobMarkActive(BPLib_STOR_CACHE_Job_t *job)
{
    BPLib_MEM_Lock_t                *lock;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;
    BPLib_STOR_CACHE_Pool_t                     *pool;

    pool  = BPLib_STOR_CACHE_GetParentPoolFromLink(&job->link);
    admin = BPLib_STOR_CACHE_GetAdmin(pool);

    lock = BPLib_STOR_CACHE_LockResource(pool);
    BPLib_STOR_CACHE_MarkActiveInternal(&admin->active_list, job);
    // TODO OSAL BPLib_STOR_CACHE_LockBroadcastSignal(lock);
    BPLib_STOR_CACHE_LockRelease(lock);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_GetNextActive
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Job_t *BPLib_STOR_CACHE_GetNextActive(BPLib_STOR_CACHE_Pool_t *pool)
{
    BPLib_STOR_CACHE_Job_t                 *job;
    BPLib_STOR_CACHE_Lock_t                *lock;
    BPLib_STOR_CACHE_Block_t               *jblk;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;

    admin = BPLib_STOR_CACHE_GetAdmin(pool);

    do
    {
        lock = BPLib_STOR_CACHE_LockResource(pool);

        /* if the head is reached here, then the list is empty */
        jblk = BPLib_STOR_CACHE_GetNextBlock(&admin->active_list);
        if (BPLib_STOR_CACHE_IsListHead(jblk))
        {
            jblk = NULL;
        }
        else
        {
            BPLib_STOR_CACHE_ExtractNode(jblk);
        }
        BPLib_STOR_CACHE_LockRelease(lock);

        job = BPLib_STOR_CACHE_Cast(jblk);
    }
    while (job == NULL && jblk != NULL);

    return job;
}

void BPLib_STOR_CACHE_RunAll(BPLib_STOR_CACHE_Pool_t *pool, void *arg)
{
    BPLib_STOR_CACHE_Job_t *job;

    /* forward any bundles between interfaces, based on active flow list */
    while (true)
    {
        job = BPLib_STOR_CACHE_GetNextActive(pool);
        if (job == NULL)
        {
            break;
        }

        /* note it may not be necessary to call this _every_ cycle, this could be
         * deferred to a lower priority task if necessary, but this will be sure to get it done */
        BPLib_STOR_CACHE_Maintain(pool);

        if (job->handler != NULL)
        {
            job->handler(arg, &job->link);
        }
    }
}
