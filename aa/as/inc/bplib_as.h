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

/*
** Include
*/

#include "bplib_api_types.h"
#include "bplib_cfg.h"

/**
 * \brief Node MIB counters housekeeping payload
 */

typedef struct BPLib_NodeMibCountersHkTlm_Payload BPLib_NodeMibCountersHkTlm_Payload_t;
typedef struct BPLib_SourceMibCountersHkTlm_Payload BPLib_SourceMibCountersHkTlm_Payload_t;

struct BPLib_NodeMibCountersHkTlm_Payload
{
    uint32_t BundleCountGeneratedAnonymous;         /**< \brief Bundle count generated anonymous */
    uint32_t SystemNodeUpTime;                      /**< \brief System node up time */
    uint32_t AcceptedDirectiveCount;                /**< \brief Bundle Agent Accepted Directive Count */
    uint32_t RejectedDirectiveCount;                /**< \brief Bundle Agent Rejected Directive Count */
    uint32_t BundleCountGeneratedCustody;           /**< \brief Bundle Number of custody signal generated */
    uint32_t BundleCountGeneratedBsrReceived;       /**< \brief Bundle Received BSR Counts */
    uint32_t BundleCountGeneratedBsrAccepted;       /**< \brief Bundle Accepted BSR Counts */
    uint32_t BundleCountGeneratedBsrForwarded;      /**< \brief Bundle Forwarded BSR Counts */
    uint32_t BundleCountGeneratedBsrDelivered;      /**< \brief Bundle Delivered BSR Counts */
    uint32_t BundleCountGeneratedBsrDeleted;        /**< \brief Bundle Deleted BSR Counts */
    uint32_t BundleCountInvalidPrimaryBlock;        /**< \brief Bundle unprocessed due to Invalid primary block Counts */
    uint32_t BundleCountCustodySignalReceived;      /**< \brief Received custody signal Bundle Counts */
    uint32_t BundleCountDeletedUnauthorizedSrc;     /**< \brief Deleted bundle Counts for unauthorized Src EID*/
    uint32_t BundleCountGeneratedCrsReceived;       /**< \brief Bundle Received CRS Counts */
    uint32_t BundleCountGeneratedCrsAccepted;       /**< \brief Bundle Accepted CRS Counts */
    uint32_t BundleCountGeneratedCrsForwarded;      /**< \brief Bundle Forwarded CRS Counts */
    uint32_t BundleCountGeneratedCrsDelivered;      /**< \brief Bundle Delivered CRS Counts */
    uint32_t BundleCountGeneratedCrsDeleted;        /**< \brief Bundle Deleted CRS Counts */
    uint32_t BundleCountMaxCrsRateExceeded;         /**< \brief Number of CRS bundles not sent to avoid exceeding max rate */
    uint32_t NodeStartupCounter;                    /**< \brief Number of times the node has been started up */
    uint32_t BundleCountGeneratedCrs;               /**< \brief Number of CRSs generated */
    uint32_t BundleCountReceivedCrs;                /**< \brief Number of CRSs received */

    uint32_t AduCountDelivered;                     /**< \brief ADU Delivered Count */
    uint32_t AduCountReceived;                      /**< \brief ADU Received Count */
    
    uint32_t Spare1;        
    uint32_t TimeBootEra;                   /**< \brief Boot Era for Monotonic Time */
    int64_t  MonotonicTime;                 /**< \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;             /**< \brief Time Correlation Factor */    
};

/**
 * \brief Source MIB counters housekeeping payload
 */
