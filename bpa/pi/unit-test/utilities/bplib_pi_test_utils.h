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

#ifndef BPLIB_PI_TEST_UTILS_H
#define BPLIB_PI_TEST_UTILS_H

/*
** Include
*/

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_api_types.h"
#include "bplib_pi.h"
#include "bplib_fwp.h"
#include "bplib_nc.h"
#include "bplib_qm_handlers.h"
#include "bplib_em_handlers.h"
#include "bpa_fwp_stubs.h"     /* For ADUP stubs */

/* Macro to add test case */
#define ADD_TEST(test) UtTest_Add(test, BPLib_PI_Test_Setup, BPLib_PI_Test_Teardown, #test)


/*
** Global Data
*/

extern BPLib_Instance_t BplibInst;
extern uint64_t BPLib_PI_SequenceNums[BPLIB_MAX_NUM_CHANNELS];

/*
** Function Definitions
*/

void BPLib_PI_Test_Setup(void);
void BPLib_PI_Test_Teardown(void);

void TestBplibPi_Register(void);

#endif /* BPLIB_PI_TEST_UTILS_H */
