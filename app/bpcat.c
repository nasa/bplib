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

/*******************************************************************************
** Includes
*/
#include "bpcat_types.h"
#include "bpcat_fwp.h"
#include "bpcat_task.h"
#include "bpcat_cla.h"

#include "osapi.h"

/*******************************************************************************
** Configuration Definitions
*/
#define BPCAT_NUM_GEN_WORKER            1
#define BPCAT_GEN_WORKER_TIMEOUT        100u
#define BPCAT_MEMPOOL_LEN               8000000u
#define BPCAT_QM_MAX_JOBS               1024u
#define BPCAT_JOBS_PER_CYCLE            100

/*******************************************************************************
** Global State
*/
static BPCat_AppData_t AppData;
static BPCat_Task_t CLAOutTask;
static BPCat_Task_t CLAInTask;
static BPCat_Task_t GenWorkers[BPCAT_NUM_GEN_WORKER];

/*******************************************************************************
** Generic Worker Task Functions
*/
static BPLib_Status_t BPCat_GenWorkerTaskSetup()
{
    /* Generic Worker does not need any pre-task setup */
    printf("BPLib generic-worker reporting for duty\n");
    return BPCAT_SUCCESS;
}

static BPLib_Status_t BPCat_GenWorkerTaskTeardown()
{
    /* Generic Worker does not need any post-task teardown */
    return BPCAT_SUCCESS;
}

static void* BPCat_GenWorkerTaskFunc(BPCat_AppData_t* gAppData)
{
    while (gAppData->Running)
    {
        BPLib_QM_RunJob(&gAppData->BPLibInst, BPCAT_GEN_WORKER_TIMEOUT);
    }
    return NULL;
}

/*******************************************************************************
** Task Start/Stop
*/
static BPCat_Status_t BPCat_StartTasks()
{
    int i;
    BPCat_Status_t Status;

    /* Genworkers TaskInit */
    for (i = 0; i < BPCAT_NUM_GEN_WORKER; i++)
    {
        GenWorkers[i].TaskSetup = BPCat_GenWorkerTaskSetup;
        GenWorkers[i].TaskTeardown = BPCat_GenWorkerTaskTeardown;
        GenWorkers[i].TaskFunc = BPCat_GenWorkerTaskFunc;
        Status = BPCat_TaskInit(&GenWorkers[i]);
        if (Status != BPCAT_SUCCESS)
        {
            fprintf(stderr, "Failed to initialize Generic Worker Task\n");
            return Status;
        }
    }

    /* CLA TaskInit */
    CLAOutTask.TaskSetup = BPCat_CLAOutSetup;
    CLAOutTask.TaskTeardown = BPCat_CLAOutTeardown;
    CLAOutTask.TaskFunc = BPCat_CLAOutTaskFunc;
    Status = BPCat_TaskInit(&CLAOutTask);
    if (Status != BPCAT_SUCCESS)
    {
        fprintf(stderr, "Failed to initialize CLA-Egress Task\n");
        return Status;
    }
    CLAInTask.TaskSetup = BPCat_CLAInSetup;
    CLAInTask.TaskTeardown = BPCat_CLAOutTeardown;
    CLAInTask.TaskFunc = BPCat_CLAInTaskFunc;
    Status = BPCat_TaskInit(&CLAInTask);
    if (Status != BPCAT_SUCCESS)
    {
        fprintf(stderr, "Failed to initialize CLA-Ingress Task\n");
        return Status;
    }

    /* Start the generic workers first so BPLib is ready to do work */
    for (i = 0; i < BPCAT_NUM_GEN_WORKER; i++)
    {
        Status = BPCat_TaskStart(&GenWorkers[i], &AppData);
        if (Status != BPCAT_SUCCESS)
        {
            fprintf(stderr, "Failed to start Generic Worker Task\n");
            return Status;
        }
    }

    /* Start the CLAs */
    Status = BPCat_TaskStart(&CLAOutTask, &AppData);
    if (Status != BPCAT_SUCCESS)
    {
        fprintf(stderr, "Failed to start CLA-Egress Task\n");
        return Status;
    }
    Status = BPCat_TaskStart(&CLAInTask, &AppData);
    if (Status != BPCAT_SUCCESS)
    {
        fprintf(stderr, "Failed to start CLA-Ingress Task\n");
        return Status;
    }

    return BPCAT_SUCCESS;
}

