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

#ifndef BPLIB_AS_INTERNAL_H
#define BPLIB_AS_INTERNAL_H

/* ======== */
/* Includes */
/* ======== */

#include "bplib_api_types.h"

/* ======= */
/* Globals */
/* ======= */

/**
  * \brief  Used to access counters in the node and source payload structs
  * \anchor BPLib_AS_Counter_t
  */
typedef enum
{
    /* Counters in node and source packets */
    ADU_COUNT_DELIVERED                    = 0 , /** \brief BPLib_AS_NodeCounters.AduCountDelivered                  | BPLib_AS_SourceCounters.SourceCounters[X].AduCountDelivered */
    ADU_COUNT_RECEIVED                     = 1 , /** \brief BPLib_AS_NodeCounters.AduCountReceived                   | BPLib_AS_SourceCounters.SourceCounters[X].AduCountReceived */
    BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT  = 2 , /** \brief BPLib_AS_NodeCounters.BundleAgentAcceptedDirectiveCount  | BPLib_AS_SourceCounters.SourceCounters[X].BundleAgentAcceptedDirectiveCount */
    BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT  = 3 , /** \brief BPLib_AS_NodeCounters.BundleAgentRejectedDirectiveCount  | BPLib_AS_SourceCounters.SourceCounters[X].BundleAgentRejectedDirectiveCount */
    BUNDLE_COUNT_ABANDONED                 = 4 , /** \brief BPLib_AS_NodeCounters.BundleCountAbandoned               | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountAbandoned */
    BUNDLE_COUNT_CUSTODY_RE_FORWARDED      = 5 , /** \brief BPLib_AS_NodeCounters.BundleCountCustodyReForwarded      | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountCustodyReForwarded */
    BUNDLE_COUNT_CUSTODY_REJECTED          = 6 , /** \brief BPLib_AS_NodeCounters.BundleCountCustodyRejected         | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountCustodyRejected */
    BUNDLE_COUNT_CUSTODY_REQUEST           = 7 , /** \brief BPLib_AS_NodeCounters.BundleCountCustodyRequest          | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountCustodyRequest */
    BUNDLE_COUNT_CUSTODY_SIGNAL_RECEIVED   = 8 , /** \brief BPLib_AS_NodeCounters.BundleCountCustodySignalReceived   | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountCustodySignalReceived */
    BUNDLE_COUNT_CUSTODY_TRANSFERRED       = 9 , /** \brief BPLib_AS_NodeCounters.BundleCountCustodyTransferred      | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountCustodyTransferred */
    BUNDLE_COUNT_DELETED                   = 10, /** \brief BPLib_AS_NodeCounters.BundleCountDeleted                 | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDeleted */
    BUNDLE_COUNT_DELETED_BAD_EID           = 11, /** \brief BPLib_AS_NodeCounters.BundleCountDeletedBadEid           | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDeletedBadEid */
    BUNDLE_COUNT_DELETED_CANCELLED         = 12, /** \brief BPLib_AS_NodeCounters.BundleCountDeletedCancelled        | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDeletedCancelled */
    BUNDLE_COUNT_DELETED_EXPIRED           = 13, /** \brief BPLib_AS_NodeCounters.BundleCountDeletedExpired          | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDeletedExpired */
    BUNDLE_COUNT_DELETED_FORWARD_FAILED    = 14, /** \brief BPLib_AS_NodeCounters.BundleCountDeletedForwardFailed    | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDeletedForwardFailed */
    BUNDLE_COUNT_DELETED_HOP_EXCEEDED      = 15, /** \brief BPLib_AS_NodeCounters.BundleCountDeletedHopExceeded      | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDeletedHopExceeded */
    BUNDLE_COUNT_DELETED_INVALID_PAYLOAD   = 16, /** \brief BPLib_AS_NodeCounters.BundleCountDeletedInvalidPayload   | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDeletedInvalidPayload */
    BUNDLE_COUNT_DELETED_NO_STORAGE        = 17, /** \brief BPLib_AS_NodeCounters.BundleCountDeletedNoStorage        | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDeletedNoStorage */
    BUNDLE_COUNT_DELETED_TOO_LONG          = 18, /** \brief BPLib_AS_NodeCounters.BundleCountDeletedTrafficPared     | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDeletedTrafficPared */
    BUNDLE_COUNT_DELETED_TRAFFIC_PARED     = 19, /** \brief BPLib_AS_NodeCounters.BundleCountDeletedUnauthorized     | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDeletedUnauthorized */
    BUNDLE_COUNT_DELETED_UNAUTHORIZED      = 20, /** \brief BPLib_AS_NodeCounters.BundleCountDeletedUnintelligible   | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDeletedUnintelligible */
    BUNDLE_COUNT_DELETED_UNINTELLIGIBLE    = 21, /** \brief BPLib_AS_NodeCounters.BundleCountDeletedUnsupportedBlock | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDeletedUnsupportedBlock */
    BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK = 22, /** \brief BPLib_AS_NodeCounters.BundleCountDeletedTooLong          | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDeletedTooLong */
    BUNDLE_COUNT_DELIVERED                 = 23, /** \brief BPLib_AS_NodeCounters.BundleCountDelivered               | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDelivered */
    BUNDLE_COUNT_DEPLETED                  = 24, /** \brief BPLib_AS_NodeCounters.BundleCountDepleted                | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDepleted */
    BUNDLE_COUNT_DISCARDED                 = 25, /** \brief BPLib_AS_NodeCounters.BundleCountDiscarded               | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountDiscarded */
    BUNDLE_COUNT_FORWARDED                 = 26, /** \brief BPLib_AS_NodeCounters.BundleCountForwarded               | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountForwarded */
    BUNDLE_COUNT_FORWARDED_FAILED          = 27, /** \brief BPLib_AS_NodeCounters.BundleCountForwardedFailed         | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountForwardedFailed */
    BUNDLE_COUNT_FRAGMENT_ERROR            = 28, /** \brief BPLib_AS_NodeCounters.BundleCountFragmentError           | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountFragmentError */
    BUNDLE_COUNT_FRAGMENTED                = 29, /** \brief BPLib_AS_NodeCounters.BundleCountFragmented              | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountFragmented */
    BUNDLE_COUNT_GENERATED_ACCEPTED        = 30, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedAccepted       | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountGeneratedAccepted */
    BUNDLE_COUNT_GENERATED_ANONYMOUS       = 31, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedAnonymous      | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountGeneratedAnonymous */
    BUNDLE_COUNT_GENERATED_BSR_ACCEPTED    = 32, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedBsrAccepted */
    BUNDLE_COUNT_GENERATED_BSR_DELETED     = 33, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedBsrDeleted */
    BUNDLE_COUNT_GENERATED_BSR_DELIVERED   = 34, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedBsrDelivered */
    BUNDLE_COUNT_GENERATED_BSR_FORWARDED   = 35, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedBsrForwarded */
    BUNDLE_COUNT_GENERATED_BSR_RECEIVED    = 36, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedBsrReceived */
    BUNDLE_COUNT_GENERATED_CRS             = 37, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedCrs */
    BUNDLE_COUNT_GENERATED_CRS_ACCEPTED    = 38, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedCrsAccepted */
    BUNDLE_COUNT_GENERATED_CRS_DELETED     = 39, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedCrsDeleted */
    BUNDLE_COUNT_GENERATED_CRS_DELIVERED   = 40, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedCrsDelivered */
    BUNDLE_COUNT_GENERATED_CRS_FORWARDED   = 41, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedCrsForwarded */
    BUNDLE_COUNT_GENERATED_CRS_RECEIVED    = 42, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedCrsReceived */
    BUNDLE_COUNT_GENERATED_CUSTODY         = 43, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedCustody */
    BUNDLE_COUNT_GENERATED_CUSTODY_SIGNAL  = 44, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedCustodySignal  | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountGeneratedCustodySignal */
    BUNDLE_COUNT_GENERATED_FRAGMENT        = 45, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedFragment       | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountGeneratedFragment */
    BUNDLE_COUNT_GENERATED_REJECTED        = 46, /** \brief BPLib_AS_NodeCounters.BundleCountGeneratedRejected       | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountGeneratedRejected */
    BUNDLE_COUNT_IN_CUSTODY                = 47, /** \brief BPLib_AS_NodeCounters.BundleCountInCustody               | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountInCustody */
    BUNDLE_COUNT_INVALID_PRIMARY_BLOCK     = 48, /** \brief BPLib_AS_NodeCounters.BundleCountInvalidPrimaryBlock     | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountInvalidPrimaryBlock */
    BUNDLE_COUNT_MAX_BSR_RATE_EXCEEDED     = 49, /** \brief BPLib_AS_NodeCounters.BundleCountMaxBsrRateExceeded      | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountMaxBsrRateExceeded */
    BUNDLE_COUNT_MAX_CRS_RATE_EXCEEDED     = 50, /** \brief BPLib_AS_NodeCounters.BundleCountMaxCrsRateExceeded */
    BUNDLE_COUNT_NO_CONTACT                = 51, /** \brief BPLib_AS_NodeCounters.BundleCountNoContact               | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountNoContact */
    BUNDLE_COUNT_NO_FURTHER_INFO           = 52, /** \brief BPLib_AS_NodeCounters.BundleCountNoFurtherInfo           | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountNoFurtherInfo */
    BUNDLE_COUNT_NO_ROUTE                  = 53, /** \brief BPLib_AS_NodeCounters.BundleCountNoRoute                 | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountNoRoute */
    BUNDLE_COUNT_REASSEMBLED               = 54, /** \brief BPLib_AS_NodeCounters.BundleCountReassembled             | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReassembled */
    BUNDLE_COUNT_RECEIVED                  = 55, /** \brief BPLib_AS_NodeCounters.BundleCountReceived                | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReceived */
    BUNDLE_COUNT_RECEIVED_ADMIN_RECORD     = 56, /** \brief BPLib_AS_NodeCounters.BundleCountReceivedAdminRecord     | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReceivedAdminRecord */
    BUNDLE_COUNT_RECEIVED_BSR_ACCEPTED     = 57, /** \brief BPLib_AS_NodeCounters.BundleCountReceivedBsrAccepted     | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReceivedBsrAccepted */
    BUNDLE_COUNT_RECEIVED_BSR_DELETED      = 58, /** \brief BPLib_AS_NodeCounters.BundleCountReceivedBsrDeleted      | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReceivedBsrDeleted */
    BUNDLE_COUNT_RECEIVED_BSR_DELIVERED    = 59, /** \brief BPLib_AS_NodeCounters.BundleCountReceivedBsrDelivered    | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReceivedBsrDelivered */
    BUNDLE_COUNT_RECEIVED_BSR_FORWARDED    = 60, /** \brief BPLib_AS_NodeCounters.BundleCountReceivedBsrForwarded    | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReceivedBsrForwarded */
    BUNDLE_COUNT_RECEIVED_BSR_RECEIVED     = 61, /** \brief BPLib_AS_NodeCounters.BundleCountReceivedBsrReceived     | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReceivedBsrReceived */
    BUNDLE_COUNT_RECEIVED_CRS              = 62, /** \brief BPLib_AS_NodeCounters.BundleCountReceivedCrs             | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReceivedCrs */
    BUNDLE_COUNT_RECEIVED_CRS_ACCEPTED     = 63, /** \brief BPLib_AS_NodeCounters.BundleCountReceivedCrsAccepted     | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReceivedCrsAccepted */
    BUNDLE_COUNT_RECEIVED_CRS_DELETED      = 64, /** \brief BPLib_AS_NodeCounters.BundleCountReceivedCrsDeleted      | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReceivedCrsDeleted */
    BUNDLE_COUNT_RECEIVED_CRS_DELIVERED    = 65, /** \brief BPLib_AS_NodeCounters.BundleCountReceivedCrsDelivered    | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReceivedCrsDelivered */
    BUNDLE_COUNT_RECEIVED_CRS_FORWARDED    = 66, /** \brief BPLib_AS_NodeCounters.BundleCountReceivedCrsForwarded    | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReceivedCrsForwarded */
    BUNDLE_COUNT_RECEIVED_CRS_RECEIVED     = 67, /** \brief BPLib_AS_NodeCounters.BundleCountReceivedCrsReceived     | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReceivedCrsReceived */
    BUNDLE_COUNT_RECEIVED_CUSTODY_SIGNAL   = 68, /** \brief BPLib_AS_NodeCounters.BundleCountReceivedCustodySignal   | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReceivedCustodySignal */
    BUNDLE_COUNT_RECEIVED_FRAGMENT         = 69, /** \brief BPLib_AS_NodeCounters.BundleCountReceivedFragment        | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReceivedFragment */
    BUNDLE_COUNT_REDUNDANT                 = 70, /** \brief BPLib_AS_NodeCounters.BundleCountRedundant               | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountRedundant */
    BUNDLE_COUNT_REJECTED_CUSTODY          = 71, /** \brief BPLib_AS_NodeCounters.BundleCountRejectedCustody         | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountRejectedCustody */
    BUNDLE_COUNT_RETURNED                  = 72, /** \brief BPLib_AS_NodeCounters.BundleCountReturned                | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountReturned */
    BUNDLE_COUNT_STORAGE_AVAILABLE         = 73, /** \brief BPLib_AS_NodeCounters.BundleCountStoreageAvailable       | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountStoreageAvailable */
    BUNDLE_COUNT_STORED                    = 74, /** \brief BPLib_AS_NodeCounters.BundleCountStored                  | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountStored */
    BUNDLE_COUNT_UNINTELLIGIBLE_BLOCK      = 75, /** \brief BPLib_AS_NodeCounters.BundleCountUnintelligibleBlock     | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountUnintelligibleBlock */
    BUNDLE_COUNT_UNINTELLIGIBLE_EID        = 76, /** \brief BPLib_AS_NodeCounters.BundleCountUnintelligibleEid       | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountUnintelligibleEid */
    BUNDLE_COUNT_UNPROCESSED_BLOCKS        = 77, /** \brief BPLib_AS_NodeCounters.BundleCountUnprocessedBlocks       | BPLib_AS_SourceCounters.SourceCounters[X].BundleCountUnprocessedBlocks */
    NODE_STARTUP_COUNTER                   = 78, /** \brief BPLib_AS_NodeCounters.NodeStartupCounter                 | BPLib_AS_SourceCounters.SourceCounters[X].NodeStartupCounter */
} BPLib_AS_Counter_t;

