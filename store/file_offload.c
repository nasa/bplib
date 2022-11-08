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

#include "bplib.h"
#include "v7_cache.h"
#include "v7_mpool_ref.h"
#include "v7_mpool_bblocks.h"
#include "v7_mpstream.h"

/* for now this uses POSIX files directly */
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BPLIB_FILE_PATH_SIZE     128
#define BPLIB_FILE_OFFLOAD_MAGIC 0xdb5e774e

static bplib_mpool_block_t *bplib_file_offload_instantiate(bplib_mpool_ref_t parent, void *init_arg);
static int bplib_file_offload_configure(bplib_mpool_block_t *svc, int key, bplib_cache_module_valtype_t vt,
                                        const void *val);
static int bplib_file_offload_query(bplib_mpool_block_t *svc, int key, bplib_cache_module_valtype_t vt,
                                    const void **val);
static int bplib_file_offload_start(bplib_mpool_block_t *svc);
static int bplib_file_offload_stop(bplib_mpool_block_t *svc);
static int bplib_file_offload_offload(bplib_mpool_block_t *svc, bp_sid_t *sid, bplib_mpool_block_t *pblk);
static int bplib_file_offload_restore(bplib_mpool_block_t *svc, bp_sid_t sid, bplib_mpool_block_t **pblk_out);
static int bplib_file_offload_release(bplib_mpool_block_t *svc, bp_sid_t sid);

typedef struct bplib_file_offload_state
{
    char     base_dir[BPLIB_FILE_PATH_SIZE];
    bp_sid_t last_sid;

} bplib_file_offload_state_t;

typedef struct bplib_file_offload_record
{
    uint32_t check_val;
    uint32_t num_blocks;
    uint32_t num_bytes;
    uint32_t crc;
} bplib_file_offload_record_t;

static const bplib_cache_offload_api_t BPLIB_FILE_OFFLOAD_INTERNAL_API = {
    .std.module_type = bplib_cache_module_type_offload,
    .std.instantiate = bplib_file_offload_instantiate,
    .std.configure   = bplib_file_offload_configure,
    .std.query       = bplib_file_offload_query,
    .std.start       = bplib_file_offload_start,
    .std.stop        = bplib_file_offload_stop,
    .offload         = bplib_file_offload_offload,
    .restore         = bplib_file_offload_restore,
    .release         = bplib_file_offload_release};

const bplib_cache_module_api_t *BPLIB_FILE_OFFLOAD_API =
    (const bplib_cache_module_api_t *)&BPLIB_FILE_OFFLOAD_INTERNAL_API;

int bplib_file_offload_construct_block(void *arg, bplib_mpool_block_t *blk)
{
    return BP_SUCCESS;
}

bplib_mpool_block_t *bplib_file_offload_instantiate(bplib_mpool_ref_t parent, void *init_arg)
{
    bplib_mpool_t *pool;

    static const bplib_mpool_blocktype_api_t offload_block_api = {.construct = bplib_file_offload_construct_block,
                                                                  .destruct  = NULL};

    pool = bplib_mpool_get_parent_pool_from_link(bplib_mpool_dereference(parent));
    bplib_mpool_register_blocktype(pool, BPLIB_FILE_OFFLOAD_MAGIC, &offload_block_api,
                                   sizeof(bplib_file_offload_state_t));

    return bplib_mpool_ref_make_block(parent, BPLIB_FILE_OFFLOAD_MAGIC, init_arg);
}

