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

/* Test that BPLib_TIME_Init initializes normally */
void Test_BPLib_TIME_Init_Nominal(void)
{
    int64_t ExpEpochOffset;

    /* Set test epoch data */
    TestHostEpochYear = 1980;
    TestHostEpochDay = 1;

    /* Calculate expected epoch offset */
    ExpEpochOffset = BPLIB_TIME_EPOCH_YEAR - TestHostEpochYear;
    ExpEpochOffset *= BPLIB_TIME_YEAR_IN_MSEC;
    ExpEpochOffset *= -1;

    /* Set to uninitialized */
    BPLib_TIME_GlobalData.InitState = BPLIB_TIME_UNINIT;

    UtAssert_INT32_EQ(BPLib_TIME_Init(), BPLIB_SUCCESS);

    UtAssert_EQ(int64_t, ExpEpochOffset, BPLib_TIME_GlobalData.EpochOffset);
    UtAssert_INT32_EQ(BPLib_TIME_GlobalData.InitState, BPLIB_TIME_INIT);
    UtAssert_INT32_EQ(BPLib_TIME_GlobalData.TimeData.CurrBootEra, 1);
    UtAssert_STUB_COUNT(OS_read, 1);
}

/* Test that BPLib_TIME_Init does not initialize if BPA_TIMEP_GetMonotonicTime is null */
void Test_BPLib_TIME_Init_ProxyNull1(void)
{
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime = NULL;

    UtAssert_INT32_EQ(BPLib_TIME_Init(), BPLIB_TIME_UNINIT_ERROR);
    UtAssert_INT32_EQ(BPLib_TIME_GlobalData.InitState, BPLIB_TIME_UNINIT);
}


/* Test that BPLib_TIME_Init does not initialize if BPA_TIMEP_GetHostEpoch is null */
void Test_BPLib_TIME_Init_ProxyNull2(void)
{
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostEpoch = NULL;

    UtAssert_INT32_EQ(BPLib_TIME_Init(), BPLIB_TIME_UNINIT_ERROR);
    UtAssert_INT32_EQ(BPLib_TIME_GlobalData.InitState, BPLIB_TIME_UNINIT);
}

/* Test that BPLib_TIME_Init does not initialize if BPA_TIMEP_GetHostClockState is null */
void Test_BPLib_TIME_Init_ProxyNull3(void)
{
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostClockState = NULL;

    UtAssert_INT32_EQ(BPLib_TIME_Init(), BPLIB_TIME_UNINIT_ERROR);
    UtAssert_INT32_EQ(BPLib_TIME_GlobalData.InitState, BPLIB_TIME_UNINIT);
}

/* Test that BPLib_TIME_Init does not initialize if BPA_TIMEP_GetHostTime is null */
void Test_BPLib_TIME_Init_ProxyNull4(void)
{
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostTime = NULL;

    UtAssert_INT32_EQ(BPLib_TIME_Init(), BPLIB_TIME_UNINIT_ERROR);
    UtAssert_INT32_EQ(BPLib_TIME_GlobalData.InitState, BPLIB_TIME_UNINIT);
}

/* Test that BPLib_TIME_Init does not initialize if reading the time data fails */
void Test_BPLib_TIME_Init_FailedRead(void)
{
    int64_t ExpEpochOffset;

    /* Set test epoch data */
    TestHostEpochYear = 1980;
    TestHostEpochDay = 1;

    /* Calculate expected epoch offset */
    ExpEpochOffset = BPLIB_TIME_EPOCH_YEAR - TestHostEpochYear;
    ExpEpochOffset *= BPLIB_TIME_YEAR_IN_MSEC;
    ExpEpochOffset *= -1;

    /* Set to uninitialized */
    BPLib_TIME_GlobalData.InitState = BPLIB_TIME_UNINIT;

    /* Ensure BPLib_TIME_ReadTimeDataFromFile fails */
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);

    UtAssert_INT32_EQ(BPLib_TIME_Init(), BPLIB_TIME_READ_ERROR);

    UtAssert_EQ(int64_t, ExpEpochOffset, BPLib_TIME_GlobalData.EpochOffset);
    UtAssert_INT32_EQ(BPLib_TIME_GlobalData.InitState, BPLIB_TIME_UNINIT);
    UtAssert_INT32_EQ(BPLib_TIME_GlobalData.TimeData.CurrBootEra, 0);
    UtAssert_STUB_COUNT(OS_read, 0);
}

