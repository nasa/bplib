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

#include "bplib_eid_test_utils.h"

/* =========== */
/* Global Data */
/* =========== */

BPLib_EID_t EID_Test;
BPLib_EID_Pattern_t EID_Pattern;

void BPLib_EID_Test_Setup(void)
{
    /* Initialize test environment to default state for every test */

    memset(&EID_Test, 0, sizeof(BPLib_EID_t));
    memset(&EID_Pattern, 0, sizeof(BPLib_EID_Pattern_t));

    UT_ResetState(0);
}

void BPLib_EID_Test_Teardown(void)
{
    /* Clean up test environment */
}

void UtTest_Setup(void)
{
    TestBplibEid_Register();
}