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

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_api_types.h"
#include "v7_mpool.h"

typedef enum
{
    bplib_dataservice_type_undefined,
    bplib_dataservice_type_application,
    bplib_dataservice_type_storage,
    bplib_dataservice_type_max
} bplib_dataservice_type_t;

bp_handle_t bplib_dataservice_add_base_intf(bplib_routetbl_t *rtbl, bp_ipn_t node_number);
bp_handle_t bplib_dataservice_attach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *ipn, bplib_dataservice_type_t type,
                                     bplib_mpool_ref_t blkref);
bplib_mpool_ref_t bplib_dataservice_detach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *ipn);

#ifdef __cplusplus
}
#endif

#endif /* BPLIB_DATASERVICE_H */
