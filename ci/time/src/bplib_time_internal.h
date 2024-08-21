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
#include "bplib_fwp.h"

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
** Global Data
*/

extern BPLib_TIME_GlobalData_t BPLib_TIME_GlobalData;
extern BPLib_FWP_ProxyCallbacks_t BPLib_FWP_ProxyCallbacks;


/*
** Exported Functions
*/

/**
 * \brief Read Correlation Factor (CF) from Ring Buffer
 *
 *  \par Description
 *       Gets the CF for a given boot era from the ring buffer file
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] CorrelationFactor Pointer to return CF in
 * 
 *  \param[in] BootEra Entry in ring buffer to read from
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Operation was successful
 *  \retval BPLIB_TIME_READ_ERROR Read operation could not be completed
 */
BPLib_Status_t BPLib_TIME_ReadCfFromBuffer(int64_t *CorrelationFactor, uint32_t BootEra);

/**
 * \brief Write Correlation Factor (CF) to Ring Buffer
 *
 *  \par Description
 *       Writes the CF for a given boot era to the ring buffer file
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] CorrelationFactor Value to write to file
 * 
 *  \param[in] BootEra Entry in ring buffer to write to
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Operation was successful
 *  \retval BPLIB_TIME_WRITE_ERROR Write operation could not be completed
 */
BPLib_Status_t BPLib_TIME_WriteCfToBuffer(int64_t CorrelationFactor, uint32_t BootEra);

/**
 * \brief Read Last Valid DTN Time from Ring Buffer
 *
 *  \par Description
 *       Gets the last valid DTN time for a given boot era from the ring buffer file
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] LastValidDtnTime Pointer to return DTN time in
 * 
 *  \param[in] BootEra Entry in ring buffer to read from
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Operation was successful
 *  \retval BPLIB_TIME_READ_ERROR Read operation could not be completed
 */
BPLib_Status_t BPLib_TIME_ReadDtnTimeFromBuffer(int64_t *LastValidDtnTime, uint32_t BootEra);

/**
 * \brief Write Last Valid DTN Time to Ring Buffer
 *
 *  \par Description
 *       Writes the last valid DTN time for a given boot era to the ring buffer file
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] LastValidDtnTime Value to write to file
 * 
 *  \param[in] BootEra Entry in ring buffer to write to
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Operation was successful
 *  \retval BPLIB_TIME_WRITE_ERROR Write operation could not be completed
 */
BPLib_Status_t BPLib_TIME_WriteDtnTimeToBuffer(int64_t LastValidDtnTime, uint32_t BootEra);

/**
 * \brief Get Estimated DTN Time
 *
 *  \par Description
 *       Gets the DTN time from the given monotonic time. If no CF can be found for the
 *       given boot era, attempt to use a last valid DTN time value to estimate the DTN
 *       time. If no value exists, return 0.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] MonotonicTime Monotonic time to convert to DTN time
 *
 *  \return DTN time
 *  \retval Zero A valid DTN time could not be calculated
 *  \retval Nonzero Estimated DTN time that may be less than the true time but not greater
 */
uint64_t BPLib_TIME_GetEstimatedDtnTime(BPLib_TIME_MonotonicTime_t MonotonicTime);

#endif /* BPLIB_TIME_INTERNAL_H */
