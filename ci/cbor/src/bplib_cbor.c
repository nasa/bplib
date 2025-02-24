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

/*
** Include
*/

#include <stdio.h>

#include "bplib_cbor.h"

#include "qcbor/qcbor_encode.h"
#include "qcbor/qcbor_decode.h"


// Local prototype for the simple QCBOR test function.
int32_t BPLib_CBOR_SimpleValuesTest1(void);


/*
 ** Macros
 */

#define CheckResults(Enc, Expected) \
   UsefulBuf_Compare(Enc, (UsefulBufC){Expected, sizeof(Expected)})

// One big buffer that can be used by any test in this file.

static uint8_t spBigBuf[2200];

/*
   85               # array(5)
   F5               # primitive(21)
   F4               # primitive(20)
   F6               # primitive(22)
   F7               # primitive(23)
   A1               # map(1)
      65            # text(5)
         554E446566 # "UNDef"
      F7            # primitive(23)
 */
static const uint8_t spExpectedEncodedSimple[] = {
   0x85, 0xf5, 0xf4, 0xf6, 0xf7, 0xa1, 0x65, 0x55, 0x4e, 0x44, 0x65, 0x66, 0xf7};

/*
** Function Definitions
*/

void BPLib_CBOR_DecodeAgeData(QCBORDecodeContext *DecodeCtx, BPLib_Bundle_t *Bundle)
{

}

BPLib_Status_t BPLib_CBOR_DecodeBundle(QCBORDecodeContext *DecodeCtx,
                                       BPLib_Bundle_t *CandidateBundle,
                                       size_t CandidateBundleLen)
{
    BPLib_Status_t Status;
    uint8_t* CandidateBytes;

    UsefulBufC UBufC;

    if (CandidateBundle == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }
    if (CandidateBundleLen <= 2)
    {
        return BPLIB_CBOR_DEC_ERR;
    }

    /* Check for CBOR Indefinite Array manually: this avoids a layer of QCBOR calls. */
    CandidateBytes = (uint8_t*)(CandidateBundle);
    if ((CandidateBytes[0] != 0x9F) || (CandidateBytes[CandidateBundleLen - 1] != 0xFF))
    {
        return BPLIB_CBOR_DEC_ERR;
    }

    /* Init QCBOR Decode Engine with the Candidate Bundle */
    UBufC.ptr = (const void*)((uint8_t*)CandidateBundle + 1);
    UBufC.len = CandidateBundleLen - 2;
    QCBORDecode_Init(DecodeCtx, UBufC, QCBOR_DECODE_MODE_NORMAL);

    Status = BPLib_CBOR_PrimaryBlockParse(DecodeCtx);
    if (Status != BPLIB_SUCCESS)
    {
        return Status;
    }

    return BPLIB_SUCCESS;

}

BPLib_Status_t BPLib_CBOR_DecodeCanonical(BPLib_Bundle_t *Bundle, const void *data_ptr, size_t data_size)
{

    return BPLIB_SUCCESS;
}

void BPLib_CBOR_DecodeHopCountData(QCBORDecodeContext *DecodeCtx, BPLib_Bundle_t *Bundle)
{

}

BPLib_Status_t BPLib_CBOR_DecodePayload(BPLib_Bundle_t *Bundle, const void *data_ptr, size_t data_size)
{

    return BPLIB_SUCCESS;
}

void BPLib_CBOR_DecodePrevNodeData(QCBORDecodeContext *DecodeCtx, BPLib_Bundle_t *Bundle)
{

}

void BPLib_CBOR_DecodePrimary(QCBORDecodeContext *DecodeCtx, PrimaryBlock_t *v)
{

}

void BPLib_CBOR_EncodeAgeData(QCBOREncodeContext *EncodeCtx, BPLib_Bundle_t *Bundle)
{

}

void BPLib_CBOR_EncodeBundle(QCBOREncodeContext *EncodeCtx, BPLib_Bundle_t *Bundle)
{

}

void BPLib_CBOR_EncodeCRC(QCBOREncodeContext *enc)
{

}

BPLib_Status_t BPLib_CBOR_EncodeCanonical(BPLib_Bundle_t *Bundle)
{

    return BPLIB_SUCCESS;
}

void BPLib_CBOR_EncodeHopCountData(QCBOREncodeContext *EncodeCtx, BPLib_Bundle_t *Bundle)
{

}

BPLib_Status_t BPLib_CBOR_EncodePayload(BPLib_Bundle_t *Bundle, const void *data_ptr, size_t data_size)
{

    return BPLIB_SUCCESS;
}

void BPLib_CBOR_EncodePrevNodeData(QCBOREncodeContext *EncodeCtx, BPLib_Bundle_t *Bundle)
{

}

void BPLib_CBOR_EncodePrimary(QCBOREncodeContext *EncodeCtx, BPLib_Bundle_t *Bundle)
{

}

BPLib_Status_t BPLib_CBOR_Init(void) {
    int32_t Status;

    Status = BPLib_CBOR_SimpleValuesTest1();

    if (Status == 0) {
        return BPLIB_SUCCESS;
    }
    return BPLIB_ERROR;
}

BPLib_Status_t BPLib_CBOR_ValidateCRC(void)
{

    return BPLIB_SUCCESS;
}

