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

#include "bplib_api_types.h"
#include "bplib_cfg.h"
#include <string.h>

/* ====== */
/* Macros */
/* ====== */

/*
** Number of members in the BPLib_NodeMibCountersHkTlm_Payload_t struct,
** used for looping over members in a get/set function
*/
#define NODE_COUNTERS_PKT_SIZE 28

/* ======= */
/* Typdefs */
/* ======= */

/**
 * \brief Node MIB counters housekeeping payload
 */
typedef struct
{
    uint32_t BundleAgentAcceptedDirectiveCount;     /** \brief Number of control directives received from the Monitor and Control interface that have been accepted */
    uint32_t BundleAgentRejectedDirectiveCount;     /** \brief Number of control directives received from the Monitor and Control interface that have been rejected as being invalid */
    uint32_t BundleCountGeneratedAccepted;          /** \brief Number of Accepted Bundle Transmission Requests  */
    uint32_t BundleCountGeneratedRejected;          /** \brief Number of Rejected Bundle Transmission Requests */
    uint32_t BundleCountGeneratedCustody;           /** \brief Number of Custody Signal Bundles generated since the last counter reset */
    uint32_t BundleCountGeneratedBsrReceived;       /** \brief Number of Bundle Reception Status Report generated since the last counter reset */
    uint32_t BundleCountGeneratedBsrAccepted;       /** \brief Number of Bundle Custody Accepted Status Report generated since the last counter reset */
    uint32_t BundleCountGeneratedBsrForwarded;      /** \brief Number of Bundle Forwarded Status Report generated since the last counter reset */
    uint32_t BundleCountGeneratedBsrDelivered;      /** \brief Number of Bundle Delivered Status Report generated since the last counter reset */
    uint32_t BundleCountGeneratedBsrDeleted;        /** \brief Number of Bundle Deleted Status Report generated since the last counter reset */
    uint32_t BundleCountMaxBsrRateExceeded;         /** \brief Number of BSR bundles not sent because sending would exceed a maximum rate. */
    uint32_t BundleCountGeneratedAnonymous;         /** \brief Number of Anonomous Bundles Created */
    uint32_t BundleCountGeneratedFragment;          /** \brief Number of Bundle Fragments that were Generated */
    uint32_t BundleCountReceived;                   /** \brief Number of Bundles Received from another DTN Node */
    uint32_t BundleCountReceivedFragment;           /** \brief Number of Bundles Received that were Marked as Fragments */
    uint32_t BundleCountUnprocessedBlocks;          /** \brief Number of Unprocessed Blocks Removed from Received Bundles */
    uint32_t BundleCountInvalidPrimaryBlock;        /** \brief Number of Unprocessed Bundles received with Invalid Primary Blocks */
    uint32_t BundleCountCustodySignalReceived;      /** \brief Number of Custody Signal Bundles received */
    uint32_t BundleCountForwarded;                  /** \brief Number of Bundles Forwarded to another DTN Node */
    uint32_t BundleCountReturned;                   /** \brief Number of Bundles Returned to Sender */
    uint32_t BundleCountFragmented;                 /** \brief Number of Bundles that needed to be Fragmented  */
    uint32_t BundleCountReassembled;                /** \brief Total number of Bundles delivered that were fragments and needed to be reassembled */
    uint32_t BundleCountFragmentError;              /** \brief Number of Fragments discarded due to bad offset or ADU length */
    uint32_t BundleCountDelivered;                  /** \brief Total number of Bundles Delivered to this node, including fragments */
    uint32_t BundleCountAbandoned;                  /** \brief Number of Abandoned Bundle Payloads */
    uint32_t AduCountDelivered;                     /** \brief Number of ADUs Delivered to the application */
    uint32_t BundleCountDeleted;                    /** \brief Total Number of Bundle Deletions */
    uint32_t BundleCountDeletedExpired;             /** \brief Number of Bundles Deletions due to Lifetime Expired Condition */
    uint32_t BundleCountDeletedHopExceeded;         /** \brief Number of Bundles Deletions due to Hop Limit Exceeded Condition */
    uint32_t BundleCountDeletedInvalidPayload;      /** \brief Number of Bundle Deletions due having a Corrupted Payload Block */
    uint32_t BundleCountDeletedForwardFailed;       /** \brief Number of Bundles Deletions due to Forwarding Failed Condition */
    uint32_t BundleCountDeletedTrafficPared;        /** \brief Number of Bundles Deletions due to Traffic Pared Condition */
    uint32_t BundleCountDeletedUnintelligible;      /** \brief Number of Bundles Deletions due to Block Unintelligible Condition */
    uint32_t BundleCountDeletedUnsupportedBlock;    /** \brief Number of Bundles Deletions due to Unsupported Block Condition */
    uint32_t BundleCountDeletedCancelled;           /** \brief Number of Bundles Deletions due to Transmission Cancelled Condition */
    uint32_t BundleCountDiscarded;                  /** \brief Number of Bundles Discarded */
    uint32_t BundleCountDeletedNoStorage;           /** \brief Number of Bundles deleted due to insufficient storage */
    uint32_t BundleCountDeletedBadEid;              /** \brief Number of Bundles deleted due to having a unrecognized destination EID */
    uint32_t BundleCountDeletedUnauthorized;        /** \brief Number of Bundles deleted due to having a unrecognized source EID. Incremented if the bundle is not in the set of authorized source EIDs configured for the node. */
    uint32_t BundleCountDeletedtooLong;             /** \brief Number of Bundles deleted due to being longer than paramSetMaxBundleLength */
    uint32_t BundleCountCustodyTransferred;         /** \brief Number of successful Custody Transfers from the Local node to the next neighboring Custodian Node */
    uint32_t BundleCountCustodyRejected;            /** \brief Number of unsuccessful Custody Transfers from the Local node to the next neighboring Custodian Node */
    uint32_t BundleCountCustodyRequest;             /** \brief Number of bundles received that are requesting Custody Transfer */
    uint32_t BundleCountCustodyReForwarded;         /** \brief Number of Bundles reforward due to custody timeout */
    uint32_t BundleCountNoFurtherInfo;              /** \brief Number of bundles for which successful Custody Signals generated with No Further Information */
    uint32_t BundleCountRedundant;                  /** \brief Number of bundles for which successful Custody Signals generated for Duplicate Bundle reception */
    uint32_t BundleCountDepleted;                   /** \brief Number of bundles for which rejected Custody Signals generated indicating rejection due to depleted storage */
    uint32_t BundleCountUnintelligibleEid;          /** \brief Number of bundles for which rejected Custody Signals generated indicating the any EID in the Primary Header is unknown */
    uint32_t BundleCountNoRoute;                    /** \brief Number of bundles for which rejected Custody Signals generated indicating the Destination is not reachable */
    uint32_t BundleCountNoContact;                  /** \brief Number of bundles for which rejected Custody Signals generated indicating the Destination is not reachable before the Bundle expires */
    uint32_t BundleCountUnintelligibleBlock;        /** \brief Number of bundles for which Custody Signals indicating the Bundle contained an unknown block type */
    uint32_t BundleCountReceivedCustodySignal;      /** \brief Number of bundles for which Custody Signals Received */
    uint32_t BundleCountGeneratedCustodySignal;     /** \brief Number of bundles for which Custody Signals Generated */
    uint32_t BundleCountRejectedCustody;            /** \brief Number of Bundles where this node rejected custody. */
    uint32_t BundleCountGeneratedCrsReceived;       /** \brief Number of received bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    uint32_t BundleCountGeneratedCrsAccepted;       /** \brief Number of accepted bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    uint32_t BundleCountGeneratedCrsForwarded;      /** \brief Number of forwarded bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    uint32_t BundleCountGeneratedCrsDelivered;      /** \brief Number of delivered bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    uint32_t BundleCountGeneratedCrsDeleted;        /** \brief Number of deleted bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    uint32_t BundleCountMaxCrsRateExceeded;         /** \brief Number of CRS bundles not sent because sending would exceed a maximum rate. */
    uint32_t AduCountReceived;                      /** \brief Number of ADUs Received from the application */
    uint32_t NodeStartupCounter;                    /** \brief Number of times a node is started up. */
    uint32_t BundleCountGeneratedCrs;               /** \brief Number of Compressed Reporting Signal (CRS) generated since last counter reset. */
    uint32_t BundleCountForwardedFailed;            /** \brief Number of Bundles where forwarding to another DTN Node failed */
    uint32_t BundleCountReceivedBsrReceived;        /** \brief Number of Bundle Reception Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrAccepted;        /** \brief Number of Bundle Custody Accepted Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrForwarded;       /** \brief Number of Bundle Forwarded Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrDelivered;       /** \brief Number of Bundle Delivered Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrDeleted;         /** \brief Number of Bundle Deleted Status Report received since the last counter reset */
    uint32_t BundleCountReceivedCrs;                /** \brief Number of Compressed Reporting Signals (CRSs) received since last counter reset. */
    uint32_t BundleCountReceivedCrsReceived;        /** \brief Number of received bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of received bundles per source node ID. */
    uint32_t BundleCountReceivedCrsAccepted;        /** \brief Number of accepted bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of accepted bundles per source node ID. */
    uint32_t BundleCountReceivedCrsForwarded;       /** \brief Number of forwarded bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of forwarded bundles per source node ID. */
    uint32_t BundleCountReceivedCrsDelivered;       /** \brief Number of delivered bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of delivered bundles per source node ID. */
    uint32_t BundleCountReceivedCrsDeleted;         /** \brief Number of deleted bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of deleted bundles per source node ID. */
    uint32_t BundleCountReceivedAdminRecord;        /** \brief Number of admin record bundles received for this DTN Node. */

    uint32_t Spare1;
    uint32_t TimeBootEra;                           /** \brief Boot Era for Monotonic Time */
    int64_t  MonotonicTime;                         /** \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;                     /** \brief Time Correlation Factor */
} BPLib_NodeMibCountersHkTlm_Payload_t;

