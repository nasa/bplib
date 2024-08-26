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

#include "bplib_time_test_utils.h"


/*
** Function Definitions
*/

/*
** Stub function definitions for Time Proxy
*/

int64_t Test_BPA_TIMEP_GetMonotonicTime(void)
{
    return (int64_t) BPLIB_TIME_TEST_MONOTONIC_TIME_VALUE;
}

void Test_BPA_TIMEP_GetHostEpoch(BPLib_TIME_Epoch_t *Epoch)
{
    return;
}

BPLib_TIME_ClockState_t Test_BPA_TIMEP_GetHostClockState_Valid(void)
{
    return BPLIB_TIME_CLOCK_VALID;
}

BPLib_TIME_ClockState_t Test_BPA_TIMEP_GetHostClockState_Invalid(void)
{
    return BPLIB_TIME_CLOCK_INVALID;
}


int64_t Test_BPA_TIMEP_GetHostTime(void)
{
    return (int64_t) BPLIB_TIME_TEST_HOST_TIME_VALUE;
}

void BPLib_TIME_Test_Setup(void)
{
    /* Initialize test environment to default state for every test */
    UT_ResetState(0);

    /* Clear global data */
    memset(&BPLib_TIME_GlobalData, 0, sizeof(BPLib_TIME_GlobalData));

    /* Set up proxy callback functions */
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostClockState = Test_BPA_TIMEP_GetHostClockState_Valid;
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostEpoch = Test_BPA_TIMEP_GetHostEpoch;
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostTime = Test_BPA_TIMEP_GetHostTime;
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime = Test_BPA_TIMEP_GetMonotonicTime;
}

void BPLib_TIME_Test_Teardown(void)
{
    /* Clean up test environment */
}

void UtTest_Setup(void)
{
    TestBplibTime_Register();
    TestBplibTimeInternal_Register();
}
