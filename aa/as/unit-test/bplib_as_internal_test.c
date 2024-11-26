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

/* ======== */
/* Includes */
/* ======== */

#include "bplib_as_test_utils.h"

void Test_BPLib_AS_EidIsValid_Nominal(void)
{
    UtAssert_BOOL_TRUE(BPLib_AS_EidIsValid(0));
    UtAssert_BOOL_TRUE(BPLib_AS_EidIsValid(5));
    UtAssert_BOOL_TRUE(BPLib_AS_EidIsValid(BPLIB_MAX_NUM_SOURCE_EID - 1));
}

void Test_BPLib_AS_EidIsValid_Error(void)
{
    UtAssert_BOOL_FALSE(BPLib_AS_EidIsValid(-1));
    UtAssert_BOOL_FALSE(BPLib_AS_EidIsValid(BPLIB_MAX_NUM_SOURCE_EID));
}

void TestBplibAsInternal_Register(void)
{
    ADD_TEST(Test_BPLib_AS_EidIsValid_Nominal);
    ADD_TEST(Test_BPLib_AS_EidIsValid_Error);
}