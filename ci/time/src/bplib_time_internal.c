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

/**
 * @file
 *
 * Internal function definitions for Time Management
 */

/*
** Include
*/

#include "bplib_time.h"
#include "bplib_time_internal.h"
#include "bplib_fwp.h"


/*
** Function Definitions
*/

/* Read a CF from the ring buffer */
int64_t BPLib_TIME_GetCfFromBuffer(uint32_t BootEra)
{
    return BPLib_TIME_GlobalData.TimeData.CfRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN];
}

/* Set a CF value in the ring buffer */
void BPLib_TIME_SetCfInBuffer(int64_t CF, uint32_t BootEra)
{
    BPLib_TIME_GlobalData.TimeData.CfRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN] = CF;
}

/* Read a last valid DTN time from the ring buffer file */
uint64_t BPLib_TIME_GetDtnTimeFromBuffer(uint32_t BootEra)
{
    return BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN];
}

/* Set a last valid DTN time value in the ring buffer */
void BPLib_TIME_SetDtnTimeInBuffer(uint64_t DtnTime, uint32_t BootEra)
{
    BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN] = DtnTime;
}

/* Read current time data from file */
BPLib_Status_t BPLib_TIME_ReadTimeDataFromFile(void)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    int32 OsalStatus;
    
    /* Try opening existing file or creating new file */
    OsalStatus = OS_OpenCreate(&BPLib_TIME_GlobalData.FileHandle, BPLIB_TIME_FILE_NAME, 
                                    OS_FILE_FLAG_CREATE, OS_READ_WRITE);
    if (OsalStatus == OS_SUCCESS)
    {
        /* Read time data from file */
        OsalStatus = OS_read(BPLib_TIME_GlobalData.FileHandle, (void *) &BPLib_TIME_GlobalData.TimeData, 
                                            sizeof(BPLib_TIME_FileData_t));
        if (OsalStatus < 0)
        {
            Status = BPLIB_TIME_READ_ERROR;
        }

        (void) OS_close(BPLib_TIME_GlobalData.FileHandle);
    }
    else 
    {
        Status = BPLIB_TIME_READ_ERROR;
    }

    return Status;
}

/* Write current time data to file */
BPLib_Status_t BPLib_TIME_WriteTimeDataToFile(void)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    int32 OsalStatus;

    /* Try opening existing file or creating new file */
    OsalStatus = OS_OpenCreate(&BPLib_TIME_GlobalData.FileHandle, BPLIB_TIME_FILE_NAME, 
                                    OS_FILE_FLAG_CREATE, OS_READ_WRITE);
    if (OsalStatus == OS_SUCCESS)
    {
        /* Dump current time data to file */
        OsalStatus = OS_write(BPLib_TIME_GlobalData.FileHandle, 
                (void *) &BPLib_TIME_GlobalData.TimeData, sizeof(BPLib_TIME_FileData_t));
        if (OsalStatus < 0)
        {
            Status = BPLIB_TIME_WRITE_ERROR;
        }

        (void) OS_close(BPLib_TIME_GlobalData.FileHandle);
    }
    else 
    {
        Status = BPLIB_TIME_WRITE_ERROR;
    }

    return Status;
}

/* Get estimated DTN time */
uint64_t BPLib_TIME_GetEstimatedDtnTime(BPLib_TIME_MonotonicTime_t MonotonicTime)
{
    int64_t  CF = 0;
    uint64_t EstCf = 0;
    uint64_t EstDtnTime = (uint64_t) MonotonicTime.Time;

    /* An invalid boot era indicates an absolute DTN time was passed in */
    if (MonotonicTime.BootEra != BPLIB_TIME_INVALID_BOOT_ERA)
    {
        CF = BPLib_TIME_GetCfFromBuffer(MonotonicTime.BootEra);

        /* If no valid CF found, try to get a last valid DTN time to use instead */
        if (CF == 0 && MonotonicTime.BootEra != 0)
        {
            EstCf = BPLib_TIME_GetDtnTimeFromBuffer(MonotonicTime.BootEra - 1);

            /* If no valid value for a CF can be found, return 0 */
            if (EstCf == 0)
            {
                return 0;
            }
            else
            {
                EstDtnTime += EstCf;
            }
        }
        else
        {
            EstDtnTime += (uint64_t) CF;
        }
    }

    return EstDtnTime;
}

/* Safe conversion of epoch offsets */
int64_t BPLib_TIME_SafeOffset(int64_t HostEpoch, int64_t DtnEpoch, int64_t Multiplier)
{
    int64_t Offset;
    int8 Sign;

    if (HostEpoch < DtnEpoch)
    {
        Sign = -1;
        Offset = DtnEpoch - HostEpoch;
    }
    else 
    {
        Sign = 1;
        Offset = HostEpoch - DtnEpoch;
    }

    Offset *= Multiplier;
    Offset *= Sign;

    return Offset;
}