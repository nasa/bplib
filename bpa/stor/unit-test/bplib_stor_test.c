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
 * Include
 */
#include "bplib_stor_test_utils.h"

/*
** Test function for
** int BPLib_STOR_Init()
*/
void Test_BPLib_STOR_Init(void)
{
    UtAssert_INT32_EQ(BPLib_STOR_Init(), BPLIB_SUCCESS);
}


void TestBplibStor_Register(void)
{
    UtTest_Add(Test_BPLib_STOR_Init, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_Init");
}
