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
 * \file
 *  Unit tests for bplib_time.c
 */

/*
** Include Files
*/

#include "bplib_time_test_utils.h"


/*
** Function Definitions
*/

void Test_BPLib_TIME_Init(void)
{
    UtAssert_INT32_EQ(BPLib_TIME_Init(), BPLIB_UNIMPLEMENTED);
}

void Test_BPLib_TIME_GetMonotonicTime_Nominal(void)
{
    BPLib_TIME_MonotonicTime_t MonotonicTime;
    int32_t ExpBootEra = 1;

    BPLib_TIME_GlobalData.TimeData.CurrentBootEra = ExpBootEra;

    BPLib_TIME_GetMonotonicTime(&MonotonicTime);

    UtAssert_True(MonotonicTime.Time == BPLIB_TIME_TEST_MONOTONIC_TIME_VALUE, 
                    "Monotonic time (%ld) is equal to expected time (%ld)", 
                    MonotonicTime.Time, BPLIB_TIME_TEST_MONOTONIC_TIME_VALUE);
    UtAssert_INT32_EQ(MonotonicTime.BootEra, ExpBootEra);
}

void Test_BPLib_TIME_GetMonotonicTime_Null(void)
{
    BPLib_TIME_MonotonicTime_t *MonotonicTime = NULL;

    BPLib_TIME_GetMonotonicTime(MonotonicTime);

    UtAssert_NULL(MonotonicTime);
}

void Test_BPLib_TIME_CalculateCorrelationFactor_Nominal(void)
{
    int64_t ExpCf;
    int64_t ActualCf;

    // Make sure host clock state returns valid
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostClockState = Test_BPA_TIMEP_GetHostClockState_Valid;

    BPLib_TIME_GlobalData.EpochOffset = 5;
    ExpCf = BPLIB_TIME_TEST_HOST_TIME_VALUE - BPLIB_TIME_TEST_MONOTONIC_TIME_VALUE +
            BPLib_TIME_GlobalData.EpochOffset;

    ActualCf = BPLib_TIME_CalculateCorrelationFactor();

    UtAssert_True(ActualCf == ExpCf, "CF (%ld) is equal to expected CF (%ld)",
                    ActualCf, ExpCf);
}

void Test_BPLib_TIME_CalculateCorrelationFactor_InvalidClock(void)
{
    int64_t ExpCf = 0;
    int64_t ActualCf;

    // Make sure host clock state returns invalid
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostClockState = Test_BPA_TIMEP_GetHostClockState_Invalid;

    ActualCf = BPLib_TIME_CalculateCorrelationFactor();

    UtAssert_True(ActualCf == ExpCf, "CF (%ld) is equal to expected CF (%ld)",
                    ActualCf, ExpCf);
}

void Test_BPLib_TIME_GetCorrelationFactor(void)
{
    int64_t ExpCf = 1234;
    int64_t ActualCf;

    BPLib_TIME_GlobalData.CurrentCorrelationFactor = 1234;

    ActualCf = BPLib_TIME_GetCorrelationFactor();
    UtAssert_True(ActualCf == ExpCf, "CF (%ld) is equal to expected CF (%ld)",
                    ActualCf, ExpCf);
}

void Test_BPLib_TIME_GetDtnTime(void)
{
    BPLib_TIME_MonotonicTime_t MonotonicTime;

    UtAssert_True(BPLib_TIME_GetDtnTime(MonotonicTime) == 0, "DTN time is expected value");
}

void Test_BPLib_TIME_GetTimeDelta(void)
{
    BPLib_TIME_MonotonicTime_t Time1;
    BPLib_TIME_MonotonicTime_t Time2;
    uint64_t Delta;

    UtAssert_True(BPLib_TIME_GetTimeDelta(Time1, Time2, &Delta) == BPLIB_UNIMPLEMENTED,
                            "Delta is expected value");
}

void Test_BPLib_TIME_MaintenanceActivities(void)
{
    UtAssert_INT32_EQ(BPLib_TIME_MaintenanceActivities(), BPLIB_UNIMPLEMENTED);
}

void TestBplibTime_Register(void)
{
    ADD_TEST(Test_BPLib_TIME_Init);
    ADD_TEST(Test_BPLib_TIME_GetMonotonicTime_Nominal);
    ADD_TEST(Test_BPLib_TIME_GetMonotonicTime_Null);
    ADD_TEST(Test_BPLib_TIME_CalculateCorrelationFactor_Nominal);
    ADD_TEST(Test_BPLib_TIME_CalculateCorrelationFactor_InvalidClock);
    ADD_TEST(Test_BPLib_TIME_GetCorrelationFactor);
    ADD_TEST(Test_BPLib_TIME_GetDtnTime);
    ADD_TEST(Test_BPLib_TIME_GetTimeDelta);
    ADD_TEST(Test_BPLib_TIME_MaintenanceActivities);
}
