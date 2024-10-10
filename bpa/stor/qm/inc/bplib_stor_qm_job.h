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

#ifndef BPLIB_STOR_CACHE_JOB_H
#define BPLIB_STOR_CACHE_JOB_H

#include <string.h>

#include "bplib_stor_cache_block.h"

/**
 * @brief Cast a block to a job type
 *
 * @param cb
 * @return BPLib_STOR_CACHE_Job_t*
 */
BPLib_STOR_CACHE_Job_t *BPLib_STOR_CACHE_JobCast(BPLib_STOR_CACHE_Block_t *cb);

void BPLib_STOR_CACHE_JobInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Job_t *jblk);

/**
 * @brief Mark a given job as runnable
 *
 * This marks it so it will be processed during the next call to forward data
 *
 * @note This is handled automatically by functions which append to a duct subq, such
 * as BPLib_STOR_QM_DuctTryPush() and BPLib_STOR_QM_DuctTryMoveAll().
 * Applictions only need to explicitly call this API to mark it as active if there is
 * some other factor that requires it to be processed again.
 *
 * @param job Job that is ready to run
 */
void BPLib_STOR_CACHE_JobMarkActive(BPLib_STOR_CACHE_Job_t *job);

/**
 * @brief Get the next active duct in the pool
 *
 * The given callback function will be invoked for all ducts which are
 * currently marked as active
 *
 * @param pool
 * @return BPLib_STOR_CACHE_Job_t *
 */
BPLib_STOR_CACHE_Job_t *BPLib_STOR_CACHE_JobGetNextActive(BPLib_STOR_CACHE_Pool_t *pool);

void BPLib_STOR_CACHE_JobRunAll(BPLib_STOR_CACHE_Pool_t *pool, void *arg);

#endif /* BPLIB_STOR_CACHE_JOB_H */
