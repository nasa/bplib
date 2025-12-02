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

#ifndef BPLIB_NC_INTERNAL_H
#define BPLIB_NC_INTERNAL_H

/*
** Include
*/

#include "bplib_api_types.h"
#include "bplib_nc_payloads.h"


/*
** Global Data
*/

extern BPLib_SourceMibConfigHkTlm_Payload_t    BPLib_NC_SourceMibConfigPayload;
extern BPLib_NodeMibConfigHkTlm_Payload_t      BPLib_NC_NodeMibConfigPayload;
extern BPLib_ChannelContactStatHkTlm_Payload_t BPLib_NC_ChannelContactStatsPayload;


/*
** Function Definitions
*/

/**
 * \brief Update contact telemetry
 *
 * \par Description
 *      Update the contact telemetry values to new contact configuration values
 *
 */
void BPLib_NC_UpdateContactHkTlm(void);

/**
 * \brief Update channel telemetry
 *
 * \par Description
 *      Update the channel telemetry values to new channel configuration values
 *
 */
void BPLib_NC_UpdateChannelHkTlm(void);

/**
 * \brief Validate PARAM_BUNDLE_SIZE_NO_FRAGMENT
 *
 * \param[in] TblDataPtr Pointer to Node MIB configuration
 *
 * \return Execution status
 * \retval True MIB item is valid
 * \retval False MIB item is not valid
 */
bool BPLib_NC_ValidParamBundleSizeNoFragment(BPLib_NC_MibPerNodeConfig_t *TblDataPtr);

/**
 * \brief Validate PARAM_SET_MAX_SEQUENCE_NUM
 *
 * \param[in] TblDataPtr Pointer to Node MIB configuration
 *
 * \return Execution status
 * \retval True MIB item is valid
 * \retval False MIB item is not valid
 */
bool BPLib_NC_ValidParamSetMaxSequenceNum(BPLib_NC_MibPerNodeConfig_t *TblDataPtr);

/**
 * \brief Validate PARAM_SET_MAX_PAYLOAD_LENGTH
 *
 * \param[in] TblDataPtr Pointer to Node MIB configuration
 *
 * \return Execution status
 * \retval True MIB item is valid
 * \retval False MIB item is not valid
 */
bool BPLib_NC_ValidParamMaxPayloadLength(BPLib_NC_MibPerNodeConfig_t *TblDataPtr);

/**
 * \brief Validate PARAM_SET_MAX_BUNDLE_LENGTH
 *
 * \param[in] TblDataPtr Pointer to Node MIB configuration
 *
 * \return Execution status
 * \retval True MIB item is valid
 * \retval False MIB item is not valid
 */
bool BPLib_NC_ValidParamMaxBundleLength(BPLib_NC_MibPerNodeConfig_t *TblDataPtr);

/**
 * \brief Validate PARAM_SET_NODE_DTN_TIME
 *
 * \param[in] TblDataPtr Pointer to Node MIB configuration
 *
 * \return Execution status
 * \retval True MIB item is valid
 * \retval False MIB item is not valid
 */
bool BPLib_NC_ValidParamSetNodeDtnTime(BPLib_NC_MibPerNodeConfig_t *TblDataPtr);

/**
 * \brief Validate PARAM_SET_BEHAVIOR_EVENT_REPORTING
 *
 * \param[in] TblDataPtr Pointer to Node MIB configuration
 *
 * \return Execution status
 * \retval True MIB item is valid
 * \retval False MIB item is not valid
 */
bool BPLib_NC_ValidParamSetBehaviorEventReporting(BPLib_NC_MibPerNodeConfig_t *TblDataPtr);

/**
 * \brief Validate PARAM_SET_MAX_LIFETIME
 *
 * \param[in] TblDataPtr Pointer to Node MIB configuration
 *
 * \return Execution status
 * \retval True MIB item is valid
 * \retval False MIB item is not valid
 */
bool BPLib_NC_ValidParamSetMaxLifetime(BPLib_NC_MibPerNodeConfig_t *TblDataPtr);

#endif // BPLIB_NC_INTERNAL_H
