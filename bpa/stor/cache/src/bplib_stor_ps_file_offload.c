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

/* for now this uses POSIX files directly */
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "crc.h"

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"
#include "bplib_stor_cache_module_api.h"
#include "bplib_stor_cache_internal.h"

#include "bplib_stor_ps_file_offload_types.h"
#include "bplib_stor_ps_file_offload.h"

#define BPLIB_FILE_PATH_SIZE     128
#define BPLIB_FILE_OFFLOAD_MAGIC 0xdb5e774e

static BPLib_STOR_CACHE_Block_t *BPLib_STOR_PS_FileOffloadInstantiate(BPLib_STOR_CACHE_Ref_t parent, void *init_arg);
static int BPLib_STOR_PS_FileOffloadConfigure(BPLib_STOR_CACHE_Block_t *svc, int key, BPLib_STOR_CACHE_ModuleValtype_t vt,
                                        const void *val);
static int BPLib_STOR_PS_FileOffloadQuery(BPLib_STOR_CACHE_Block_t *svc, int key, BPLib_STOR_CACHE_ModuleValtype_t vt,
                                    const void **val);
static int BPLib_STOR_PS_FileOffloadStart(BPLib_STOR_CACHE_Block_t *svc);
static int BPLib_STOR_PS_FileOffloadStop(BPLib_STOR_CACHE_Block_t *svc);
static int BPLib_STOR_PS_FileOffloadOffload(BPLib_STOR_CACHE_Block_t *svc, bp_sid_t *sid, BPLib_STOR_CACHE_Block_t *pblk);
static int BPLib_STOR_PS_FileOffloadRestore(BPLib_STOR_CACHE_Block_t *svc, bp_sid_t sid, BPLib_STOR_CACHE_Block_t **pblk_out);
static int BPLib_STOR_PS_FileOffloadRelease(BPLib_STOR_CACHE_Block_t *svc, bp_sid_t sid);

typedef struct BPLib_STOR_PS_FileOffloadState
{
    char     base_dir[BPLIB_FILE_PATH_SIZE];
    bp_sid_t last_sid;

} BPLib_STOR_PS_FileOffloadState_t;

typedef struct BPLib_STOR_PS_FileOffloadRecord
{
    uint32_t check_val;
    uint32_t num_blocks;
    uint32_t num_bytes;
    uint32_t crc;
} BPLib_STOR_PS_FileOffloadRecord_t;

static const BPLib_STOR_PS_OffloadApi_t BPLIB_FILE_OFFLOAD_INTERNAL_API = {
    .std.module_type = BPLib_STOR_CACHE_ModuleTypeOffload,
    .std.instantiate = BPLib_STOR_PS_FileOffloadInstantiate,
    .std.configure   = BPLib_STOR_PS_FileOffloadConfigure,
    .std.query       = BPLib_STOR_PS_FileOffloadQuery,
    .std.start       = BPLib_STOR_PS_FileOffloadStart,
    .std.stop        = BPLib_STOR_PS_FileOffloadStop,
    .offload         = BPLib_STOR_PS_FileOffloadOffload,
    .restore         = BPLib_STOR_PS_FileOffloadRestore,
    .release         = BPLib_STOR_PS_FileOffloadRelease};

const BPLib_STOR_CACHE_ModuleApi_t *BPLIB_FILE_OFFLOAD_API =
    (const BPLib_STOR_CACHE_ModuleApi_t *)&BPLIB_FILE_OFFLOAD_INTERNAL_API;

int BPLib_STOR_PS_FileOffloadConstruct_block(void *arg, BPLib_STOR_CACHE_Block_t *blk)
{
    return BPLIB_SUCCESS;
}

