/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

#ifndef TEST_BPLIB_BASE_H
#define TEST_BPLIB_BASE_H

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"
#include "bplib_rbtree.h"
#include "bplib_dataservice.h"
#include "bplib.h"
#include "bplib_os.h"
#include "bplib_api_types.h"
#include "../src/v7_base_internal.h"

void UT_lib_ingress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_egress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_baseintf_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_sizet_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_uint64_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_int8_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_lib_bool_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void TestBplibBase_Register(void);
void TestBplibBase_ClaApi_Register(void);
void TestBplibBase_DataServiceApi_Register(void);
void TestBplibBase_Routing_Register(void);

#endif
