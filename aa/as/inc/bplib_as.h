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
#include "bplib_eventids.h"
#include "bplib_nc_payloads.h"

#include "bplib_em.h"
#include "bplib_as_internal.h"

/* ====== */
/* Macros */
/* ====== */

#define BPLIB_AS_NUM_CNTRS      (79u)   /** \brief Total number of MIB counters */

/* ======= */
/* Typdefs */
/* ======= */

/**
 * \brief  Node MIB counters housekeeping payload
 * \anchor BPLib_NodeMibCountersHkTlm_Payload_t
 */
typedef struct
{
    uint32_t AduCountDelivered;                  /** \brief Number of ADUs Delivered to the application */
    uint32_t AduCountReceived;                   /** \brief Number of ADUs Received from the application */
    uint32_t BundleAgentAcceptedDirectiveCount;  /** \brief Number of control directives received from the Monitor and Control interface that have been accepted */
    uint32_t BundleAgentRejectedDirectiveCount;  /** \brief Number of control directives received from the Monitor and Control interface that have been rejected as being invalid */
    uint32_t BundleCountAbandoned;               /** \brief Number of Abandoned Bundle Payloads */
    uint32_t BundleCountCustodyReForwarded;      /** \brief Number of Bundles reforward due to custody timeout */
    uint32_t BundleCountCustodyRejected;         /** \brief Number of unsuccessful Custody Transfers from the Local node to the next neighboring Custodian Node */
    uint32_t BundleCountCustodyRequest;          /** \brief Number of bundles received that are requesting Custody Transfer */
    uint32_t BundleCountCustodySignalReceived;   /** \brief Number of Custody Signal Bundles received */
    uint32_t BundleCountCustodyTransferred;      /** \brief Number of successful Custody Transfers from the Local node to the next neighboring Custodian Node */
    uint32_t BundleCountDeleted;                 /** \brief Total Number of Bundle Deletions */
    uint32_t BundleCountDeletedBadEid;           /** \brief Number of Bundles deleted due to having a unrecognized destination EID */
    uint32_t BundleCountDeletedCancelled;        /** \brief Number of Bundles Deletions due to Transmission Cancelled Condition */
    uint32_t BundleCountDeletedExpired;          /** \brief Number of Bundles Deletions due to Lifetime Expired Condition */
    uint32_t BundleCountDeletedForwardFailed;    /** \brief Number of Bundles Deletions due to Forwarding Failed Condition */
    uint32_t BundleCountDeletedHopExceeded;      /** \brief Number of Bundles Deletions due to Hop Limit Exceeded Condition */
    uint32_t BundleCountDeletedInvalidPayload;   /** \brief Number of Bundle Deletions due having a Corrupted Payload Block */
    uint32_t BundleCountDeletedNoStorage;        /** \brief Number of Bundles deleted due to insufficient storage */
    uint32_t BundleCountDeletedTooLong;          /** \brief Number of Bundles deleted due to being longer than paramSetMaxBundleLength */
    uint32_t BundleCountDeletedTrafficPared;     /** \brief Number of Bundles Deletions due to Traffic Pared Condition */
    uint32_t BundleCountDeletedUnauthorized;     /** \brief Number of Bundles deleted due to having a unrecognized source EID. Incremented if the bundle is not in the set of authorized source EIDs configured for the node. */
    uint32_t BundleCountDeletedUnintelligible;   /** \brief Number of Bundles Deletions due to Block Unintelligible Condition */
    uint32_t BundleCountDeletedUnsupportedBlock; /** \brief Number of Bundles Deletions due to Unsupported Block Condition */
    uint32_t BundleCountDelivered;               /** \brief Total number of Bundles Delivered to this node, including fragments */
    uint32_t BundleCountDepleted;                /** \brief Number of bundles for which rejected Custody Signals generated indicating rejection due to depleted storage */
    uint32_t BundleCountDiscarded;               /** \brief Number of Bundles Discarded */
    uint32_t BundleCountForwarded;               /** \brief Number of Bundles Forwarded to another DTN Node */
    uint32_t BundleCountForwardedFailed;         /** \brief Number of Bundles where forwarding to another DTN Node failed */
    uint32_t BundleCountFragmentError;           /** \brief Number of Fragments discarded due to bad offset or ADU length */
    uint32_t BundleCountFragmented;              /** \brief Number of Bundles that needed to be Fragmented  */
    uint32_t BundleCountGeneratedAccepted;       /** \brief Number of Accepted Bundle Transmission Requests  */
    uint32_t BundleCountGeneratedAnonymous;      /** \brief Number of Anonomous Bundles Created */
    uint32_t BundleCountGeneratedBsrAccepted;    /** \brief Number of Bundle Custody Accepted Status Report generated since the last counter reset */
    uint32_t BundleCountGeneratedBsrDeleted;     /** \brief Number of Bundle Deleted Status Report generated since the last counter reset */
    uint32_t BundleCountGeneratedBsrDelivered;   /** \brief Number of Bundle Delivered Status Report generated since the last counter reset */
    uint32_t BundleCountGeneratedBsrForwarded;   /** \brief Number of Bundle Forwarded Status Report generated since the last counter reset */
    uint32_t BundleCountGeneratedBsrReceived;    /** \brief Number of Bundle Reception Status Report generated since the last counter reset */
    uint32_t BundleCountGeneratedCrs;            /** \brief Number of Compressed Reporting Signal (CRS) generated since last counter reset. */
    uint32_t BundleCountGeneratedCrsAccepted;    /** \brief Number of accepted bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    uint32_t BundleCountGeneratedCrsDeleted;     /** \brief Number of deleted bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    uint32_t BundleCountGeneratedCrsDelivered;   /** \brief Number of delivered bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    uint32_t BundleCountGeneratedCrsForwarded;   /** \brief Number of forwarded bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    uint32_t BundleCountGeneratedCrsReceived;    /** \brief Number of received bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    uint32_t BundleCountGeneratedCustody;        /** \brief Number of Custody Signal Bundles generated since the last counter reset */
    uint32_t BundleCountGeneratedCustodySignal;  /** \brief Number of bundles for which Custody Signals Generated */
    uint32_t BundleCountGeneratedFragment;       /** \brief Number of Bundle Fragments that were Generated */
    uint32_t BundleCountGeneratedRejected;       /** \brief Number of Rejected Bundle Transmission Requests */
    uint32_t BundleCountInCustody;               /** \brief  */
    uint32_t BundleCountInvalidPrimaryBlock;     /** \brief Number of Unprocessed Bundles received with Invalid Primary Blocks */
    uint32_t BundleCountMaxBsrRateExceeded;      /** \brief Number of BSR bundles not sent because sending would exceed a maximum rate. */
    uint32_t BundleCountMaxCrsRateExceeded;      /** \brief Number of CRS bundles not sent because sending would exceed a maximum rate. */
    uint32_t BundleCountNoContact;               /** \brief Number of bundles for which rejected Custody Signals generated indicating the Destination is not reachable before the Bundle expires */
    uint32_t BundleCountNoFurtherInfo;           /** \brief Number of bundles for which successful Custody Signals generated with No Further Information */
    uint32_t BundleCountNoRoute;                 /** \brief Number of bundles for which rejected Custody Signals generated indicating the Destination is not reachable */
    uint32_t BundleCountReassembled;             /** \brief Total number of Bundles delivered that were fragments and needed to be reassembled */
    uint32_t BundleCountReceived;                /** \brief Number of Bundles Received from another DTN Node */
    uint32_t BundleCountReceivedAdminRecord;     /** \brief Number of admin record bundles received for this DTN Node. */
    uint32_t BundleCountReceivedBsrAccepted;     /** \brief Number of Bundle Custody Accepted Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrDeleted;      /** \brief Number of Bundle Deleted Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrDelivered;    /** \brief Number of Bundle Delivered Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrForwarded;    /** \brief Number of Bundle Forwarded Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrReceived;     /** \brief Number of Bundle Reception Status Report received since the last counter reset */
    uint32_t BundleCountReceivedCrs;             /** \brief Number of Compressed Reporting Signals (CRSs) received since last counter reset. */
    uint32_t BundleCountReceivedCrsAccepted;     /** \brief Number of accepted bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of accepted bundles per source node ID. */
    uint32_t BundleCountReceivedCrsDeleted;      /** \brief Number of deleted bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of deleted bundles per source node ID. */
    uint32_t BundleCountReceivedCrsDelivered;    /** \brief Number of delivered bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of delivered bundles per source node ID. */
    uint32_t BundleCountReceivedCrsForwarded;    /** \brief Number of forwarded bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of forwarded bundles per source node ID. */
    uint32_t BundleCountReceivedCrsReceived;     /** \brief Number of received bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of received bundles per source node ID. */
    uint32_t BundleCountReceivedCustodySignal;   /** \brief Number of bundles for which Custody Signals Received */
    uint32_t BundleCountReceivedFragment;        /** \brief Number of Bundles Received that were Marked as Fragments */
    uint32_t BundleCountRedundant;               /** \brief Number of bundles for which successful Custody Signals generated for Duplicate Bundle reception */
    uint32_t BundleCountRejectedCustody;         /** \brief Number of Bundles where this node rejected custody. */
    uint32_t BundleCountReturned;                /** \brief Number of Bundles Returned to Sender */
    uint32_t BundleCountStorageAvailable;        /** \brief  */
    uint32_t BundleCountStored;                  /** \brief  */
    uint32_t BundleCountUnintelligibleBlock;     /** \brief Number of bundles for which Custody Signals indicating the Bundle contained an unknown block type */
    uint32_t BundleCountUnintelligibleEid;       /** \brief Number of bundles for which rejected Custody Signals generated indicating the any EID in the Primary Header is unknown */
    uint32_t BundleCountUnprocessedBlocks;       /** \brief Number of Unprocessed Blocks Removed from Received Bundles */
    uint32_t NodeStartupCounter;                 /** \brief Number of times a node is started up. */

    uint32_t TimeBootEra;                           /** \brief Boot Era for Monotonic Time */
    int64_t  MonotonicTime;                         /** \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;                     /** \brief Time Correlation Factor */
} BPLib_NodeMibCountersHkTlm_Payload_t;

