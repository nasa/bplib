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

#include "bplib_pi_test_utils.h"


/*
** Global Data
*/

BPLib_Instance_t BplibInst;
BPLib_PI_ChannelTable_t TestChanTbl;
BPLib_NC_MibPerNodeConfig_t TestMibTbl;

/*
** Function Definitions
*/

void BPLib_PI_Test_Setup(void)
{
    /* Initialize test environment to default state for every test */
    UT_ResetState(0);

    memset(&BplibInst, 0, sizeof(BPLib_Instance_t));

    BPLib_NC_ConfigPtrs.ChanConfigPtr = &TestChanTbl;
    BPLib_NC_ConfigPtrs.MibPnConfigPtr = &TestMibTbl;

    TestMibTbl.Configs[PARAM_SET_MAX_SEQUENCE_NUM] = 100;

    BPLib_FWP_ProxyCallbacks.BPA_ADUP_AddApplication = BPA_ADUP_AddApplication;
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StartApplication = BPA_ADUP_StartApplication;
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StopApplication = BPA_ADUP_StopApplication;
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_RemoveApplication = BPA_ADUP_RemoveApplication;

    UT_SetHandlerFunction(UT_KEY(BPLib_EM_SendEvent), UT_Handler_BPLib_EM_SendEvent, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_QM_WaitQueueTryPull), UT_Handler_BPLib_QM_WaitQueueTryPull, NULL);

    memset(&BPLib_PI_SequenceNums, 0, sizeof(BPLib_PI_SequenceNums));
}

void BPLib_PI_Test_Teardown(void)
{
    /* Clean up test environment */
}

void UtTest_Setup(void)
{
    TestBplibPi_Register();
}
