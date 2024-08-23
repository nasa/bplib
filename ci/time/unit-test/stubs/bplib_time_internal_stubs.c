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
 * Generated stub function for BPLib_TIME_ReadCfFromBuffer()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TIME_ReadCfFromBuffer(int64_t *CorrelationFactor, uint32_t BootEra)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_ReadCfFromBuffer, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_TIME_ReadCfFromBuffer, int64_t *, CorrelationFactor);
    UT_GenStub_AddParam(BPLib_TIME_ReadCfFromBuffer, uint32_t, BootEra);

    UT_GenStub_Execute(BPLib_TIME_ReadCfFromBuffer, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_ReadCfFromBuffer, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_ReadDtnTimeFromBuffer()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TIME_ReadDtnTimeFromBuffer(uint64_t *LastValidDtnTime, uint32_t BootEra)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_ReadDtnTimeFromBuffer, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_TIME_ReadDtnTimeFromBuffer, uint64_t *, LastValidDtnTime);
    UT_GenStub_AddParam(BPLib_TIME_ReadDtnTimeFromBuffer, uint32_t, BootEra);

    UT_GenStub_Execute(BPLib_TIME_ReadDtnTimeFromBuffer, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_ReadDtnTimeFromBuffer, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_WriteCfToBuffer()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TIME_WriteCfToBuffer(int64_t CorrelationFactor, uint32_t BootEra)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_WriteCfToBuffer, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_TIME_WriteCfToBuffer, int64_t, CorrelationFactor);
    UT_GenStub_AddParam(BPLib_TIME_WriteCfToBuffer, uint32_t, BootEra);

    UT_GenStub_Execute(BPLib_TIME_WriteCfToBuffer, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_WriteCfToBuffer, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TIME_WriteDtnTimeToBuffer()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TIME_WriteDtnTimeToBuffer(uint64_t LastValidDtnTime, uint32_t BootEra)
{
    UT_GenStub_SetupReturnBuffer(BPLib_TIME_WriteDtnTimeToBuffer, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_TIME_WriteDtnTimeToBuffer, uint64_t, LastValidDtnTime);
    UT_GenStub_AddParam(BPLib_TIME_WriteDtnTimeToBuffer, uint32_t, BootEra);

    UT_GenStub_Execute(BPLib_TIME_WriteDtnTimeToBuffer, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_TIME_WriteDtnTimeToBuffer, BPLib_Status_t);
}
