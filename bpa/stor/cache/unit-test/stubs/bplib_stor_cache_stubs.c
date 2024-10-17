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

#include "bplib_mem.h"

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"
#include "bplib_stor_cache_module_api.h"
#include "bplib_stor_cache_ref.h"

#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_Attach()
 * ----------------------------------------------------
 */
bp_handle_t BPLib_STOR_QM_Attach(BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *service_addr)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_Attach, bp_handle_t);

    UT_GenStub_AddParam(BPLib_STOR_QM_Attach, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_Attach, const bp_ipn_addr_t *, service_addr);

    UT_GenStub_Execute(BPLib_STOR_QM_Attach, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_Attach, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_PS_Configure()
 * ----------------------------------------------------
 */
int BPLib_STOR_PS_Configure(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t module_intf_id, int key, BPLib_STOR_QM_ModuleValtype_t vt,
                          const void *val)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_PS_Configure, int);

    UT_GenStub_AddParam(BPLib_STOR_PS_Configure, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_PS_Configure, bp_handle_t, module_intf_id);
    UT_GenStub_AddParam(BPLib_STOR_PS_Configure, int, key);
    UT_GenStub_AddParam(BPLib_STOR_PS_Configure, BPLib_STOR_QM_ModuleValtype_t, vt);
    UT_GenStub_AddParam(BPLib_STOR_PS_Configure, const void *, val);

    UT_GenStub_Execute(BPLib_STOR_PS_Configure, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_PS_Configure, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_DebugScanQueue()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_DebugScanQueue(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_DebugScanQueue, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_DebugScanQueue, bp_handle_t, intf_id);

    UT_GenStub_Execute(BPLib_STOR_CACHE_DebugScanQueue, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_Detach()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_Detach(BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *service_addr)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_Detach, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_Detach, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_Detach, const bp_ipn_addr_t *, service_addr);

    UT_GenStub_Execute(BPLib_STOR_QM_Detach, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_Detach, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_PS_Query()
 * ----------------------------------------------------
 */
int BPLib_STOR_PS_Query(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t module_intf_id, int key, BPLib_STOR_QM_ModuleValtype_t vt,
                      const void **val)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_PS_Query, int);

    UT_GenStub_AddParam(BPLib_STOR_PS_Query, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_PS_Query, bp_handle_t, module_intf_id);
    UT_GenStub_AddParam(BPLib_STOR_PS_Query, int, key);
    UT_GenStub_AddParam(BPLib_STOR_PS_Query, BPLib_STOR_QM_ModuleValtype_t, vt);
    UT_GenStub_AddParam(BPLib_STOR_PS_Query, const void **, val);

    UT_GenStub_Execute(BPLib_STOR_PS_Query, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_PS_Query, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_RegisterModuleService()
 * ----------------------------------------------------
 */
bp_handle_t BPLib_STOR_QM_RegisterModuleService(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t cache_intf_id,
                                                const BPLib_STOR_QM_ModuleApi_t *api, void *init_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_RegisterModuleService, bp_handle_t);

    UT_GenStub_AddParam(BPLib_STOR_QM_RegisterModuleService, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_QM_RegisterModuleService, bp_handle_t, cache_intf_id);
    UT_GenStub_AddParam(BPLib_STOR_QM_ModuleApi_t *, api);
    UT_GenStub_AddParam(BPLib_STOR_QM_RegisterModuleService, void *, init_arg);

    UT_GenStub_Execute(BPLib_STOR_QM_RegisterModuleService, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_RegisterModuleService, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_PS_Start()
 * ----------------------------------------------------
 */
int BPLib_STOR_PS_Start(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t module_intf_id)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_PS_Start, int);

    UT_GenStub_AddParam(BPLib_STOR_PS_Start, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_PS_Start, bp_handle_t, module_intf_id);

    UT_GenStub_Execute(BPLib_STOR_PS_Start, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_PS_Start, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_PS_Stop()
 * ----------------------------------------------------
 */
int BPLib_STOR_PS_Stop(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t module_intf_id)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_PS_Stop, int);

    UT_GenStub_AddParam(BPLib_STOR_PS_Stop, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_PS_Stop, bp_handle_t, module_intf_id);

    UT_GenStub_Execute(BPLib_STOR_PS_Stop, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_PS_Stop, int);
}
