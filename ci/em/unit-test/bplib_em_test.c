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

/* ======== */
/* Includes */
/* ======== */

#include "bplib_em_test_utils.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

// Test that EM initializes nominally
void Test_BPLib_EM_Init_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_Init();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

// Test that EM generates an event under nominal conditions
void Test_BPA_EM_SendEvent_Nominal(void)
{
    BPLib_Status_t Status;
    uint16_t EventID;
    BPLib_EM_EventType_t EventType;

    /* INFO event with a digit */
    EventID = 0;
    EventType = BPLib_EM_EventType_INFORMATION;
    Status = BPLib_EM_SendEvent(EventID, EventType, "INFO event message with a digit: %d", 100);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventID, EventID);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventType, EventType);
    UtAssert_STRINGBUF_EQ("INFO event message with a digit: 100", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPA_EVP_SendEvent.Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);

    /* INFO event with a string */
    EventID = 1;
    EventType = BPLib_EM_EventType_INFORMATION;
    Status = BPLib_EM_SendEvent(EventID, EventType, "INFO event message with a string: %s", "useless");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventID, EventID);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventType, EventType);
    UtAssert_STRINGBUF_EQ("INFO event message with a string: useless", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPA_EVP_SendEvent.Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);

    /* INFO event with a digit and a string */
    EventID = 2;
    EventType = BPLib_EM_EventType_INFORMATION;
    Status = BPLib_EM_SendEvent(EventID, EventType, "INFO event message with a digit and a string: %.3f %s", 1.001, "fish");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventID, EventID);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventType, EventType);
    UtAssert_STRINGBUF_EQ("INFO event message with a digit and a string: 1.001 fish", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPA_EVP_SendEvent.Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);


    /* ERROR event with a digit */
    EventID = 3;
    EventType = BPLib_EM_EventType_ERROR;
    Status = BPLib_EM_SendEvent(EventID, EventType,  "ERROR event message with a digit: %d", 55);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventID, EventID);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventType, EventType);
    UtAssert_STRINGBUF_EQ("ERROR event message with a digit: 55", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPA_EVP_SendEvent.Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);

    /* ERROR event with a string */
    EventID = 4;
    EventType = BPLib_EM_EventType_ERROR;
    Status = BPLib_EM_SendEvent(EventID, EventType,  "ERROR event message with a string: %s", "the meaning");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventID, EventID);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventType, EventType);
    UtAssert_STRINGBUF_EQ("ERROR event message with a string: the meaning", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPA_EVP_SendEvent.Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);

    /* ERROR event with a digit and a string */
    EventID = 5;
    EventType = BPLib_EM_EventType_ERROR;
    Status = BPLib_EM_SendEvent(EventID, EventType,  "ERROR event message with a digit and a string: %.2f %s", 2.002, "fish");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventID, EventID);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventType, EventType);
    UtAssert_STRINGBUF_EQ("ERROR event message with a digit and a string: 2.00 fish", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPA_EVP_SendEvent.Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);

    /* DEBUG event with a digit */
    EventID = 6;
    EventType = BPLib_EM_EventType_DEBUG;
    Status = BPLib_EM_SendEvent(EventID, EventType,  "DEBUG event message with a digit: %d", 30);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventID, EventID);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventType, EventType);
    UtAssert_STRINGBUF_EQ("DEBUG event message with a digit: 30", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPA_EVP_SendEvent.Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);

    /* DEBUG event with a string */
    EventID = 7;
    EventType = BPLib_EM_EventType_DEBUG;
    Status = BPLib_EM_SendEvent(EventID, EventType,  "DEBUG event message with a string: %s", "of life");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventID, EventID);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventType, EventType);
    UtAssert_STRINGBUF_EQ("DEBUG event message with a string: of life", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPA_EVP_SendEvent.Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);

    /* DEBUG event with a digit and a string */
    EventID = 8;
    EventType = BPLib_EM_EventType_DEBUG;
    Status = BPLib_EM_SendEvent(EventID, EventType,  "DEBUG event message with a digit and a string: %.2f %s", 1230.00002458, "fish");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventID, EventID);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventType, EventType);
    UtAssert_STRINGBUF_EQ("DEBUG event message with a digit and a string: 1230.00 fish", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPA_EVP_SendEvent.Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);

    /* CRITICAL event with a digit */
    EventID = 9;
    EventType = BPLib_EM_EventType_CRITICAL;
    Status = BPLib_EM_SendEvent(EventID, EventType,  "CRITICAL event message with a digit: %d", 11);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventID, EventID);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventType, EventType);
    UtAssert_STRINGBUF_EQ("CRITICAL event message with a digit: 11", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPA_EVP_SendEvent.Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);

    /* CRITICAL event with a string */
    EventID = 10;
    EventType = BPLib_EM_EventType_CRITICAL;
    Status = BPLib_EM_SendEvent(EventID, EventType,  "CRITICAL event message with a string: %s", "is 42");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventID, EventID);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventType, EventType);
    UtAssert_STRINGBUF_EQ("CRITICAL event message with a string: is 42", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPA_EVP_SendEvent.Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);


    /* CRITICAL event with a digit and a string */
    EventID = 11;
    EventType = BPLib_EM_EventType_CRITICAL;
    Status = BPLib_EM_SendEvent(EventID, EventType,  "CRITICAL event message with a digit and a string: %d %s", 8100, "fish");

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventID, EventID);
    UtAssert_EQ(uint16_t, context_BPA_EVP_SendEvent.EventType, EventType);
    UtAssert_STRINGBUF_EQ("CRITICAL event message with a digit and a string: 8100 fish", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPA_EVP_SendEvent.Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
}

void Test_BPA_EM_SendEvent_ExpandedTextError(void)
{
    BPLib_Status_t Status;

    /* Pass in Spec value that will cause vsprintf 
        to return a negative (failure) value */
    Status = BPLIB_UNKNOWN;
    Status = BPLib_EM_SendEvent(42, BPLib_EM_EventType_INFORMATION, NULL, NULL);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_EM_EXPANDED_TEXT_ERROR);
}

void TestBplibEm_Register(void)
{
    ADD_TEST(Test_BPLib_EM_Init_Nominal);
    ADD_TEST(Test_BPA_EM_SendEvent_Nominal);
    ADD_TEST(Test_BPA_EM_SendEvent_ExpandedTextError);
}