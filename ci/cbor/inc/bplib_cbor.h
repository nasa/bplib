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
#include "bplib_bblocks.h"

/*
** Exported Functions
*/

/**
 * \brief     Decode a bundle
 * \details   Decode a bundle and fill in related bundle metadata
 * \param[in] CandBundle (const void*) Candidate Bundle pointer
 * \param[in] CandBundleLen (size_t) length of the provided Candidate Bundle
 * \param[in] bundle (BPLib_Bundle_t*) pointer to the bundle metadata (to be filled out)
 * \return    Execution status
 * \retval    BPLIB_SUCCESS: Successful execution
 * \retval    BPLIB_NULL_PTR_ERROR: invalid input pointer
 */
BPLib_Status_t BPLib_CBOR_DecodeBundle(const void* CandBundle, size_t CandBundleLen, BPLib_Bundle_t *bundle);


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


#endif /* BPLIB_CBOR_H */