/**
  * \brief  Source MIB counters housekeeping payload
  * \anchor BPLib_SourceMibCountersSet_t
  */
typedef struct
{
    char SourceEID[BPLIB_MAX_EID_LENGTH];        /** \brief Source EID this telemetry corresponds to */
    uint32_t AduCountDelivered;                  /** \brief Number of ADUs Delivered to the application */
    uint32_t AduCountReceived;                   /** \brief Number of ADUs Received from the application */
    uint32_t BundleCountAbandoned;               /** \brief Number of Abandoned Bundle Payloads */
    uint32_t BundleCountCustodyReForwarded;      /** \brief Number of Bundles reforward due to custody timeout */
    uint32_t BundleCountCustodyRejected;         /** \brief Number of unsuccessful Custody Transfers from the Local node to the next neighboring Custodian Node */
    uint32_t BundleCountCustodyRequest;          /** \brief Number of bundles received that are requesting Custody Transfer */
    uint32_t BundleCountCustodyTransferred;      /** \brief Number of successful Custody Transfers from the Local node to the next neighboring Custodian Node */
    uint32_t BundleCountDeleted;                 /** \brief Total Number of Bundle Deletions */
    uint32_t BundleCountDeletedBadEid;           /** \brief Number of Bundles deleted due to having a unrecognized destination EID */
    uint32_t BundleCountDeletedCancelled;        /** \brief Number of Bundles Deletions due to Transmission Cancelled Condition */
    uint32_t BundleCountDeletedExpired;          /** \brief Number of Bundles Deletions due to Lifetime Expired Condition */
    uint32_t BundleCountDeletedForwardFailed;    /** \brief Number of Bundles Deletions due to Forwarding Failed Condition */
    uint32_t BundleCountDeletedHopExceeded;      /** \brief Number of Bundles Deletions due to Hop Limit Exceeded Condition */
    uint32_t BundleCountDeletedInvalidPayload;   /** \brief Number of Bundle Deletions due having a Corrupted Payload Block */
    uint32_t BundleCountDeletedNoStorage;        /** \brief Number of Bundles deleted due to insufficient storage */
    uint32_t BundleCountDeletedTooLong;          /** \brief Number of Bundles deleted due to being longer than paramSetMaxBundleLength */
    uint32_t BundleCountDeletedTrafficPared;     /** \brief Number of Bundles Deletions due to Traffic Pared Condition */
    uint32_t BundleCountDeletedUnauthorized;     /** \brief Number of Bundles deleted due to having a unrecognized source EID. Incremented if the bundle is not in the set of authorized source EIDs configured for the node. */
    uint32_t BundleCountDeletedUnintelligible;   /** \brief Number of Bundles Deletions due to Block Unintelligible Condition */
    uint32_t BundleCountDeletedUnsupportedBlock; /** \brief Number of Bundles Deletions due to Unsupported Block Condition */
    uint32_t BundleCountDelivered;               /** \brief Total number of Bundles Delivered to this node, including fragments */
    uint32_t BundleCountDepleted;                /** \brief Number of bundles for which rejected Custody Signals generated indicating rejection due to depleted storage */
    uint32_t BundleCountDiscarded;               /** \brief Number of Bundles Discarded */
    uint32_t BundleCountForwarded;               /** \brief Number of Bundles Forwarded to another DTN Node */
    uint32_t BundleCountForwardedFailed;         /** \brief Number of Bundles where forwarding to another DTN Node failed */
    uint32_t BundleCountFragmentError;           /** \brief Number of Fragments discarded due to bad offset or ADU length */
    uint32_t BundleCountFragmented;              /** \brief Number of Bundles that needed to be Fragmented  */
    uint32_t BundleCountGeneratedAccepted;       /** \brief Number of Accepted Bundle Transmission Requests  */
    uint32_t BundleCountGeneratedCustodySignal;  /** \brief Number of bundles for which Custody Signals Generated */
    uint32_t BundleCountGeneratedFragment;       /** \brief Number of Bundle Fragments that were Generated */
    uint32_t BundleCountGeneratedRejected;       /** \brief Number of Rejected Bundle Transmission Requests */
    uint32_t BundleCountMaxBsrRateExceeded;      /** \brief Number of BSR bundles not sent because sending would exceed a maximum rate. */
    uint32_t BundleCountNoContact;               /** \brief Number of bundles for which rejected Custody Signals generated indicating the Destination is not reachable before the Bundle expires */
    uint32_t BundleCountNoFurtherInfo;           /** \brief Number of bundles for which successful Custody Signals generated with No Further Information */
    uint32_t BundleCountNoRoute;                 /** \brief Number of bundles for which rejected Custody Signals generated indicating the Destination is not reachable */
    uint32_t BundleCountReassembled;             /** \brief Total number of Bundles delivered that were fragments and needed to be reassembled */
    uint32_t BundleCountReceived;                /** \brief Number of Bundles Received from another DTN Node */
    uint32_t BundleCountReceivedAdminRecord;     /** \brief Number of admin record bundles received for this DTN Node. */
    uint32_t BundleCountReceivedBsrAccepted;     /** \brief Number of Bundle Custody Accepted Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrDeleted;      /** \brief Number of Bundle Deleted Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrDelivered;    /** \brief Number of Bundle Delivered Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrForwarded;    /** \brief Number of Bundle Forwarded Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrReceived;     /** \brief Number of Bundle Reception Status Report received since the last counter reset */
    uint32_t BundleCountReceivedCrsAccepted;     /** \brief Number of accepted bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of accepted bundles per source node ID. */
    uint32_t BundleCountReceivedCrsDeleted;      /** \brief Number of deleted bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of deleted bundles per source node ID. */
    uint32_t BundleCountReceivedCrsDelivered;    /** \brief Number of delivered bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of delivered bundles per source node ID. */
    uint32_t BundleCountReceivedCrsForwarded;    /** \brief Number of forwarded bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of forwarded bundles per source node ID. */
    uint32_t BundleCountReceivedCrsReceived;     /** \brief Number of received bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of received bundles per source node ID. */
    uint32_t BundleCountReceivedCustodySignal;   /** \brief Number of bundles for which Custody Signals Received */
    uint32_t BundleCountReceivedFragment;        /** \brief Number of Bundles Received that were Marked as Fragments */
    uint32_t BundleCountRedundant;               /** \brief Number of bundles for which successful Custody Signals generated for Duplicate Bundle reception */
    uint32_t BundleCountRejectedCustody;         /** \brief Number of Bundles where this node rejected custody. */
    uint32_t BundleCountReturned;                /** \brief Number of Bundles Returned to Sender */
    uint32_t BundleCountUnintelligibleBlock;     /** \brief Number of bundles for which Custody Signals indicating the Bundle contained an unknown block type */
    uint32_t BundleCountUnintelligibleEid;       /** \brief Number of bundles for which rejected Custody Signals generated indicating the any EID in the Primary Header is unknown */
    uint32_t BundleCountUnprocessedBlocks;       /** \brief Number of Unprocessed Blocks Removed from Received Bundles */
} BPLib_SourceMibCountersSet_t;

