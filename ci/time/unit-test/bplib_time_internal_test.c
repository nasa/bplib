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

/* Test getting a CF from the ring buffer */
void Test_BPLib_TIME_GetCfFromBuffer_Nominal(void)
{
    uint32_t BootEra;
    int64_t  ExpCf = 20;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN - 2;
    BootEra = BPLib_TIME_GlobalData.TimeData.CurrBootEra - 1;
    BPLib_TIME_GlobalData.TimeData.CfRingBuff[BootEra] = ExpCf;

    UtAssert_EQ(int64_t, BPLib_TIME_GetCfFromBuffer(BootEra), ExpCf);
}

/* Test getting a CF from the ring buffer when it wraps around */
void Test_BPLib_TIME_GetCfFromBuffer_WrapAround(void)
{
    uint32_t BootEra;
    int64_t  ExpCf = 20;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN + 1;
    BootEra = BPLIB_TIME_MAX_BUFFER_LEN;
    BPLib_TIME_GlobalData.TimeData.CfRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN] = ExpCf;

    UtAssert_EQ(int64_t, BPLib_TIME_GetCfFromBuffer(BootEra), ExpCf);
}

/* Test getting a CF from the ring buffer when the boot era is too big */
void Test_BPLib_TIME_GetCfFromBuffer_TooBig(void)
{
    uint32_t BootEra;
    int64_t  ExpCf = 0;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN - 2;
    BootEra = BPLib_TIME_GlobalData.TimeData.CurrBootEra + 1;

    UtAssert_EQ(int64_t, BPLib_TIME_GetCfFromBuffer(BootEra), ExpCf);
}

/* Test getting a CF from the ring buffer when the boot era is too small */
void Test_BPLib_TIME_GetCfFromBuffer_TooSmall(void)
{
    uint32_t BootEra;
    int64_t  ExpCf = 0;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN * 2;
    BootEra = 0;

    UtAssert_EQ(int64_t, BPLib_TIME_GetCfFromBuffer(BootEra), ExpCf);
}

/* Test setting a CF in the ring buffer */
void Test_BPLib_TIME_SetCfInBuffer_Nominal(void)
{
    uint32_t BootEra;
    int64_t  ExpCf = 100;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN - 2;
    BootEra = BPLib_TIME_GlobalData.TimeData.CurrBootEra - 1;

    BPLib_TIME_SetCfInBuffer(ExpCf, BootEra);

    UtAssert_EQ(int64_t, 
                BPLib_TIME_GlobalData.TimeData.CfRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN],
                ExpCf);
}

/* Test setting a CF in the ring buffer when it wraps around */
void Test_BPLib_TIME_SetCfInBuffer_WrapAround(void)
{
    int64_t  ExpCf   = 100;
    uint32_t BootEra;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN + 1;
    BootEra = BPLIB_TIME_MAX_BUFFER_LEN;

    BPLib_TIME_SetCfInBuffer(ExpCf, BootEra);

    UtAssert_EQ(int64_t,
                BPLib_TIME_GlobalData.TimeData.CfRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN],
                ExpCf);
}

/* Test setting a CF in the ring buffer when the boot era is too big */
void Test_BPLib_TIME_SetCfInBuffer_TooBig(void)
{
    uint32_t BootEra;
    int64_t  ExpCf = 1234;
    int64_t  NewCf = 5678;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN - 2;
    BootEra = BPLib_TIME_GlobalData.TimeData.CurrBootEra + 1;
    BPLib_TIME_GlobalData.TimeData.CfRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN] = ExpCf;

    BPLib_TIME_SetCfInBuffer(NewCf, BootEra);

    UtAssert_EQ(int64_t, 
                BPLib_TIME_GlobalData.TimeData.CfRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN], 
                ExpCf);
}

/* Test setting a CF in the ring buffer when the boot era is too small */
void Test_BPLib_TIME_SetCfInBuffer_TooSmall(void)
{
    uint32_t BootEra;
    int64_t  ExpCf = 1234;
    int64_t  NewCf = 5678;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN * 2;
    BootEra = 0;
    BPLib_TIME_GlobalData.TimeData.CfRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN] = ExpCf;

    BPLib_TIME_SetCfInBuffer(NewCf, BootEra);

    UtAssert_EQ(int64_t, 
                BPLib_TIME_GlobalData.TimeData.CfRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN], 
                ExpCf);
}