typedef struct BPLib_SourceMibCountersSet
{    
    char SourceEID[BPLIB_MAX_EID_LENGTH];       /**< \brief Source EID this telemetry corresponds to */
    uint32_t BundleCountGeneratedAccepted;      /**< \brief Number of accepted bundle transmission requests */
    uint32_t BundleCountGeneratedRejected;      /**< \brief Number of rejected bundle transmission requests */
    uint32_t BundleCountMaxBsrRateExceeded;     /**< \brief Number of BSR bundles not sent because sending would exceed a maximum rate */
    uint32_t BundleCountGeneratedFragment;      /**< \brief Number of generated bundle fragments */
    uint32_t BundleCountReceived;               /**< \brief Number of bundles received from another node */
    uint32_t BundleCountReceivedFragment;       /**< \brief Number of bundles received that were marked as fragments. */
    uint32_t BundleCountUnprocessedBlocks;      /**< \brief Number of unprocessed blocks removed from received bundles. */
    uint32_t BundleCountForwarded;              /**< \brief Number of bundles forwarded to another DTN node. */
    uint32_t BundleCountReturned;               /**< \brief Number of bundles returned to sender. */
    uint32_t BundleCountFragmented;             /**< \brief Number of bundles that needed to be fragmented. */   
    uint32_t BundleCountReassembled;            /**< \brief Number of bundles reassembled from fragments. */
    uint32_t BundleCountFragmentError;          /**< \brief Number of fragments discarded due to bad offset or ADU length. */
    uint32_t BundleCountDelivered;              /**< \brief Number of bundles destined for this node. */
    uint32_t BundleCountAbandoned;              /**< \brief Number of abandoned bundle payloads. */
    uint32_t AduCountDelivered;                 /**< \brief Number of ADUs delivered to the application. */
    uint32_t BundleCountStored;                 /**< \brief Number of bundles currently in storage. */
    uint32_t KbytesCountStorageAvailable;       /**< \brief Kilobytes of free space left to store additional bundles. */
    uint32_t BundleCountDeleted;                /**< \brief Total number of bundle deletions. */
    uint32_t BundleCountDeletedExpired;         /**< \brief Number of bundle deletions due to expired lifetime condition. */
    uint32_t BundleCountDeletedHopExceeded;     /**< \brief Number of bundle deletions due to exceeded hop limit condition. */
    uint32_t BundleCountDeletedInvalidPayload;  /**< \brief Number of bundle deletions due to having a corrupted payload block. */
    uint32_t BundleCountDeletedForwardFailed;   /**< \brief Number of bundle deletions due to Forwarding Failed condition. */
    uint32_t BundleCountDeletedTrafficPared;    /**< \brief Number of bundle deletions due to Traffic Pared Condition. */
    uint32_t BundleCountDeletedCancelled;       /**< \brief Number of bundle deletions due to Transmission Cancelled Condition. */
    uint32_t BundleCountDeletedUnintelligible;  /**< \brief Number of bundle deletions due to Block Unintelligible Condition. */
    uint32_t BundleCountDeletedUnsupportedBlock;/**< \brief Number of bundle deletions due to Unsupported Block Condition. */
    uint32_t BundleCountDiscarded;              /**< \brief Number of discarded bundles. */
    uint32_t BundleCountDeletedNoStorage;       /**< \brief Number of bundles deleted due to insufficient storage. */
    uint32_t BundleCountDeletedBadEid;          /**< \brief Number of bundles deleted due to an unrecognized destination EID. */
    uint32_t BundleCountDeletedtooLong;         /**< \brief Number of bundles deleted due to being longer than paramSetMaxBundleLength. */
    uint32_t BundleCountCustodyTransferred;     /**< \brief Number of successful Custody Transfers from the local node to the next neighboring custodian node. */
    uint32_t BundleCountCustodyRejected;        /**< \brief Number of unsuccessful Custody Transfers from the local node to the next neighboring custodian node. */
    uint32_t BundleCountCustodyRequest;         /**< \brief Number of bundles received that are requesting Custody Transfer. */
    uint32_t BundleCountInCustody;              /**< \brief Number of bundles currently in custody at this node. */
    uint32_t BundleCountCustodyReForwarded;     /**< \brief Number of bundles reforward due to custody timeout. */
    uint32_t BundleCountNoFurtherInfo;          /**< \brief Number of bundles for which successful Custody Signals generated with No Further Information. */
    uint32_t BundleCountRedundant;              /**< \brief Number of bundles for which successful Custody Signals were generated for duplicate bundle reception. */
    uint32_t BundleCountDepleted;               /**< \brief Number of bundles for which rejected Custody Signals were generated indicating rejection due to depleted storage. */
    uint32_t BundleCountUnintelligibleEid;      /**< \brief Number of bundles for which rejected Custody Signals were generated indicating the any EID in the Primary Header is unknown. */
    uint32_t BundleCountNoRoute;                /**< \brief Number of bundles for which rejected Custody Signals were generated indicating the Destination is not reachable. */
    uint32_t BundleCountNoContact;              /**< \brief Number of bundles for which rejected Custody Signals were generated indicating the Destination is not reachable before the bundle expires. */
    uint32_t BundleCountUnintelligibleBlock;    /**< \brief Number of bundles for which Custody Signals indicated that the bundle contained an unknown block type. */
    uint32_t BundleCountReceivedCustodySignal;  /**< \brief Number of bundles for which Custody Signals were received. */
    uint32_t BundleCountRejectedCustody;        /**< \brief Number of bundles where this node rejected custody. */
    uint32_t AduCountReceived;                  /**< \brief Number of ADUs received from the application. */
    uint32_t BundleIngressRateBytesPerSec;      /**< \brief Rate of bundle ingress in bytes per second. */
    uint32_t BundleIngressRateBundlesPerSec;    /**< \brief Rate of bundle ingress in bundles per second. */
    uint32_t BundleEgressRateBytesPerSec;       /**< \brief Rate of bundle egress in bytes per second. */
    uint32_t BundleEgressRateBundlesPerSec;     /**< \brief Rate of bundle egress in bundles per second. */
    uint32_t BundleCountForwardedFailed;        /**< \brief Number of bundles where forwarding to another DTN node failed. */
    uint32_t BundleCountReceivedBsrReceived;    /**< \brief Number of Bundle Reception Status Reports received since the last counter reset. */
    uint32_t BundleCountReceivedBsrAccepted;    /**< \brief Number of Bundle Custody Accepted Status Reports received since the last counter reset. */
    uint32_t BundleCountReceivedBsrForwarded;   /**< \brief Number of Bundle Forwarded Status Reports received since the last counter reset. */
    uint32_t BundleCountReceivedBsrDelivered;   /**< \brief Number of Bundle Delivered Status Reports received since the last counter reset. */
    uint32_t BundleCountReceivedBsrDeleted;     /**< \brief Number of Bundle Deleted Status Reports received since the last counter reset. */
    uint32_t BundleCountReceivedCrsReceived;    /**< \brief Number of received bundle reports included in each received CRS since the last counter reset. */
    uint32_t BundleCountReceivedCrsAccepted;    /**< \brief Number of accepted bundle reports included in each received CRS since the last counter reset. */
    uint32_t BundleCountReceivedCrsForwarded;   /**< \brief Number of forwarded bundle reports included in each received CRS since the last counter reset. */
    uint32_t BundleCountReceivedCrsDelivered;   /**< \brief Number of delivered bundle reports included in each received CRS since the last counter reset. */
    uint32_t BundleCountReceivedCrsDeleted;     /**< \brief Number of deleted bundle reports included in each received CRS since the last counter reset. */
    uint32_t BundleCountReceivedAdminRecord;    /**< \brief Number of admin record bundles received for this DTN Node. */
    uint32_t Spare1;
}BPLib_SourceMibCountersSet_t;

struct BPLib_SourceMibCountersHkTlm_Payload
{
    BPLib_SourceMibCountersSet_t SourceCounters[BPLIB_MAX_NUM_SOURCE_EID]; /**< \brief Counters for each source */
    
    uint32_t Spare2;
    uint32_t TimeBootEra;                   /**< \brief Boot Era for Monotonic Time */
    int64_t  MonotonicTime;                 /**< \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;             /**< \brief Time Correlation Factor */
};

/*
** Exported Functions
*/

/**
 * \brief Admin Statistics initialization
 *
 *  \par Description
 *       AS initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
int BPLib_AS_Init(void);

#endif /* BPLIB_AS_H */