BPLib_STOR_CACHE_Block_t *BPLib_STOR_PS_FileOffloadInstantiate(BPLib_STOR_CACHE_Ref_t parent, void *init_arg)
{
    BPLib_STOR_CACHE_Pool_t *pool;

    static const BPLib_STOR_CACHE_BlocktypeApi_t offload_block_api = {.construct = BPLib_STOR_CACHE_ConstructBlockref,
                                                                      .destruct  = NULL};

    pool = BPLib_STOR_CACHE_MpoolGetParentPoolFromLink(BPLib_STOR_CACHE_MpoolDereference(parent));
    BPLib_STOR_CACHE_MpoolRegisterBlocktype(pool, BPLIB_FILE_OFFLOAD_MAGIC, &offload_block_api,
                                   sizeof(BPLib_STOR_PS_FileOffloadState_t));

    return BPLib_STOR_CACHE_MpoolRefMakeBlock(parent, BPLIB_FILE_OFFLOAD_MAGIC, init_arg);
}

int BPLib_STOR_PS_FileOffloadConfigure(BPLib_STOR_CACHE_Block_t *svc, int key, BPLib_STOR_CACHE_ModuleValtype_t vt, const void *val)
{
    BPLib_STOR_PS_FileOffloadState_t *state;
    int                         result;

    result = BPLIB_ERROR;
    state  = BPLib_STOR_CACHE_MpoolGenericDataCast(svc, BPLIB_FILE_OFFLOAD_MAGIC);
    if (state != NULL)
    {
        switch (key)
        {
            case BPLib_STOR_CACHE_ConfkeyOffloadBaseDir:
                strncpy(state->base_dir, val, sizeof(state->base_dir) - 1);
                state->base_dir[sizeof(state->base_dir) - 1] = 0;
                result                                       = BPLIB_SUCCESS;
                break;
        }
    }

    return result;
}
int BPLib_STOR_PS_FileOffloadQuery(BPLib_STOR_CACHE_Block_t *svc, int key, BPLib_STOR_CACHE_ModuleValtype_t vt, const void **val)
{
    return 0;
}
int BPLib_STOR_PS_FileOffloadStart(BPLib_STOR_CACHE_Block_t *svc)
{
    BPLib_STOR_PS_FileOffloadState_t *state;
    int                         result;

    result = BPLIB_ERROR;
    state  = BPLib_STOR_CACHE_MpoolGenericDataCast(svc, BPLIB_FILE_OFFLOAD_MAGIC);
    if (state != NULL)
    {
        result = mkdir(state->base_dir, 0700);
        if (result == 0 || errno == EEXIST)
        {
            result = BPLIB_SUCCESS;
        }
        else
        {
            result = BPLIB_ERROR;
        }
    }

    return result;
}
int BPLib_STOR_PS_FileOffloadStop(BPLib_STOR_CACHE_Block_t *svc)
{
    return 0;
}

static void BPLib_STOR_PS_FileOffloadSidToName(BPLib_STOR_PS_FileOffloadState_t *state, char *name_buf, size_t name_sz,
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
                bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "mkdir(%s): %s\n", name_buf, strerror(errno));
            }
            *pd2 = '/';
        }
        if (pd1 != NULL)
        {
            *pd1   = 0;
            result = mkdir(name_buf, 0755);
            if (result != 0)
            {
                bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "mkdir(%s): %s\n", name_buf, strerror(errno));
            }
            *pd1 = '/';
        }
    }
}

static int BPLib_STOR_PS_FileOffloadWrite_block_content(int fd, BPLib_STOR_PS_FileOffloadRecord_t *rec, const void *ptr, size_t sz)
{
    ssize_t res;
    int     status;

    if (ptr == NULL)
    {
        return BPLIB_ERROR;
    }

    rec->num_bytes += sz;
    rec->crc = BPLib_STOR_CACHE_CrcUpdate(&BPLIB_CRC32_CASTAGNOLI, rec->crc, ptr, sz);

    res = write(fd, ptr, sz);
    if (res == sz)
    {
        status = BPLIB_SUCCESS;
    }
    else
    {
        bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "write(): %s\n", strerror(errno));
        status = BPLIB_ERROR;
    }

    return status;
}