/*
 * \brief Source MIB counters housekeeping payload
 */
typedef struct
{
    char SourceEID[BPLIB_MAX_EID_LENGTH];       /**< \brief Source EID this telemetry corresponds to */
    uint32_t BundleCountGeneratedAccepted;        /** \brief Number of Accepted Bundle Transmission Requests */
    uint32_t BundleCountGeneratedRejected;        /** \brief Number of Rejected Bundle Transmission Requests */
    uint32_t BundleCountMaxBsrRateExceeded;       /** \brief Number of BSR bundles not sent because sending would exceed a maximum rate. */
    uint32_t BundleCountGeneratedFragment;        /** \brief Number of Bundle Fragments that were Generated */
    uint32_t BundleCountReceived;                 /** \brief Number of Bundles Received from another DTN Node */
    uint32_t BundleCountReceivedFragment;         /** \brief Number of Bundles Received that were Marked as Fragments */
    uint32_t BundleCountUnprocessedBlocks;        /** \brief Number of Unprocessed Blocks Removed from Received Bundles */
    uint32_t BundleCountForwarded;                /** \brief Number of Bundles Forwarded to another DTN Node */
    uint32_t BundleCountReturned;                 /** \brief Number of Bundles Returned to Sender */
    uint32_t BundleCountFragmented;               /** \brief Number of Bundles that needed to be Fragmented */
    uint32_t BundleCountReassembled;              /** \brief Total number of Bundles delivered that were fragments and needed to be reassembled */
    uint32_t BundleCountFragmentError;            /** \brief Number of Fragments discarded due to bad offset or ADU length */
    uint32_t BundleCountDelivered;                /** \brief Total number of Bundles Delivered to this node, including fragments */
    uint32_t BundleCountAbandoned;                /** \brief Number of Abandoned Bundle Payloads */
    uint32_t AduCountDelivered;                   /** \brief Number of ADUs Delivered to the application */
    uint32_t BundleCountDeleted;                  /** \brief Total Number of Bundle Deletions */
    uint32_t BundleCountDeletedExpired;           /** \brief Number of Bundles Deletions due to Lifetime Expired Condition */
    uint32_t BundleCountDeletedHopExceeded;       /** \brief Number of Bundles Deletions due to Hop Limit Exceeded Condition */
    uint32_t BundleCountDeletedInvalidPayload;    /** \brief Number of Bundle Deletions due having a Corrupted Payload Block */
    uint32_t BundleCountDeletedForwardFailed;     /** \brief Number of Bundles Deletions due to Forwarding Failed Condition */
    uint32_t BundleCountDeletedTrafficPared;      /** \brief Number of Bundles Deletions due to Traffic Pared Condition */
    uint32_t BundleCountDeletedUnintelligible;    /** \brief Number of Bundles Deletions due to Block Unintelligible Condition */
    uint32_t BundleCountDeletedUnsupportedBlock;  /** \brief Number of Bundles Deletions due to Unsupported Block Condition */
    uint32_t BundleCountDeletedCancelled;         /** \brief Number of Bundles Deletions due to Transmission Cancelled Condition */
    uint32_t BundleCountDiscarded;                /** \brief Number of Bundles Discarded */
    uint32_t BundleCountDeletedNoStorage;         /** \brief Number of Bundles deleted due to insufficient storage */
    uint32_t BundleCountDeletedBadEid;            /** \brief Number of Bundles deleted due to having a unrecognized destination EID */
    uint32_t BundleCountDeletedtooLong;           /** \brief Number of Bundles deleted due to being longer than paramSetMaxBundleLength */
    uint32_t BundleCountCustodyTransferred;       /** \brief Number of successful Custody Transfers from the Local node to the next neighboring Custodian Node */
    uint32_t BundleCountCustodyRejected;          /** \brief Number of unsuccessful Custody Transfers from the Local node to the next neighboring Custodian Node */
    uint32_t BundleCountCustodyRequest;           /** \brief Number of bundles received that are requesting Custody Transfer */
    uint32_t BundleCountCustodyReForwarded;       /** \brief Number of Bundles reforward due to custody timeout */
    uint32_t BundleCountNoFurtherInfo;            /** \brief Number of bundles for which successful Custody Signals generated with No Further Information */
    uint32_t BundleCountRedundant;                /** \brief Number of bundles for which successful Custody Signals generated for Duplicate Bundle reception */
    uint32_t BundleCountDepleted;                 /** \brief Number of bundles for which rejected Custody Signals generated indicating rejection due to depleted storage */
    uint32_t BundleCountUnintelligibleEid;        /** \brief Number of bundles for which rejected Custody Signals generated indicating the any EID in the Primary Header is unknown */
    uint32_t BundleCountNoRoute;                  /** \brief Number of bundles for which rejected Custody Signals generated indicating the Destination is not reachable */
    uint32_t BundleCountNoContact;                /** \brief Number of bundles for which rejected Custody Signals generated indicating the Destination is not reachable before the Bundle expires */
    uint32_t BundleCountUnintelligibleBlock;      /** \brief Number of bundles for which Custody Signals indicating the Bundle contained an unknown block type */
    uint32_t BundleCountReceivedCustodySignal;    /** \brief Number of bundles for which Custody Signals Received */
    uint32_t BundleCountGeneratedCustodySignal;   /** \brief Number of bundles for which Custody Signals Generated */
    uint32_t BundleCountRejectedCustody;          /** \brief Number of Bundles where this node rejected custody. */
    uint32_t AduCountReceived;                    /** \brief Number of ADUs Received from the application */
    uint32_t BundleCountForwardedFailed;          /** \brief Number of Bundles where forwarding to another DTN Node failed */
    uint32_t BundleCountReceivedBsrReceived;      /** \brief Number of Bundle Reception Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrAccepted;      /** \brief Number of Bundle Custody Accepted Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrForwarded;     /** \brief Number of Bundle Forwarded Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrDelivered;     /** \brief Number of Bundle Delivered Status Report received since the last counter reset */
    uint32_t BundleCountReceivedBsrDeleted;       /** \brief Number of Bundle Deleted Status Report received since the last counter reset */
    uint32_t BundleCountReceivedCrsReceived;      /** \brief Number of received bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of received bundles per source node ID. */
    uint32_t BundleCountReceivedCrsAccepted;      /** \brief Number of accepted bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of accepted bundles per source node ID. */
    uint32_t BundleCountReceivedCrsForwarded;     /** \brief Number of forwarded bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of forwarded bundles per source node ID. */
    uint32_t BundleCountReceivedCrsDelivered;     /** \brief Number of delivered bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of delivered bundles per source node ID. */
    uint32_t BundleCountReceivedCrsDeleted;       /** \brief Number of deleted bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of deleted bundles per source node ID. */
    uint32_t BundleCountReceivedAdminRecord;      /** \brief Number of admin record bundles received for this DTN Node. */
    uint32_t Spare1;
} BPLib_SourceMibCountersSet_t;

