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

// Only uncomment locally, shouldn't be included in the main branch
#define BPLIB_CBOR_DEBUG_PRINTS_ENABLED (1)

#if (BPLIB_CBOR_DEBUG_PRINTS_ENABLED)
#include <stdio.h>
#endif

/*
** Exported Functions
*/

/**
 * \brief     Decode Canonical Block Data
 * \details   Decode Canonical Block Data and fill in related bundle metadata
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] bundle (BPLib_Bundle_t*) pointer to the bundle metadata (to be filled out)
 * \param[in] CanonicalBlockIndex (uint32_t) which bundle extension block metadata to fill out
 * \param[in] CandBundle (const void *) Candidate bundle to decode
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 * \retval    BPLIB_CBOR_DEC_CANON_BLOCK_INDEX_ERR: index past max supported canonical blocks
 */
BPLib_Status_t BPLib_CBOR_DecodeCanonical(QCBORDecodeContext* ctx, BPLib_Bundle_t* bundle,
    uint32_t CanonicalBlockIndex, const void *CandBundle);


/**
 * \brief     Decode Primary Block Data
 * \details   Decode Primary Block Data and fill in related bundle metadata
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] bundle (BPLib_Bundle_t*) pointer to the bundle metadata (to be filled out)
 * \param[in] CandBundle (const void *) Candidate bundle to decode
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 * \retval    BPLIB_CBOR_DEC_PRIM_ENTER_ARRAY_ERR: CBOR primary block decode error - entry array
 * \retval    BPLIB_CBOR_DEC_PRIM_EXIT_ARRAY_ERR: CBOR primary block decode error - exit array
 * \retval    BPLIB_CBOR_DEC_PRIM_VERSION_DEC_ERR: CBOR primary block decode error - decode version field
 * \retval    BPLIB_CBOR_DEC_PRIM_WRONG_VERSION_ERR: CBOR primary block decode error - wrong version field
 * \retval    BPLIB_CBOR_DEC_PRIM_FLAG_DEC_ERR: CBOR primary block decode error - decode flag field
 * \retval    BPLIB_CBOR_DEC_PRIM_WRONG_FLAG_ERR: CBOR primary block decode error - wrong flag field
 * \retval    BPLIB_CBOR_DEC_PRIM_CRC_TYPE_DEC_ERR: CBOR primary block decode error - decode crc type field
 * \retval    BPLIB_CBOR_DEC_PRIM_DEST_EID_DEC_ERR: CBOR primary block decode error - decode Dest EID field
 * \retval    BPLIB_CBOR_DEC_PRIM_SRC_EID_DEC_ERR: CBOR primary block decode error - decode Src EID field
 * \retval    BPLIB_CBOR_DEC_PRIM_REPORT_EID_DEC_ERR: CBOR primary block decode error - decode Report-To EID field
 * \retval    BPLIB_CBOR_DEC_PRIM_CREATE_TIME_DEC_ERR: CBOR primary block decode error - decode create time field
 * \retval    BPLIB_CBOR_DEC_PRIM_LIFETIME_DEC_ERR: CBOR primary block decode error - decode lifetime field
 * \retval    BPLIB_CBOR_DEC_PRIM_CRC_VAL_DEC_ERR: CBOR primary block decode error - decode crc value field
 */
BPLib_Status_t BPLib_CBOR_DecodePrimary(QCBORDecodeContext* ctx, BPLib_Bundle_t* bundle,
                                                            const void *CandBundle);



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
 */
typedef BPLib_Status_t (*QCBOR_UInt64Parser)(QCBORDecodeContext* ctx, uint64_t* parsed);


/**
 * \brief     Function pointer for a function that decodes a BPLib_EID_t type
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] parsed (BPLib_EID_t*) pointer to the field that needs to be filled with decoded data
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 */
typedef BPLib_Status_t (*QCBOR_EIDParser)(QCBORDecodeContext* ctx, BPLib_EID_t* parsed);


/**
 * \brief     Function pointer for a function that decodes a BPLib_CreationTimeStamp_t
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] parsed (BPLib_CreationTimeStamp_t*) pointer to the field that needs to be filled with decoded data
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
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
 */
typedef BPLib_Status_t (*QCBOR_CRCParser)(QCBORDecodeContext* ctx, uint64_t* parsed,
                                            uint64_t crc_type);



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
 */
BPLib_Status_t BPLib_QCBOR_EnterDefiniteArray(QCBORDecodeContext* ctx, size_t* ArrayLen);


/**
 * \brief     Exits a CBOR definite array (always to be used with BPLib_QCBOR_EnterDefiniteArray)
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
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
 */
BPLib_Status_t BPLib_QCBOR_UInt64ParserImpl(QCBORDecodeContext* ctx, uint64_t* parsed);

