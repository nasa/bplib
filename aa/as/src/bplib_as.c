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

#include "bplib_as.h"
#include "bplib_as_internal.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

BPLib_Status_t BPLib_AS_Init(void)
{
    BPLib_Status_t Status;

    Status = BPLib_AS_InitMutex();

    /* Instantiate all payloads under the stewardship of AS */
    BPLib_AS_ResetAllCounters();

    return Status;
}

void BPLib_AS_Increment(BPLib_EID_t EID, BPLib_AS_Counter_t Counter, uint32_t Amount)
{
    BPLib_Status_t Status;
    uint8_t MibArrayIndex;

    if (BPLib_EID_IsMatch(EID, BPLIB_EID_INSTANCE))
    { /* Increment a node counter */
        /* Prevent modification of counters from other tasks while modifying them */
        BPLib_AS_LockCounters();

        /* Use BPLib_AS_SetCounter to evaluate source MIB array index and counter ranges */
        Status = BPLib_AS_SetCounter(EID, Counter, BPLib_AS_NodeCountersPayload.NodeCounters[Counter] + Amount);
        if (Status != BPLIB_SUCCESS)
        {
            BPLib_EM_SendEvent(BPLIB_AS_SET_CTR_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Could not set node counter %d to %d, RC = %d",
                                Counter,
                                BPLib_AS_NodeCountersPayload.NodeCounters[Counter] + Amount,
                                Status);
        }

        /* Allow counters to be modified by other tasks after operation has finished */
        BPLib_AS_UnlockCounters();
    }
    else
    { /* Increment a source counter */
        /* Prevent modification of counters from other tasks while modifying them */
        BPLib_AS_LockCounters();

        /* Allow counters to be modified by other tasks after operation has finished */
        BPLib_AS_UnlockCounters();
    }
}

void BPLib_AS_Decrement(BPLib_EID_t EID, BPLib_AS_Counter_t Counter, uint32_t Amount)
{
    BPLib_Status_t Status;
    uint8_t MibArrayIndex;

    if (BPLib_EID_IsMatch(EID, BPLIB_EID_INSTANCE))
    { /* Increment a node counter */
        /* Prevent modification of counters from other tasks while modifying them */
        BPLib_AS_LockCounters();

        /* Use BPLib_AS_SetCounter to evaluate EID and counter ranges */
        Status = BPLib_AS_SetCounter(EID, Counter, BPLib_AS_NodeCountersPayload.NodeCounters[Counter] - Amount);
        if (Status != BPLIB_SUCCESS)
        {
            BPLib_EM_SendEvent(BPLIB_AS_SET_CTR_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Could not set counter %d to %d, RC = %d",
                                Counter,
                                BPLib_AS_NodeCountersPayload.NodeCounters[Counter] - Amount,
                                Status);
        }

        /* Allow counters to be modified by other tasks after operation has finished */
        BPLib_AS_UnlockCounters();
    }
    else
    { /* Increment a source counter */
        /* Prevent modification of counters from other tasks while modifying them */
        BPLib_AS_LockCounters();

        /* Allow counters to be modified by other tasks after operation has finished */
        BPLib_AS_UnlockCounters();
    }
}

BPLib_Status_t BPLib_AS_ResetCounter(uint8_t MibArrayIndex, BPLib_AS_Counter_t Counter)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (MibArrayIndex == BPLIB_AS_NODE_CNTR_INDICATOR)
    {
        if (Counter < BPLIB_AS_NUM_NODE_CNTRS)
        { // Counter is within range
            /* Prevent modification of counters while outputting */
            BPLib_AS_LockCounters();

            BPLib_AS_NodeCountersPayload.NodeCounters[Counter] = 0;

            /* Allow counters to be modified again */
            BPLib_AS_UnlockCounters();
        }
        else
        { // Counter is out of valid range
            Status = BPLIB_AS_UNKNOWN_NODE_CNTR;
        }
    }
    else
    {
        if (Counter < BPLIB_AS_NUM_SOURCE_CNTRS)
        { // Counter is within range
            if (MibArrayIndex < BPLIB_MAX_NUM_SOURCE_EID)
            {
                /* Prevent modification of counters while outputting */
                BPLib_AS_LockCounters();

                BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters[Counter] = 0;

                /* Allow counters to be modified again */
                BPLib_AS_UnlockCounters();
            }
            else
            {
                Status = BPLIB_AS_INVALID_MIB_INDEX;
            }
        }
        else
        {
            Status = BPLIB_AS_UNKNOWN_SRC_CNTR;
        }
    }

    return Status;
}

