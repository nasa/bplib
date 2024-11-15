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

/* ======== */
/* Includes */
/* ======== */

#include "bplib_as_test_utils.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

void Test_BPLib_AS_SetSourceCounterValues(int32_t SourceEid, uint32_t SetValue)
{
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountDelivered                  = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountReceived                   = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountAbandoned               = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyReForwarded      = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected         = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRequest          = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyTransferred      = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeleted                 = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedBadEid           = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedCancelled        = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedExpired          = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedForwardFailed    = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedHopExceeded      = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedInvalidPayload   = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedNoStorage        = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTooLong          = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTrafficPared     = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnauthorized     = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnintelligible   = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnsupportedBlock = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDelivered               = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDepleted                = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDiscarded               = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwarded               = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwardedFailed         = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmentError           = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmented              = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted       = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedCustodySignal  = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedFragment       = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedRejected       = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountMaxBsrRateExceeded      = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoContact               = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoFurtherInfo           = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoRoute                 = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReassembled             = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceived                = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedAdminRecord     = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrAccepted     = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDeleted      = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDelivered    = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrForwarded    = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrReceived     = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsAccepted     = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDeleted      = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDelivered    = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsForwarded    = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsReceived     = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCustodySignal   = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedFragment        = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRedundant               = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRejectedCustody         = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReturned                = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleBlock     = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleEid       = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnprocessedBlocks       = SetValue;

}

void Test_BPLib_AS_SourceCountersValueTest(int32_t SourceEid, uint32_t ActualValue)
{
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountDelivered);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountReceived);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountAbandoned);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyReForwarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRequest);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyTransferred);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeleted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedBadEid);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedCancelled);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedExpired);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedForwardFailed);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedHopExceeded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedInvalidPayload);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedNoStorage);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTooLong);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTrafficPared);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnauthorized);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnintelligible);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnsupportedBlock);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDelivered);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDepleted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDiscarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwardedFailed);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmentError);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmented);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedCustodySignal);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedFragment);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedRejected);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountMaxBsrRateExceeded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoContact);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoFurtherInfo);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoRoute);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReassembled);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceived);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedAdminRecord);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrAccepted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDeleted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDelivered);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrForwarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrReceived);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsAccepted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDeleted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDelivered);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsForwarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsReceived);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCustodySignal);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedFragment);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRedundant);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRejectedCustody);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReturned);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleBlock);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleEid);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnprocessedBlocks);
}

void Test_BPLib_AS_SetBundleCounterValues(int32_t SourceEid, uint32_t SetValue)
{
    BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrAccepted  = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDeleted   = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDelivered = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrForwarded = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrReceived  = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrs          = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsAccepted  = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDeleted   = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDelivered = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsForwarded = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsReceived  = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustody      = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountInvalidPrimaryBlock   = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountMaxCrsRateExceeded    = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountReceivedCrs           = SetValue;

    BPLib_AS_NodeCountersPayload.AduCountDelivered                                              = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountDelivered                  = SetValue;

    BPLib_AS_NodeCountersPayload.AduCountReceived                                               = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountReceived                   = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountAbandoned                                           = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountAbandoned               = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountCustodyReForwarded                                  = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyReForwarded      = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected                                     = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected         = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountCustodyRequest                                      = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRequest          = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountCustodyTransferred                                  = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyTransferred      = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeleted                                             = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeleted                 = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeletedExpired                                      = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedExpired          = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeletedHopExceeded                                  = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedHopExceeded      = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeletedInvalidPayload                               = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedInvalidPayload   = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeletedNoStorage                                    = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedNoStorage        = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeletedTooLong                                      = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTooLong          = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeletedTrafficPared                                 = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTrafficPared     = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeletedUnintelligible                               = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnintelligible   = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeletedUnsupportedBlock                             = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnsupportedBlock = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDelivered                                           = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDelivered               = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDepleted                                            = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDepleted                = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDiscarded                                           = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDiscarded               = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountForwarded                                           = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwarded               = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountForwardedFailed                                     = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwardedFailed         = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountFragmentError                                       = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmentError           = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountFragmented                                          = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmented              = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted                                   = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted       = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustodySignal                              = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedCustodySignal  = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountGeneratedFragment                                   = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedFragment       = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountGeneratedRejected                                   = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedRejected       = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountMaxBsrRateExceeded                                  = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountMaxBsrRateExceeded      = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountNoFurtherInfo                                       = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoFurtherInfo           = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountReassembled                                         = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReassembled             = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountReceived                                            = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceived                = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrAccepted                                 = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrAccepted     = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDeleted                                  = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDeleted      = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDelivered                                = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDelivered    = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrForwarded                                = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrForwarded    = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrReceived                                 = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrReceived     = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsAccepted                                 = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsAccepted     = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDeleted                                  = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDeleted      = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDelivered                                = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDelivered    = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsForwarded                                = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsForwarded    = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsReceived                                 = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsReceived     = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountReceivedCustodySignal                               = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCustodySignal   = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountReceivedFragment                                    = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedFragment        = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountRedundant                                           = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRedundant               = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountRejectedCustody                                     = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRejectedCustody         = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleBlock                                 = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleBlock     = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleEid                                   = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleEid       = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountUnprocessedBlocks                                   = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnprocessedBlocks       = SetValue;
}

