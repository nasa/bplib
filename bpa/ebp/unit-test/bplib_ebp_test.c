/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/*
 * Includes
 */
#include "bplib_ebp_test_utils.h"

/*
** Test function for
** int BPLib_EBP_Init()
*/
void Test_BPLib_EBP_Init(void)
{
    UtAssert_INT32_EQ(BPLib_EBP_Init(), BP_SUCCESS);
}


void TestBplibEbp_Register(void)
{
    UtTest_Add(Test_BPLib_EBP_Init, NULL, NULL, "Test_BPLib_EBP_Init");
}
