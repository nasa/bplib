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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <string.h>
#include <assert.h>

#include "bplib.h"
#include "bplib_os.h"
#include "v7_types.h"
#include "v7_mpstream.h"
#include "v7_mpool_bblocks.h"

void bplib_mpool_start_stream_init(bplib_mpool_stream_t *mps, bplib_mpool_t *pool, bplib_mpool_stream_dir_t dir,
                                   bp_crctype_t crctype)
{
    memset(mps, 0, sizeof(*mps));

    bplib_mpool_init_list_head(NULL, &mps->head);

    mps->dir       = dir;
    mps->pool      = pool;
    mps->last_eblk = &mps->head;

    switch (crctype)
    {
        case bp_crctype_CRC16:
            mps->crc_params = &BPLIB_CRC16_X25;
            break;
        case bp_crctype_CRC32C:
            mps->crc_params = &BPLIB_CRC32_CASTAGNOLI;
            break;
        default:
            mps->crc_params = &BPLIB_CRC_NONE;
            break;
    }

    mps->crcval = bplib_crc_initial_value(mps->crc_params);
}

size_t bplib_mpool_stream_write(bplib_mpool_stream_t *mps, const void *data, size_t size)
{
    bplib_mpool_block_t *next_block;
    const uint8_t       *chunk_p;
    uint8_t             *out_p;
    size_t               chunk_sz;
    size_t               remain_sz;

    if (mps->dir != bplib_mpool_stream_dir_write || size == 0)
    {
        return 0;
    }

    remain_sz = size;
    chunk_p   = data;
    while (remain_sz > 0)
    {
        /* If no block is ready, get one now */
        if (mps->curr_pos >= mps->curr_limit)
        {
            next_block = bplib_mpool_bblock_cbor_alloc(mps->pool);
            if (next_block == NULL)
            {
                break;
            }

            bplib_mpool_bblock_cbor_append(&mps->head, next_block);

            mps->last_eblk  = next_block;
            mps->curr_pos   = 0;
            mps->curr_limit = bplib_mpool_get_generic_data_capacity(next_block);
        }

        chunk_sz = mps->curr_limit - mps->curr_pos;
        if (chunk_sz > remain_sz)
        {
            chunk_sz = remain_sz;
        }

        out_p = bplib_mpool_bblock_cbor_cast(mps->last_eblk);
        out_p += mps->curr_pos;
        memcpy(out_p, chunk_p, chunk_sz);
        mps->crcval = bplib_crc_update(mps->crc_params, mps->crcval, out_p, chunk_sz);

        mps->curr_pos += chunk_sz;
        bplib_mpool_bblock_cbor_set_size(mps->last_eblk, mps->curr_pos);
        mps->stream_position += chunk_sz;
        remain_sz -= chunk_sz;
        chunk_p += chunk_sz;
    }

    return (size - remain_sz);
}

