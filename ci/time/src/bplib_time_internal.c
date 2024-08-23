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

/* Read a CF from the ring buffer file */
BPLib_Status_t BPLib_TIME_ReadCfFromBuffer(int64_t *CorrelationFactor, uint32_t BootEra)
{
    return BPLIB_UNIMPLEMENTED;
}

/* Write a CF to the ring buffer file */
BPLib_Status_t BPLib_TIME_WriteCfToBuffer(int64_t CorrelationFactor, uint32_t BootEra)
{
    return BPLIB_UNIMPLEMENTED;
}

/* Read a last valid DTN time from the ring buffer file */
BPLib_Status_t BPLib_TIME_ReadDtnTimeFromBuffer(uint64_t *LastValidDtnTime, uint32_t BootEra)
{
    return BPLIB_UNIMPLEMENTED;
}

/* Write a last valid DTN time to the ring buffer file */
BPLib_Status_t BPLib_TIME_WriteDtnTimeToBuffer(uint64_t LastValidDtnTime, uint32_t BootEra)
{
    return BPLIB_UNIMPLEMENTED;
}

/* Get estimated DTN time */
uint64_t BPLib_TIME_GetEstimatedDtnTime(BPLib_TIME_MonotonicTime_t MonotonicTime)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    int64_t CF = 0;

    /* A boot era of -1 indicates an absolute DTN time was passed in */
    if (MonotonicTime.BootEra == -1)
    {
        return (uint64_t) MonotonicTime.Time;
    }
    else 
    {
        /* Attempt to read CF from ring buffer file*/
        Status = BPLib_TIME_ReadCfFromBuffer(&CF, MonotonicTime.BootEra);

        /* If no valid CF found, try to get a last valid DTN time to use instead */
        if (Status != BPLIB_SUCCESS || CF == 0)
        {
            Status = BPLib_TIME_ReadDtnTimeFromBuffer((uint64_t *) &CF, 
                                                        MonotonicTime.BootEra - 1);

            /* If no valid value for a CF can be found, return 0 */
            if (Status != BPLIB_SUCCESS || CF == 0)
            {
                return 0;
            }
        }

        /* Use whatever valid CF was found to calculate estimated DTN time */
        return (uint64_t) (MonotonicTime.Time + CF);
    }
}