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

#ifndef BPLIB_TIME_H
#define BPLIB_TIME_H

/*
** Include
*/

#include "bplib.h"


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
    uint32_t Year;
    uint32_t Day;
    uint32_t Hour;
    uint32_t Minute;
    uint32_t Second;
    uint32_t Msec;

} BPA_TIMEP_Epoch_t;

/**
**  \brief Enumerated types for the possible host clock states
*/
typedef enum 
{
    BPA_TIMEP_CLOCK_INVALID = 0,
    BPA_TIMEP_CLOCK_VALID   = 1

} BPA_TIMEP_ClockState_t;


/*
** Exported Functions
*/

/**
 * \brief Time Management initialization
 *
 *  \par Description
 *       TIME initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BP_SUCCESS Initialization was successful
 */
int BPLib_TIME_Init(void);

#endif /* BPLIB_TIME_H */
