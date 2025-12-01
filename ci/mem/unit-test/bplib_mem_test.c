/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
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

void Test_BPLib_MEM_CopyOutFromOffset_BadSize(void)
{
    BPLib_MEM_Block_t FirstBlock;
    BPLib_Bundle_t Bundle;
    char OutputBuffer[2048];
    uint64_t NumBytesToCopy = 100;
    size_t OutputBufferSize = 10;
    memset(&Bundle, 0, sizeof(Bundle));

    Bundle.blob = &FirstBlock;
    UtAssert_INT32_EQ(BPLib_MEM_CopyOutFromOffset(&Bundle, 0, NumBytesToCopy, OutputBuffer, OutputBufferSize), BPLIB_BUF_LEN_ERROR);
}


void TestBplibMem_Register(void)
{
    UtTest_Add(Test_BPLib_MEM_CopyOutFromOffset_NullInputErrors, BPLib_MEM_Test_Setup, BPLib_MEM_Test_Teardown, "Test_BPLib_MEM_CopyOutFromOffset_NullInputErrors");
    UtTest_Add(Test_BPLib_MEM_CopyOutFromOffset_BadSize, BPLib_MEM_Test_Setup, BPLib_MEM_Test_Teardown, "Test_BPLib_MEM_CopyOutFromOffset_BadSize");
}
