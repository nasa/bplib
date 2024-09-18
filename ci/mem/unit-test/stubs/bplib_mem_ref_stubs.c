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
 * Auto-Generated stub implementations for functions defined in v7_mpool_ref header
 */

#include "bplib_mem_ref.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_RefCreate()
 * ----------------------------------------------------
 */
BPLib_MEM_ref_t BPLib_MEM_RefCreate(BPLib_MEM_block_t *blk)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_RefCreate, BPLib_MEM_ref_t);

    UT_GenStub_AddParam(BPLib_MEM_RefCreate, BPLib_MEM_block_t *, blk);

    UT_GenStub_Execute(BPLib_MEM_RefCreate, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_RefCreate, BPLib_MEM_ref_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_RefDuplicate()
 * ----------------------------------------------------
 */
BPLib_MEM_ref_t BPLib_MEM_RefDuplicate(BPLib_MEM_ref_t refptr)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_RefDuplicate, BPLib_MEM_ref_t);

    UT_GenStub_AddParam(BPLib_MEM_RefDuplicate, BPLib_MEM_ref_t, refptr);

    UT_GenStub_Execute(BPLib_MEM_RefDuplicate, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_RefDuplicate, BPLib_MEM_ref_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_RefFromBlock()
 * ----------------------------------------------------
 */
BPLib_MEM_ref_t BPLib_MEM_RefFromBlock(BPLib_MEM_block_t *rblk)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_RefFromBlock, BPLib_MEM_ref_t);

    UT_GenStub_AddParam(BPLib_MEM_RefFromBlock, BPLib_MEM_block_t *, rblk);

    UT_GenStub_Execute(BPLib_MEM_RefFromBlock, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_RefFromBlock, BPLib_MEM_ref_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_RefMakeBlock()
 * ----------------------------------------------------
 */
BPLib_MEM_block_t *BPLib_MEM_RefMakeBlock(BPLib_MEM_ref_t refptr, uint32_t magic_number, void *init_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_MEM_RefMakeBlock, BPLib_MEM_block_t *);

    UT_GenStub_AddParam(BPLib_MEM_RefMakeBlock, BPLib_MEM_ref_t, refptr);
    UT_GenStub_AddParam(BPLib_MEM_RefMakeBlock, uint32_t, magic_number);
    UT_GenStub_AddParam(BPLib_MEM_RefMakeBlock, void *, init_arg);

    UT_GenStub_Execute(BPLib_MEM_RefMakeBlock, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_MEM_RefMakeBlock, BPLib_MEM_block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_MEM_RefRelease()
 * ----------------------------------------------------
 */
void BPLib_MEM_RefRelease(BPLib_MEM_ref_t refptr)
{
    UT_GenStub_AddParam(BPLib_MEM_RefRelease, BPLib_MEM_ref_t, refptr);

    UT_GenStub_Execute(BPLib_MEM_RefRelease, Basic, NULL);
}
