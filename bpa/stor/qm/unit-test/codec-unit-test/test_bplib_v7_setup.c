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

/*
 * Includes
 */
#include "test_bplib_v7.h"

void UT_V7_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    UT_Stub_SetReturnValue(FuncKey, UserObj);
}

void UT_V7_GetTime_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint64_t retval = 0;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void UT_V7_sizet_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    size_t retval = 0;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void UT_V7_uint64_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32    StatusCode;
    uint64_t Result;

    UT_Stub_GetInt32StatusCode(Context, &StatusCode);

    Result = StatusCode;

    UT_Stub_SetReturnValue(FuncKey, Result);
}

void UT_V7_int8_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int8 retval = 0;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void UT_V7_uint8_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32   StatusCode;
    uint8_t Result;

    UT_Stub_GetInt32StatusCode(Context, &StatusCode);

    Result = StatusCode;

    UT_Stub_SetReturnValue(FuncKey, Result);
}

int Test_BPLib_STOR_CACHE_V7WriterFuncStub(void *arg, const void *data_ptr, size_t data_len)
{
    return UT_DEFAULT_IMPL(Test_BPLib_STOR_CACHE_V7WriterFuncStub);
}

void Test_BPLib_STOR_CACHE_V7EncodeFuncStub(v7_encode_state_t *enc, const void *arg) {}

void Test_BPLib_STOR_CACHE_V7DecodeFuncStub(v7_decode_state_t *dec, void *arg) {}

void UtTest_Setup(void)
{
    TestBpV7_Register();
    TestBpV7AdminRecord_Register();
    TestBpV7Basetypes_Register();
    TestBpV7Bitmap_Register();
    TestBpV7BundleAgeBlock_Register();
    TestBpV7CanonicalBlock_Rgister();
    TestV7BpContainer_Rgister();
    TestV7BpCrc_Rgister();
    TestV7BpEndpointid_Rgister();
    TestV7BpHopCountBlock_Rgister();
    TestV7BpPreviousNodeBlock_Rgister();
    TestV7BpPrimaryBlock_Rgister();
    TestV7CodecCommon_Rgister();
    TestV7CustodyAcknowledgementRecord_Rgister();
    TestV7CustodyTrackingRecord_Rgister();
    TestV7DecodecApi_Rgister();
    TestV7EncodeApi_Rgister();
}
