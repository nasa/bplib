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
** Include
*/

#include "bplib_bi_test_utils.h"
#include "bplib_time_handlers.h"

/*
** Global Data
*/

BPLib_Bundle_t DeserializedBundle;

/*
** Function Definitions
*/

void BPLib_BI_Test_Setup(void)
{
    /* Initialize test environment to default state for every test */
    UT_ResetState(0);

    memset(&DeserializedBundle, 0, sizeof(DeserializedBundle));

    DeserializedBundle.blocks.PrimaryBlock.Lifetime = 100000;
    DeserializedBundle.blocks.PayloadHeader.BlockType = BPLib_BlockType_Payload;
    DeserializedBundle.blocks.ExtBlocks[0].Header.BlockType = BPLib_BlockType_HopCount;
    DeserializedBundle.blocks.ExtBlocks[0].Header.BlockNum = 2;
    DeserializedBundle.blocks.ExtBlocks[1].Header.BlockType = BPLib_BlockType_Age;
    DeserializedBundle.blocks.ExtBlocks[1].Header.BlockNum = 3;
    DeserializedBundle.blocks.ExtBlocks[2].Header.BlockType = BPLib_BlockType_PrevNode;
    DeserializedBundle.blocks.ExtBlocks[2].Header.BlockNum = 4;
    DeserializedBundle.blocks.ExtBlocks[3].Header.BlockType = BPLib_BlockType_Reserved;

    UT_SetHandlerFunction(UT_KEY(BPLib_AS_Increment), UT_Handler_BPLib_AS_Increment, NULL);
}

void BPLib_BI_Test_Teardown(void)
{
    /* Clean up test environment */
}

void UtTest_Setup(void)
{
    TestBplibBi_Register();
}
