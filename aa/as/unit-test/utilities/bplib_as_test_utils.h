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

#ifndef BPLIB_AS_TEST_UTILS_H
#define BPLIB_AS_TEST_UTILS_H

/*
** Include
*/

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_api_types.h"
#include "bplib_as.h"
#include "bplib_em_handlers.h"

#include "bplib_fwp.h"
#include "bpa_fwp_stubs.h"  /* For EVP callback stubs */

/* ================ */
/* Type Definitions */
/* ================ */

/* Unit test increment/decrement hook information */
typedef struct
{
    int16_t SourceEid;
    BPLib_AS_Counter_t Counter;
    uint32_t Amount;
} BPLib_AS_IncrementDecrementContext_t;

/* =========== */
/* Global Data */
/* =========== */

extern BPLib_AS_IncrementDecrementContext_t Context_BPLib_AS_IncrementDecrement;

/* ====== */
/* Macros */
/* ====== */

// Add a test case
#define ADD_TEST(test) UtTest_Add(test, BPLib_AS_Test_Setup, BPLib_AS_Test_Teardown, #test)

/* =================== */
/* Function Prototypes */
/* =================== */

/* Set and check node counter values */
void Test_BPLib_AS_SetNodeCounterValues(uint32 SetValue);
void Test_BPLib_AS_NodeCountersValueTest(uint32 ActualValue);

/* Set and check source counter values */
void Test_BPLib_AS_SetSourceCounterValues(int32_t SourceEid, uint32_t SetValue);
void Test_BPLib_AS_SourceCountersValueTest(int32_t SourceEid, uint32_t ActualValue);

/* Set bundle counter values */
void Test_BPLib_AS_SetBundleCounterValues(int32_t SourceEid, uint32_t SetValue);
void Test_BPLib_AS_BundleCountersValueTest(int32_t SourceEid, uint32_t ActualValue);

/* Set error counter values */
void Test_BPLib_AS_SetErrorCounterValues(int32_t SourceEid, uint32_t SetValue);
void Test_BPLib_AS_ErrorCountersValueTest(int32_t SourceEid, uint32_t ActualValue);

void BPLib_AS_Test_Verify_Event(uint16_t EventNum, int32_t EventID, const char* EventText);

void BPLib_AS_Test_Setup(void);
void BPLib_AS_Test_Teardown(void);

void TestBplibAs_Register(void);
void TestBplibAsInternal_Register(void);

#endif /* BPLIB_AS_TEST_UTILS_H */