static void BPCat_StopTasks()
{
    int i;
    BPCat_Status_t Status;

    /* Stop CLA Tasks */
    Status = BPCat_TaskStop(&CLAOutTask);
    if (Status != BPCAT_SUCCESS)
    {
        fprintf(stderr, "Failed to stop CLA-Egress Task\n");
    }
    Status = BPCat_TaskStop(&CLAInTask);
    if (Status != BPCAT_SUCCESS)
    {
        fprintf(stderr, "Failed to stop CLA-Ingress Task\n");
    }

    /* Stop Generic Workers */
    for (i = 0; i < BPCAT_NUM_GEN_WORKER; i++)
    {
        Status = BPCat_TaskStop(&GenWorkers[i]);
        if (Status != BPCAT_SUCCESS)
        {
            fprintf(stderr, "Failed to stop generic worker\n");
        }
    }

    return;
}

/*******************************************************************************
** Main
**   Note: Because BPLib is dependent on OSAL, we have to use OS_Application_Startup().
*/
void BPCat_Main()
{
    BPLib_Status_t BPLibStatus;
    BPCat_Status_t Status;

    /* FWP */
    Status = BPCat_FWP_Init();
    if (Status != BPLIB_SUCCESS)
    {
        printf("Failed to init FWP\n");
        return;
    }

    /* EM */
    BPLibStatus = BPLib_EM_Init();
    if (BPLibStatus != BPLIB_SUCCESS)
    {
        printf("Failed to init EM\n");
        return;
    }

    /* Time Management */
    /* Without modifying the TIME module, I was unable to get this to work.
    ** We need a follow-on ticket to abstract the TIME module and AS module's
    ** use of OSAL. Because no data is being put into BPLib, nothing will break
    ** by having this commented out.
    BPLibStatus = BPLib_TIME_Init();
    if (BPLibStatus != BPLIB_SUCCESS)
    {
        printf("Failed to init time\n");
        return;
    }
    */

    /* MEM */
    AppData.PoolMem = (void *)calloc(BPCAT_MEMPOOL_LEN, 1);
    if (AppData.PoolMem == NULL)
    {
        fprintf(stderr, "Failed to calloc() memory for the BPLib Memory Pool\n");
        return;
    }
    BPLibStatus = BPLib_MEM_PoolInit(&AppData.BPLibInst.pool, AppData.PoolMem,
        (size_t)BPCAT_MEMPOOL_LEN);
    if (BPLibStatus != BPLIB_SUCCESS)
    {
        fprintf(stderr, "Failed to initialize MEM\n");
        return;
    }

    /* QM */
    BPLibStatus = BPLib_QM_QueueTableInit(&AppData.BPLibInst, BPCAT_QM_MAX_JOBS);
    if (BPLibStatus != BPLIB_SUCCESS)
    {
        fprintf(stderr, "Failed to initialize QM\n");
        return;
    }

    /* Start CLAs and Gen Workers */
    Status = BPCat_StartTasks();
    if (Status != BPCAT_SUCCESS)
    {
        fprintf(stderr, "Failed to start BPCat tasks\n");
        return;
    }

    /* Run until a SIGINT (CTRL-C) sets AppRun to 0 */
    while (AppData.Running)
    {
        BPLib_QM_SortJobs(&AppData.BPLibInst, BPCAT_JOBS_PER_CYCLE);
        /* BPLib_ScanCache goes here */
    }

    /* Exit Signal Received */

    /* Cleanup */
    BPCat_StopTasks();
    BPLib_QM_QueueTableDestroy(&AppData.BPLibInst);
    free(AppData.PoolMem);
}

void SigHandler(int signo)
{
    if (signo == SIGINT)
    {
        AppData.Running = 0;
    }
}

void OS_Application_Startup()
{
    OS_API_Init();

    AppData.Running = 1;
    if (signal(SIGINT, SigHandler) == SIG_ERR) {
        fprintf(stderr, "Failed to register signal handler for SIGINT.\n");
        return;
    }

    BPCat_Main();
}
