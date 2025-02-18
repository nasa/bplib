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

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_AddMibArrayKey()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_AS_AddMibArrayKey(const BPLib_EID_Pattern_t *EID_Patterns)
{
    UT_GenStub_SetupReturnBuffer(BPLib_AS_AddMibArrayKey, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_AS_AddMibArrayKey, const BPLib_EID_Pattern_t *, EID_Patterns);

    UT_GenStub_Execute(BPLib_AS_AddMibArrayKey, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_AS_AddMibArrayKey, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_Decrement()
 * ----------------------------------------------------
 */
void BPLib_AS_Decrement(BPLib_EID_t EID, BPLib_AS_Counter_t Counter, uint32_t Amount)
{
    UT_GenStub_AddParam(BPLib_AS_Decrement, BPLib_EID_t, EID);
    UT_GenStub_AddParam(BPLib_AS_Decrement, BPLib_AS_Counter_t, Counter);
    UT_GenStub_AddParam(BPLib_AS_Decrement, uint32_t, Amount);

    UT_GenStub_Execute(BPLib_AS_Decrement, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_Increment()
 * ----------------------------------------------------
 */
void BPLib_AS_Increment(BPLib_EID_t EID, BPLib_AS_Counter_t Counter, uint32_t Amount)
{
    UT_GenStub_AddParam(BPLib_AS_Increment, BPLib_EID_t, EID);
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
void BPLib_AS_ResetBundleCounters(void)
{

    UT_GenStub_Execute(BPLib_AS_ResetBundleCounters, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_ResetCounter()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_AS_ResetCounter(uint16_t MibArrayIndex, BPLib_AS_Counter_t Counter)
{
    UT_GenStub_SetupReturnBuffer(BPLib_AS_ResetCounter, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_AS_ResetCounter, uint16_t, MibArrayIndex);
    UT_GenStub_AddParam(BPLib_AS_ResetCounter, BPLib_AS_Counter_t, Counter);

    UT_GenStub_Execute(BPLib_AS_ResetCounter, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_AS_ResetCounter, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_ResetErrorCounters()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_AS_ResetErrorCounters(uint8_t MibArrayIndex)
{
    UT_GenStub_SetupReturnBuffer(BPLib_AS_ResetErrorCounters, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_AS_ResetErrorCounters, uint8_t, MibArrayIndex);

    UT_GenStub_Execute(BPLib_AS_ResetErrorCounters, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_AS_ResetErrorCounters, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_ResetSourceCounters()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_AS_ResetSourceCounters(uint8_t MibArrayIndex)
{
    UT_GenStub_SetupReturnBuffer(BPLib_AS_ResetSourceCounters, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_AS_ResetSourceCounters, uint8_t, MibArrayIndex);

    UT_GenStub_Execute(BPLib_AS_ResetSourceCounters, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_AS_ResetSourceCounters, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_SendNodeMibCountersHk()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_AS_SendNodeMibCountersHk(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_AS_SendNodeMibCountersHk, BPLib_Status_t);

    UT_GenStub_Execute(BPLib_AS_SendNodeMibCountersHk, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_AS_SendNodeMibCountersHk, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_AS_SendSourceMibCountersHk()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_AS_SendSourceMibCountersHk(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_AS_SendSourceMibCountersHk, BPLib_Status_t);

    UT_GenStub_Execute(BPLib_AS_SendSourceMibCountersHk, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_AS_SendSourceMibCountersHk, BPLib_Status_t);
}
