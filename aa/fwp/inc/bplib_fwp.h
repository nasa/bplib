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

#ifndef BPLIB_FWP_H
#define BPLIB_FWP_H

/*
** Include
*/

#include "bplib.h"


/*
** Type Definitions
*/

typedef struct 
{
    /* Time Proxy function callbacks */
    int64_t (*BPA_TIMEP_GetMonotonicTime)(void);
    void (*BPA_TIMEP_GetHostEpoch)(BPLib_TIME_Epoch_t *Epoch);
    BPLib_TIME_ClockState_t (*BPA_TIMEP_GetHostClockState)(void);
    int64_t (*BPA_TIMEP_GetHostTime)(void);
    
    /* PerfLog Proxy function callbacks */
    void (*BPA_PERFLOGP_Entry)(uint32_t PerfLogID);
    void (*BPA_PERFLOGP_Exit)(uint32_t PerfLogID);

    /* Add other proxies' function callbacks here: TODO */

} BPLib_FWP_ProxyCallbacks_t;


/*
** Global Data
*/

extern BPLib_FWP_ProxyCallbacks_t BPLib_FWP_ProxyCallbacks;


/*
** Exported Functions
*/

/**
 * \brief Framework Proxy initialization
 *
 *  \par Description
 *       FWP initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
BPLib_Status_t BPLib_FWP_Init(BPLib_FWP_ProxyCallbacks_t Callbacks);

#endif /* BPLIB_FWP_H */
