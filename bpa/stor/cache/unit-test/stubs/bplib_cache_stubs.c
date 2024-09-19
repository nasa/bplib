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

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in bplib_cache.header
 */

#include "bplib_cache.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_Module_attach()
 * ----------------------------------------------------
 */
bp_handle_t BPLib_STOR_CACHE_Module_attach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *service_addr)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_Module_attach, bp_handle_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_attach, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_attach, const bp_ipn_addr_t *, service_addr);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Module_attach, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_Module_attach, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_Module_configure()
 * ----------------------------------------------------
 */
int BPLib_STOR_CACHE_Module_configure(bplib_routetbl_t *tbl, bp_handle_t module_intf_id, int key, BPLib_STOR_CACHE_Module_module_valtype_t vt,
                          const void *val)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_Module_configure, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_configure, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_configure, bp_handle_t, module_intf_id);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_configure, int, key);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_configure, BPLib_STOR_CACHE_Module_module_valtype_t, vt);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_configure, const void *, val);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Module_configure, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_Module_configure, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_Module_debug_scan()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_Module_debug_scan(bplib_routetbl_t *tbl, bp_handle_t intf_id)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_debug_scan, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_debug_scan, bp_handle_t, intf_id);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Module_debug_scan, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_Module_detach()
 * ----------------------------------------------------
 */
int BPLib_STOR_CACHE_Module_detach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *service_addr)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_Module_detach, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_detach, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_detach, const bp_ipn_addr_t *, service_addr);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Module_detach, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_Module_detach, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_Module_query()
 * ----------------------------------------------------
 */
int BPLib_STOR_CACHE_Module_query(bplib_routetbl_t *tbl, bp_handle_t module_intf_id, int key, BPLib_STOR_CACHE_Module_module_valtype_t vt,
                      const void **val)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_Module_query, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_query, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_query, bp_handle_t, module_intf_id);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_query, int, key);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_query, BPLib_STOR_CACHE_Module_module_valtype_t, vt);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_query, const void **, val);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Module_query, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_Module_query, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_Module_register_module_service()
 * ----------------------------------------------------
 */
bp_handle_t BPLib_STOR_CACHE_Module_register_module_service(bplib_routetbl_t *tbl, bp_handle_t cache_intf_id,
                                                const BPLib_STOR_CACHE_Module_module_api_t *api, void *init_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_Module_register_module_service, bp_handle_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_register_module_service, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_register_module_service, bp_handle_t, cache_intf_id);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_register_module_service, const BPLib_STOR_CACHE_Module_module_api_t *, api);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_register_module_service, void *, init_arg);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Module_register_module_service, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_Module_register_module_service, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_Module_start()
 * ----------------------------------------------------
 */
int BPLib_STOR_CACHE_Module_start(bplib_routetbl_t *tbl, bp_handle_t module_intf_id)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_Module_start, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_start, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_start, bp_handle_t, module_intf_id);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Module_start, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_Module_start, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_Module_stop()
 * ----------------------------------------------------
 */
int BPLib_STOR_CACHE_Module_stop(bplib_routetbl_t *tbl, bp_handle_t module_intf_id)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_Module_stop, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_stop, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Module_stop, bp_handle_t, module_intf_id);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Module_stop, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_Module_stop, int);
}
