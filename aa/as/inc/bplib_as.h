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

/* ====== */
/* Macros */
/* ====== */

#define BPLIB_AS_NUM_CNTRS      (76u)   /** \brief Total number of MIB counters */
#define BPLIB_AS_NUM_NODE_CNTRS (69u)   /** \brief Number of resettable node MIB counters */
#define BPLIB_AS_NUM_SRC_CNTRS  (49u)   /** \brief Number of resettable source MIB counters */
#define BPLIB_AS_NUM_BNDL_CNTRS (64u)   /** \brief Number of bundle-related MIB counters */
#define BPLIB_AS_BNDL_SRC_START (51u)   /** \brief Index 51 is the start of bundle-related source counters */
#define BPLIB_AS_NUM_ERR_CNTRS  (15u)   /** \brief Number of error-related MIB counters */

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
    uint32_t BundleCountDeletedTooLong;             /** \brief Number of Bundles deleted due to being longer than paramSetMaxBundleLength */
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
    char SourceEID[BPLIB_MAX_EID_LENGTH];         /**< \brief Source EID this telemetry corresponds to */
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
    uint32_t BundleCountDeletedTooLong;           /** \brief Number of Bundles deleted due to being longer than paramSetMaxBundleLength */
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
 * \brief Used to access counters in the node and source payload structs
 */