/* Test getting a last valid DTN time from the ring buffer */
void Test_BPLib_TIME_GetDtnTimeFromBuffer_Nominal(void)
{
    uint32_t BootEra;
    int64_t  ExpDtnTime = 20;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN - 2;
    BootEra = BPLib_TIME_GlobalData.TimeData.CurrBootEra - 1;
    BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN] = ExpDtnTime;

    UtAssert_EQ(int64_t, BPLib_TIME_GetDtnTimeFromBuffer(BootEra), ExpDtnTime);
}

/* Test getting a last valid DTN time from the ring buffer when it wraps around */
void Test_BPLib_TIME_GetDtnTimeFromBuffer_WrapAround(void)
{
    uint32_t BootEra;
    int64_t  ExpDtnTime = 20;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN + 1;
    BootEra = BPLIB_TIME_MAX_BUFFER_LEN;
    BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN] = ExpDtnTime;

    UtAssert_EQ(int64_t, BPLib_TIME_GetDtnTimeFromBuffer(BootEra), ExpDtnTime);
}

/* Test getting a last valid DTN time from the ring buffer when the boot era is too big */
void Test_BPLib_TIME_GetDtnTimeFromBuffer_TooBig(void)
{
    uint32_t BootEra;
    int64_t  ExpDtnTime = 0;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN - 2;
    BootEra = BPLib_TIME_GlobalData.TimeData.CurrBootEra + 1;

    UtAssert_EQ(int64_t, BPLib_TIME_GetDtnTimeFromBuffer(BootEra), ExpDtnTime);
}

/* Test getting a last valid DTN time from the ring buffer when the boot era is too small */
void Test_BPLib_TIME_GetDtnTimeFromBuffer_TooSmall(void)
{
    uint32_t BootEra;
    int64_t  ExpDtnTime = 0;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN * 2;
    BootEra = 0;

    UtAssert_EQ(int64_t, BPLib_TIME_GetDtnTimeFromBuffer(BootEra), ExpDtnTime);
}

/* Test setting a last valid DTN time in the ring buffer */
void Test_BPLib_TIME_SetDtnTimeInBuffer_Nominal(void)
{
    uint32_t BootEra;
    int64_t  ExpDtnTime = 100;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN - 2;
    BootEra = BPLib_TIME_GlobalData.TimeData.CurrBootEra - 1;

    BPLib_TIME_SetDtnTimeInBuffer(ExpDtnTime, BootEra);

    UtAssert_EQ(int64_t, 
                BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN],
                ExpDtnTime);
}

/* Test setting a last valid DTN time in the ring buffer when it wraps around */
void Test_BPLib_TIME_SetDtnTimeInBuffer_WrapAround(void)
{
    int64_t  ExpDtnTime = 100;
    uint32_t BootEra;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN + 1;
    BootEra = BPLIB_TIME_MAX_BUFFER_LEN;

    BPLib_TIME_SetDtnTimeInBuffer(ExpDtnTime, BootEra);

    UtAssert_EQ(int64_t,
                BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN],
                ExpDtnTime);
}

/* Test setting a last valid DTN time in the ring buffer when the boot era is too big */
void Test_BPLib_TIME_SetDtnTimeInBuffer_TooBig(void)
{
    uint32_t BootEra;
    int64_t  ExpDtnTime = 1234;
    int64_t  NewDtnTime = 5678;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN - 2;
    BootEra = BPLib_TIME_GlobalData.TimeData.CurrBootEra + 1;
    BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN] = ExpDtnTime;

    BPLib_TIME_SetDtnTimeInBuffer(NewDtnTime, BootEra);

    UtAssert_EQ(int64_t, 
                BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN], 
                ExpDtnTime);
}

/* Test setting a last valid DTN time in the ring buffer when the boot era is too small */
void Test_BPLib_TIME_SetDtnTimeInBuffer_TooSmall(void)
{
    uint32_t BootEra;
    int64_t  ExpDtnTime = 1234;
    int64_t  NewDtnTime = 5678;

    /* Set global/test data */
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BPLIB_TIME_MAX_BUFFER_LEN * 2;
    BootEra = 0;
    BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN] = ExpDtnTime;

    BPLib_TIME_SetDtnTimeInBuffer(NewDtnTime, BootEra);

    UtAssert_EQ(int64_t, 
                BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[BootEra % BPLIB_TIME_MAX_BUFFER_LEN], 
                ExpDtnTime);
}

