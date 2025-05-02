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
    uint32_t ChanId;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);

    for (ChanId = 0; ChanId < BPLIB_MAX_NUM_CHANNELS; ChanId++)
    {
        ChanTbl.Configs[ChanId].HopLimit = 10;
        ChanTbl.Configs[ChanId].CrcType = BPLib_CRC_Type_CRC16;
        ChanTbl.Configs[ChanId].DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
        ChanTbl.Configs[ChanId].PayloadBlkConfig.IncludeBlock = true;
        ChanTbl.Configs[ChanId].PayloadBlkConfig.BlockNum = 1;

    }

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_SUCCESS);
}

void Test_BPLib_PI_ValidateConfigs_RegStateInv(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].RegState = 10;

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_PI_ValidateConfigs_HopLimitInv(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].HopLimit = 0;
    ChanTbl.Configs[0].RegState = BPLIB_PI_ACTIVE;

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_PI_ValidateConfigs_CrcTypeInv(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].HopLimit = 10;
    ChanTbl.Configs[0].RegState = BPLIB_PI_PASSIVE_DEFER;
    ChanTbl.Configs[0].CrcType = BPLib_CRC_Type_None;

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_PI_ValidateConfigs_FlagsInv(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].HopLimit = 10;
    ChanTbl.Configs[0].RegState = BPLIB_PI_PASSIVE_ABANDON;
    ChanTbl.Configs[0].CrcType = BPLib_CRC_Type_CRC32C;
    ChanTbl.Configs[0].BundleProcFlags = 0xFF;

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_PI_ValidateConfigs_DtnDestEid(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].HopLimit = 10;
    ChanTbl.Configs[0].RegState = BPLIB_PI_PASSIVE_ABANDON;
    ChanTbl.Configs[0].CrcType = BPLib_CRC_Type_CRC32C;
    ChanTbl.Configs[0].DestEID.Scheme = BPLIB_EID_SCHEME_DTN;

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_PI_ValidateConfigs_DestEidInv(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].HopLimit = 10;
    ChanTbl.Configs[0].RegState = BPLIB_PI_PASSIVE_ABANDON;
    ChanTbl.Configs[0].CrcType = BPLib_CRC_Type_CRC32C;
    ChanTbl.Configs[0].DestEID.Scheme = BPLIB_EID_SCHEME_IPN;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), false);

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_PI_ValidateConfigs_RepToEidInv(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].HopLimit = 10;
    ChanTbl.Configs[0].RegState = BPLIB_PI_PASSIVE_ABANDON;
    ChanTbl.Configs[0].CrcType = BPLib_CRC_Type_CRC32C;
    ChanTbl.Configs[0].DestEID.Scheme = BPLIB_EID_SCHEME_IPN;

    UT_SetDeferredRetcode(UT_KEY(BPLib_EID_IsValid), 1, true);
    UT_SetDeferredRetcode(UT_KEY(BPLib_EID_IsValid), 1, false);

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_PI_ValidateConfigs_SizeInv(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].HopLimit = 10;
    ChanTbl.Configs[0].RegState = BPLIB_PI_PASSIVE_ABANDON;
    ChanTbl.Configs[0].CrcType = BPLib_CRC_Type_CRC32C;
    ChanTbl.Configs[0].DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
    ChanTbl.Configs[0].MaxBundlePayloadSize = BPLIB_MAX_PAYLOAD_SIZE + 1;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_PI_ValidateConfigs_LifetimeInv(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].HopLimit = 10;
    ChanTbl.Configs[0].RegState = BPLIB_PI_PASSIVE_ABANDON;
    ChanTbl.Configs[0].CrcType = BPLib_CRC_Type_CRC32C;
    ChanTbl.Configs[0].DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
    ChanTbl.Configs[0].Lifetime = BPLIB_MAX_LIFETIME_ALLOWED + 1;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_PI_ValidateConfigs_NoPayload(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].HopLimit = 10;
    ChanTbl.Configs[0].RegState = BPLIB_PI_PASSIVE_ABANDON;
    ChanTbl.Configs[0].CrcType = BPLib_CRC_Type_CRC32C;
    ChanTbl.Configs[0].DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
    ChanTbl.Configs[0].PayloadBlkConfig.IncludeBlock = false;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_PI_ValidateConfigs_BadPayloadBlockNum(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].HopLimit = 10;
    ChanTbl.Configs[0].RegState = BPLIB_PI_PASSIVE_ABANDON;
    ChanTbl.Configs[0].CrcType = BPLib_CRC_Type_CRC32C;
    ChanTbl.Configs[0].DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
    ChanTbl.Configs[0].PayloadBlkConfig.BlockNum = 100;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_PI_ValidateConfigs_BadPrevNodeBlk(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].HopLimit = 10;
    ChanTbl.Configs[0].RegState = BPLIB_PI_PASSIVE_ABANDON;
    ChanTbl.Configs[0].CrcType = BPLib_CRC_Type_CRC32C;
    ChanTbl.Configs[0].DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
    ChanTbl.Configs[0].PayloadBlkConfig.BlockNum = 1;

    ChanTbl.Configs[0].PrevNodeBlkConfig.CrcType = 5;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_PI_ValidateConfigs_BadAgeBlk(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].HopLimit = 10;
    ChanTbl.Configs[0].RegState = BPLIB_PI_PASSIVE_ABANDON;
    ChanTbl.Configs[0].CrcType = BPLib_CRC_Type_CRC32C;
    ChanTbl.Configs[0].DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
    ChanTbl.Configs[0].PayloadBlkConfig.BlockNum = 1;
    ChanTbl.Configs[0].PrevNodeBlkConfig.BlockNum = 2;
    ChanTbl.Configs[0].PrevNodeBlkConfig.CrcType = BPLib_CRC_Type_CRC16;
    ChanTbl.Configs[0].AgeBlkConfig.CrcType = BPLib_CRC_Type_CRC32C;

    ChanTbl.Configs[0].AgeBlkConfig.BlockProcFlags = 0xffffff;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_PI_ValidateConfigs_BadHopBlk(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].HopLimit = 10;
    ChanTbl.Configs[0].RegState = BPLIB_PI_PASSIVE_ABANDON;
    ChanTbl.Configs[0].CrcType = BPLib_CRC_Type_CRC32C;
    ChanTbl.Configs[0].DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
    ChanTbl.Configs[0].PayloadBlkConfig.BlockNum = 1;
    ChanTbl.Configs[0].PrevNodeBlkConfig.BlockNum = 2;
    ChanTbl.Configs[0].AgeBlkConfig.BlockNum = 3;
    ChanTbl.Configs[0].PrevNodeBlkConfig.CrcType = BPLib_CRC_Type_CRC16;
    ChanTbl.Configs[0].AgeBlkConfig.CrcType = BPLib_CRC_Type_CRC32C;

    ChanTbl.Configs[0].HopCountBlkConfig.BlockNum = 3;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_INVALID_CONFIG_ERR);
}