/**
 * \brief     Decodes a BPLib_EID_t type for EIDs that cannot have DTN schemes
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] parsed (BPLib_EID_t*) pointer to the field that needs to be filled with decoded data
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 */
BPLib_Status_t BPLib_QCBOR_EidDtnNoneParserImpl(QCBORDecodeContext* ctx, BPLib_EID_t* parsed);

/**
 * \brief     Decodes a BPLib_EID_t type for report-to-EIDs
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] parsed (BPLib_EID_t*) pointer to the field that needs to be filled with decoded data
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 */
BPLib_Status_t BPLib_QCBOR_ReportToEidParserImpl(QCBORDecodeContext* ctx, BPLib_EID_t* parsed);

/**
 * \brief     Decodes a BPLib_CreationTimeStamp_t
 * \param[in] ctx (QCBORDecodeContext*) QCBOR decode context instance pointer
 * \param[in] parsed (BPLib_CreationTimeStamp_t*) pointer to the field that needs to be filled with decoded data
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
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
 */
BPLib_Status_t BPLib_QCBOR_CRCParserImpl(QCBORDecodeContext* ctx, uint64_t* parsed, uint64_t crc_type);




/**
 * \brief Encodes the primary block data into the output buffer
 *
 * \param[in] StoredBundle (BPLib_Bundle_t*) Pointer to the bundle from which to copy the data.
 * \param[out] OutputBuffer (void*) Destination buffer, to put the copied data.
 * \param[in] OutputBufferSize (size_t) The maximum number of bytes than can be copied to OutputBuffer.
 * \param[out] NumBytesCopied (size_t*) The actual number of bytes copied.
 *
 * \return Status of the operation.
 */
BPLib_Status_t BPLib_CBOR_EncodePrimary(BPLib_Bundle_t* StoredBundle,
    void* OutputBuffer,
    size_t OutputBufferSize,
    size_t* NumBytesCopied);

/**
 * \brief Copies the Primary Block out of a stored bundle,
 *        or encodes the primary (if never previously encoded)
 *
 * This function copies the Primary Block from the specified bundle into the provided buffer.
 * If the bundle has more Primary Block than OutputBufferSize, this function returns BPLIB_BUF_LEN_ERROR
 *
 * \param[in] StoredBundle Pointer to the bundle from which to copy the data.
 * \param[out] OutputBuffer A buffer to store the copied data.
 * \param[in] OutputBufferSize The maximum number of bytes to copy.
 * \param[out] NumBytesCopied The actual number of bytes copied.
 *
 * \return Status of the operation.
 */
BPLib_Status_t BPLib_CBOR_CopyOrEncodePrimary(BPLib_Bundle_t* StoredBundle,
    void* OutputBuffer,
    size_t OutputBufferSize,
    size_t* NumBytesCopied);


/**
 * \brief Returns the number of extension blocks populated in bundle metadata
 *
 * \param[in] StoredBundle Pointer to the bundle
 *
 * \return (uint32_t) number of extension blocks populated in bundle metadata
 */
uint32_t BPLib_CBOR_GetNumExtensionBlocks(BPLib_Bundle_t* StoredBundle);


/**
 * \brief Encodes the extension block's block-specific data, putting the output in the provided buffer
 *
 * \param[in] CurrExtBlock (BPLib_ExtensionBlock_t*) Pointer to the extension block to be encoded
 * \param[in] ExtensionBlockIndex (UsefulBuf*) Pointer to the output buffer info (ptr and len)
 *
 * \return Number of bytes encoded and output to the buffer.
 */
size_t BPLib_CBOR_EncodeBlockSpecificData(BPLib_ExtensionBlock_t* CurrExtBlock,
    UsefulBuf* CurrentContextPlace);

/**
 * \brief Encodes the specified extension block data into the output buffer
 *
 * \param[in] StoredBundle (BPLib_Bundle_t*) Pointer to the bundle from which to copy the data.
 * \param[in] ExtensionBlockIndex (uint32_t) Pointer to the bundle from which to copy the data.
 * \param[out] OutputBuffer (void*) Destination buffer, to put the copied data.
 * \param[in] OutputBufferSize (size_t) The maximum number of bytes than can be copied to OutputBuffer.
 * \param[out] NumBytesCopied (size_t*) The actual number of bytes copied.
 *
 * \return Status of the operation.
 */
BPLib_Status_t BPLib_CBOR_EncodeExtensionBlock(BPLib_Bundle_t* StoredBundle,
    uint32_t ExtensionBlockIndex,
    void* OutputBuffer,
    size_t OutputBufferSize,
    size_t* NumBytesCopied);

