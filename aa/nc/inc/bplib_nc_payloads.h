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
    uint8_t ChanId;         /**< \brief Channel ID */
    uint8_t Spare[3];       /**< \brief Spare bytes */
} BPLib_AddApplication_Payload_t;

typedef struct
{
    uint8_t ChanId;         /**< \brief Channel ID */
    uint8_t Spare[3];       /**< \brief Spare bytes */
} BPLib_RemoveApplication_Payload_t;

typedef struct
{
    uint32_t ExampleParameter;
} BPLib_SetRegistrationState_Payload_t;

typedef struct
{
    uint8_t ChanId;         /**< \brief Channel ID */
    uint8_t Spare[3];       /**< \brief Spare bytes */
} BPLib_StartApplication_Payload_t;

typedef struct
{
    uint8_t ChanId;         /**< \brief Channel ID */
    uint8_t Spare[3];       /**< \brief Spare bytes */
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
    uint32_t ExampleParameter;
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
    uint32_t ExampleParameter;
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
    uint32_t State;             /**< \brief Configured, Running, or Off */
    uint32_t RegistrationState; /**< \brief Active, PassiveDefered or PassiveAbandon */
    uint32_t OutputQueueID;     /**< \brief Output queue ID */
} BPLib_ChannelHkTlmPayloadSet_t;

typedef struct
{
    uint32_t    ContactID;                              /**< \brief Unique ID for this contact */
    uint32_t    State;                                  /**< \brief Configured, Running, or Off */
    char        EIDs[BPLIB_MAX_EID_LENGTH];             /**< \brief List of EIDs */
    uint32_t    OutputQueueID;                          /**< \brief Output queue ID */
    uint32_t    Spare1;
} BPLib_ContactHkTlmPayloadSet_t;

typedef struct
{
    BPLib_ChannelHkTlmPayloadSet_t ChannelStatus[BPLIB_MAX_NUM_CHANNELS]; /**< \brief Status for each channel */
    BPLib_ContactHkTlmPayloadSet_t ContactStatus[BPLIB_MAX_NUM_CONTACTS]; /**< \brief Status for each contact */
    uint32_t Spare2;
    uint32_t TimeBootEra;                   /**< \brief Boot Era for Monotonic Time */
    int64_t  MonotonicTime;                 /**< \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;             /**< \brief Time Correlation Factor */
} BPLib_ChannelContactStatHkTlm_Payload_t;

/**
 * \brief Node MIB configuration housekeeping payload
 */
typedef struct
{
    char SystemNodeName[BPLIB_MAX_NUM_STRING];                  /**< \brief Textual name of a DTN Node entity */
    char SystemNodeOwner[BPLIB_MAX_NUM_STRING];                 /**< \brief Textual identifier of the node manager */
    char SystemSoftwareExec[BPLIB_MAX_NUM_STRING];              /**< \brief Textual identification of the underlying OS or excutive */
    char SystemSoftwareExecVersion[BPLIB_MAX_NUM_STRING];       /**< \brief Textual representation of the version and patch-level */
    char BundleAgentVersion[BPLIB_MAX_NUM_STRING];              /**< \brief Version of the executing Node */
    char BundleAgentOperationalState[BPLIB_MAX_NUM_STRING];     /**< \brief Indication of the Node operational state */
    char BundleAgentConfiguration[BPLIB_MAX_NUM_STRING];        /**< \brief Indication of the current Node configuration */
    uint32_t BundleAgentNodeNumber;                             /**< \brief Node Number */
    uint32_t BundleAgentAvailableStorage;                       /**< \brief Amount of memory initially allocated for bundle storage */
    uint32_t BundleAgentCtdbSize;                               /**< \brief Storage allocatted to Custody Transfer Database (CTDB) use */
    uint32_t ParamSetMaxSequenceNum;                            /**< \brief Maximum bundle sequence number */
    uint32_t ParamSetMaxPayloadLength;                          /**< \brief Maximum payload length for fragmentation */
    uint32_t ParamSetMaxBundleLength;                           /**< \brief Maximum bundle length */
    uint32_t ParamSetMaxTransmissionId;                         /**< \brief Maximum value for the transmissionID before it wraps back around */
    uint32_t ParamSetMaxForwardingRetries;                      /**< \brief Number of Bundle Forwarding Retries to attempt before giving up */
    uint32_t ParamSetMaxSimultaneousContacts;                   /**< \brief Maximum number of simultaneous contacts */
    uint32_t ParamSetBehaviorEventReporting;                    /**< \brief Indication that only events at specified level or above are generated and reported via the M&C interface */
    bool ParamSupportBIBE;                                      /**< \brief Indication of node’s Bundle In Bundle Encapsulation (BIBE) capability */
    bool ParamSupportCustody;                                   /**< \brief Indication of whether the node can become a Custodian */
    bool ParamSupportConfidentiality;                           /**< \brief Indication of node’s confidentiality handling capabilities */
    bool ParamSetBehaviorReturnToSender;                        /**< \brief Flag indicating that bundles which cannot be forwarded should be returned to sender */
    bool ParamSetBehaviorTransIdReuse;                          /**< \brief Flag indicating whether transmission IDs should be reused on retries */
    bool ParamSetBehaviorTransIdRollover;                       /**< \brief Flag indicating that the transmissionID has wrapped around */
    bool ParamSetBehaviorWrapResponse;                          /**< \brief Indication of whether the oldest or newest bundle will be dropped when CTDB buffer is full */
    uint8_t  Version;                                           /**< \brief Bundle Protocol version */
    uint32_t BundleSizeFragment;                                /**< \brief Maximum size of bundles that can traverse DTN without additional bundle layer fragmentation */
    uint32_t TimeBootEra;                                       /**< \brief Boot Era for Monotonic Time */
    uint64_t MonotonicTime;                                     /**< \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;                                 /**< \brief Time Correlation Factor */
} BPLib_NodeMibConfigHkTlm_Payload_t;

