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
 * Auto-Generated stub implementations for functions defined in bplib_stor_qm_ducts header
 */

#include "bplib_stor_qm_ducts.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_DuctAlloc()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_QM_DuctAlloc(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number, void *init_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_DuctAlloc, BPLib_STOR_CACHE_Block_t *);

    UT_GenStub_AddParam(BPLib_STOR_QM_DuctAlloc, BPLib_STOR_CACHE_Pool_t *, pool);
    UT_GenStub_AddParam(BPLib_STOR_QM_DuctAlloc, uint32_t, magic_number);
    UT_GenStub_AddParam(BPLib_STOR_QM_DuctAlloc, void *, init_arg);

    UT_GenStub_Execute(BPLib_STOR_QM_DuctAlloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_DuctAlloc, BPLib_STOR_CACHE_Block_t *);
}

#ifdef QM_SUBQ
/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_DuctDisable()
 * ----------------------------------------------------
 */
uint32_t BPLib_STOR_QM_DuctDisable(BPLib_STOR_CACHE_SubqWorkitem_t *subq)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_DuctDisable, uint32_t);

    UT_GenStub_AddParam(BPLib_STOR_QM_DuctDisable, BPLib_STOR_CACHE_SubqWorkitem_t *, subq);

    UT_GenStub_Execute(BPLib_STOR_QM_DuctDisable, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_DuctDisable, uint32_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_DuctEnable()
 * ----------------------------------------------------
 */
void BPLib_STOR_QM_DuctEnable(BPLib_STOR_CACHE_SubqWorkitem_t *subq, uint32_t depth_limit)
{
    UT_GenStub_AddParam(BPLib_STOR_QM_DuctEnable, BPLib_STOR_CACHE_SubqWorkitem_t *, subq);
    UT_GenStub_AddParam(BPLib_STOR_QM_DuctEnable, uint32_t, depth_limit);

    UT_GenStub_Execute(BPLib_STOR_QM_DuctEnable, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_DuctModifyFlags()
 * ----------------------------------------------------
 */
bool BPLib_STOR_QM_DuctModifyFlags(BPLib_STOR_CACHE_Block_t *cb, uint32_t set_bits, uint32_t clear_bits)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_DuctModifyFlags, bool);

    UT_GenStub_AddParam(BPLib_STOR_QM_DuctModifyFlags, BPLib_STOR_CACHE_Block_t *, cb);
    UT_GenStub_AddParam(BPLib_STOR_QM_DuctModifyFlags, uint32_t, set_bits);
    UT_GenStub_AddParam(BPLib_STOR_QM_DuctModifyFlags, uint32_t, clear_bits);

    UT_GenStub_Execute(BPLib_STOR_QM_DuctModifyFlags, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_DuctModifyFlags, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_DuctTryMoveAll()
 * ----------------------------------------------------
 */
uint32_t BPLib_STOR_QM_DuctTryMoveAll(BPLib_STOR_CACHE_SubqWorkitem_t *subq_dst,
                                      BPLib_STOR_CACHE_SubqWorkitem_t *subq_src, BPLib_TIME_MonotonicTime_t abs_timeout)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_DuctTryMoveAll, uint32_t);

    UT_GenStub_AddParam(BPLib_STOR_QM_DuctTryMoveAll, BPLib_STOR_CACHE_SubqWorkitem_t *, subq_dst);
    UT_GenStub_AddParam(BPLib_STOR_QM_DuctTryMoveAll, BPLib_STOR_CACHE_SubqWorkitem_t *, subq_src);
    UT_GenStub_AddParam(BPLib_STOR_QM_DuctTryMoveAll, BPLib_TIME_MonotonicTime_t, abs_timeout);

    UT_GenStub_Execute(BPLib_STOR_QM_DuctTryMoveAll, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_DuctTryMoveAll, uint32_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_DuctTryPull()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_QM_DuctTryPull(BPLib_STOR_CACHE_SubqWorkitem_t *subq_src,
                                                    BPLib_TIME_MonotonicTime_t       abs_timeout)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_DuctTryPull, BPLib_STOR_CACHE_Block_t *);

    UT_GenStub_AddParam(BPLib_STOR_QM_DuctTryPull, BPLib_STOR_CACHE_SubqWorkitem_t *, subq_src);
    UT_GenStub_AddParam(BPLib_STOR_QM_DuctTryPull, BPLib_TIME_MonotonicTime_t, abs_timeout);

    UT_GenStub_Execute(BPLib_STOR_QM_DuctTryPull, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_DuctTryPull, BPLib_STOR_CACHE_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_DuctTryPush()
 * ----------------------------------------------------
 */
bool BPLib_STOR_QM_DuctTryPush(BPLib_STOR_CACHE_SubqWorkitem_t *subq_dst, BPLib_STOR_CACHE_Block_t *qblk,
                               BPLib_TIME_MonotonicTime_t abs_timeout)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_DuctTryPush, bool);

    UT_GenStub_AddParam(BPLib_STOR_QM_DuctTryPush, BPLib_STOR_CACHE_SubqWorkitem_t *, subq_dst);
    UT_GenStub_AddParam(BPLib_STOR_QM_DuctTryPush, BPLib_STOR_CACHE_Block_t *, qblk);
    UT_GenStub_AddParam(BPLib_STOR_QM_DuctTryPush, BPLib_TIME_MonotonicTime_t, abs_timeout);

    UT_GenStub_Execute(BPLib_STOR_QM_DuctTryPush, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_DuctTryPush, bool);
}
#endif // QM_SUBQ