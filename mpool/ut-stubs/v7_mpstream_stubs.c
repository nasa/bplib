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
 * Generated stub function for bplib_mpool_start_stream_init()
 * ----------------------------------------------------
 */
void bplib_mpool_start_stream_init(bplib_mpool_stream_t *mps, bplib_mpool_t *pool, bplib_mpool_stream_dir_t dir)
{
    UT_GenStub_AddParam(bplib_mpool_start_stream_init, bplib_mpool_stream_t *, mps);
    UT_GenStub_AddParam(bplib_mpool_start_stream_init, bplib_mpool_t *, pool);
    UT_GenStub_AddParam(bplib_mpool_start_stream_init, bplib_mpool_stream_dir_t, dir);

    UT_GenStub_Execute(bplib_mpool_start_stream_init, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_stream_attach()
 * ----------------------------------------------------
 */
void bplib_mpool_stream_attach(bplib_mpool_stream_t *mps, bplib_mpool_block_t *head)
{
    UT_GenStub_AddParam(bplib_mpool_stream_attach, bplib_mpool_stream_t *, mps);
    UT_GenStub_AddParam(bplib_mpool_stream_attach, bplib_mpool_block_t *, head);

    UT_GenStub_Execute(bplib_mpool_stream_attach, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_stream_close()
 * ----------------------------------------------------
 */
void bplib_mpool_stream_close(bplib_mpool_stream_t *mps)
{
    UT_GenStub_AddParam(bplib_mpool_stream_close, bplib_mpool_stream_t *, mps);

    UT_GenStub_Execute(bplib_mpool_stream_close, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_stream_read()
 * ----------------------------------------------------
 */
size_t bplib_mpool_stream_read(bplib_mpool_stream_t *mps, void *data, size_t size)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_stream_read, size_t);

    UT_GenStub_AddParam(bplib_mpool_stream_read, bplib_mpool_stream_t *, mps);
    UT_GenStub_AddParam(bplib_mpool_stream_read, void *, data);
    UT_GenStub_AddParam(bplib_mpool_stream_read, size_t, size);

    UT_GenStub_Execute(bplib_mpool_stream_read, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_stream_read, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_stream_seek()
 * ----------------------------------------------------
 */
size_t bplib_mpool_stream_seek(bplib_mpool_stream_t *mps, size_t position)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_stream_seek, size_t);

    UT_GenStub_AddParam(bplib_mpool_stream_seek, bplib_mpool_stream_t *, mps);
    UT_GenStub_AddParam(bplib_mpool_stream_seek, size_t, position);

    UT_GenStub_Execute(bplib_mpool_stream_seek, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_stream_seek, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_stream_write()
 * ----------------------------------------------------
 */
size_t bplib_mpool_stream_write(bplib_mpool_stream_t *mps, const void *data, size_t size)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_stream_write, size_t);

    UT_GenStub_AddParam(bplib_mpool_stream_write, bplib_mpool_stream_t *, mps);
    UT_GenStub_AddParam(bplib_mpool_stream_write, const void *, data);
    UT_GenStub_AddParam(bplib_mpool_stream_write, size_t, size);

    UT_GenStub_Execute(bplib_mpool_stream_write, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_stream_write, size_t);
}
