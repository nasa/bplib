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

#ifndef BPLIB_STOR_CACHE_MPSTREAM_H
#define BPLIB_STOR_CACHE_MPSTREAM_H

#include <string.h>

#include "bplib_api_types.h"
#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"
#include "bplib_mem.h"
#include "crc.h"

typedef enum BPLib_STOR_CACHE_StreamDir
{
    BPLib_STOR_CACHE_StreamDirUndefined,
    BPLib_STOR_CACHE_StreamDirWrite,
    BPLib_STOR_CACHE_StreamDirRead
} BPLib_STOR_CACHE_StreamDir_t;

typedef struct BPLib_STOR_CACHE_Stream
{
    BPLib_STOR_CACHE_StreamDir_t dir;
    BPLib_STOR_CACHE_Pool_t           *pool;
    BPLib_STOR_CACHE_Block_t     *last_eblk;
    BPLib_STOR_CACHE_Block_t      head;
    size_t                   curr_pos;
    size_t                   curr_limit;
    size_t                   stream_position;
} BPLib_STOR_CACHE_Stream_t;

void   BPLib_STOR_CACHE_StartStreamInit(BPLib_STOR_CACHE_Stream_t *mps, BPLib_STOR_CACHE_Pool_t *pool, BPLib_STOR_CACHE_StreamDir_t dir);
size_t BPLib_STOR_CACHE_StreamWrite(BPLib_STOR_CACHE_Stream_t *mps, const void *data, size_t size);
size_t BPLib_STOR_CACHE_StreamRead(BPLib_STOR_CACHE_Stream_t *mps, void *data, size_t size);
size_t BPLib_STOR_CACHE_StreamSeek(BPLib_STOR_CACHE_Stream_t *mps, size_t target_position);
void   BPLib_STOR_CACHE_StreamAttach(BPLib_STOR_CACHE_Stream_t *mps, BPLib_STOR_CACHE_Block_t *head);
static inline size_t BPLib_STOR_CACHE_StreamTell(const BPLib_STOR_CACHE_Stream_t *mps)
{
    return mps->stream_position;
}
void BPLib_STOR_CACHE_StreamClose(BPLib_STOR_CACHE_Stream_t *mps);

#endif /* BPLIB_STOR_CACHE_MPSTREAM_H */