/**
 * \brief Encodes the payload into the output buffer
 *
 * Note that this function effectively re-implements parts of the QCBOR library,
 * to prevent an additional memory copy of the payload data.
 * Specifically, instead of adding the ADU data using QCBOREncode_OpenBytes,
 * we build our own CBOR byte array head info before copying in the data from our memory blocks.
 * The use of QCBOREncode_OpenBytes and QCBOREncode_CloseBytes would result in an extra copy of the ADU,
 * since QCBOR doesn't know the ADU size until QCBOREncode_CloseBytes is called.
 *
 * \param[in] StoredBundle (BPLib_Bundle_t*) Pointer to the bundle from which to copy the data.
 * \param[out] OutputBuffer (void*) Destination buffer, to put the copied data.
 * \param[in] OutputBufferSize (size_t) The maximum number of bytes than can be copied to OutputBuffer.
 * \param[out] NumBytesCopied (size_t*) The actual number of bytes copied.
 *
 * \return Status of the operation.
 */
BPLib_Status_t BPLib_CBOR_EncodePayload(BPLib_Bundle_t* StoredBundle,
    void* OutputBuffer,
    size_t OutputBufferSize,
    size_t* NumBytesCopied);


/**
 * \brief Copies the Payload Block out of a stored bundle,
 *        or encodes the primary (if never previously encoded)
 *
 * This function copies the Payload Block from the specified bundle into the provided buffer.
 * If the bundle has more Payload Block than OutputBufferSize, this function returns BPLIB_BUF_LEN_ERROR
 *
 * \param[in] StoredBundle Pointer to the bundle from which to copy the data.
 * \param[out] OutputBuffer A buffer to store the copied data.
 * \param[in] OutputBufferSize The maximum number of bytes to copy.
 * \param[out] NumBytesCopied The actual number of bytes copied.
 *
 * \return Status of the operation.
 */
BPLib_Status_t BPLib_CBOR_CopyOrEncodePayload(BPLib_Bundle_t* StoredBundle,
    void* OutputBuffer,
    size_t OutputBufferSize,
    size_t* NumBytesCopied);


/**
 * \brief     Encodes a BPLib_EID_t type
 * \param[in] Context (QCBOREncodeContext*) QCBOR encode context instance pointer
 * \param[in] SourceData (BPLib_EID_t*) pointer to the field that needs to be encoded
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 */
BPLib_Status_t BPLib_CBOR_EncodeEID(QCBOREncodeContext* Context, BPLib_EID_t* SourceData);



/**
 * \brief     Encodes a BPLib_CreationTimeStamp_t
 * \param[in] Context (QCBOREncodeContext*) QCBOR encode context instance pointer
 * \param[in] TimeStamp (BPLib_CreationTimeStamp_t*) pointer to the field that needs to be encoded
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 */
BPLib_Status_t BPLib_CBOR_EncodeCreationTimeStamp(QCBOREncodeContext* Context, BPLib_CreationTimeStamp_t* TimeStamp);

/**
 * \brief     Encodes a CRC value
 * \param[in] Context (QCBOREncodeContext*) QCBOR encode context instance pointer
 * \param[in] CrcValue (uint64_t) CRC Value to be encoded
 * \param[in] CrcType (uint64_t) specifies the expected CRC type (None, CRC16, or CRC32C)
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 */
BPLib_Status_t BPLib_CBOR_EncodeCrcValue(QCBOREncodeContext* Context, uint64_t CrcValue, uint64_t CrcType);

/**
 * \brief     Validates a block's CRC value
 * \param[in] EncodedBundle (const void *) Pointer to the CBOR-encoded bundle buffer
 * \param[in] CrcType (BPLib_CRC_Type_t) Specifies the expected CRC type (None, CRC16, or CRC32C)
 * \param[in] ExpectedCrc (BPLib_CRC_Val_t) Value to check the calculated CRC against
 * \param[in] BlockOffsetStart (size_t) Offset in EncodedBundle where the block starts
 * \param[in] BlockLength (size_t) Length of the block in EncodedBundle
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_INVALID_CRC_ERROR: CRC was invalid
 */
BPLib_Status_t BPLib_CBOR_ValidateBlockCrc(const void *EncodedBundle,
    BPLib_CRC_Type_t CrcType, BPLib_CRC_Val_t ExpectedCrc,
    size_t BlockOffsetStart, size_t BlockLength);

/**
 * \brief     Generates a block's CRC value
 * \param[in] EncodedBundle (const void *) Pointer to the CBOR-encoded bundle buffer
 * \param[in] CrcType (BPLib_CRC_Type_t) Specifies the expected CRC type (None, CRC16, or CRC32C)
 * \param[in] BlockOffsetStart (size_t) Offset in EncodedBundle where the block starts
 * \param[in] BlockLength (size_t) Length of the block in EncodedBundle
 */
void BPLib_CBOR_GenerateBlockCrc(const void *EncodedBundle, BPLib_CRC_Type_t CrcType,
                                size_t BlockOffsetStart, size_t BlockLength);

#endif /* BPLIB_CBOR_INTERNAL_H */
