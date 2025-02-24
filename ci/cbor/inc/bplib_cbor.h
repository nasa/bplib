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

#ifndef BPLIB_CBOR_H
#define BPLIB_CBOR_H

/*
** Include
*/

#include "bplib.h"
#include "bplib_api_types.h"

#include "qcbor/qcbor_encode.h"
#include "qcbor/qcbor_decode.h"
#include "qcbor/qcbor_spiffy_decode.h"

typedef struct BPLib_CreationTimestamp
{
    uint64_t CreationTimestamp;
    uint64_t SequenceNumber;
} BPLib_CreationTimestamp_t;

/*
** Exported Functions
*/

/**
 * \brief CBOR initialization
 *
 *  \par Description
 *       CBOR initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
BPLib_Status_t BPLib_CBOR_Init(void);

/**
 * @brief The CBOR APIs provide various CBOR encode and decode functions that
 * use the BPLib_Bundle_t data structure for both input and output.
 * BPLib_Bundle_t has the BPLib_BBlocks_t bundle data structure and a blob for CBOR data.
 * The functions that are specific to a canonical block type are provided for convenience.
 */

          void BPLib_CBOR_DecodeAgeData(QCBORDecodeContext *DecodeCtx, BPLib_Bundle_t *Bundle);
BPLib_Status_t BPLib_CBOR_DecodeBundle(QCBORDecodeContext *DecodeCtx, BPLib_Bundle_t *CandidateBundle, size_t CandidateBundleLen);
BPLib_Status_t BPLib_CBOR_DecodeCanonical(BPLib_Bundle_t *Bundle, const void *data_ptr, size_t data_size);
          void BPLib_CBOR_DecodeHopCountData(QCBORDecodeContext *DecodeCtx, BPLib_Bundle_t *Bundle);
BPLib_Status_t BPLib_CBOR_DecodePayload(BPLib_Bundle_t *Bundle, const void *data_ptr, size_t data_size);
          void BPLib_CBOR_DecodePrevNodeData(QCBORDecodeContext *DecodeCtx, BPLib_Bundle_t *Bundle);
          void BPLib_CBOR_DecodePrimary(QCBORDecodeContext *DecodeCtx, PrimaryBlock_t *v);

          void BPLib_CBOR_EncodeAgeData(QCBOREncodeContext *EncodeCtx, BPLib_Bundle_t *Bundle);
          void BPLib_CBOR_EncodeBundle(QCBOREncodeContext *EncodeCtx, BPLib_Bundle_t *Bundle);
BPLib_Status_t BPLib_CBOR_EncodeCanonical(BPLib_Bundle_t *Bundle);
          void BPLib_CBOR_EncodeCRC(QCBOREncodeContext *enc);
          void BPLib_CBOR_EncodeHopCountData(QCBOREncodeContext *EncodeCtx, BPLib_Bundle_t *Bundle);
BPLib_Status_t BPLib_CBOR_EncodePayload(BPLib_Bundle_t *Bundle, const void *data_ptr, size_t data_size);
          void BPLib_CBOR_EncodePrevNodeData(QCBOREncodeContext *EncodeCtx, BPLib_Bundle_t *Bundle);
          void BPLib_CBOR_EncodePrimary(QCBOREncodeContext *EncodeCtx, BPLib_Bundle_t *Bundle);
BPLib_Status_t BPLib_CBOR_ValidateCRC(void);


    QCBORError BPLib_CBOR_CreationTimestampParse(QCBORDecodeContext* ctx, BPLib_CreationTimestamp_t* CreationTimestamp);
    QCBORError BPLib_CBOR_EIDParse(QCBORDecodeContext* ctx, BPLib_EID_t* eid);
BPLib_Status_t BPLib_CBOR_PrimaryBlockParse(QCBORDecodeContext* ctx);
BPLib_Status_t BPLib_CBOR_CanonicalBlockParse(QCBORDecodeContext* ctx);

#endif /* BPLIB_CBOR_H */