/*
 * \brief Packet for all source counters accessed via an index into SourceCounters
 */
typedef struct
{
    BPLib_SourceMibCountersSet_t SourceCounters[BPLIB_MAX_NUM_SOURCE_EID]; /**< \brief Counters for each source */

    uint32_t Spare2;
    uint32_t TimeBootEra;                   /**< \brief Boot Era for Monotonic Time */
    int64_t  MonotonicTime;                 /**< \brief Monotonic Time Counter */
    int64_t  CorrelationFactor;             /**< \brief Time Correlation Factor */
} BPLib_SourceMibCountersHkTlm_Payload_t;

/*
 * \brief Used to access counters in the BPLib_NodeMibCountersHkTlm_Payload_t struct
 */
typedef enum
{
    BUNDLE_CNT_GEN_ANONYMOUS    = 0,  /* Bundle count generated anonymous */
    SYSTEM_NODE_UP_TIME         = 1,  /* System node up time */
    ACCEPTED_DIRECTIVE_CNT      = 2,  /* Bundle Agent Accepted Directive Count */
    REJECTED_DIRECTIVE_CNT      = 3,  /* Bundle Agent Rejected Directive Count */
    BUNDLE_CNT_GEN_CUSTODY      = 4,  /* Bundle Number of custody signal generated */
    BUNDLE_CNT_GEN_BSR_RECV     = 5,  /* Bundle Received BSR Counts */
    BUNDLE_CNT_GEN_BSR_ACCPT    = 6,  /* Bundle Accepted BSR Counts */
    BUNDLE_CNT_GEN_BSR_FORW     = 7,  /* Bundle Forwarded BSR Counts */
    BUNDLE_CNT_GEN_BSR_DELVR    = 8,  /* Bundle Delivered BSR Counts */
    BUNDLE_CNT_GEN_BSR_DEL      = 9,  /* Bundle Deleted BSR Counts */
    BUNDLE_CNT_INVAL_PRI_BLK    = 10, /* Bundle unprocessed due to Invalid primary block Counts */
    BUNDLE_CNT_CS_RECV          = 11, /* Received custody signal Bundle Counts */
    BUNDLE_CNT_DEL_UNAUTH_SRC   = 12, /* Deleted bundle Counts for unauthorized Src EID */
    BUNDLE_CNT_GEN_CRS_RECV     = 13, /* Bundle Received CRS Counts */
    BUNDLE_CNT_GEN_CRS_ACCPT    = 14, /* Bundle Accepted CRS Counts */
    BUNDLE_CNT_GEN_CRS_FORW     = 15, /* Bundle Forwarded CRS Counts */
    BUNDLE_CNT_GEN_CRS_DELVR    = 16, /* Bundle Delivered CRS Counts */
    BUNDLE_CNT_GEN_CRS_DEL      = 17, /* Bundle Deleted CRS Counts */
    BUNDLE_CNT_MAX_CRS_RATE_EXC = 18, /* Number of CRS bundles not sent to avoid exceeding max rate */
    NODE_STARTUP_COUNTER        = 19, /* Number of times the node has been started up */
    BUNDLE_CNT_GEN_CRS          = 20, /* Number of CRSs generated */
    BUNDLE_CNT_RECV_CRS         = 21, /* Number of CRSs received */
    ADU_COUNT_DELIVERED         = 22, /* ADU Delivered Count */
    ADU_COUNT_RECEIVED          = 23, /* ADU Received Count */
    SPARE                       = 24, /* Irrelevant spare value for alignment */
    TIME_BOOT_ERA               = 25, /* Boot Era for Monotonic Time */
    TIME_MONOTONIC_CNT          = 26, /* Monotonic Time Counter */
    TIME_CF                     = 27, /* Time Correlation Factor */
} BPLib_AS_NodeCounter_t;

