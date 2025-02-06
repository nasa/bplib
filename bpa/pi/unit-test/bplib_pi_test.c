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

#include "bplib_pi_test_utils.h"

/* Test nominal add application function */
void Test_BPLib_PI_AddApplication_Nominal(void)
{
    uint8_t ChanId = 0;

    UtAssert_INT32_EQ(BPLib_PI_AddApplication(ChanId), BPLIB_SUCCESS);
}

/* Test nominal remove application function */
void Test_BPLib_PI_RemoveApplication_Nominal(void)
{
    uint8_t ChanId = 0;

    UtAssert_INT32_EQ(BPLib_PI_RemoveApplication(ChanId), BPLIB_SUCCESS);
}

/* Test nominal validate configs function */
void Test_BPLib_PI_ValidateConfigs_Nominal(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_SUCCESS);
}

/* Test validate configs function when it fails */
void Test_BPLib_PI_ValidateConfigs_Failure(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].MaxBundlePayloadSize = -1;

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_PI_INVALID_CONFIG_ERROR);
}

/* Test nominal ingress function */
void Test_BPLib_PI_Ingress_Nominal(void)
{
    uint8_t ChanId = 0;
    uint8_t AduPtr[10];
    BPLib_MEM_Block_t Block;
    size_t AduSize = 0;

    memset(&Block, 0, sizeof(Block));

    UT_SetDeferredRetcode(UT_KEY(BPLib_MEM_BlockAlloc), 1, (UT_IntReturn_t) &Block);
    UT_SetDeferredRetcode(UT_KEY(BPLib_MEM_BlockListAlloc), 1, (UT_IntReturn_t) &Block);

    UtAssert_INT32_EQ(BPLib_PI_Ingress(&BplibInst, ChanId, AduPtr, AduSize), BPLIB_SUCCESS);
}

/* Test ingress function null checks */
void Test_BPLib_PI_Ingress_Null(void)
{
    uint8_t ChanId = 0;
    uint8_t AduPtr[10];
    BPLib_MEM_Block_t Block;
    size_t AduSize = 0;

    /* Null BPLib instance */
    UtAssert_INT32_EQ(BPLib_PI_Ingress(NULL, ChanId, AduPtr, AduSize), BPLIB_NULL_PTR_ERROR);
    
    /* Null ADU pointer */
    UtAssert_INT32_EQ(BPLib_PI_Ingress(&BplibInst, ChanId, NULL, AduSize), BPLIB_NULL_PTR_ERROR);

    /* The block allocation function returns null by default */
    UtAssert_INT32_EQ(BPLib_PI_Ingress(&BplibInst, ChanId, AduPtr, AduSize), BPLIB_NULL_PTR_ERROR);

    /* Successful block allocation but failed blob allocation */
    UT_SetDeferredRetcode(UT_KEY(BPLib_MEM_BlockAlloc), 1, (UT_IntReturn_t) &Block);
    UtAssert_INT32_EQ(BPLib_PI_Ingress(&BplibInst, ChanId, AduPtr, AduSize), BPLIB_NULL_PTR_ERROR);
}

/* Test nominal egress function */
void Test_BPLib_PI_Egress_Nominal(void)
{
    // uint8_t ChanId = 0;
    // uint8_t AduPtr[10];
    // size_t BufLen = 10;
    // size_t AduSize;
    // uint32_t Timeout = 1000;
    // BPLib_Bundle_t BundleBuf;
    // BPLib_MEM_Block_t Block;

    // memset(&BundleBuf, 0, sizeof(BPLib_Bundle_t));
    // memset(&Block, 0, sizeof(BPLib_MEM_Block_t));

    // Block.chunk_len = 5;
    // Block.next = NULL;
    // BundleBuf.blob = &Block;

    // UT_SetDataBuffer(UT_KEY(BPLib_QM_WaitQueueTryPull), &BundleBuf, sizeof(BundleBuf), false);
    // UT_SetDefaultReturnValue(UT_KEY(BPLib_QM_WaitQueueTryPull), true);

    // UtAssert_INT32_EQ(BPLib_PI_Egress(&BplibInst, ChanId, AduPtr, &AduSize, BufLen, Timeout), BPLIB_SUCCESS);
}

/* Test egress function null checks */
void Test_BPLib_PI_Egress_Null(void)
{
    uint8_t ChanId = 0;
    uint8_t AduPtr[10];
    size_t BufLen = 10;
    size_t AduSize;
    uint32_t Timeout = 1000;

    /* Null BPLib instance */
    UtAssert_INT32_EQ(BPLib_PI_Egress(NULL, ChanId, AduPtr, &AduSize, BufLen, Timeout), BPLIB_NULL_PTR_ERROR);

    /* Null ADU pointer */
    UtAssert_INT32_EQ(BPLib_PI_Egress(&BplibInst, ChanId, NULL, &AduSize, BufLen, Timeout), BPLIB_NULL_PTR_ERROR);

    /* Null ADU size */
    UtAssert_INT32_EQ(BPLib_PI_Egress(&BplibInst, ChanId, AduPtr, NULL, BufLen, Timeout), BPLIB_NULL_PTR_ERROR);
}

/* Test egress function timeout */
void Test_BPLib_PI_Egress_Timeout(void)
{
    uint8_t ChanId = 0;
    uint8_t AduPtr[10];
    size_t BufLen = 10;
    size_t AduSize;
    uint32_t Timeout = 1000;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_QM_WaitQueueTryPull), false);

    UtAssert_INT32_EQ(BPLib_PI_Egress(&BplibInst, ChanId, AduPtr, &AduSize, BufLen, Timeout), BPLIB_PI_TIMEOUT);
}


void TestBplibPi_Register(void)
{
    ADD_TEST(Test_BPLib_PI_AddApplication_Nominal);

    ADD_TEST(Test_BPLib_PI_RemoveApplication_Nominal);

    ADD_TEST(Test_BPLib_PI_ValidateConfigs_Nominal);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_Failure);

    ADD_TEST(Test_BPLib_PI_Ingress_Nominal);
    ADD_TEST(Test_BPLib_PI_Ingress_Null);

    ADD_TEST(Test_BPLib_PI_Egress_Nominal);
    ADD_TEST(Test_BPLib_PI_Egress_Null);
    ADD_TEST(Test_BPLib_PI_Egress_Timeout);
}
