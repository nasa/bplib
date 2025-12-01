/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */
#include "bpcat_task.h"

BPCat_Status_t BPCat_TaskInit(BPCat_Task_t* task)
{
    BPCat_Status_t Status;

    if (task == NULL)
    {
        return BPCAT_NULL_PTR_ERR;
    }
    if ((task->TaskSetup == NULL) || (task->TaskTeardown == NULL) || (task->TaskFunc == NULL))
    {
        return BPCAT_NULL_PTR_ERR;
    }

    Status = task->TaskSetup(task->TaskId);
    if (Status != BPCAT_SUCCESS)
    {
        return Status;
    }

    return BPCAT_SUCCESS;
}

BPCat_Status_t BPCat_TaskStart(BPCat_Task_t* task, BPCat_AppData_t* AppData)
{
    int rc;

    if ((task == NULL) || (AppData == NULL))
    {
        return BPCAT_NULL_PTR_ERR;
    }

    /* Launch the task in it's own thread */
    rc = pthread_create(&task->Handle, NULL, (void* (*)(void*))task->TaskFunc,
        (void*)(AppData));
    if (rc != 0)
    {
        perror("pthread_create()");
        return BPCAT_TASK_INIT_ERR;
    }

    return BPCAT_SUCCESS;
}

BPCat_Status_t BPCat_TaskStop(BPCat_Task_t* task)
{
    BPCat_Status_t Status;

    if (task == NULL)
    {
        return BPCAT_NULL_PTR_ERR;
    }

    /* Join */
    pthread_join(task->Handle, NULL);

    Status = task->TaskTeardown(task->TaskId);
    if (Status != BPCAT_SUCCESS)
    {
        return Status;
    }

    return BPCAT_SUCCESS;
}
