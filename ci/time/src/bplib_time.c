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

    /* Clear memory */
    memset(&BPLib_TIME_GlobalData, 0, sizeof(BPLib_TIME_GlobalData_t));

    /* Verify that all proxy callbacks have been already set */
    if (BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime == NULL ||
        BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostEpoch == NULL ||
        BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostClockState == NULL ||
        BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostTime == NULL)
    {
        BPLib_TIME_GlobalData.InitState = BPLIB_TIME_UNINIT;

        return BPLIB_TIME_UNINIT_ERROR;
    }

    /* Get the offset between the host epoch and the DTN epoch */
    BPLib_TIME_GlobalData.EpochOffset = BPLib_TIME_GetEpochOffset();

    /* Read in initial time data from file */
    Status = BPLib_TIME_ReadTimeDataFromFile();
    if (Status == BPLIB_SUCCESS)
    {
        /* Set time management state to initialized */
        BPLib_TIME_GlobalData.InitState = BPLIB_TIME_INIT;

        /* Increment boot era */
        BPLib_TIME_GlobalData.TimeData.CurrBootEra += 1;

        /* Run through initial maintenance activities */
        Status = BPLib_TIME_MaintenanceActivities();
    }

    return Status;
}

/* Get current monotonic time */
int64_t BPLib_TIME_GetMonotonicTime(void)
{
    return BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime();
}

/* Get current boot era */
uint32_t BPLib_TIME_GetBootEra(void)
{
    if (BPLib_TIME_GlobalData.InitState == BPLIB_TIME_INIT)
    {
        return BPLib_TIME_GlobalData.TimeData.CurrBootEra;
    }
    else
    {
        return 0;
    }
}

/* Calculate current correlation factor */
int64_t BPLib_TIME_CalculateCorrelationFactor(void)
{
    int64_t CF = 0;

    if (BPLib_TIME_GlobalData.InitState == BPLIB_TIME_INIT &&
        BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostClockState() == BPLIB_TIME_CLOCK_VALID)
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
    /* Get the current CF set by the Time maintenance activities */
    if (BPLib_TIME_GlobalData.InitState == BPLIB_TIME_INIT)
    {
        return BPLib_TIME_GlobalData.CurrentCf;
    }

    return 0;
}

/* Convert provided monotonic time to DTN time */
uint64_t BPLib_TIME_GetDtnTime(BPLib_TIME_MonotonicTime_t MonotonicTime)
{
    uint64_t DtnTime = 0;
    int64_t  CF      = 0;

    /* Return 0 if time has not been initialized */
    if (BPLib_TIME_GlobalData.InitState != BPLIB_TIME_INIT)
    {
        return DtnTime;
    }

    /* Use most recently calculated CF if looking for CF in current boot era */
    if (MonotonicTime.BootEra == BPLib_TIME_GlobalData.TimeData.CurrBootEra)
    {
        CF = BPLib_TIME_GlobalData.CurrentCf;
    }
    /* Read from ring buffer file to get past CFs */
    else 
    {
        CF = BPLib_TIME_GetCfFromBuffer(MonotonicTime.BootEra);
    }

    /* Check that the CF value is valid */
    if (CF != 0)
    {
        DtnTime = (uint64_t) (MonotonicTime.Time + CF);
    }

    return DtnTime;
}

uint64_t BPLib_TIME_GetCurrentDtnTime(void)
{
    /* Return 0 if time has not been initialized or if the CF is 0 */
    if (BPLib_TIME_GlobalData.InitState != BPLIB_TIME_INIT || 
        BPLib_TIME_GlobalData.CurrentCf == 0)
    {
        return 0;
    }
    else
    {
        return BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime() + BPLib_TIME_GlobalData.CurrentCf;
    }
}

/* Get delta between two provided monotonic times */
BPLib_Status_t BPLib_TIME_GetTimeDelta(BPLib_TIME_MonotonicTime_t Time1,
                                       BPLib_TIME_MonotonicTime_t Time2, int64_t *Delta)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    uint64_t EstDtnTime1, EstDtnTime2;

    /* Return error if time has not been initialized */
    if (BPLib_TIME_GlobalData.InitState != BPLIB_TIME_INIT)
    {
        return BPLIB_TIME_UNINIT_ERROR;
    }

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
        /* Else return estimated DTN time */
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
    BPLib_Status_t Status  = BPLIB_SUCCESS;
    uint32_t       BootEra = BPLib_TIME_GlobalData.TimeData.CurrBootEra;
    int64_t        NewCf;
    int64_t        CurrMonotonicTime;

    NewCf = BPLib_TIME_CalculateCorrelationFactor();

    /* If a valid CF was calculated */
    if (NewCf != 0)
    {
        CurrMonotonicTime = BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime();

        /* Update CF and Last Valid DTN time in ring buffers */
        BPLib_TIME_SetCfInBuffer(NewCf, BootEra);
        BPLib_TIME_SetDtnTimeInBuffer((uint64_t) (CurrMonotonicTime + NewCf), BootEra);

        /* Write new ring buffers to time data file */
        Status = BPLib_TIME_WriteTimeDataToFile();
    }

    /* Update CF in global data regardless of if it's valid or not */
    BPLib_TIME_GlobalData.CurrentCf = NewCf;

    return Status;
}
