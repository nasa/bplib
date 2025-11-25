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

#include "bplib_cla_test_utils.h"
#include "bpa_fwp_stubs.h"
#include "bplib_fwp.h"
#include "bplib_nc.h"
#include "bplib_em_handlers.h" /* For BPLib_EM_SendEvent handler */

/* ==================== */
/* Function Definitions */
/* ==================== */

void BPLib_CLA_Test_Verify_Event(uint16_t EventNum, int32_t EventID, const char* EventText)
{
    /* Check the string */
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[EventNum].EventID, EventID);
    UtAssert_STRINGBUF_EQ(EventText, BPLIB_EM_EXPANDED_EVENT_SIZE,
                            context_BPLib_EM_SendEvent[EventNum].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
}

void BPLib_CLA_Test_Setup(void)
{
    BPLib_CLA_ContactsTable_t TestContactsTbl;

    /* Initialize test environment to default state for every test */
    UT_ResetState(0);

    /* Set up callbacks */
    BPLib_FWP_ProxyCallbacks.BPA_CLAP_ContactSetup    = BPA_CLAP_ContactSetup;
    BPLib_FWP_ProxyCallbacks.BPA_CLAP_ContactStart    = BPA_CLAP_ContactStart;
    BPLib_FWP_ProxyCallbacks.BPA_CLAP_ContactStop     = BPA_CLAP_ContactStop;
    BPLib_FWP_ProxyCallbacks.BPA_CLAP_ContactTeardown = BPA_CLAP_ContactTeardown;

    /* Prime the Contacts Configuration to return valid values */
    memset((void*) &TestContactsTbl,  0, sizeof(BPLib_CLA_ContactsTable_t));
    BPLib_NC_ConfigPtrs.ContactsConfigPtr = &TestContactsTbl;

    UT_SetHandlerFunction(UT_KEY(BPLib_QM_DuctPull), UT_Handler_BPLib_QM_DuctPull, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_EM_SendEvent), UT_Handler_BPLib_EM_SendEvent, NULL);
}

void BPLib_CLA_Test_Teardown(void)
{
    /* Clean up test environment */
}

void UtTest_Setup(void)
{
    TestBplibCla_Register();
    TestBplibClaInternal_Register();
}
