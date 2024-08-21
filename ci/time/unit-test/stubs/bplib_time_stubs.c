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
void BPLib_TIME_GetMonotonicTime(BPLib_TIME_MonotonicTime_t *MonotonicTime)
{
    UT_GenStub_AddParam(BPLib_TIME_GetMonotonicTime, BPLib_TIME_MonotonicTime_t *, MonotonicTime);

    UT_GenStub_Execute(BPLib_TIME_GetMonotonicTime, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_GetTimeDelta()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TIME_GetTimeDelta(BPLib_TIME_MonotonicTime_t Time1, BPLib_TIME_MonotonicTime_t Time2,
                                       uint64_t *Delta)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_GetTimeDelta, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_TIME_GetTimeDelta, BPLib_TIME_MonotonicTime_t, Time1);
    UT_GenStub_AddParam(BPLib_TIME_GetTimeDelta, BPLib_TIME_MonotonicTime_t, Time2);
    UT_GenStub_AddParam(BPLib_TIME_GetTimeDelta, uint64_t *, Delta);

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
