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

#ifndef V7_MPSTREAM_H
#define V7_MPSTREAM_H

#include <string.h>

#include "bplib_api_types.h"
#include "v7_types.h"
#include "v7_mpool.h"
#include "crc.h"

typedef enum bplib_mpool_stream_dir
{
    bplib_mpool_stream_dir_undefined,
    bplib_mpool_stream_dir_write,
    bplib_mpool_stream_dir_read
} bplib_mpool_stream_dir_t;

typedef struct bplib_mpool_stream
{
    bplib_mpool_stream_dir_t dir;
    bplib_mpool_t           *pool;
    bplib_mpool_block_t     *last_eblk;
    bplib_mpool_block_t      head;
    size_t                   curr_pos;
    size_t                   curr_limit;
    size_t                   stream_position;
} bplib_mpool_stream_t;

void   bplib_mpool_start_stream_init(bplib_mpool_stream_t *mps, bplib_mpool_t *pool, bplib_mpool_stream_dir_t dir);
size_t bplib_mpool_stream_write(bplib_mpool_stream_t *mps, const void *data, size_t size);
size_t bplib_mpool_stream_read(bplib_mpool_stream_t *mps, void *data, size_t size);
size_t bplib_mpool_stream_seek(bplib_mpool_stream_t *mps, size_t target_position);
void   bplib_mpool_stream_attach(bplib_mpool_stream_t *mps, bplib_mpool_block_t *head);
static inline size_t bplib_mpool_stream_tell(const bplib_mpool_stream_t *mps)
{
    return mps->stream_position;
}

void bplib_mpool_stream_close(bplib_mpool_stream_t *mps);

#endif /* V7_MPSTREAM_H */
