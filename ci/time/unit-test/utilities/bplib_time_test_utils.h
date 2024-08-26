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

#ifndef BPLIB_TIME_TEST_UTILS_H
#define BPLIB_TIME_TEST_UTILS_H

/*
** Include
*/

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_api_types.h"
#include "bplib_time.h"
#include "bplib_time_internal.h"


/*
** Macro Definitions
*/

/* Macro to add test case */
#define ADD_TEST(test) UtTest_Add(test, BPLib_TIME_Test_Setup, BPLib_TIME_Test_Teardown, #test)

#define BPLIB_TIME_TEST_MONOTONIC_TIME_VALUE    ((uint64_t) 1234)
#define BPLIB_TIME_TEST_HOST_TIME_VALUE         ((uint64_t) 5678)


/*
** Function Definitions
*/

/*
** Stub function definitions for Time Proxy
*/

int64_t Test_BPA_TIMEP_GetMonotonicTime(void);
void Test_BPA_TIMEP_GetHostEpoch(BPLib_TIME_Epoch_t *Epoch);
BPLib_TIME_ClockState_t Test_BPA_TIMEP_GetHostClockState_Valid(void);
BPLib_TIME_ClockState_t Test_BPA_TIMEP_GetHostClockState_Invalid(void);
int64_t Test_BPA_TIMEP_GetHostTime(void);

void BPLib_TIME_Test_Setup(void);
void BPLib_TIME_Test_Teardown(void);

void TestBplibTime_Register(void);
void TestBplibTimeInternal_Register(void);

#endif /* BPLIB_TIME_TEST_UTILS_H */
