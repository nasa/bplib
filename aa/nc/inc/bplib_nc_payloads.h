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

#ifndef BPLIB_NC_PAYLOADS_H
#define BPLIB_NC_PAYLOADS_H

/* ======== */
/* Includes */
/* ======== */

#include "bplib_api_types.h"
#include "bplib_cfg.h"
#include "bplib_eid.h"
#include "bplib_cla.h"

/* =================== */
/* Payload Definitions */
/* =================== */

typedef struct
{
    uint16_t MibArrayIndex; /** \brief Index into source MIB counter array corresponding to counters to reset */
    uint8_t  Spare[2];      /** \brief Spare for 32-bit alignment */
    uint32_t Counter;       /** \brief Counter to reset */
} BPLib_ResetCounter_Payload_t;

typedef struct
{
    uint16_t MibArrayIndex; /** \brief Index into source MIB counter array corresponding to counters to reset */
    uint8_t  Spare[2];      /** \brief Spare for 32-bit alignment */
} BPLib_ResetSourceCounters_Payload_t;

typedef struct
{
    uint16_t MibArrayIndex; /** \brief Index into source MIB counter array corresponding to counters to reset */
    uint8_t  Spare[2];      /** \brief Spare for 32-bit alignment */
} BPLib_ResetErrorCounters_Payload_t;

typedef struct
{
    uint8_t ChanId;         /** \brief Channel ID */
    uint8_t Spare[3];       /** \brief Spare bytes */
} BPLib_AddApplication_Payload_t;

typedef struct
{
    uint8_t ChanId;         /** \brief Channel ID */
    uint8_t Spare[3];       /** \brief Spare bytes */
} BPLib_RemoveApplication_Payload_t;

typedef struct
{
    uint32_t ExampleParameter;
} BPLib_SetRegistrationState_Payload_t;

typedef struct
{
    uint8_t ChanId;         /** \brief Channel ID */
    uint8_t Spare[3];       /** \brief Spare bytes */
} BPLib_StartApplication_Payload_t;

typedef struct
{
    uint8_t ChanId;         /** \brief Channel ID */
    uint8_t Spare[3];       /** \brief Spare bytes */
} BPLib_StopApplication_Payload_t;

typedef struct
{
    uint32_t ExampleParameter;
} BPLib_AddAuthSources_Payload_t;

typedef struct
{
    uint32_t ExampleParameter;
} BPLib_RemoveAuthSources_Payload_t;

typedef struct
{
    uint32_t ExampleParameter;
} BPLib_AddAuthCustodySources_Payload_t;

typedef struct
{
    uint32_t ExampleParameter;
} BPLib_RemoveAuthCustodySources_Payload_t;

typedef struct
{
    uint32_t ExampleParameter;
} BPLib_AddAuthCustodians_Payload_t;

typedef struct
{
    uint32_t ExampleParameter;
} BPLib_RemoveAuthCustodians_Payload_t;

typedef struct
{
    uint32_t ExampleParameter;
} BPLib_AddAuthReportToEid_Payload_t;

typedef struct
{
    uint32_t ExampleParameter;
} BPLib_RemoveAuthReportToEid_Payload_t;

typedef struct
{
    uint32_t ExampleParameter;
} BPLib_AddLatency_Payload_t;

typedef struct
{
    uint32_t ExampleParameter;
} BPLib_RemoveLatency_Payload_t;

typedef struct
{
    uint32_t ContactId;
} BPLib_ContactSetup_Payload_t;

typedef struct
{
    uint32_t ContactId;
} BPLib_ContactStart_Payload_t;

typedef struct
{
    uint32_t ContactId;
} BPLib_ContactStop_Payload_t;

typedef struct
{
    uint32_t ContactId;
} BPLib_ContactTeardown_Payload_t;

typedef struct
{
    BPLib_EID_Pattern_t EID_Patterns[BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET];
} BPLib_AddMibArrayKey_Payload_t;

