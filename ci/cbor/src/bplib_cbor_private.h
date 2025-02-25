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

#ifndef BPLIB_CBOR_PRIVATE_H
#define BPLIB_CBOR_PRIVATE_H


#include "bplib.h"
#include "bplib_api_types.h"
#include "bplib_bblocks.h"
#include "bplib_cbor.h"


#include "qcbor/qcbor_encode.h"
#include "qcbor/qcbor_decode.h"
#include "qcbor/qcbor_spiffy_decode.h"



/*
** Exported Functions
*/


BPLib_Status_t BPLib_CBOR_DecodeCanonical(QCBORDecodeContext* ctx, BPLib_Bundle_t* bundle);

BPLib_Status_t BPLib_CBOR_DecodePrimary(QCBORDecodeContext* ctx, BPLib_Bundle_t* bundle);



/*******************************************************************************
* RFC-9171 Type Parsers
*/
typedef BPLib_Status_t (*QCBOR_UInt64Parser)(QCBORDecodeContext* ctx, uint64_t* parsed);

typedef BPLib_Status_t (*QCBOR_EIDParser)(QCBORDecodeContext* ctx, BPLib_EID_t* parsed);

typedef BPLib_Status_t (*QCBOR_CRCParser)(QCBORDecodeContext* ctx, uint64_t* parsed);

typedef BPLib_Status_t (*QCBOR_TimestampParser)(QCBORDecodeContext* ctx, BPLib_CreationTimeStamp_t* parsed);


/*******************************************************************************
* RFC-9171 Type Parsers (Implementation Prototypes)
*/
BPLib_Status_t BPLib_QCBOR_UInt64ParserImpl(QCBORDecodeContext* ctx, uint64_t* parsed);

BPLib_Status_t BPLib_QCBOR_EIDParserImpl(QCBORDecodeContext* ctx, BPLib_EID_t* parsed);

BPLib_Status_t BPLib_QCBOR_CRCParserImpl(QCBORDecodeContext* ctx, uint64_t* parsed);

BPLib_Status_t BPLib_QCBOR_TimestampParserImpl(QCBORDecodeContext* ctx, BPLib_CreationTimeStamp_t* parsed);

/*******************************************************************************
* Exported Parsing Helpers
*/
BPLib_Status_t BPLib_QCBOR_EnterDefiniteArray(QCBORDecodeContext* ctx, size_t* ArrayLen);

BPLib_Status_t BPLib_QCBOR_ExitDefiniteArray(QCBORDecodeContext* ctx);

#endif