typedef enum
{
    /* Counters in node and source packets */
    BUNDLE_CNT_RECV_ADMIN_REC    =  0, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReceivedAdminRecord     | BPLib_SourceMibCountersSet_t::BundleCountReceivedAdminRecord */
    BUNDLE_CNT_RET               =  1, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReturned                | BPLib_SourceMibCountersSet_t::BundleCountReturned */
    BUNDLE_CNT_DEL_FORW_FAILED   =  2, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDeletedForwardFailed    | BPLib_SourceMibCountersSet_t::BundleCountDeletedForwardFailed */
    BUNDLE_CNT_DEL_CANCELLED     =  3, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDeletedCancelled        | BPLib_SourceMibCountersSet_t::BundleCountDeletedCancelled */
    BUNDLE_CNT_DEL_BAD_EID       =  4, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDeletedBadEid           | BPLib_SourceMibCountersSet_t::BundleCountDeletedBadEid */
    BUNDLE_CNT_NO_ROUTE          =  5, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountNoRoute                 | BPLib_SourceMibCountersSet_t::BundleCountNoRoute */
    BUNDLE_CNT_NO_CONTACT        =  6, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountNoContact               | BPLib_SourceMibCountersSet_t::BundleCountNoContact */
    ADU_CNT_DELVR                =  7, /* BPLib_NodeMibCountersHkTlm_Payload_t::AduCountDelivered                  | BPLib_SourceMibCountersSet_t::AduCountDelivered */
    ADU_CNT_RECV                 =  8, /* BPLib_NodeMibCountersHkTlm_Payload_t::AduCountReceived                   | BPLib_SourceMibCountersSet_t::AduCountReceived */
    BUNDLE_CNT_GEN_ACCPT         =  9, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedAccepted       | BPLib_SourceMibCountersSet_t::BundleCountGeneratedAccepted */
    BUNDLE_CNT_GEN_REJ           = 10, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedRejected       | BPLib_SourceMibCountersSet_t::BundleCountGeneratedRejected */
    BUNDLE_CNT_GEN_FRAG          = 11, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedFragment       | BPLib_SourceMibCountersSet_t::BundleCountGeneratedFragment */
    BUNDLE_CNT_RECV              = 12, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReceived                | BPLib_SourceMibCountersSet_t::BundleCountReceived */
    BUNDLE_CNT_RECV_FRAG         = 13, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReceivedFragment        | BPLib_SourceMibCountersSet_t::BundleCountReceivedFragment */
    BUNDLE_CNT_UNPROC_BLKS       = 14, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountUnprocessedBlocks       | BPLib_SourceMibCountersSet_t::BundleCountUnprocessedBlocks */
    BUNDLE_CNT_FORW              = 15, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountForwarded               | BPLib_SourceMibCountersSet_t::BundleCountForwarded */
    BUNDLE_CNT_FRAG              = 16, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountFragmented              | BPLib_SourceMibCountersSet_t::BundleCountFragmented */
    BUNDLE_CNT_REASSEMBLED       = 17, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReassembled             | BPLib_SourceMibCountersSet_t::BundleCountReassembled */
    BUNDLE_CNT_FRAG_ERR          = 18, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountFragmentError           | BPLib_SourceMibCountersSet_t::BundleCountFragmentError */
    BUNDLE_CNT_DELVR             = 19, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDelivered               | BPLib_SourceMibCountersSet_t::BundleCountDelivered */
    BUNDLE_CNT_ABAND             = 20, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountAbandoned               | BPLib_SourceMibCountersSet_t::BundleCountAbandoned */
    BUNDLE_CNT_DEL               = 21, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDeleted                 | BPLib_SourceMibCountersSet_t::BundleCountDeleted */
    BUNDLE_CNT_DEL_EXP           = 22, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDeletedExpired          | BPLib_SourceMibCountersSet_t::BundleCountDeletedExpired */
    BUNDLE_CNT_DEL_HOP_EXCD      = 23, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDeletedHopExceeded      | BPLib_SourceMibCountersSet_t::BundleCountDeletedHopExceeded */
    BUNDLE_CNT_DEL_INVAL_PAY     = 24, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDeletedInvalidPayload   | BPLib_SourceMibCountersSet_t::BundleCountDeletedInvalidPayload */
    BUNDLE_CNT_DEL_TRAF_PARED    = 25, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDeletedTrafficPared     | BPLib_SourceMibCountersSet_t::BundleCountDeletedTrafficPared */
    BUNDLE_CNT_DEL_UNINTEL       = 26, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDeletedUnintelligible   | BPLib_SourceMibCountersSet_t::BundleCountDeletedUnintelligible */
    BUNDLE_CNT_DEL_UNSUPPORT_BLK = 27, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDeletedUnsupportedBlock | BPLib_SourceMibCountersSet_t::BundleCountDeletedUnsupportedBlock */
    BUNDLE_CNT_DISCARDED         = 28, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDiscarded               | BPLib_SourceMibCountersSet_t::BundleCountDiscarded */
    BUNDLE_CNT_DEL_NO_STOR       = 29, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDeletedNoStorage        | BPLib_SourceMibCountersSet_t::BundleCountDeletedNoStorage */
    BUNDLE_CNT_DEL_TOO_LONG      = 30, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDeletedTooLong          | BPLib_SourceMibCountersSet_t::BundleCountDeletedTooLong */
    BUNDLE_CNT_CUSTODY_TRANS     = 31, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountCustodyTransferred      | BPLib_SourceMibCountersSet_t::BundleCountCustodyTransferred */
    BUNDLE_CNT_CUSTODY_REJ       = 32, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountCustodyRejected         | BPLib_SourceMibCountersSet_t::BundleCountCustodyRejected */
    BUNDLE_CNT_CUSTODY_REQ       = 33, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountCustodyRequest          | BPLib_SourceMibCountersSet_t::BundleCountCustodyRequest */
    BUNDLE_CNT_CUSTODY_RE_FORW   = 34, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountCustodyReForwarded      | BPLib_SourceMibCountersSet_t::BundleCountCustodyReForwarded */
    BUNDLE_CNT_NO_FURTHER_INFO   = 35, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountNoFurtherInfo           | BPLib_SourceMibCountersSet_t::BundleCountNoFurtherInfo */
    BUNDLE_CNT_RED               = 36, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountRedundant               | BPLib_SourceMibCountersSet_t::BundleCountRedundant */
    BUNDLE_CNT_DEPLETED          = 37, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDepleted                | BPLib_SourceMibCountersSet_t::BundleCountDepleted */
    BUNDLE_CNT_UNINTEL_EID       = 38, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountUnintelligibleEid       | BPLib_SourceMibCountersSet_t::BundleCountUnintelligibleEid */
    BUNDLE_CNT_UNINTEL_BLK       = 39, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountUnintelligibleBlock     | BPLib_SourceMibCountersSet_t::BundleCountUnintelligibleBlock */
    BUNDLE_CNT_RECV_CS           = 40, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReceivedCustodySignal   | BPLib_SourceMibCountersSet_t::BundleCountReceivedCustodySignal */
    BUNDLE_CNT_GEN_CS            = 41, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedCustodySignal  | BPLib_SourceMibCountersSet_t::BundleCountGeneratedCustodySignal */
    BUNDLE_CNT_REJ_CUSTODY       = 42, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountRejectedCustody         | BPLib_SourceMibCountersSet_t::BundleCountRejectedCustody */
    BUNDLE_CNT_FORW_FAILED       = 43, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountForwardedFailed         | BPLib_SourceMibCountersSet_t::BundleCountForwardedFailed */
    BUNDLE_CNT_RECV_BSR_RECV     = 44, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReceivedBsrReceived     | BPLib_SourceMibCountersSet_t::BundleCountReceivedBsrReceived */
    BUNDLE_CNT_RECV_BSR_ACCPT    = 45, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReceivedBsrAccepted     | BPLib_SourceMibCountersSet_t::BundleCountReceivedBsrAccepted */
    BUNDLE_CNT_RECV_BSR_FORW     = 46, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReceivedBsrForwarded    | BPLib_SourceMibCountersSet_t::BundleCountReceivedBsrForwarded */
    BUNDLE_CNT_RECV_BSR_DELVR    = 47, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReceivedBsrDelivered    | BPLib_SourceMibCountersSet_t::BundleCountReceivedBsrDelivered */
    BUNDLE_CNT_RECV_BSR_DEL      = 48, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReceivedBsrDeleted      | BPLib_SourceMibCountersSet_t::BundleCountReceivedBsrDeleted */
    BUNDLE_CNT_RECV_CRS_RECV     = 49, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReceivedCrsReceived     | BPLib_SourceMibCountersSet_t::BundleCountReceivedCrsReceived */
    BUNDLE_CNT_RECV_CRS_ACCPT    = 50, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReceivedCrsAccepted     | BPLib_SourceMibCountersSet_t::BundleCountReceivedCrsAccepted */
    BUNDLE_CNT_RECV_CRS_FORW     = 51, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReceivedCrsForwarded    | BPLib_SourceMibCountersSet_t::BundleCountReceivedCrsForwarded */
    BUNDLE_CNT_RECV_CRS_DELVR    = 52, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReceivedCrsDelivered    | BPLib_SourceMibCountersSet_t::BundleCountReceivedCrsDelivered */
    BUNDLE_CNT_RECV_CRS_DEL      = 53, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReceivedCrsDeleted      | BPLib_SourceMibCountersSet_t::BundleCountReceivedCrsDeleted */
    BUNDLE_CNT_MAX_BSR_RATE_EXCD = 54, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountMaxBsrRateExceeded      | BPLib_SourceMibCountersSet_t::BundleCountMaxBsrRateExceeded */

    /* Counters only in node packets */
    BUNDLE_CNT_GEN_CUSTODY       = 55, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedCustody */
    BUNDLE_CNT_GEN_BSR_RECV      = 56, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedBsrReceived */
    BUNDLE_CNT_GEN_BSR_ACCPT     = 57, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedBsrAccepted */
    BUNDLE_CNT_GEN_BSR_FORW      = 58, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedBsrForwarded */
    BUNDLE_CNT_GEN_BSR_DELVR     = 59, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedBsrDelivered */
    BUNDLE_CNT_GEN_BSR_DEL       = 60, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedBsrDeleted */
    BUNDLE_CNT_INVAL_PRI_BLK     = 61, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountInvalidPrimaryBlock */
    BUNDLE_CNT_CS_RECV           = 62, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountCustodySignalReceived */
    BUNDLE_CNT_GEN_CRS_RECV      = 63, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedCrsReceived */
    BUNDLE_CNT_GEN_CRS_ACCPT     = 64, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedCrsAccepted */
    BUNDLE_CNT_GEN_CRS_FORW      = 65, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedCrsForwarded */
    BUNDLE_CNT_GEN_CRS_DELVR     = 66, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedCrsDelivered */
    BUNDLE_CNT_GEN_CRS_DEL       = 67, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedCrsDeleted */
    BUNDLE_CNT_MAX_CRS_RATE_EXCD = 68, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountMaxCrsRateExceeded */
    BUNDLE_CNT_GEN_CRS           = 69, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedCrs */
    BUNDLE_CNT_RECV_CRS          = 70, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountReceivedCrs */
    BUNDLE_CNT_DEL_UNAUTH        = 71, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountDeletedUnauthorized */
    BUNDLE_CNT_GEN_ANON          = 72, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleCountGeneratedAnonymous */
    NODE_STARTUP_CNTR            = 73, /* BPLib_NodeMibCountersHkTlm_Payload_t::NodeStartupCounter */
    BUNDLE_AGT_ACCPT_CNT         = 74, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleAgentAcceptedDirectiveCount */
    BUNDLE_AGT_REJ_CNT           = 75, /* BPLib_NodeMibCountersHkTlm_Payload_t::BundleAgentRejectedDirectiveCount */
} BPLib_AS_Counter_t;

