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
 * Public APIs for Time Management
 */

/*
** Include
*/

#include "bplib_time.h"
#include "bplib_time_internal.h"
#include "bplib_fwp.h"

#include "osapi.h"
#include <string.h>

/*
** Global Data
*/

BPLib_TIME_GlobalData_t BPLib_TIME_GlobalData;


/*
** Function Definitions
*/

/* Time Management initialization */
BPLib_Status_t BPLib_TIME_Init(void) {
    BPLib_Status_t Status = BPLIB_SUCCESS;
    BPLib_TIME_Epoch_t HostEpoch;
    int64_t YearOffset, DayOffset, HourOffset, MinOffset, SecOffset, MsecOffset;

    /* Clear memory */
    memset(&BPLib_TIME_GlobalData, 0, sizeof(BPLib_TIME_GlobalData_t));

    /* Get epoch offset (Host Epoch - DTN Epoch) */
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostEpoch(&HostEpoch);

    YearOffset = HostEpoch.Year - BPLIB_TIME_EPOCH_YEAR;
    DayOffset = HostEpoch.Day - BPLIB_TIME_EPOCH_DAY;
    HourOffset = HostEpoch.Hour - BPLIB_TIME_EPOCH_HOUR;
    MinOffset = HostEpoch.Minute - BPLIB_TIME_EPOCH_MINUTE;
    SecOffset = HostEpoch.Second - BPLIB_TIME_EPOCH_SECOND;
    MsecOffset = HostEpoch.Msec - BPLIB_TIME_EPOCH_MSEC;

    BPLib_TIME_GlobalData.EpochOffset  = MsecOffset;
    BPLib_TIME_GlobalData.EpochOffset += SecOffset  * 1000;
    BPLib_TIME_GlobalData.EpochOffset += MinOffset  * 1000 * 60;
    BPLib_TIME_GlobalData.EpochOffset += HourOffset * 1000 * 60 * 60;
    BPLib_TIME_GlobalData.EpochOffset += DayOffset  * 1000 * 60 * 60 * 24;
    BPLib_TIME_GlobalData.EpochOffset += YearOffset * 1000 * 60 * 60 * 24 * 365;

    /* TODO add missing leap days........... */

    /* Try opening existing file or creating new file */
    if (OS_OpenCreate(&BPLib_TIME_GlobalData.FileHandle, BPLIB_TIME_FILE_NAME, 
                                        OS_FILE_FLAG_CREATE, OS_READ_WRITE) == OS_SUCCESS)
    {
        /* Try to read file */
        if (OS_read(BPLib_TIME_GlobalData.FileHandle, (void *) &BPLib_TIME_GlobalData.TimeData, 
                                            sizeof(BPLib_TIME_FileData_t)) == OS_SUCCESS)
        {
            BPLib_TIME_GlobalData.TimeData.CurrentBootEra = 
                    (BPLib_TIME_GlobalData.TimeData.CurrentBootEra + 1) % BPLIB_TIME_MAX_BUFFER_LEN;
        }

        /* Overwrite existing file contents with new time data to update boot era */
        if (OS_write(BPLib_TIME_GlobalData.FileHandle, (void *) &BPLib_TIME_GlobalData.TimeData, 
                                            sizeof(BPLib_TIME_FileData_t)) != OS_SUCCESS)
        {
            return BPLIB_TIME_WRITE_ERROR;
        }
    }
    else 
    {
        return BPLIB_TIME_READ_ERROR;
    }

    (void) OS_close(BPLib_TIME_GlobalData.FileHandle);

    /* Initialize CF to 0  */
    BPLib_TIME_GlobalData.CurrentCorrelationFactor = 0;

    /* Run time maintenance activities */
    Status = BPLib_TIME_MaintenanceActivities();

    return Status;
}

/* Get monotonic time from Time Proxy */
void BPLib_TIME_GetMonotonicTime(BPLib_TIME_MonotonicTime_t *MonotonicTime)
{
    if (MonotonicTime != NULL)
    {
        MonotonicTime->BootEra = BPLib_TIME_GlobalData.TimeData.CurrentBootEra;
        MonotonicTime->Time = BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime();
    }
}