static int BPLib_STOR_PS_FileOffloadRead_block_content(int fd, BPLib_STOR_PS_FileOffloadRecord_t *rec, void *ptr, size_t sz)
{
    ssize_t res;
    int     status;

    if (ptr == NULL)
    {
        return BPLIB_ERROR;
    }

    if (rec->num_bytes < sz)
    {
        return BPLIB_ERROR;
    }

    rec->num_bytes -= sz;

    res = read(fd, ptr, sz);
    if (res == sz)
    {
        status   = BPLIB_SUCCESS;
        rec->crc = BPLib_STOR_CACHE_CrcUpdate(&BPLIB_CRC32_CASTAGNOLI, rec->crc, ptr, sz);
    }
    else
    {
        bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "write(): %s\n", strerror(errno));
        status = BPLIB_ERROR;
    }

    return status;
}

static int BPLib_STOR_PS_FileOffloadWrite_payload(int fd, BPLib_STOR_PS_FileOffloadRecord_t *rec,
                                            BPLib_STOR_CACHE_BblockCanonical_t *c_block)
{
    BPLib_STOR_CACHE_ListIter_t it;
    int                     iter_stat;
    int                     write_status;

    write_status = BPLib_STOR_PS_FileOffloadWriteBlockContent(fd, rec, &c_block->block_encode_size_cache,
                                                          sizeof(c_block->block_encode_size_cache));
    if (write_status == BPLIB_SUCCESS)
    {
        write_status = BPLib_STOR_PS_FileOffloadWriteBlockContent(fd, rec, &c_block->encoded_content_length,
                                                              sizeof(c_block->encoded_content_length));
    }
    if (write_status == BPLIB_SUCCESS)
    {
        write_status = BPLib_STOR_PS_FileOffloadWriteBlockContent(fd, rec, &c_block->encoded_content_offset,
                                                              sizeof(c_block->encoded_content_offset));
    }

    iter_stat = BPLib_STOR_CACHE_MpoolListIterGotoFirst(&c_block->chunk_list, &it);
    while (write_status == BPLIB_SUCCESS && iter_stat == BPLIB_SUCCESS)
    {
        write_status = BPLib_STOR_PS_FileOffloadWriteBlockContent(fd, rec, BPLib_STOR_CACHE_MpoolBblockCborCast(it.position),
                                                              BPLib_STOR_CACHE_MpoolGetUserContentSize(it.position));
        iter_stat    = BPLib_STOR_CACHE_MpoolListIterForward(&it);
    }

    return write_status;
}

static int BPLib_STOR_PS_FileOffloadWrite_blocks(int fd, BPLib_STOR_PS_FileOffloadRecord_t *rec, BPLib_STOR_CACHE_Block_t *blk)
{
    BPLib_STOR_CACHE_BblockPrimary_t   *pri_block;
    BPLib_STOR_CACHE_BblockCanonical_t *c_block;
    BPLib_STOR_CACHE_ListIter_t         it;
    int                             iter_stat;
    int                             write_status;

    c_block   = NULL;
    pri_block = BPLib_STOR_CACHE_MpoolBblockPrimaryCast(blk);
    if (pri_block == NULL)
    {
        return BPLIB_ERROR;
    }

    ++rec->num_blocks;
    write_status = BPLib_STOR_PS_FileOffloadWriteBlockContent(fd, rec, &pri_block->data, sizeof(pri_block->data));

    iter_stat = BPLib_STOR_CACHE_MpoolListIterGotoFirst(&pri_block->cblock_list, &it);
    while (write_status == BPLIB_SUCCESS && iter_stat == BPLIB_SUCCESS)
    {
        c_block   = BPLib_STOR_CACHE_MpoolBblockCanonicalCast(it.position);
        iter_stat = BPLib_STOR_CACHE_MpoolListIterForward(&it);
        if (c_block != NULL)
        {
            ++rec->num_blocks;
            write_status =
                BPLib_STOR_PS_FileOffloadWriteBlockContent(fd, rec, &c_block->canonical_logical_data.canonical_block,
                                                       sizeof(c_block->canonical_logical_data.canonical_block));
            if (write_status == BPLIB_SUCCESS)
            {
                if (c_block->canonical_logical_data.canonical_block.blockType == bp_blocktype_payloadBlock)
                {
                    write_status = BPLib_STOR_PS_FileOffloadWritePayload(fd, rec, c_block);
                }
                else
                {
                    write_status = BPLib_STOR_PS_FileOffloadWriteBlockContent(
                        fd, rec, &c_block->canonical_logical_data.data, sizeof(c_block->canonical_logical_data.data));
                }
            }
        }
    }

    return write_status;
}

