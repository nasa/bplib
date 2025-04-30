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

#ifndef BPLIB_AS_H
#define BPLIB_AS_H

/* ======== */
/* Includes */
/* ======== */

#include <string.h>
#include "bplib_api_types.h"
#include "bplib_cfg.h"
#include "bplib_nc_payloads.h"
#include "bplib_eventids.h"
#include "bplib_em.h"
#include "bplib_eid.h"

/* ====== */
/* Macros */
/* ====== */

#define BPLIB_AS_NUM_NODE_CNTRS      (79u)                    /** \brief Number of node counters (also total number of counters) */
#define BPLIB_AS_NUM_SOURCE_CNTRS    (56u)                    /** \brief Number of source counters */
#define BPLIB_AS_NODE_CNTR_INDICATOR (BPLIB_MAX_NUM_MIB_SETS) /** \brief Indicates that only the node counter passed in should be modified, not the source counter */

/* ======= */
/* Typdefs */
/* ======= */

/**
  * \brief  Used to as indices into the counter arrays in the node and source payloads
  * \anchor BPLib_AS_Counter_t
  */
typedef enum
{
    /* Common (node and source shared) counters */
    ADU_COUNT_DELIVERED                    = 0,  /** \brief Number of ADUs Delivered to the application */
    ADU_COUNT_RECEIVED                     = 1,  /** \brief Number of ADUs Received from the application */
    BUNDLE_COUNT_ABANDONED                 = 2,  /** \brief Number of Abandoned Bundle Payloads */
    BUNDLE_COUNT_CUSTODY_REJECTED          = 3,  /** \brief Number of unsuccessful Custody Transfers from the Local node to the next neighboring Custodian Node */
    BUNDLE_COUNT_CUSTODY_REQUEST           = 4,  /** \brief Number of bundles received that are requesting Custody Transfer */
    BUNDLE_COUNT_CUSTODY_RE_FORWARDED      = 5,  /** \brief Number of Bundles reforward due to custody timeout */
    BUNDLE_COUNT_CUSTODY_TRANSFERRED       = 6,  /** \brief Number of successful Custody Transfers from the Local node to the next neighboring Custodian Node */
    BUNDLE_COUNT_DELETED                   = 7,  /** \brief Total Number of Bundle Deletions */
    BUNDLE_COUNT_DELETED_BAD_EID           = 8,  /** \brief Number of Bundles deleted due to having a unrecognized destination EID */
    BUNDLE_COUNT_DELETED_CANCELLED         = 9,  /** \brief Number of Bundles Deletions due to Transmission Cancelled Condition */
    BUNDLE_COUNT_DELETED_EXPIRED           = 10, /** \brief Number of Bundles Deletions due to Lifetime Expired Condition */
    BUNDLE_COUNT_DELETED_FORWARD_FAILED    = 11, /** \brief Number of Bundles Deletions due to Forwarding Failed Condition */
    BUNDLE_COUNT_DELETED_HOP_EXCEEDED      = 12, /** \brief Number of Bundles Deletions due to Hop Limit Exceeded Condition */
    BUNDLE_COUNT_DELETED_INVALID_PAYLOAD   = 13, /** \brief Number of Bundle Deletions due having a Corrupted Payload Block */
    BUNDLE_COUNT_DELETED_NO_STORAGE        = 14, /** \brief Number of Bundles deleted due to insufficient storage */
    BUNDLE_COUNT_DELETED_TOO_LONG          = 15, /** \brief Number of Bundles deleted due to being longer than paramSetMaxBundleLength */
    BUNDLE_COUNT_DELETED_TRAFFIC_PARED     = 16, /** \brief Number of Bundles Deletions due to Traffic Pared Condition */
    BUNDLE_COUNT_DELETED_UNAUTHORIZED      = 17, /** \brief Number of Bundles deleted due to having a unrecognized source EID. Incremented if the bundle is not in the set of authorized source EIDs configured for the node. */
    BUNDLE_COUNT_DELETED_UNINTELLIGIBLE    = 18, /** \brief Number of Bundles Deletions due to Block Unintelligible Condition */
    BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK = 19, /** \brief Number of Bundles Deletions due to Unsupported Block Condition */
    BUNDLE_COUNT_DELIVERED                 = 20, /** \brief Total number of Bundles Delivered to this node, including fragments */
    BUNDLE_COUNT_DEPLETED                  = 21, /** \brief Number of bundles for which rejected Custody Signals generated indicating rejection due to depleted storage */
    BUNDLE_COUNT_DISCARDED                 = 22, /** \brief Number of Bundles Discarded */
    BUNDLE_COUNT_FORWARDED                 = 23, /** \brief Number of Bundles Forwarded to another DTN Node */
    BUNDLE_COUNT_FORWARDED_FAILED          = 24, /** \brief Number of Bundles where forwarding to another DTN Node failed */
    BUNDLE_COUNT_FRAGMENTED                = 25, /** \brief Number of Bundles that needed to be Fragmented  */
    BUNDLE_COUNT_FRAGMENT_ERROR            = 26, /** \brief Number of Fragments discarded due to bad offset or ADU length */
    BUNDLE_COUNT_GENERATED_ACCEPTED        = 27, /** \brief Number of Accepted Bundle Transmission Requests  */
    BUNDLE_COUNT_GENERATED_CUSTODY_SIGNAL  = 28, /** \brief Number of bundles for which Custody Signals Generated */
    BUNDLE_COUNT_GENERATED_FRAGMENT        = 29, /** \brief Number of Bundle Fragments that were Generated */
    BUNDLE_COUNT_GENERATED_REJECTED        = 30, /** \brief Number of Rejected Bundle Transmission Requests */
    BUNDLE_COUNT_MAX_BSR_RATE_EXCEEDED     = 31, /** \brief Number of BSR bundles not sent because sending would exceed a maximum rate. */
    BUNDLE_COUNT_NO_CONTACT                = 32, /** \brief Number of bundles for which rejected Custody Signals generated indicating the Destination is not reachable before the Bundle expires */
    BUNDLE_COUNT_NO_FURTHER_INFO           = 33, /** \brief Number of bundles for which successful Custody Signals generated with No Further Information */
    BUNDLE_COUNT_NO_ROUTE                  = 34, /** \brief Number of bundles for which rejected Custody Signals generated indicating the Destination is not reachable */
    BUNDLE_COUNT_REASSEMBLED               = 35, /** \brief Total number of Bundles delivered that were fragments and needed to be reassembled */
    BUNDLE_COUNT_RECEIVED                  = 36, /** \brief Number of Bundles Received from another DTN Node */
    BUNDLE_COUNT_RECEIVED_ADMIN_RECORD     = 37, /** \brief Number of admin record bundles received for this DTN Node. */
    BUNDLE_COUNT_RECEIVED_BSR_ACCEPTED     = 38, /** \brief Number of Bundle Custody Accepted Status Report received since the last counter reset */
    BUNDLE_COUNT_RECEIVED_BSR_DELETED      = 39, /** \brief Number of Bundle Deleted Status Report received since the last counter reset */
    BUNDLE_COUNT_RECEIVED_BSR_DELIVERED    = 40, /** \brief Number of Bundle Delivered Status Report received since the last counter reset */
    BUNDLE_COUNT_RECEIVED_BSR_FORWARDED    = 41, /** \brief Number of Bundle Forwarded Status Report received since the last counter reset */
    BUNDLE_COUNT_RECEIVED_BSR_RECEIVED     = 42, /** \brief Number of Bundle Reception Status Report received since the last counter reset */
    BUNDLE_COUNT_RECEIVED_CRS_ACCEPTED     = 43, /** \brief Number of accepted bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of accepted bundles per source node ID. */
    BUNDLE_COUNT_RECEIVED_CRS_DELETED      = 44, /** \brief Number of deleted bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of deleted bundles per source node ID. */
    BUNDLE_COUNT_RECEIVED_CRS_DELIVERED    = 45, /** \brief Number of delivered bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of delivered bundles per source node ID. */
    BUNDLE_COUNT_RECEIVED_CRS_FORWARDED    = 46, /** \brief Number of forwarded bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of forwarded bundles per source node ID. */
    BUNDLE_COUNT_RECEIVED_CRS_RECEIVED     = 47, /** \brief Number of received bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of received bundles per source node ID. */
    BUNDLE_COUNT_RECEIVED_CUSTODY_SIGNAL   = 48, /** \brief Number of bundles for which Custody Signals Received */
    BUNDLE_COUNT_RECEIVED_FRAGMENT         = 49, /** \brief Number of Bundles Received that were Marked as Fragments */
    BUNDLE_COUNT_REDUNDANT                 = 50, /** \brief Number of bundles for which successful Custody Signals generated for Duplicate Bundle reception */
    BUNDLE_COUNT_REJECTED_CUSTODY          = 51, /** \brief Number of Bundles where this node rejected custody. */
    BUNDLE_COUNT_RETURNED                  = 52, /** \brief Number of Bundles Returned to Sender */
    BUNDLE_COUNT_UNINTELLIGIBLE_BLOCK      = 53, /** \brief Number of bundles for which Custody Signals indicating the Bundle contained an unknown block type */
    BUNDLE_COUNT_UNINTELLIGIBLE_EID        = 54, /** \brief Number of bundles for which rejected Custody Signals generated indicating the any EID in the Primary Header is unknown */
    BUNDLE_COUNT_UNPROCESSED_BLOCKS        = 55, /** \brief Number of Unprocessed Blocks Removed from Received Bundles */

    /* Node-only counters */
    BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT  = 56, /** \brief Number of control directives received from the Monitor and Control interface that have been accepted */
    BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT  = 57, /** \brief Number of control directives received from the Monitor and Control interface that have been rejected as being invalid */
    BUNDLE_COUNT_CUSTODY_SIGNAL_RECEIVED   = 58, /** \brief Number of Custody Signal Bundles received */
    BUNDLE_COUNT_GENERATED_ANONYMOUS       = 59, /** \brief Number of Anonomous Bundles Created */
    BUNDLE_COUNT_GENERATED_BSR_ACCEPTED    = 60, /** \brief Number of Bundle Custody Accepted Status Report generated since the last counter reset */
    BUNDLE_COUNT_GENERATED_BSR_DELETED     = 61, /** \brief Number of Bundle Deleted Status Report generated since the last counter reset */
    BUNDLE_COUNT_GENERATED_BSR_DELIVERED   = 62, /** \brief Number of Bundle Delivered Status Report generated since the last counter reset */
    BUNDLE_COUNT_GENERATED_BSR_FORWARDED   = 63, /** \brief Number of Bundle Forwarded Status Report generated since the last counter reset */
    BUNDLE_COUNT_GENERATED_BSR_RECEIVED    = 64, /** \brief Number of Bundle Reception Status Report generated since the last counter reset */
    BUNDLE_COUNT_GENERATED_CRS             = 65, /** \brief Number of Compressed Reporting Signal (CRS) generated since last counter reset. */
    BUNDLE_COUNT_GENERATED_CRS_ACCEPTED    = 66, /** \brief Number of accepted bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    BUNDLE_COUNT_GENERATED_CRS_DELETED     = 67, /** \brief Number of deleted bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    BUNDLE_COUNT_GENERATED_CRS_DELIVERED   = 68, /** \brief Number of delivered bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    BUNDLE_COUNT_GENERATED_CRS_FORWARDED   = 69, /** \brief Number of forwarded bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    BUNDLE_COUNT_GENERATED_CRS_RECEIVED    = 70, /** \brief Number of received bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    BUNDLE_COUNT_GENERATED_CUSTODY         = 71, /** \brief Number of Custody Signal Bundles generated since the last counter reset */
    BUNDLE_COUNT_INVALID_PRIMARY_BLOCK     = 72, /** \brief Number of Unprocessed Bundles received with Invalid Primary Blocks */
    BUNDLE_COUNT_IN_CUSTODY                = 73, /** \brief  */
    BUNDLE_COUNT_MAX_CRS_RATE_EXCEEDED     = 74, /** \brief Number of CRS bundles not sent because sending would exceed a maximum rate. */
    BUNDLE_COUNT_RECEIVED_CRS              = 75, /** \brief Number of Compressed Reporting Signals (CRSs) received since last counter reset. */
    BUNDLE_COUNT_STORAGE_AVAILABLE         = 76, /** \brief  */
    BUNDLE_COUNT_STORED                    = 77, /** \brief  */
    NODE_STARTUP_COUNTER                   = 78, /** \brief Number of times a node is started up. */
} BPLib_AS_Counter_t;

