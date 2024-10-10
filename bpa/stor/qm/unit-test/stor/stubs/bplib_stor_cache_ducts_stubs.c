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
 * Auto-Generated stub implementations for functions defined in v7_mpool_flows header
 */

#include "bplib_cache_bblocks.h"
#include "bplib_stor_qm.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_FlowAlloc()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_FlowAlloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_FlowAlloc, BPLib_STOR_CACHE_Block_t *);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowAlloc, bplib_mpool_t *, pool);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowAlloc, uint32_t, magic_number);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowAlloc, void *, init_arg);

    UT_GenStub_Execute(BPLib_STOR_CACHE_FlowAlloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_FlowAlloc, BPLib_STOR_CACHE_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_FlowCast()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Flow_t *BPLib_STOR_CACHE_FlowCast(BPLib_STOR_CACHE_Block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_FlowCast, BPLib_STOR_CACHE_Flow_t *);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowCast, BPLib_STOR_CACHE_Block_t *, cb);

    UT_GenStub_Execute(BPLib_STOR_CACHE_FlowCast, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_FlowCast, BPLib_STOR_CACHE_Flow_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_FlowDisable()
 * ----------------------------------------------------
 */
uint32_t BPLib_STOR_CACHE_FlowDisable(BPLib_STOR_CACHE_SubqWorkitem_t *subq)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_FlowDisable, uint32_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowDisable, BPLib_STOR_CACHE_SubqWorkitem_t *, subq);

    UT_GenStub_Execute(BPLib_STOR_CACHE_FlowDisable, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_FlowDisable, uint32_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_FlowEnable()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_FlowEnable(BPLib_STOR_CACHE_SubqWorkitem_t *subq, uint32_t depth_limit)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowEnable, BPLib_STOR_CACHE_SubqWorkitem_t *, subq);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowEnable, uint32_t, depth_limit);

    UT_GenStub_Execute(BPLib_STOR_CACHE_FlowEnable, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_FlowModifyFlags()
 * ----------------------------------------------------
 */
bool BPLib_STOR_CACHE_FlowModifyFlags(BPLib_STOR_CACHE_Block_t *cb, uint32_t set_bits, uint32_t clear_bits)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_FlowModifyFlags, bool);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowModifyFlags, BPLib_STOR_CACHE_Block_t *, cb);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowModifyFlags, uint32_t, set_bits);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowModifyFlags, uint32_t, clear_bits);

    UT_GenStub_Execute(BPLib_STOR_CACHE_FlowModifyFlags, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_FlowModifyFlags, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_FlowTryMoveAll()
 * ----------------------------------------------------
 */
uint32_t BPLib_STOR_CACHE_FlowTryMoveAll(BPLib_STOR_CACHE_SubqWorkitem_t *subq_dst, BPLib_STOR_CACHE_SubqWorkitem_t *subq_src,
                                       bplib_monotime_t abs_timeout)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_FlowTryMoveAll, uint32_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowTryMoveAll, BPLib_STOR_CACHE_SubqWorkitem_t *, subq_dst);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowTryMoveAll, BPLib_STOR_CACHE_SubqWorkitem_t *, subq_src);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowTryMoveAll, bplib_monotime_t, abs_timeout);

    UT_GenStub_Execute(BPLib_STOR_CACHE_FlowTryMoveAll, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_FlowTryMoveAll, uint32_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_FlowTryPull()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_FlowTryPull(BPLib_STOR_CACHE_SubqWorkitem_t *subq_src, bplib_monotime_t abs_timeout)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_FlowTryPull, BPLib_STOR_CACHE_Block_t *);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowTryPull, BPLib_STOR_CACHE_SubqWorkitem_t *, subq_src);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowTryPull, bplib_monotime_t, abs_timeout);

    UT_GenStub_Execute(BPLib_STOR_CACHE_FlowTryPull, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_FlowTryPull, BPLib_STOR_CACHE_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_FlowTryPush()
 * ----------------------------------------------------
 */
bool BPLib_STOR_CACHE_FlowTryPush(BPLib_STOR_CACHE_SubqWorkitem_t *subq_dst, BPLib_STOR_CACHE_Block_t *qblk,
                               bplib_monotime_t abs_timeout)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_FlowTryPush, bool);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowTryPush, BPLib_STOR_CACHE_SubqWorkitem_t *, subq_dst);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowTryPush, BPLib_STOR_CACHE_Block_t *, qblk);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_FlowTryPush, bplib_monotime_t, abs_timeout);

    UT_GenStub_Execute(BPLib_STOR_CACHE_FlowTryPush, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_FlowTryPush, bool);
}
