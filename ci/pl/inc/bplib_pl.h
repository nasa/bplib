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

#ifndef BPLIB_PL_H
#define BPLIB_PL_H

/*
** Include
*/

#include "bplib.h"


/*
** Exported Functions
*/

/**
 * \brief Performance Logger initialization
 *
 *  \par Description
 *       PD initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
int BPLib_PL_Init(void);

/**
 * \brief FWP PerfLog Proxy Entry
 *
 *  \par Description
 *       Entry marker for use with Software Performance Tool
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] Perf Log ID
 * 
 *  \return Execution status
 *  \retval BPLIB_SUCCESS BPLib_PL_PerfLogEntry was successful
 *  \retval BPLIB_PL_NULL_CALLBACK_ERROR when the callback is NULL
 */
BPLib_Status_t BPLib_PL_PerfLogEntry(uint32_t PerfLogID);

/**
 * \brief FWP PerfLog Proxy Exit
 *
 *  \par Description
 *       Exit marker for use with Software Performance Tool
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] Perf Log ID
 * 
 *  \return Execution status
 *  \retval BPLIB_SUCCESS BPLib_PL_PerfLogExit was successful
 *  \retval BPLIB_PL_NULL_CALLBACK_ERROR when the callback is NULL
 */
BPLib_Status_t BPLib_PL_PerfLogExit(uint32_t PerfLogID);

#endif /* BPLIB_PL_H */