/**
 * \brief  Node MIB counters housekeeping payload
 * \anchor BPLib_NodeMibCountersHkTlm_Payload_t
 */
typedef struct
{
    /**
      * \brief Array of all node counters
      * \note  See BPLib_AS_Counter_t for counter details
      * \ref   BPLib_AS_Counter_t
      */
    uint32_t NodeCounters[BPLIB_AS_NUM_NODE_CNTRS];

    uint32_t Spare;
    int64_t  MonotonicTime;     /** \brief Monotonic Time Counter */
    int64_t  CorrelationFactor; /** \brief Time Correlation Factor */
} BPLib_NodeMibCountersHkTlm_Payload_t;

/**
  * \brief  Source MIB counters housekeeping payload
  * \anchor BPLib_SourceMibCounters_t
  */
typedef struct
{
    /**
      * \brief Source EID patters this telemetry corresponds to
      * \ref BPLib_EID_Pattern_t
      */
    BPLib_EID_Pattern_t EidPatterns[BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET];

    uint8_t ActiveKeys; /** \brief Indicates how many keys are active in EidPatterns */

    uint8_t Spare1[3]; /* Spare for alignment */

    /**
      * \brief Array of all source counters
      * \ref BPLib_AS_Counter_t
      */
    uint32_t SourceCounters[BPLIB_AS_NUM_SOURCE_CNTRS];

    uint8_t Spare2[4]; /* Spare for alignment */
} BPLib_SourceMibCounters_t;