/*
 * \brief Determines whether the intended operation is for a node or source counter
 */
typedef enum
{
    NODE_COUNTER   = 1, /* Intent is for a counter in the BPLib_SourceMibCountersHkTlm_Payload_t struct */
    SOURCE_COUNTER = 2, /* Intent is for a counter in the BPLib_SourceMibCountersSet_t struct */
} BPLib_AS_CounterPacket_t;

/* ======= */
/* Externs */
/* ======= */

/* Global node counter packet manipulated by AS */
extern BPLib_NodeMibCountersHkTlm_Payload_t BPLib_AS_NodeCountersPayload;

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
  * \brief     Allows the caller to get access to certain values in counter packets specified by the parameters.
  *            This is necessary to simplify the code and avoid walls of switch-case statements
  * \details   Accessor function for counters used by Admin Statistics
  * \note      Return type is void* since the data type of the values in the packets can vary
  * \param[in] CounterType  BPLib_AS_CounterPacket_t that indicates whether the caller is requesting a node counter or source counter
  * \param[in] CounterIndex BPLib_AS_NodeCounter_t that indicates which counter to access
  * \return    Counter value as a void*
  */
void* BPLib_AS_Get(BPLib_AS_CounterPacket_t CounterType, BPLib_AS_NodeCounter_t CounterIndex);

