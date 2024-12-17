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

#ifndef BPLIB_NC_TEST_UTILS_H
#define BPLIB_NC_TEST_UTILS_H

/* ======== */
/* Includes */
/* ======== */

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_nc.h"
#include "bpa_fwp_stubs.h"       /* For ADUP stubs */
#include "bplib_em_handlers.h"   /* For BPLib_EM_SendEvent handler */
#include "bplib_as_handlers.h" /* For BPLib_AS_Increment and BPLib_AS_Decrement handler */

/* ====== */
/* Macros */
/* ====== */

// Add a test case
#define ADD_TEST(test) UtTest_Add(test, BPLib_NC_Test_Setup, BPLib_NC_Test_Teardown, #test)

/* =================== */
/* Function Prototypes */
/* =================== */

/* Event verification */
void BPLib_NC_Test_Verify_Event(uint16_t EventNum, int32_t EventID, const char* EventText);

/* AS increment argument verification */
void Test_BPLib_NC_VerifyIncrement(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Amount, int16_t CallNum);

/* AS decrement argument verification */
void Test_BPLib_NC_VerifyDecrement(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Amount, int16_t CallNum);

void BPLib_NC_Test_Setup(void);
void BPLib_NC_Test_Teardown(void);

void TestBplibNc_Register(void);

#endif /* BPLIB_NC_TEST_UTILS_H */
