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

#ifndef BPLIB_EM_TEST_UTILS_H
#define BPLIB_EM_TEST_UTILS_H

/* ======== */
/* Includes */
/* ======== */
#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_api_types.h"
#include "bplib.h"
#include "bplib_em.h"

/* ================ */
/* Type Definitions */
/* ================ */
/* Unit test check event hook information */
typedef struct
{
    uint16      ExpectedEvent;
    uint32      MatchCount;
    const char *ExpectedFormat;
} UT_CheckEvent_t;

extern BPLib_FWP_ProxyCallbacks_t BPLib_FWP_ProxyCallbacks;

/* ====== */
/* Macros */
/* ====== */
#define ADD_TEST(test) UtTest_Add(test, BPLib_EM_Test_Setup, BPLib_EM_Test_Teardown, #test)

/* ==================== */
/* Function Definitions */
/* ==================== */
void BPLib_EM_Test_Setup(void);
void BPLib_EM_Test_Teardown(void);

void UT_CheckEvent_Setup_Impl(UT_CheckEvent_t *Evt, uint16 ExpectedEvent, const char *EventName,
                              const char *ExpectedFormat);

void TestBplibEm_Register(void);

#endif /* BPLIB_EM_TEST_UTILS_H */
