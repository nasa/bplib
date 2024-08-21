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

void Test_BPLib_TIME_GetMonotonicTime(void)
{
    // TODO fully test this with simulated proxy callbacks
    BPLib_TIME_GetMonotonicTime(NULL);

    // UtAssert_True(MonotonicTime.Time == 0, "Monotonic time is expected value");
}

void Test_BPLib_TIME_CalculateCorrelationFactor(void)
{
    UtAssert_True(BPLib_TIME_CalculateCorrelationFactor() == 0, "CF is expected value");
}

void Test_BPLib_TIME_GetCorrelationFactor(void)
{
    UtAssert_True(BPLib_TIME_GetCorrelationFactor() == 0, "CF is expected value");
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
    ADD_TEST(Test_BPLib_TIME_GetMonotonicTime);
    ADD_TEST(Test_BPLib_TIME_CalculateCorrelationFactor);
    ADD_TEST(Test_BPLib_TIME_GetCorrelationFactor);
    ADD_TEST(Test_BPLib_TIME_GetDtnTime);
    ADD_TEST(Test_BPLib_TIME_GetTimeDelta);
    ADD_TEST(Test_BPLib_TIME_MaintenanceActivities);
}
