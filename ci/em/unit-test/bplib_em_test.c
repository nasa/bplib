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
#include "bplib_em_test_utils.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

void Test_BPLib_EM_Init_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_Init();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPA_EM_SendEvent_Nominal(void)
{
    /* INFO, ERROR, DEBUG, CRITICAL events, each with a digit, string, then digit AND string for formatted string replacements */
    // UT_CHECKEVENT_SETUP(&EventTest, 42, "INFO event message test");
}

void Test_BPA_EM_SendEvent_TruncatedString(void)
{
    /* TODO: - Create char array of size BPLIB_EM_MAX_MESSAGE_LENGTH + 5 */
    /*       - Loop over array assigning each element in array to A (set last character to H) */
    /*       - Pass character array to BPLib_EM_SendEvent as EventText argument */

    /* Create a string that will be truncated */
    // char LongString[BPLIB_EM_MAX_MESSAGE_LENGTH + 5];
}

void Test_BPA_EVP_SendEvent_LenError(void)
{
    /* TODO: Test if len < 2 */
    /* TODO: test if len > mission max len */
}

void TestBplibEm_Register(void)
{
    ADD_TEST(Test_BPLib_EM_Init_Nominal);
}