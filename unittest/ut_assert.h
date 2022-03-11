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

#ifndef UT_ASSERT_H
#define UT_ASSERT_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "stdio.h"
#include "stdarg.h"
#include "stdbool.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define ut_assert(e, ...) _ut_assert(e, __FILE__, __LINE__, __VA_ARGS__)
#define ut_check(e)       _ut_assert(e, __FILE__, __LINE__, "Failure")

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

void ut_reset(void);
bool _ut_assert(bool e, const char *file, int line, const char *fmt, ...);
int  ut_failures(void);

#endif /* UT_ASSERT_H */
