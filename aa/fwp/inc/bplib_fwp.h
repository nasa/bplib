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

#ifndef BPLIB_FWP_H
#define BPLIB_FWP_H

/*
** Include
*/

#include "bplib.h"

/*
** Type Definitions
*/

typedef struct 
{
    /* Time Proxy function callbacks */
    int64_t (*BPA_TIMEP_GetMonotonicTime)(void);
    void (*BPA_TIMEP_GetHostEpoch)(BPLib_TIME_Epoch_t *Epoch);
    BPLib_TIME_ClockState_t (*BPA_TIMEP_GetHostClockState)(void);
    int64_t (*BPA_TIMEP_GetHostTime)(void);
    
    /* PerfLog Proxy function callbacks */
    void (*BPA_PERFLOGP_Entry)(uint32_t PerfLogID);
    void (*BPA_PERFLOGP_Exit)(uint32_t PerfLogID);

    /* Table Proxy function callbacks */
    int32_t (*BPA_TABLEP_SingleTableUpdate)(int16_t TblHandle);

    /* Event Proxy function callbacks */
    BPLib_Status_t (*BPA_EVP_Init)(void);
    BPLib_Status_t (*BPA_EVP_SendEvent)(uint16_t EventID, BPLib_EM_EventType_t EventType, char const* EventText);

    /* ADU Proxy function callbacks */
    BPLib_Status_t (*BPA_ADUP_AddApplication)(uint8_t ChanId);
    BPLib_Status_t (*BPA_ADUP_StartApplication)(uint8_t ChanId);
    BPLib_Status_t (*BPA_ADUP_StopApplication)(uint8_t ChanId);

    /* Add other proxies' function callbacks here: TODO */

} BPLib_FWP_ProxyCallbacks_t;

/*
** Channel Config Table
*/
typedef struct
{
    uint32_t    Version;
    uint32_t    CrcType;
    uint64_t    BundleProcFlags;
    char        DestEID[BPLIB_MAX_EID_LENGTH];
    char        SrcEID[BPLIB_MAX_EID_LENGTH];
    char        ReportToEID[BPLIB_MAX_EID_LENGTH];
    uint64_t    LifeTime;
} BPLib_PriBlkConfig_t;

typedef struct
{
    uint32_t    BlockType;
    uint32_t    BlockNum;
    uint64_t    BlockProcFlags;
    uint32_t    CrcType;
    uint32_t    Spare;
} BPLib_CanBlkConfig_t;

typedef enum {
    Active,
    PassiveDefer,
    PassiveAbandon,
} RegistrationState_t;

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
    BPLib_PriBlkConfig_t   PriBlkConfig;
    BPLib_CanBlkConfig_t   CanBlkConfig[BPLIB_MAX_NUM_CANONICAL_BLOCK];
} BPLib_ChannelSet_t;

typedef struct
{
    BPLib_ChannelSet_t ChannelSet[BPLIB_MAX_NUM_CHANNELS];
} BPLib_ChannelTable_t;

/*
** Global Data
*/

extern BPLib_FWP_ProxyCallbacks_t BPLib_FWP_ProxyCallbacks;


/*
** Exported Functions
*/

/**
 * \brief Framework Proxy initialization
 *
 *  \par Description
 *       FWP initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
BPLib_Status_t BPLib_FWP_Init(BPLib_FWP_ProxyCallbacks_t Callbacks);


#endif /* BPLIB_FWP_H */
