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

#ifndef BPLIB_EID_TEST_UTILS_H
#define BPLIB_EID_TEST_UTILS_H

/* ======== */
/* Includes */
/* ======== */

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_eid.h"
#include "bplib_cfg.h"

/* =========== */
/* Global Data */
/* =========== */

extern BPLib_EID_t EID_Actual;
extern BPLib_EID_t EID_Reference;
extern BPLib_EID_Pattern_t EID_Pattern;

/* ====== */
/* Macros */
/* ====== */

// Add a test case
#define ADD_TEST(test) UtTest_Add(test, BPLib_EID_Test_Setup, BPLib_EID_Test_Teardown, #test)

/* ==================== */
/* Function Definitions */
/* ==================== */

void BPLib_EID_Test_Setup(void);
void BPLib_EID_Test_Teardown(void);
void TestBplibEid_Register(void);

#endif /* BPLIB_EID_TEST_UTILS_H */