/**
  * \brief  Packet for all source counters accessed via an index into SourceCounters
  * \anchor BPLib_SourceMibCountersHkTlm_Payload_t
  */
typedef struct
{
    BPLib_SourceMibCountersSet_t SourceCounters[BPLIB_MAX_NUM_SOURCE_EID]; /** \brief Counters for each source */

    uint32_t Spare2;
    uint32_t TimeBootEra;                   /** \brief Boot Era for Monotonic Time */
    int64_t  MonotonicTime;                 /** \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;             /** \brief Time Correlation Factor */
} BPLib_SourceMibCountersHkTlm_Payload_t;

/* ======= */
/* Globals */
/* ======= */

extern BPLib_NodeMibCountersHkTlm_Payload_t    BPLib_AS_NodeCountersPayload;        /** \brief Global node MIB counter payload */
extern BPLib_SourceMibCountersHkTlm_Payload_t  BPLib_AS_SourceCountersPayload;      /** \brief Global source MID counter payload */
extern BPLib_ChannelContactStatHkTlm_Payload_t BPLib_AS_ChannelContactStatsPayload; /** \brief Global channel contact statistics payload */

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
 * \anchor    BPLib_AS_Init
 */
BPLib_Status_t BPLib_AS_Init(void);

/**
 * \brief     Add an amount to the counter specified by the source EID and counter
 * \details   Incrementing function for counters used by Admin Statistics
 * \note      Internal uint32_t value is passed by reference when using BPLib_AS_Get()
 * \note      Amount must be positive
 * \param[in] SourceEid (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters array.
 * \param[in] Counter (BPLib_AS_Counter_t) Counter to increment
 * \param[in] Amount (uint32_t) Amount to increment Counter by
 * \return    void
 * \secreflist
 * \refitem   BPLib_AS_Get [BPLib_AS_Get()]
 * \refitem   BPLib_SourceMibCountersHkTlm_Payload_t
 * \refitem   BPLib_AS_Counter_t
 * \refitem   BPLib_AS_Set [BPLib_AS_Set()]
 * \refitem   BPLib_AS_EidIsValid [BPLib_AS_EidIsValid()]
 * \endsecreflist
 * \anchor    BPLib_AS_Increment
 */
