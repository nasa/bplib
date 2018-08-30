/******************************************************************************
 * Filename     : bplib_blk_cteb.c
 * Purpose      : Bundle Protocol Custody Transfer Extension Block
 * Design Notes :
 *
 * -------------------------------------------------
 *         Custody Transfer Extension Block
 * -------------------------------------------------
 * |    MSB    |           |           |    LSB    |
 * | (8 bits)  | (8 bits)  | (8 bits)  | (8 bits)  |
 * |-----------|-----------|-----------|-----------|
 * |                                               |
 * |              Primary Bundle Block             |
 * |                                               |
 * |-----------|-----------|-----------------------|
 * .           .           .                       .
 * |-----------|-----------|-----------------------|
 * |    0xA    | Blk Flags |     Block Length      | +4 --> Custody Transfer Extension Block
 * |-----------|-----------|-----------------------|
 * |                  Custody ID                   | +8
 * |-----------------------|-----------------------|
 * |    Custodian Node     |   Custodian Service   | +12
 * |-----------------------------------------------|
 *
 ******************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdlib.h>
#include "bplib.h"
#include "bplib_blk.h"
#include "bplib_blk_cteb.h"
#include "bplib_sdnv.h"

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
int bplib_blk_cteb_read (void* block, int size, bp_blk_cteb_t* cteb, int update_indices)
{
    uint8_t* buffer = (uint8_t*)block;
    uint8_t flags = 0;
    int bytes_read = 0;

    /* Check Size */
    if(size < 1) return BP_BUNDLEPARSEERR;

    /* Read Custody Information */
    if(!update_indices)
    {
        bplib_sdnv_read(buffer, size, &cteb->bf, &flags);
        bplib_sdnv_read(buffer, size, &cteb->blklen, &flags);
        bplib_sdnv_read(buffer, size, &cteb->cid, &flags);
        bplib_sdnv_read(buffer, size, &cteb->cstnode, &flags);
        bytes_read = bplib_sdnv_read(buffer, size, &cteb->cstserv, &flags);
    }
    else
    {
        cteb->bf.index = 1;
        cteb->blklen.index = bplib_sdnv_read(buffer, size, &cteb->bf, &flags);
        cteb->cid.index = bplib_sdnv_read(buffer, size, &cteb->blklen, &flags);
        cteb->cstnode.index = bplib_sdnv_read(buffer, size, &cteb->cid, &flags);
        cteb->cstserv.index = bplib_sdnv_read(buffer, size, &cteb->cstnode, &flags);
        bytes_read = bplib_sdnv_read(buffer, size, &cteb->cstserv, &flags);
    }

    /* Success Oriented Error Checking */
    if(flags != BP_SUCCESS) return BP_BUNDLEPARSEERR;

    /* Return Bytes Read */
    return bytes_read;
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
int bplib_blk_cteb_write (void* block, int size, bp_blk_cteb_t* cteb, int update_indices)
{
    uint8_t* buffer = (uint8_t*)block;
    uint8_t flags = 0;
    int bytes_written = 0;

    /* Check Size */
    if(size < 1) return BP_BUNDLEPARSEERR;

    /* Set Block Flags */
    cteb->bf.value |= BP_BLK_REPALL_MASK;

    /* Write Block */
    buffer[0] = BP_CTEB_BLK_TYPE; // block type
    if(!update_indices)
    {
        bplib_sdnv_write(buffer, size, cteb->bf, &flags);
        bplib_sdnv_write(buffer, size, cteb->cid, &flags);
        bplib_sdnv_write(buffer, size, cteb->cstnode, &flags);
        bytes_written = bplib_sdnv_write(buffer, size, cteb->cstserv, &flags);
    }
    else
    {
        cteb->bf.index      = 1;
        cteb->blklen.index  = bplib_sdnv_write(buffer, size, cteb->bf, &flags);
        cteb->cid.index     = cteb->blklen.index + cteb->blklen.width;
        cteb->cstnode.index = bplib_sdnv_write(buffer, size, cteb->cid, &flags);
        cteb->cstserv.index = bplib_sdnv_write(buffer, size, cteb->cstnode, &flags);
        bytes_written       = bplib_sdnv_write(buffer, size, cteb->cstserv, &flags);
    }

    cteb->blklen.value = bytes_written - cteb->cid.index;
    bplib_sdnv_write(buffer, size, cteb->blklen, &flags);

    /* Return Written */
    return bytes_written;
}
