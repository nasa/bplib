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

/* Global Variables */
UT_CheckEvent_t EventTest;

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
    BPLib_Status_t Status;

    /* INFO event with a digit */
    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_SendEvent(42, BPLib_EM_EventType_INFO, "INFO event message with a digit: %d", 100);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* INFO event with a string */
    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_SendEvent(42, BPLib_EM_EventType_INFO, "INFO event message with a string: %s", "useless");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* INFO event with a digit and a string */
    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_SendEvent(42, BPLib_EM_EventType_INFO, "INFO event message with a digit and a string: %.2f %s", 1.001, "fish");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* ERROR event with a digit */
    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_SendEvent(42, BPLib_EM_EventType_ERROR, "ERROR event message with a digit: %d", 55);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* ERROR event with a string */
    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_SendEvent(42, BPLib_EM_EventType_ERROR, "ERROR event message with a string: %s", "the meaning");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* ERROR event with a digit and a string */
    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_SendEvent(42, BPLib_EM_EventType_ERROR, "ERROR event message with a digit and a string: %.2f %s", 2.002, "fish");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* DEBUG event with a digit */
    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_SendEvent(42, BPLib_EM_EventType_DEBUG, "DEBUG event message with a digit: %d", 30);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* DEBUG event with a string */
    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_SendEvent(42, BPLib_EM_EventType_DEBUG, "DEBUG event message with a string: %s", "of life");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* DEBUG event with a digit and a string */
    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_SendEvent(42, BPLib_EM_EventType_DEBUG, "DEBUG event message with a digit and a string: %d %s", 1230.00002458, "fish");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* CRITICAL event with a digit */
    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_SendEvent(42, BPLib_EM_EventType_CRITICAL, "CRITICAL event message with a digit: %d", 11);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* CRITICAL event with a string */
    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_SendEvent(42, BPLib_EM_EventType_CRITICAL, "CRITICAL event message with a string: %s", "is 42");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    /* CRITICAL event with a digit and a string */
    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_SendEvent(42, BPLib_EM_EventType_CRITICAL, "CRITICAL event message with a digit and a string: %d %s", 8100, "fish");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPA_EM_SendEvent_TruncatedString(void)
{
    char LongString[BPLIB_EM_MAX_MESSAGE_LENGTH + 5];
    BPLib_Status_t Status;
    int stringLoopControl;

    for(stringLoopControl = 0; stringLoopControl < BPLIB_EM_MAX_MESSAGE_LENGTH + 4; stringLoopControl++)
    {
        LongString[stringLoopControl] = 'A';
    }

    LongString[stringLoopControl] = 'H';

    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_SendEvent(142, BPLib_EM_EventType_INFO, LongString);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_EM_STRING_TRUNCATED);
}

void TestBplibEm_Register(void)
{
    ADD_TEST(Test_BPLib_EM_Init_Nominal);
    ADD_TEST(Test_BPA_EM_SendEvent_Nominal);
    ADD_TEST(Test_BPA_EM_SendEvent_TruncatedString);
}