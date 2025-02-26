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
#include "bplib_mem.h"
#include "bplib_cfg.h"

#define QM_NO_WAIT       WAITQUEUE_NO_WAIT  /**< Constant for no wait */
#define QM_WAIT_FOREVER  WAITQUEUE_WAIT_FOREVER  /**< Constant for indefinite wait */

/**
 * @enum BPLib_JobState
 * @brief Enumeration representing the various job states in the Bundle Protocol.
 * 
 * These states represent different phases of contact and channel activities, 
 * as well as signaling states without associated job functions.
 */
typedef enum BPLib_JobState
{
    CONTACT_IN_BI_TO_EBP = 0,       /**< Contact state: BI to EBP */
    CONTACT_IN_EBP_TO_CT = 1,       /**< Contact state: EBP to CT */
    CONTACT_IN_CT_TO_STOR = 2,      /**< Contact state: CT to Storage */
    CONTACT_OUT_STOR_TO_CT = 3,     /**< Contact state: Storage to CT */
    CONTACT_OUT_CT_TO_EBP = 4,      /**< Contact state: CT to EBP */
    CONTACT_OUT_EBP_TO_BI = 5,      /**< Contact state: EBP to BI */
    CHANNEL_IN_PI_TO_EBP = 6,       /**< Channel state: PI to EBP */
    CHANNEL_IN_EBP_TO_CT = 7,       /**< Channel state: EBP to CT */
    CHANNEL_IN_CT_TO_STOR = 8,      /**< Channel state: CT to Storage */
    CHANNEL_OUT_STOR_TO_CT = 9,     /**< Channel state: Storage to CT */
    CHANNEL_OUT_CT_TO_EBP = 10,     /**< Channel state: CT to EBP */
    CHANNEL_OUT_EBP_TO_PI = 11,     /**< Channel state: EBP to PI */
    NUM_GENWORKER_STATES = 12,      /**< Total number of worker states */
    NO_NEXT_STATE = 13,             /**< Signaling state with no next state */
} BPLib_QM_JobState_t;

/**
 * @enum BPLib_QM_Priority
 * @brief Enumeration for job priority.
 */
typedef enum BPLib_QM_Priority
{
    QM_PRI_NORMAL = 1 /**< Normal priority for jobs */
} BPLib_QM_Priority_t;

/**
 * @struct BPLib_Instance
 * @brief Represents a QM instance with its associated job memory and wait queues.
 * 
 * This structure holds the necessary data for queue management, job states, 
 * and memory allocations within BPLib.
 */
typedef struct BPLib_Instance
{
    BPLib_MEM_Pool_t pool; /**< Memory pool for this BPLib Instance */

    void* job_mem; /**< Memory for generic worker jobs */
    BPLib_QM_WaitQueue_t GenericWorkerJobs; /**< Queue of generic worker jobs */
    void* unsorted_job_mem; /**< Memory for unsorted jobs */
    BPLib_QM_WaitQueue_t UnsortedJobs; /**< Queue of unsorted jobs */
    void* ContactEgressMem[BPLIB_MAX_NUM_CONTACTS]; /**< Memory for contact egress jobs */
    BPLib_QM_WaitQueue_t ContactEgressJobs[BPLIB_MAX_NUM_CONTACTS]; /**< Queue of contact egress jobs */
    void* BundleCacheListMem; /**< Memory for bundle cache */
    BPLib_QM_WaitQueue_t BundleCacheList; /**< Queue of bundles in cache */
    void* ChannelEgressMem[BPLIB_MAX_NUM_CHANNELS];   /**< Memory for channel egress jobs */
    BPLib_QM_WaitQueue_t ChannelEgressJobs[BPLIB_MAX_NUM_CHANNELS]; /**< Queue of channel egress jobs */
    
} BPLib_Instance_t;

/**
 * @struct BPLib_QM_JobContext_t
 * @brief Represents a job's context information. Alone, this is considered an
 *        unsorted job.
 */
typedef struct
{
    BPLib_Bundle_t*     Bundle;    /**< Pointer to the bundle associated with this unsorted job */
    BPLib_QM_JobState_t NextState; /**< The next state for the job */
    BPLib_QM_Priority_t Priority;  /**< Priority of the job */
    uint16_t            EgressId;  /**< For egressing bundles, ID of channel/contact to send to */
} BPLib_QM_JobContext_t;

/**
 * @typedef BPLib_QM_JobFunc_t
 * @brief Type definition for the job function used in the QM.
 * 
 * The function takes an instance and a job context and returns the next job state.
 */
typedef BPLib_QM_JobState_t (*BPLib_QM_JobFunc_t)(BPLib_Instance_t* Inst, BPLib_QM_JobContext_t *Context);

/**
 * @struct BPLib_QM_Job
 * @brief Represents a single job in the Queue Manager.
 */
typedef struct BPLib_QM_Job
{
    BPLib_QM_JobFunc_t     JobFunc; /**< Function to be called for this job */
    BPLib_QM_JobContext_t *Context; /**< Job context information */
} BPLib_QM_Job_t;


/**
 * @brief Initializes the Queue Table for a specific instance.
 * 
 * This function initializes the memory and queues for the Queue Manager instance.
 * 
 * @param[in] inst The instance to be initialized.
 * @param[in] max_jobs The maximum number of jobs that can be queued.
 * 
 * @return Status of the initialization (success or failure).
 */
BPLib_Status_t BPLib_QM_QueueTableInit(BPLib_Instance_t* inst, size_t max_jobs);

/**
 * @brief Destroys the Queue Table for a specific instance.
 * 
 * This function cleans up and frees the memory used by the instance's job queues.
 * 
 * @param[in] inst The instance to be destroyed.
 */
void BPLib_QM_QueueTableDestroy(BPLib_Instance_t* inst);

/**
 * @brief Sorts the jobs in the queue based on their priority.
 * 
 * This function rearranges the jobs in the queue according to their priority.
 * 
 * @param[in] inst The instance containing the jobs to be sorted.
 * @param[in] num_jobs The number of jobs to be sorted.
 */
void BPLib_QM_SortJobs(BPLib_Instance_t* inst, size_t num_jobs);

/**
 * @brief Executes a single job in the Queue Manager.
 * 
 * This function runs a job, potentially blocking until the specified timeout.
 * 
 * @param[in] inst The instance where the job is to be run.
 * @param[in] timeout_ms Timeout in milliseconds for job execution.
 */
void BPLib_QM_RunJob(BPLib_Instance_t* inst, int timeout_ms);

/**
 * @brief Adds an unsorted job to the queue.
 * 
 * This function adds a job to the unsorted job queue, with the specified state 
 * and priority, and a timeout for processing.
 * 
 * @param[in] inst The instance to which the job is to be added.
 * @param[in] bundle The bundle associated with the job.
 * @param[in] state The initial state of the job.
 * @param[in] priority The priority of the job.
 * @param[in] EgressId For egressing bundles, a contact/channel ID to go to
 * @param[in] timeout_ms Timeout in milliseconds for adding the job.
 * 
 * @return Status of the job addition (success or failure).
 */
BPLib_Status_t BPLib_QM_AddUnsortedJob(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle,
    BPLib_QM_JobState_t state, BPLib_QM_Priority_t priority, uint16_t EgressId, int timeout_ms);

#endif /* BPLIB_QM_H */
