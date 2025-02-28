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
    uint32_t MinBootEra;

    /* 
    ** Ensure the boot era has a present slot in the ring buffer: must be less than or 
    ** equal to the current boot era and greater than the current boot era minus the
    ** maximum buffer length (or 0). 
    */ 
   
    if (BPLib_TIME_GlobalData.TimeData.CurrBootEra >= BPLIB_TIME_MAX_BUFFER_LEN)
    {
        MinBootEra = BPLib_TIME_GlobalData.TimeData.CurrBootEra - BPLIB_TIME_MAX_BUFFER_LEN + 1;
    }
    else
    {
        MinBootEra = 0;
    }

    if ((BootEra <= BPLib_TIME_GlobalData.TimeData.CurrBootEra) && (BootEra >= MinBootEra))
    {
        return BPLib_TIME_GlobalData.TimeData.CfRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN];
    }

    return 0;
}

/* Set a CF value in the ring buffer */
void BPLib_TIME_SetCfInBuffer(int64_t CF, uint32_t BootEra)
{
    uint32_t MinBootEra;

    /* 
    ** Ensure the boot era has a present slot in the ring buffer: must be less than or 
    ** equal to the current boot era and greater than the current boot era minus the
    ** maximum buffer length (or 0). 
    */ 
   
    if (BPLib_TIME_GlobalData.TimeData.CurrBootEra >= BPLIB_TIME_MAX_BUFFER_LEN)
    {
        MinBootEra = BPLib_TIME_GlobalData.TimeData.CurrBootEra - BPLIB_TIME_MAX_BUFFER_LEN + 1;
    }
    else
    {
        MinBootEra = 0;
    }

    if ((BootEra <= BPLib_TIME_GlobalData.TimeData.CurrBootEra) && (BootEra >= MinBootEra))
    {
        BPLib_TIME_GlobalData.TimeData.CfRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN] = CF;
    }

    return;
}

/* Read a last valid DTN time from the ring buffer file */
uint64_t BPLib_TIME_GetDtnTimeFromBuffer(uint32_t BootEra)
{
    uint32_t MinBootEra;
    
    /* 
    ** Ensure the boot era has a present slot in the ring buffer: must be less than or 
    ** equal to the current boot era and greater than the current boot era minus the
    ** maximum buffer length (or 0). 
    */ 
   
    if (BPLib_TIME_GlobalData.TimeData.CurrBootEra >= BPLIB_TIME_MAX_BUFFER_LEN)
    {
        MinBootEra = BPLib_TIME_GlobalData.TimeData.CurrBootEra - BPLIB_TIME_MAX_BUFFER_LEN + 1;
    }
    else
    {
        MinBootEra = 0;
    }

    if ((BootEra <= BPLib_TIME_GlobalData.TimeData.CurrBootEra) && (BootEra >= MinBootEra))
    {
        return BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN];
    }

    return 0;
}

/* Set a last valid DTN time value in the ring buffer */
void BPLib_TIME_SetDtnTimeInBuffer(uint64_t DtnTime, uint32_t BootEra)
{
    uint32_t MinBootEra;
    
    /* 
    ** Ensure the boot era has a present slot in the ring buffer: must be less than or 
    ** equal to the current boot era and greater than the current boot era minus the
    ** maximum buffer length (or 0). 
    */ 
   
    if (BPLib_TIME_GlobalData.TimeData.CurrBootEra >= BPLIB_TIME_MAX_BUFFER_LEN)
    {
        MinBootEra = BPLib_TIME_GlobalData.TimeData.CurrBootEra - BPLIB_TIME_MAX_BUFFER_LEN + 1;
    }
    else
    {
        MinBootEra = 0;
    }

    if ((BootEra <= BPLib_TIME_GlobalData.TimeData.CurrBootEra) && (BootEra >= MinBootEra))
    {
        BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN] = DtnTime;
    }

    return;
}

