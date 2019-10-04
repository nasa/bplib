/************************************************************************
 * File: pay.c
 *
 *  Copyright 2019 United States Government as represented by the 
 *  Administrator of the National Aeronautics and Space Administration. 
 *  All Other Rights Reserved.  
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be 
 *  used, distributed and modified only pursuant to the terms of that 
 *  agreement.
 *
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "sdnv.h"
#include "pay.h"
#include "v6.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * pay_read -
 *
 *  block - pointer to block holding bundle block [INPUT]
 *  size - size of block [INPUT]
 *  pay - point to a payload block structure [OUTPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Next index
 *-------------------------------------------------------------------------------------*/
int pay_read (void* block, int size, bp_blk_pay_t* pay, bool update_indices, uint16_t* sdnvflags)
{
    uint8_t* buffer = (uint8_t*)block;
    int bytes_read = 0;
    uint16_t flags = 0;

    /* Check Size */
    if(size < 1) return bplog(BP_BUNDLEPARSEERR, "Invalid size of payload block: %d\n", size);

    /* Read Block Information */
    if(!update_indices)
    {
        sdnv_read(buffer, size, &pay->bf, &flags);
        bytes_read = sdnv_read(buffer, size, &pay->blklen, &flags);
    }
    else
    {
        pay->bf.width = 0;
        pay->blklen.width = 0;

        pay->bf.index = 1;
        pay->blklen.index = sdnv_read(buffer, size, &pay->bf, &flags);
        bytes_read = sdnv_read(buffer, size, &pay->blklen, &flags);
    }

    /* Success Oriented Error Checking */
    if(flags != 0)
    {
        *sdnvflags |= flags;
        return bplog(BP_BUNDLEPARSEERR, "Flags raised during processing of payload block (%08X)\n", flags);
    }
    else
    {
        pay->payptr = &buffer[bytes_read];
        pay->paysize = pay->blklen.value;
        return bytes_read;
    }
}

/*--------------------------------------------------------------------------------------
 * pay_write -
 *
 *  block - pointer to memory that holds bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  pay - pointer to a payload block structure [INPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int pay_write (void* block, int size, bp_blk_pay_t* pay, bool update_indices, uint16_t* sdnvflags)
{
    uint8_t* buffer = (uint8_t*)block;
    int bytes_written = 0;
    uint16_t flags = 0;

    /* Check Size */
    if(size < 1) return bplog(BP_BUNDLEPARSEERR, "Invalid size of payload block: %d\n", size);

    /* Set Block Flags */
    pay->bf.value = 0;
    pay->bf.value |= BP_BLK_REPALL_MASK;
    pay->bf.value |= BP_BLK_LASTBLOCK_MASK;

    /* Set Block Length */
    if(pay->payptr) pay->blklen.value = pay->paysize;

    /* Write Block */
    buffer[0] = BP_PAY_BLK_TYPE;
    if(!update_indices)
    {
        sdnv_write(buffer, size, pay->bf, &flags);
        bytes_written = sdnv_write(buffer, size, pay->blklen, &flags);
    }
    else
    {
        pay->bf.width = 0;
        pay->blklen.width = 0;

        pay->bf.index = 1;
        pay->blklen.index = sdnv_write(buffer, size, pay->bf, &flags);
        bytes_written = sdnv_write(buffer, size, pay->blklen, &flags);
    }

    /* Success Oriented Error Checking */
    if(flags != 0)
    {
        *sdnvflags |= flags;
        return bplog(BP_BUNDLEPARSEERR, "Flags raised during processing of payload block (%08X)\n", flags);
    }
    else
    {
        return bytes_written;
    }
}
