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
 * Auto-Generated stub implementations for functions defined in bplib_as_internal header
 */

#include "bplib_as_internal.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_EidIsValid()
 * ----------------------------------------------------
 */
bool BPLib_AS_EidIsValid(int16_t SourceEid)
{
    UT_GenStub_SetupReturnBuffer(BPLib_AS_EidIsValid, bool);

    UT_GenStub_AddParam(BPLib_AS_EidIsValid, int16_t, SourceEid);

    UT_GenStub_Execute(BPLib_AS_EidIsValid, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_AS_EidIsValid, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_InitMutex()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_AS_InitMutex(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_AS_InitMutex, BPLib_Status_t);

    UT_GenStub_Execute(BPLib_AS_InitMutex, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_AS_InitMutex, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_LockCounters()
 * ----------------------------------------------------
 */
void BPLib_AS_LockCounters(void)
{

    UT_GenStub_Execute(BPLib_AS_LockCounters, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_SetCounter()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_AS_SetCounter(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Value)
{
    UT_GenStub_SetupReturnBuffer(BPLib_AS_SetCounter, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_AS_SetCounter, int16_t, SourceEid);
    UT_GenStub_AddParam(BPLib_AS_SetCounter, BPLib_AS_Counter_t, Counter);
    UT_GenStub_AddParam(BPLib_AS_SetCounter, uint32_t, Value);

    UT_GenStub_Execute(BPLib_AS_SetCounter, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_AS_SetCounter, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_UnlockCounters()
 * ----------------------------------------------------
 */
void BPLib_AS_UnlockCounters(void)
{

    UT_GenStub_Execute(BPLib_AS_UnlockCounters, Basic, NULL);
}