/* =================== */
/* Function Prototypes */
/* =================== */

/**
 * \brief     Checks if the source EID matches an expected, valid pattern
 * \details   Eventually some more advanced checks will occur with something like regex.
 * \note      The source EID is not expected to be int16_t typed data but it is for now, as a placeholder
 * \param[in] SourceEid (int16_t) Indicator of which source in the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters
 *                      array should be modified
 * \return    Source EID validity
 * \retval    true: The source EID matches the criteria required to be valid <enter criteria here>
 * \retval    false: The source EID is invalid in some way
 * \secreflist
 * \refitem   BPLib_SourceMibCountersHkTlm_Payload_t
 * \endsecreflist
 * \anchor    BPLib_AS_EidIsValid
 */
bool BPLib_AS_EidIsValid(int16_t SourceEid);

/**
  * \brief      Allows the caller to get access to certain values in counter packets specified by the parameters.
  *             This is necessary to simplify the code and avoid walls of switch-case statements
  * \details    Accessor function for counters used by Admin Statistics
  * \note       This function assumes all modifiable counter types are uint32_t
  * \note       This function does not handle error events since it's entirely internal
  * \param[in]  SourceEid (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters
  *                       array.
  * \param[in]  Counter   (BPLib_AS_Counter_t) Counter to access
  * \param[out] ReturnPtr (uint32_t*) What the counter value requested is stored in
  * \return     Execution status
  * \retval     BPLIB_AS_INVALID_EID: Source EID did not pass criteria in BPLib_AS_EidIsValid()
  * \retval     BPLIB_AS_UNKNOWN_NODE_CNTR: The node-specific counter did not match a recognized value
  * \retval     BPLIB_AS_UNKNOWN_SRC_CNTR: The source-specific counter did not match a recognized value
  * \retval     BPLIB_SUCCESS: Successful execution
  * \secreflist
  * \refitem   BPLib_SourceMibCountersHkTlm_Payload_t
  * \refitem   BPLib_AS_Counter_t
  * \refitem   BPLib_AS_EidIsValid [BPLib_AS_EidIsValid()]
  * \endsecreflist
  * \anchor     BPLib_AS_Get
  */
