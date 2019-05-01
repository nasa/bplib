/************************************************************************
 * File: bplib_blk_bib.c
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
#include "bplib_blk_bib.h"
#include "bplib_crc.h"
#include "bplib_sdnv.h"
#include "bplib_os.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_blk_bib_read -
 *
 *  block - pointer to block holding bundle block [INPUT]
 *  size - size of block [INPUT]
 *  bib - pointer to a bundle integrity block structure to be populated by this function [OUTPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Number of bytes read
 *-------------------------------------------------------------------------------------*/
int bplib_blk_bib_read (void* block, int size, bp_blk_bib_t* bib, bool update_indices)
{
    uint8_t* buffer = (uint8_t*)block;
    uint16_t flags = 0;
    int bytes_read = 0;

    /* Check Size */
    if(size < 1) return BP_BUNDLEPARSEERR;

    /* Check Block Type */
    if(buffer[0] != BP_BIB_BLK_TYPE) return BP_BUNDLEPARSEERR;

    /* Read Integrity Information */
    if(!update_indices)
    {
        bplib_sdnv_read(buffer, size, &bib->bf, &flags);
        bplib_sdnv_read(buffer, size, &bib->blklen, &flags);
        bplib_sdnv_read(buffer, size, &bib->paytype, &flags);
        bytes_read = bplib_sdnv_read(buffer, size, &bib->paycrc, &flags);
    }
    else
    {
        bib->bf.width = 0;
        bib->blklen.width = 0;
        bib->paycrc.width = 0;
        bib->paytype.width = 0;

        bib->bf.index = 1;
        bib->blklen.index = bplib_sdnv_read(buffer, size, &bib->bf, &flags);
        bib->paytype.index = bplib_sdnv_read(buffer, size, &bib->blklen, &flags);
        bib->paycrc.index = bplib_sdnv_read(buffer, size, &bib->paytype, &flags);
        bytes_read = bplib_sdnv_read(buffer, size, &bib->paycrc, &flags);
    }

    /* Success Oriented Error Checking */
    if(flags != 0)  return BP_BUNDLEPARSEERR - 300;
    else            return bytes_read;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_bib_write -
 *
 *  block - pointer to memory that holds bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  bib - pointer to a bundle integrity block structure used to write the block [INPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Number of bytes written
 *-------------------------------------------------------------------------------------*/
int bplib_blk_bib_write (void* block, int size, bp_blk_bib_t* bib, bool update_indices)
{
    uint8_t* buffer = (uint8_t*)block;
    uint16_t flags = 0;
    int bytes_written = 0;

    /* Check Size */
    if(size < 1) return BP_BUNDLEPARSEERR;

    /* Set Block Flags */
    bib->bf.value |= BP_BLK_REPALL_MASK;

    /* Write Block */
    buffer[0] = BP_BIB_BLK_TYPE; // block type
    if(!update_indices)
    {
        bplib_sdnv_write(buffer, size, bib->bf, &flags);
        bplib_sdnv_write(buffer, size, bib->paytype, &flags);
        bytes_written = bplib_sdnv_write(buffer, size, bib->paycrc, &flags);
    }
    else
    {
        bib->bf.width = 0;
        bib->blklen.width = 0;
        bib->paycrc.width = 0;
        bib->paytype.width = 0;

        bib->bf.index      = 1;
        bib->blklen.index  = bplib_sdnv_write(buffer, size, bib->bf,      &flags);
        bib->paytype.index = bib->blklen.index + bib->blklen.width;
        bib->paycrc.index  = bplib_sdnv_write(buffer, size, bib->paytype, &flags);
        bytes_written      = bplib_sdnv_write(buffer, size, bib->paycrc,  &flags);
    }

    bib->blklen.value = bytes_written - bib->paytype.index;
    bplib_sdnv_write(buffer, size, bib->blklen, &flags);

    /* Success Oriented Error Checking */
    if(flags != 0)  return BP_BUNDLEPARSEERR;
    else            return bytes_written;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_bib_update -
 *
 *  block - pointer to memory that holds bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  payload - pointer to payload memory buffer [INPUT]
 *  payload_size - number of bytes to crc over [INPUT]
 *  bib - pointer to a bundle integrity block structure used to write the block [INPUT]
 *
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_blk_bib_update (void* block, int size, void* payload, int payload_size, bp_blk_bib_t* bib)
{
    assert(bib);
    assert(payload);

    uint8_t* buffer = (uint8_t*)block;
    uint16_t flags = 0;

    /* Check Size */
    if(size < (int)(bib->paycrc.index + bib->paycrc.width)) return BP_BUNDLEPARSEERR;

    /* Calculate and Write Fragment Payload CRC */
    if(bib->paytype.value == BP_BIB_CRC16)
    {
        bib->paycrc.value = bplib_crc16((uint8_t*)payload, payload_size, 0);
        bplib_sdnv_write(buffer, size, bib->paycrc, &flags);
    }

    /* Check for Errors */
    if(flags != 0)  return BP_BUNDLEPARSEERR;
    else            return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_bib_verify -
 *
 *  payload - pointer to payload memory buffer [INPUT]
 *  payload_size - number of bytes to crc over [INPUT]
 *  bib - pointer to a bundle integrity block structure used to write the block [INPUT]
 *
 *  Returns:    success or error code
 *-------------------------------------------------------------------------------------*/
int bplib_blk_bib_verify (void* payload, int payload_size, bp_blk_bib_t* bib)
{
    assert(payload);
    assert(bib);

    /* Calculate and Verify Payload CRC */
    if(bib->paytype.value == BP_BIB_CRC16)
    {
        uint16_t crc = bplib_crc16((uint8_t*)payload, payload_size, 0);
        if(bib->paycrc.value != crc)
        {
            /* Return Failure */
            return BP_FAILEDINTEGRITYCHECK;
        }
    }

    /* Return Success */
    return BP_SUCCESS;
}
