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
#ifndef BPLIB_QM_JOB_H
#define BPLIB_QM_JOB_H

#include "bplib_api_types.h"
#include "bplib_mem.h"

/* Forward declaration of BPLib_Instance_t, defined in bplib_qm.h */
typedef struct BPLib_Instance BPLib_Instance_t;

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
 * @struct BPLib_QM_Job
 * @brief Represents a single job in the Queue Manager.
 */
typedef struct
{
    BPLib_Bundle_t*     Bundle;    /**< Pointer to the bundle associated with this job */
    BPLib_QM_JobState_t NextState; /**< The next state for the job */
    BPLib_QM_Priority_t Priority;  /**< Priority of the job */
} BPLib_QM_Job_t;

/**
 * @typedef BPLib_QM_JobFunc_t
 * @brief Type definition for the job function used in the QM.
 * 
 * The function takes an instance and a job context and returns the next job state.
 */
typedef BPLib_QM_JobState_t (*BPLib_QM_JobFunc_t)(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle);

/**
 * @brief Looks up the job function associated with a given job state.
 * 
 * This function returns the appropriate job function (callback) based on the provided job state. 
 * The job function is used to handle the execution of jobs within the QM subsystem.
 * 
 * @param[in] job_state The state of the job for which the function is being looked up.
 * 
 * @return A function pointer to the job function corresponding to the specified job state.
 */
BPLib_QM_JobFunc_t BPLib_QM_JobLookup(BPLib_QM_JobState_t job_state);

#endif /* BPLIB_QM_JOB_H */
