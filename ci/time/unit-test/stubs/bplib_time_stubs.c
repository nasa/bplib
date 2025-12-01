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
 * Auto-Generated stub implementations for functions defined in bplib_time header
 */

#include "bplib_time.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_CalculateCorrelationFactor()
 * ----------------------------------------------------
 */
int64_t BPLib_TIME_CalculateCorrelationFactor(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_CalculateCorrelationFactor, int64_t);

    UT_GenStub_Execute(BPLib_TIME_CalculateCorrelationFactor, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_CalculateCorrelationFactor, int64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_GetBootEra()
 * ----------------------------------------------------
 */
uint32_t BPLib_TIME_GetBootEra(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_GetBootEra, uint32_t);

    UT_GenStub_Execute(BPLib_TIME_GetBootEra, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_GetBootEra, uint32_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_GetCorrelationFactor()
 * ----------------------------------------------------
 */
int64_t BPLib_TIME_GetCorrelationFactor(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_GetCorrelationFactor, int64_t);

    UT_GenStub_Execute(BPLib_TIME_GetCorrelationFactor, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_GetCorrelationFactor, int64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_GetCurrentDtnTime()
 * ----------------------------------------------------
 */
uint64_t BPLib_TIME_GetCurrentDtnTime(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_GetCurrentDtnTime, uint64_t);

    UT_GenStub_Execute(BPLib_TIME_GetCurrentDtnTime, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_GetCurrentDtnTime, uint64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_GetDtnTime()
 * ----------------------------------------------------
 */
uint64_t BPLib_TIME_GetDtnTime(BPLib_TIME_MonotonicTime_t MonotonicTime)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_GetDtnTime, uint64_t);

    UT_GenStub_AddParam(BPLib_TIME_GetDtnTime, BPLib_TIME_MonotonicTime_t, MonotonicTime);

    UT_GenStub_Execute(BPLib_TIME_GetDtnTime, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_GetDtnTime, uint64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_GetMonotonicTime()
 * ----------------------------------------------------
 */
int64_t BPLib_TIME_GetMonotonicTime(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_GetMonotonicTime, int64_t);

    UT_GenStub_Execute(BPLib_TIME_GetMonotonicTime, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_GetMonotonicTime, int64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_GetTimeDelta()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TIME_GetTimeDelta(BPLib_TIME_MonotonicTime_t Time1, BPLib_TIME_MonotonicTime_t Time2,
                                       int64_t *Delta)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_GetTimeDelta, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_TIME_GetTimeDelta, BPLib_TIME_MonotonicTime_t, Time1);
    UT_GenStub_AddParam(BPLib_TIME_GetTimeDelta, BPLib_TIME_MonotonicTime_t, Time2);
    UT_GenStub_AddParam(BPLib_TIME_GetTimeDelta, int64_t *, Delta);

    UT_GenStub_Execute(BPLib_TIME_GetTimeDelta, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_GetTimeDelta, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_Init()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TIME_Init(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_Init, BPLib_Status_t);

    UT_GenStub_Execute(BPLib_TIME_Init, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_Init, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_MaintenanceActivities()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TIME_MaintenanceActivities(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_MaintenanceActivities, BPLib_Status_t);

    UT_GenStub_Execute(BPLib_TIME_MaintenanceActivities, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_MaintenanceActivities, BPLib_Status_t);
}
