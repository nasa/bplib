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

#ifndef BPLIB_TLM_H
#define BPLIB_TLM_H

/*
** Include
*/

#include "bplib.h"


/*
** Exported Functions
*/

/**
 * \brief Telemetry Management initialization
 *
 *  \par Description
 *       TLM initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
BPLib_Status_t BPLib_TLM_Init(void);

/**
 * \brief Telemetry Management Send Per Node MIB configuration HK packet
 *
 *  \par Description
 *       Telemetry Management Send Per Node MIB configuration HK packet
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] Telemetry payload data NodeMIBConfigTlmPayload
 * 
 *  \return Execution status
 *  \retval BPLIB_SUCCESS BPLib_TLM_SendNodeMibConfigPkt was successful
 *  \retval BPLIB_PL_NULL_CALLBACK_ERROR when the callback is NULL
 */
BPLib_Status_t BPLib_TLM_SendNodeMibConfigPkt(BPLib_NodeMibConfigHkTlm_Payload_t* NodeMIBConfigTlmPayload);

/**
 * \brief Telemetry Management Send Per Source MIB configuration HK packet
 *
 *  \par Description
 *       Telemetry Management Send Per Source MIB configuration HK packet
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] Telemetry payload data SrcMIBConfigTlmPayload
 * 
 *  \return Execution status
 *  \retval BPLIB_SUCCESS BPLib_TLM_SendPerSourceMibConfigPkt was successful
 *  \retval BPLIB_PL_NULL_CALLBACK_ERROR when the callback is NULL
 */
BPLib_Status_t BPLib_TLM_SendPerSourceMibConfigPkt(BPLib_SourceMibConfigHkTlm_Payload_t* SrcMIBConfigTlmPayload);

/**
 * \brief Telemetry Management Send Per Node MIB counter HK packet
 *
 *  \par Description
 *       Telemetry Management Send Per Node MIB counter HK packet
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] Telemetry payload data NodeMIBCounterTlmPayload
 * 
 *  \return Execution status
 *  \retval BPLIB_SUCCESS BPLib_TLM_SendNodeMibCounterPkt was successful
 *  \retval BPLIB_PL_NULL_CALLBACK_ERROR when the callback is NULL
 */
BPLib_Status_t BPLib_TLM_SendNodeMibCounterPkt(BPLib_NodeMibCountersHkTlm_Payload_t* NodeMIBCounterTlmPayload);

/**
 * \brief Telemetry Management Send Per Source MIB counter HK packet
 *
 *  \par Description
 *       Telemetry Management Send Per Source MIB counter HK packet
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] Telemetry payload data SrcMIBCounterTlmPayload
 * 
 *  \return Execution status
 *  \retval BPLIB_SUCCESS BPLib_TLM_SendPerSourceMibCounterPkt was successful
 *  \retval BPLIB_PL_NULL_CALLBACK_ERROR when the callback is NULL
 */
BPLib_Status_t BPLib_TLM_SendPerSourceMibCounterPkt(BPLib_SourceMibCountersHkTlm_Payload_t* SrcMIBCounterTlmPayload);

/**
 * \brief Telemetry Management Send Channel/Contact HK packet
 *
 *  \par Description
 *       Telemetry Management Send Channel/Contact HK packet
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] Telemetry payload data ChannelContactTlmPayload
 * 
 *  \return Execution status
 *  \retval BPLIB_SUCCESS BPLib_TLM_SendChannelContactPkt was successful
 *  \retval BPLIB_PL_NULL_CALLBACK_ERROR when the callback is NULL
 */
BPLib_Status_t BPLib_TLM_SendChannelContactPkt(BPLib_ChannelContactStatHkTlm_Payload_t* ChannelContactTlmPayload);

/**
 * \brief Telemetry Management Send Storage HK packet
 *
 *  \par Description
 *       Telemetry Management Send Storage HK packet
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] Telemetry payload data StorTlmPayload
 * 
 *  \return Execution status
 *  \retval BPLIB_SUCCESS BPLib_TLM_SendStoragePkt was successful
 *  \retval BPLIB_PL_NULL_CALLBACK_ERROR when the callback is NULL
 */
BPLib_Status_t BPLib_TLM_SendStoragePkt(BPLib_StorageHkTlm_Payload_t* StorTlmPayload);

#endif /* BPLIB_TLM_H */

