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
#include "bplib_fwp.h"
#include "bplib_time.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

BPLib_Status_t BPLib_AS_Init(void)
{
    BPLib_Status_t Status;
    BPLib_TIME_MonotonicTime_t Time;

    Status = BPLib_AS_InitMutex();

    /* Instantiate all payloads under the stewardship of AS */
    BPLib_AS_ResetAllCounters();
    BPLib_AS_InitializeReportsHkTlm();

    BPLib_TIME_GetMonotonicTime(&Time);

    (void) BPLib_AS_SetCounter(BPLIB_EID_INSTANCE, NODE_STARTUP_COUNTER, Time.BootEra);

    return Status;
}

void BPLib_AS_Increment(BPLib_EID_t EID, BPLib_AS_Counter_t Counter, uint32_t Amount)
{
    BPLib_Status_t Status;

    if (BPLib_EID_IsMatch(&EID, &BPLIB_EID_INSTANCE))
    { /* Increment a node counter */
        /* Prevent modification of counters from other tasks while modifying them */
        BPLib_AS_LockCounters();

        /* Use BPLib_AS_SetCounter to evaluate source MIB array index and counter ranges */
        Status = BPLib_AS_SetCounter(EID, Counter, BPLib_AS_NodeCountersPayload.NodeCounters[Counter] + Amount);
        if (Status != BPLIB_SUCCESS)
        {
            BPLib_EM_SendEvent(BPLIB_AS_SET_CTR_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Could not increment node counter %d to %d, RC = %d",
                                Counter,
                                BPLib_AS_NodeCountersPayload.NodeCounters[Counter] + Amount,
                                Status);
        }

        /* Allow counters to be modified by other tasks after operation has finished */
        BPLib_AS_UnlockCounters();
    }
    // else
    // { /* Increment a source counter */
    //     /* Prevent modification of counters from other tasks while modifying them */
    //     BPLib_AS_LockCounters();

    //     /* Allow counters to be modified by other tasks after operation has finished */
    //     BPLib_AS_UnlockCounters();
    // }
}

void BPLib_AS_Decrement(BPLib_EID_t EID, BPLib_AS_Counter_t Counter, uint32_t Amount)
{
    BPLib_Status_t Status;

    if (BPLib_EID_IsMatch(&EID, &BPLIB_EID_INSTANCE))
    { /* Increment a node counter */
        /* Prevent modification of counters from other tasks while modifying them */
        BPLib_AS_LockCounters();

        /* Use BPLib_AS_SetCounter to evaluate EID and counter ranges */
        Status = BPLib_AS_SetCounter(EID, Counter, BPLib_AS_NodeCountersPayload.NodeCounters[Counter] - Amount);
        if (Status != BPLIB_SUCCESS)
        {
            BPLib_EM_SendEvent(BPLIB_AS_SET_CTR_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Could not decrement node counter %d to %d, RC = %d",
                                Counter,
                                BPLib_AS_NodeCountersPayload.NodeCounters[Counter] - Amount,
                                Status);
        }

        /* Allow counters to be modified by other tasks after operation has finished */
        BPLib_AS_UnlockCounters();
    }
    // else
    // { /* Increment a source counter */
    //     /* Prevent modification of counters from other tasks while modifying them */
    //     BPLib_AS_LockCounters();

    //     /* Allow counters to be modified by other tasks after operation has finished */
    //     BPLib_AS_UnlockCounters();
    // }
}

BPLib_Status_t BPLib_AS_ResetCounter(uint16_t MibArrayIndex, BPLib_AS_Counter_t Counter)
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
    else if (MibArrayIndex < BPLIB_MAX_NUM_MIB_SETS)
    {
        if (Counter < BPLIB_AS_NUM_SOURCE_CNTRS)
        { // Counter is within range
            /* Prevent modification of counters while outputting */
            BPLib_AS_LockCounters();

            BPLib_AS_SourceCountersPayload.MibArray[MibArrayIndex].SourceCounters[Counter] = 0;

            /* Allow counters to be modified again */
            BPLib_AS_UnlockCounters();
        }
        else
        {
            Status = BPLIB_AS_UNKNOWN_SRC_CNTR;
        }
    }
    else
    {
        Status = BPLIB_AS_INVALID_MIB_INDEX;
    }

    return Status;
}

