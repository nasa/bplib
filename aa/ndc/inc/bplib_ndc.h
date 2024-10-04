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

#ifndef BPLIB_NDC_H
#define BPLIB_NDC_H

/*
** Include
*/

#include "bplib_api_types.h"


/**
 * \brief Channel/contact status housekeeping payload
 */

typedef struct BPLib_ChannelContactStatHkTlm_Payload BPLib_ChannelContactStatHkTlm_Payload_t;
typedef struct BPLib_NodeMibConfigHkTlm_Payload BPLib_NodeMibConfigHkTlm_Payload_t;
typedef struct BPLib_SourceMibConfigHkTlm_Payload BPLib_SourceMibConfigHkTlm_Payload_t;

typedef struct BPLib_ChannelHkTlmPayloadSet
{
    uint32_t    LocalServiceNum;    /**< \brief Service number for application sending/receiving ADUs on this channel */
    uint32_t    Status;             /**< \brief Configured, Running, or Off */
    uint32_t    RegistrationState;  /**< \brief Active, PassiveDefered or PassiveAbandon */
    uint32_t    OutputQueueID;      /**< \brief Output queue ID */
}BPLib_ChannelHkTlmPayloadSet_t;

typedef struct BPLib_ContactHkTlmPayloadSet
{
    uint32_t    ContactID;                              /**< \brief Unique ID for this contact */
    uint32_t    Status;                                 /**< \brief Configured, Running, or Off */
    char        EIDs[BPLIB_MAX_EID_LIST_STRING];        /**< \brief List of EIDs */
    uint32_t    OutputQueueID;                          /**< \brief Output queue ID */
    uint32_t    Spare1;
}BPLib_ContactHkTlmPayloadSet_t;

struct BPLib_ChannelContactStatHkTlm_Payload
{
    BPLib_ChannelHkTlmPayloadSet_t ChannelStatus[BPLIB_MAX_NUM_CHANNELS]; /**< \brief Status for each channel */
    BPLib_ContactHkTlmPayloadSet_t ContactStatus[BPLIB_MAX_NUM_CONTACTS]; /**< \brief Status for each contact */
    
    uint32_t Spare2;
    uint32_t TimeBootEra;                   /**< \brief Boot Era for Monotonic Time */
    int64_t  MonotonicTime;                 /**< \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;             /**< \brief Time Correlation Factor */
};

/**
 * \brief Node MIB configuration housekeeping payload
 */
struct BPLib_NodeMibConfigHkTlm_Payload
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
    uint8_t Spare1;
    uint32_t BundleSizeFragment;                                /**< \brief Maximum size of bundles that can traverse DTN without additional bundle layer fragmentation */
    
    uint32_t TimeBootEra;                   /**< \brief Boot Era for Monotonic Time */
    int64_t  MonotonicTime;                 /**< \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;             /**< \brief Time Correlation Factor */
};

/**
 * \brief Source MIB configuration housekeeping payload
 */

typedef struct SourceMibConfigSet
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
}SourceMibConfigSet_t;

struct BPLib_SourceMibConfigHkTlm_Payload
{
    SourceMibConfigSet_t SourceConfigs[BPLIB_MAX_NUM_SOURCE_EID];
    uint32_t Spare2;
    uint32_t TimeBootEra;                   /**< \brief Boot Era for Monotonic Time */
    int64_t  MonotonicTime;                 /**< \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;             /**< \brief Time Correlation Factor */        
};

/*
** Exported Functions
*/

/**
 * \brief Node Configuration initialization
 *
 *  \par Description
 *       NDC initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
int BPLib_NDC_Init(void);

#endif /* BPLIB_NDC_H */
