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
 *  Unit tests for bplib_time_internal.c
 */

/*
** Include Files
*/

#include "bplib_time_test_utils.h"


/*
** Function Definitions
*/

void Test_BPLib_TIME_GetCfFromBuffer(void)
{
    uint32_t BootEra = 0;

    UtAssert_True(BPLib_TIME_GetCfFromBuffer(BootEra) == 0, "CF is expected value");
}

void Test_BPLib_TIME_SetCfInBuffer(void)
{
    int64_t  CorrelationFactor = 0;
    uint32_t BootEra = 0;

    BPLib_TIME_SetCfInBuffer(CorrelationFactor, BootEra);

    UtAssert_True(BPLib_TIME_GlobalData.TimeData.CfRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN] == CorrelationFactor,
                        "CF is expected value");
}

void Test_BPLib_TIME_GetDtnTimeFromBuffer(void)
{
    uint32_t BootEra = 0;

    UtAssert_True(BPLib_TIME_GetDtnTimeFromBuffer(BootEra) == 0, "DTN time is expected value");
}

void Test_BPLib_TIME_SetDtnTimeInBuffer(void)
{
    int64_t  DtnTime = 0;
    uint32_t BootEra = 0;

    BPLib_TIME_SetDtnTimeInBuffer(DtnTime, BootEra);

    UtAssert_True(BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN] == DtnTime,
                        "DTN time is expected value");
}

void Test_BPLib_TIME_ReadTimeDataFromFile(void)
{
    UtAssert_INT32_EQ(BPLib_TIME_ReadTimeDataFromFile(), BPLIB_SUCCESS);
}

void Test_BPLib_TIME_WriteTimeDataToFile(void)
{
    UtAssert_INT32_EQ(BPLib_TIME_WriteTimeDataToFile(), BPLIB_SUCCESS);
}

void Test_BPLib_TIME_GetEstimatedDtnTime(void)
{
    BPLib_TIME_MonotonicTime_t MonotonicTime;

    UtAssert_True(BPLib_TIME_GetEstimatedDtnTime(MonotonicTime) == 0, 
                            "Estimated DTN time is expected value");
}


void TestBplibTimeInternal_Register(void)
{
    ADD_TEST(Test_BPLib_TIME_GetCfFromBuffer);
    ADD_TEST(Test_BPLib_TIME_SetCfInBuffer);
    ADD_TEST(Test_BPLib_TIME_GetDtnTimeFromBuffer);
    ADD_TEST(Test_BPLib_TIME_SetDtnTimeInBuffer);
    ADD_TEST(Test_BPLib_TIME_ReadTimeDataFromFile);
    ADD_TEST(Test_BPLib_TIME_WriteTimeDataToFile);
    ADD_TEST(Test_BPLib_TIME_GetEstimatedDtnTime);
}
