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