/* Test that monotonic time is returned in nominal case */
void Test_BPLib_TIME_GetMonotonicTime_Nominal(void)
{
    BPLib_TIME_MonotonicTime_t MonotonicTime;
    int32_t ExpBootEra = 1;
    int64_t ExpMonotonicTime = 1234;

    /* Set test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = ExpBootEra;
    TestMonotonicTime = ExpMonotonicTime;

    BPLib_TIME_GetMonotonicTime(&MonotonicTime);

    UtAssert_EQ(int64_t, MonotonicTime.Time, ExpMonotonicTime);
    UtAssert_INT32_EQ(MonotonicTime.BootEra, ExpBootEra);
}

/* Test that nothing happens if monotonic time is null */
void Test_BPLib_TIME_GetMonotonicTime_Null(void)
{
    BPLib_TIME_MonotonicTime_t *MonotonicTime = NULL;

    BPLib_TIME_GetMonotonicTime(MonotonicTime);

    UtAssert_NULL(MonotonicTime);
}

/* Test that nothing happens if time is uninitialized */
void Test_BPLib_TIME_GetMonotonicTime_Uninit(void)
{
    BPLib_TIME_MonotonicTime_t MonotonicTime;

    MonotonicTime.BootEra = 1;
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = 2;

    /* Set init state to uninitialized */
    BPLib_TIME_GlobalData.InitState = BPLIB_TIME_UNINIT;

    BPLib_TIME_GetMonotonicTime(&MonotonicTime);

    UtAssert_INT32_NEQ(MonotonicTime.BootEra, BPLib_TIME_GlobalData.TimeData.CurrBootEra);
}

/* Test that a positive CF is returned */
void Test_BPLib_TIME_CalculateCorrelationFactor_NominalPos(void)
{
    int64_t ExpCf;

    /* Set test data */
    TestHostTime = 5678;
    TestMonotonicTime = 1234;
    BPLib_TIME_GlobalData.EpochOffset = 5;
    ExpCf = TestHostTime - TestMonotonicTime + BPLib_TIME_GlobalData.EpochOffset;

    UtAssert_EQ(int64_t, BPLib_TIME_CalculateCorrelationFactor(), ExpCf);
}

/* Test that a negative CF is returned */
void Test_BPLib_TIME_CalculateCorrelationFactor_NominalNeg(void)
{
    int64_t ExpCf;

    /* Set test data */
    TestHostTime = 5678;
    TestMonotonicTime = 1234;
    BPLib_TIME_GlobalData.EpochOffset = -90000;
    ExpCf = TestHostTime - TestMonotonicTime + BPLib_TIME_GlobalData.EpochOffset;

    UtAssert_EQ(int64_t, BPLib_TIME_CalculateCorrelationFactor(), ExpCf);
}

/* Test that the CF is 0 if the clock is invalid */
void Test_BPLib_TIME_CalculateCorrelationFactor_InvalidClock(void)
{
    int64_t ExpCf = 0;

    /* Make sure host clock state returns invalid */
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostClockState = Test_BPA_TIMEP_GetHostClockState_Invalid;

    UtAssert_EQ(int64_t, BPLib_TIME_CalculateCorrelationFactor(), ExpCf);
}

/* Test that the CF is 0 if time is uninitialized */
void Test_BPLib_TIME_CalculateCorrelationFactor_Uninit(void)
{
    int64_t ExpCf = 0;

    /* Make sure time is set to uninitialized */
    BPLib_TIME_GlobalData.InitState = BPLIB_TIME_UNINIT;

    UtAssert_EQ(int64_t, BPLib_TIME_CalculateCorrelationFactor(), ExpCf);
}

/* Test that the expected CF is returned in nominal case */
void Test_BPLib_TIME_GetCorrelationFactor_Nominal(void)
{
    int64_t ExpCf = 1234;

    BPLib_TIME_GlobalData.CurrentCf = 1234;

    UtAssert_EQ(int64_t, BPLib_TIME_GetCorrelationFactor(), ExpCf);
}

/* Test that the CF returns 0 if time is uninitialized */
void Test_BPLib_TIME_GetCorrelationFactor_Uninit(void)
{
    int64_t ExpCf = 0;

    BPLib_TIME_GlobalData.CurrentCf = 1234;

    BPLib_TIME_GlobalData.InitState = BPLIB_TIME_UNINIT;

    UtAssert_EQ(int64_t, BPLib_TIME_GetCorrelationFactor(), ExpCf);
}

/* Test that the right DTN time is returned for a current boot era time */
void Test_BPLib_TIME_GetDtnTime_CurrBootEra(void)
{
    BPLib_TIME_MonotonicTime_t MonotonicTime;
    uint64_t ExpDtnTime;

    /* Set global data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = 3;
    BPLib_TIME_GlobalData.CurrentCf = 1234;

    /* Set test data */
    MonotonicTime.BootEra = 3;
    MonotonicTime.Time = 5678;

    ExpDtnTime = 1234 + 5678;

    UtAssert_EQ(uint64_t, BPLib_TIME_GetDtnTime(MonotonicTime), ExpDtnTime);
}

/* Test that the right DTN time is returned for a past boot era time */
void Test_BPLib_TIME_GetDtnTime_PastBootEra(void)
{
    BPLib_TIME_MonotonicTime_t MonotonicTime;
    uint64_t ExpDtnTime;

    /* Set global data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = 3;
    BPLib_TIME_GlobalData.CurrentCf = 1234;
    BPLib_TIME_GlobalData.TimeData.CfRingBuff[2 % BPLIB_TIME_MAX_BUFFER_LEN] = -1234;

    /* Set test data */
    MonotonicTime.BootEra = 2;
    MonotonicTime.Time = 5678;

    ExpDtnTime = 5678 - 1234;

    UtAssert_EQ(uint64_t, BPLib_TIME_GetDtnTime(MonotonicTime), ExpDtnTime);
}

/* Test that DTN time is returned as 0 if the CF is 0 */
void Test_BPLib_TIME_GetDtnTime_ZeroCf(void)
{
    BPLib_TIME_MonotonicTime_t MonotonicTime;
    uint64_t ExpDtnTime;

    /* Set global data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = 3;
    BPLib_TIME_GlobalData.CurrentCf = 0;

    /* Set test data */
    MonotonicTime.BootEra = 3;
    MonotonicTime.Time = 5678;

    ExpDtnTime = 0;

    UtAssert_EQ(uint64_t, BPLib_TIME_GetDtnTime(MonotonicTime), ExpDtnTime);
}

/* Test that DTN time returns 0 if time is uninitialized */
void Test_BPLib_TIME_GetDtnTime_Uninit(void)
{
    BPLib_TIME_MonotonicTime_t MonotonicTime;
    uint64_t ExpDtnTime = 0;

    BPLib_TIME_GlobalData.InitState = BPLIB_TIME_UNINIT;

    UtAssert_EQ(uint64_t, BPLib_TIME_GetDtnTime(MonotonicTime), ExpDtnTime);
}

/* Test that an error is returned if time is uninitialized */
void Test_BPLib_TIME_GetTimeDelta_Uninit(void)
{
    BPLib_TIME_MonotonicTime_t Time1;
    BPLib_TIME_MonotonicTime_t Time2;
    int64_t Delta;

    /* Get state to uninitialized */
    BPLib_TIME_GlobalData.InitState = BPLIB_TIME_UNINIT;

    UtAssert_INT32_EQ(BPLib_TIME_GetTimeDelta(Time1, Time2, &Delta), BPLIB_TIME_UNINIT_ERROR);
}

/* Test that the delta is returned for the same boot era */
void Test_BPLib_TIME_GetTimeDelta_SameBootEra(void)
{
    BPLib_TIME_MonotonicTime_t Time1;
    BPLib_TIME_MonotonicTime_t Time2;
    int64_t Delta;
    int64_t ExpDelta;

    /* Set test data */
    Time1.BootEra = 1;
    Time1.Time = 5678;

    Time2.BootEra = 1;
    Time2.Time = 1234;

    ExpDelta = Time1.Time - Time2.Time;

    UtAssert_INT32_EQ(BPLib_TIME_GetTimeDelta(Time1, Time2, &Delta), BPLIB_SUCCESS);
    UtAssert_EQ(int64_t, Delta, ExpDelta);
}

/* Test that the delta is returned for different boot eras */
void Test_BPLib_TIME_GetTimeDelta_DiffBootEra(void)
{
    BPLib_TIME_MonotonicTime_t Time1;
    BPLib_TIME_MonotonicTime_t Time2;
    int64_t Delta;
    int64_t ExpDelta;

    /* Set global data */
    BPLib_TIME_GlobalData.TimeData.CfRingBuff[1] = 20;
    BPLib_TIME_GlobalData.TimeData.CfRingBuff[2] = 1500;
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = 3;
    
    /* Set test data */
    Time1.BootEra = 1;
    Time1.Time = 1234;

    Time2.BootEra = 2;
    Time2.Time = 5678;

    ExpDelta = (Time1.Time + BPLib_TIME_GlobalData.TimeData.CfRingBuff[1]) - 
               (Time2.Time + BPLib_TIME_GlobalData.TimeData.CfRingBuff[2]);
    
    UtAssert_INT32_EQ(BPLib_TIME_GetTimeDelta(Time1, Time2, &Delta), BPLIB_SUCCESS);
    UtAssert_EQ(int64_t, Delta, ExpDelta);
}

/* Test that the an error is returned when no delta can be calculated for Time1 */
void Test_BPLib_TIME_GetTimeDelta_InvalidDelta1(void)
{
    BPLib_TIME_MonotonicTime_t Time1;
    BPLib_TIME_MonotonicTime_t Time2;
    int64_t Delta;
    int64_t ExpDelta = 0;

    /* Set global data */
    BPLib_TIME_GlobalData.TimeData.CfRingBuff[1] = 0;
    BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[1] = 0;
    BPLib_TIME_GlobalData.TimeData.CfRingBuff[2] = 1500;
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = 3;
    
    /* Set test data */
    Time1.BootEra = 1;
    Time1.Time = 1234;

    Time2.BootEra = 2;
    Time2.Time = 5678;
    
    UtAssert_INT32_EQ(BPLib_TIME_GetTimeDelta(Time1, Time2, &Delta), BPLIB_TIME_UNDEF_DELTA_ERROR);
    UtAssert_EQ(int64_t, Delta, ExpDelta);
}

/* Test that the an error is returned when no delta can be calculated for Time2 */
void Test_BPLib_TIME_GetTimeDelta_InvalidDelta2(void)
{
    BPLib_TIME_MonotonicTime_t Time1;
    BPLib_TIME_MonotonicTime_t Time2;
    int64_t Delta;
    int64_t ExpDelta = 0;

    /* Set global data */
    BPLib_TIME_GlobalData.TimeData.CfRingBuff[1] = 20;
    BPLib_TIME_GlobalData.TimeData.CfRingBuff[2] = 0;
    BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[2] = 0;
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = 3;
    
    /* Set test data */
    Time1.BootEra = 1;
    Time1.Time = 1234;

    Time2.BootEra = 2;
    Time2.Time = 5678;
    
    UtAssert_INT32_EQ(BPLib_TIME_GetTimeDelta(Time1, Time2, &Delta), BPLIB_TIME_UNDEF_DELTA_ERROR);
    UtAssert_EQ(int64_t, Delta, ExpDelta);
}


/* Test that the CF is updated locally when it returns 0 but not in the ring buffer */
void Test_BPLib_TIME_MaintenanceActivities_ZeroCf(void)
{
    int64_t ExpCurrCf = 0;
    int64_t ExpBuffCf = 1234;
    uint64_t ExpDtnTime = 0;

    /* Set global data */
    BPLib_TIME_GlobalData.CurrentCf = ExpBuffCf;
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = 1;
    BPLib_TIME_GlobalData.TimeData.CfRingBuff[1 % BPLIB_TIME_MAX_BUFFER_LEN] = ExpBuffCf;

    /* Make sure host clock state returns invalid */
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostClockState = Test_BPA_TIMEP_GetHostClockState_Invalid;

    UtAssert_INT32_EQ(BPLib_TIME_MaintenanceActivities(), BPLIB_SUCCESS);
    UtAssert_EQ(int64_t, BPLib_TIME_GlobalData.CurrentCf, ExpCurrCf);
    UtAssert_EQ(int64_t, BPLib_TIME_GlobalData.TimeData.CfRingBuff[1 % BPLIB_TIME_MAX_BUFFER_LEN], ExpBuffCf);
    UtAssert_EQ(int64_t, BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[1 % BPLIB_TIME_MAX_BUFFER_LEN], ExpDtnTime);
    UtAssert_STUB_COUNT(OS_write, 0);
}

/* Test that the CF is updated when a new one is calculated */
void Test_BPLib_TIME_MaintenanceActivities_NewCf(void)
{
    int64_t ExpCurrCf;
    int64_t ExpBuffCf;
    uint64_t ExpDtnTime;
    uint32_t BootEra = 1;

    /* Set global data */
    BPLib_TIME_GlobalData.CurrentCf = 1234;
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BootEra;

    /* Set test data */
    TestHostTime = 5678;
    TestMonotonicTime = 1234;
    BPLib_TIME_GlobalData.EpochOffset = 5;
    ExpCurrCf = TestHostTime - TestMonotonicTime + BPLib_TIME_GlobalData.EpochOffset;
    ExpBuffCf = ExpCurrCf;
    ExpDtnTime = TestMonotonicTime + ExpCurrCf;

    UtAssert_INT32_EQ(BPLib_TIME_MaintenanceActivities(), BPLIB_SUCCESS);
    UtAssert_EQ(int64_t, BPLib_TIME_GlobalData.CurrentCf, ExpCurrCf);
    UtAssert_EQ(int64_t, BPLib_TIME_GlobalData.TimeData.CfRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN], ExpBuffCf);
    UtAssert_EQ(int64_t, BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN], ExpDtnTime);
    UtAssert_STUB_COUNT(OS_write, 1);
}