void Test_BPLib_PI_ValidateConfigs_BadPayloadBlk(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].HopLimit = 10;
    ChanTbl.Configs[0].RegState = BPLIB_PI_PASSIVE_ABANDON;
    ChanTbl.Configs[0].CrcType = BPLib_CRC_Type_CRC32C;
    ChanTbl.Configs[0].DestEID.Scheme = BPLIB_EID_SCHEME_IPN;
    ChanTbl.Configs[0].PayloadBlkConfig.BlockNum = 1;
    ChanTbl.Configs[0].PrevNodeBlkConfig.BlockNum = 2;
    ChanTbl.Configs[0].AgeBlkConfig.BlockNum = 3;
    ChanTbl.Configs[0].PrevNodeBlkConfig.CrcType = BPLib_CRC_Type_CRC16;
    ChanTbl.Configs[0].AgeBlkConfig.CrcType = BPLib_CRC_Type_CRC32C;
    ChanTbl.Configs[0].HopCountBlkConfig.BlockNum = 4;

    ChanTbl.Configs[0].PayloadBlkConfig.BlockProcFlags = 0xffffff;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_EID_IsValid), true);

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_INVALID_CONFIG_ERR);
}

/* Test nominal ingress function */
void Test_BPLib_PI_Ingress_Nominal(void)
{
    uint8_t ChanId = 0;
    uint8_t AduPtr[10];
    size_t AduSize = 0;
    BPLib_Bundle_t Bundle;

    memset(&Bundle, 0, sizeof(Bundle));

    UT_SetDeferredRetcode(UT_KEY(BPLib_MEM_BundleAlloc), 1, (UT_IntReturn_t) &Bundle);

    UtAssert_INT32_EQ(BPLib_PI_Ingress(&BplibInst, ChanId, AduPtr, AduSize), BPLIB_SUCCESS);

    /* Ensure Node Config is locked and unlocked */
    UtAssert_STUB_COUNT(BPLib_NC_ReaderLock, 1);
    UtAssert_STUB_COUNT(BPLib_NC_ReaderUnlock, 1);
}

/* Test ingress function null checks */
void Test_BPLib_PI_Ingress_Null(void)
{
    uint8_t ChanId = 0;
    uint8_t AduPtr[10];
    size_t AduSize = 0;

    /* Null BPLib instance */
    UtAssert_INT32_EQ(BPLib_PI_Ingress(NULL, ChanId, AduPtr, AduSize), BPLIB_NULL_PTR_ERROR);
    
    /* Null ADU pointer */
    UtAssert_INT32_EQ(BPLib_PI_Ingress(&BplibInst, ChanId, NULL, AduSize), BPLIB_NULL_PTR_ERROR);

    /* Null channel configuration */
    BPLib_NC_ConfigPtrs.ChanConfigPtr = NULL;
    UtAssert_INT32_EQ(BPLib_PI_Ingress(&BplibInst, ChanId, AduPtr, AduSize), BPLIB_NULL_PTR_ERROR);

    /* The block allocation function returns null by default */
    UtAssert_INT32_EQ(BPLib_PI_Ingress(&BplibInst, ChanId, AduPtr, AduSize), BPLIB_NULL_PTR_ERROR);
}