int32_t BPLib_CBOR_SimpleValuesTest1(void)
{
   QCBOREncodeContext ECtx;
   int nReturn = 0;

   QCBOREncode_Init(&ECtx, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
   QCBOREncode_OpenArray(&ECtx);

   QCBOREncode_AddBool(&ECtx, true);
   QCBOREncode_AddBool(&ECtx, false);
   QCBOREncode_AddNULL(&ECtx);
   QCBOREncode_AddUndef(&ECtx);

   QCBOREncode_OpenMap(&ECtx);

   QCBOREncode_AddUndefToMapSZ(&ECtx, "UNDef");
   QCBOREncode_CloseMap(&ECtx);

   QCBOREncode_CloseArray(&ECtx);

   UsefulBufC ECBOR;
   if(QCBOREncode_Finish(&ECtx, &ECBOR)) {
      nReturn = -1;
   }

   if(CheckResults(ECBOR, spExpectedEncodedSimple))
      return -2;

   return(nReturn);
}

QCBORError BPLib_CBOR_CreationTimestampParse(QCBORDecodeContext* ctx, BPLib_CreationTimestamp_t* CreationTimestamp)
{
    QCBORItem CurrItem;
    QCBORError QStatus = QCBOR_SUCCESS; // @note QStatus is not used correctly.
    uint64_t parsed;

    /* Enter Timestamp Array */
    QCBORDecode_EnterArray(ctx, &CurrItem);
    if (QStatus != QCBOR_SUCCESS)
    {
        return QStatus;
    }

    /* First Element of the array should be creation timestamp */
    QCBORDecode_GetUInt64(ctx, &parsed);
    if (QStatus != QCBOR_SUCCESS)
    {
        return QStatus;
    }
    printf("Creation Timestamp: %lu\n", parsed);

    /* Second Element of the array should be creation timestamp */
    QCBORDecode_GetUInt64(ctx, &parsed);
    if (QStatus != QCBOR_SUCCESS)
    {
        return QStatus;
    }

    /* Exit Timestamp Array */
    QCBORDecode_ExitArray(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return QStatus;
    }

    return QCBOR_SUCCESS;
}

/* EID Type */
QCBORError BPLib_CBOR_EIDParse(QCBORDecodeContext* ctx, BPLib_EID_t* eid)
{
    QCBORItem CurrItem;
    QCBORError QStatus  = QCBOR_SUCCESS; // @note QStatus is not used correctly.
    uint64_t parsed;

    /* Enter Primary Block Array */
    QCBORDecode_EnterArray(ctx, &CurrItem);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return QStatus;
    }

    /* URI */
    QCBORDecode_GetUInt64(ctx, &parsed);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return QStatus;
    }
    printf("URI: %lu\n", parsed);

    /* Enter SSP Array */
    QCBORDecode_EnterArray(ctx, &CurrItem);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return QStatus;
    }

    /* Node Number */
    QCBORDecode_GetUInt64(ctx, &parsed);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return QStatus;
    }
    printf("Node Number: %lu\n", parsed);

    /* Node Service */
    QCBORDecode_GetUInt64(ctx, &parsed);
    printf("Node Service: %lu\n", parsed);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return QStatus;
    }

    /* Exit SSP Array */
    QCBORDecode_ExitArray(ctx);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return QStatus;
    }

    /* Exit Primary Block Array*/
    QCBORDecode_ExitArray(ctx);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return QStatus;
    }

    return QCBOR_SUCCESS;
}

/* Block Functions */
BPLib_Status_t BPLib_CBOR_PrimaryBlockParse(QCBORDecodeContext* ctx)
{
    QCBORError QStatus = QCBOR_SUCCESS; // @note QStatus is not used correctly.
    QCBORItem CurrItem;
    uint64_t parsed;

    /* Ctx should be set to allow us to enter a definite array */
    QCBORDecode_EnterArray(ctx, &CurrItem);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    /* Version */
    QCBORDecode_GetUInt64(ctx, &parsed);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }
    printf("Version: %lu\n", parsed);

    /* Flags */
    QCBORDecode_GetUInt64(ctx, &parsed);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }
    printf("Flags: %lu\n", parsed);

    /* It is worth validating the flags here because we can potential stop processing if somethings wrong. */

    /* CRC Type */
    QCBORDecode_GetUInt64(ctx, &parsed);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }
    printf("CRC Type: %ld\n", parsed);

    /* EIDs */
    if(BPLib_CBOR_EIDParse(ctx, NULL) != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }
    if(BPLib_CBOR_EIDParse(ctx, NULL) != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }
    if(BPLib_CBOR_EIDParse(ctx, NULL) != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    /* Creation Timestamp */
    if (BPLib_CBOR_CreationTimestampParse(ctx, NULL) != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    /* Lifetime */
    QCBORDecode_GetUInt64(ctx, &parsed);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }
    printf("Lifetime: %ld\n", parsed);

    /* CRC: For now I'm skipping this entirely */
    if (QCBORDecode_GetNext(ctx, &CurrItem) != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }
    if (CurrItem.uDataType != QCBOR_TYPE_BYTE_STRING)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    /* Leave the array, which should align the next parse function to an array start*/
    QCBORDecode_ExitArray(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_PRI_ERR;
    }

    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_CBOR_CanonicalBlockParse(QCBORDecodeContext* ctx)
{
    return BPLIB_CBOR_DEC_ERR;
}
