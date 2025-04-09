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

#include "bplib_cbor_internal.h"

#include <qcbor/qcbor_decode.h>
#include <qcbor/qcbor_spiffy_decode.h>

#include <stdio.h>

#define CBOR_INDEF_ARR_START          ((uint8_t)0x9F)
#define CBOR_INDEF_ARR_END            ((uint8_t)0xFF)


/*******************************************************************************
* Exported Functions
*/

BPLib_Status_t BPLib_CBOR_DecodeBundle(const void* CandBundle, size_t CandBundleLen, BPLib_Bundle_t* bundle)
{
    BPLib_Status_t Status;
    QCBORDecodeContext ctx;
    QCBORError QStatus;
    QCBORError NextElementType;
    QCBORItem OuterArr;
    QCBORItem PeekItem;
    UsefulBufC UBufC;
    uint32_t CanonicalBlockIndex = 0;

    if ((CandBundle == NULL) || (bundle == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    // TODO figure out minimum length
    /* A CandBundleLen less than 2 implies empty contents. */
    if (CandBundleLen <= 2)
    {
        return BPLIB_CBOR_DEC_BUNDLE_TOO_SHORT_ERR;
    }

    /* Verify bundle is not longer than maximum allowed length */
    BPLib_NC_ReaderLock();
    if (CandBundleLen > BPLib_NC_ConfigPtrs.MibPnConfigPtr->ParamSetMaxBundleLength)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED_TOO_LONG, 1);
        return BPLIB_CBOR_DEC_BUNDLE_TOO_LONG_DEC_ERR;
    }
    BPLib_NC_ReaderUnlock();

    #if (BPLIB_CBOR_DEBUG_PRINTS_ENABLED)
    printf("Candidate bundle received with size %lu: \n", CandBundleLen);
    for (size_t i = 0 ; i < CandBundleLen; i++)
    {
        printf("0x%02x, ", ((uint8_t*)CandBundle)[i]);
        if (((i+1) % 8) == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
    #endif

    /* Init QCBOR Decode Engine with the Candidate Bundle */
    UBufC.ptr = (const void*)((uint8_t*)CandBundle);
    UBufC.len = CandBundleLen;
    QCBORDecode_Init(&ctx, UBufC, QCBOR_DECODE_MODE_NORMAL);

    /* Enter Array */
    QCBORDecode_EnterArray(&ctx, &OuterArr);
    QStatus = QCBORDecode_GetError(&ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_BUNDLE_ENTER_ARRAY_ERR;
    }

    /* First, parse the primary block */
    Status = BPLib_CBOR_DecodePrimary(&ctx, bundle, CandBundle);
    if (Status != BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_INVALID_PRIMARY_BLOCK, 1);
        return Status;
    }

    /*
    ** Iterate over each canonical block 
    ** Note: This is an indefinite array, so we're forced to use a loop with breakout approach.
    */
    for (CanonicalBlockIndex = 0; CanonicalBlockIndex < BPLIB_MAX_NUM_CANONICAL_BLOCKS; CanonicalBlockIndex++)
    {
        /* If we're out of elements, break */
        NextElementType = QCBORDecode_PeekNext(&ctx, &PeekItem);
        if (NextElementType == QCBOR_ERR_NO_MORE_ITEMS)
        {
            break;
        }

        /* Decode the next canonical block */
        Status = BPLib_CBOR_DecodeCanonical(&ctx, bundle, CanonicalBlockIndex, CandBundle);
        if (Status != BPLIB_SUCCESS)
        {
            break;
        }
    };

    QCBORDecode_ExitArray(&ctx);
    QStatus = QCBORDecode_GetError(&ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_BUNDLE_EXIT_ARRAY_ERR;
    }

    return Status;
}
