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
 * Auto-Generated stub implementations for functions defined in v7_mpstream header
 */

#include "v7_mpstream.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_StartStreamInit()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_StartStreamInit(BPLib_STOR_CACHE_Stream_t *mps, bplib_mpool_t *pool, BPLib_STOR_CACHE_StreamDir_t dir)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_StartStreamInit, BPLib_STOR_CACHE_Stream_t *, mps);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_StartStreamInit, bplib_mpool_t *, pool);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_StartStreamInit, BPLib_STOR_CACHE_StreamDir_t, dir);

    UT_GenStub_Execute(BPLib_STOR_CACHE_StartStreamInit, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_StreamAttach()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_StreamAttach(BPLib_STOR_CACHE_Stream_t *mps, BPLib_STOR_CACHE_Block_t *head)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_StreamAttach, BPLib_STOR_CACHE_Stream_t *, mps);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_StreamAttach, BPLib_STOR_CACHE_Block_t *, head);

    UT_GenStub_Execute(BPLib_STOR_CACHE_StreamAttach, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_StreamClose()
 * ----------------------------------------------------
 */
void BPLib_STOR_CACHE_StreamClose(BPLib_STOR_CACHE_Stream_t *mps)
{
    UT_GenStub_AddParam(BPLib_STOR_CACHE_StreamClose, BPLib_STOR_CACHE_Stream_t *, mps);

    UT_GenStub_Execute(BPLib_STOR_CACHE_StreamClose, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_StreamRead()
 * ----------------------------------------------------
 */
size_t BPLib_STOR_CACHE_StreamRead(BPLib_STOR_CACHE_Stream_t *mps, void *data, size_t size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_StreamRead, size_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_StreamRead, BPLib_STOR_CACHE_Stream_t *, mps);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_StreamRead, void *, data);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_StreamRead, size_t, size);

    UT_GenStub_Execute(BPLib_STOR_CACHE_StreamRead, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_StreamRead, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_StreamSeek()
 * ----------------------------------------------------
 */
size_t BPLib_STOR_CACHE_StreamSeek(BPLib_STOR_CACHE_Stream_t *mps, size_t target_position)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_StreamSeek, size_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_StreamSeek, BPLib_STOR_CACHE_Stream_t *, mps);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_StreamSeek, size_t, target_position);

    UT_GenStub_Execute(BPLib_STOR_CACHE_StreamSeek, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_StreamSeek, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_StreamWrite()
 * ----------------------------------------------------
 */
size_t BPLib_STOR_CACHE_StreamWrite(BPLib_STOR_CACHE_Stream_t *mps, const void *data, size_t size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_StreamWrite, size_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_StreamWrite, BPLib_STOR_CACHE_Stream_t *, mps);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_StreamWrite, const void *, data);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_StreamWrite, size_t, size);

    UT_GenStub_Execute(BPLib_STOR_CACHE_StreamWrite, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_StreamWrite, size_t);
}