void TestBplibTime_Register(void)
{
    ADD_TEST(Test_BPLib_TIME_Init_Nominal);
    ADD_TEST(Test_BPLib_TIME_Init_ProxyNull1);
    ADD_TEST(Test_BPLib_TIME_Init_ProxyNull2);
    ADD_TEST(Test_BPLib_TIME_Init_ProxyNull3);
    ADD_TEST(Test_BPLib_TIME_Init_ProxyNull4);
    ADD_TEST(Test_BPLib_TIME_Init_FailedRead);

    ADD_TEST(Test_BPLib_TIME_GetMonotonicTime_Nominal);
    ADD_TEST(Test_BPLib_TIME_GetMonotonicTime_Null);
    ADD_TEST(Test_BPLib_TIME_GetMonotonicTime_Uninit);

    ADD_TEST(Test_BPLib_TIME_CalculateCorrelationFactor_NominalPos);
    ADD_TEST(Test_BPLib_TIME_CalculateCorrelationFactor_NominalNeg);
    ADD_TEST(Test_BPLib_TIME_CalculateCorrelationFactor_InvalidClock);
    ADD_TEST(Test_BPLib_TIME_CalculateCorrelationFactor_Uninit);

    ADD_TEST(Test_BPLib_TIME_GetCorrelationFactor_Nominal);
    ADD_TEST(Test_BPLib_TIME_GetCorrelationFactor_Uninit);

    ADD_TEST(Test_BPLib_TIME_GetDtnTime_CurrBootEra);
    ADD_TEST(Test_BPLib_TIME_GetDtnTime_PastBootEra);
    ADD_TEST(Test_BPLib_TIME_GetDtnTime_ZeroCf);
    ADD_TEST(Test_BPLib_TIME_GetDtnTime_Uninit);

    ADD_TEST(Test_BPLib_TIME_GetTimeDelta_Uninit);
    ADD_TEST(Test_BPLib_TIME_GetTimeDelta_SameBootEra);
    ADD_TEST(Test_BPLib_TIME_GetTimeDelta_DiffBootEra);
    ADD_TEST(Test_BPLib_TIME_GetTimeDelta_InvalidDelta1);
    ADD_TEST(Test_BPLib_TIME_GetTimeDelta_InvalidDelta2);

    ADD_TEST(Test_BPLib_TIME_MaintenanceActivities_ZeroCf);
    ADD_TEST(Test_BPLib_TIME_MaintenanceActivities_NewCf);
}