size_t bplib_mpool_stream_seek(bplib_mpool_stream_t *mps, size_t target_position)
{
    bplib_mpool_block_t *next_block;
    uint8_t             *curr_p;
    size_t               block_avail;
    size_t               chunk_sz;

    /*
     * Loop to move the stream position forward
     * On a read this should just advance through the existing data
     * On a write this should generate zeros to fill the gap
     * If a CRC calculation is included, this will update the CRC accordingly
     */
    while (target_position > mps->stream_position)
    {
        chunk_sz = target_position - mps->stream_position;

        /* If no block is ready, get one now */
        if (mps->curr_pos >= mps->curr_limit)
        {
            if (mps->dir == bplib_mpool_stream_dir_read)
            {
                next_block = bplib_mpool_get_next_block(mps->last_eblk);
            }
            else if (mps->dir == bplib_mpool_stream_dir_write)
            {
                next_block = bplib_mpool_bblock_cbor_alloc(mps->pool);
            }
            else
            {
                next_block = NULL;
            }

            curr_p = bplib_mpool_bblock_cbor_cast(next_block);
            if (curr_p == NULL)
            {
                break;
            }

            mps->curr_pos  = 0;
            mps->last_eblk = next_block;

            if (mps->dir == bplib_mpool_stream_dir_write)
            {
                bplib_mpool_bblock_cbor_append(&mps->head, next_block);
                mps->curr_limit = bplib_mpool_get_generic_data_capacity(next_block);
            }
            else
            {
                mps->curr_limit = bplib_mpool_get_user_content_size(next_block);
            }
        }
        else
        {
            curr_p = bplib_mpool_bblock_cbor_cast(mps->last_eblk);
        }

        block_avail = mps->curr_limit - mps->curr_pos;

        if (block_avail < chunk_sz)
        {
            chunk_sz = block_avail;
        }

        /*
         * On a write stream this must zero-fill the data
         */
        if (mps->dir == bplib_mpool_stream_dir_write)
        {
            memset(curr_p + mps->curr_pos, 0, chunk_sz);
            bplib_mpool_bblock_cbor_set_size(mps->last_eblk, mps->curr_pos + chunk_sz);
        }

        mps->crcval = bplib_crc_update(mps->crc_params, mps->crcval, curr_p + mps->curr_pos, chunk_sz);
        mps->curr_pos += chunk_sz;
        mps->stream_position += chunk_sz;
    }

    /*
     * Loop to move the stream position backwards
     * On a read this should just back up through the existing data
     * On a write this should drop/delete data
     * If a CRC calculation is being done, this will invalidate the CRC
     */
    while (target_position < mps->stream_position)
    {
        chunk_sz = mps->stream_position - target_position;

        if (mps->curr_pos == 0)
        {
            /* Need to move back a block */
            next_block = bplib_mpool_get_prev_block(mps->last_eblk);

            if (mps->dir == bplib_mpool_stream_dir_write)
            {
                bplib_mpool_recycle_block(mps->last_eblk);
                mps->curr_limit = bplib_mpool_get_generic_data_capacity(next_block);
                mps->curr_pos   = bplib_mpool_get_generic_data_capacity(next_block);
            }
            else
            {
                mps->curr_limit = bplib_mpool_get_user_content_size(next_block);
                mps->curr_pos   = mps->curr_limit;
            }

            mps->last_eblk = next_block;
        }

        if (mps->curr_pos < chunk_sz)
        {
            chunk_sz = mps->curr_pos;
        }

        mps->curr_pos -= chunk_sz;
        mps->stream_position -= chunk_sz;
    }

    return mps->stream_position;
}

size_t bplib_mpool_stream_read(bplib_mpool_stream_t *mps, void *data, size_t size)
{
    bplib_mpool_block_t *next_block;
    const uint8_t       *in_p;
    uint8_t             *chunk_p;
    size_t               chunk_sz;
    size_t               remain_sz;

    if (mps->dir != bplib_mpool_stream_dir_read || size == 0)
    {
        return 0;
    }

    remain_sz = size;
    chunk_p   = data;
    while (remain_sz > 0)
    {
        /* If no block is ready, get one now */
        if (mps->curr_pos >= mps->curr_limit)
        {
            next_block = bplib_mpool_get_next_block(mps->last_eblk);
            in_p       = bplib_mpool_bblock_cbor_cast(next_block);
            if (in_p == NULL)
            {
                /* end of stream */
                break;
            }

            mps->last_eblk  = next_block;
            mps->curr_limit = bplib_mpool_get_user_content_size(next_block);
            mps->curr_pos   = 0;
        }
        else
        {
            in_p = bplib_mpool_bblock_cbor_cast(mps->last_eblk);
        }

        chunk_sz = mps->curr_limit - mps->curr_pos;
        if (chunk_sz > remain_sz)
        {
            chunk_sz = remain_sz;
        }

        in_p += mps->curr_pos;
        memcpy(chunk_p, in_p, chunk_sz);
        mps->crcval = bplib_crc_update(mps->crc_params, mps->crcval, in_p, chunk_sz);

        mps->curr_pos += chunk_sz;
        mps->stream_position += chunk_sz;
        remain_sz -= chunk_sz;
        chunk_p += chunk_sz;
    }

    return (size - remain_sz);
}

void bplib_mpool_stream_attach(bplib_mpool_stream_t *mps, bplib_mpool_block_t *head)
{
    bplib_mpool_merge_list(head, &mps->head);
    bplib_mpool_extract_node(&mps->head);

    mps->last_eblk       = &mps->head;
    mps->curr_limit      = 0;
    mps->curr_pos        = 0;
    mps->stream_position = 0;
    mps->crcval          = bplib_crc_initial_value(mps->crc_params);
}

uint8_t bplib_mpool_stream_get_crc_bit_size(const bplib_mpool_stream_t *mps)
{
    return bplib_crc_get_width(mps->crc_params);
}

bp_crcval_t bplib_mpool_stream_get_current_crc(const bplib_mpool_stream_t *mps)
{
    return bplib_crc_finalize(mps->crc_params, mps->crcval);
}

void bplib_mpool_stream_close(bplib_mpool_stream_t *mps)
{
    /* discard anything that wasn't saved (will be a no-op if it was saved) */
    bplib_mpool_recycle_all_blocks_in_list(mps->pool, &mps->head);
}
