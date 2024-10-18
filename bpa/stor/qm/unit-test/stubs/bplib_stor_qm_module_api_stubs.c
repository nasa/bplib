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
 * Auto-Generated stub implementations for functions defined in bplib_stor_cache_module_api header
 */

#include "bplib_stor_cache_module_api.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_Configure()
 * ----------------------------------------------------
 */
static int BPLib_STOR_QM_Configure(BPLib_STOR_CACHE_Block_t *svc, BPLib_Handle_t cache_intf_id, int key,
                                      BPLib_STOR_QM_ModuleValtype_t vt, const void *val)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_Configure, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_Configure, BPLib_STOR_CACHE_Block_t *, svc);
    UT_GenStub_AddParam(BPLib_STOR_QM_Configure, BPLib_Handle_t, cache_intf_id);
    UT_GenStub_AddParam(BPLib_STOR_QM_Configure, int, key);
    UT_GenStub_AddParam(BPLib_STOR_QM_Configure, BPLib_STOR_QM_ModuleValtype_t, vt);
    UT_GenStub_AddParam(BPLib_STOR_QM_Configure, const void *, val);

    UT_GenStub_Execute(BPLib_STOR_QM_Configure, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_Configure, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_Instantiate()
 * ----------------------------------------------------
 */
static BPLib_STOR_CACHE_Block_t *BPLib_STOR_QM_Instantiate(BPLib_STOR_CACHE_Ref_t parent, void *init_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_Instantiate, BPLib_STOR_CACHE_Block_t *);

    UT_GenStub_AddParam(BPLib_STOR_QM_Instantiate, BPLib_STOR_CACHE_Ref_t, parent);
    UT_GenStub_AddParam(BPLib_STOR_QM_Instantiate, void *, init_arg);

    UT_GenStub_Execute(BPLib_STOR_QM_Instantiate, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_Instantiate, BPLib_STOR_CACHE_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_Offload()
 * ----------------------------------------------------
 */
static int BPLib_STOR_CACHE_Offload(BPLib_STOR_CACHE_Block_t *svc, BPLib_STOR_CACHE_Sid_t *sid, BPLib_STOR_CACHE_Block_t *pblk)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_Offload, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_Offload, BPLib_STOR_CACHE_Block_t *, svc);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Offload, BPLib_STOR_CACHE_Sid_t *, sid);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Offload, BPLib_STOR_CACHE_Block_t *, pblk);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Offload, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_Offload, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_Query()
 * ----------------------------------------------------
 */
static int BPLib_STOR_QM_Query(BPLib_STOR_CACHE_Block_t *svc, int key, BPLib_STOR_QM_ModuleValtype_t vt,
                                  const void **val)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_Query, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_Query, BPLib_STOR_CACHE_Block_t *, svc);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Query, int, key);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Query, BPLib_STOR_QM_ModuleValtype_t, vt);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Query, const void **, val);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Query, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_Query, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_Release()
 * ----------------------------------------------------
 */
static int BPLib_STOR_CACHE_Release(BPLib_STOR_CACHE_Block_t *svc, BPLib_STOR_CACHE_Sid_t sid)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_Release, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_Release, BPLib_STOR_CACHE_Block_t *, svc);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Release, BPLib_STOR_CACHE_Sid_t, sid);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Release, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_Release, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_Restore()
 * ----------------------------------------------------
 */
static int BPLib_STOR_CACHE_Restore(BPLib_STOR_CACHE_Block_t *svc, BPLib_STOR_CACHE_Sid_t sid, BPLib_STOR_CACHE_Block_t **pblk_out)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_Restore, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_Restore, BPLib_STOR_CACHE_Block_t *, svc);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Restore, BPLib_STOR_CACHE_Sid_t, sid);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_Restore, BPLib_STOR_CACHE_Block_t **, pblk_out);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Restore, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_Restore, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_Start()
 * ----------------------------------------------------
 */
static int BPLib_STOR_QM_Start(BPLib_STOR_CACHE_Block_t *svc)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_Start, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_Start, BPLib_STOR_CACHE_Block_t *, svc);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Start, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_Start, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_Stop()
 * ----------------------------------------------------
 */
static int BPLib_STOR_QM_Stop(BPLib_STOR_CACHE_Block_t *svc)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_Stop, int);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_Stop, BPLib_STOR_CACHE_Block_t *, svc);

    UT_GenStub_Execute(BPLib_STOR_CACHE_Stop, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_Stop, int);
}