BPLib_Status_t BPLib_AS_ResetSourceCounters(uint16_t MibArrayIndex)
{
    BPLib_Status_t Status;

    /* Default to BPLIB_SUCCESS */
    Status = BPLIB_SUCCESS;

    if (MibArrayIndex < BPLIB_MAX_NUM_MIB_SETS)
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

BPLib_Status_t BPLib_AS_ResetErrorCounters(uint16_t MibArrayIndex)
{
    BPLib_Status_t Status;

    /* Default to BPLIB_SUCCESS */
    Status = BPLIB_SUCCESS;

    if (MibArrayIndex < BPLIB_MAX_NUM_MIB_SETS)
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

    for (SourceCtrl = 0; SourceCtrl < BPLIB_MAX_NUM_MIB_SETS; SourceCtrl++)
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

/* Send Node MIB Reports housekeeping telemetry */
BPLib_Status_t BPLib_AS_SendNodeMibReportsHk(void)
{
    return BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibReportsPkt(&BPLib_AS_NodeReportsPayload);
}

BPLib_Status_t BPLib_AS_AddMibArrayKey(const BPLib_EID_Pattern_t* EID_Patterns)
{
    BPLib_Status_t Status;
    uint8_t NumKeysGiven;
    uint8_t MibIndex;
    uint8_t PatternIndex;
    uint8_t InputIndex;
    BPLib_EID_Pattern_t CurrPattern;
    BPLib_EID_Pattern_t InputPattern;

    Status       = BPLIB_SUCCESS;
    NumKeysGiven = 0;

    for (InputIndex = 0; InputIndex < BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET; InputIndex++)
    { /* Loop through each given pattern */
        InputPattern = EID_Patterns[InputIndex];
        if (BPLib_EID_PatternIsValid(&InputPattern))
        { /* Given pattern is valid, see if it can be added to the MIB array */
            if (InputPattern.Scheme != BPLIB_EID_SCHEME_RESERVED)
            { /* The key provided has values */
                for (MibIndex = 0; MibIndex < BPLIB_MAX_NUM_MIB_SETS; MibIndex++)
                { /* Loop through every entry in the MIB key array */
                    for (PatternIndex = 0; PatternIndex < BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET; PatternIndex++)
                    { /* Loop through every pattern in the MIB key array entry */
                        CurrPattern = BPLib_AS_SourceCountersPayload.MibArray[MibIndex].EidPatterns[PatternIndex];

                        if (CurrPattern.Scheme != BPLIB_EID_SCHEME_RESERVED)
                        { /* A pattern was found for this MIB array entry */
                            /*
                            ** Check if the head or tail of the input pattern lies between the
                            ** head and tail of a key that is already present in the MIB key array
                            */
                            if ((InputPattern.MaxAllocator >= CurrPattern.MinAllocator  && /*  ------------------ */
                                 InputPattern.MaxAllocator <= CurrPattern.MaxAllocator) || /* | Check input key's */
                                (InputPattern.MinAllocator >= CurrPattern.MinAllocator  && /* | allocator range   */
                                 InputPattern.MinAllocator <= CurrPattern.MaxAllocator)    /*  ------------------ */
                                ||
                                (InputPattern.MaxNode      >= CurrPattern.MinNode       && /*  ------------------ */
                                 InputPattern.MaxNode      <= CurrPattern.MaxNode)      || /* | Check input key's */
                                (InputPattern.MinNode      >= CurrPattern.MinNode       && /* | node range        */
                                 InputPattern.MinNode      <= CurrPattern.MaxNode)         /*  ------------------ */
                                ||
                                (InputPattern.MaxService   >= CurrPattern.MinService    && /*  ------------------ */
                                 InputPattern.MaxService   <= CurrPattern.MaxService)   || /* | Check input key's */
                                (InputPattern.MinService   >= CurrPattern.MinService    && /* | service range     */
                                 InputPattern.MinService   <= CurrPattern.MaxService))     /*  ------------------ */
                            { /* An overlap was found */
                                Status = BPLIB_AS_MIB_KEYS_OVERLAP;

                                BPLib_EM_SendEvent(BPLIB_AS_ADD_MIB_ARRAY_KEY_DBG_EID,
                                                    BPLib_EM_EventType_DEBUG,
                                                    "MIB array key overlap found between input #%d and MIB #%d, active key #%d",
                                                    InputIndex,
                                                    MibIndex,
                                                    PatternIndex);

                                break;
                            }
                        }
                    }

                    if (Status != BPLIB_SUCCESS)
                    { /* Stop searching the existing keys when an overlap is found */
                        break;
                    }
                }

                if (Status == BPLIB_SUCCESS)
                {
                    NumKeysGiven++;
                }
            }
        }
        else
        {
            Status = BPLIB_INVALID_EID_PATTERN;

            BPLib_EM_SendEvent(BPLIB_AS_ADD_MIB_ARRAY_KEY_DBG_EID,
                                BPLib_EM_EventType_DEBUG,
                                "Input key #%d is an invalid EID pattern",
                                InputIndex);
        }

        if (Status != BPLIB_SUCCESS)
        { /* Leave the key comparison and verification loops when an error is found */
            break;
        }
    }

    if (Status == BPLIB_SUCCESS)
    { /* Add the key(s) */
        if (NumKeysGiven > 0)
        { /* At least 1 key was provided as input */
            /* Default to failure to save on extra logic */
            Status = BPLIB_AS_MIB_KEY_ARRAY_FULL;

            for (MibIndex = 0; MibIndex < BPLIB_MAX_NUM_MIB_SETS; MibIndex++)
            { /* Loop through every MIB array key entry */
                if ((BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET - BPLib_AS_SourceCountersPayload.MibArray[MibIndex].ActiveKeys) >= NumKeysGiven)
                { /* Space is available for the given key(s) to be  added */
                    for (PatternIndex = 0; PatternIndex < NumKeysGiven; PatternIndex++)
                    { /* Loop through input keys and add to MIB key array */
                        /* ActiveKeys also serves as the index of the next open key slot */
                        InputIndex = BPLib_AS_SourceCountersPayload.MibArray[MibIndex].ActiveKeys + PatternIndex;
                        BPLib_AS_SourceCountersPayload.MibArray[MibIndex].EidPatterns[InputIndex] = EID_Patterns[PatternIndex];
                    }

                    /* Update the number of active keys for the MIB array entry */
                    BPLib_AS_SourceCountersPayload.MibArray[MibIndex].ActiveKeys += NumKeysGiven;

                    /* Indicate that a key was added */
                    Status = BPLIB_SUCCESS;
                    break;
                }
            }
        }
        else
        {
            Status = BPLIB_AS_NO_KEYS_GIVEN;
        }
    }

    if (Status == BPLIB_AS_MIB_KEY_ARRAY_FULL)
    {
        BPLib_EM_SendEvent(BPLIB_AS_ADD_MIB_ARRAY_KEY_DBG_EID,
                            BPLib_EM_EventType_DEBUG,
                            "EID key array is full");
    }

    return Status;
}