/* ======= */
/* Globals */
/* ======= */

extern BPLib_NodeMibCountersHkTlm_Payload_t    BPLib_AS_NodeCountersPayload;
extern BPLib_SourceMibCountersHkTlm_Payload_t  BPLib_AS_SourceCountersPayload;
extern BPLib_ChannelContactStatHkTlm_Payload_t BPLib_AS_ChannelContactStatsPayload;

extern BPLib_AS_Counter_t ResettableNodeCounters[BPLIB_AS_NUM_NODE_CNTRS];
extern BPLib_AS_Counter_t ResettableSourceCounters[BPLIB_AS_NUM_SRC_CNTRS];
extern BPLib_AS_Counter_t BundleCounters[BPLIB_AS_NUM_BNDL_CNTRS];
extern BPLib_AS_Counter_t ErrorCounters[BPLIB_AS_NUM_ERR_CNTRS];

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
 * \brief     Checks if the source EID matches an expected, valid pattern
 * \details   Eventually some more advanced checks will occur with something like regex.
 * \note      The source EID is not expected to be int16_t typed data but it is for now, as a placeholder
 * \param[in] SourceEid (int16_t) Indicator of which source in the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters
 *                      array should be modified
 * \return    Source EID validity
 * \retval    true: The source EID matches the criteria required to be valid <enter criteria here>
 * \retval    false: The source EID is invalid in some way
 */