void BPLib_AS_Increment(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Amount);

/**
 * \brief     Subtract an amount from the counter specified by the source EID and counter
 * \details   Decrementing function for counters used by Admin Statistics
 * \note      Internal uint32_t values is passed by reference when using BPLib_AS_Get()
 * \note      Amount must be positive
 * \param[in] SourceEid (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters array.
 * \param[in] Counter (BPLib_AS_Counter_t) Counter to decrement
 * \param[in] Amount (uint32_t) Amount to decrement by
 * \return    void
 * \secreflist
 * \refitem   BPLib_AS_Get [BPLib_AS_Get()]
 * \refitem   BPLib_SourceMibCountersHkTlm_Payload_t
 * \refitem   BPLib_AS_Counter_t
 * \refitem   BPLib_AS_Set [BPLib_AS_Set()]
 * \refitem   BPLib_AS_EidIsValid [BPLib_AS_EidIsValid()]
 * \endsecreflist
 * \anchor    BPLib_AS_Decrement
 */
void BPLib_AS_Decrement(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Amount);

/**
 * \brief     Set to zero all resettable MIB counters associated with the given source EID pattern
 * \details   See function body and reference the BPLib_AS_Counter_t struct to see which counters are reset
 * \note      Cycles through source counters and uses BPLib_AS_Set() to reset them to 0
 * \param[in] SourceEid (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters array.
 * \return    Execution status
 * \retval    BPLIB_AS_INVALID_EID: Source EID did not pass criteria in BPLib_AS_EidIsValid()
 * \retval    BPLIB_SUCCESS: Successful execution
 * \secreflist
 * \refitem   BPLib_AS_Counter_t
 * \refitem   BPLib_AS_Set [BPLib_AS_Set()]
 * \refitem   BPLib_SourceMibCountersHkTlm_Payload_t
 * \refitem   BPLib_AS_EidIsValid [BPLib_AS_EidIsValid()]
 * \endsecreflist
 * \anchor    BPLib_AS_ResetSourceCounters
 */