int bplib_file_offload_configure(bplib_mpool_block_t *svc, int key, bplib_cache_module_valtype_t vt, const void *val)
{
    bplib_file_offload_state_t *state;
    int                         result;

    result = BP_ERROR;
    state  = bplib_mpool_generic_data_cast(svc, BPLIB_FILE_OFFLOAD_MAGIC);
    if (state != NULL)
    {
        switch (key)
        {
            case bplib_cache_confkey_offload_base_dir:
                strncpy(state->base_dir, val, sizeof(state->base_dir) - 1);
                state->base_dir[sizeof(state->base_dir) - 1] = 0;
                result                                       = BP_SUCCESS;
                break;
        }
    }

    return result;
}
int bplib_file_offload_query(bplib_mpool_block_t *svc, int key, bplib_cache_module_valtype_t vt, const void **val)
{
    return 0;
}
int bplib_file_offload_start(bplib_mpool_block_t *svc)
{
    bplib_file_offload_state_t *state;
    int                         result;

    result = BP_ERROR;
    state  = bplib_mpool_generic_data_cast(svc, BPLIB_FILE_OFFLOAD_MAGIC);
    if (state != NULL)
    {
        result = mkdir(state->base_dir, 0700);
        if (result == 0 || errno == EEXIST)
        {
            result = BP_SUCCESS;
        }
        else
        {
            result = BP_ERROR;
        }
    }

    return result;
}
int bplib_file_offload_stop(bplib_mpool_block_t *svc)
{
    return 0;
}

static void bplib_file_offload_sid_to_name(bplib_file_offload_state_t *state, char *name_buf, size_t name_sz,
                                           bp_sid_t sid, bool create_dirs)
{
    char *p;
    int   result;
    char *pd1;
    char *pd2;

    result = snprintf(name_buf, name_sz, "%s/%02x/%02x/%08x.dat", state->base_dir, (unsigned int)((sid >> 0) & 0xFF),
                      (unsigned int)((sid >> 8) & 0xFF), (unsigned int)((sid >> 16) & 0xFFFFFFFF));

    if (result > 0 && result < name_sz && create_dirs)
    {
        pd1 = NULL;
        pd2 = NULL;
        p   = name_buf + result;
        while (p > name_buf)
        {
            --p;
            if (*p == '/')
            {
                if (pd1 == NULL)
                {
                    pd1 = p;
                }
                else if (pd2 == NULL)
                {
                    pd2 = p;
                }
                else
                {
                    break;
                }
            }
        }

        if (pd2 != NULL)
        {
            *pd2   = 0;
            result = mkdir(name_buf, 0755);
            if (result != 0)
            {
                bplog(NULL, BP_FLAG_DIAGNOSTIC, "mkdir(%s): %s\n", name_buf, strerror(errno));
            }
            *pd2 = '/';
        }
        if (pd1 != NULL)
        {
            *pd1   = 0;
            result = mkdir(name_buf, 0755);
            if (result != 0)
            {
                bplog(NULL, BP_FLAG_DIAGNOSTIC, "mkdir(%s): %s\n", name_buf, strerror(errno));
            }
            *pd1 = '/';
        }
    }
}

static int bplib_file_offload_write_block_content(int fd, bplib_file_offload_record_t *rec, const void *ptr, size_t sz)
{
    ssize_t res;
    int     status;

    if (ptr == NULL)
    {
        return BP_ERROR;
    }

    rec->num_bytes += sz;
    rec->crc = bplib_crc_update(&BPLIB_CRC32_CASTAGNOLI, rec->crc, ptr, sz);

    res = write(fd, ptr, sz);
    if (res == sz)
    {
        status = BP_SUCCESS;
    }
    else
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "write(): %s\n", strerror(errno));
        status = BP_ERROR;
    }

    return status;
}

static int bplib_file_offload_read_block_content(int fd, bplib_file_offload_record_t *rec, void *ptr, size_t sz)
{
    ssize_t res;
    int     status;

    if (ptr == NULL)
    {
        return BP_ERROR;
    }

    if (rec->num_bytes < sz)
    {
        return BP_ERROR;
    }

    rec->num_bytes -= sz;

    res = read(fd, ptr, sz);
    if (res == sz)
    {
        status   = BP_SUCCESS;
        rec->crc = bplib_crc_update(&BPLIB_CRC32_CASTAGNOLI, rec->crc, ptr, sz);
    }
    else
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "write(): %s\n", strerror(errno));
        status = BP_ERROR;
    }

    return status;
}

