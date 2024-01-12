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

#ifndef v7_cache_h
#define v7_cache_h

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"

#include "bplib_api_types.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/
typedef enum bplib_cache_module_type
{
    bplib_cache_module_type_none,
    bplib_cache_module_type_internal,
    bplib_cache_module_type_offload
} bplib_cache_module_type_t;

typedef enum bplib_cache_module_valtype
{
    bplib_cache_module_valtype_none,
    bplib_cache_module_valtype_integer,
    bplib_cache_module_valtype_string
} bplib_cache_module_valtype_t;

typedef enum bplib_cache_confkey
{
    bplib_cache_confkey_none,
    bplib_cache_confkey_offload_base_dir,
} bplib_cache_confkey_t;

struct bplib_cache_module_api
{
    bplib_cache_module_type_t module_type;
    bplib_mpool_block_t *(*instantiate)(bplib_mpool_ref_t parent_ref, void *init_arg);
    int (*configure)(bplib_mpool_block_t *svc, int key, bplib_cache_module_valtype_t vt, const void *val);
    int (*query)(bplib_mpool_block_t *svc, int key, bplib_cache_module_valtype_t vt, const void **val);
    int (*start)(bplib_mpool_block_t *svc);
    int (*stop)(bplib_mpool_block_t *svc);
};

typedef struct bplib_cache_offload_api
{
    bplib_cache_module_api_t std;
    int (*offload)(bplib_mpool_block_t *svc, bp_sid_t *sid, bplib_mpool_block_t *pblk);
    int (*restore)(bplib_mpool_block_t *svc, bp_sid_t sid, bplib_mpool_block_t **pblk);
    int (*release)(bplib_mpool_block_t *svc, bp_sid_t sid);
} bplib_cache_offload_api_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Service API */
bp_handle_t bplib_cache_attach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *service_addr);
int         bplib_cache_detach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *service_addr);

bp_handle_t bplib_cache_register_module_service(bplib_routetbl_t *tbl, bp_handle_t cache_intf_id,
                                                const bplib_cache_module_api_t *api, void *init_arg);
int bplib_cache_configure(bplib_routetbl_t *tbl, bp_handle_t module_intf_id, int key, bplib_cache_module_valtype_t vt,
                          const void *val);
int bplib_cache_query(bplib_routetbl_t *tbl, bp_handle_t module_intf_id, int key, bplib_cache_module_valtype_t vt,
                      const void **val);
int bplib_cache_start(bplib_routetbl_t *tbl, bp_handle_t module_intf_id);
int bplib_cache_stop(bplib_routetbl_t *tbl, bp_handle_t module_intf_id);

void bplib_cache_debug_scan(bplib_routetbl_t *tbl, bp_handle_t intf_id);

#ifdef __cplusplus
}
#endif

#endif /* v7_cache_h */
