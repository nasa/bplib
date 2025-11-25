/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
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
 * \brief Encodes the stored bundle, putting the output in the provided buffer
 *
 * \param[in] StoredBundle (BPLib_Bundle_t*) Pointer to the stored source bundle info
 * \param[out] OutputBuffer (void*) An output buffer to store the encoded bundle.
 * \param[in] OutputBufferSize (size_t) The maximum number of bytes to output.
 * \param[out] NumBytesCopied The actual number of bytes copied.
 *
 * \return Status of the operation.
 */
BPLib_Status_t BPLib_CBOR_EncodeBundle(BPLib_Bundle_t* StoredBundle,
    void* OutputBuffer,
    size_t OutputBufferSize,
    size_t* NumBytesCopied);

/**
 * \brief     Verify that the bundle processing flags aren't invalid
 * \note      RFC 9171 policy that anonymous bundle transmission is permitted is
 *            not honored by the DTN requirements. Therefore, there is no check on
 *            the state of the "Bundle must not be fragmented" flag in relation to
 *            all status report request flags being zero.
 * \param[in] BundleProcFlags (uint64_t) Bundle processing flags to be verified
 *            are to be validated
 * \return    Execution Status
 * \retval    BPLIB_SUCCESS: Bundle processing flags are valid
 * \retval    BPLIB_CBOR_DEC_PRIM_WRONG_FLAG_ERR: Bundle processing flags are invalid
 */
BPLib_Status_t BPLib_CBOR_VerifyBundleProcFlags(uint64_t BundleProcFlags);

#endif /* BPLIB_CBOR_H */
