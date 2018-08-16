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
 * 
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_blk_cteb_read (void* contents, int size, bp_blk_cteb_t* cteb)
{
    uint8_t* buffer = (uint8_t*)contents;
    uint8_t flags = 0;
    int index = 0;

    /* Read Custody Information */
    index += bplib_sdnv_read(&buffer[index], size - index, &cteb->cid, &flags);
    index += bplib_sdnv_read(&buffer[index], size - index, &cteb->cstnode, &flags);
    index += bplib_sdnv_read(&buffer[index], size - index, &cteb->cstserv, &flags);
    if(flags != BP_SUCCESS) return BP_BUNDLEPARSEERR;

    /* Return Bytes Read */
    return index;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_cteb_write - 
 * 
 *  block - pointer to memory that holds bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  cteb - pointer to a custody transfer extension block structure used to write the block [INPUT]
 * 
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_blk_cteb_write (void* block, int size, bp_blk_cteb_t* cteb)
{
    uint8_t* buffer = (uint8_t*)block;
    
    /* Check Size */
    if(size < BP_CTEB_BLK_LENGTH) return BP_BUNDLEPARSEERR;

    /* Set Block Flags */
    uint8_t blk_flags = 0;
    blk_flags |= BP_BLK_REPALL_MASK;

    /* Write Block */
    buffer[0] = BP_CTEB_BLK_TYPE; // block type
    bplib_sdnv_write(&buffer[1],  1,  blk_flags); 
    bplib_sdnv_write(&buffer[2],  2,  BP_CTEB_BLK_LENGTH - 4);
    bplib_sdnv_write(&buffer[4],  4,  cteb->cid);
    bplib_sdnv_write(&buffer[8],  2,  cteb->cstnode);
    bplib_sdnv_write(&buffer[10], 2,  cteb->cstserv);

    /* Return Block Size */
    return BP_CTEB_BLK_LENGTH;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_cteb_update - 
 * 
 *  block - pointer to memory that holds bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  cid - custody id
 * 
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_blk_cteb_update (void* block, int size, uint32_t cid)
{
    uint8_t* buffer = (uint8_t*)block;
    
    /* Check Size */
    if(size < BP_CTEB_BLK_LENGTH) return BP_BUNDLEPARSEERR;

    /* Write CID */
    bplib_sdnv_write(&buffer[4], 4, cid);

    /* Return Number of Bytes Written */
    return 4;
}
