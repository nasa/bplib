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

#include "bplib_pl.h"


/*
** Function Definitions
*/

BPLib_Status_t BPLib_TLM_Init(void)
{
    return BPLIB_SUCCESS; 
}

BPLib_Status_t BPLib_TLM_SendNodeMibConfigPkt(BPLib_NodeMibConfigHkTlm_Payload_t* NodeMIBConfigTlmPayload)
{
    if (BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibConfigPkt)
    {
        BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibConfigPkt(NodeMIBConfigTlmPayload);
    }
    else 
    {
        return BPLIB_PL_NULL_CALLBACK_ERROR;                
    }
    return BPLIB_SUCCESS;    
}

BPLib_Status_t BPLib_TLM_SendPerSourceMibConfigPkt(BPLib_SourceMibConfigHkTlm_Payload_t* SrcMIBConfigTlmPayload)
{
    if (BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibConfigPkt)
    {
        BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibConfigPkt(SrcMIBConfigTlmPayload);
    }
    else 
    {
        return BPLIB_PL_NULL_CALLBACK_ERROR;                
    }
    return BPLIB_SUCCESS;        
}

BPLib_Status_t BPLib_TLM_SendNodeMibCounterPkt(BPLib_NodeMibCountersHkTlm_Payload_t* NodeMIBCounterTlmPayload)
{
    if (BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibCounterPkt)
    {
        BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibCounterPkt(NodeMIBCounterTlmPayload);
    }
    else 
    {
        return BPLIB_PL_NULL_CALLBACK_ERROR;                
    }
    return BPLIB_SUCCESS;          
}

BPLib_Status_t BPLib_TLM_SendPerSourceMibCounterPkt(BPLib_SourceMibCountersHkTlm_Payload_t* SrcMIBCounterTlmPayload)
{
    if (BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibCounterPkt)
    {
        BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibCounterPkt(SrcMIBCounterTlmPayload);
    }
    else 
    {
        return BPLIB_PL_NULL_CALLBACK_ERROR;                
    }
    return BPLIB_SUCCESS;              
}

BPLib_Status_t BPLib_TLM_SendChannelContactPkt(BPLib_ChannelContactStatHkTlm_Payload_t* ChannelContactTlmPayload)
{
    if (BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendChannelContactPkt)
    {
        BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendChannelContactPkt(ChannelContactTlmPayload);
    }
    else 
    {
        return BPLIB_PL_NULL_CALLBACK_ERROR;                
    }
    return BPLIB_SUCCESS;                  
}

BPLib_Status_t BPLib_TLM_SendStoragePkt(BPLib_StorageHkTlm_Payload_t* StorTlmPayload)
{
    if (BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendStoragePkt)
    {
        BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendStoragePkt(StorTlmPayload);
    }
    else 
    {
        return BPLIB_PL_NULL_CALLBACK_ERROR;                
    }
    return BPLIB_SUCCESS;     
}
