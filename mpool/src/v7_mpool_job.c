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

#include "bplib.h"
#include "bplib_os.h"
#include "v7_mpool_internal.h"

/*-----------------------------------------------------------------
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_job_init(bplib_mpool_block_t *base_block, bplib_mpool_job_t *jblk)
{
    bplib_mpool_init_secondary_link(base_block, &jblk->link, bplib_mpool_blocktype_job);
}

/*-----------------------------------------------------------------
 *
 *-----------------------------------------------------------------*/
bplib_mpool_job_t *bplib_mpool_job_cast(bplib_mpool_block_t *cb)
{
    if (cb != NULL && cb->type == bplib_mpool_blocktype_job)
    {
        return (bplib_mpool_job_t *)cb;
    }

    return NULL;
}

/*-----------------------------------------------------------------
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_job_cancel_internal(bplib_mpool_job_t *job)
{
    assert(job->link.type == bplib_mpool_blocktype_job);
    bplib_mpool_extract_node(&job->link);
}

/*-----------------------------------------------------------------
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_job_mark_active_internal(bplib_mpool_block_t *active_list, bplib_mpool_job_t *job)
{
    /* first cancel the job it if it was already active */
    /* this permits it to be marked as active multiple times, it will still only be in the runnable list once */
    bplib_mpool_job_cancel_internal(job);
    if (job->handler)
    {
        bplib_mpool_insert_before(active_list, &job->link);
    }
}

/*-----------------------------------------------------------------
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_job_mark_active(bplib_mpool_job_t *job)
{
    bplib_mpool_lock_t                *lock;
    bplib_mpool_block_admin_content_t *admin;
    bplib_mpool_t                     *pool;

    pool  = bplib_mpool_get_parent_pool_from_link(&job->link);
    admin = bplib_mpool_get_admin(pool);

    lock = bplib_mpool_lock_resource(pool);
    bplib_mpool_job_mark_active_internal(&admin->active_list, job);
    bplib_mpool_lock_broadcast_signal(lock);
    bplib_mpool_lock_release(lock);
}

/*-----------------------------------------------------------------
 *
 *-----------------------------------------------------------------*/
bplib_mpool_job_t *bplib_mpool_job_get_next_active(bplib_mpool_t *pool)
{
    bplib_mpool_job_t                 *job;
    bplib_mpool_lock_t                *lock;
    bplib_mpool_block_t               *jblk;
    bplib_mpool_block_admin_content_t *admin;

    admin = bplib_mpool_get_admin(pool);

    do
    {
        lock = bplib_mpool_lock_resource(pool);

        /* if the head is reached here, then the list is empty */
        jblk = bplib_mpool_get_next_block(&admin->active_list);
        if (bplib_mpool_is_list_head(jblk))
        {
            jblk = NULL;
        }
        else
        {
            bplib_mpool_extract_node(jblk);
        }
        bplib_mpool_lock_release(lock);

        job = bplib_mpool_job_cast(jblk);
    }
    while (job == NULL && jblk != NULL);

    return job;
}

/*-----------------------------------------------------------------
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_job_run_all(bplib_mpool_t *pool, void *arg)
{
    bplib_mpool_job_t *job;

    /* forward any bundles between interfaces, based on active flow list */
    while (true)
    {
        job = bplib_mpool_job_get_next_active(pool);
        if (job == NULL)
        {
            break;
        }

        /* note it may not be necessary to call this _every_ cycle, this could be
         * deferred to a lower priority task if necessary, but this will be sure to get it done */
        bplib_mpool_maintain(pool);

        if (job->handler != NULL)
        {
            job->handler(arg, &job->link);
        }
    }
}
