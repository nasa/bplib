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
#include "bplib_mem_test_utils.h"


void Test_BPLib_MEM_CopyOutFromOffset_NullInputErrors(void)
{
    BPLib_MEM_Block_t FirstBlock;
    BPLib_Bundle_t Bundle;
    char OutputBuffer[2048];
    memset(&Bundle, 0, sizeof(Bundle));
    
    /* all three null */
    UtAssert_INT32_EQ(BPLib_MEM_CopyOutFromOffset(NULL, 0, 0, NULL, 0), BPLIB_NULL_PTR_ERROR);

    /* output buffer null */
    Bundle.blob = &FirstBlock;
    UtAssert_INT32_EQ(BPLib_MEM_CopyOutFromOffset(&Bundle, 0, 0, NULL, 0), BPLIB_NULL_PTR_ERROR);
    
    /* blob null */
    Bundle.blob = NULL;
    UtAssert_INT32_EQ(BPLib_MEM_CopyOutFromOffset(&Bundle, 0, 0, OutputBuffer, 0), BPLIB_NULL_PTR_ERROR);
}


void TestBplibMem_Register(void)
{
    UtTest_Add(Test_BPLib_MEM_CopyOutFromOffset_NullInputErrors, BPLib_MEM_Test_Setup, BPLib_MEM_Test_Teardown, "Test_BPLib_MEM_CopyOutFromOffset_NullInputErrors");
}
