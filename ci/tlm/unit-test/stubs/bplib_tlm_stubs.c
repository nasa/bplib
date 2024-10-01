/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core
 * Flight System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator
 * of the National Aeronautics and Space Administration. All Rights Reserved.
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
 * Auto-Generated stub implementations for functions defined in bplib_tlm header
 */

#include "bplib_tlm.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TLM_Init()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TLM_Init(void) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_TLM_Init, BPLib_Status_t);

  UT_GenStub_Execute(BPLib_TLM_Init, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_TLM_Init, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TLM_SendChannelContactPkt()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TLM_SendChannelContactPkt(
    BPLib_ChannelContactStatHkTlm_Payload_t *ChannelContactTlmPayload) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_TLM_SendChannelContactPkt, BPLib_Status_t);

  UT_GenStub_AddParam(BPLib_TLM_SendChannelContactPkt,
                      BPLib_ChannelContactStatHkTlm_Payload_t *,
                      ChannelContactTlmPayload);

  UT_GenStub_Execute(BPLib_TLM_SendChannelContactPkt, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_TLM_SendChannelContactPkt,
                                   BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TLM_SendNodeMibConfigPkt()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TLM_SendNodeMibConfigPkt(
    BPLib_NodeMibConfigHkTlm_Payload_t *NodeMIBConfigTlmPayload) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_TLM_SendNodeMibConfigPkt, BPLib_Status_t);

  UT_GenStub_AddParam(BPLib_TLM_SendNodeMibConfigPkt,
                      BPLib_NodeMibConfigHkTlm_Payload_t *,
                      NodeMIBConfigTlmPayload);

  UT_GenStub_Execute(BPLib_TLM_SendNodeMibConfigPkt, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_TLM_SendNodeMibConfigPkt,
                                   BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TLM_SendNodeMibCounterPkt()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TLM_SendNodeMibCounterPkt(
    BPLib_NodeMibCountersHkTlm_Payload_t *NodeMIBCounterTlmPayload) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_TLM_SendNodeMibCounterPkt, BPLib_Status_t);

  UT_GenStub_AddParam(BPLib_TLM_SendNodeMibCounterPkt,
                      BPLib_NodeMibCountersHkTlm_Payload_t *,
                      NodeMIBCounterTlmPayload);

  UT_GenStub_Execute(BPLib_TLM_SendNodeMibCounterPkt, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_TLM_SendNodeMibCounterPkt,
                                   BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TLM_SendPerSourceMibConfigPkt()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TLM_SendPerSourceMibConfigPkt(
    BPLib_SourceMibConfigHkTlm_Payload_t *SrcMIBConfigTlmPayload) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_TLM_SendPerSourceMibConfigPkt,
                               BPLib_Status_t);

  UT_GenStub_AddParam(BPLib_TLM_SendPerSourceMibConfigPkt,
                      BPLib_SourceMibConfigHkTlm_Payload_t *,
                      SrcMIBConfigTlmPayload);

  UT_GenStub_Execute(BPLib_TLM_SendPerSourceMibConfigPkt, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_TLM_SendPerSourceMibConfigPkt,
                                   BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TLM_SendPerSourceMibCounterPkt()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_TLM_SendPerSourceMibCounterPkt(
    BPLib_SourceMibCountersHkTlm_Payload_t *SrcMIBCounterTlmPayload) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_TLM_SendPerSourceMibCounterPkt,
                               BPLib_Status_t);

  UT_GenStub_AddParam(BPLib_TLM_SendPerSourceMibCounterPkt,
                      BPLib_SourceMibCountersHkTlm_Payload_t *,
                      SrcMIBCounterTlmPayload);

  UT_GenStub_Execute(BPLib_TLM_SendPerSourceMibCounterPkt, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_TLM_SendPerSourceMibCounterPkt,
                                   BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_TLM_SendStoragePkt()
 * ----------------------------------------------------
 */
BPLib_Status_t
BPLib_TLM_SendStoragePkt(BPLib_StorageHkTlm_Payload_t *StorTlmPayload) 
{
  UT_GenStub_SetupReturnBuffer(BPLib_TLM_SendStoragePkt, BPLib_Status_t);

  UT_GenStub_AddParam(BPLib_TLM_SendStoragePkt, BPLib_StorageHkTlm_Payload_t *,
                      StorTlmPayload);

  UT_GenStub_Execute(BPLib_TLM_SendStoragePkt, Basic, NULL);

  return UT_GenStub_GetReturnValue(BPLib_TLM_SendStoragePkt, BPLib_Status_t);
}