/* Read current time data from file */
BPLib_Status_t BPLib_TIME_ReadTimeDataFromFile(void)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    int32 OsalStatus;
    
    /* Try opening existing file or creating new file */
    OsalStatus = OS_OpenCreate(&BPLib_TIME_GlobalData.FileHandle, BPLIB_TIME_FILE_NAME, 
                                    OS_FILE_FLAG_CREATE, OS_READ_WRITE);
    printf("OSAL STATUS %d %s\n", OsalStatus, BPLIB_TIME_FILE_NAME);
    if (OsalStatus == OS_SUCCESS)
    {
        /* Read time data from file */
        OsalStatus = OS_read(BPLib_TIME_GlobalData.FileHandle, (void *) &BPLib_TIME_GlobalData.TimeData, 
                                            sizeof(BPLib_TIME_FileData_t));

        /* OSAL should return either 0 (file was just created) or the expected file size */
        if (OsalStatus != 0 && OsalStatus != sizeof(BPLib_TIME_FileData_t))
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
        if (OsalStatus != sizeof(BPLib_TIME_FileData_t))
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
    uint64_t EstDtnTime = 0; /* Return 0 if all following checks fail */

    /* An invalid boot era indicates an absolute DTN time was passed in, return that */
    if (MonotonicTime.BootEra == BPLIB_TIME_INVALID_BOOT_ERA)
    {
        EstDtnTime = (uint64_t) MonotonicTime.Time;
    }
    else
    {
        CF = BPLib_TIME_GetCfFromBuffer(MonotonicTime.BootEra);

        /* If a nonzero CF can be found, return an exact DTN time */
        if (CF != 0)
        {
            EstDtnTime = (uint64_t) (MonotonicTime.Time + CF);
        }
        /* If no valid CF found, try the last valid DTN time from previous boot era */
        else
        {
            EstCf = BPLib_TIME_GetDtnTimeFromBuffer(MonotonicTime.BootEra - 1);

            /* Get an estimated DTN time using a last valid DTN time value */
            if (EstCf != 0)
            {
                EstDtnTime = ((uint64_t) MonotonicTime.Time) + EstCf;
            }
            /* Else return 0 */
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

/* Get epoch offset (Host Epoch - DTN Epoch) */
int64_t BPLib_TIME_GetEpochOffset(void)
{
    BPLib_TIME_Epoch_t HostEpoch;
    int64_t  EpochOffset = 0;
    uint32_t NumLeapDays = 0;
    uint32_t StartYear, EndYear;
    uint16_t Year;

    /* Get host epoch */
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostEpoch(&HostEpoch);

    /* Get offsets in milliseconds and add them to the epoch offset */
    EpochOffset += BPLib_TIME_SafeOffset(HostEpoch.Year, BPLIB_TIME_EPOCH_YEAR, 
                                                            BPLIB_TIME_YEAR_IN_MSEC);
    EpochOffset += BPLib_TIME_SafeOffset(HostEpoch.Day, BPLIB_TIME_EPOCH_DAY, 
                                                            BPLIB_TIME_DAY_IN_MSEC);
    EpochOffset += BPLib_TIME_SafeOffset(HostEpoch.Hour, BPLIB_TIME_EPOCH_HOUR, 
                                                            BPLIB_TIME_HOUR_IN_MSEC);
    EpochOffset += BPLib_TIME_SafeOffset(HostEpoch.Minute, BPLIB_TIME_EPOCH_MINUTE, 
                                                            BPLIB_TIME_MINUTE_IN_MSEC);
    EpochOffset += BPLib_TIME_SafeOffset(HostEpoch.Second, BPLIB_TIME_EPOCH_SECOND, 
                                                            BPLIB_TIME_SECOND_IN_MSEC);
    EpochOffset += BPLib_TIME_SafeOffset(HostEpoch.Msec, BPLIB_TIME_EPOCH_MSEC, 1);

    /*
    ** Get any missing leap days
    */

    /* If the host epoch comes before the DTN epoch */   
    if (HostEpoch.Year < BPLIB_TIME_EPOCH_YEAR)
    {
        StartYear = HostEpoch.Year;
        EndYear = BPLIB_TIME_EPOCH_YEAR;
    }
    /* If the host epoch is on or after the DTN epoch */   
    else
    {
        StartYear = BPLIB_TIME_EPOCH_YEAR;
        EndYear = HostEpoch.Year;
    }

    /* Get all leap years in this range */
    for (Year = StartYear; Year < EndYear; Year++)
    {
        /* 
        ** Leap years occur on years divisible by 4, but not on years divisible by 100
        ** except for years divisible by 400. For instance, the year 2000 is a leap year
        ** but the year 1900 is not.
        */
        if ((Year % 4 == 0) && (Year % 100 != 0 || Year % 400 == 0))
        {
            NumLeapDays++;
        }
    }

    /* Add or subtract leap days from offset, depending on if offset should be pos/neg */
    if (HostEpoch.Year < BPLIB_TIME_EPOCH_YEAR)
    {
        EpochOffset -= (NumLeapDays * BPLIB_TIME_DAY_IN_MSEC);
    }
    else
    {
        EpochOffset += (NumLeapDays * BPLIB_TIME_DAY_IN_MSEC);
    }

    return EpochOffset;
}
