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

#ifndef TEST_BPLIB_V7_H
#define TEST_BPLIB_V7_H

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"
#include "cbor.h"
#include "v7.h"
#include "v7_codec.h"
#include "v7_mpool_bblocks.h"
#include "v7_codec_internal.h"
#include "v7_decode_internal.h"
#include "v7_encode_internal.h"
#include "bplib_os.h"

void UT_V7_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_V7_GetTime_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_V7_sizet_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_V7_uint64_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_V7_int8_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_V7_uint8_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
int  test_bplib_v7_writer_func_stub(void *arg, const void *data_ptr, size_t data_len);
void test_bplib_v7_encode_func_stub(v7_encode_state_t *enc, const void *arg);
void test_bplib_v7_decode_func_stub(v7_decode_state_t *dec, void *arg);

void TestBpV7_Register(void);
void TestBpV7AdminRecord_Register(void);
void TestBpV7Basetypes_Register(void);
void TestBpV7Bitmap_Register(void);
void TestBpV7BundleAgeBlock_Register(void);
void TestBpV7CanonicalBlock_Rgister(void);
void TestV7BpContainer_Rgister(void);
void TestV7BpCrc_Rgister(void);
void TestV7BpEndpointid_Rgister(void);
void TestV7BpHopCountBlock_Rgister(void);
void TestV7BpPreviousNodeBlock_Rgister(void);
void TestV7BpPrimaryBlock_Rgister(void);
void TestV7CodecCommon_Rgister(void);
void TestV7CustodyAcknowledgementRecord_Rgister(void);
void TestV7CustodyTrackingRecord_Rgister(void);
void TestV7DecodecApi_Rgister(void);
void TestV7EncodeApi_Rgister(void);

#endif