static int BPLib_STOR_PS_FileOffloadRead_payload(int fd, BPLib_STOR_PS_FileOffloadRecord_t *rec, BPLib_STOR_CACHE_Pool_t *pool,
                                           BPLib_STOR_CACHE_BblockCanonical_t *c_block)
{
    BPLib_STOR_CACHE_Block_t *eblk;
    size_t               chunk_sz;
    int                  read_status;

    eblk = NULL;

    /* payload block: size and offset info written in native form, followed by encoded CBOR data */

    read_status = BPLib_STOR_PS_FileOffloadReadBlockContent(fd, rec, &c_block->block_encode_size_cache,
                                                        sizeof(c_block->block_encode_size_cache));
    if (read_status == BPLIB_SUCCESS)
    {
        read_status = BPLib_STOR_PS_FileOffloadReadBlockContent(fd, rec, &c_block->encoded_content_length,
                                                            sizeof(c_block->encoded_content_length));
    }
    if (read_status == BPLIB_SUCCESS)
    {
        read_status = BPLib_STOR_PS_FileOffloadReadBlockContent(fd, rec, &c_block->encoded_content_offset,
                                                            sizeof(c_block->encoded_content_offset));
    }

    /* The remainder of the chunk is CBOR data */
    if (read_status == BPLIB_SUCCESS)
    {
        while (rec->num_bytes > 0)
        {
            eblk = BPLib_STOR_CACHE_MpoolBblockCborAlloc(pool);
            if (eblk == NULL)
            {
                /* out of memory */
                read_status = BPLIB_ERROR;
                break;
            }

            chunk_sz = BPLib_STOR_CACHE_MpoolGetGenericDataCapacity(eblk);
            if (chunk_sz > rec->num_bytes)
            {
                chunk_sz = rec->num_bytes;
            }

            read_status = BPLib_STOR_PS_FileOffloadReadBlockContent(fd, rec, BPLib_STOR_CACHE_MpoolBblockCborCast(eblk), chunk_sz);
            if (read_status != BPLIB_SUCCESS)
            {
                break;
            }

            BPLib_STOR_CACHE_MpoolBblockCborSetSize(eblk, chunk_sz);
            BPLib_STOR_CACHE_MpoolBblockCborAppend(&c_block->chunk_list, eblk);
            eblk = NULL;
        }
    }

    if (eblk != NULL)
    {
        BPLib_STOR_CACHE_MpoolRecycleBlock(eblk);
    }

    /* This should have read the entire remainder, if not then there was a problem */
    return read_status;
}

