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

#include "bplib_tlm_test_utils.h"
#include "bplib_tlm.h"

BPLib_FWP_ProxyCallbacks_t BPLib_FWP_ProxyCallbacks;

BPLib_Status_t BPA_TLMP_Init(void)
{
    return BPLIB_SUCCESS;    
}

BPLib_Status_t BPA_TLMP_SendNodeMibConfigPkt(BPLib_NodeMibConfigHkTlm_Payload_t* NodeMIBConfigTlmPayload)
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPA_TLMP_SendPerSourceMibConfigPkt(BPLib_SourceMibConfigHkTlm_Payload_t* SrcMIBConfigTlmPayload)
{
    return BPLIB_SUCCESS;    
}

BPLib_Status_t BPA_TLMP_SendNodeMibCounterPkt(BPLib_NodeMibCountersHkTlm_Payload_t* NodeMIBCounterTlmPayload)
{
    return BPLIB_SUCCESS;    
}

BPLib_Status_t BPA_TLMP_SendPerSourceMibCounterPkt(BPLib_SourceMibCountersHkTlm_Payload_t* SrcMIBCounterTlmPayload )
{
    return BPLIB_SUCCESS;    
}

BPLib_Status_t BPA_TLMP_SendChannelContactPkt(BPLib_ChannelContactStatHkTlm_Payload_t* ChannelContactTlmPayload)
{
    return BPLIB_SUCCESS;    
}

BPLib_Status_t BPA_TLMP_SendStoragePkt(BPLib_StorageHkTlm_Payload_t* StorTlmPayload)
{
    return BPLIB_SUCCESS;    
}



void Test_BPLib_TLM_Init_Nominal(void)
{
    UtAssert_INT32_EQ(BPLib_TLM_Init(), BPLIB_SUCCESS);
}

void Test_BPLib_TLM_SendNodeMibConfigPkt_Nominal(void)
{
    BPLib_NodeMibConfigHkTlm_Payload_t NodeMIBConfigTlmPayload;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibConfigPkt = BPA_TLMP_SendNodeMibConfigPkt;
    UtAssert_INT32_EQ(BPLib_TLM_SendNodeMibConfigPkt(&NodeMIBConfigTlmPayload), BPLIB_SUCCESS);        
}

void Test_BPLib_TLM_SendPerSourceMibConfigPkt_Nominal(void)
{
    BPLib_SourceMibConfigHkTlm_Payload_t SrcMIBConfigTlmPayload;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibConfigPkt = BPA_TLMP_SendPerSourceMibConfigPkt;
    UtAssert_INT32_EQ(BPLib_TLM_SendPerSourceMibConfigPkt(&SrcMIBConfigTlmPayload), BPLIB_SUCCESS);            
}

void Test_BPLib_TLM_SendNodeMibCounterPkt_Nominal(void)
{
    BPLib_NodeMibCountersHkTlm_Payload_t NodeMIBCounterTlmPayload;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibCounterPkt = BPA_TLMP_SendNodeMibCounterPkt;
    UtAssert_INT32_EQ(BPLib_TLM_SendNodeMibCounterPkt(&NodeMIBCounterTlmPayload), BPLIB_SUCCESS);                
}

void Test_BPLib_TLM_SendPerSourceMibCounterPkt_Nominal(void)
{
    BPLib_SourceMibCountersHkTlm_Payload_t SrcMIBCounterTlmPayload;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibCounterPkt = BPA_TLMP_SendPerSourceMibCounterPkt;
    UtAssert_INT32_EQ(BPLib_TLM_SendPerSourceMibCounterPkt(&SrcMIBCounterTlmPayload), BPLIB_SUCCESS);                    
}

void Test_BPLib_TLM_SendChannelContactPkt_Nominal(void)
{
    BPLib_ChannelContactStatHkTlm_Payload_t ChannelContactTlmPayload;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendChannelContactPkt = BPA_TLMP_SendChannelContactPkt;
    UtAssert_INT32_EQ(BPLib_TLM_SendChannelContactPkt(&ChannelContactTlmPayload), BPLIB_SUCCESS);                        
}

void Test_BPLib_TLM_SendStoragePkt_Nominal(void)
{
    BPLib_StorageHkTlm_Payload_t StorTlmPayload;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendStoragePkt = BPA_TLMP_SendStoragePkt;
    UtAssert_INT32_EQ(BPLib_TLM_SendStoragePkt(&StorTlmPayload), BPLIB_SUCCESS);                        
}

void Test_BPLib_TLM_SendNodeMibConfigPkt_NULL(void)
{
    BPLib_NodeMibConfigHkTlm_Payload_t NodeMIBConfigTlmPayload;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibConfigPkt = NULL;
    UtAssert_INT32_NEQ(BPLib_TLM_SendNodeMibConfigPkt(&NodeMIBConfigTlmPayload), BPLIB_SUCCESS);        
}

void Test_BPLib_TLM_SendPerSourceMibConfigPkt_NULL(void)
{
    BPLib_SourceMibConfigHkTlm_Payload_t SrcMIBConfigTlmPayload;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibConfigPkt = NULL;
    UtAssert_INT32_NEQ(BPLib_TLM_SendPerSourceMibConfigPkt(&SrcMIBConfigTlmPayload), BPLIB_SUCCESS);            
}

