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


/*
** Global Data
*/

BPLib_TIME_GlobalData_t BPLib_TIME_GlobalData;
BPLib_FWP_ProxyCallbacks_t BPLib_FWP_ProxyCallbacks;


/*
** Function Definitions
*/

/* Time Management initialization */
BPLib_Status_t BPLib_TIME_Init(void) {
    /* TODO read from ring buffer file */
    BPLib_TIME_GlobalData.CurrentBootEra = 0;

    return BPLIB_UNIMPLEMENTED;
}

/* Get monotonic time from Time Proxy */
void BPLib_TIME_GetMonotonicTime(BPLib_TIME_MonotonicTime_t *MonotonicTime)
{
    if (MonotonicTime != NULL)
    {
        MonotonicTime->BootEra = BPLib_TIME_GlobalData.CurrentBootEra;
        MonotonicTime->Time = BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime();
    }
}

/* Calculate current correlation factor */
int64_t BPLib_TIME_CalculateCorrelationFactor(void)
{
    return 0;
}

/* Get current correlation factor */
int64_t BPLib_TIME_GetCorrelationFactor(void)
{
    return BPLib_TIME_GlobalData.CurrentCorrelationFactor;
}

/* Convert provided monotonic time to DTN time */
uint64_t BPLib_TIME_GetDtnTime(BPLib_TIME_MonotonicTime_t MonotonicTime)
{
    return 0;
}

/* Get delta between two provided monotonic times */
BPLib_Status_t BPLib_TIME_GetTimeDelta(BPLib_TIME_MonotonicTime_t Time1,
                                       BPLib_TIME_MonotonicTime_t Time2, uint64_t *Delta)
{
    return BPLIB_UNIMPLEMENTED;
}                                       

/* Perform time maintenance activities */
BPLib_Status_t BPLib_TIME_MaintenanceActivities(void)
{
    return BPLIB_UNIMPLEMENTED;
}