/**
  * \brief  Packet for all source counters accessed via an index into MibArray
  * \anchor BPLib_SourceMibCountersHkTlm_Payload_t
  */
typedef struct
{
    BPLib_SourceMibCounters_t MibArray[BPLIB_MAX_NUM_MIB_SETS]; /** \brief Counters for each source */

    int64_t  MonotonicTime;                 /** \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;             /** \brief Time Correlation Factor */
} BPLib_SourceMibCountersHkTlm_Payload_t;

/*
** Node MIB Reports Payload
*/
typedef struct
{
    char     SystemNodeName[BPLIB_MAX_STR_LENGTH];              /** \brief Human readable name given to entity */
    char     SystemNodeOwner[BPLIB_MAX_STR_LENGTH];             /** \brief Identifies primary manager of the node */
    char     SystemSoftwareExec[BPLIB_MAX_STR_LENGTH];          /** \brief ID of the OS or executive controlling the resources */
    char     SystemSoftwareExecVersion[BPLIB_MAX_STR_LENGTH];   /** \brief Version of software */

    char     BundleAgentSoftwareVersion[BPLIB_MAX_STR_LENGTH];  /** \brief Version of the Bundle Protocol Agent */
    char     BundleAgentOperationalState[BPLIB_MAX_STR_LENGTH]; /** \brief Operational state of Bundle Protocol Agent */
    char     BundleAgentConfiguration[BPLIB_MAX_STR_LENGTH];    /** \brief Indication of the BPA configuration */
    char     ParamSupportedCLAs[BPLIB_MAX_STR_LENGTH];          /** \brief Supported CLAs */
    char     NodeActiveEndpoints[BPLIB_MAX_STR_LENGTH];         /** \brief List of active endpoints on the Node */

    uint32_t SystemNodeUpTime;                                  /** \brief The time in seconds since this node has been reinitialized */
    uint32_t BundleAgentAvailableStorage;                       /** \brief Total amount of memory allocated for bundle storage for the node  */
    uint32_t KbytesCountStorageAvailable;                       /** \brief Kilobytes free space left to store additional Bundles */

    uint32_t BundleIngressRateBytesPerSec;                      /** \brief Rate of bundles received from CLAs in bytes per second */
    uint32_t BundleIngressRateBundlesPerSec;                    /** \brief Rate of bundles received from CLAs in bundles per second */
    uint32_t BundleEgressRateBytesPerSec;                       /** \brief Rate of bundles forwarded from CLAs in bytes per second */
    uint32_t BundleEgressRateBundlesPerSec;                     /** \brief Rate of bundle forwarded from CLAs in bundles per second */
    uint32_t BundleIngestedRateBundlesPerSec;                   /** \brief Rate of bundles ingested locally in bundles per second */
    uint32_t BundleIngestedRateBytesPerSec;                     /** \brief Rate of bundles ingested locally in bytes per second */
    uint32_t BundleDeliveryRateBundlesPerSec;                   /** \brief Rate of bundles delivered locally in bundles per second */
    uint32_t BundleDeliveryRateBytesPerSec;                     /** \brief Rate of bundles delivered locally in bytes per second */
    uint32_t BundleIngressRejectedRateBytesPerSec;              /** \brief Rate of bundles received from CLAs in bytes per second and then rejected */
    uint32_t BundleIngressRejectedRateBundlesPerSec;            /** \brief Rate of bundles received from CLAs in bundles per second and then rejected */

    uint32_t Spare;
    int64_t  MonotonicTime;                                     /** \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;                                 /** \brief Time Correlation Factor */

} BPLib_NodeMibReportsHkTlm_Payload_t;

