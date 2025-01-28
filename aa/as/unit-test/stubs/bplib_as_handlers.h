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

#ifndef BPLIB_AS_HANDLERS_H
#define BPLIB_AS_HANDLERS_H

/* ======== */
/* Includes */
/* ======== */

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_as.h"

/* ====== */
/* Macros */
/* ====== */

#define UT_MAX_INCDEC_DEPTH (50u)

/* ================ */
/* Type Definitions */
/* ================ */

/* Unit test increment/decrement hook information */
typedef struct
{
    int16_t            SourceEid;
    BPLib_AS_Counter_t Counter;
    uint32_t           Amount;
} BPLib_AS_IncrementDecrementContext_t;

/* =========== */
/* Global Data */
/* =========== */

extern BPLib_AS_IncrementDecrementContext_t Context_BPLib_AS_Increment[];
extern BPLib_AS_IncrementDecrementContext_t Context_BPLib_AS_Decrement[];

/* ==================== */
/* Function Definitions */
/* ==================== */

void UT_Handler_BPLib_AS_Increment(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);

void UT_Handler_BPLib_AS_Decrement(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);

#endif /* BPLIB_AS_HANDLERS_H */