/* Calculate current correlation factor */
int64_t BPLib_TIME_CalculateCorrelationFactor(void)
{
    int64_t CF = 0;

    if (BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostClockState() == BPLIB_TIME_CLOCK_VALID)
    {
        CF = BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostTime() - 
             BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime() +
             BPLib_TIME_GlobalData.EpochOffset;
    }

    return CF;
}

/* Get current correlation factor */
int64_t BPLib_TIME_GetCorrelationFactor(void)
{
    return BPLib_TIME_GlobalData.CurrentCorrelationFactor;
}

/* Convert provided monotonic time to DTN time */
uint64_t BPLib_TIME_GetDtnTime(BPLib_TIME_MonotonicTime_t MonotonicTime)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    int64_t CF = 0;
    uint64_t DtnTime;

    /* Use most recently calculated CF if looking for CF in current boot era */
    if (MonotonicTime.BootEra == BPLib_TIME_GlobalData.TimeData.CurrentBootEra)
    {
        CF = BPLib_TIME_GlobalData.CurrentCorrelationFactor;
    }
    /* Read from ring buffer file to get past CFs */
    else 
    {
        Status = BPLib_TIME_ReadCfFromBuffer(&CF, MonotonicTime.BootEra);
    }

    /* Return 0 if no valid CF can be found */
    if (Status != BPLIB_SUCCESS || CF == 0)
    {
        DtnTime = 0;
    }
    /* Else return valid DTN time */
    else
    {
        DtnTime = (uint64_t) (MonotonicTime.Time + CF);
    }

    return DtnTime;
}

/* Get delta between two provided monotonic times */
BPLib_Status_t BPLib_TIME_GetTimeDelta(BPLib_TIME_MonotonicTime_t Time1,
                                       BPLib_TIME_MonotonicTime_t Time2, uint64_t *Delta)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    uint64_t EstDtnTime1;
    uint64_t EstDtnTime2;

    /* Do simple subtraction if both times are in same boot era */
    if (Time1.BootEra == Time2.BootEra)
    {
        *Delta = Time1.Time - Time2.Time;
    }
    /* Convert to estimated DTN time if times are in different boot eras */
    else
    {
        EstDtnTime1 = BPLib_TIME_GetEstimatedDtnTime(Time1);
        EstDtnTime2 = BPLib_TIME_GetEstimatedDtnTime(Time2);

        /* Return error if one or both times cannot be converted */
        if (EstDtnTime1 == 0 || EstDtnTime2 == 0)
        {
            *Delta = 0;

            Status = BPLIB_TIME_UNDEF_DELTA_ERROR;
        }
        else
        {
            *Delta = EstDtnTime1 - EstDtnTime2;
        }
    }

    return Status;
}                                       

/* Perform time maintenance activities */
BPLib_Status_t BPLib_TIME_MaintenanceActivities(void)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    int64_t NewCf;
    int64_t CurrentMonotonicTime;
    uint64_t CurrentDtnTime;

    NewCf = BPLib_TIME_CalculateCorrelationFactor();

    /* If a valid CF was calculated */
    if (NewCf != 0)
    {
        /* If CF has changed, update CF in ring buffer file */
        if (NewCf != BPLib_TIME_GlobalData.CurrentCorrelationFactor)
        {
            Status = BPLib_TIME_WriteCfToBuffer(NewCf, BPLib_TIME_GlobalData.TimeData.CurrentBootEra);
        }

        /* Save current DTN time to ring buffer to track most recent valid DTN time */
        if (Status == BPLIB_SUCCESS)
        {
            CurrentMonotonicTime = BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime();
            CurrentDtnTime = (uint64_t) (CurrentMonotonicTime + NewCf);

            Status = BPLib_TIME_WriteDtnTimeToBuffer(CurrentDtnTime, BPLib_TIME_GlobalData.TimeData.CurrentBootEra);
        }
    }

    /* Update CF in global data regardless of if it's valid or not */
    BPLib_TIME_GlobalData.CurrentCorrelationFactor = NewCf;

    return Status;
}