typedef struct
{
    uint32_t ExampleParameter;
} BPLib_RemoveMibArrayKey_Payload_t;

typedef struct
{
    BPLib_EID_Pattern_t EidPattern;
    uint32_t            MibItem;
    uint32_t            Value;
} BPLib_SetMibItem_Payload_t;

typedef struct
{
    uint32_t ExampleParameter;
} BPLib_AddStorageAllocation_Payload_t;

typedef struct
{
    uint32_t ExampleParameter;
} BPLib_RemoveStorageAllocation_Payload_t;

/**
 * \brief Channel/contact status housekeeping payload
 */
typedef struct
{
    uint32_t LocalServiceNum;   /**< \brief Service number for application sending/receiving ADUs on this channel */
    uint32_t State;             /**< \brief Added, started, stopped, or removed */
    uint32_t RegistrationState; /**< \brief Active, PassiveDefered or PassiveAbandon */
    uint32_t Spare;
} BPLib_ChannelHkTlmPayloadSet_t;

typedef struct
{
    uint32_t State;             /**< \brief Set up, started, stopped, or torn down */
    uint32_t Spare;
    BPLib_EID_Pattern_t DestEIDs[BPLIB_MAX_CONTACT_DEST_EIDS];  /**< \brief Destination EIDs */
} BPLib_ContactHkTlmPayloadSet_t;

typedef struct
{
    BPLib_ChannelHkTlmPayloadSet_t ChannelStatus[BPLIB_MAX_NUM_CHANNELS]; /**< \brief Status for each channel */
    BPLib_ContactHkTlmPayloadSet_t ContactStatus[BPLIB_MAX_NUM_CONTACTS]; /**< \brief Status for each contact */

    int64_t  MonotonicTime;                 /**< \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;             /**< \brief Time Correlation Factor */
} BPLib_ChannelContactStatHkTlm_Payload_t;


#define BPLIB_NC_NODE_MIB_CONFIG_NUM 7
#define BPLIB_NC_SOURCE_MIB_CONFIG_BASE 6
#define BPLIB_NC_SOURCE_MIB_CONFIG_NUM 13


typedef enum
{
    /* Node-only configs */
    PARAM_BUNDLE_SIZE_NO_FRAGMENT         = 0,  /** \brief Max bundle size before fragmentation */
    PARAM_SET_MAX_SEQUENCE_NUM            = 1,  /** \brief Max bundle sequence number allowable */
    PARAM_SET_MAX_PAYLOAD_LENGTH          = 2,  /** \brief Max payload length for fragmentation */
    PARAM_SET_MAX_BUNDLE_LENGTH           = 3,  /** \brief Max bundle length for processing */
    PARAM_SET_NODE_DTN_TIME               = 4,  /** \brief Time being tracked by the node */
    PARAM_SET_BEHAVIOR_EVENT_REPORTING    = 5,  /** \brief Inclusive level of events to be generated */

    /* Node and source configs */
    PARAM_SET_MAX_LIFETIME                = 6,  /** \brief Max bundle lifetime for retention of the bundle */

    /* Source-only configs */
    PARAM_SET_MAX_BSR_GENERATION_RATE     = 7,  /** \brief Maximum number of BSRs per minute that a node can generate overall and on behalf of each source */
    PARAM_SET_MAX_CBR_GENERATION_RATE     = 8,  /** \brief Maximum number of CBRs per minute that a node can generate overall and on behalf of each source */
    BUNDLE_SET_BEHAVIOR_RCVD_BSR_GENERATE = 9,  /** \brief Flag indicating that BSR should be generated for bundles received */
    BUNDLE_SET_BEHAVIOR_ACPT_BSR_GENERATE = 10, /** \brief Flag indicating that BSR should be generated for bundles accepted to custody */
    BUNDLE_SET_BEHAVIOR_FWRD_BSR_GENERATE = 11, /** \brief Flag indicating that BSR should be generated for bundles forwarded */
    BUNDLE_SET_BEHAVIOR_DLVR_BSR_GENERATE = 12, /** \brief Flag indicating that BSR should be generated for bundles delivered */
    BUNDLE_SET_BEHAVIOR_DLTD_BSR_GENERATE = 13, /** \brief Flag indicating that BSR should be generated for bundles deleted */ 
    BUNDLE_SET_BEHAVIOR_RCVD_CBR_GENERATE = 14, /** \brief Flag indicating that CBR should be generated for bundles received */
    BUNDLE_SET_BEHAVIOR_ACPT_CBR_GENERATE = 15, /** \brief Flag indicating that CBR should be generated for bundles accepted to custody */
    BUNDLE_SET_BEHAVIOR_FWRD_CBR_GENERATE = 16, /** \brief Flag indicating that CBR should be generated for bundles forwarded */
    BUNDLE_SET_BEHAVIOR_DLVR_CBR_GENERATE = 17, /** \brief Flag indicating that CBR should be generated for bundles delivered */
    BUNDLE_SET_BEHAVIOR_DLTD_CBR_GENERATE = 18, /** \brief Flag indicating that CBR should be generated for bundles deleted */
} BPLib_NC_Config_t;

