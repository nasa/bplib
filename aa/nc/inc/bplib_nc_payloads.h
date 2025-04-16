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

/*
** MIB Config Per Node Policy Table
*/
typedef struct
{
    BPLib_EID_t InstanceEID; /** \brief Endpoint ID of this instance of FSW for this endpoint */

    /* Node only */
    char     SystemNodeName[BPLIB_MAX_EID_LENGTH];              /** \brief Human readable name given to entity */
    char     SystemSoftwareExec[BPLIB_MAX_EID_LENGTH];          /** \brief ID of the OS or executive controlling the resources */
    char     SystemSoftwareExecVersion[BPLIB_MAX_EID_LENGTH];   /** \brief Version of software */
    char     BundleAgentSoftwareVersion[BPLIB_MAX_EID_LENGTH];  /** \brief Version of the Bundle Protocol Agent */
    char     BundleAgentOperationalState[BPLIB_MAX_EID_LENGTH]; /** \brief Operational state of Bundle Protocol Agent */
    char     BundleAgentConfiguration[BPLIB_MAX_EID_LENGTH];    /** \brief Configuration of Bundle Protocol Agent */
    char     ParamSupportedCLAs[BPLIB_MAX_EID_LENGTH];          /** \brief Supported CLAs */
    char     NodeActiveEndpoints[BPLIB_MAX_EID_LENGTH];         /** \brief List of active endpoints on the Node */
    uint32_t SystemNodeUpTime;                                  /** \brief The time in seconds since this node has been reinitialized */
    uint32_t NodeStartupCounter;                                /** \brief Number of times a node is started up */
    uint32_t BundleAgentAvailableStorage;                       /** \brief Total amount of memory allocated for bundle storage for the node  */
    uint32_t BundleSizeNoFragment;                              /** \brief Max bundle size before fragmentation */
    uint32_t BundleIngressRejectedRateBytesPerSec;              /** \brief Rate of bundles received from CLAs in bytes per second and then rejected */
    uint32_t BundleIngressRejectedRateBundlesPerSec;            /** \brief Rate of bundles received from CLAs in bundles per second and then rejected */
    uint32_t ParamSetMaxSequenceNum;                            /** \brief Max bundle sequence number allowable */
    uint32_t ParamSetMaxPayloadLength;                          /** \brief Max payload length for fragmentation */
    uint32_t ParamSetMaxBundleLength;                           /** \brief Max bundle length for processing */
    uint32_t ParamSetNodeDTNTime;                               /** \brief Time being tracked by the node */
    uint32_t ParamSetBehaviorEventReporting;                    /** \brief Inclusive level of events to be generated */

    /* Per Node and per Source */
    uint32_t ParamSetMaxLifetime;               /** \brief Max bundle lifetime for retention of the bundle */
    uint32_t KbytesCountStorageAvailable;       /** \brief Kilobytes free space left to store additional Bundles */
    uint32_t BundleIngressRateBytesPerSec;      /** \brief Rate of bundles received from CLAs in bytes per second */
    uint32_t BundleIngressRateBundlesPerSec;    /** \brief Rate of bundles received from CLAs in bundles per second */
    uint32_t BundleEgressRateBytesPerSec;       /** \brief Rate of bundles forwarded from CLAs in bytes per second */
    uint32_t BundleIngestedRateBundlesPerSec;   /** \brief Rate of bundles ingested locally in bundles per second */
    uint32_t BundleIngestedRateBytesPerSec;     /** \brief Rate of bundles ingested locally in bytes per second */
    uint32_t BundleDeliveryRateBundlesPerSec;   /** \brief Rate of bundles delivered locally in bundles per second */
    uint32_t BundleDeliveryRateBytesPerSec;     /** \brief Rate of bundles delivered locally in bytes per second */
    uint32_t BundleEgressRateBundlesPerSec;     /** \brief Rate of bundle forwarded from CLAs in bundles per second */
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

/**
 * \brief Channel/contact status housekeeping payload
 */
typedef struct
{
    uint32_t LocalServiceNum;   /** \brief Service number for application sending/receiving ADUs on this channel */
    uint32_t State;             /** \brief Configured, Running, or Off */
    uint32_t RegistrationState; /** \brief Active, PassiveDefered or PassiveAbandon */
    uint32_t OutputQueueID;     /** \brief Output queue ID */
} BPLib_ChannelHkTlmPayloadSet_t;

typedef struct
{
    uint32_t                    ContactID;                              /** \brief Unique ID for this contact */
    BPLib_CLA_ContactRunState_t State;                                  /** \brief Configured, Running, or Off */
    uint8_t                     Spare[3];
    char                        EIDs[BPLIB_MAX_EID_LENGTH];             /** \brief List of EIDs */
    uint32_t                    OutputQueueID;                          /** \brief Output queue ID */
    uint32_t                    Spare1;
} BPLib_ContactHkTlmPayloadSet_t;

typedef struct
{
    BPLib_ChannelHkTlmPayloadSet_t ChannelStatus[BPLIB_MAX_NUM_CHANNELS]; /** \brief Status for each channel */
    BPLib_ContactHkTlmPayloadSet_t ContactStatus[BPLIB_MAX_NUM_CONTACTS]; /** \brief Status for each contact */
    uint32_t Spare2;
    uint32_t TimeBootEra;                   /** \brief Boot Era for Monotonic Time */
    int64_t  MonotonicTime;                 /** \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;             /** \brief Time Correlation Factor */
} BPLib_ChannelContactStatHkTlm_Payload_t;

/**
 * \brief Node MIB configuration housekeeping payload
 */
typedef struct
{
    BPLib_NC_MIBConfigPNTable_t Values; /** \brief All configuration values as recorded in the MIB per Node Configuration */

    uint32_t TimeBootEra;               /** \brief Boot Era for Monotonic Time */
    uint64_t MonotonicTime;             /** \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;         /** \brief Time Correlation Factor */
} BPLib_NodeMibConfigHkTlm_Payload_t;

/**
 * \brief Source MIB configuration housekeeping payload
 */
typedef struct
{
    char SourceEID[BPLIB_MAX_NUM_STRING];       /** \brief Source EID to which this telemetry corresponds */
    bool BundleSetBehaviorReceivedBSRGenerate;  /** \brief Flag indicating that Bundle Status Report (BSR) should be generated for bundles received */
    bool BundleSetBehaviorAcceptedBSRGenerate;  /** \brief Flag indicating that BSR should be generated for bundles accepted to custody */
    bool BundleSetBehaviorForwardedBSRGenerate; /** \brief Flag indicating that BSR should be generated for bundles forwarded */
    bool BundleSetBehaviorDeliveredBSRGenerate; /** \brief Flag indicating that BSR should be generated for bundles delivered */
    bool BundleSetBehaviorDeletedBSRGenerate;   /** \brief Flag indicating that BSR should be generated for bundles deleted */ 
    bool BundleSetBehaviorReceivedCBRGenerate;  /** \brief Flag indicating that Compressed Bundle Reporting (CBR) should be generated for bundles received */
    bool BundleSetBehaviorAcceptedCBRGenerate;  /** \brief Flag indicating that CBR should be generated for bundles accepted to custody */
    bool BundleSetBehaviorForwardedCBRGenerate; /** \brief Flag indicating that CBR should be generated for bundles forwarded */
    bool BundleSetBehaviorDeliveredCBRGenerate; /** \brief Flag indicating that CBR should be generated for bundles delivered */
    bool BundleSetBehaviorDeletedCBRGenerate;   /** \brief Flag indicating that CBR should be generated for bundles deleted */
    uint16_t Spare1;
    uint32_t ParamSetMaxLifetime;               /** \brief Maximum bundle lifetime */
    uint32_t ParamSetMaxBSRGenerationRate;      /** \brief Maximum number of BSRs per minute that a node can generate overall and on behalf of each source */
    uint32_t ParamSetMaxCBRGenerationRate;      /** \brief Maximum number of CBRs per minute that a node can generate overall and on behalf of each source */
} BPLib_SourceMibConfigSet_t;

typedef struct
{
    BPLib_SourceMibConfigSet_t SourceConfigs[BPLIB_MAX_NUM_MIB_SETS];
    uint32_t                   Spare2;
    uint32_t                   TimeBootEra;       /** \brief Boot Era for Monotonic Time */
    int64_t                    MonotonicTime;     /** \brief Monotonic Time Counter */
    int64_t                    CorrelationFactor; /** \brief Time Correlation Factor */
} BPLib_SourceMibConfigHkTlm_Payload_t;

#endif // BPLIB_NC_PAYLOADS_H