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
#ifndef BPLIB_BPCAT_TASK_H
#define BPLIB_BPCAT_TASK_H

#include "bpcat_types.h"

#include <stdio.h>
#include <pthread.h>

typedef struct BPCat_Task
{
    void* (*TaskFunc)(BPCat_AppData_t* AppData);
    BPCat_Status_t (*TaskSetup)(void);
    BPCat_Status_t (*TaskTeardown)(void);
    pthread_t TaskHandle;
} BPCat_Task_t;


BPCat_Status_t BPCat_TaskInit(BPCat_Task_t* task);

BPCat_Status_t BPCat_TaskStart(BPCat_Task_t* task, BPCat_AppData_t* AppData);

BPCat_Status_t BPCat_TaskStop(BPCat_Task_t* task);


#endif /* BPLIB_BPCAT_TASK_H */
