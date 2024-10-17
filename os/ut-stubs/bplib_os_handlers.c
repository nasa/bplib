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

/**
 * @file
 *
 * Handler companions for generated BPLib OS stubs
 */

/* UT includes */
#include "uttest.h"
#include "utstubs.h"
#include "utgenstub.h"

/*----------------------------------------------------------------
 *
 * This just translates the 32-bit return status into a 64-bit
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_bplib_os_get_dtntime_ms(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32    StatusCode;
    uint64_t Result;

    UT_Stub_GetInt32StatusCode(Context, &StatusCode);

    Result = StatusCode;

    UT_Stub_SetReturnValue(FuncKey, Result);
}
