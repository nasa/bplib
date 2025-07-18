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
 * Public header file for the Time Management component
 */

#ifndef BPLIB_TIME_H
#define BPLIB_TIME_H

/*
** Include
*/

#include "bplib_api_types.h"


/*
** Macro Definitions
*/

/**
** \brief Time data file name 
*/
#define BPLIB_TIME_FILE_NAME "/cf/bplib_time.dat"

/**
** \brief Length of the time data ring buffers
*/
#define BPLIB_TIME_MAX_BUFFER_LEN   32


/*
** Type Definitions
*/

/**
**  \brief Epoch structure
**
**  \par Description
**       Structure containing the values that make up an epoch: year, day, hour, minute,
**       second, and millisecond.
*/
typedef struct
{
    uint16_t Year;
    uint16_t Day;
    uint16_t Hour;
    uint16_t Minute;
    uint16_t Second;
    uint16_t Msec;

} BPLib_TIME_Epoch_t;

/**
**  \brief Enumerated types for the possible host clock states
*/
typedef enum 
{
    BPLIB_TIME_CLOCK_INVALID = 0,
    BPLIB_TIME_CLOCK_VALID   = 1

} BPLib_TIME_ClockState_t;

/**
**  \brief Monotonic time
**
**  \par Description
**       Monotonic time is associated with a time elapsed counter and a boot era where
**       that counter started from.
*/
typedef struct 
{
    int64_t  Time;                      /**< \brief Time elapsed counter (in msecs) */
    uint32_t BootEra;                   /**< \brief Boot era that counter started from */
} BPLib_TIME_MonotonicTime_t;


/*
** Exported Functions
*/

/**
 * \brief Time Management initialization
 *
 *  \par Description
 *       Initialize Time Management by initializing global memory, calculating the epoch 
 *       offset, reading in the contents of the time data file, incrementing the boot era,
 *       and performing the maintenance activities.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 *  \retval BPLIB_TIME_READ_ERROR Error reading in the time data file
 *  \retval BPLIB_TIME_WRITE_ERROR Error writing to the time data file
 */
BPLib_Status_t BPLib_TIME_Init(void);

/**
 * \brief Get Monotonic Time
 *
 *  \par Description
 *       Gets current monotonic time and boot era
 *
 *  \par Assumptions, External Events, and Notes:
 *       - Time Management must already be initialized (see BPLib_TIME_Init)
 * 
 */
int64_t  BPLib_TIME_GetMonotonicTime(void);

uint32_t BPLib_TIME_GetBootEra(void);

/**
 * \brief Calculate Correlation Factor
 *
 *  \par Description
 *       Returns Correlation Factor (CF) by calculating the following:
 *       CF = (Host_Time - Monotonic_Time) + Epoch_Offset
 *
 *  \par Assumptions, External Events, and Notes:
 *       - Time Management must already be initialized (see BPLib_TIME_Init)
 *
 *  \return Correlation Factor
 */
int64_t  BPLib_TIME_CalculateCorrelationFactor(void);

/**
 * \brief Get Correlation Factor
 *
 *  \par Description
 *       Returns current Correlation Factor (CF)
 *
 *  \par Assumptions, External Events, and Notes:
 *       - Time Management must already be initialized (see BPLib_TIME_Init)
 *
 *  \return Correlation Factor
 */
int64_t  BPLib_TIME_GetCorrelationFactor(void);

/**
 * \brief Get DTN Time
 *
 *  \par Description
 *       Returns DTN time for given monotonic time by calculating the following:
 *       DTN_Time = Monotonic_Time + CF
 *
 *  \par Assumptions, External Events, and Notes:
 *       - Time Management must already be initialized (see BPLib_TIME_Init)
 * 
 *  \param[in] MonotonicTime Monotonic time to convert to DTN time
 *
 *  \return DTN time
 *  \retval Zero A valid DTN time could not be calculated
 *  \retval Nonzero A valid DTN time
 */
uint64_t BPLib_TIME_GetDtnTime(BPLib_TIME_MonotonicTime_t MonotonicTime);

uint64_t BPLib_TIME_GetCurrentDtnTime(void);

/**
 * \brief Get Time Delta
 *
 *  \par Description
 *       Gets the delta between two monotonic time values
 *       - If the two times are in the same boot era, a subtraction operation is done
 *       - If the two times can be converted to DTN times, convert and subtract
 *       - If the two times can be converted to estimated DTN times, convert and subtract.
 *         This delta may be less than the true delta, but never greater.
 *       - Else, return an error.
 *
 *  \par Assumptions, External Events, and Notes:
 *       - Time Management must already be initialized (see BPLib_TIME_Init)
 * 
 *  \param[in] Time1 First time value
 * 
 *  \param[in] Time2 Second time value, subtracted from Time1
 *
 *  \param[in] Delta Pointer to delta value to be set
 * 
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Calculation was successful
 *  \retval BPLIB_TIME_UNDEF_DELTA_ERROR Calculation could not be done
 */
BPLib_Status_t BPLib_TIME_GetTimeDelta(BPLib_TIME_MonotonicTime_t Time1,
                                       BPLib_TIME_MonotonicTime_t Time2, int64_t *Delta);

/**
 * \brief Time Maintenance Activities
 *
 *  \par Description
 *       Perform the following maintenance activities:
 *       - Calculate the current CF
 *       - If there's a new valid CF, save it in the ring buffer file
 *       - With the new CF, calculate the current DTN time and save it in the file
 *
 *  \par Assumptions, External Events, and Notes:
 *       - Maintenance activities should be triggered by some external signal 
 *         on a regular basis
 *       - Time Management must already be initialized (see BPLib_TIME_Init)
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Maintenance activities were successful
 *  \retval BPLIB_TIME_WRITE_ERROR Error writing to the time data file
 */
BPLib_Status_t BPLib_TIME_MaintenanceActivities(void);

#endif /* BPLIB_TIME_H */