/* =================== */
/* Function Prototypes */
/* =================== */

/**
 * \brief     Instantiate a mutex to guard access to counters and initialize counter payloads
 * \details   Admin Statistics initialization
 * \note      Only returns BPLIB_SUCCESS for now
 * \param[in] void No arguments accepted
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Initialization was successful
 */
BPLib_Status_t BPLib_AS_Init(void);

/**
 * \brief     Add an amount to the counter specified by the given EID and counter
 * \details   Incrementing function for counters used by Admin Statistics
 * \note      Amount must be positive
 * \param[in] EID (BPLib_EID_t) EID whose associated counter should be incremented
 * \param[in] Counter (BPLib_AS_Counter_t) Counter to increment
 * \param[in] Amount (uint32_t) Positive integer to increment Counter by
 * \return    void
 * \ref       BPLib_SourceMibCountersHkTlm_Payload_t
 * \ref       BPLib_AS_Counter_t
 * \ref       BPLib_AS_SetCounter [BPLib_AS_SetCounter()]
 * \ref       BPLib_EID_t
 * \ref       BPLib_AS_LockCounters [BPLib_AS_LockCounters]
 * \ref       BPLib_AS_UnlockCounters [BPLib_AS_UnlockCounters()]
 */
void BPLib_AS_Increment(BPLib_EID_t EID, BPLib_AS_Counter_t Counter, uint32_t Amount);