/*
** MIB Config Per Node Policy Table
*/
typedef struct
{
    BPLib_EID_t InstanceEID;                    /** \brief Endpoint ID of this BP instance */

    uint32_t Configs[BPLIB_NC_NODE_MIB_CONFIG_NUM];
    uint32_t Spare;

} BPLib_NC_MibPerNodeConfig_t;



/*
** MIB Config Per Source Policy Table
*/
typedef struct
{
    /**
     * The SrcEID field is the EID pattern.
     * The EIDPattern field was a string value of "ipn" or "dtn". It is now
     * the BPLib_EID_Scheme_t enum in BPLib_EID_Pattern_t.
     */
    BPLib_EID_Pattern_t SrcEIDs[BPLIB_MAX_NUM_MIB_PS_EID_PATTERNS];
    uint32_t            Configs[BPLIB_NC_SOURCE_MIB_CONFIG_NUM];
    uint32_t            Spare;
} BPLib_NC_MIBConfigPSSet_t;

typedef struct
{
    BPLib_NC_MIBConfigPSSet_t MIB_PS_Set[BPLIB_MAX_NUM_MIB_PS_CFG_ENTRIES];
} BPLib_NC_MIBConfigPSTable_t;

/**
 * \brief Node MIB configuration housekeeping payload
 */
typedef struct
{
    BPLib_NC_MibPerNodeConfig_t Values; /** \brief All configuration values as recorded in the MIB per Node Configuration */

    int64_t MonotonicTime;              /** \brief Monotonic Time Counter */
    int64_t CorrelationFactor;          /** \brief Time Correlation Factor */
} BPLib_NodeMibConfigHkTlm_Payload_t;

/**
 * \brief Source MIB configuration housekeeping payload
 */
typedef struct
{
    char SourceEID[BPLIB_MAX_STR_LENGTH];           /** \brief Source EID to which this telemetry corresponds */
    uint32_t Configs[BPLIB_NC_SOURCE_MIB_CONFIG_NUM];

    /*
    ** TODO may be able to combine flag configs into a single uint32 instead of one each?
    ** Figure out logic later. Source EID also needs to become a BPLib_EID_t type
    */

    uint32_t Spare;
} BPLib_SourceMibConfigSet_t;

typedef struct
{
    BPLib_SourceMibConfigSet_t SourceConfigs[BPLIB_MAX_NUM_MIB_SETS];
    int64_t                    MonotonicTime;     /** \brief Monotonic Time Counter */
    int64_t                    CorrelationFactor; /** \brief Time Correlation Factor */
} BPLib_SourceMibConfigHkTlm_Payload_t;

#endif // BPLIB_NC_PAYLOADS_H