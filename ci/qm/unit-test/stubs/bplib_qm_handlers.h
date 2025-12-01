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

#ifndef BPLIB_QM_HANDLERS_H
#define BPLIB_QM_HANDLERS_H

/*
** Include 
*/

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_qm.h"

#define QM_MAX_INCDEC_DEPTH (100u)

typedef struct
{
    BPLib_QM_JobState_t State;
    BPLib_Bundle_t* Bundle;
} BPLib_QM_CreateJobContext_t;

typedef struct
{
    BPLib_Bundle_t* Bundle;
} BPLib_QM_WaitQueuePushContext_t;

/*
** Global Data
*/
extern BPLib_QM_CreateJobContext_t Context_BPLib_QM_CreateJob[];
extern BPLib_QM_WaitQueuePushContext_t Context_BPLib_QM_WaitQueueTryPush[];

/*
** Function Definitions
*/

void UT_Handler_BPLib_QM_WaitQueueTryPull(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);

void UT_Handler_BPLib_QM_WaitQueueTryPush(void* UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);

void UT_Handler_BPLib_QM_DuctPull(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);

void UT_Handler_BPLib_QM_CreateJob(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);

#endif /* BPLIB_QM_HANDLERS_H */
