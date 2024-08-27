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

#define BPLIB_TIME_MAX_BUFFER_LEN (32)

#define BPLIB_TIME_EPOCH_YEAR   (2000)
#define BPLIB_TIME_EPOCH_DAY    (0)
#define BPLIB_TIME_EPOCH_HOUR   (0)
#define BPLIB_TIME_EPOCH_MINUTE (0)
#define BPLIB_TIME_EPOCH_SECOND (0)
#define BPLIB_TIME_EPOCH_MSEC   (0)


/*
** Type Definitions
*/

/**
**  \brief Initialization state of Time Management component
*/
typedef enum
{
    BPLIB_TIME_UNINIT = 0,      /**< \brief Time Management has not initialized */
    BPLIB_TIME_INIT   = 0       /**< \brief Time Management has initialized */
} BPLib_TIME_InitState_t;

/**
**  \brief Data stored in time ring buffer file
*/
typedef struct 
{
    int32_t  CurrBootEra;                                /**< \brief Current boot era */
    int64_t  CfRingBuff[BPLIB_TIME_MAX_BUFFER_LEN];      /**< \brief Ring buffer of past CFs */
    uint64_t DtnTimeRingBuff[BPLIB_TIME_MAX_BUFFER_LEN]; /**< \brief Ring buffer of past last valid DTN time values */
} BPLib_TIME_FileData_t;

/**
**  \brief Time Management Global Data
*/
typedef struct
{
    int64_t   CurrentCf;                /**< \brief Most recent CF */
    int32_t   CurrentBootEra;           /**< \brief Number of times time has rebooted */
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
 *       Returns the CF for the provided boot era from the ring buffer
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] BootEra Entry in ring buffer to read from
 *
 *  \return Execution status
 *  \retval Correlation Factor
 */
int64_t BPLib_TIME_GetCfFromBuffer(uint32_t BootEra);

/**
 * \brief Set Correlation Factor (CF) in local ring buffer
 *
 *  \par Description
 *       Sets the CF for the provided boot era in the ring buffer
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
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] BootEra Entry in ring buffer to read from
 *
 *  \return Execution status
 *  \retval Last Valid DTN Time
 */
uint64_t BPLib_TIME_GetDtnTimeFromBuffer(uint32_t BootEra);

/**
 * \brief Set Last Valid DTN Time in local ring buffer
 *
 *  \par Description
 *       Sets the Last Valid DTN Time for the provided boot era in the ring buffer
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] BootEra Entry in ring buffer to set
 */
void BPLib_TIME_SetDtnTimeInBuffer(uint64_t DtnTime, uint32_t BootEra);

/**
 * \brief Read Last Valid DTN Time from Ring Buffer
 *
 *  \par Description
 *       Gets the last valid DTN time for a given boot era from the ring buffer
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
 *  \retval BPLIB_TIME_UNINIT_ERROR Time Management has not been initialized properly
 */
BPLib_Status_t BPLib_TIME_ReadDtnTimeFromBuffer(uint64_t *LastValidDtnTime, uint32_t BootEra);

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