/* Test reading time data when a new file has been created */
void Test_BPLib_TIME_ReadTimeDataFromFile_New(void)
{
    UT_SetDefaultReturnValue(UT_KEY(OS_read), 0);

    UtAssert_INT32_EQ(BPLib_TIME_ReadTimeDataFromFile(), BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(OS_read, 1);
}

/* Test reading time data when the file already exists */
void Test_BPLib_TIME_ReadTimeDataFromFile_Exists(void)
{
    UT_SetDefaultReturnValue(UT_KEY(OS_read), sizeof(BPLib_TIME_FileData_t));

    UtAssert_INT32_EQ(BPLib_TIME_ReadTimeDataFromFile(), BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(OS_read, 1);
}

/* Test reading time data when opening the file fails */
void Test_BPLib_TIME_ReadTimeDataFromFile_FailOpen(void)
{
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);

    UtAssert_INT32_EQ(BPLib_TIME_ReadTimeDataFromFile(), BPLIB_TIME_READ_ERROR);
    UtAssert_STUB_COUNT(OS_read, 0);
}

/* Test reading time data when reading the file fails */
void Test_BPLib_TIME_ReadTimeDataFromFile_FailRead(void)
{
    UT_SetDefaultReturnValue(UT_KEY(OS_read), sizeof(BPLib_TIME_FileData_t) - 1);

    UtAssert_INT32_EQ(BPLib_TIME_ReadTimeDataFromFile(), BPLIB_TIME_READ_ERROR);
    UtAssert_STUB_COUNT(OS_read, 1);
}

/* Test writing time data in nominal case */
void Test_BPLib_TIME_WriteTimeDataToFile_Nominal(void)
{
    UT_SetDefaultReturnValue(UT_KEY(OS_write), sizeof(BPLib_TIME_FileData_t));

    UtAssert_INT32_EQ(BPLib_TIME_WriteTimeDataToFile(), BPLIB_SUCCESS);
    UtAssert_STUB_COUNT(OS_write, 1);
}

/* Test writing time data when opening the file fails */
void Test_BPLib_TIME_WriteTimeDataToFile_FailOpen(void)
{
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);

    UtAssert_INT32_EQ(BPLib_TIME_WriteTimeDataToFile(), BPLIB_TIME_WRITE_ERROR);
    UtAssert_STUB_COUNT(OS_write, 0);
}

/* Test writing time data when writing to the file fails */
void Test_BPLib_TIME_WriteTimeDataToFile_FailWrite(void)
{
    UT_SetDefaultReturnValue(UT_KEY(OS_write), OS_ERROR);

    UtAssert_INT32_EQ(BPLib_TIME_WriteTimeDataToFile(), BPLIB_TIME_WRITE_ERROR);
    UtAssert_STUB_COUNT(OS_write, 1);
}

/* Test that DTN time is returned when an absolute boot era is passed in */
void Test_BPLib_TIME_GetEstimatedDtnTime_AbsBootEra(void)
{
    BPLib_TIME_MonotonicTime_t MonotonicTime;
    uint64_t ExpDtnTime = 1234;

    /* Set test data */
    MonotonicTime.Time = ExpDtnTime;
    MonotonicTime.BootEra = BPLIB_TIME_INVALID_BOOT_ERA;

    UtAssert_EQ(int64_t, BPLib_TIME_GetEstimatedDtnTime(MonotonicTime), ExpDtnTime);
}

/* Test that DTN time is returned when an exact time can be calculated */
void Test_BPLib_TIME_GetEstimatedDtnTime_ExactTime(void)
{
    BPLib_TIME_MonotonicTime_t MonotonicTime;
    uint64_t ExpDtnTime;
    uint32_t BootEra = 1;
    int64_t  ExpCf = 5678;

    /* Set global data */
    BPLib_TIME_GlobalData.TimeData.CfRingBuff[BootEra] = ExpCf;
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = 2;

    /* Set test data */
    MonotonicTime.Time = 1234;
    MonotonicTime.BootEra = BootEra;
    ExpDtnTime = MonotonicTime.Time + ExpCf;

    UtAssert_EQ(int64_t, BPLib_TIME_GetEstimatedDtnTime(MonotonicTime), ExpDtnTime);
}

