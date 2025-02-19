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

#ifndef BPLIB_PI_H
#define BPLIB_PI_H

/*
** Include
*/

#include "bplib_api_types.h"
#include "bplib_cfg.h"
#include "bplib_as.h"
#include "bplib_eid.h"
#include "bplib_qm.h"


/*
** Type Definitions
*/

/**
** \brief Primary block configurations
*/
typedef struct
{
    uint8_t          Version;  // TODO can this be anything other than v7??
    BPLib_CRC_Type_t CrcType;
    uint64_t         BundleProcFlags;
    BPLib_EID_t      DestEID;
    BPLib_EID_t      SrcEID;
    BPLib_EID_t      ReportToEID;
    uint64_t         Lifetime;
} BPLib_PI_PriBlkConfig_t;

/**
** \brief Canonical block configurations
*/
typedef struct
{
    uint32_t         BlockType;
    uint32_t         BlockNum;
    uint64_t         BlockProcFlags;
    BPLib_CRC_Type_t CrcType;
    uint32_t         Spare;
} BPLib_PI_CanBlkConfig_t;

/**
** \brief Channel registration state
*/
typedef enum {
    BPLIB_PI_ACTIVE = 0,
    BPLIB_PI_PASSIVE_DEFER = 1,
    BPLIB_PI_PASSIVE_ABANDON = 2
} BPLib_PI_RegistrationState_t;

/**
** \brief Channel configurations
*/
typedef struct
{
    bool                    AddAutomatically;
    bool                    RequestCustody;
    bool                    IncludePrevNodeBlk;
    bool                    IncludeHopCountBlk;
    bool                    AduWrapping;
    bool                    AduUnwrapping;
    uint8_t                 RegState;
    uint8_t                 HopLimit;
    uint32_t                LocalServiceNumber;
    uint32_t                MaxBundlePayloadSize;
    BPLib_PI_PriBlkConfig_t PriBlkConfig;
    BPLib_PI_CanBlkConfig_t CanBlkConfig[BPLIB_MAX_NUM_EXTENSION_BLOCKS + 1];
} BPLib_PI_Config_t;

/**
** \brief Channel configuration table
*/
typedef struct
{
    BPLib_PI_Config_t Configs[BPLIB_MAX_NUM_CHANNELS];
} BPLib_PI_ChannelTable_t;


/*
** Exported Functions
*/

/**
 * \brief Add Application
 *
 *  \par Description
 *       Add application configurations to Payload Interface
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] ChanId Channel ID
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Operation was successful
 */
BPLib_Status_t BPLib_PI_AddApplication(uint8_t ChanId);

/**
 * \brief Remove Application
 *
 *  \par Description
 *       Remove application configurations from Payload Interface
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] ChanId Channel ID
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Operation was successful
 */
BPLib_Status_t BPLib_PI_RemoveApplication(uint8_t ChanId);

/**
 * \brief Validate configurations
 *
 *  \par Description
 *       Validate configuration table parameters
 *
 *  \par Assumptions, External Events, and Notes:
 *       - This function is called by whatever external task handles table management.
 *         Every time a new channel table is loaded, this function should be called to
 *         validate its parameters.
 *
 *  \param[in] TblData Pointer to the config table
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Validation was successful
 */
BPLib_Status_t BPLib_PI_ValidateConfigs(void *TblData);

/**
 * \brief Bundle Ingress
 *
 *  \par Description
 *       PI receives an ADU, turns it into a deserialized bundle, and pushes it to QM
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] Inst Pointer to an the BPLib instance state struct
 *  \param[in] ChanId Channel ID
 *  \param[in] AduPtr Pointer to the ADU
 *  \param[in] AduSize Length of the provided ADU
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Operation was successful
 */
BPLib_Status_t BPLib_PI_Ingress(BPLib_Instance_t *Inst, uint8_t ChanId, 
                                                        void *AduPtr, size_t AduSize);

/**
 * \brief Bundle Egress
 *
 *  \par Description
 *       PI pulls an ADU off the relevant channel out queue to return to calling function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] Inst Pointer to an the BPLib instance state struct
 *  \param[in] ChanId Channel ID
 *  \param[in] AduPtr Pointer to the returned ADU
 *  \param[in] AduSize Pointer to set to the received ADU's size
 *  \param[in] BufLen Length of the buffer provided (the ADU pointer)
 *  \param[in] Timeout Total time to pend on channel out queue (in milliseconds)
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Operation was successful
 */
BPLib_Status_t BPLib_PI_Egress(BPLib_Instance_t *Inst, uint8_t ChanId, void *AduPtr, 
                                    size_t *AduSize, size_t BufLen, uint32_t Timeout);

#endif /* BPLIB_PI_H */