void Test_BPLib_AS_BundleCountersValueTest(int32_t SourceEid, uint32_t ActualValue)
{
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountCustodySignalReceived);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrAccepted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDeleted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrDelivered);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrForwarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedBsrReceived);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrs);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsAccepted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDeleted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsDelivered);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsForwarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedCrsReceived);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustody);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountInvalidPrimaryBlock);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountMaxCrsRateExceeded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountReceivedCrs);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.AduCountDelivered);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountDelivered);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.AduCountReceived);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].AduCountReceived);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountAbandoned);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountAbandoned);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountCustodyReForwarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyReForwarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountCustodyRequest);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRequest);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountCustodyTransferred);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyTransferred);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeleted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeleted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedExpired);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedExpired);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedHopExceeded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedHopExceeded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedInvalidPayload);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedInvalidPayload);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedNoStorage);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedNoStorage);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedTooLong);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTooLong);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedTrafficPared);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTrafficPared);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedUnintelligible);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnintelligible);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedUnsupportedBlock);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnsupportedBlock);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDelivered);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDelivered);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDepleted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDepleted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDiscarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDiscarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountForwarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountForwardedFailed);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountForwardedFailed);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountFragmentError);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmentError);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountFragmented);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmented);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedAccepted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedAccepted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedCustodySignal);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedCustodySignal);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedFragment);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedFragment);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedRejected);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedRejected);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountMaxBsrRateExceeded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountMaxBsrRateExceeded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountNoFurtherInfo);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountNoFurtherInfo);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountReassembled);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReassembled);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountReceived);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceived);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrAccepted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrAccepted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDeleted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDeleted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrDelivered);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrDelivered);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrForwarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrForwarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountReceivedBsrReceived);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedBsrReceived);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsAccepted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsAccepted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDeleted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDeleted);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsDelivered);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsDelivered);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsForwarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsForwarded);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountReceivedCrsReceived);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCrsReceived);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountReceivedCustodySignal);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedCustodySignal);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountReceivedFragment);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountReceivedFragment);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountRedundant);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRedundant);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountRejectedCustody);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRejectedCustody);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleBlock);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleBlock);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountUnintelligibleEid);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnintelligibleEid);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountUnprocessedBlocks);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnprocessedBlocks);
}

void Test_BPLib_AS_SetErrorCounterValues(int32_t SourceEid, uint32_t SetValue)
{
    BPLib_AS_NodeCountersPayload.BundleAgentRejectedDirectiveCount = SetValue;
    BPLib_AS_NodeCountersPayload.BundleCountInvalidPrimaryBlock    = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountAbandoned                                           = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountAbandoned               = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected                                     = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected         = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeletedBadEid                                       = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedBadEid           = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeletedInvalidPayload                               = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedInvalidPayload   = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeletedNoStorage                                    = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedNoStorage        = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeletedTooLong                                      = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTooLong          = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeletedUnauthorized                                 = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnauthorized     = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeletedUnintelligible                               = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnintelligible   = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountDeletedUnsupportedBlock                             = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnsupportedBlock = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountFragmentError                                       = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmentError           = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountGeneratedRejected                                   = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedRejected       = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountRejectedCustody                                     = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRejectedCustody         = SetValue;

    BPLib_AS_NodeCountersPayload.BundleCountUnprocessedBlocks                                   = SetValue;
    BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnprocessedBlocks       = SetValue;
}

void Test_BPLib_AS_ErrorCountersValueTest(int32_t SourceEid, uint32_t ActualValue)
{
    /* Assert that node error counter are the actual value */
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleAgentRejectedDirectiveCount);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountInvalidPrimaryBlock);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountAbandoned);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountAbandoned);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountCustodyRejected);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountCustodyRejected);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedBadEid);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedBadEid);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedInvalidPayload);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedInvalidPayload);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedNoStorage);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedNoStorage);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedTooLong);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedTooLong);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedUnauthorized);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnauthorized);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedUnintelligible);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnintelligible);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountDeletedUnsupportedBlock);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountDeletedUnsupportedBlock);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountFragmentError);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountFragmentError);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountGeneratedRejected);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountGeneratedRejected);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountRejectedCustody);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountRejectedCustody);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_NodeCountersPayload.BundleCountUnprocessedBlocks);
    UtAssert_EQ(uint32_t, ActualValue, BPLib_AS_SourceCountersPayload.SourceCounters[SourceEid].BundleCountUnprocessedBlocks);
}

void BPLib_AS_Test_Setup(void)
{
    /* Initialize the node counter payload to 0s */
    memset((void*) &BPLib_AS_NodeCountersPayload, 0, sizeof(BPLib_AS_NodeCountersPayload));

    /* Initialize the source counter payload to 0s */
    memset((void*) &BPLib_AS_SourceCountersPayload, 0, sizeof(BPLib_AS_SourceCountersPayload));

    /* Initialize test environment to default state for every test */
    UT_ResetState(0);
}

void BPLib_AS_Test_Teardown(void)
{
    /* Clean up test environment */
}

void UtTest_Setup(void)
{
    TestBplibAs_Register();
}
