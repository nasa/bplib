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

#ifndef BPLIB_DATASERVICE_H
#define BPLIB_DATASERVICE_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_api_types.h"

#include "bplib_mem.h"

typedef enum
{
    BPLib_STOR_CACHE_DataserviceTypeUndefined,
    BPLib_STOR_CACHE_DataserviceTypeApplication,
    BPLib_STOR_CACHE_DataserviceTypeStorage,
    BPLib_STOR_CACHE_DataserviceTypeMax
}BPLib_STOR_CACHE_DataserviceType_t;

bp_handle_t BPLib_STOR_CACHE_DataserviceAddBaseIntf(BPLib_STOR_CACHE_Block_t *rtbl, bp_ipn_t node_number);

#ifdef QM_QUEUE_TABLE
bp_handle_t BPLib_STOR_CACHE_DataserviceAttach(
    BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *ipn,
    BPLib_STOR_CACHE_DataserviceType_t type,
    BPLib_STOR_CACHE_Ref_t blkref);
#endif // QM_QUEUE_TABLE

BPLib_STOR_CACHE_Ref_t BPLib_STOR_CACHE_DataserviceDetach(BPLib_STOR_CACHE_Block_t *tbl, const bp_ipn_addr_t *ipn);

#endif /* BPLIB_DATASERVICE_H */