static BPLib_STOR_CACHE_Block_t *BPLib_STOR_PS_FileOffloadReadBlocks(int fd, BPLib_STOR_PS_FileOffloadRecord_t *rec,
                                                           BPLib_STOR_CACHE_Pool_t *pool)
{
    BPLib_STOR_CACHE_Block_t            *pblk;
    BPLib_STOR_CACHE_Block_t            *cblk;
    BPLib_STOR_CACHE_BblockPrimary_t   *pri_block;
    BPLib_STOR_CACHE_BblockCanonical_t *c_block;
    int                             read_status;

    read_status = BPLIB_SUCCESS;

    cblk    = NULL;
    c_block = NULL;
    pblk    = BPLib_STOR_CACHE_MpoolBblockPrimaryAlloc(pool, 0, NULL, BPLIB_MPOOL_ALLOC_PRI_MLO, BPLIB_MONOTIME_ZERO);
    if (pblk != NULL)
    {
        pri_block = BPLib_STOR_CACHE_MpoolBblockPrimaryCast(pblk);
        assert(pri_block != NULL);

        if (rec->num_blocks > 0)
        {
            read_status = BPLib_STOR_PS_FileOffloadReadBlockContent(fd, rec, &pri_block->data, sizeof(pri_block->data));
            if (read_status == BPLIB_SUCCESS)
            {
                --rec->num_blocks;
                while (rec->num_blocks > 0)
                {
                    cblk = BPLib_STOR_CACHE_MpoolBblockCanonicalAlloc(pool, 0, NULL);
                    if (cblk == NULL)
                    {
                        read_status = BPLIB_ERROR;
                        break;
                    }

                    c_block = BPLib_STOR_CACHE_MpoolBblockCanonicalCast(cblk);
                    assert(c_block != NULL);

                    --rec->num_blocks;
                    read_status =
                        BPLib_STOR_PS_FileOffloadReadBlockContent(fd, rec, &c_block->canonical_logical_data.canonical_block,
                                                              sizeof(c_block->canonical_logical_data.canonical_block));
                    if (read_status == BPLIB_SUCCESS)
                    {
                        if (c_block->canonical_logical_data.canonical_block.blockType == bp_blocktype_payloadBlock)
                        {
                            /* payload block has multiple parts */
                            read_status = BPLib_STOR_PS_FileOffloadReadPayload(fd, rec, pool, c_block);
                        }
                        else
                        {
                            /* other extension block, the whole thing is written in native form (known size) */
                            read_status =
                                BPLib_STOR_PS_FileOffloadReadBlockContent(fd, rec, &c_block->canonical_logical_data.data,
                                                                      sizeof(c_block->canonical_logical_data.data));
                        }
                    }
                    if (read_status != BPLIB_SUCCESS)
                    {
                        break;
                    }

                    BPLib_STOR_CACHE_MpoolBblockPrimaryAppend(pri_block, cblk);
                    cblk    = NULL;
                    c_block = NULL;
                }
            }
        }
    }

    if (cblk != NULL)
    {
        BPLib_STOR_CACHE_MpoolRecycleBlock(cblk);
    }

    if (read_status != BPLIB_SUCCESS && pblk != NULL)
    {
        BPLib_STOR_CACHE_MpoolRecycleBlock(pblk);
    }

    return pblk;
}

