
#include "bplib_cbor_private.h"

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
    const uint32_t MAX_CANONICAL_BLOCKS = BPLIB_MAX_NUM_EXTENSION_BLOCKS + 1;

    if ((CandBundle == NULL) || (bundle == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* A CandBundleLen less than 2 implies empty contents. */
    if (CandBundleLen <= 2)
    {
        return BPLIB_CBOR_DEC_ERR;
    }

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

    /* Init QCBOR Decode Engine with the Candidate Bundle */
    UBufC.ptr = (const void*)((uint8_t*)CandBundle);
    UBufC.len = CandBundleLen;
    QCBORDecode_Init(&ctx, UBufC, QCBOR_DECODE_MODE_NORMAL);


    /* Enter Array */
    QCBORDecode_EnterArray(&ctx, &OuterArr);
    QStatus = QCBORDecode_GetError(&ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_ERR;
    }

    /* First, parse the primary block */
    Status = BPLib_CBOR_DecodePrimary(&ctx, bundle);
    if (Status != BPLIB_SUCCESS)
    {
        return Status;
    }

    /*
    ** Iterate over each canonical block 
    ** Note: This is an indefinite array, so we're forced to use a loop with breakout approach.
    */
    for (CanonicalBlockIndex = 0; CanonicalBlockIndex < MAX_CANONICAL_BLOCKS; CanonicalBlockIndex++)
    {
        /* If we're out of elements, break */
        NextElementType = QCBORDecode_PeekNext(&ctx, &PeekItem);
        if (NextElementType == QCBOR_ERR_NO_MORE_ITEMS)
        {
            break;
        }

        /* Decode the next canonical block */
        Status = BPLib_CBOR_DecodeCanonical(&ctx, bundle, CanonicalBlockIndex);
        if (Status != BPLIB_SUCCESS)
        {
            break;
        }
    };

    /* sanity check that we didn't have extra data left after decoding the canonical blocks */
    NextElementType = QCBORDecode_PeekNext(&ctx, &PeekItem);
    if (NextElementType != QCBOR_ERR_NO_MORE_ITEMS)
    {
        return BPLIB_CBOR_DEC_PAST_MAX_BLOCKS;
    }

    QCBORDecode_ExitArray(&ctx);
    QStatus = QCBORDecode_GetError(&ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_ERR;
    }

    return Status;
}