void Test_BPLib_TLM_SendNodeMibCounterPkt_NULL(void)
{
    BPLib_NodeMibCountersHkTlm_Payload_t NodeMIBCounterTlmPayload;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibCounterPkt = NULL;
    UtAssert_INT32_NEQ(BPLib_TLM_SendNodeMibCounterPkt(&NodeMIBCounterTlmPayload), BPLIB_SUCCESS);                
}

void Test_BPLib_TLM_SendPerSourceMibCounterPkt_NULL(void)
{
    BPLib_SourceMibCountersHkTlm_Payload_t SrcMIBCounterTlmPayload;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibCounterPkt = NULL;
    UtAssert_INT32_NEQ(BPLib_TLM_SendPerSourceMibCounterPkt(&SrcMIBCounterTlmPayload), BPLIB_SUCCESS);                    
}

void Test_BPLib_TLM_SendChannelContactPkt_NULL(void)
{
    BPLib_ChannelContactStatHkTlm_Payload_t ChannelContactTlmPayload;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendChannelContactPkt = NULL;
    UtAssert_INT32_NEQ(BPLib_TLM_SendChannelContactPkt(&ChannelContactTlmPayload), BPLIB_SUCCESS);                        
}

void Test_BPLib_TLM_SendStoragePkt_NULL(void)
{
    BPLib_StorageHkTlm_Payload_t StorTlmPayload;
    BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendStoragePkt = NULL;
    UtAssert_INT32_NEQ(BPLib_TLM_SendStoragePkt(&StorTlmPayload), BPLIB_SUCCESS);                        
}

void TestBplibTlm_Register(void)
{
    UtTest_Add(Test_BPLib_TLM_Init_Nominal, BPLib_TLM_Test_Setup, BPLib_TLM_Test_Teardown, "Test_BPLib_TLM_Init_Nominal");
    UtTest_Add(Test_BPLib_TLM_SendNodeMibConfigPkt_Nominal, BPLib_TLM_Test_Setup, BPLib_TLM_Test_Teardown, "Test_BPLib_TLM_SendNodeMibConfigPkt_Nominal");
    UtTest_Add(Test_BPLib_TLM_SendPerSourceMibConfigPkt_Nominal, BPLib_TLM_Test_Setup, BPLib_TLM_Test_Teardown, "Test_BPLib_TLM_SendPerSourceMibConfigPkt_Nominal");
    UtTest_Add(Test_BPLib_TLM_SendNodeMibCounterPkt_Nominal, BPLib_TLM_Test_Setup, BPLib_TLM_Test_Teardown, "Test_BPLib_TLM_SendNodeMibCounterPkt_Nominal");
    UtTest_Add(Test_BPLib_TLM_SendPerSourceMibCounterPkt_Nominal, BPLib_TLM_Test_Setup, BPLib_TLM_Test_Teardown, "Test_BPLib_TLM_SendPerSourceMibCounterPkt_Nominal");
    UtTest_Add(Test_BPLib_TLM_SendChannelContactPkt_Nominal, BPLib_TLM_Test_Setup, BPLib_TLM_Test_Teardown, "Test_BPLib_TLM_SendChannelContactPkt_Nominal");
    UtTest_Add(Test_BPLib_TLM_SendStoragePkt_Nominal, BPLib_TLM_Test_Setup, BPLib_TLM_Test_Teardown, "Test_BPLib_TLM_SendStoragePkt_Nominal");
    UtTest_Add(Test_BPLib_TLM_SendNodeMibConfigPkt_NULL, BPLib_TLM_Test_Setup, BPLib_TLM_Test_Teardown, "Test_BPLib_TLM_SendNodeMibConfigPkt_NULL");
    UtTest_Add(Test_BPLib_TLM_SendPerSourceMibConfigPkt_NULL, BPLib_TLM_Test_Setup, BPLib_TLM_Test_Teardown, "Test_BPLib_TLM_SendPerSourceMibConfigPkt_NULL");
    UtTest_Add(Test_BPLib_TLM_SendNodeMibCounterPkt_NULL, BPLib_TLM_Test_Setup, BPLib_TLM_Test_Teardown, "Test_BPLib_TLM_SendNodeMibCounterPkt_NULL");
    UtTest_Add(Test_BPLib_TLM_SendPerSourceMibCounterPkt_NULL, BPLib_TLM_Test_Setup, BPLib_TLM_Test_Teardown, "Test_BPLib_TLM_SendPerSourceMibCounterPkt_NULL");
    UtTest_Add(Test_BPLib_TLM_SendChannelContactPkt_NULL, BPLib_TLM_Test_Setup, BPLib_TLM_Test_Teardown, "Test_BPLib_TLM_SendChannelContactPkt_NULL");
    UtTest_Add(Test_BPLib_TLM_SendStoragePkt_NULL, BPLib_TLM_Test_Setup, BPLib_TLM_Test_Teardown, "Test_BPLib_TLM_SendStoragePkt_NULL");
}