static int bplib_file_offload_write_payload(int fd, bplib_file_offload_record_t *rec,
                                            bplib_mpool_bblock_canonical_t *c_block)
{
    bplib_mpool_list_iter_t it;
    int                     iter_stat;
    int                     write_status;

    write_status = bplib_file_offload_write_block_content(fd, rec, &c_block->block_encode_size_cache,
                                                          sizeof(c_block->block_encode_size_cache));
    if (write_status == BP_SUCCESS)
    {
        write_status = bplib_file_offload_write_block_content(fd, rec, &c_block->encoded_content_length,
                                                              sizeof(c_block->encoded_content_length));
    }
    if (write_status == BP_SUCCESS)
    {
        write_status = bplib_file_offload_write_block_content(fd, rec, &c_block->encoded_content_offset,
                                                              sizeof(c_block->encoded_content_offset));
    }

    iter_stat = bplib_mpool_list_iter_goto_first(&c_block->chunk_list, &it);
    while (write_status == BP_SUCCESS && iter_stat == BP_SUCCESS)
    {
        write_status = bplib_file_offload_write_block_content(fd, rec, bplib_mpool_bblock_cbor_cast(it.position),
                                                              bplib_mpool_get_user_content_size(it.position));
        iter_stat    = bplib_mpool_list_iter_forward(&it);
    }

    return write_status;
}

static int bplib_file_offload_write_blocks(int fd, bplib_file_offload_record_t *rec, bplib_mpool_block_t *blk)
{
    bplib_mpool_bblock_primary_t   *pri_block;
    bplib_mpool_bblock_canonical_t *c_block;
    bplib_mpool_list_iter_t         it;
    int                             iter_stat;
    int                             write_status;

    c_block   = NULL;
    pri_block = bplib_mpool_bblock_primary_cast(blk);
    if (pri_block == NULL)
    {
        return BP_ERROR;
    }

    ++rec->num_blocks;
    write_status = bplib_file_offload_write_block_content(fd, rec, &pri_block->data, sizeof(pri_block->data));

    iter_stat = bplib_mpool_list_iter_goto_first(&pri_block->cblock_list, &it);
    while (write_status == BP_SUCCESS && iter_stat == BP_SUCCESS)
    {
        c_block   = bplib_mpool_bblock_canonical_cast(it.position);
        iter_stat = bplib_mpool_list_iter_forward(&it);
        if (c_block != NULL)
        {
            ++rec->num_blocks;
            write_status =
                bplib_file_offload_write_block_content(fd, rec, &c_block->canonical_logical_data.canonical_block,
                                                       sizeof(c_block->canonical_logical_data.canonical_block));
            if (write_status == BP_SUCCESS)
            {
                if (c_block->canonical_logical_data.canonical_block.blockType == bp_blocktype_payloadBlock)
                {
                    write_status = bplib_file_offload_write_payload(fd, rec, c_block);
                }
                else
                {
                    write_status = bplib_file_offload_write_block_content(
                        fd, rec, &c_block->canonical_logical_data.data, sizeof(c_block->canonical_logical_data.data));
                }
            }
        }
    }

    return write_status;
}

static int bplib_file_offload_read_payload(int fd, bplib_file_offload_record_t *rec, bplib_mpool_t *pool,
                                           bplib_mpool_bblock_canonical_t *c_block)
{
    bplib_mpool_block_t *eblk;
    size_t               chunk_sz;
    int                  read_status;

    eblk = NULL;

    /* payload block: size and offset info written in native form, followed by encoded CBOR data */

    read_status = bplib_file_offload_read_block_content(fd, rec, &c_block->block_encode_size_cache,
                                                        sizeof(c_block->block_encode_size_cache));
    if (read_status == BP_SUCCESS)
    {
        read_status = bplib_file_offload_read_block_content(fd, rec, &c_block->encoded_content_length,
                                                            sizeof(c_block->encoded_content_length));
    }
    if (read_status == BP_SUCCESS)
    {
        read_status = bplib_file_offload_read_block_content(fd, rec, &c_block->encoded_content_offset,
                                                            sizeof(c_block->encoded_content_offset));
    }

    /* The remainder of the chunk is CBOR data */
    if (read_status == BP_SUCCESS)
    {
        while (rec->num_bytes > 0)
        {
            eblk = bplib_mpool_bblock_cbor_alloc(pool);
            if (eblk == NULL)
            {
                /* out of memory */
                read_status = BP_ERROR;
                break;
            }

            chunk_sz = bplib_mpool_get_generic_data_capacity(eblk);
            if (chunk_sz > rec->num_bytes)
            {
                chunk_sz = rec->num_bytes;
            }

            read_status = bplib_file_offload_read_block_content(fd, rec, bplib_mpool_bblock_cbor_cast(eblk), chunk_sz);
            if (read_status != BP_SUCCESS)
            {
                break;
            }

            bplib_mpool_bblock_cbor_set_size(eblk, chunk_sz);
            bplib_mpool_bblock_cbor_append(&c_block->chunk_list, eblk);
            eblk = NULL;
        }
    }

    if (eblk != NULL)
    {
        bplib_mpool_recycle_block(eblk);
    }

    /* This should have read the entire remainder, if not then there was a problem */
    return read_status;
}