BPLib_Status_t BPLib_AS_ResetSourceCounters(int16_t SourceEid);

/**
 * \brief     Set to zero all resettable MIB counters associated with bundles
 * \details   See function body and reference the BPLib_AS_Counter_t struct to see which counters are reset
 * \note      Cycles through bundle-related source and node counters and uses BPLib_AS_Set() to reset them to 0
 * \param[in] SourceEid (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters array.
 * \return    Execution status
 * \retval    BPLIB_AS_INVALID_EID: Source EID did not pass criteria in BPLib_AS_EidIsValid()
 * \retval    BPLIB_SUCCESS: Successful execution
 * \secreflist
 * \refitem   BPLib_AS_Counter_t
 * \refitem   BPLib_AS_Set [BPLib_AS_Set()]
 * \refitem   BPLib_SourceMibCountersHkTlm_Payload_t
 * \refitem   BPLib_AS_EidIsValid [BPLib_AS_EidIsValid()]
 * \endsecreflist
 * \anchor    BPLib_AS_ResetBundleCounters
 */
BPLib_Status_t BPLib_AS_ResetBundleCounters(int16_t SourceEid);

/**
 * \brief     Set to zero all resettable MIB error counters
 * \details   See function body and reference the BPLib_AS_Counter_t struct to see which counters are reset
 * \note      Cycles through error counters and uses BPLib_AS_Set() to reset them to 0
 * \param[in] SourceEid (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters array.
 * \return    Execution status
 * \retval    BPLIB_AS_INVALID_EID: Source EID did not pass criteria in BPLib_AS_EidIsValid()
 * \retval    BPLIB_SUCCESS: Successful execution
 * \secreflist
 * \refitem   BPLib_AS_Counter_t
 * \refitem   BPLib_AS_Set [BPLib_AS_Set()]
 * \refitem   BPLib_SourceMibCountersHkTlm_Payload_t
 * \refitem   BPLib_AS_EidIsValid [BPLib_AS_EidIsValid()]
 * \endsecreflist
 * \anchor    BPLib_AS_ResetErrorCounters
 */
BPLib_Status_t BPLib_AS_ResetErrorCounters(int16_t SourceEid);

/**
 * \brief     Set every counter value in the source and node counter packets to zero
 * \details   Zeroing out function used by Admin Statistics
 * \note      Cycles through each possible source EID and possible counter then sets counters to 0 with BPLib_AS_Set()
 * \param[in] void No arguments accepted
 * \return    void
 * \ref       BPLib_AS_Set [BPLib_AS_Set()]
 * \anchor    BPLib_AS_ResetAllCounters
 */
void BPLib_AS_ResetAllCounters(void);

#endif /* BPLIB_AS_H */
