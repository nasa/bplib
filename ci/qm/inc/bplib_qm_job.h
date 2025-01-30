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

#include "bplib_qm.h"

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
BPLib_QM_JobFunc_t BPLib_QM_Job_Lookup(BPLib_QM_JobState_t job_state);

#endif /* BPLIB_QM_JOB_H */
