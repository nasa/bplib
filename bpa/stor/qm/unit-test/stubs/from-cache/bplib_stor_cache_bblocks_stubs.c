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
 * Auto-Generated stub implementations for functions defined in BPLIB_STOR_CACHE_Bblocks header
 */

#include "bplib_mem.h"
#include "utgenstub.h"

#include "bplib_cache.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_BblockCanonicalAlloc()
 * ----------------------------------------------------
 */
BPLIB_STOR_CACHE_Block_t *BPLIB_STOR_CACHE_BblockCanonicalAlloc(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number, void *init_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLIB_STOR_CACHE_BblockCanonicalAlloc, BPLIB_STOR_CACHE_Block_t *);

    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCanonicalAlloc, BPLib_STOR_CACHE_Pool_t *, pool);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCanonicalAlloc, uint32_t, magic_number);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCanonicalAlloc, void *, init_arg);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_BblockCanonicalAlloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLIB_STOR_CACHE_BblockCanonicalAlloc, BPLIB_STOR_CACHE_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_BblockCanonicalCast()
 * ----------------------------------------------------
 */
BPLIB_STOR_CACHE_BblockCanonical_t *BPLIB_STOR_CACHE_BblockCanonicalCast(BPLIB_STOR_CACHE_Block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLIB_STOR_CACHE_BblockCanonicalCast, BPLIB_STOR_CACHE_BblockCanonical_t *);

    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCanonicalCast, BPLIB_STOR_CACHE_Block_t *, cb);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_BblockCanonicalCast, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLIB_STOR_CACHE_BblockCanonicalCast, BPLIB_STOR_CACHE_BblockCanonical_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_BblockCanonicalDropEncode()
 * ----------------------------------------------------
 */
void BPLIB_STOR_CACHE_BblockCanonicalDropEncode(BPLIB_STOR_CACHE_BblockCanonical_t *ccb)
{
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCanonicalDropEncode, BPLIB_STOR_CACHE_BblockCanonical_t *, ccb);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_BblockCanonicalDropEncode, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_BblockCborAlloc()
 * ----------------------------------------------------
 */
BPLIB_STOR_CACHE_Block_t *BPLIB_STOR_CACHE_BblockCborAlloc(BPLib_STOR_CACHE_Pool_t *pool)
{
    UT_GenStub_SetupReturnBuffer(BPLIB_STOR_CACHE_BblockCborAlloc, BPLIB_STOR_CACHE_Block_t *);

    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCborAlloc, BPLib_STOR_CACHE_Pool_t *, pool);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_BblockCborAlloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLIB_STOR_CACHE_BblockCborAlloc, BPLIB_STOR_CACHE_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_BblockCborAppend()
 * ----------------------------------------------------
 */
void BPLIB_STOR_CACHE_BblockCborAppend(BPLIB_STOR_CACHE_Block_t *head, BPLIB_STOR_CACHE_Block_t *blk)
{
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCborAppend, BPLIB_STOR_CACHE_Block_t *, head);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCborAppend, BPLIB_STOR_CACHE_Block_t *, blk);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_BblockCborAppend, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_BblockCborCast()
 * ----------------------------------------------------
 */
void *BPLIB_STOR_CACHE_BblockCborCast(BPLIB_STOR_CACHE_Block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLIB_STOR_CACHE_BblockCborCast, void *);

    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCborCast, BPLIB_STOR_CACHE_Block_t *, cb);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_BblockCborCast, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLIB_STOR_CACHE_BblockCborCast, void *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_BblockCborExport()
 * ----------------------------------------------------
 */
size_t BPLIB_STOR_CACHE_BblockCborExport(BPLIB_STOR_CACHE_Block_t *list, void *out_ptr, size_t max_out_size, size_t seek_start,
                                      size_t max_count)
{
    UT_GenStub_SetupReturnBuffer(BPLIB_STOR_CACHE_BblockCborExport, size_t);

    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCborExport, BPLIB_STOR_CACHE_Block_t *, list);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCborExport, void *, out_ptr);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCborExport, size_t, max_out_size);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCborExport, size_t, seek_start);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCborExport, size_t, max_count);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_BblockCborExport, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLIB_STOR_CACHE_BblockCborExport, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_BblockCborSetSize()
 * ----------------------------------------------------
 */
