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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <string.h>
#include <assert.h>

#include "bplib.h"
#include "bplib_api_types.h"
#include "bplib_mem.h"
#include "bplib_mem_internal.h"

#include "osapi.h"

/**
 * Lacking bplib_os.h and bplib/os, improvised MEM-local BPLib_MEM_OS_* calls.
 */

#define OSAL_LOCALTIME_SECS_AT_2000 946684800

static OS_time_t BPLIB_MEM_OSAL_LOCALTIME_DTN_CONV;

static osal_id_t BPLib_MEM_OS_FileDataLock;

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_OS_Lock -
 *-------------------------------------------------------------------------------------*/
void BPLib_MEM_OS_Lock(bp_handle_t h)
{
    osal_id_t id;

    id = OS_ObjectIdFromInteger(bp_handle_to_serial(h, BPLIB_HANDLE_MPOOL_BASE));

    OS_CondVarLock(id);
}

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_OS_Unlock -
 *-------------------------------------------------------------------------------------*/
void BPLib_MEM_OS_Unlock(bp_handle_t h)
{
    osal_id_t id;

    id = OS_ObjectIdFromInteger(bp_handle_to_serial(h, BPLIB_HANDLE_MPOOL_BASE));

    OS_CondVarUnlock(id);
}


/*--------------------------------------------------------------------------------------
 * BPLib_MEM_OS_Init -
 *-------------------------------------------------------------------------------------*/
void BPLib_MEM_OS_Init(void)
{
    OS_MutSemCreate(&BPLib_MEM_OS_FileDataLock, "bplib_mem_osal", 0);
    BPLIB_MEM_OSAL_LOCALTIME_DTN_CONV = OS_TimeAssembleFromMilliseconds(OSAL_LOCALTIME_SECS_AT_2000, 0);
}

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_OS_CreateLock -
 *-------------------------------------------------------------------------------------*/
bp_handle_t BPLib_MEM_OS_CreateLock(void)
{
    char      lock_name[OS_MAX_API_NAME];
    int32     status;
    osal_id_t id;

    snprintf(lock_name, sizeof(lock_name), "bpl_mem%02u", BPLib_MEM_OS_NextSerial());
    status = OS_CondVarCreate(&id, lock_name, 0);
    if (status != OS_SUCCESS)
    {
        return BP_INVALID_HANDLE;
    }

    return bp_handle_from_serial(OS_ObjectIdToInteger(id), BPLIB_HANDLE_OS_BASE);
}

unsigned int BPLib_MEM_OS_NextSerial(void)
{
    static unsigned int last_serial_num = 0;
    unsigned int        next_serial;

    OS_MutSemTake(BPLib_MEM_OS_FileDataLock);
    next_serial = ++last_serial_num;
    OS_MutSemGive(BPLib_MEM_OS_FileDataLock);

    return next_serial;
}

int BPLib_MEM_OS_WaitUntilMs(bp_handle_t h, uint64_t abs_dtntime_ms)
{
    osal_id_t id;
    OS_time_t until_time;
    int32     status;

    id = OS_ObjectIdFromInteger(bp_handle_to_serial(h, BPLIB_HANDLE_OS_BASE));

    if (abs_dtntime_ms == BPLIB_MEM_DTNTIME_INFINITE)
    {
        /* Block Forever until Success */
        status = OS_CondVarWait(id);
    }
    else
    {
        until_time = OS_TimeFromTotalMilliseconds(abs_dtntime_ms);

        /* change the epoch from DTN (2000) to OSAL */
        until_time = OS_TimeAdd(until_time, BPLIB_MEM_OSAL_LOCALTIME_DTN_CONV);

        /* Block on Timed Wait and Update Timeout */
        status = OS_CondVarTimedWait(id, &until_time);
    }

    /* check for timeout error explicitly and translate to BPLIB_TIMEOUT */
    if (status == OS_ERROR_TIMEOUT)
    {
        return BPLIB_MEM_TIMEOUT;
    }

    /* other unexpected/unhandled errors become BPLIB_ERROR */
    if (status != OS_SUCCESS)
    {
        return BPLIB_ERROR;
    }

    /* status of 0 indicates success */
    return BPLIB_SUCCESS;
}

uint64_t BPLib_MEM_OS_GetDtnTimeMs(void)
{
    OS_time_t ref_tm;
    OS_GetLocalTime(&ref_tm);

    /* Convert to DTN epoch */
    ref_tm = OS_TimeSubtract(ref_tm, BPLIB_MEM_OSAL_LOCALTIME_DTN_CONV);

    /* Convert to milliseconds */
    return OS_TimeGetTotalMilliseconds(ref_tm);
}
