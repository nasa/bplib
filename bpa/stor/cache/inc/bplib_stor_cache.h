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

#ifndef BPLIB_STOR_CACHE_H
#define BPLIB_STOR_CACHE_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_api_types.h"

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Endpoint IDs */
#define BP_MAX_EID_STRING 128
#define BP_IPN_NULL       0

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

bp_handle_t BPLib_STOR_CACHE_CreateRamStorage(BPLib_STOR_QM_QueueTbl_t *rtbl, const bp_ipn_addr_t *storage_addr);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_STOR_CACHE_H */
