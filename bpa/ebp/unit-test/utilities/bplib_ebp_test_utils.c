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

#include "bplib_ebp_test_utils.h"
#include "bplib_time_handlers.h"

/*
** Global Data
*/

BPLib_PI_ChannelTable_t TestChanConfigPtr;


/*
** Function Definitions
*/

void BPLib_EBP_Test_Setup(void)
{
    /* Initialize test environment to default state for every test */
    UT_ResetState(0);

    UT_SetHandlerFunction(UT_KEY(BPLib_TIME_GetTimeDelta), UT_Handler_BPLib_TIME_GetTimeDelta, NULL);

    memset(&TestChanConfigPtr, 0, sizeof(TestChanConfigPtr));

    BPLib_NC_ConfigPtrs.ChanConfigPtr = &TestChanConfigPtr;
}

void BPLib_EBP_Test_Teardown(void)
{
    /* Clean up test environment */
}

void UtTest_Setup(void)
{
    TestBplibEbp_Register();
}
