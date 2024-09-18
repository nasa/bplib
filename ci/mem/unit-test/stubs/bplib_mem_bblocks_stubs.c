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
 * Auto-Generated stub implementations for functions defined in v7_mpool_bblocks header
 */

#include "bplib_mem.h"
#include "bplib_mem_bundle.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BblockCanonicalAlloc()
 * ----------------------------------------------------
 */
BPLib_MEM_block_t *BPLib_MEM_BblockCanonicalAlloc(BPLib_MEM_t *pool, uint32_t magic_number, void *init_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_BblockCanonicalAlloc, BPLib_MEM_block_t *);

    UT_GenStub_AddParam(BPLib_MEM_BblockCanonicalAlloc, BPLib_MEM_t *, pool);
    UT_GenStub_AddParam(BPLib_MEM_BblockCanonicalAlloc, uint32_t, magic_number);
    UT_GenStub_AddParam(BPLib_MEM_BblockCanonicalAlloc, void *, init_arg);

    UT_GenStub_Execute(BPLib_MEM_BblockCanonicalAlloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_BblockCanonicalAlloc, BPLib_MEM_block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BblockCanonicalCast()
 * ----------------------------------------------------
 */
BPLib_MEM_BblockCanonical_t *BPLib_MEM_BblockCanonicalCast(BPLib_MEM_block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_BblockCanonicalCast, BPLib_MEM_BblockCanonical_t *);

    UT_GenStub_AddParam(BPLib_MEM_BblockCanonicalCast, BPLib_MEM_block_t *, cb);

    UT_GenStub_Execute(BPLib_MEM_BblockCanonicalCast, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_BblockCanonicalCast, BPLib_MEM_BblockCanonical_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BblockCanonicalDropEncode()
 * ----------------------------------------------------
 */
void BPLib_MEM_BblockCanonicalDropEncode(BPLib_MEM_BblockCanonical_t *ccb)
{
    UT_GenStub_AddParam(BPLib_MEM_BblockCanonicalDropEncode, BPLib_MEM_BblockCanonical_t *, ccb);

    UT_GenStub_Execute(BPLib_MEM_BblockCanonicalDropEncode, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BblockCborAlloc()
 * ----------------------------------------------------
 */
BPLib_MEM_block_t *BPLib_MEM_BblockCborAlloc(BPLib_MEM_t *pool)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_BblockCborAlloc, BPLib_MEM_block_t *);

    UT_GenStub_AddParam(BPLib_MEM_BblockCborAlloc, BPLib_MEM_t *, pool);

    UT_GenStub_Execute(BPLib_MEM_BblockCborAlloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_BblockCborAlloc, BPLib_MEM_block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BblockCborAppend()
 * ----------------------------------------------------
 */
void BPLib_MEM_BblockCborAppend(BPLib_MEM_block_t *head, BPLib_MEM_block_t *blk)
{
    UT_GenStub_AddParam(BPLib_MEM_BblockCborAppend, BPLib_MEM_block_t *, head);
    UT_GenStub_AddParam(BPLib_MEM_BblockCborAppend, BPLib_MEM_block_t *, blk);

    UT_GenStub_Execute(BPLib_MEM_BblockCborAppend, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BblockCborCast()
 * ----------------------------------------------------
 */
void *BPLib_MEM_BblockCborCast(BPLib_MEM_block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_BblockCborCast, void *);

    UT_GenStub_AddParam(BPLib_MEM_BblockCborCast, BPLib_MEM_block_t *, cb);

    UT_GenStub_Execute(BPLib_MEM_BblockCborCast, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_BblockCborCast, void *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BblockCborExport()
 * ----------------------------------------------------
 */
size_t BPLib_MEM_BblockCborExport(BPLib_MEM_block_t *list, void *out_ptr, size_t max_out_size, size_t seek_start,
                                      size_t max_count)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_BblockCborExport, size_t);

    UT_GenStub_AddParam(BPLib_MEM_BblockCborExport, BPLib_MEM_block_t *, list);
    UT_GenStub_AddParam(BPLib_MEM_BblockCborExport, void *, out_ptr);
    UT_GenStub_AddParam(BPLib_MEM_BblockCborExport, size_t, max_out_size);
    UT_GenStub_AddParam(BPLib_MEM_BblockCborExport, size_t, seek_start);
    UT_GenStub_AddParam(BPLib_MEM_BblockCborExport, size_t, max_count);

    UT_GenStub_Execute(BPLib_MEM_BblockCborExport, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_BblockCborExport, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BblockCborSetSize()
 * ----------------------------------------------------
 */
void BPLib_MEM_BblockCborSetSize(BPLib_MEM_block_t *cb, size_t user_content_size)
{
    UT_GenStub_AddParam(BPLib_MEM_BblockCborSetSize, BPLib_MEM_block_t *, cb);
    UT_GenStub_AddParam(BPLib_MEM_BblockCborSetSize, size_t, user_content_size);

    UT_GenStub_Execute(BPLib_MEM_BblockCborSetSize, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BblockPrimaryAlloc()
 * ----------------------------------------------------
 */
BPLib_MEM_block_t *BPLib_MEM_BblockPrimaryAlloc(BPLib_MEM_t *pool, uint32_t magic_number, void *init_arg,
                                                      uint8_t priority, uint64_t timeout)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_BblockPrimaryAlloc, BPLib_MEM_block_t *);

    UT_GenStub_AddParam(BPLib_MEM_BblockPrimaryAlloc, BPLib_MEM_t *, pool);
    UT_GenStub_AddParam(BPLib_MEM_BblockPrimaryAlloc, uint32_t, magic_number);
    UT_GenStub_AddParam(BPLib_MEM_BblockPrimaryAlloc, void *, init_arg);
    UT_GenStub_AddParam(BPLib_MEM_BblockPrimaryAlloc, uint8_t, priority);
    UT_GenStub_AddParam(BPLib_MEM_BblockPrimaryAlloc, uint64_t, timeout);

    UT_GenStub_Execute(BPLib_MEM_BblockPrimaryAlloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_BblockPrimaryAlloc, BPLib_MEM_block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BblockPrimaryAppend()
 * ----------------------------------------------------
 */
void BPLib_MEM_BblockPrimaryAppend(BPLib_MEM_BblockPrimary_t *cpb, BPLib_MEM_block_t *ccb)
{
    UT_GenStub_AddParam(BPLib_MEM_BblockPrimaryAppend, BPLib_MEM_BblockPrimary_t *, cpb);
    UT_GenStub_AddParam(BPLib_MEM_BblockPrimaryAppend, BPLib_MEM_block_t *, ccb);

    UT_GenStub_Execute(BPLib_MEM_BblockPrimaryAppend, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BblockPrimaryCast()
 * ----------------------------------------------------
 */
BPLib_MEM_BblockPrimary_t *BPLib_MEM_BblockPrimaryCast(BPLib_MEM_block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_BblockPrimaryCast, BPLib_MEM_BblockPrimary_t *);

    UT_GenStub_AddParam(BPLib_MEM_BblockPrimaryCast, BPLib_MEM_block_t *, cb);

    UT_GenStub_Execute(BPLib_MEM_BblockPrimaryCast, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_BblockPrimaryCast, BPLib_MEM_BblockPrimary_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BblockPrimaryDropEncode()
 * ----------------------------------------------------
 */
void BPLib_MEM_BblockPrimaryDropEncode(BPLib_MEM_BblockPrimary_t *cpb)
{
    UT_GenStub_AddParam(BPLib_MEM_BblockPrimaryDropEncode, BPLib_MEM_BblockPrimary_t *, cpb);

    UT_GenStub_Execute(BPLib_MEM_BblockPrimaryDropEncode, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_BblockPrimaryLocateCanonical()
 * ----------------------------------------------------
 */
BPLib_MEM_block_t *BPLib_MEM_BblockPrimaryLocateCanonical(BPLib_MEM_BblockPrimary_t *cpb,
                                                                 bp_blocktype_t                block_type)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_BblockPrimaryLocateCanonical, BPLib_MEM_block_t *);

    UT_GenStub_AddParam(BPLib_MEM_BblockPrimaryLocateCanonical, BPLib_MEM_BblockPrimary_t *, cpb);
    UT_GenStub_AddParam(BPLib_MEM_BblockPrimaryLocateCanonical, bp_blocktype_t, block_type);

    UT_GenStub_Execute(BPLib_MEM_BblockPrimaryLocateCanonical, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_BblockPrimaryLocateCanonical, BPLib_MEM_block_t *);
}
