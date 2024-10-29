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
#include "osapi.h"


/*
** Macro Definitions
*/

#define BPLIB_TIME_EPOCH_YEAR       (2000)  /**< \brief DTN epoch year (2000) */
#define BPLIB_TIME_EPOCH_DAY        (1)     /**< \brief DTN epoch day (Jan 1) */
#define BPLIB_TIME_EPOCH_HOUR       (0)     /**< \brief DTN epoch hour (12am) */
#define BPLIB_TIME_EPOCH_MINUTE     (0)     /**< \brief DTN epoch minute (00:00) */
#define BPLIB_TIME_EPOCH_SECOND     (0)     /**< \brief DTN epoch second (0) */
#define BPLIB_TIME_EPOCH_MSEC       (0)     /**< \brief DTN epoch millisecond (0) */

#define BPLIB_TIME_YEAR_IN_MSEC     ((int64_t) 31536000000) /**< \brief Msecs in a year */
#define BPLIB_TIME_DAY_IN_MSEC      ((int64_t) 86400000)    /**< \brief Msecs in a day */
#define BPLIB_TIME_HOUR_IN_MSEC     ((int64_t) 3600000)     /**< \brief Msecs in an hour */
#define BPLIB_TIME_MINUTE_IN_MSEC   ((int64_t) 60000)       /**< \brief Msecs in a minute */
#define BPLIB_TIME_SECOND_IN_MSEC   ((int64_t) 1000)        /**< \brief Msecs in a second */

/**
**  \brief Invalid boot era, indicates that the associated time is an absolute DTN time
*/
#define BPLIB_TIME_INVALID_BOOT_ERA (0xffffffff)


/*
** Type Definitions
*/

/**
**  \brief Initialization state of Time Management component
*/
typedef enum
{
    BPLIB_TIME_UNINIT = 0,      /**< \brief Time Management has not initialized */
    BPLIB_TIME_INIT   = 1       /**< \brief Time Management has initialized */
} BPLib_TIME_InitState_t;

/**
**  \brief Data stored in time ring buffer file
*/
typedef struct 
{
    uint32_t CurrBootEra;                                /**< \brief Current boot era */
    int64_t  CfRingBuff[BPLIB_TIME_MAX_BUFFER_LEN];      /**< \brief Ring buffer of past CFs */
    uint64_t DtnTimeRingBuff[BPLIB_TIME_MAX_BUFFER_LEN]; /**< \brief Ring buffer of past last valid DTN time values */
} BPLib_TIME_FileData_t;

/**
**  \brief Time Management Global Data
*/
typedef struct
{
    int64_t   CurrentCf;                /**< \brief Most recent CF */
    int64_t   EpochOffset;              /**< \brief Offset between host and DTN epochs */
    osal_id_t FileHandle;               /**< \brief OSAL handle for time data file */
    BPLib_TIME_InitState_t InitState;   /**< \brief Initialization state of TIME */
    BPLib_TIME_FileData_t TimeData;     /**< \brief Local copy of time file data */
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
 * \brief Get Correlation Factor (CF) from local ring buffer
 *
 *  \par Description
 *       Returns the CF for the provided boot era from the ring buffer. If the boot era is
 *       invalid, 0 is returned.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] BootEra Entry in ring buffer to read from
 *
 *  \return Correlation Factor
 */
int64_t BPLib_TIME_GetCfFromBuffer(uint32_t BootEra);

/**
 * \brief Set Correlation Factor (CF) in local ring buffer
 *
 *  \par Description
 *       Sets the CF for the provided boot era in the ring buffer if the boot era is valid
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] BootEra Entry in ring buffer to set
 */
void BPLib_TIME_SetCfInBuffer(int64_t CF, uint32_t BootEra);

/**
 * \brief Get Last Valid DTN Time from local ring buffer
 *
 *  \par Description
 *       Returns the Last Valid DTN Time for the provided boot era from the ring buffer
 *       If the boot era is invalid, 0 is returned.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] BootEra Entry in ring buffer to read from
 *
 *  \return Last Valid DTN Time
 */
uint64_t BPLib_TIME_GetDtnTimeFromBuffer(uint32_t BootEra);

/**
 * \brief Set Last Valid DTN Time in local ring buffer
 *
 *  \par Description
 *       Sets the Last Valid DTN Time for the provided boot era in the ring buffer if the 
 *       boot era is valid.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] BootEra Entry in ring buffer to set
 */
void BPLib_TIME_SetDtnTimeInBuffer(uint64_t DtnTime, uint32_t BootEra);

/**
 * \brief Read Time Data from File
 *
 *  \par Description
 *       Read in the file data into the local time data structure
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Operation was successful
 *  \retval BPLIB_TIME_READ_ERROR Read operation could not be completed
 */
BPLib_Status_t BPLib_TIME_ReadTimeDataFromFile(void);

/**
 * \brief Write Time Data to File
 *
 *  \par Description
 *       Writes the current local time data to the time data file
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Operation was successful
 *  \retval BPLIB_TIME_WRITE_ERROR Write operation could not be completed
 */
BPLib_Status_t BPLib_TIME_WriteTimeDataToFile(void);

/**
 * \brief Get Estimated DTN Time
 *
 *  \par Description
 *       Gets the DTN time from the given monotonic time. If no CF can be found for the
 *       given boot era and there is a valid DTN time for the past boot era, attempt to 
 *       use a last valid DTN time value for the previous boot era to estimate the DTN 
 *       time. This functions as a timestamp for the last time the clock was valid before 
 *       time rebooted and can be used as a pseudo-CF. The pseudo-CF does not account for 
 *       any time elapsed while the system was down/ the clock was invalid, but as this 
 *       is intended to only be used by BPLib_TIME_GetTimeDelta, a value smaller than the 
 *       true value is an accepted error range. If no estimated DTN time can be found,
 *       the function returns 0.
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

/**
 * \brief Get Safe Epoch Offset
 *
 *  \par Description
 *       Gets the offset between the two epoch values and multiplies them to convert to
 *       milliseconds. The offset is positive if HostEpoch is greater, negative if
 *       DtnEpoch is greater.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] HostEpoch A host epoch value
 * 
 *  \param[in] DtnEpoch A DTN epoch value
 * 
 *  \param[in] Multiplier Value to multiply offset by to convert offset to milliseconds
 *
 *  \return Offset in milliseconds
 */
int64_t BPLib_TIME_SafeOffset(int64_t HostEpoch, int64_t DtnEpoch, int64_t Multiplier);

/**
 * \brief Get Epoch Offset
 *
 *  \par Description
 *       Gets the offset between the host epoch and the DTN epoch in units of milliseconds. 
 *       This includes leap days but not leap seconds.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Offset in milliseconds
 */
int64_t BPLib_TIME_GetEpochOffset(void);

#endif /* BPLIB_TIME_INTERNAL_H */
