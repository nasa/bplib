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

#ifndef BPLIB_BI_H
#define BPLIB_BI_H

/*
** Include
*/

#include "bplib_api_types.h"
#include "bplib_cla.h"
#include "bplib_qm.h"
#include "bplib_bundle.h"

/*
** Exported Functions
*/

/**
 * \brief Bundle Interface initialization
 *
 *  \par Description
 *       BI initialization function
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
BPLib_Status_t BPLib_BI_Init(void);

/**
 * \brief Function for Receiving Bundle from CLA
 *
 *  \par Description
 *       Receive candidate bundle from CLA, CBOR decode it, then place the deserialized bundle to EBP In Queue
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] inst Pointer to a valid BPLib_Instance_t
 *  \param[in] BundleInPtr Pointer to the bundle
 *  \param[in] Size Bundle size
 * 
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
BPLib_Status_t BPLib_BI_RecvFullBundleIn(BPLib_Instance_t* inst, const void *BundleInPtr, size_t Size);


/**
 * \brief Function for receiving control message from CLA
 *
 *  \par Description
 *       Receive Control Messages from CLA, pass them to CT
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 * 
 *  \param[in] CtrlMsg Control Message
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
BPLib_Status_t BPLib_BI_RecvCtrlMsg(BPLib_CLA_CtrlMsg_t* CtrlMsg);

/**
 * \brief Function for validating the deserialized bundle
 *
 *  \par Description
 *       Validate deserialized bundle after CBOR decoding
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Execution status
 *  \retval BPLIB_SUCCESS Initialization was successful
 */
BPLib_Status_t BPLib_BI_ValidateBundle(void);

#endif /* BPLIB_BI_H */
