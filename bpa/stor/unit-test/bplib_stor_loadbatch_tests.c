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

/*
 * Include
 */
#include "bplib_stor_test_utils.h"

/* Test that Init handles NULL pointer */
void Test_BPLib_STOR_LoadBatch_InitNull(void)
{
    UtAssert_INT32_EQ(BPLib_STOR_LoadBatch_Init(NULL), BPLIB_NULL_PTR_ERROR);
}

/* Test the Init properly places the batch in initial condition */
void Test_BPLib_STOR_LoadBatch_InitNominal(void)
{
    BPLib_STOR_LoadBatch_t Batch;

    /* Init succeeds */
    UtAssert_INT32_EQ(BPLib_STOR_LoadBatch_Reset(&Batch), BPLIB_SUCCESS);

    /* Counting state is correct */
    UtAssert_INT32_EQ(Batch.Size, 0);
    UtAssert_INT32_EQ(Batch.ReadIndex, 0);
}

/* Test that Reset handles NULL pointer */
void Test_BPLib_STOR_LoadBatch_ResetNull(void)
{
    UtAssert_INT32_EQ(BPLib_STOR_LoadBatch_Reset(NULL), BPLIB_NULL_PTR_ERROR);
}

/* Test that Reset properly places the batch in empty condition */
void Test_BPLib_STOR_LoadBatch_ResetNominal(void)
{
    BPLib_STOR_LoadBatch_t Batch;
    Batch.Size = 5;
    Batch.ReadIndex = 5;

    /* Reset succeeds */
    UtAssert_INT32_EQ(BPLib_STOR_LoadBatch_Reset(&Batch), BPLIB_SUCCESS);

    /* Counting state is correct */
    UtAssert_INT32_EQ(Batch.Size, 0);
    UtAssert_INT32_EQ(Batch.ReadIndex, 0);
}

void Test_BPLib_STOR_LoadBatch_IsEmptyNull(void)
{
    /* If IsEmpty is given a null pointer, correct behavior is to return true */
    UtAssert_BOOL_TRUE(BPLib_STOR_LoadBatch_IsEmpty(NULL));
}

void Test_BPLib_STOR_LoadBatch_IsEmptyNominal(void)
{

}

void Test_BPLib_STOR_LoadBatch_IsConsumedNull(void)
{
    /* If IsConsumed is given a null pointer, correct behavior is to return true */
    UtAssert_BOOL_TRUE(BPLib_STOR_LoadBatch_IsEmpty(NULL));
}

void Test_BPLib_STOR_LoadBatch_IsConsumedNominal(void)
{

}

void Test_BPLib_STOR_LoadBatch_AddIDNull(void)
{
    UtAssert_INT32_EQ(BPLib_STOR_LoadBatch_AddID(NULL, 1), BPLIB_NULL_PTR_ERROR);
}

void Test_BPLib_STOR_LoadBatch_AddIDNominal(void)
{
    
}

void Test_BPLib_STOR_LoadBatch_PeekNextIDNull(void)
{
    BPLib_STOR_LoadBatch_t Batch;
    int64_t RetID;

    UtAssert_INT32_EQ(BPLib_STOR_LoadBatch_PeekNextID(NULL, &RetID), BPLIB_NULL_PTR_ERROR);
    UtAssert_INT32_EQ(BPLib_STOR_LoadBatch_PeekNextID(&Batch, NULL), BPLIB_NULL_PTR_ERROR);
}

void Test_BPLib_STOR_LoadBatch_PeekNextIDNominal(void)
{

}

void Test_BPLib_STOR_LoadBatch_AdvanceReaderNull(void)
{
    UtAssert_INT32_EQ(BPLib_STOR_LoadBatch_AdvanceReader(NULL), BPLIB_NULL_PTR_ERROR);
}

void Test_BPLib_STOR_LoadBatch_AdvanceReaderNominal(void)
{

}

void TestBplib_STOR_LoadBatch_Register(void)
{
    /* Init */
    UtTest_Add(Test_BPLib_STOR_LoadBatch_InitNull, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_LoadBatch_InitNull");
    UtTest_Add(Test_BPLib_STOR_LoadBatch_InitNominal, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_LoadBatch_InitNominal");

    /* Reset */
    UtTest_Add(Test_BPLib_STOR_LoadBatch_ResetNull, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_LoadBatch_ResetNull");
    UtTest_Add(Test_BPLib_STOR_LoadBatch_ResetNominal, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_LoadBatch_ResetNominal");

    /* IsEmpty */
    UtTest_Add(Test_BPLib_STOR_LoadBatch_IsEmptyNull, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_LoadBatch_IsEmptyNull");
    UtTest_Add(Test_BPLib_STOR_LoadBatch_IsEmptyNominal, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_LoadBatch_IsEmptyNominal");

    /* IsConsumed */
    UtTest_Add(Test_BPLib_STOR_LoadBatch_IsConsumedNull, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_LoadBatch_IsConsumedNull");
    UtTest_Add(Test_BPLib_STOR_LoadBatch_IsConsumedNominal, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_LoadBatch_IsConsumedNominal");

    /* AddID */
    UtTest_Add(Test_BPLib_STOR_LoadBatch_AddIDNull, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "BPLib_STOR_LoadBatch_AddIDNull");
    UtTest_Add(Test_BPLib_STOR_LoadBatch_AddIDNominal, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_LoadBatch_AddIDNominal");

    /* PeekNextID */
    UtTest_Add(Test_BPLib_STOR_LoadBatch_PeekNextIDNull, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_LoadBatch_PeekNextIDNull");
    UtTest_Add(Test_BPLib_STOR_LoadBatch_PeekNextIDNominal, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_LoadBatch_PeekNextIDNominal");

    /* AdvanceReader */
    UtTest_Add(Test_BPLib_STOR_LoadBatch_AdvanceReaderNull, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_LoadBatch_AdvanceReaderNull");
    UtTest_Add(Test_BPLib_STOR_LoadBatch_AdvanceReaderNominal, BPLib_STOR_Test_Setup, BPLib_STOR_Test_Teardown, "Test_BPLib_STOR_LoadBatch_AdvanceReaderNominal");
}
