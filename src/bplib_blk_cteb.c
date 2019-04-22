/************************************************************************
 * File: bplib_blk_cteb.c
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_blk.h"
#include "bplib_blk_cteb.h"
#include "bplib_sdnv.h"
#include "bplib_os.h"

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/
#ifndef _GNU_
/*--------------------------------------------------------------------------------------
 * strnlen -
 *-------------------------------------------------------------------------------------*/
inline int strnlen(const char* str, int maxlen)
{
    int len;
    for(len = 0; len < maxlen; len++)
    {
        if(str[len] == '\0')
        {
            return len;
        }
    }
    return maxlen;
}
#endif

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_blk_cteb_read -
 *
 *  block - pointer to block holding bundle block [INPUT]
 *  size - size of block [INPUT]
 *  cteb - pointer to a custody transfer extension block structure to be populated by this function [OUTPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Next index
 *-------------------------------------------------------------------------------------*/
int bplib_blk_cteb_read (void* block, int size, bp_blk_cteb_t* cteb, bool update_indices)
{
    uint8_t* buffer = (uint8_t*)block;
    uint16_t flags = 0;
    int bytes_read = 0;
    int eid_index = 0;
    int eid_len = 0;
    int eid_status = 0;

    /* Check Size */
    if(size < 1) return BP_BUNDLEPARSEERR;

    /* Read Custody Information */
    if(!update_indices)
    {
        bplib_sdnv_read(buffer, size, &cteb->bf, &flags);
        bplib_sdnv_read(buffer, size, &cteb->blklen, &flags);
    }
    else
    {
        cteb->bf.width = 0;
        cteb->blklen.width = 0;
        cteb->cid.width = 0;

        cteb->bf.index = 1;
        cteb->blklen.index = bplib_sdnv_read(buffer, size, &cteb->bf, &flags);
        cteb->cid.index = bplib_sdnv_read(buffer, size, &cteb->blklen, &flags);
    }

    eid_index = bplib_sdnv_read(buffer, size, &cteb->cid, &flags);
    eid_len = strnlen((char*)&buffer[eid_index], size - eid_index - 1) + 1; // include null-terminator

    eid_status = bplib_eid2ipn((char*)&buffer[eid_index], eid_len, &cteb->cstnode, &cteb->cstserv);
    if(eid_status != BP_SUCCESS) return eid_status;

    bytes_read = eid_index + eid_len;

    /* Success Oriented Error Checking */
    if(flags != 0)  return BP_BUNDLEPARSEERR;
    else            return bytes_read;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_cteb_write -
 *
 *  block - pointer to memory that holds bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  cteb - pointer to a custody transfer extension block structure used to write the block [INPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Number of bytes written
 *-----------------------------------------------------------------,--------------------*/
int bplib_blk_cteb_write (void* block, int size, bp_blk_cteb_t* cteb, bool update_indices)
{
    uint8_t* buffer = (uint8_t*)block;
    uint16_t flags = 0;
    int bytes_written = 0;
    int eid_index = 0;
    int eid_len = 0;

    /* Check Size */
    if(size < 1) return BP_BUNDLEPARSEERR;

    /* Set Block Flags */
    cteb->bf.value |= BP_BLK_REPALL_MASK;

    /* Write Block */
    buffer[0] = BP_CTEB_BLK_TYPE; // block type
    if(!update_indices)
    {
        bplib_sdnv_write(buffer, size, cteb->bf, &flags);
    }
    else
    {
        cteb->bf.width      = 0;
        cteb->blklen.width  = 0;
        cteb->cid.width     = 0;
        cteb->bf.index      = 1;
        cteb->blklen.index  = bplib_sdnv_write(buffer, size, cteb->bf, &flags);
        cteb->cid.index     = cteb->blklen.index + cteb->blklen.width;
    }

    eid_index = bplib_sdnv_write(buffer, size, cteb->cid, &flags);

    eid_len = strnlen(cteb->csteid, BP_MAX_EID_STRING - 1);
    if((eid_index + eid_len + 1) > size) return BP_BUNDLEPARSEERR;
    memcpy(&buffer[eid_index], cteb->csteid, eid_len);
    buffer[eid_index + eid_len] = '\0';

    bytes_written = eid_index + eid_len + 1;
    cteb->blklen.value = bytes_written - cteb->cid.index;
    bplib_sdnv_write(buffer, size, cteb->blklen, &flags);

    /* Success Oriented Error Checking */
    if(flags != 0)  return BP_BUNDLEPARSEERR;
    else            return bytes_written;
}