/**
 * \brief     Modify the counter specified by the counter type and counter index to the desired value
 * \details   Mutator function for counters used by Admin Statistics
 * \note      Casts void* to the appropriate data type so it's possible some data could be truncated.
 *            void* is used since the data type of the value in the packet can vary
 * \param[in] CounterType     BPLib_AS_CounterPacket_t that indicates whether the caller is requesting a node counter or source counter
 * \param[in] CounterIndex    BPLib_AS_NodeCounter_t that indicates which counter to mutate
 * \param[in] DesiredValuePtr void* that holds the value that the caller wishes to set the counter represented by CounterType and CounterIndex
 * \return    Execution status
 * \retval    BPLIB_SUCCESS:                 Mutation was successful
 * \retval    BPLIB_INVALID_NODE_CNTR_INDEX: Index into node counter packet is out of range
 */
BPLib_Status_t BPLib_AS_Set(BPLib_AS_CounterPacket_t CounterType, BPLib_AS_NodeCounter_t CounterIndex, void* DesiredValuePtr);

/**
 * \brief     Add 1 to the counter specified by the counter type and counter index
 * \details   Incrementing function for counters used by Admin Statistics
 * \note      Casting to the appropriate data type is used since the data type of the value in the packet can vary
 * \param[in] CounterType  BPLib_AS_CounterPacket_t that indicates whether the caller is requesting a node counter or source counter
 * \param[in] CounterIndex BPLib_AS_NodeCounter_t that indicates which counter to increment
 * \return    Execution status
 * \retval    Status is determined by BPLib_AS_Set()
 * \retval    BPLIB_SUCCESS:                 Mutation was successful
 * \retval    BPLIB_INVALID_NODE_CNTR_INDEX: Index into node counter packet is out of range
 */