static bplib_mpool_block_t *bplib_file_offload_read_blocks(int fd, bplib_file_offload_record_t *rec,
                                                           bplib_mpool_t *pool)
{
    bplib_mpool_block_t            *pblk;
    bplib_mpool_block_t            *cblk;
    bplib_mpool_bblock_primary_t   *pri_block;
    bplib_mpool_bblock_canonical_t *c_block;
    int                             read_status;

    read_status = BP_SUCCESS;

    cblk    = NULL;
    c_block = NULL;
    pblk    = bplib_mpool_bblock_primary_alloc(pool, 0, NULL, BPLIB_MPOOL_ALLOC_PRI_MLO, 0);
    if (pblk != NULL)
    {
        pri_block = bplib_mpool_bblock_primary_cast(pblk);
        assert(pri_block != NULL);

        if (rec->num_blocks > 0)
        {
            read_status = bplib_file_offload_read_block_content(fd, rec, &pri_block->data, sizeof(pri_block->data));
            if (read_status == BP_SUCCESS)
            {
                --rec->num_blocks;
                while (rec->num_blocks > 0)
                {
                    cblk = bplib_mpool_bblock_canonical_alloc(pool, 0, NULL);
                    if (cblk == NULL)
                    {
                        read_status = BP_ERROR;
                        break;
                    }

                    c_block = bplib_mpool_bblock_canonical_cast(cblk);
                    assert(c_block != NULL);

                    --rec->num_blocks;
                    read_status =
                        bplib_file_offload_read_block_content(fd, rec, &c_block->canonical_logical_data.canonical_block,
                                                              sizeof(c_block->canonical_logical_data.canonical_block));
                    if (read_status == BP_SUCCESS)
                    {
                        if (c_block->canonical_logical_data.canonical_block.blockType == bp_blocktype_payloadBlock)
                        {
                            /* payload block has multiple parts */
                            read_status = bplib_file_offload_read_payload(fd, rec, pool, c_block);
                        }
                        else
                        {
                            /* other extension block, the whole thing is written in native form (known size) */
                            read_status =
                                bplib_file_offload_read_block_content(fd, rec, &c_block->canonical_logical_data.data,
                                                                      sizeof(c_block->canonical_logical_data.data));
                        }
                    }
                    if (read_status != BP_SUCCESS)
                    {
                        break;
                    }

                    bplib_mpool_bblock_primary_append(pri_block, cblk);
                    cblk    = NULL;
                    c_block = NULL;
                }
            }
        }
    }

    if (cblk != NULL)
    {
        bplib_mpool_recycle_block(cblk);
    }

    if (read_status != BP_SUCCESS && pblk != NULL)
    {
        bplib_mpool_recycle_block(pblk);
    }

    return pblk;
}