/* Test ingress function channel ID check */
void Test_BPLib_PI_Ingress_BadChanId(void)
{
    uint8_t ChanId = BPLIB_MAX_NUM_CHANNELS;
    uint8_t AduPtr[10];
    size_t AduSize = 0;

    UtAssert_INT32_EQ(BPLib_PI_Ingress(&BplibInst, ChanId, AduPtr, AduSize), BPLIB_INVALID_CHAN_ID_ERR);
}

/* Test ingress function null memory allocation */
void Test_BPLib_PI_Ingress_NullMem(void)
{
    uint8_t ChanId = 0;
    uint8_t AduPtr[10];
    size_t AduSize = 0;

    UT_SetDeferredRetcode(UT_KEY(BPLib_MEM_BundleAlloc), 1, (UT_IntReturn_t) NULL);

    UtAssert_INT32_EQ(BPLib_PI_Ingress(&BplibInst, ChanId, AduPtr, AduSize), BPLIB_NULL_PTR_ERROR);
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 2);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_PI_INGRESS_ERR_EID);
}


/* Test nominal egress function */
void Test_BPLib_PI_Egress_Nominal(void)
{
    uint8_t ChanId = 0;
    uint8_t AduPtr[10];
    size_t BufLen = 10;
    size_t AduSize;
    uint32_t Timeout = 1000;
    BPLib_Bundle_t Bundle;
    BPLib_Bundle_t *BundlePtr = &Bundle;

    Bundle.blocks.PayloadHeader.DataSize = 10;
    
    UT_SetDefaultReturnValue(UT_KEY(BPLib_QM_WaitQueueTryPull), true);
    UT_SetDataBuffer(UT_KEY(BPLib_QM_WaitQueueTryPull), &BundlePtr, sizeof(BPLib_Bundle_t *), false);

    UtAssert_INT32_EQ(BPLib_PI_Egress(&BplibInst, ChanId, AduPtr, &AduSize, BufLen, Timeout), BPLIB_SUCCESS);
    UtAssert_INT32_EQ(AduSize, Bundle.blocks.PayloadHeader.DataSize);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_PI_EGRESS_DBG_EID);
}

/* Test egress function when copy fails */
void Test_BPLib_PI_Egress_BadCopy(void)
{
    uint8_t ChanId = 0;
    uint8_t AduPtr[10];
    size_t BufLen = 10;
    size_t AduSize;
    uint32_t Timeout = 1000;
    BPLib_Bundle_t Bundle;
    BPLib_Bundle_t *BundlePtr = &Bundle;

    Bundle.blocks.PayloadHeader.DataSize = 10;
    
    UT_SetDefaultReturnValue(UT_KEY(BPLib_QM_WaitQueueTryPull), true);
    UT_SetDataBuffer(UT_KEY(BPLib_QM_WaitQueueTryPull), &BundlePtr, sizeof(BPLib_Bundle_t *), false);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_CopyOutFromOffset), BPLIB_ERROR);

    UtAssert_INT32_EQ(BPLib_PI_Egress(&BplibInst, ChanId, AduPtr, &AduSize, BufLen, Timeout), BPLIB_ERROR);
    UtAssert_INT32_EQ(AduSize, 0);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_PI_EGRESS_ERR_EID);
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

/* Test egress function invalid channel ID  */
void Test_BPLib_PI_Egress_BadChanId(void)
{
    uint8_t ChanId = BPLIB_MAX_NUM_CHANNELS;
    uint8_t AduPtr[10];
    size_t BufLen = 10;
    size_t AduSize = 10;
    uint32_t Timeout = 1000;

    UtAssert_INT32_EQ(BPLib_PI_Egress(&BplibInst, ChanId, AduPtr, &AduSize, BufLen, Timeout), BPLIB_INVALID_CHAN_ID_ERR);
    UtAssert_INT32_EQ(AduSize, 0);
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
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_RegStateInv);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_HopLimitInv);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_CrcTypeInv);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_FlagsInv);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_DtnDestEid);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_DestEidInv);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_RepToEidInv);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_SizeInv);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_LifetimeInv);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_NoPayload);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_BadPayloadBlockNum);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_BadPrevNodeBlk);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_BadAgeBlk);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_BadHopBlk);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_BadPayloadBlk);

    ADD_TEST(Test_BPLib_PI_Ingress_Nominal);
    ADD_TEST(Test_BPLib_PI_Ingress_Null);
    ADD_TEST(Test_BPLib_PI_Ingress_BadChanId);
    ADD_TEST(Test_BPLib_PI_Ingress_NullMem);

    ADD_TEST(Test_BPLib_PI_Egress_Nominal);
    ADD_TEST(Test_BPLib_PI_Egress_Null);
    ADD_TEST(Test_BPLib_PI_Egress_Timeout);
    ADD_TEST(Test_BPLib_PI_Egress_BadChanId);
    ADD_TEST(Test_BPLib_PI_Egress_BadCopy);        
}