/* Test that DTN time is returned when an estimated time can be calculated */
void Test_BPLib_TIME_GetEstimatedDtnTime_EstTime(void)
{
    BPLib_TIME_MonotonicTime_t MonotonicTime;
    uint64_t ExpDtnTime;
    uint32_t BootEra = 1;
    int64_t  ExpLastValidTime = 5678;

    /* Set global data */
    BPLib_TIME_GlobalData.TimeData.DtnTimeRingBuff[BootEra - 1] = ExpLastValidTime;
    BPLib_TIME_GlobalData.TimeData.CurrBootEra = BootEra + 1;

    /* Set test data */
    MonotonicTime.Time = 1234;
    MonotonicTime.BootEra = BootEra;
    ExpDtnTime = MonotonicTime.Time + ExpLastValidTime;

    UtAssert_EQ(int64_t, BPLib_TIME_GetEstimatedDtnTime(MonotonicTime), ExpDtnTime);
}

/* Test that 0 is returned when no estimated time can be calculated */
void Test_BPLib_TIME_GetEstimatedDtnTime_Undef(void)
{
    BPLib_TIME_MonotonicTime_t MonotonicTime;
    uint64_t ExpDtnTime;
    uint32_t BootEra = 1;

    /* Set test data */
    MonotonicTime.Time = 1234;
    MonotonicTime.BootEra = BootEra;
    ExpDtnTime = 0;

    UtAssert_EQ(int64_t, BPLib_TIME_GetEstimatedDtnTime(MonotonicTime), ExpDtnTime);
}

/* Test that the right offset is returned when the DTN epoch is the larger value */
void Test_BPLib_TIME_SafeOffset_DtnGth(void)
{
    /* Set test data */
    int64_t HostEpoch = 0x12341234;
    int64_t DtnEpoch = 0xdeadbeef;
    int64_t Multiplier = 0x2;
    int64_t ExpOffset = (DtnEpoch - HostEpoch) * Multiplier;
    
    ExpOffset *= -1;

    UtAssert_EQ(int64_t, BPLib_TIME_SafeOffset(HostEpoch, DtnEpoch, Multiplier), ExpOffset);
}

/* Test that the right offset is returned when the host epoch is the larger value */
void Test_BPLib_TIME_SafeOffset_HostGth(void)
{
    /* Set test data */
    int64_t HostEpoch = 0x12341234;
    int64_t DtnEpoch = 0x00005678;
    int64_t Multiplier = 0xbeef;
    int64_t ExpOffset = (HostEpoch - DtnEpoch) * Multiplier;

    UtAssert_EQ(int64_t, BPLib_TIME_SafeOffset(HostEpoch, DtnEpoch, Multiplier), ExpOffset);
}

/* Test that the right offset is returned when the host epoch is the Unix epoch */
void Test_BPLib_TIME_GetEpochOffset_Nominal(void)
{
    int64_t ExpEpochOffset;

    /* Set test epoch data */
    TestHostEpochYear = 1970;
    TestHostEpochDay = 1;

    /* Calculate expected epoch offset */
    ExpEpochOffset = BPLIB_TIME_EPOCH_YEAR - TestHostEpochYear;
    ExpEpochOffset *= BPLIB_TIME_YEAR_IN_MSEC;

    /* Leap years: 1972, 1976, 1980, 1984, 1988, 1992, 1996 */
    ExpEpochOffset += (7 * BPLIB_TIME_DAY_IN_MSEC);
    ExpEpochOffset *= -1;

    UtAssert_EQ(int64_t, ExpEpochOffset, BPLib_TIME_GetEpochOffset());
}

