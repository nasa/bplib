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

#ifndef BPLIB_AS_INTERNAL_H
#define BPLIB_AS_INTERNAL_H

/* ======== */
/* Includes */
/* ======== */

#include "bplib_api_types.h"
#include "bplib_as.h"

/* =================== */
/* Function Prototypes */
/* =================== */

/**
 * \brief     Checks if the source EID matches an expected, valid pattern
 * \details   Eventually some more advanced checks will occur with something like regex.
 * \note      The source EID is not expected to be int16_t typed data but it is for now, as a placeholder
 * \param[in] SourceEid (int16_t) Indicator of which source in the BPLib_SourceMibCountersHkTlm_Payload_t::MibArray
 *                      array should be modified
 * \return    Source EID validity
 * \retval    true: The source EID matches the criteria required to be valid <enter criteria here>
 * \retval    false: The source EID is invalid in some way
 * \secreflist
 * \refitem   BPLib_SourceMibCountersHkTlm_Payload_t
 * \endsecreflist
 * \anchor    BPLib_AS_EidIsValid
 */
bool BPLib_AS_EidIsValid(int16_t SourceEid);

#endif // BPLIB_AS_INTERNAL_H