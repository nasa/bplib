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

#include "bplib_stor_test_utils.h"
#include "bplib_pi.h"
#include "bplib_cla.h"
#include "bplib_fwp.h"

#include <string.h>
#include <stdlib.h>

/*
** Global Data
*/

BPLib_Instance_t BplibInst;
BPLib_PI_ChannelTable_t TestChanTbl;
BPLib_CLA_ContactsTable_t TestContTbl;
BPLib_Bundle_t TestBundle;

BPLib_MEM_Block_t* BBlocksMem;
BPLib_MEM_Block_t* BlobMem;


/*
** Test Setup Helpers
*/
void BPLib_STOR_Test_CreateTestBundle(BPLib_Bundle_t* Bundle)
{
    /* Test Bundle (Minimal Metadata needed to store) */
    Bundle->blocks.PrimaryBlock.Timestamp.CreateTime = 797186475264;
    Bundle->blocks.PrimaryBlock.Lifetime = 5000;
    Bundle->blocks.PrimaryBlock.DestEID.Node = 100;
    Bundle->blocks.PrimaryBlock.DestEID.Service = 1;

    /* MEM is stubbed so the tests can check the call's in UTAssert 
    ** Create a single-block data blob with the string "CBOR Blob"
    */
    Bundle->blob = (BPLib_MEM_Block_t*)calloc(1, sizeof(BPLib_MEM_Block_t));
    Bundle->blob->next = NULL;
    strcpy((char*)Bundle->blob->user_data.raw_bytes, "CBOR Blob");
    Bundle->blob->used_len = 10; // strlen + 1 for \0
}

void BPLib_STOR_Test_FreeTestBundle(BPLib_Bundle_t* Bundle)
{
    BPLib_MEM_Block_t* CurrBlock;
    BPLib_MEM_Block_t* NextBlock;

    if (Bundle == NULL)
    {
        return;
    }

    CurrBlock = Bundle->blob;
    while (CurrBlock != NULL)
    {
        NextBlock = CurrBlock->next;
        free(CurrBlock);
        CurrBlock = NextBlock;
    }
}

/*
** Function Definitions
*/
void BPLib_STOR_Test_Setup(void)
{
    /* Initialize test environment to default state for every test */
    UT_ResetState(0);

    BPLib_NC_ConfigPtrs.ChanConfigPtr = &TestChanTbl;
    BPLib_NC_ConfigPtrs.ContactsConfigPtr = &TestContTbl;
    BPLib_FWP_ProxyCallbacks.BPA_TIMEP_GetHostTime = BPA_TIMEP_GetHostTime;

    UT_SetHandlerFunction(UT_KEY(BPLib_EM_SendEvent), UT_Handler_BPLib_EM_SendEvent, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_QM_CreateJob), UT_Handler_BPLib_QM_CreateJob, NULL);

    /* Init Storage */
    BPLib_STOR_Init(&BplibInst);
}

void BPLib_STOR_Test_Teardown(void)
{
    /* Destroy Storage */
    BPLib_STOR_Destroy(&BplibInst);
}

void BPLib_STOR_Test_SetupOneBundleStored(void)
{
    BPLib_STOR_Test_Setup();
    BPLib_STOR_Test_CreateTestBundle(&TestBundle);
    BPLib_STOR_StoreBundle(&BplibInst, &TestBundle);
    BPLib_STOR_FlushPending(&BplibInst);

    /* We expect load bundle to MEM_Alloc twice for the test bundle: Setup BlockAlloc
    ** to return 2 chunks
    */
    BBlocksMem = calloc(1, sizeof(BPLib_MEM_Block_t));
    BlobMem = calloc(1, sizeof(BPLib_MEM_Block_t));
    UT_SetDeferredRetcode(UT_KEY(BPLib_MEM_BlockAlloc), 1, (uintptr_t)BBlocksMem);
    UT_SetDeferredRetcode(UT_KEY(BPLib_MEM_BlockAlloc), 1, (uintptr_t)BlobMem);
}

void BPLib_STOR_Test_TeardownOneBundleStored(void)
{
    BPLib_STOR_Test_FreeTestBundle(&TestBundle);
    BPLib_STOR_Test_Teardown();
    free(BBlocksMem);
    free(BlobMem);
    /* SQL is run with :memory:, so the bundle automatically is deleted after test exit */
}

void UtTest_Setup(void)
{
    //TestBplibStor_Register();
    TestBplib_STOR_LoadBatch_Register();
}
