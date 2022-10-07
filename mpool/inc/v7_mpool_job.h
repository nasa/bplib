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

#ifndef V7_MPOOL_JOB_H
#define V7_MPOOL_JOB_H

#include <string.h>

#include "v7_mpool.h"

typedef struct bplib_mpool_job
{
    bplib_mpool_block_t         link;
    bplib_mpool_callback_func_t handler;
} bplib_mpool_job_t;

typedef struct bplib_mpool_job_statechange
{
    bplib_mpool_job_t base_job;

    /* JPHFIX: this single event handler may be separated into per-event-type handlers */
    bplib_mpool_callback_func_t event_handler;
} bplib_mpool_job_statechange_t;

/**
 * @brief Cast a block to a job type
 *
 * @param cb
 * @return bplib_mpool_job_t*
 */
bplib_mpool_job_t *bplib_mpool_job_cast(bplib_mpool_block_t *cb);

void bplib_mpool_job_init(bplib_mpool_block_t *base_block, bplib_mpool_job_t *jblk);

/**
 * @brief Mark a given job as runnable
 *
 * This marks it so it will be processed during the next call to forward data
 *
 * @note This is handled automatically by functions which append to a flow subq, such
 * as bplib_mpool_flow_try_push() and bplib_mpool_flow_try_move_all().
 * Applictions only need to explicitly call this API to mark it as active if there is
 * some other factor that requires it to be processed again.
 *
 * @param pool
 * @param jblk Job that is ready to run
 */
void bplib_mpool_job_mark_active(bplib_mpool_job_t *job);

/**
 * @brief Get the next active flow in the pool
 *
 * The given callback function will be invoked for all flows which are
 * currently marked as active
 *
 * @param pool
 * @return bplib_mpool_job_t *
 */
bplib_mpool_job_t *bplib_mpool_job_get_next_active(bplib_mpool_t *pool);

void bplib_mpool_job_run_all(bplib_mpool_t *pool, void *arg);

#endif /* V7_MPOOL_JOB_H */
