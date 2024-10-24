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

#include <assert.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * 
 * bplib_stor_cache_internal.h contains declarations for all STOR CACHE
 * internal data structures and functions. bplib_stor_cache_internal.c has a subset.
 */

#include "bplib_stor_cache_internal.h"

void BPLib_STOR_CACHE_SubqWorkitemInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_SubqWorkitem_t *wblk)
{
    BPLib_STOR_CACHE_JobInit(base_block, &wblk->job_header);
    BPLib_STOR_CACHE_SubqInit(base_block, &wblk->base_subq);
    wblk->current_depth_limit = 0;
}

static BPLib_Status_t BPLib_STOR_CACHE_DuctEventHandler(void *arg, BPLib_STOR_CACHE_Block_t *jblk)
{
    BPLib_STOR_CACHE_Block_t                *fblk;
    BPLib_STOR_CACHE_Duct_t            *duct;
    uint32_t                          changed_flags;
    BPLib_STOR_CACHE_DuctGenericEvent_t event;
    bool                              was_running;
    bool                              is_running;

    fblk = BPLib_STOR_CACHE_GetBlockFromLink(jblk);
    duct = BPLib_STOR_CACHE_DuctCast(fblk);
    if (duct == NULL)
    {
        return BPLIB_ERROR;
    }

    was_running   = BPLib_STOR_CACHE_DuctIsUp(duct);
    changed_flags = duct->pending_state_flags ^ duct->current_state_flags;
    duct->current_state_flags ^= changed_flags;
    is_running = BPLib_STOR_CACHE_DuctIsUp(duct);

    /* detect changes from up->down or vice versa */
    /* this is the combination of several flags, so its not simply checking changed_flags */
    if (was_running != is_running)
    {
        if (is_running)
        {
            event.intf_state.event_type = BPLib_STOR_CACHE_DuctEventUp;
        }
        else
        {
            event.intf_state.event_type = BPLib_STOR_CACHE_DuctEventDown;
        }

        event.intf_state.intf_id = BPLib_STOR_CACHE_GetExternalId(fblk);
        duct->statechange_job.event_handler(&event, fblk);
    }

    if (changed_flags & BPLIB_CACHE_STATE_FLAG_POLL)
    {
        event.event_type = BPLib_STOR_CACHE_DuctEventPoll;
        duct->statechange_job.event_handler(&event, fblk);
    }

    return BPLIB_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_DuctInit
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_DuctInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Duct_t *fblk)
{
    /* now init the link structs */
    BPLib_STOR_CACHE_JobInit(base_block, &fblk->statechange_job.base_job);
    fblk->statechange_job.base_job.handler = BPLib_STOR_CACHE_DuctEventHandler;

    BPLib_STOR_CACHE_SubqWorkitemInit(base_block, &fblk->ingress);
    BPLib_STOR_CACHE_SubqWorkitemInit(base_block, &fblk->egress);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_JobInit
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_JobInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Job_t *jblk)
{
    BPLib_STOR_CACHE_InitSecondaryLink(base_block, &jblk->link, BPLib_STOR_CACHE_BlocktypeJob);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_job_cast
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Job_t *BPLib_STOR_CACHE_JobCast(BPLib_STOR_CACHE_Block_t *cb)
{
    if (cb != NULL && cb->type == BPLib_STOR_CACHE_BlocktypeJob)
    {
        return (BPLib_STOR_CACHE_Job_t *)cb;
    }

    return NULL;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_job_cancel_internal
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_JobCancelInternal(BPLib_STOR_CACHE_Job_t *job)
{
    assert(job->link.type == BPLib_STOR_CACHE_BlocktypeJob);
    BPLib_STOR_CACHE_ExtractNode(&job->link);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_job_mark_active_internal
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
 * Function: BPLib_STOR_CACHE_JobMarkActive
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_JobMarkActive(BPLib_STOR_CACHE_Job_t *job)
{
    BPLib_MEM_Lock_t                     *lock;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;
    BPLib_STOR_CACHE_Pool_t              *pool;

    pool  = BPLib_STOR_CACHE_GetParentPoolFromLink(&job->link);
    admin = BPLib_STOR_CACHE_GetAdmin(pool);

    lock = BPLib_MEM_LockResource(pool);
    BPLib_STOR_CACHE_JobMarkActiveInternal(&admin->active_list, job);
    BPLib_MEM_LockBroadCastSignal(lock);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_JobGetNextActive
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Job_t *BPLib_STOR_CACHE_JobGetNextActive(BPLib_STOR_CACHE_Pool_t *pool)
{
    BPLib_STOR_CACHE_Job_t               *job;
    BPLib_MEM_Lock_t                     *lock;
    BPLib_STOR_CACHE_Block_t             *jblk;
    BPLib_STOR_CACHE_BlockAdminContent_t *admin;

    admin = BPLib_STOR_CACHE_GetAdmin(pool);

    do
    {
        lock = BPLib_MEM_LockResource(pool);

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
        BPLib_MEM_LockRelease(lock);

        job = BPLib_STOR_CACHE_JobCast(jblk);
    }
    while (job == NULL && jblk != NULL);

    return job;
}

void BPLib_STOR_CACHE_JobRunAll(BPLib_STOR_CACHE_Pool_t *pool, void *arg)
{
    BPLib_STOR_CACHE_Job_t *job;

    /* forward any bundles between interfaces, based on active flow list */
    while (true)
    {
        job = BPLib_STOR_CACHE_JobGetNextActive(pool);
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

