/******************************************************************************
 * Filename     : bplib_blk_bib.c
 * Purpose      : Bundle Protocol Bundle Integrity Block
 * Design Notes :
 * 
 * -------------------------------------------------
 *              Bundle Integrity Block
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
 * |    0xD    | Blk Flags |     Block Length      | +4 --> Bundle Integrity Block
 * |-----------|-----------|-----------------------|
 * |    Payload CRC Type   |      Payload CRC      | +8
 * -------------------------------------------------
 * 
 ******************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdlib.h>
#include "bplib.h"
#include "bplib_blk.h"
#include "bplib_blk_bib.h"
#include "bplib_crc.h"
#include "bplib_sdnv.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_blk_bib_read - 
 * 
 *  block - pointer to block holding bundle block [INPUT]
 *  size - size of block [INPUT]
 *  bib - pointer to a bundle integrity block structure to be populated by this function [OUTPUT]
 * 
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_blk_bib_read (void* contents, int size, bp_blk_bib_t* bib)
{
    uint8_t* buffer = (uint8_t*)contents;
    uint8_t flags = 0;
    int index = 0;

    /* Read Custody Information */
    index += bplib_sdnv_read(&buffer[index], size - index, &bib->paytype, &flags);
    index += bplib_sdnv_read(&buffer[index], size - index, &bib->paycrc,  &flags);
    if(flags != BP_SUCCESS) return BP_BUNDLEPARSEERR;

    return index;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_bib_write - 
 * 
 *  block - pointer to memory that holds bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  bib - pointer to a bundle integrity block structure used to write the block [INPUT]
 * 
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_blk_bib_write (void* block, int size, bp_blk_bib_t* bib)
{
    uint8_t* buffer = (uint8_t*)block;
    
    /* Check Size */
    if(size < BP_BIB_BLK_LENGTH) return BP_BUNDLEPARSEERR;

    /* Set Block Flags */
    uint8_t blk_flags = 0;
    blk_flags |= BP_BLK_REPALL_MASK;

    /* Write Block */
    buffer[0] = BP_BIB_BLK_TYPE; // block type
    bplib_sdnv_write(&buffer[1],  1,  blk_flags); 
    bplib_sdnv_write(&buffer[2],  2,  BP_BIB_BLK_LENGTH - 4);
    bplib_sdnv_write(&buffer[4],  2,  bib->paytype);
    bplib_sdnv_write(&buffer[6],  2,  bib->paycrc);

    /* Return Block Size */
    return BP_BIB_BLK_LENGTH;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_bib_update - 
 * 
 *  block - pointer to memory that holds bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  payload - pointer to payload memory buffer [INPUT]
 *  payload_size - number of bytes to crc over [INPUT]
 *  bib- pointer to a bundle integrity block structure used to write the block [INPUT]
 * 
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_blk_bib_update (void* block, int size, void* payload, int payload_size, bp_blk_bib_t* bib)
{
    uint8_t* buffer = (uint8_t*)block;

    /* Check Size */
    if(size < BP_BIB_BLK_LENGTH) return BP_BUNDLEPARSEERR;

    /* Calculate and Write Fragment Payload CRC */
    if(bib->paytype == BP_BIB_CRC16)
    {
        bib->paycrc = bplib_crc16((uint8_t*)payload, payload_size, 0);
        bplib_sdnv_write(&buffer[6], 2, bib->paycrc);
    }

    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_bib_verify - 
 * 
 *  payload - pointer to payload memory buffer [INPUT]
 *  payload_size - number of bytes to crc over [INPUT]
 *  bib- pointer to a bundle integrity block structure used to write the block [INPUT]
 * 
 *  Returns:    success or error code
 *-------------------------------------------------------------------------------------*/
int bplib_blk_bib_verify (void* payload, int payload_size, bp_blk_bib_t* bib)
{
    /* Calculate and Verify Payload CRC */
    if(bib->paytype == BP_BIB_CRC16)
    {
        uint16_t crc = bplib_crc16((uint8_t*)payload, payload_size, 0);
        if(bib->paycrc != crc)
        {
            /* Return Failure */
            return BP_FAILEDINTEGRITYCHECK;
        }
    }

    /* Return Success */
    return BP_SUCCESS;
}
