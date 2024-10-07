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
 * Auto-Generated stub implementations for functions defined in bplib_stor_cache_types header
 */

#include "bplib_stor_cache_types.h"
#include "utgenstub.h"

#ifdef UNSTUB // TODO Some "stubs" are implemented ok in src.
/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_BblockCanonicalInit()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_BblockCanonicalInit(BPLib_STOR_CACHE_Block_t           *base_block,
                                          BPLib_STOR_CACHE_BblockCanonical_t *cblk)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_BblockCanonicalInit, BPLib_STOR_CACHE_Block_t *, base_block);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_BblockCanonicalInit, BPLib_STOR_CACHE_BblockCanonical_t *, cblk);

    UT_GenStub_Execute(BPLib_STOR_CACHE_BblockCanonicalInit, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_BblockPrimaryInit()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_BblockPrimaryInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_BblockPrimary_t *pblk)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_BblockPrimaryInit, BPLib_STOR_CACHE_Block_t *, base_block);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_BblockPrimaryInit, BPLib_STOR_CACHE_BblockPrimary_t *, pblk);

    UT_GenStub_Execute(BPLib_STOR_CACHE_BblockPrimaryInit, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_DebugPrintListStats()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_DebugPrintListStats(BPLib_STOR_CACHE_Block_t *list, const char *label)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_DebugPrintListStats, BPLib_STOR_CACHE_Block_t *, list);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_DebugPrintListStats, const char *, label);

    UT_GenStub_Execute(BPLib_STOR_CACHE_DebugPrintListStats, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_DebugScanPool()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_DebugScanPool(BPLib_STOR_CACHE_Pool_t *pool)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_DebugScanPool, BPLib_STOR_CACHE_Pool_t *, pool);

    UT_GenStub_Execute(BPLib_STOR_CACHE_DebugScanPool, Basic, NULL);
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
#endif // UNSTUB

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_DuctInit()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_DuctInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Duct_t *fblk)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_DuctInit, BPLib_STOR_CACHE_Block_t *, base_block);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_DuctInit, BPLib_STOR_CACHE_Duct_t *, fblk);

    UT_GenStub_Execute(BPLib_STOR_CACHE_DuctInit, Basic, NULL);
}

#ifdef UNSTUB
/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_SubqInit()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_SubqInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_SubqBase_t *qblk)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_SubqInit, BPLib_STOR_CACHE_Block_t *, base_block);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_SubqInit, BPLib_STOR_CACHE_SubqBase_t *, qblk);

    UT_GenStub_Execute(BPLib_STOR_CACHE_SubqInit, Basic, NULL);
}
#endif // UNSTUB

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_QueueIngressToParent()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_QueueIngressToParent(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_QueueIngressToParent, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_QueueIngressToParent, void *, arg);
    UT_GenStub_AddParam(BPLib_STOR_QM_QueueIngressToParent, BPLib_STOR_CACHE_Block_t *, subq_src);

    UT_GenStub_Execute(BPLib_STOR_QM_QueueIngressToParent, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_QueueIngressToParent, int);
}