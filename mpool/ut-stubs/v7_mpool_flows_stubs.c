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

#include "v7_mpool_flows.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_flow_alloc()
 * ----------------------------------------------------
 */
bplib_mpool_block_t *bplib_mpool_flow_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_flow_alloc, bplib_mpool_block_t *);

    UT_GenStub_AddParam(bplib_mpool_flow_alloc, bplib_mpool_t *, pool);
    UT_GenStub_AddParam(bplib_mpool_flow_alloc, uint32_t, magic_number);
    UT_GenStub_AddParam(bplib_mpool_flow_alloc, void *, init_arg);

    UT_GenStub_Execute(bplib_mpool_flow_alloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_flow_alloc, bplib_mpool_block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_flow_cast()
 * ----------------------------------------------------
 */
bplib_mpool_flow_t *bplib_mpool_flow_cast(bplib_mpool_block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_flow_cast, bplib_mpool_flow_t *);

    UT_GenStub_AddParam(bplib_mpool_flow_cast, bplib_mpool_block_t *, cb);

    UT_GenStub_Execute(bplib_mpool_flow_cast, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_flow_cast, bplib_mpool_flow_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_flow_disable()
 * ----------------------------------------------------
 */
uint32_t bplib_mpool_flow_disable(bplib_mpool_subq_workitem_t *subq)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_flow_disable, uint32_t);

    UT_GenStub_AddParam(bplib_mpool_flow_disable, bplib_mpool_subq_workitem_t *, subq);

    UT_GenStub_Execute(bplib_mpool_flow_disable, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_flow_disable, uint32_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_flow_enable()
 * ----------------------------------------------------
 */
void bplib_mpool_flow_enable(bplib_mpool_subq_workitem_t *subq, uint32_t depth_limit)
{
    UT_GenStub_AddParam(bplib_mpool_flow_enable, bplib_mpool_subq_workitem_t *, subq);
    UT_GenStub_AddParam(bplib_mpool_flow_enable, uint32_t, depth_limit);

    UT_GenStub_Execute(bplib_mpool_flow_enable, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_flow_modify_flags()
 * ----------------------------------------------------
 */
bool bplib_mpool_flow_modify_flags(bplib_mpool_block_t *cb, uint32_t set_bits, uint32_t clear_bits)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_flow_modify_flags, bool);

    UT_GenStub_AddParam(bplib_mpool_flow_modify_flags, bplib_mpool_block_t *, cb);
    UT_GenStub_AddParam(bplib_mpool_flow_modify_flags, uint32_t, set_bits);
    UT_GenStub_AddParam(bplib_mpool_flow_modify_flags, uint32_t, clear_bits);

    UT_GenStub_Execute(bplib_mpool_flow_modify_flags, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_flow_modify_flags, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_flow_try_move_all()
 * ----------------------------------------------------
 */
uint32_t bplib_mpool_flow_try_move_all(bplib_mpool_subq_workitem_t *subq_dst, bplib_mpool_subq_workitem_t *subq_src,
                                       uint64_t abs_timeout)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_flow_try_move_all, uint32_t);

    UT_GenStub_AddParam(bplib_mpool_flow_try_move_all, bplib_mpool_subq_workitem_t *, subq_dst);
    UT_GenStub_AddParam(bplib_mpool_flow_try_move_all, bplib_mpool_subq_workitem_t *, subq_src);
    UT_GenStub_AddParam(bplib_mpool_flow_try_move_all, uint64_t, abs_timeout);

    UT_GenStub_Execute(bplib_mpool_flow_try_move_all, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_flow_try_move_all, uint32_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_flow_try_pull()
 * ----------------------------------------------------
 */
bplib_mpool_block_t *bplib_mpool_flow_try_pull(bplib_mpool_subq_workitem_t *subq_src, uint64_t abs_timeout)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_flow_try_pull, bplib_mpool_block_t *);

    UT_GenStub_AddParam(bplib_mpool_flow_try_pull, bplib_mpool_subq_workitem_t *, subq_src);
    UT_GenStub_AddParam(bplib_mpool_flow_try_pull, uint64_t, abs_timeout);

    UT_GenStub_Execute(bplib_mpool_flow_try_pull, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_flow_try_pull, bplib_mpool_block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_flow_try_push()
 * ----------------------------------------------------
 */
bool bplib_mpool_flow_try_push(bplib_mpool_subq_workitem_t *subq_dst, bplib_mpool_block_t *qblk, uint64_t abs_timeout)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_flow_try_push, bool);

    UT_GenStub_AddParam(bplib_mpool_flow_try_push, bplib_mpool_subq_workitem_t *, subq_dst);
    UT_GenStub_AddParam(bplib_mpool_flow_try_push, bplib_mpool_block_t *, qblk);
    UT_GenStub_AddParam(bplib_mpool_flow_try_push, uint64_t, abs_timeout);

    UT_GenStub_Execute(bplib_mpool_flow_try_push, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_flow_try_push, bool);
}
