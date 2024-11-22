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
 * Auto-Generated stub implementations for functions defined in bplib_as header
 */

#include "bplib_as.h"
#include "utgenstub.h"

/* ======= */
/* Globals */
/* ======= */

BPLib_NodeMibCountersHkTlm_Payload_t    BPLib_AS_NodeCountersPayload;
BPLib_SourceMibCountersHkTlm_Payload_t  BPLib_AS_SourceCountersPayload;
BPLib_ChannelContactStatHkTlm_Payload_t BPLib_AS_ChannelContactStatsPayload;

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_Decrement()
 * ----------------------------------------------------
 */
void BPLib_AS_Decrement(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Amount)
{
    UT_GenStub_AddParam(BPLib_AS_Decrement, int16_t, SourceEid);
    UT_GenStub_AddParam(BPLib_AS_Decrement, BPLib_AS_Counter_t, Counter);
    UT_GenStub_AddParam(BPLib_AS_Decrement, uint32_t, Amount);

    UT_GenStub_Execute(BPLib_AS_Decrement, Basic, NULL);
}

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
 * Generated stub function for BPLib_AS_Get()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_AS_Get(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t *ReturnPtr)
{
    UT_GenStub_SetupReturnBuffer(BPLib_AS_Get, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_AS_Get, int16_t, SourceEid);
    UT_GenStub_AddParam(BPLib_AS_Get, BPLib_AS_Counter_t, Counter);
    UT_GenStub_AddParam(BPLib_AS_Get, uint32_t *, ReturnPtr);

    UT_GenStub_Execute(BPLib_AS_Get, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_AS_Get, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_Increment()
 * ----------------------------------------------------
 */
void BPLib_AS_Increment(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Amount)
{
    UT_GenStub_AddParam(BPLib_AS_Increment, int16_t, SourceEid);
    UT_GenStub_AddParam(BPLib_AS_Increment, BPLib_AS_Counter_t, Counter);
    UT_GenStub_AddParam(BPLib_AS_Increment, uint32_t, Amount);

    UT_GenStub_Execute(BPLib_AS_Increment, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_Init()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_AS_Init(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_AS_Init, BPLib_Status_t);

    UT_GenStub_Execute(BPLib_AS_Init, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_AS_Init, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_ResetAllCounters()
 * ----------------------------------------------------
 */
void BPLib_AS_ResetAllCounters(void)
{

    UT_GenStub_Execute(BPLib_AS_ResetAllCounters, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_ResetBundleCounters()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_AS_ResetBundleCounters(int16_t SourceEid)
{
    UT_GenStub_SetupReturnBuffer(BPLib_AS_ResetBundleCounters, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_AS_ResetBundleCounters, int16_t, SourceEid);

    UT_GenStub_Execute(BPLib_AS_ResetBundleCounters, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_AS_ResetBundleCounters, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_ResetErrorCounters()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_AS_ResetErrorCounters(int16_t SourceEid)
{
    UT_GenStub_SetupReturnBuffer(BPLib_AS_ResetErrorCounters, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_AS_ResetErrorCounters, int16_t, SourceEid);

    UT_GenStub_Execute(BPLib_AS_ResetErrorCounters, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_AS_ResetErrorCounters, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_ResetSourceCounters()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_AS_ResetSourceCounters(int16_t SourceEid)
{
    UT_GenStub_SetupReturnBuffer(BPLib_AS_ResetSourceCounters, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_AS_ResetSourceCounters, int16_t, SourceEid);

    UT_GenStub_Execute(BPLib_AS_ResetSourceCounters, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_AS_ResetSourceCounters, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_Set()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_AS_Set(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Value)
{
    UT_GenStub_SetupReturnBuffer(BPLib_AS_Set, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_AS_Set, int16_t, SourceEid);
    UT_GenStub_AddParam(BPLib_AS_Set, BPLib_AS_Counter_t, Counter);
    UT_GenStub_AddParam(BPLib_AS_Set, uint32_t, Value);

    UT_GenStub_Execute(BPLib_AS_Set, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_AS_Set, BPLib_Status_t);
}
