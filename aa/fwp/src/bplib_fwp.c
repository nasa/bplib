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

/*
** Include
*/

#include "bplib_fwp.h"


/*
** Global Data
*/

BPLib_FWP_ProxyCallbacks_t BPLib_FWP_ProxyCallbacks;


/*
** Function Definitions
*/

/* Initialize proxy callbacks */
BPLib_Status_t BPLib_FWP_Init(BPLib_FWP_ProxyCallbacks_t Callbacks) {

    if (Callbacks.BPA_TIMEP_GetHostEpoch == NULL || 
        Callbacks.BPA_TIMEP_GetHostTime == NULL ||
        Callbacks.BPA_TIMEP_GetMonotonicTime == NULL)
    {
        return BPLIB_FWP_CALLBACK_INIT_ERROR;
    }
    else
    {
        /* Initialize Time Proxy callbacks */
        BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostEpoch = Callbacks.BPA_TIMEP_GetHostEpoch;
        BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostTime = Callbacks.BPA_TIMEP_GetHostTime;
        BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime = Callbacks.BPA_TIMEP_GetMonotonicTime;

        /* Initialize other proxies' callbacks TODO */
    }

    return BPLIB_SUCCESS;
}