static int BPLib_STOR_PS_FileOffloadOffload(BPLib_STOR_CACHE_Block_t *svc, bp_sid_t *sid, BPLib_STOR_CACHE_Block_t *pblk)
{
    char                        bundle_file[BPLIB_FILE_PATH_SIZE];
    BPLib_STOR_PS_FileOffloadState_t *state;
    BPLib_STOR_PS_FileOffloadRecord_t rec;
    off_t                       off;
    int                         result;
    int                         fd;

    state = BPLib_STOR_CACHE_MpoolGenericDataCast(svc, BPLIB_FILE_OFFLOAD_MAGIC);
    if (state == NULL)
    {
        return bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "Not a valid offload state object\n");
    }

    result = BPLIB_ERROR;

    ++state->last_sid;
    *sid = state->last_sid;
    BPLib_STOR_PS_FileOffloadSidToName(state, bundle_file, sizeof(bundle_file), state->last_sid, true);

    fd = open(bundle_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd >= 0)
    {
        memset(&rec, 0, sizeof(rec));
        rec.check_val = BPLIB_FILE_OFFLOAD_MAGIC;
        rec.crc       = BPLib_STOR_CACHE_CrcInitialValue(&BPLIB_CRC32_CASTAGNOLI);

        off = lseek(fd, sizeof(rec), SEEK_SET);
        if (off != sizeof(rec))
        {
            bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "lseek(): %s\n", strerror(errno));
        }

        result = BPLib_STOR_PS_FileOffloadWriteBlocks(fd, &rec, pblk);
        if (result == BPLIB_SUCCESS)
        {
            fsync(fd);

            lseek(fd, 0, SEEK_SET);
            rec.crc = BPLib_STOR_CACHE_CrcFinalize(&BPLIB_CRC32_CASTAGNOLI, rec.crc);
            if (write(fd, &rec, sizeof(rec)) != sizeof(rec))
            {
                result = BPLIB_ERROR;
            }
        }

        close(fd);
    }

    return result;
}

static int BPLib_STOR_PS_FileOffloadRestore(BPLib_STOR_CACHE_Block_t *svc, bp_sid_t sid, BPLib_STOR_CACHE_Block_t **pblk_out)
{
    char                        bundle_file[BPLIB_FILE_PATH_SIZE];
    BPLib_STOR_PS_FileOffloadState_t *state;
    BPLib_STOR_CACHE_Block_t        *pblk;
    BPLib_STOR_PS_FileOffloadRecord_t rec;
    bp_crcval_t                 crcval;
    BPLib_STOR_CACHE_Pool_t              *pool;
    int                         result;
    int                         fd;

    state = BPLib_STOR_CACHE_MpoolGenericDataCast(svc, BPLIB_FILE_OFFLOAD_MAGIC);
    if (state == NULL)
    {
        return bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "Not a valid offload state object\n");
    }

    pool = BPLib_STOR_CACHE_MpoolGetParentPoolFromLink(svc);

    result = BPLIB_ERROR;
    pblk   = NULL;

    BPLib_STOR_PS_FileOffloadSidToName(state, bundle_file, sizeof(bundle_file), sid, false);

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
        rec.crc = BPLib_STOR_CACHE_CrcInitialValue(&BPLIB_CRC32_CASTAGNOLI);

        pblk = BPLib_STOR_PS_FileOffloadReadBlocks(fd, &rec, pool);
        if (pblk != NULL)
        {
            rec.crc = BPLib_STOR_CACHE_CrcFinalize(&BPLIB_CRC32_CASTAGNOLI, rec.crc);
            if (rec.crc == crcval)
            {
                result = BPLIB_SUCCESS;
            }
            else
            {
                bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "CRC mismatch during bundle restore\n");
            }
        }
    }
    while (false);

    if (fd >= 0)
    {
        close(fd);
    }

    if (pblk != NULL && result != BPLIB_SUCCESS)
    {
        BPLib_STOR_CACHE_MpoolRecycleBlock(pblk);
        pblk = NULL;
    }

    *pblk_out = pblk;

    return result;
}

static int BPLib_STOR_PS_FileOffloadRelease(BPLib_STOR_CACHE_Block_t *svc, bp_sid_t sid)
{
    char                        bundle_file[BPLIB_FILE_PATH_SIZE];
    BPLib_STOR_PS_FileOffloadState_t *state;

    state = BPLib_STOR_CACHE_MpoolGenericDataCast(svc, BPLIB_FILE_OFFLOAD_MAGIC);
    if (state == NULL)
    {
        return bplog(NULL, BPLIB_MEM_FLAG_DIAGNOSTIC, "Not a valid offload state object\n");
    }

    BPLib_STOR_PS_FileOffloadSidToName(state, bundle_file, sizeof(bundle_file), sid, false);

    unlink(bundle_file);

    return 0;
}
