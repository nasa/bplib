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
 * Auto-Generated stub implementations for functions defined in bplib_time_internal header
 */

#include "bplib_time_internal.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_GetCfFromBuffer()
 * ----------------------------------------------------
 */
int64_t BPLib_TIME_GetCfFromBuffer(uint32_t BootEra)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_GetCfFromBuffer, int64_t);

    UT_GenStub_AddParam(BPLib_TIME_GetCfFromBuffer, uint32_t, BootEra);

    UT_GenStub_Execute(BPLib_TIME_GetCfFromBuffer, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_GetCfFromBuffer, int64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_GetDtnTimeFromBuffer()
 * ----------------------------------------------------
 */
uint64_t BPLib_TIME_GetDtnTimeFromBuffer(uint32_t BootEra)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_GetDtnTimeFromBuffer, uint64_t);

    UT_GenStub_AddParam(BPLib_TIME_GetDtnTimeFromBuffer, uint32_t, BootEra);

    UT_GenStub_Execute(BPLib_TIME_GetDtnTimeFromBuffer, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_GetDtnTimeFromBuffer, uint64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_GetEpochOffset()
 * ----------------------------------------------------
 */
int64_t BPLib_TIME_GetEpochOffset(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_GetEpochOffset, int64_t);

    UT_GenStub_Execute(BPLib_TIME_GetEpochOffset, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_GetEpochOffset, int64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_GetEstimatedDtnTime()
 * ----------------------------------------------------
 */
uint64_t BPLib_TIME_GetEstimatedDtnTime(BPLib_TIME_MonotonicTime_t MonotonicTime)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_GetEstimatedDtnTime, uint64_t);

    UT_GenStub_AddParam(BPLib_TIME_GetEstimatedDtnTime, BPLib_TIME_MonotonicTime_t, MonotonicTime);

    UT_GenStub_Execute(BPLib_TIME_GetEstimatedDtnTime, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_GetEstimatedDtnTime, uint64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_ReadTimeDataFromFile()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TIME_ReadTimeDataFromFile(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_ReadTimeDataFromFile, BPLib_Status_t);

    UT_GenStub_Execute(BPLib_TIME_ReadTimeDataFromFile, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_ReadTimeDataFromFile, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_SafeOffset()
 * ----------------------------------------------------
 */
int64_t BPLib_TIME_SafeOffset(int64_t HostEpoch, int64_t DtnEpoch, int64_t Multiplier)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_SafeOffset, int64_t);

    UT_GenStub_AddParam(BPLib_TIME_SafeOffset, int64_t, HostEpoch);
    UT_GenStub_AddParam(BPLib_TIME_SafeOffset, int64_t, DtnEpoch);
    UT_GenStub_AddParam(BPLib_TIME_SafeOffset, int64_t, Multiplier);

    UT_GenStub_Execute(BPLib_TIME_SafeOffset, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_SafeOffset, int64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_SetCfInBuffer()
 * ----------------------------------------------------
 */
void BPLib_TIME_SetCfInBuffer(int64_t CF, uint32_t BootEra)
{
    UT_GenStub_AddParam(BPLib_TIME_SetCfInBuffer, int64_t, CF);
    UT_GenStub_AddParam(BPLib_TIME_SetCfInBuffer, uint32_t, BootEra);

    UT_GenStub_Execute(BPLib_TIME_SetCfInBuffer, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_SetDtnTimeInBuffer()
 * ----------------------------------------------------
 */
void BPLib_TIME_SetDtnTimeInBuffer(uint64_t DtnTime, uint32_t BootEra)
{
    UT_GenStub_AddParam(BPLib_TIME_SetDtnTimeInBuffer, uint64_t, DtnTime);
    UT_GenStub_AddParam(BPLib_TIME_SetDtnTimeInBuffer, uint32_t, BootEra);

    UT_GenStub_Execute(BPLib_TIME_SetDtnTimeInBuffer, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_WriteTimeDataToFile()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TIME_WriteTimeDataToFile(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_WriteTimeDataToFile, BPLib_Status_t);

    UT_GenStub_Execute(BPLib_TIME_WriteTimeDataToFile, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_WriteTimeDataToFile, BPLib_Status_t);
}