void BPLIB_STOR_CACHE_BblockCborSetSize(BPLIB_STOR_CACHE_Block_t *cb, size_t user_content_size)
{
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCborSetSize, BPLIB_STOR_CACHE_Block_t *, cb);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockCborSetSize, size_t, user_content_size);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_BblockCborSetSize, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_BblockPrimaryAlloc()
 * ----------------------------------------------------
 */
BPLIB_STOR_CACHE_Block_t *BPLIB_STOR_CACHE_BblockPrimaryAlloc(BPLib_STOR_CACHE_Pool_t *pool, uint32_t magic_number, void *init_arg,
                                                      uint8_t priority, uint64_t timeout)
{
    UT_GenStub_SetupReturnBuffer(BPLIB_STOR_CACHE_BblockPrimaryAlloc, BPLIB_STOR_CACHE_Block_t *);

    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockPrimaryAlloc, BPLib_STOR_CACHE_Pool_t *, pool);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockPrimaryAlloc, uint32_t, magic_number);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockPrimaryAlloc, void *, init_arg);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockPrimaryAlloc, uint8_t, priority);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockPrimaryAlloc, uint64_t, timeout);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_BblockPrimaryAlloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLIB_STOR_CACHE_BblockPrimaryAlloc, BPLIB_STOR_CACHE_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_BblockPrimaryAppend()
 * ----------------------------------------------------
 */
void BPLIB_STOR_CACHE_BblockPrimaryAppend(BPLIB_STOR_CACHE_BblockPrimary_t *cpb, BPLIB_STOR_CACHE_Block_t *ccb)
{
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockPrimaryAppend, BPLIB_STOR_CACHE_BblockPrimary_t *, cpb);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockPrimaryAppend, BPLIB_STOR_CACHE_Block_t *, ccb);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_BblockPrimaryAppend, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_BblockPrimaryCast()
 * ----------------------------------------------------
 */
BPLIB_STOR_CACHE_BblockPrimary_t *BPLIB_STOR_CACHE_BblockPrimaryCast(BPLIB_STOR_CACHE_Block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(BPLIB_STOR_CACHE_BblockPrimaryCast, BPLIB_STOR_CACHE_BblockPrimary_t *);

    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockPrimaryCast, BPLIB_STOR_CACHE_Block_t *, cb);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_BblockPrimaryCast, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLIB_STOR_CACHE_BblockPrimaryCast, BPLIB_STOR_CACHE_BblockPrimary_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_BblockPrimaryDropEncode()
 * ----------------------------------------------------
 */
void BPLIB_STOR_CACHE_BblockPrimaryDropEncode(BPLIB_STOR_CACHE_BblockPrimary_t *cpb)
{
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockPrimaryDropEncode, BPLIB_STOR_CACHE_BblockPrimary_t *, cpb);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_BblockPrimaryDropEncode, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_BblockPrimaryLocateCanonical()
 * ----------------------------------------------------
 */
BPLIB_STOR_CACHE_Block_t *BPLIB_STOR_CACHE_BblockPrimaryLocateCanonical(BPLIB_STOR_CACHE_BblockPrimary_t *cpb,
                                                                 BPLib_STOR_CACHE_Blocktype_t                block_type)
{
    UT_GenStub_SetupReturnBuffer(BPLIB_STOR_CACHE_BblockPrimaryLocateCanonical, BPLIB_STOR_CACHE_Block_t *);

    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockPrimaryLocateCanonical, BPLIB_STOR_CACHE_BblockPrimary_t *, cpb);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_BblockPrimaryLocateCanonical, BPLib_STOR_CACHE_Blocktype_t, block_type);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_BblockPrimaryLocateCanonical, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLIB_STOR_CACHE_BblockPrimaryLocateCanonical, BPLIB_STOR_CACHE_Block_t *);
}
