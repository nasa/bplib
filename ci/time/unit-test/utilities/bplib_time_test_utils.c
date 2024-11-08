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
** Global Data
*/

uint16_t TestEpochYear;


/*
** Function Definitions
*/


/* */
void UT_BPA_TIMEP_GetHostEpoch_Handler(void *UserObj, UT_EntryKey_t FuncKey, 
                                              const UT_StubContext_t *Context)
{
    BPLib_TIME_Epoch_t *Epoch = (BPLib_TIME_Epoch_t *) UT_Hook_GetArgValueByName(Context, 
                                                            "Epoch", BPLib_TIME_Epoch_t *);

    memset(Epoch, 0, sizeof(BPLib_TIME_Epoch_t));
    
    Epoch->Year = TestEpochYear;
    Epoch->Day = 1;
}

void BPLib_TIME_Test_Setup(void)
{
    /* Initialize test environment to default state for every test */
    UT_ResetState(0);

    /* Clear global data */
    memset(&BPLib_TIME_GlobalData, 0, sizeof(BPLib_TIME_GlobalData));

    /* Set default initialization state */
    BPLib_TIME_GlobalData.InitState = BPLIB_TIME_INIT;

    /* Set up proxy callback functions */
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostClockState = BPA_TIMEP_GetHostClockState;
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostEpoch = BPA_TIMEP_GetHostEpoch;
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostTime = BPA_TIMEP_GetHostTime;
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetMonotonicTime = BPA_TIMEP_GetMonotonicTime;

    /* Default host epoch is 1970 */
    TestEpochYear = 1970;

    UT_SetHandlerFunction(UT_KEY(BPA_TIMEP_GetHostEpoch), 
                                UT_BPA_TIMEP_GetHostEpoch_Handler, NULL);

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
