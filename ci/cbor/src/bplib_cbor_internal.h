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

#ifndef BPLIB_CBOR_INTERNAL_H
#define BPLIB_CBOR_INTERNAL_H


#include "bplib_api_types.h"
#include "bplib_bblocks.h"
#include "bplib_cbor.h"


#include "qcbor/qcbor_encode.h"
#include "qcbor/qcbor_decode.h"
#include "qcbor/qcbor_spiffy_decode.h"


#include <stdio.h>


/*
** Exported Functions
*/

/**
 * \brief     Decode Canonical Block Data
 * \details   Decode Canonical Block Data and fill in related bundle metadata
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] bundle (BPLib_Bundle_t*) pointer to the bundle metadata (to be filled out)
 * \param[in] CanonicalBlockIndex (uint32_t) which bundle extension block metadata to fill out
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 * \retval    BPLIB_CBOR_DEC_CANON_LIM_ERR: index past max supported canonical blocks
 * \retval    BPLIB_CBOR_DEC_ERR: QCBOR library decode error
 * \retval    BPLIB_CBOR_DEC_CANON_ERR: other decode error
 */
BPLib_Status_t BPLib_CBOR_DecodeCanonical(QCBORDecodeContext* ctx,
                                          BPLib_Bundle_t* bundle,
                                          uint32_t CanonicalBlockIndex);


/**
 * \brief     Decode Primary Block Data
 * \details   Decode Primary Block Data and fill in related bundle metadata
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] bundle (BPLib_Bundle_t*) pointer to the bundle metadata (to be filled out)
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 * \retval    BPLIB_CBOR_DEC_PRI_ERR: decode error
 */
BPLib_Status_t BPLib_CBOR_DecodePrimary(QCBORDecodeContext* ctx, BPLib_Bundle_t* bundle);



/*******************************************************************************
* RFC-9171 Type Parsers
*/

/**
 * \brief     Function pointer for a function that decodes a uint64_t type
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] parsed (uint64_t*) pointer to the field that needs to be filled with decoded data
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 * \retval    BPLIB_CBOR_DEC_ERR: decode error
 */
typedef BPLib_Status_t (*QCBOR_UInt64Parser)(QCBORDecodeContext* ctx, uint64_t* parsed);


/**
 * \brief     Function pointer for a function that decodes a BPLib_EID_t type
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] parsed (BPLib_EID_t*) pointer to the field that needs to be filled with decoded data
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 * \retval    BPLIB_CBOR_DEC_ERR: decode error
 */
typedef BPLib_Status_t (*QCBOR_EIDParser)(QCBORDecodeContext* ctx, BPLib_EID_t* parsed);


/**
 * \brief     Function pointer for a function that decodes a BPLib_CreationTimeStamp_t
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] parsed (BPLib_CreationTimeStamp_t*) pointer to the field that needs to be filled with decoded data
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 * \retval    BPLIB_CBOR_DEC_ERR: decode error
 */
typedef BPLib_Status_t (*QCBOR_TimestampParser)(QCBORDecodeContext* ctx, BPLib_CreationTimeStamp_t* parsed);


/**
 * \brief     Function pointer for a function that decodes a CRC value
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] parsed (uint64_t*) pointer to the field that needs to be filled with decoded data
 * \param[in] crc_type (uint64_t) specifies the expected CRC type (None, CRC16, or CRC32C)
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 * \retval    BPLIB_CBOR_DEC_ERR: decode error
 */
typedef BPLib_Status_t (*QCBOR_CRCParser)(QCBORDecodeContext* ctx, uint64_t* parsed, uint64_t crc_type);



/*******************************************************************************
* Exported Parsing Helpers
*/


/**
 * \brief     Enters a CBOR definite array (always to be used with BPLib_QCBOR_ExitDefiniteArray)
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] parsed (uint64_t*) pointer to the field that needs to be filled with decoded data
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 * \retval    BPLIB_CBOR_DEC_ERR: decode error
 */
BPLib_Status_t BPLib_QCBOR_EnterDefiniteArray(QCBORDecodeContext* ctx, size_t* ArrayLen);


/**
 * \brief     Exits a CBOR definite array (always to be used with BPLib_QCBOR_EnterDefiniteArray)
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 * \retval    BPLIB_CBOR_DEC_ERR: decode error
 */
BPLib_Status_t BPLib_QCBOR_ExitDefiniteArray(QCBORDecodeContext* ctx);


/*******************************************************************************
* RFC-9171 Type Parsers (Implementation Prototypes)
*/


/**
 * \brief     Decodes a uint64_t type
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] parsed (uint64_t*) pointer to the field that needs to be filled with decoded data
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 * \retval    BPLIB_CBOR_DEC_ERR: decode error
 */
BPLib_Status_t BPLib_QCBOR_UInt64ParserImpl(QCBORDecodeContext* ctx, uint64_t* parsed);


/**
 * \brief     Decodes a BPLib_EID_t type
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] parsed (BPLib_EID_t*) pointer to the field that needs to be filled with decoded data
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 * \retval    BPLIB_CBOR_DEC_ERR: decode error
 */
BPLib_Status_t BPLib_QCBOR_EIDParserImpl(QCBORDecodeContext* ctx, BPLib_EID_t* parsed);



/**
 * \brief     Decodes a BPLib_CreationTimeStamp_t
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] parsed (BPLib_CreationTimeStamp_t*) pointer to the field that needs to be filled with decoded data
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 * \retval    BPLIB_CBOR_DEC_ERR: decode error
 */
BPLib_Status_t BPLib_QCBOR_TimestampParserImpl(QCBORDecodeContext* ctx, BPLib_CreationTimeStamp_t* parsed);



/**
 * \brief     Decodes a CRC value
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] parsed (uint64_t*) pointer to the field that needs to be filled with decoded data
 * \param[in] crc_type (uint64_t) specifies the expected CRC type (None, CRC16, or CRC32C)
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 * \retval    BPLIB_CBOR_DEC_ERR: decode error
 */
BPLib_Status_t BPLib_QCBOR_CRCParserImpl(QCBORDecodeContext* ctx, uint64_t* parsed, uint64_t crc_type);


#endif /* BPLIB_CBOR_INTERNAL_H */
