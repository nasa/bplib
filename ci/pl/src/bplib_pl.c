/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */

/*
** Include
*/

#include "bplib_pl.h"
#include "bplib_fwp.h"


/*
** Function Definitions
*/

int BPLib_PL_Init(void) {
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_PL_PerfLogEntry(uint32_t PerfLogID)
{
    if (BPLib_FWP_ProxyCallbacks.BPA_PERFLOGP_Entry)
    {
        BPLib_FWP_ProxyCallbacks.BPA_PERFLOGP_Entry(PerfLogID);
    }
    else 
    {
        return BPLIB_PL_NULL_CALLBACK_ERROR;                
    }
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_PL_PerfLogExit(uint32_t PerfLogID)
{
    if (BPLib_FWP_ProxyCallbacks.BPA_PERFLOGP_Exit)
    {
        BPLib_FWP_ProxyCallbacks.BPA_PERFLOGP_Exit(PerfLogID);
    }
    else 
    {
        return BPLIB_PL_NULL_CALLBACK_ERROR;        
    }
    return BPLIB_SUCCESS;        
}