bool BPLib_AS_EidIsValid(int16_t SourceEid);

/**
  * \brief      Allows the caller to get access to certain values in counter packets specified by the parameters.
  *             This is necessary to simplify the code and avoid walls of switch-case statements
  * \details    Accessor function for counters used by Admin Statistics
  * \note       This function assumes all modifiable counter types are uint32_t
  * \param[in]  SourceEid (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters
  *                       array. SourceEid of -1 (BPLIB_AS_NODE_EID) indicates that the node counter is the counter
  *                       whose value is to be returned.
  * \param[in]  Counter   (BPLib_AS_Counter_t) Counter to access
  * \param[out] ReturnPtr (uint32_t*) What the counter value requested is stored in
  * \return     Execution status
  * \retval     BPLIB_AS_INVALID_EID: Source EID is <= -2 or >= BPLIB_MAX_NUM_SOURCE_EID
  * \retval     BPLIB_AS_UNKNOWN_NODE_CNTR: The node-specific counter did not match a recognized value
  * \retval     BPLIB_AS_UNKNOWN_SRC_CNTR: The source-specific counter did not match a recognized value
  * \retval     BPLIB_SUCCESS: Successful execution
  */
BPLib_Status_t BPLib_AS_Get(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t* ReturnPtr);

/**
 * \brief     Modify the counter specified by the source EID and counter to the desired value
 * \details   Mutator function for counters used by Admin Statistics
 * \note      This function assumes all modifiable counters are of type uint32_t
 * \param[in] SourceEid (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters
 *                      array. SourceEid of -1 (BPLIB_AS_NODE_EID) indicates that the node counter is the counter
 *                      whose value is to be returned.
 * \param[in] Counter   (BPLib_AS_Counter_t) Counter to mutate
 * \param[in] Value     (uint32_t) Value that the caller wishes to set the counter represented by Counter and SourceEid
 * \return    Execution status
 * \retval    BPLIB_AS_INVALID_EID: Source EID is <= -2 or >= BPLIB_MAX_NUM_SOURCE_EID
 * \retval    BPLIB_AS_UNKNOWN_NODE_CNTR: The node-specific counter did not match a recognized value
 * \retval    BPLIB_AS_UNKNOWN_SRC_CNTR: The source-specific counter did not match a recognized value
 * \retval    BPLIB_SUCCESS: Successful execution
 */
BPLib_Status_t BPLib_AS_Set(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Value);

/**
 * \brief     Add 1 to the counter specified by the source EID and counter
 * \details   Incrementing function for counters used by Admin Statistics
 * \note      Internal uint32_t value is passed by reference when using BPLib_AS_Get()
 * \param[in] SourceEid (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters array.
 *                      SourceEid of -1 (BPLIB_AS_NODE_EID) indicates that the node counter is the counter whose value
 *                      is to be returned.
 * \param[in] Counter (BPLib_AS_Counter_t) Counter to increment
 * \return    Execution status
 * \retval    Status is determined by BPLib_AS_Get() and BPLib_AS_Set()
 * \retval    BPLIB_AS_INVALID_EID: Source EID is <= -2 or >= BPLIB_MAX_NUM_SOURCE_EID
 * \retval    BPLIB_AS_UNKNOWN_NODE_CNTR: The node-specific counter did not match a recognized value
 * \retval    BPLIB_AS_UNKNOWN_SRC_CNTR: The source-specific counter did not match a recognized value
 * \retval    BPLIB_SUCCESS: Successful execution
 */
