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
#include "bplib_os.h"

#include "cteb.h"
#include "sdnv.h"
#include "v6.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * cteb_read -
 *
 *  block - pointer to block holding bundle block [INPUT]
 *  size - size of block [INPUT]
 *  cteb - pointer to a custody transfer extension block structure to be populated by this function [OUTPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Next index
 *-------------------------------------------------------------------------------------*/
int cteb_read(const void *block, int size, bp_blk_cteb_t *cteb, bool update_indices, uint32_t *flags)
{
    uint8_t *buffer     = (uint8_t *)block;
    int      bytes_read = 0;
    int      eid_index  = 0;
    int      eid_len    = 0;
    int      eid_status = 0;
    uint32_t sdnvflags  = 0;

    /* Check Size */
    if (size < 1)
    {
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Invalid size for the CTEB block: %d\n", size);
    }

    /* Read Custody Information */
    if (!update_indices)
    {
        sdnv_read(buffer, size, &cteb->bf, &sdnvflags);
        sdnv_read(buffer, size, &cteb->blklen, &sdnvflags);
    }
    else
    {
        cteb->bf.width     = 0;
        cteb->blklen.width = 0;
        cteb->cid.width    = 0;

        cteb->bf.index     = 1;
        cteb->blklen.index = sdnv_read(buffer, size, &cteb->bf, &sdnvflags);
        cteb->cid.index    = sdnv_read(buffer, size, &cteb->blklen, &sdnvflags);
    }

    /* Read Custodian EID */
    eid_index = sdnv_read(buffer, size, &cteb->cid, &sdnvflags);
    eid_len   = cteb->blklen.value - (eid_index - cteb->cid.index);
    if (eid_len + eid_index > size)
    {
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "CTEB block terminated prematurely: %d > %d\n",
                     eid_len + eid_index, size);
    }
    eid_status = bplib_eid2ipn((char *)&buffer[eid_index], eid_len, &cteb->cstnode, &cteb->cstserv);
    if (eid_status != BP_SUCCESS)
    {
        return eid_status;
    }

    /* Set Bytes Read */
    bytes_read = eid_index + eid_len;

    /* Success Oriented Error Checking */
    if (sdnvflags != 0)
    {
        *flags |= sdnvflags;
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Flags raised during processing of CTEB (%08X)\n", sdnvflags);
    }
    else
    {
        return bytes_read;
    }
}

/*--------------------------------------------------------------------------------------
 * cteb_write -
 *
 *  block - pointer to memory that holds bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  cteb - pointer to a custody transfer extension block structure used to write the block [INPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Number of bytes written
 *-----------------------------------------------------------------,--------------------*/
int cteb_write(void *block, int size, bp_blk_cteb_t *cteb, bool update_indices, uint32_t *flags)
{
    uint8_t *buffer        = (uint8_t *)block;
    int      bytes_written = 0;
    int      eid_index     = 0;
    int      eid_len       = 0;
    uint32_t sdnvflags     = 0;

    /* Check Size */
    if (size < 1)
    {
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Insufficient room for the CTEB block: %d\n", size);
    }

    /* Set Block Flags */
    cteb->bf.value |= BP_BLK_REPALL_MASK;

    /* Write Block */
    buffer[0] = BP_CTEB_BLK_TYPE; /* block type */
    if (!update_indices)
    {
        sdnv_write(buffer, size, cteb->bf, &sdnvflags);
    }
    else
    {
        cteb->bf.width     = 0;
        cteb->blklen.width = 1; /* reserve one byte (7 bits) to hold length of block */
        cteb->cid.width    = 0;
        cteb->bf.index     = 1;
        cteb->blklen.index = sdnv_write(buffer, size, cteb->bf, &sdnvflags);
        cteb->cid.index    = cteb->blklen.index + cteb->blklen.width;
    }

    eid_index = sdnv_write(buffer, size, cteb->cid, &sdnvflags);

    eid_len = bplib_os_strnlen(cteb->csteid, BP_MAX_EID_STRING);
    if ((eid_index + eid_len) > size)
    {
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "CTEB block terminated prematurely: %d > %d\n",
                     eid_len + eid_index, size);
    }
    memcpy(&buffer[eid_index], cteb->csteid, eid_len);

    /* Write Block Length */
    bytes_written      = eid_index + eid_len;
    cteb->blklen.value = bytes_written - cteb->cid.index;
    if (cteb->blklen.value <= 0x7F)
    {
        sdnv_write(buffer, size, cteb->blklen, &sdnvflags);
    }
    else
    {
        bplog(&sdnvflags, BP_FLAG_SDNV_OVERFLOW, "CTEB block length too large: %d\n", cteb->blklen.value);
    }

    /* Success Oriented Error Checking */
    if (sdnvflags != 0)
    {
        *flags |= sdnvflags;
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Flags raised during processing of CTEB (%08X)\n", sdnvflags);
    }
    else
    {
        return bytes_written;
    }
}