BPLib_Status_t BPLib_AS_ResetSourceCounters(uint8_t MibArrayIndex)
{
    BPLib_Status_t Status;

    /* Default to BPLIB_SUCCESS */
    Status = BPLIB_SUCCESS;

    if (MibArrayIndex < BPLIB_MAX_NUM_SOURCE_EID)
    {
        /* Prevent modification of counters from other tasks while modifying them */
        BPLib_AS_LockCounters();

        memset((void*) &BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters,
                0, sizeof(BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters));

        /* Allow counters to be modified by other tasks after operation has finished */
        BPLib_AS_UnlockCounters();
    }
    else
    {
        Status = BPLIB_AS_INVALID_MIB_INDEX;
    }

    return Status;
}

void BPLib_AS_ResetBundleCounters()
{
    /* Prevent modification of counters from other tasks while modifying them */
    BPLib_AS_LockCounters();

    /* Only reset bundle-related node counters */
    BPLib_AS_NodeCountersPayload.NodeCounters[ADU_COUNT_DELIVERED]                    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[ADU_COUNT_RECEIVED]                     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_ABANDONED]                 = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_RE_FORWARDED]      = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REJECTED]          = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REQUEST]           = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_TRANSFERRED]       = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED]                   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_EXPIRED]           = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_HOP_EXCEEDED]      = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_INVALID_PAYLOAD]   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_NO_STORAGE]        = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_TOO_LONG]          = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_TRAFFIC_PARED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNAUTHORIZED]      = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNINTELLIGIBLE]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK] = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELIVERED]                 = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DEPLETED]                  = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DISCARDED]                 = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FORWARDED]                 = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FORWARDED_FAILED]          = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENT_ERROR]            = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENTED]                = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_ACCEPTED]        = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CUSTODY_SIGNAL]  = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_FRAGMENT]        = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_REJECTED]        = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_MAX_BSR_RATE_EXCEEDED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_NO_FURTHER_INFO]           = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REASSEMBLED]               = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED]                  = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_ACCEPTED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELETED]      = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELIVERED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_FORWARDED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_RECEIVED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_ACCEPTED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_DELETED]      = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_DELIVERED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_FORWARDED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS_RECEIVED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CUSTODY_SIGNAL]   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_FRAGMENT]         = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REDUNDANT]                 = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REJECTED_CUSTODY]          = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNINTELLIGIBLE_BLOCK]      = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNINTELLIGIBLE_EID]        = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNPROCESSED_BLOCKS]        = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_SIGNAL_RECEIVED]   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_ACCEPTED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_DELETED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_DELIVERED]   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_FORWARDED]   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_BSR_RECEIVED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS]             = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_ACCEPTED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_DELETED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_DELIVERED]   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_FORWARDED]   = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CRS_RECEIVED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_CUSTODY]         = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_INVALID_PRIMARY_BLOCK]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_MAX_BSR_RATE_EXCEEDED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_MAX_CRS_RATE_EXCEEDED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_NO_FURTHER_INFO]           = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REASSEMBLED]               = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED]                  = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_ACCEPTED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELETED]      = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_DELIVERED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_FORWARDED]    = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_BSR_RECEIVED]     = 0;
    BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_RECEIVED_CRS]              = 0;

    /* Allow counters to be modified by other tasks after operation has finished */
    BPLib_AS_UnlockCounters();
}