BPLib_Status_t BPLib_AS_Increment(int16_t SourceEid, BPLib_AS_Counter_t Counter);

/**
 * \brief     Subtract 1 from the counter specified by the source EID and counter
 * \details   Decrementing function for counters used by Admin Statistics
 * \note      Uses void* to get and set values so some information could be lost
 * \param[in] SourceEid (int16_t) Indentifier used to determine the index into the
 *                      BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters array. SourceEid of -1
 *                      (BPLIB_AS_NODE_EID) indicates that the node counter is the counter whose value
 *                      is to be returned.
 * \param[in] Counter (BPLib_AS_Counter_t) Counter to increment
 * \return    Execution status
 * \retval    Status is determined by BPLib_AS_Get() and BPLib_AS_Set()
 * \retval    BPLIB_AS_INVALID_EID: Source EID is <= -2 or >= BPLIB_MAX_NUM_SOURCE_EID
 * \retval    BPLIB_AS_UNKNOWN_NODE_CNTR: The node-specific counter did not match a recognized value
 * \retval    BPLIB_AS_UNKNOWN_SRC_CNTR: The source-specific counter did not match a recognized value
 * \retval    BPLIB_SUCCESS: Successful execution
 */
BPLib_Status_t BPLib_AS_Decrement(int16_t SourceEid, BPLib_AS_Counter_t Counter);

/**
 * \brief     Set to zero all resettable MIB counters associated with the given source EID pattern
 * \details   See header file to see which counters are considered source counters (ResettableSourceCounters array)
 * \note      Cycles through source counters and uses BPLib_AS_Set() to reset them to 0
 * \param[in] SourceEid (int16_t) Indentifier used to determine the index into the
 *                      BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters array.
 * \return    Execution status
 * \retval    BPLIB_AS_INVALID_EID: Source EID is <= -1 or >= BPLIB_MAX_NUM_SOURCE_EID
 * \retval    BPLIB_AS_RESET_SRC_ERR: Something went wrong while running BPLib_AS_Set()
 * \retval    BPLIB_SUCCESS: Successful execution
 */
BPLib_Status_t BPLib_AS_ResetSourceCounters(int16_t SourceEid);

/**
 * \brief     Set to zero all resettable MIB counters associated with bundles
 * \details   See macros to see which counters are considered bundle counters
 * \note      Cycles through bundle-related source and node counters and uses BPLib_AS_Set() to reset them to 0
 * \param[in] SourceEid (int16_t) Indentifier used to determine the index into the
 *                      BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters array.
 * \return    Execution status
 * \retval    BPLIB_AS_INVALID_EID: Source EID is <= -2 or >= BPLIB_MAX_NUM_SOURCE_EID
 * \retval    BPLIB_AS_RESET_BNDL_ERR: Something went wrong while running BPLib_AS_Set()
 * \retval    BPLIB_SUCCESS: Successful execution
 */
BPLib_Status_t BPLib_AS_ResetBundleCounters(int16_t SourceEid);

/**
 * \brief     Set to zero all resettable MIB error counters
 * \details   See macros to see which counters are considered error counters
 * \note      Cycles through error counters and uses BPLib_AS_Set() to reset them to 0
 * \param[in] SourceEid (int16_t) Indentifier used to determine the index into the
 *                      BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters array.
 * \return    Execution status
 * \retval    BPLIB_AS_RESET_ERR_ERR: Something went wrong while running BPLib_AS_Set()
 * \retval    BPLIB_SUCCESS: Successful execution
 */
BPLib_Status_t BPLib_AS_ResetErrorCounters(int16_t SourceEid);

/**
 * \brief     Set every counter value in the source and node counter packets to zero
 * \details   Zeroing out function used by Admin Statistics
 * \note      Cycles through each possible source EID and possible counter then sets counters to 0 with BPLib_AS_Set()
 * \param[in] void No arguments accepted
 * \return    Execution status
 * \retval    BPLIB_AS_RESET_ALL_ERR: Something went wrong while running BPLib_AS_Set()
 * \retval    BPLIB_SUCCESS: Successful execution
 */
BPLib_Status_t BPLib_AS_ResetAllCounters(void);

/**
 * \brief     ---
 * \details   ---
 * \note      ---
 * \param[in] void No arguments accepted
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Operation was successful
 */
BPLib_Status_t BPLib_AS_Write(void);

/**
 * \brief     ---
 * \details   ---
 * \note      ---
 * \param[in] void No arguments accepted
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Operation was successful
 */
BPLib_Status_t BPLib_AS_Restore(void);

#endif /* BPLIB_AS_H */
