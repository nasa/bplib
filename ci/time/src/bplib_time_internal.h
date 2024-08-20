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
 * Private header file for internal Time Management functions
 */

#ifndef BPLIB_TIME_INTERNAL_H
#define BPLIB_TIME_INTERNAL_H

/*
** Include
*/

#include "bplib_time.h"


/*
** Type Definitions
*/

/**
**  \brief Time Management Global Data
*/
typedef struct
{
    int64_t  CurrentCorrelationFactor;  /**< \brief Most recent CF */
    uint32_t CurrentBootEra;            /**< \brief Number of times time has rebooted */
    int64_t  EpochOffset;               /**< \brief Offset between host and DTN epochs */
} BPLib_TIME_GlobalData_t;


/*
** Exported Functions
*/

int64_t  BPLib_TIME_ReadCfFromBuffer(uint32_t BootEra);

BPLib_Status_t BPLib_TIME_StoreCfToBuffer(int64_t CorrelationFactor, uint32_t BootEra);

int64_t  BPLib_TIME_ReadDtnTimeFromBuffer(uint32_t BootEra);

BPLib_Status_t BPLib_TIME_StoreDtnTimeToBuffer(int64_t LastValidDtnTime, uint32_t BootEra);

uint64_t BPLib_TIME_GetEstimatedDtnTime(BPLib_TIME_MonotonicTime_t MonotonicTime);

#endif /* BPLIB_TIME_INTERNAL_H */