BPLib_Status_t BPLib_AS_ResetErrorCounters(uint8_t MibArrayIndex)
{
    BPLib_Status_t Status;

    /* Default to BPLIB_SUCCESS */
    Status = BPLIB_SUCCESS;

    if (MibArrayIndex < BPLIB_MAX_NUM_SOURCE_EID)
    { /* Valid MIB source counter array index */
        /* Prevent modification of counters from other tasks while modifying them */
        BPLib_AS_LockCounters();

        /* Reset the error counters associated with nodes and sources */
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_ABANDONED]                                             = 0;
        BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters[BUNDLE_COUNT_ABANDONED]                 = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_CUSTODY_REJECTED]                                      = 0;
        BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters[BUNDLE_COUNT_CUSTODY_REJECTED]          = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_BAD_EID]                                       = 0;
        BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters[BUNDLE_COUNT_DELETED_BAD_EID]           = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_INVALID_PAYLOAD]                               = 0;
        BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters[BUNDLE_COUNT_DELETED_INVALID_PAYLOAD]   = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_NO_STORAGE]                                    = 0;
        BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters[BUNDLE_COUNT_DELETED_NO_STORAGE]        = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_TOO_LONG]                                      = 0;
        BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters[BUNDLE_COUNT_DELETED_TOO_LONG]          = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNAUTHORIZED]                                  = 0;
        BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters[BUNDLE_COUNT_DELETED_UNAUTHORIZED]      = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNINTELLIGIBLE]                                = 0;
        BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters[BUNDLE_COUNT_DELETED_UNINTELLIGIBLE]    = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK]                             = 0;
        BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters[BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK] = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_FRAGMENT_ERROR]                                        = 0;
        BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters[BUNDLE_COUNT_FRAGMENT_ERROR]            = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_GENERATED_REJECTED]                                    = 0;
        BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters[BUNDLE_COUNT_GENERATED_REJECTED]        = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_REJECTED_CUSTODY]                                      = 0;
        BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters[BUNDLE_COUNT_REJECTED_CUSTODY]          = 0;

        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_UNPROCESSED_BLOCKS]                                    = 0;
        BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters[BUNDLE_COUNT_UNPROCESSED_BLOCKS]        = 0;

        /* Reset the error counters that are node-only */
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT]                              = 0;
        BPLib_AS_NodeCountersPayload.NodeCounters[BUNDLE_COUNT_INVALID_PRIMARY_BLOCK]                                 = 0;

        /* Allow counters to be modified by other tasks after operation has finished */
        BPLib_AS_UnlockCounters();
    }
    else
    { /* Invalid MIB source counter array index */
        Status = BPLIB_AS_INVALID_MIB_INDEX;
    }

    return Status;
}

void BPLib_AS_ResetAllCounters(void)
{
    int16_t SourceCtrl;

    /* Prevent modification of counters from other tasks while modifying them */
    BPLib_AS_LockCounters();

    memset((void*) BPLib_AS_NodeCountersPayload.NodeCounters, 0,
            sizeof(uint32_t) * BPLIB_AS_NUM_NODE_CNTRS);

    for (SourceCtrl = 0; SourceCtrl < BPLIB_MAX_NUM_SOURCE_EID; SourceCtrl++)
    {
        memset((void*) BPLib_AS_SourceCountersPayload.MibArray[SourceCtrl].SourceCounters, 0,
                sizeof(uint32_t) * BPLIB_AS_NUM_SOURCE_CNTRS);
    }

    /* Allow counters to be modified by other tasks after operation has finished */
    BPLib_AS_UnlockCounters();
}