/* Test that the right offset is returned when the host epoch is the year 2020 */
void Test_BPLib_TIME_GetEpochOffset_HostGthDtn(void)
{
    int64_t ExpEpochOffset;

    /* Set test epoch data */
    TestHostEpochYear = 2020;
    TestHostEpochDay = 1;

    /* Calculate expected epoch offset */
    ExpEpochOffset = TestHostEpochYear - BPLIB_TIME_EPOCH_YEAR;
    ExpEpochOffset *= BPLIB_TIME_YEAR_IN_MSEC;

    /* Leap years: 2000, 2004, 2008, 2012, 2016 */
    ExpEpochOffset += (5 * BPLIB_TIME_DAY_IN_MSEC);

    UtAssert_EQ(int64_t, ExpEpochOffset, BPLib_TIME_GetEpochOffset());
}


/* Test that the right offset is returned when the host epoch equals the DTN epoch */
void Test_BPLib_TIME_GetEpochOffset_EqualEpochs(void)
{
    int64_t ExpEpochOffset;

    /* Set test epoch data */
    TestHostEpochYear = 2000;
    TestHostEpochDay = 1;

    /* Calculate expected epoch offset */
    ExpEpochOffset = 0;

    UtAssert_EQ(int64_t, ExpEpochOffset, BPLib_TIME_GetEpochOffset());
}

void TestBplibTimeInternal_Register(void)
{
    ADD_TEST(Test_BPLib_TIME_GetCfFromBuffer_Nominal);
    ADD_TEST(Test_BPLib_TIME_GetCfFromBuffer_WrapAround);
    ADD_TEST(Test_BPLib_TIME_GetCfFromBuffer_TooBig);
    ADD_TEST(Test_BPLib_TIME_GetCfFromBuffer_TooSmall);

    ADD_TEST(Test_BPLib_TIME_SetCfInBuffer_Nominal);
    ADD_TEST(Test_BPLib_TIME_SetCfInBuffer_WrapAround);
    ADD_TEST(Test_BPLib_TIME_SetCfInBuffer_TooBig);
    ADD_TEST(Test_BPLib_TIME_SetCfInBuffer_TooSmall);
    
    ADD_TEST(Test_BPLib_TIME_GetDtnTimeFromBuffer_Nominal);
    ADD_TEST(Test_BPLib_TIME_GetDtnTimeFromBuffer_WrapAround);
    ADD_TEST(Test_BPLib_TIME_GetDtnTimeFromBuffer_TooBig);
    ADD_TEST(Test_BPLib_TIME_GetDtnTimeFromBuffer_TooSmall);

    ADD_TEST(Test_BPLib_TIME_SetDtnTimeInBuffer_Nominal);
    ADD_TEST(Test_BPLib_TIME_SetDtnTimeInBuffer_WrapAround);
    ADD_TEST(Test_BPLib_TIME_SetDtnTimeInBuffer_TooBig);
    ADD_TEST(Test_BPLib_TIME_SetDtnTimeInBuffer_TooSmall);
    
    ADD_TEST(Test_BPLib_TIME_ReadTimeDataFromFile_New);
    ADD_TEST(Test_BPLib_TIME_ReadTimeDataFromFile_Exists);
    ADD_TEST(Test_BPLib_TIME_ReadTimeDataFromFile_FailOpen);
    ADD_TEST(Test_BPLib_TIME_ReadTimeDataFromFile_FailRead);
    
    ADD_TEST(Test_BPLib_TIME_WriteTimeDataToFile_Nominal);
    ADD_TEST(Test_BPLib_TIME_WriteTimeDataToFile_FailOpen);
    ADD_TEST(Test_BPLib_TIME_WriteTimeDataToFile_FailWrite);
    
    ADD_TEST(Test_BPLib_TIME_GetEstimatedDtnTime_AbsBootEra);
    ADD_TEST(Test_BPLib_TIME_GetEstimatedDtnTime_EstTime);
    ADD_TEST(Test_BPLib_TIME_GetEstimatedDtnTime_ExactTime);
    ADD_TEST(Test_BPLib_TIME_GetEstimatedDtnTime_Undef);

    ADD_TEST(Test_BPLib_TIME_SafeOffset_DtnGth);
    ADD_TEST(Test_BPLib_TIME_SafeOffset_HostGth);

    ADD_TEST(Test_BPLib_TIME_GetEpochOffset_Nominal);
    ADD_TEST(Test_BPLib_TIME_GetEpochOffset_HostGthDtn);
    ADD_TEST(Test_BPLib_TIME_GetEpochOffset_EqualEpochs);
}
