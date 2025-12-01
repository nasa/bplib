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

#ifndef BPLIB_STOR_TEST_UTILS_H
#define BPLIB_STOR_TEST_UTILS_H

/*
** Include
*/

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_api_types.h"
#include "bplib_as_handlers.h"
#include "bplib_stor.h"
#include "bplib_stor_sql.h"
#include "bplib_em_handlers.h"
#include "bplib_qm_handlers.h"
#include "bpa_fwp_stubs.h"

/*
** Global Data
*/

extern BPLib_Instance_t BplibInst;

/*
** Helper Utilities
*/
void BPLib_STOR_Test_CreateTestBundle(BPLib_Bundle_t* Bundle);
void BPLib_STOR_Test_FreeTestBundle(BPLib_Bundle_t* Bundle);

/*
** Function Definitions
*/

void BPLib_STOR_Test_Setup(void);
void BPLib_STOR_Test_Teardown(void);

void BPLib_STOR_Test_SetupOneBundleStored(void);
void BPLib_STOR_Test_TeardownOneBundleStored(void);

void TestBplib_STOR_Register(void);
void TestBplib_STOR_LoadBatch_Register(void);
void TestBplib_STOR_Load_Register(void);
void TestBplib_STOR_Store_Register(void);

#endif /* BPLIB_STOR_TEST_UTILS_H */