/**
 * \brief Source MIB configuration housekeeping payload
 */
typedef struct
{
    char SourceEID[BPLIB_MAX_NUM_STRING];       /**< \brief Source EID to which this telemetry corresponds */
    bool BundleSetBehaviorReceivedBSRGenerate;  /**< \brief Flag indicating that Bundle Status Report (BSR) should be generated for bundles received */
    bool BundleSetBehaviorAcceptedBSRGenerate;  /**< \brief Flag indicating that BSR should be generated for bundles accepted to custody */
    bool BundleSetBehaviorForwardedBSRGenerate; /**< \brief Flag indicating that BSR should be generated for bundles forwarded */
    bool BundleSetBehaviorDeliveredBSRGenerate; /**< \brief Flag indicating that BSR should be generated for bundles delivered */
    bool BundleSetBehaviorDeletedBSRGenerate;   /**< \brief Flag indicating that BSR should be generated for bundles deleted */ 
    bool BundleSetBehaviorReceivedCBRGenerate;  /**< \brief Flag indicating that Compressed Bundle Reporting (CBR) should be generated for bundles received */
    bool BundleSetBehaviorAcceptedCBRGenerate;  /**< \brief Flag indicating that CBR should be generated for bundles accepted to custody */
    bool BundleSetBehaviorForwardedCBRGenerate; /**< \brief Flag indicating that CBR should be generated for bundles forwarded */
    bool BundleSetBehaviorDeliveredCBRGenerate; /**< \brief Flag indicating that CBR should be generated for bundles delivered */
    bool BundleSetBehaviorDeletedCBRGenerate;   /**< \brief Flag indicating that CBR should be generated for bundles deleted */
    uint16_t Spare1;
    uint32_t ParamSetMaxLifetime;               /**< \brief Maximum bundle lifetime */
    uint32_t ParamSetMaxBSRGenerationRate;      /**< \brief Maximum number of BSRs per minute that a node can generate overall and on behalf of each source */
    uint32_t ParamSetMaxCBRGenerationRate;      /**< \brief Maximum number of CBRs per minute that a node can generate overall and on behalf of each source */
} BPLib_SourceMibConfigSet_t;

typedef struct
{
    BPLib_SourceMibConfigSet_t SourceConfigs[BPLIB_MAX_NUM_MIB_SETS];
    uint32_t Spare2;
    uint32_t TimeBootEra;                   /**< \brief Boot Era for Monotonic Time */
    int64_t  MonotonicTime;                 /**< \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;             /**< \brief Time Correlation Factor */
} BPLib_SourceMibConfigHkTlm_Payload_t;


/*
** MIB Config Per Node Policy Table
*/
typedef struct
{
    char        SysNodeName[BPLIB_MAX_EID_LENGTH];
    char        SysNodeOwner[BPLIB_MAX_EID_LENGTH];
    char        SysSoftExec[BPLIB_MAX_EID_LENGTH];
    char        SysSoftExecVer[BPLIB_MAX_EID_LENGTH];
    uint32_t    BundleAgentNum;
    char        BundleAgentVer[BPLIB_MAX_EID_LENGTH];
    char        BundleAgentOpState[BPLIB_MAX_EID_LENGTH];
    char        BundleAgentConfig[BPLIB_MAX_EID_LENGTH];
    uint32_t    BundleAgentAvailStorage;
    uint32_t    BundleAgentCtdbSize;
    uint32_t    BundleSizeNoFrag;
    uint32_t    ParamSetMaxSeqenceNum;
    uint32_t    ParamSetMaxPayloadLength;
    uint32_t    ParamSetMaxBundleLength;
    uint32_t    ParamSetMaxTransmissionID;
    uint32_t    ParamSetMaxForwardingRetries;
    uint32_t    ParamSetMaxSimultaneousContacts;
    uint32_t    ParamSetBehaviorEventReporting;
    bool        ParamSetBehaviorReturnToSender;
    bool        ParamSetBehaviorTransIDReuse;
    bool        ParamSetBehaviorTransIDRollover;
    bool        ParamSetBehaviorWrapResponse;
    bool        ParamSupportBIBE;
    bool        ParamSupportCustody;
    bool        ParamSupportConfidentiality;
    bool        Spare;

} BPLib_NC_MIBConfigPNTable_t;


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
    uint32_t            ParamSetMaxLifetime;
    uint32_t            ParamSetMaxBSRGenerationRate;
    uint32_t            ParamSetMaxCBRGenerationRate;
    uint8_t             BundleSetBehaviorReceivedBSRGenerate;
    uint8_t             BundleSetBehaviorAcceptedBSRGenerate;
    uint8_t             BundleSetBehaviorForwardedBSRGenerate;
    uint8_t             BundleSetBehaviorDeliveredBSRGenerate;
    uint8_t             BundleSetBehaviorDeletedBSRGenerate;
    uint8_t             BundleSetBehaviorReceivedCBRGenerate;
    uint8_t             BundleSetBehaviorAcceptedCBRGenerate;
    uint8_t             BundleSetBehaviorForwardedCBRGenerate;
    uint8_t             BundleSetBehaviorDeliveredCBRGenerate;
    uint8_t             BundleSetBehaviorDeletedCBRGenerate;  
    uint16_t            Spare;
} BPLib_NC_MIBConfigPSSet_t;

typedef struct
{
    BPLib_NC_MIBConfigPSSet_t MIB_PS_Set[BPLIB_MAX_NUM_MIB_PS_CFG_ENTRIES];
} BPLib_NC_MIBConfigPSTable_t;

#endif // BPLIB_NC_PAYLOADS_H