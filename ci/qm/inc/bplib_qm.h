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

#ifndef BPLIB_QM_H
#define BPLIB_QM_H

#include "bplib_api_types.h"
#include "bplib_qm_waitqueue.h"
#include "bplib_qm_job.h"
#include "bplib_mem.h"
#include "bplib_cfg.h"
#include "bplib_stor.h"

#define QM_NO_WAIT          0L  /**< Constant representing no wait */
#define QM_WAIT_FOREVER    -1L /**< Constant representing an indefinite wait */
#define QM_MAX_GEN_WORKERS  8L /**< Constant representing maximum allowed generic workers */

typedef struct BPLib_QM_WorkerState
{
    BPLib_QM_Job_t CurrJob;
} BPLib_QM_WorkerState_t;

/**
 * @struct BPLib_Instance
 * @brief Represents a QM instance with its associated job memory and wait queues.
 * 
 * This structure holds the necessary data for queue management, job states, 
 * and memory allocations within BPLib.
 */
struct BPLib_Instance
{
    BPLib_MEM_Pool_t pool; /**< Memory pool for this BPLib Instance */

    /* Worker Management */
    pthread_mutex_t RegisteredWorkersLock; // Move to bplib_os
    BPLib_QM_WorkerState_t RegisteredWorkers[QM_MAX_GEN_WORKERS];
    size_t NumWorkers;

    /* Queues */
    BPLib_QM_WaitQueue_t GenericWorkerJobs; /**< Queue of jobs */
    BPLib_QM_WaitQueue_t BundleCacheList; /**< Queue of bundles in cache */
    BPLib_QM_WaitQueue_t ContactEgressJobs[BPLIB_MAX_NUM_CONTACTS]; /**< Queue of contact egress jobs */
    BPLib_QM_WaitQueue_t ChannelEgressJobs[BPLIB_MAX_NUM_CHANNELS]; /**< Queue of channel egress jobs */

    /* Bundle Storage */
    BPLib_BundleCache_t BundleStorage;

};

/**
 * @brief Initializes the Queue Table for a specific instance.
 * 
 * This function initializes the memory and queues for the Queue Manager instance.
 * 
 * @param[in] inst The instance to be initialized.
 * @param[in] MaxJobs The maximum number of jobs that can be queued.
 * 
 * @return Status of the initialization (success or failure).
 */
BPLib_Status_t BPLib_QM_QueueTableInit(BPLib_Instance_t* inst, size_t MaxJobs);

/**
 * @brief Destroys the Queue Table for a specific instance.
 * 
 * This function cleans up and frees the memory used by the instance's job queues.
 * 
 * @param[in] inst The instance to be destroyed.
 */
void BPLib_QM_QueueTableDestroy(BPLib_Instance_t* inst);

/**
 * @brief Executes a single job in the Queue Manager.
 * 
 * This function runs a job, potentially blocking until the specified timeout.
 * This function is intended to be called from a generic worker thread.
 * 
 * @param[in] inst The instance where the job is to be run.
 * @param[out] WorkerID The returned ID of this worker, which should be passed to WorkerRunJob
 * 
 * @return Status of worker registration
 */
BPLib_Status_t BPLib_QM_RegisterWorker(BPLib_Instance_t* inst, int32_t* WorkerID);

/**
 * @brief Executes a single job in the Queue Manager.
 * 
 * This function runs a job, potentially blocking until the specified timeout.
 * This function is intended to be called from a generic worker thread.
 * 
 * @param[in] inst The instance where the job is to be run.
 * @param[in] WorkerID The ID of the worker, give at init by BPLIB_QM_RegisterWorker();
 * @param[in] TimeoutMs Timeout in milliseconds to wait for a new job to be available.
 * 
 * @return Status of running the job
 */
BPLib_Status_t BPLib_QM_WorkerRunJob(BPLib_Instance_t* inst, int32_t WorkerID, int TimeoutMs);


bool BPLib_QM_IsIngressIdle(BPLib_Instance_t* Inst);


bool BPLib_QM_IsDuctEmpty(BPLib_Instance_t* Inst, uint32_t EgressID, bool LocalDelivery);


BPLib_Status_t BPLib_QM_DuctPull(BPLib_Instance_t* Inst, uint32_t EgressID, bool LocalDelivery,
    int TimeoutMs, BPLib_Bundle_t** RetBundle);

/**
 * @brief Adds a job to the queue.
 * 
 * This function adds a job to the job queue, with the specified state 
 * and priority, and a timeout for processing.
 * 
 * @param[in] inst The instance to which the job is to be added.
 * @param[in] bundle The bundle associated with the job.
 * @param[in] state The initial state of the job.
 * @param[in] priority The priority of the job.
 * @param[in] TimeoutMs Timeout in milliseconds for adding the job.
 * 
 * @return Status of the job addition (success or failure).
 */
BPLib_Status_t BPLib_QM_CreateJob(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle,
    BPLib_QM_JobState_t state, BPLib_QM_Priority_t priority, int TimeoutMs);


#endif /* BPLIB_QM_H */
