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
#include "bplib_api_types.h"
#include "test_bplib_base.h"
#include "bplib.h"

void UT_lib_ingress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    #ifdef STOR

    void *retval = NULL;
    int   count  = UT_GetStubCount(UT_KEY(BPLib_MEM_FlowTryPull));
    if (count > 15)
    {
        UT_Stub_SetReturnValue(FuncKey, retval);
    }
    else
    {
        UT_Stub_SetReturnValue(FuncKey, UserObj);
    }

    #else // STOR
    void *retval = NULL;
    UT_Stub_SetReturnValue(FuncKey, retval);
    #endif // STOR

}

void UT_lib_egress_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    #ifdef STOR

    void *retval = NULL;
    int   count  = UT_GetStubCount(UT_KEY(BPLib_MEM_FlowTryPull));
    if (count > 20)
    {
        UT_Stub_SetReturnValue(FuncKey, retval);
    }
    else
    {
        UT_Stub_SetReturnValue(FuncKey, UserObj);
    }

    #else // STOR
    void *retval = NULL;
    UT_Stub_SetReturnValue(FuncKey, retval);
    #endif // STOR

}

void UT_lib_baseintf_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    #ifdef STOR

    void *retval = NULL;
    int   count  = UT_GetStubCount(UT_KEY(BPLib_MEM_FlowTryPull));
    if (count > 25)
    {
        UT_Stub_SetReturnValue(FuncKey, retval);
    }
    else
    {
        UT_Stub_SetReturnValue(FuncKey, UserObj);
    }

    #else // STOR
    void *retval = NULL;
    UT_Stub_SetReturnValue(FuncKey, retval);
    #endif // STOR

}

void UT_lib_AltHandler_PointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    UT_Stub_SetReturnValue(FuncKey, UserObj);
}

void UT_lib_sizet_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    size_t retval = 0;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void UT_lib_uint64_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32    StatusCode;
    uint64_t Result;

    UT_Stub_GetInt32StatusCode(Context, &StatusCode);

    Result = StatusCode;

    UT_Stub_SetReturnValue(FuncKey, Result);
}

void UT_lib_int8_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int8 retval = 0;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void UT_lib_bool_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bool retval = true;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void UtTest_Setup(void)
{
    TestBplibBase_Register();
    #ifdef STOR
    TestBplibBase_ClaApi_Register();
    TestBplibBase_DataServiceApi_Register();
    TestBplibBase_Routing_Register();
    #endif // STOR
}