/**
 * \brief     Subtract an amount to the counter specified by the given EID and counter
 * \details   Decrementing function for counters used by Admin Statistics
 * \note      Amount must be positive
 * \param[in] EID (BPLib_EID_t) EID whose associated counter should be decremented
 * \param[in] Counter (BPLib_AS_Counter_t) Counter to decrement
 * \param[in] Amount (uint32_t) Positive integer to decrement Counter by
 * \return    void
 * \ref       BPLib_SourceMibCountersHkTlm_Payload_t
 * \ref       BPLib_AS_Counter_t
 * \ref       BPLib_AS_SetCounter [BPLib_AS_SetCounter()]
 * \ref       BPLib_EID_t
 * \ref       BPLib_AS_LockCounters [BPLib_AS_LockCounters]
 * \ref       BPLib_AS_UnlockCounters [BPLib_AS_UnlockCounters()]
 */
void BPLib_AS_Decrement(BPLib_EID_t EID, BPLib_AS_Counter_t Counter, uint32_t Amount);

/**
 * \brief     Set to zero to counter associated with the given MIB counter array index
 * \note      Use BPLIB_AS_NODE_CNTR_INDICATOR if the counter to be reset is a node counter
 *
 *            The index into the source MIB counter array must be known ahead of time. This can
 *            be determined by looking at the source EIDs inside the MIB array, nested within the
 *            source MIB counters payload of the source MIB counters HK packet
 * \param[in] MibArrayIndex (uint16_t) Index into BPLib_SourceMibCountersHkTlm_Payload_t::MibArray
 * \param[in] Counter       (BPLib_AS_Counter_t) Counter to reset
 * \return    Execution status
 * \retval    BPLIB_AS_UNKNOWN_NODE_CNTR: Counter is outside node counters' range
 * \retval    BPLIB_AS_UNKNOWN_SRC_CNTR: Counter is outside source counters' range
 * \retval    BPLIB_AS_INVALID_MIB_INDEX: Given MIB counter array index is out of range
 * \retval    BPLIB_SUCCESS: Successful execution
 * \ref       BPLib_AS_Counter_t
 * \ref       BPLib_AS_LockCounters [BPLib_AS_LockCounters]
 * \ref       BPLib_AS_UnlockCounters [BPLib_AS_UnlockCounters()]
 */
BPLib_Status_t BPLib_AS_ResetCounter(uint16_t MibArrayIndex, BPLib_AS_Counter_t Counter);

/**
 * \brief     Set to zero all resettable MIB counters associated with the given index into the MIB
 *            counter array
 * \details   See function body and reference the BPLib_AS_Counter_t struct to see which counters are reset
 * \note      The index into the source MIB counter array must be known ahead of time. This can
 *            be determined by looking at the source EIDs inside the MIB array, nested within the
 *            source MIB counters payload of the source MIB counters HK packet
 * \param[in] MibArrayIndex (uint16_t) Index into BPLib_SourceMibCountersHkTlm_Payload_t::MibArray
 * \return    Execution status
 * \retval    BPLIB_AS_INVALID_MIB_INDEX: Given MIB counter array index is out of range
 * \retval    BPLIB_SUCCESS: Successful execution
 * \ref       BPLib_SourceMibCountersHkTlm_Payload_t
 * \ref       BPLib_AS_Counter_t
 * \ref       BPLib_AS_LockCounters [BPLib_AS_LockCounters()]
 * \ref       BPLib_AS_UnlockCounters [BPLib_AS_UnlockCounters()]
 */
