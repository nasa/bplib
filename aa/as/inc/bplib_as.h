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

/** \brief Number of MIB counters used for looping */
#define BPLIB_AS_NUM_COUNTERS 76

/** \brief Node counter EID designator */
#define BPLIB_AS_NODE_EID -1

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
    BUNDLE_AGT_ACCPT_CNT         =  0, /** \brief Number of control directives received from the Monitor and Control interface that have been accepted */
    BUNDLE_AGT_REJ_CNT           =  1, /** \brief Number of control directives received from the Monitor and Control interface that have been rejected as being invalid */
    BUNDLE_CNT_GEN_ACCPT         =  2, /** \brief Number of Accepted Bundle Transmission Requests  */
    BUNDLE_CNT_GEN_REJ           =  3, /** \brief Number of Rejected Bundle Transmission Requests */
    BUNDLE_CNT_GEN_CUSTODY       =  4, /** \brief Number of Custody Signal Bundles generated since the last counter reset */
    BUNDLE_CNT_GEN_BSR_RECV      =  5, /** \brief Number of Bundle Reception Status Report generated since the last counter reset */
    BUNDLE_CNT_GEN_BSR_ACCPT     =  6, /** \brief Number of Bundle Custody Accepted Status Report generated since the last counter reset */
    BUNDLE_CNT_GEN_BSR_FORW      =  7, /** \brief Number of Bundle Forwarded Status Report generated since the last counter reset */
    BUNDLE_CNT_GEN_BSR_DELVR     =  8, /** \brief Number of Bundle Delivered Status Report generated since the last counter reset */
    BUNDLE_CNT_GEN_BSR_DEL       =  9, /** \brief Number of Bundle Deleted Status Report generated since the last counter reset */
    BUNDLE_CNT_MAX_BSR_RATE_EXCD = 10, /** \brief Number of BSR bundles not sent because sending would exceed a maximum rate. */
    BUNDLE_CNT_GEN_ANON          = 11, /** \brief Number of Anonomous Bundles Created */
    BUNDLE_CNT_GEN_FRAG          = 12, /** \brief Number of Bundle Fragments that were Generated */
    BUNDLE_CNT_RECV              = 13, /** \brief Number of Bundles Received from another DTN Node */
    BUNDLE_CNT_RECV_FRAG         = 14, /** \brief Number of Bundles Received that were Marked as Fragments */
    BUNDLE_CNT_UNPROC_BLKS       = 15, /** \brief Number of Unprocessed Blocks Removed from Received Bundles */
    BUNDLE_CNT_INVAL_PRI_BLK     = 16, /** \brief Number of Unprocessed Bundles received with Invalid Primary Blocks */
    BUNDLE_CNT_CS_RECV           = 17, /** \brief Number of Custody Signal Bundles received */
    BUNDLE_CNT_FORW              = 18, /** \brief Number of Bundles Forwarded to another DTN Node */
    BUNDLE_CNT_RET               = 19, /** \brief Number of Bundles Returned to Sender */
    BUNDLE_CNT_FRAG              = 20, /** \brief Number of Bundles that needed to be Fragmented  */
    BUNDLE_CNT_REASSEMBLED       = 21, /** \brief Total number of Bundles delivered that were fragments and needed to be reassembled */
    BUNDLE_CNT_FRAG_ERR          = 22, /** \brief Number of Fragments discarded due to bad offset or ADU length */
    BUNDLE_CNT_DELVR             = 23, /** \brief Total number of Bundles Delivered to this node, including fragments */
    BUNDLE_CNT_ABAND             = 24, /** \brief Number of Abandoned Bundle Payloads */
    ADU_CNT_DELVR                = 25, /** \brief Number of ADUs Delivered to the application */
    BUNDLE_CNT_DEL               = 26, /** \brief Total Number of Bundle Deletions */
    BUNDLE_CNT_DEL_EXP           = 27, /** \brief Number of Bundles Deletions due to Lifetime Expired Condition */
    BUNDLE_CNT_DEL_HOP_EXCD      = 28, /** \brief Number of Bundles Deletions due to Hop Limit Exceeded Condition */
    BUNDLE_CNT_DEL_INVAL_PAY     = 29, /** \brief Number of Bundle Deletions due having a Corrupted Payload Block */
    BUNDLE_CNT_DEL_FORW_FAILED   = 30, /** \brief Number of Bundles Deletions due to Forwarding Failed Condition */
    BUNDLE_CNT_DEL_TRAF_PARED    = 31, /** \brief Number of Bundles Deletions due to Traffic Pared Condition */
    BUNDLE_CNT_DEL_UNINTEL       = 32, /** \brief Number of Bundles Deletions due to Block Unintelligible Condition */
    BUNDLE_CNT_DEL_UNSUPPORT_BLK = 33, /** \brief Number of Bundles Deletions due to Unsupported Block Condition */
    BUNDLE_CNT_DEL_CANCELLED     = 34, /** \brief Number of Bundles Deletions due to Transmission Cancelled Condition */
    BUNDLE_CNT_DISCARDED         = 35, /** \brief Number of Bundles Discarded */
    BUNDLE_CNT_DEL_NO_STOR       = 36, /** \brief Number of Bundles deleted due to insufficient storage */
    BUNDLE_CNT_DEL_BAD_EID       = 37, /** \brief Number of Bundles deleted due to having a unrecognized destination EID */
    BUNDLE_CNT_DEL_UNAUTH        = 38, /** \brief Number of Bundles deleted due to having a unrecognized source EID. Incremented if the bundle is not in the set of authorized source EIDs configured for the node. */
    BUNDLE_CNT_DEL_TOO_LONG      = 39, /** \brief Number of Bundles deleted due to being longer than paramSetMaxBundleLength */
    BUNDLE_CNT_CUSTODY_TRANS     = 40, /** \brief Number of successful Custody Transfers from the Local node to the next neighboring Custodian Node */
    BUNDLE_CNT_CUSTODY_REJ       = 41, /** \brief Number of unsuccessful Custody Transfers from the Local node to the next neighboring Custodian Node */
    BUNDLE_CNT_CUSTODY_REQ       = 42, /** \brief Number of bundles received that are requesting Custody Transfer */
    BUNDLE_CNT_CUSTODY_RE_FORW   = 43, /** \brief Number of Bundles reforward due to custody timeout */
    BUNDLE_CNT_NO_FURTHER_INFO   = 44, /** \brief Number of bundles for which successful Custody Signals generated with No Further Information */
    BUNDLE_CNT_RED               = 45, /** \brief Number of bundles for which successful Custody Signals generated for Duplicate Bundle reception */
    BUNDLE_CNT_DEPLETED          = 46, /** \brief Number of bundles for which rejected Custody Signals generated indicating rejection due to depleted storage */
    BUNDLE_CNT_UNINTEL_EID       = 47, /** \brief Number of bundles for which rejected Custody Signals generated indicating the any EID in the Primary Header is unknown */
    BUNDLE_CNT_NO_ROUTE          = 48, /** \brief Number of bundles for which rejected Custody Signals generated indicating the Destination is not reachable */
    BUNDLE_CNT_NO_CONTACT        = 49, /** \brief Number of bundles for which rejected Custody Signals generated indicating the Destination is not reachable before the Bundle expires */
    BUNDLE_CNT_UNINTEL_BLK       = 50, /** \brief Number of bundles for which Custody Signals indicating the Bundle contained an unknown block type */
    BUNDLE_CNT_RECV_CS           = 51, /** \brief Number of bundles for which Custody Signals Received */
    BUNDLE_CNT_GEN_CS            = 52, /** \brief Number of bundles for which Custody Signals Generated */
    BUNDLE_CNT_REJ_CUSTODY       = 53, /** \brief Number of Bundles where this node rejected custody. */
    BUNDLE_CNT_GEN_CRS_RECV      = 54, /** \brief Number of received bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    BUNDLE_CNT_GEN_CRS_ACCPT     = 55, /** \brief Number of accepted bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    BUNDLE_CNT_GEN_CRS_FORW      = 56, /** \brief Number of forwarded bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    BUNDLE_CNT_GEN_CRS_DELVR     = 57, /** \brief Number of delivered bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    BUNDLE_CNT_GEN_CRS_DEL       = 58, /** \brief Number of deleted bundle reports included in each generated Compressed Reporting Signal (CRS) since the last counter reset */
    BUNDLE_CNT_MAX_CRS_RATE_EXCD = 59, /** \brief Number of CRS bundles not sent because sending would exceed a maximum rate. */
    ADU_CNT_RECV                 = 60, /** \brief Number of ADUs Received from the application */
    NODE_STARTUP_CNTR            = 61, /** \brief Number of times a node is started up. */
    BUNDLE_CNT_GEN_CRS           = 62, /** \brief Number of Compressed Reporting Signal (CRS) generated since last counter reset. */
    BUNDLE_CNT_FORW_FAILED       = 63, /** \brief Number of Bundles where forwarding to another DTN Node failed */
    BUNDLE_CNT_RECV_BSR_RECV     = 64, /** \brief Number of Bundle Reception Status Report received since the last counter reset */
    BUNDLE_CNT_RECV_BSR_ACCPT    = 65, /** \brief Number of Bundle Custody Accepted Status Report received since the last counter reset */
    BUNDLE_CNT_RECV_BSR_FORW     = 66, /** \brief Number of Bundle Forwarded Status Report received since the last counter reset */
    BUNDLE_CNT_RECV_BSR_DELVR    = 67, /** \brief Number of Bundle Delivered Status Report received since the last counter reset */
    BUNDLE_CNT_RECV_BSR_DEL      = 68, /** \brief Number of Bundle Deleted Status Report received since the last counter reset */
    BUNDLE_CNT_RECV_CRS          = 69, /** \brief Number of Compressed Reporting Signals (CRSs) received since last counter reset. */
    BUNDLE_CNT_RECV_CRS_RECV     = 70, /** \brief Number of received bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of received bundles per source node ID. */
    BUNDLE_CNT_RECV_CRS_ACCPT    = 71, /** \brief Number of accepted bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of accepted bundles per source node ID. */
    BUNDLE_CNT_RECV_CRS_FORW     = 72, /** \brief Number of forwarded bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of forwarded bundles per source node ID. */
    BUNDLE_CNT_RECV_CRS_DELVR    = 73, /** \brief Number of delivered bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of delivered bundles per source node ID. */
    BUNDLE_CNT_RECV_CRS_DEL      = 74, /** \brief Number of deleted bundle reports included in received Compressed Reporting Signals (CRSs) since the last counter reset. Also includes total number of deleted bundles per source node ID. */
    BUNDLE_CNT_RECV_ADMIN_REC    = 75, /** \brief Number of admin record bundles received for this DTN Node. */
} BPLib_AS_Counter_t;

