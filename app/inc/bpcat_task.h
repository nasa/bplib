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
#ifndef BPLIB_BPCAT_TASK_H
#define BPLIB_BPCAT_TASK_H

#include "bpcat_types.h"

#include <stdio.h>
#include <pthread.h>

typedef struct BPCat_Task
{
    void* (*TaskFunc)(BPCat_AppData_t* AppData);
    BPCat_Status_t (*TaskSetup)(uint32_t TaskId);
    BPCat_Status_t (*TaskTeardown)(uint32_t TaskId);
    pthread_t Handle;
    uint32_t TaskId;
} BPCat_Task_t;


BPCat_Status_t BPCat_TaskInit(BPCat_Task_t* task);

BPCat_Status_t BPCat_TaskStart(BPCat_Task_t* task, BPCat_AppData_t* AppData);

BPCat_Status_t BPCat_TaskStop(BPCat_Task_t* task);


#endif /* BPLIB_BPCAT_TASK_H */