static int bplib_file_offload_offload(bplib_mpool_block_t *svc, bp_sid_t *sid, bplib_mpool_block_t *pblk)
{
    char                        bundle_file[BPLIB_FILE_PATH_SIZE];
    bplib_file_offload_state_t *state;
    bplib_file_offload_record_t rec;
    off_t                       off;
    int                         result;
    int                         fd;

    state = bplib_mpool_generic_data_cast(svc, BPLIB_FILE_OFFLOAD_MAGIC);
    if (state == NULL)
    {
        return bplog(NULL, BP_FLAG_DIAGNOSTIC, "Not a valid offload state object\n");
    }

    result = BP_ERROR;

    ++state->last_sid;
    *sid = state->last_sid;
    bplib_file_offload_sid_to_name(state, bundle_file, sizeof(bundle_file), state->last_sid, true);

    fd = open(bundle_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd >= 0)
    {
        memset(&rec, 0, sizeof(rec));
        rec.check_val = BPLIB_FILE_OFFLOAD_MAGIC;
        rec.crc       = bplib_crc_initial_value(&BPLIB_CRC32_CASTAGNOLI);

        off = lseek(fd, sizeof(rec), SEEK_SET);
        if (off != sizeof(rec))
        {
            bplog(NULL, BP_FLAG_DIAGNOSTIC, "lseek(): %s\n", strerror(errno));
        }

        result = bplib_file_offload_write_blocks(fd, &rec, pblk);
        if (result == BP_SUCCESS)
        {
            fsync(fd);

            lseek(fd, 0, SEEK_SET);
            rec.crc = bplib_crc_finalize(&BPLIB_CRC32_CASTAGNOLI, rec.crc);
            if (write(fd, &rec, sizeof(rec)) != sizeof(rec))
            {
                result = BP_ERROR;
            }
        }

        close(fd);
    }

    return result;
}

static int bplib_file_offload_restore(bplib_mpool_block_t *svc, bp_sid_t sid, bplib_mpool_block_t **pblk_out)
{
    char                        bundle_file[BPLIB_FILE_PATH_SIZE];
    bplib_file_offload_state_t *state;
    bplib_mpool_block_t        *pblk;
    bplib_file_offload_record_t rec;
    bp_crcval_t                 crcval;
    bplib_mpool_t              *pool;
    int                         result;
    int                         fd;

    state = bplib_mpool_generic_data_cast(svc, BPLIB_FILE_OFFLOAD_MAGIC);
    if (state == NULL)
    {
        return bplog(NULL, BP_FLAG_DIAGNOSTIC, "Not a valid offload state object\n");
    }

    pool = bplib_mpool_get_parent_pool_from_link(svc);

    result = BP_ERROR;
    pblk   = NULL;

    bplib_file_offload_sid_to_name(state, bundle_file, sizeof(bundle_file), sid, false);

    do
    {
        fd = open(bundle_file, O_RDONLY);
        if (fd < 0)
        {
            break;
        }

        if (read(fd, &rec, sizeof(rec)) != sizeof(rec))
        {
            break;
        }

        crcval  = rec.crc;
        rec.crc = bplib_crc_initial_value(&BPLIB_CRC32_CASTAGNOLI);

        pblk = bplib_file_offload_read_blocks(fd, &rec, pool);
        if (pblk != NULL)
        {
            rec.crc = bplib_crc_finalize(&BPLIB_CRC32_CASTAGNOLI, rec.crc);
            if (rec.crc == crcval)
            {
                result = BP_SUCCESS;
            }
            else
            {
                bplog(NULL, BP_FLAG_DIAGNOSTIC, "CRC mismatch during bundle restore\n");
            }
        }
    }
    while (false);

    if (fd >= 0)
    {
        close(fd);
    }

    if (pblk != NULL && result != BP_SUCCESS)
    {
        bplib_mpool_recycle_block(pblk);
        pblk = NULL;
    }

    *pblk_out = pblk;

    return result;
}

static int bplib_file_offload_release(bplib_mpool_block_t *svc, bp_sid_t sid)
{
    char                        bundle_file[BPLIB_FILE_PATH_SIZE];
    bplib_file_offload_state_t *state;

    state = bplib_mpool_generic_data_cast(svc, BPLIB_FILE_OFFLOAD_MAGIC);
    if (state == NULL)
    {
        return bplog(NULL, BP_FLAG_DIAGNOSTIC, "Not a valid offload state object\n");
    }

    bplib_file_offload_sid_to_name(state, bundle_file, sizeof(bundle_file), sid, false);

    unlink(bundle_file);

    return 0;
}