BPLib_Status_t BPLib_AS_SendNodeMibCountersHk()
{
    BPLib_Status_t Status;

    /* Prevent modification of counters from other tasks while modifying them */
    BPLib_AS_LockCounters();

    Status = BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibCounterPkt(&BPLib_AS_NodeCountersPayload);

    /* Allow counters to be modified by other tasks after operation has finished */
    BPLib_AS_UnlockCounters();

    return Status;
}

BPLib_Status_t BPLib_AS_SendSourceMibCountersHk()
{
    BPLib_Status_t Status;

    /* Prevent modification of counters from other tasks while modifying them */
    BPLib_AS_LockCounters();

    Status = BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibCounterPkt(&BPLib_AS_SourceCountersPayload);

    /* Allow counters to be modified by other tasks after operation has finished */
    BPLib_AS_UnlockCounters();

    return Status;
}

BPLib_Status_t BPLib_AS_AddMibArrayKey(BPLib_EID_Pattern_t EID_Pattern)
{
    BPLib_Status_t Status;
    bool Overlaps;
    uint16_t HasPattern;
    uint8_t MibIndex;
    uint8_t PatternIndex;
    BPLib_EID_Pattern_t CurrPattern;

    Status     = BPLIB_SUCCESS;
    Overlaps   = false;
    HasPattern = 0;

    if (BPLib_EID_PatternIsValid(EID_Pattern))
    { /* The given EID pattern is valid */
        for (MibIndex = 0; MibIndex < BPLIB_MAX_NUM_SOURCE_EID; MibIndex++)
        { /* Loop through every entry in the MIB key array */
            for (PatternIndex = 0; PatternIndex < BPLIB_MAX_MIB_ARRAY_KEYS; PatternIndex++)
            { /* Loop through every pattern in the MIB key array entry */    
                CurrPattern = BPLib_AS_SourceCountersPayload.MibArray[MibIndex].EidPatterns[PatternIndex];
                
                if (CurrPattern.Scheme != BPLIB_EID_SCHEME_RESERVED)
                { /* A pattern was found for this MIB array entry */
                    Overlaps = (EID_Pattern.MaxAllocator >= CurrPattern.MinAllocator ||
                                EID_Pattern.MinAllocator <= CurrPattern.MaxAllocator ||
                                EID_Pattern.MaxNode      >= CurrPattern.MinNode      ||
                                EID_Pattern.MinNode      <= CurrPattern.MaxNode      ||
                                EID_Pattern.MaxService   >= CurrPattern.MinService   ||
                                EID_Pattern.MinService   <= CurrPattern.MaxService);

                    if (Overlaps)
                    { /* An overlap was detected */
                        Status = BPLIB_AS_MIB_KEYS_OVERLAP;
                        break;
                    }
                    else
                    { /* An entry with a pattern and no overlap was found, so track that */
                        HasPattern |= (1 << MibIndex);
                    }
                }
            }

            if (Overlaps)
            { /* An overlap in MIB arrray keys was found and the status needs to be reported */
                break;
            }

        }

        if (!Overlaps)
        { /* Add a key to an array entry if such an entry exists */
            for (MibIndex = 0; MibIndex < BPLIB_MAX_NUM_SOURCE_EID; MibIndex++)
            { /* Cycle through every bit of HasPattern, searching for a 0 */
                if ((HasPattern & 1) == 0)
                { /* Bit 0 is a 0, so the entry in the MIB key array is empty */
                    BPLib_AS_SourceCountersPayload.MibArray[MibIndex].EidPatterns[0] = EID_Pattern;
                    break;
                }

                /* Examine the next bit in the bit string */
                HasPattern >>= 1;
            }

            if (MibIndex == BPLIB_MAX_NUM_SOURCE_EID)
            { /* The above loop ran to completion without finding an indication that a spot is open */
                Status = BPLIB_AS_MIB_KEY_ARRAY_FULL;
            }
        }
    }
    else
    { /* Given EID pattern is invalid */
        Status = BPLIB_INVALID_EID_PATTERN;
    }

    return Status;
}