BPLib_Status_t BPLib_AS_Get(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t* ReturnPtr);

/**
 * \brief     Modify the counter specified by the source EID and counter to the desired value
 * \details   Mutator function for counters used by Admin Statistics
 * \note      This function assumes all modifiable counters are of type uint32_t
 * \note      This function does not handle error events since it's internal
 * \param[in] SourceEid (int16_t) Index into the BPLib_SourceMibCountersHkTlm_Payload_t::SourceCounters array.
 * \param[in] Counter   (BPLib_AS_Counter_t) Counter to set
 * \param[in] Value     (uint32_t) Value that the caller wishes to set the counter represented by Counter and SourceEid
 * \return    Execution status
 * \retval    BPLIB_AS_INVALID_EID: Source EID did not pass criteria in BPLib_AS_EidIsValid()
 * \retval    BPLIB_AS_UNKNOWN_NODE_CNTR: The node-specific counter did not match a recognized value
 * \retval    BPLIB_AS_UNKNOWN_SRC_CNTR: The source-specific counter did not match a recognized value
 * \retval    BPLIB_SUCCESS: Successful execution
 * \secreflist
 * \refitem   BPLib_SourceMibCountersHkTlm_Payload_t
 * \refitem   BPLib_AS_Counter_t
 * \refitem   BPLib_AS_EidIsValid [BPLib_AS_EidIsValid()]
 * \endsecreflist
 * \anchor    BPLib_AS_Set
 */
BPLib_Status_t BPLib_AS_Set(int16_t SourceEid, BPLib_AS_Counter_t Counter, uint32_t Value);

#endif // BPLIB_AS_INTERNAL_H