/* ======= */
/* Externs */
/* ======= */

extern BPLib_NodeMibCountersHkTlm_Payload_t    BPLib_AS_NodeCountersPayload;
extern BPLib_SourceMibCountersHkTlm_Payload_t  BPLib_AS_SourceCountersPayload;
extern BPLib_ChannelContactStatHkTlm_Payload_t BPLib_AS_ChannelContactStatsPayload;

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
  * \note      ReturnPtr is void* since the data type of the values in the packets can vary
  * \param[in] SourceEid (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters
  *                      array. SourceEid of -1 (BPLIB_AS_NODE_EID) indicates that the node counter is the counter
  *                      whose value is to be returned.
  * \param[in] Counter (BPLib_AS_Counter_t) Counter to access
  * \param[in] ReturnPtr (void*) What the counter value requested is store in
  * \return    Execution status
  * \retval    BPLIB_AS_INVALID_CNTR: Source EID is <= -2 or >= BPLIB_MAX_NUM_SOURCE_EID
  * \retval    BPLIB_AS_SOURCE_EID_MISSING: Source EID was -1 but the counter requested isn't a node-only counter
  * \retval    BPLIB_AS_UNKNOWN_CNTR: The requested counter isn't a counter tracked by sources
  * \retval    BPLIB_SUCCESS: Successful execution
  */
BPLib_Status_t BPLib_AS_Get(int16_t SourceEid, BPLib_AS_Counter_t Counter, void* ReturnPtr);

/**
 * \brief     Modify the counter specified by the source EID and counter to the desired value
 * \details   Mutator function for counters used by Admin Statistics
 * \note      Casts void* to the appropriate data type so it's possible some data could be truncated.
 *            void* is used since the data type of the value in the packet can vary
 * \param[in] SourceEid       (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters
  *                           array. SourceEid of -1 (BPLIB_AS_NODE_EID) indicates that the node counter is the counter
  *                           whose value is to be returned.
 * \param[in] Counter         (BPLib_AS_Counter_t) Counter to mutate
 * \param[in] DesiredValuePtr (void*) Value that the caller wishes to set the counter represented by Counter and optionally SourceEid
 * \return    Execution status
 * \retval    BPLIB_AS_INVALID_CNTR: Source EID is <= -2 or >= BPLIB_MAX_NUM_SOURCE_EID
 * \retval    BPLIB_AS_SOURCE_EID_MISSING: Source EID was -1 but the counter requested isn't a node-only counter
 * \retval    BPLIB_AS_UNKNOWN_CNTR: The requested counter isn't a counter tracked by sources
 * \retval    BPLIB_SUCCESS: Successful execution
 */
BPLib_Status_t BPLib_AS_Set(int16_t SourceEid, BPLib_AS_Counter_t Counter, void* DesiredValuePtr);

/**
 * \brief     Add 1 to the counter specified by the source EID and counter
 * \details   Incrementing function for counters used by Admin Statistics
 * \note      Uses void* to get and set values so some information could be lost
 * \param[in] SourceEid (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters array.
 *                      SourceEid of -1 (BPLIB_AS_NODE_EID) indicates that the node counter is the counter whose value
 *                      is to be returned.
 * \param[in] Counter (BPLib_AS_Counter_t) Counter to increment
 * \return    Execution status
 * \retval    Status is determined by BPLib_AS_Get() and BPLib_AS_Set()
 * \retval    BPLIB_AS_INVALID_CNTR: Source EID is <= -2 or >= BPLIB_MAX_NUM_SOURCE_EID
 * \retval    BPLIB_AS_SOURCE_EID_MISSING: Source EID was -1 but the counter requested isn't a node-only counter
 * \retval    BPLIB_AS_UNKNOWN_CNTR: The requested counter isn't a counter tracked by sources
 * \retval    BPLIB_SUCCESS: Successful execution
 */
BPLib_Status_t BPLib_AS_Increment(int16_t SourceEid, BPLib_AS_Counter_t Counter);

/**
 * \brief     Subtract 1 from the counter specified by the source EID and counter
 * \details   Decrementing function for counters used by Admin Statistics
 * \note      Uses void* to get and set values so some information could be lost
 * \param[in] SourceEid (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters array.
 *                      SourceEid of -1 (BPLIB_AS_NODE_EID) indicates that the node counter is the counter whose value
 *                      is to be returned.
 * \param[in] Counter (BPLib_AS_Counter_t) Counter to increment
 * \return    Execution status
 * \retval    Status is determined by BPLib_AS_Get() and BPLib_AS_Set()
 * \retval    BPLIB_AS_INVALID_CNTR: Source EID is <= -2 or >= BPLIB_MAX_NUM_SOURCE_EID
 * \retval    BPLIB_AS_SOURCE_EID_MISSING: Source EID was -1 but the counter requested isn't a node-only counter
 * \retval    BPLIB_AS_UNKNOWN_CNTR: The requested counter isn't a counter tracked by sources
 * \retval    BPLIB_SUCCESS: Successful execution
 */
BPLib_Status_t BPLib_AS_Decrement(int16_t SourceEid, BPLib_AS_Counter_t Counter);

/**
 * \brief     Set every counter value in the source and node counter packets to zero
 * \details   Zeroing out function used by Admin Statistics
 * \note      Cycles through each possible source EID and possible counter then sets counters to 0 with BPLib_AS_Set()
 * \param[in] void No arguments accepted
 * \return    Execution status
 * \retval    Status is determined by BPLib_AS_Set()
 * \retval    BPLIB_AS_INVALID_CNTR: Source EID is <= -2 or >= BPLIB_MAX_NUM_SOURCE_EID
 * \retval    BPLIB_AS_SOURCE_EID_MISSING: Source EID was -1 but the counter requested isn't a node-only counter
 * \retval    BPLIB_AS_UNKNOWN_CNTR: The requested counter isn't a counter tracked by sources
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