BPLib_Status_t BPLib_AS_ResetSourceCounters(uint16_t MibArrayIndex);

/**
 * \brief     Set to zero all resettable node MIB counters associated with bundles
 * \details   See function body and reference the BPLib_AS_Counter_t struct to see which counters are reset
 * \note      Diretcly sets node bundle counters in payloads to 0
 * \return    void
 * \ref       BPLib_AS_Counter_t
 * \ref       BPLib_NodeMibCountersHkTlm_Payload_t
 * \ref       BPLib_AS_LockCounters [BPLib_AS_LockCounters()]
 * \ref       BPLib_AS_UnlockCounters [BPLib_AS_UnlockCounters()]
 */
void BPLib_AS_ResetBundleCounters(void);

/**
 * \brief     Set to zero all resettable MIB error counters
 * \details   See function body and reference the BPLib_AS_Counter_t struct to see which counters are reset
 * \note      Directly sets error counters in node and source MIB counter payloads to 0
 * \param[in] MibArrayIndex (uint16_t) Index into BPLib_SourceMibCountersHkTlm_Payload_t::MibArray
 * \return    Execution status
 * \retval    BPLIB_AS_INVALID_MIB_INDEX: Index into source MIB counters array is out of range
 * \retval    BPLIB_SUCCESS: Successful execution
 * \ref       BPLib_AS_Counter_t
 * \ref       BPLib_NodeMibCountersHkTlm_Payload_t
 * \ref       BPLib_SourceMibCountersHkTlm_Payload_t
 * \ref       BPLib_AS_LockCounters [BPLib_AS_LockCounters()]
 * \ref       BPLib_AS_UnlockCounters [BPLib_AS_UnlockCounters()]
 */
BPLib_Status_t BPLib_AS_ResetErrorCounters(uint16_t MibArrayIndex);

/**
 * \brief     Set every counter value in the source and node counter packets to zero
 * \details   Zeroing out function used by Admin Statistics
 * \note      Directly sets all counters in payloads to 0
 * \param[in] void No arguments accepted
 * \return    void
 * \ref       BPLib_AS_LockCounters [BPLib_AS_LockCounters()]
 * \ref       BPLib_AS_UnlockCounters [BPLib_AS_UnlockCounters()]
 */
void BPLib_AS_ResetAllCounters(void);

/**
  * \brief     Send Per Node MIB Counter telemetry packet
  * \details   Node Configuration Send Node MIB Counters Housekeeping Packet command.
  * \note      This command is just a call to BPA_TLMP_SendNodeMibCounterPkt()
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \return    Return codes from BPA_TLMP_SendNodeMibCounterPkt() in fwp_tlmp.h
  * \ref       BPLib_AS_LockCounters [BPLib_AS_LockCounters()]
  * \ref       BPLib_AS_UnlockCounters [BPLib_AS_UnlockCounters()]
  */
BPLib_Status_t BPLib_AS_SendNodeMibCountersHk(void);

/**
  * \brief     Send Per Source MIB Counter telemetry packet
  * \details   Node Configuration Send Source MIB Counters Housekeeping Packet command.
  * \note      This command is just a call to BPA_TLMP_SendPerSourceMibCounterPkt()
  * \param[in] void No arguments accepted
  * \return    Execution status
  * \return    Return codes from BPA_TLMP_SendPerSourceMibCounterPkt() in fwp_tlmp.h
  * \ref       BPLib_AS_LockCounters [BPLib_AS_LockCounters()]
  * \ref       BPLib_AS_UnlockCounters [BPLib_AS_UnlockCounters()]
  */
BPLib_Status_t BPLib_AS_SendSourceMibCountersHk(void);

BPLib_Status_t BPLib_AS_SendNodeMibReportsHk(void);

/**
 * \note The terms key(s) and pattern(s) are used interchangeably since the MIB array keys are patterns of EIDs associated with the counters for that entry
 */
BPLib_Status_t BPLib_AS_AddMibArrayKey(const BPLib_EID_Pattern_t* EID_Patterns);

#endif /* BPLIB_AS_H */