BPLib_Status_t BPLib_AS_Increment(BPLib_AS_CounterPacket_t CounterType, BPLib_AS_NodeCounter_t CounterIndex);

/**
 * \brief     Subtract 1 to the counter specified by the counter type and counter index
 * \details   Decrementing function for counters used by Admin Statistics
 * \note      Casts void* to the appropriate data type so it's possible some data could be truncated.
 *            void* is used since the data type of the value in the packet can vary
 * \param[in] CounterType     BPLib_AS_CounterPacket_t that indicates whether the caller is requesting a node counter or source counter
 * \param[in] CounterIndex    BPLib_AS_NodeCounter_t that indicates which counter to mutate
 * \param[in] DesiredValuePtr void* that holds the value that the caller wishes to set the counter represented by CounterType and CounterIndex
 * \return    Execution status
 * \retval    BPLIB_SUCCESS:                 Mutation was successful
 * \retval    BPLIB_INVALID_NODE_CNTR_INDEX: Index into node counter packet is out of range
 */
BPLib_Status_t BPLib_AS_Decrement(BPLib_AS_CounterPacket_t CounterType, BPLib_AS_NodeCounter_t CounterIndex);
BPLib_Status_t BPLib_AS_SetAllZero(BPLib_AS_CounterPacket_t CounterType);
BPLib_Status_t BPLib_AS_Write(void);
BPLib_Status_t BPLib_AS_Restore(void);

#endif /* BPLIB_AS_H */
