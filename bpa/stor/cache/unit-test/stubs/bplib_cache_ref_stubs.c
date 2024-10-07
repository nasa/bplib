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
 * Auto-Generated stub implementations for functions defined in BPLIB_STOR_CACHE_Ref header
 */

#include "bplib_mem_ref.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_RefCreate()
 * ----------------------------------------------------
 */
BPLIB_STOR_CACHE_Ref_t BPLIB_STOR_CACHE_RefCreate(BPLIB_STOR_CACHE_Block_t *blk)
{
    UT_GenStub_SetupReturnBuffer(BPLIB_STOR_CACHE_RefCreate, BPLIB_STOR_CACHE_Ref_t);

    UT_GenStub_AddParam(BPLIB_STOR_CACHE_RefCreate, BPLIB_STOR_CACHE_Block_t *, blk);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_RefCreate, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLIB_STOR_CACHE_RefCreate, BPLIB_STOR_CACHE_Ref_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_RefDuplicate()
 * ----------------------------------------------------
 */
BPLIB_STOR_CACHE_Ref_t BPLIB_STOR_CACHE_RefDuplicate(BPLIB_STOR_CACHE_Ref_t refptr)
{
    UT_GenStub_SetupReturnBuffer(BPLIB_STOR_CACHE_RefDuplicate, BPLIB_STOR_CACHE_Ref_t);

    UT_GenStub_AddParam(BPLIB_STOR_CACHE_RefDuplicate, BPLIB_STOR_CACHE_Ref_t, refptr);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_RefDuplicate, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLIB_STOR_CACHE_RefDuplicate, BPLIB_STOR_CACHE_Ref_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_RefFromBlock()
 * ----------------------------------------------------
 */
BPLIB_STOR_CACHE_Ref_t BPLIB_STOR_CACHE_RefFromBlock(BPLIB_STOR_CACHE_Block_t *rblk)
{
    UT_GenStub_SetupReturnBuffer(BPLIB_STOR_CACHE_RefFromBlock, BPLIB_STOR_CACHE_Ref_t);

    UT_GenStub_AddParam(BPLIB_STOR_CACHE_RefFromBlock, BPLIB_STOR_CACHE_Block_t *, rblk);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_RefFromBlock, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLIB_STOR_CACHE_RefFromBlock, BPLIB_STOR_CACHE_Ref_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_RefMakeBlock()
 * ----------------------------------------------------
 */
BPLIB_STOR_CACHE_Block_t *BPLIB_STOR_CACHE_RefMakeBlock(BPLIB_STOR_CACHE_Ref_t refptr, uint32_t magic_number, void *init_arg)
{
    UT_GenStub_SetupReturnBuffer(BPLIB_STOR_CACHE_RefMakeBlock, BPLIB_STOR_CACHE_Block_t *);

    UT_GenStub_AddParam(BPLIB_STOR_CACHE_RefMakeBlock, BPLIB_STOR_CACHE_Ref_t, refptr);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_RefMakeBlock, uint32_t, magic_number);
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_RefMakeBlock, void *, init_arg);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_RefMakeBlock, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLIB_STOR_CACHE_RefMakeBlock, BPLIB_STOR_CACHE_Block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLIB_STOR_CACHE_RefRelease()
 * ----------------------------------------------------
 */
void BPLIB_STOR_CACHE_RefRelease(BPLIB_STOR_CACHE_Ref_t refptr)
{
    UT_GenStub_AddParam(BPLIB_STOR_CACHE_RefRelease, BPLIB_STOR_CACHE_Ref_t, refptr);

    UT_GenStub_Execute(BPLIB_STOR_CACHE_RefRelease, Basic, NULL);
}
