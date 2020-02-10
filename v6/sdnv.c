/************************************************************************
 * File: sdnv.c
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
 * Design Notes :
 *   1. The read routines make no assumptions on fixed lengths but there still
 *      needs to be an assumption made as to the size of the variable to hold
 *      the resulting read.  Therefore if the requested variable size is too
 *      small to hold the read value, an OVERFLOW flag is set.
 *   2. The write routines do assume a fixed length and will write an SDNV that
 *      is always the size specified regardless of the value passed to it.
 *************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "sdnv.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * sdnv_read -
 *
 *  block - pointer to memory address to read from [input]
 *  size - maximum number of bytes to read [input]
 *  value - pointer to variable that will hold the value read from block [output]
 *  width - size of value variable in bytes [input]
 *  flags - pointer to variable that will hold the flags set as result of read [output]
 *  returns - next index (number of bytes read + starting index)
 *-------------------------------------------------------------------------------------*/
int sdnv_read(uint8_t* block, int size, bp_field_t* sdnv, uint32_t* flags)
{
    assert(block);
    assert(sdnv);
    assert(flags);

    int i, width;

    /* Initialize Values */
    sdnv->value = 0;
    if(sdnv->width <= 0) width = size;
    else                 width = sdnv->width;

    /* Read SDNV */
    for(i = sdnv->index; (i < (sdnv->index + width)) && (i < size); i++)
    {
        /* Shift value and check for overflow */
        bp_val_t tmpval = sdnv->value;
        sdnv->value <<= 7;
        if(sdnv->value >> 7 != tmpval)
        {
            /* Set Overflow:
             *  The right shift caused bits to be lost which means
             *  the encoded value could not be stored in a bp_val_t */
            *flags |= BP_FLAG_SDNV_OVERFLOW;
        }
        
        /* OR in next byte */
        sdnv->value |= (block[i] & 0x7F);
        
        /* Check for end of SDNV */
        if((block[i] & 0x80) == 0x00)
        {
            /* Success: return the next index */
            return (i + 1);
        }
    }

    /* Set Incomplete:
     *  The SDNV wanted to keep going but the
     *  block ended before it was complete */
    *flags |= BP_FLAG_SDNV_INCOMPLETE;

    /* Return Next Index */
    return i;
}

/*--------------------------------------------------------------------------------------
 * sdnv_write -
 *
 *  block - pointer to memory address to be written to [input]
 *  size - number of bytes to write for value (as counted in resulting SDNV) [input]
 *  value - value to write to block [input]
 *  width - size of value variable in bytes [input]
 *  flags - pointer to variable that will hold the flags set as result of write [output]
 *  returns - next index (number of bytes read + starting index)
 *-------------------------------------------------------------------------------------*/
int sdnv_write(uint8_t* block, int size, bp_field_t sdnv, uint32_t* flags)
{
    assert(block);
    assert(flags);

    int i, fixedwidth, endindex;

    /* Initialize Bytes to Write */
    if(sdnv.width <= 0)
    {
        /* Calculate Bytes Needed to Hold Value */
        bp_val_t tmpval = sdnv.value;
        fixedwidth = 1;
        while(tmpval > 0x7F)
        {
            fixedwidth++;
            tmpval >>= 7;
        }
    }
    else
    {
        /* Set Fixed Width */
        fixedwidth = sdnv.width;
    }
    
    /* Check for Truncation */
    if(fixedwidth > (size - (int)sdnv.index))
    {
        /* Truncate Width */
        *flags |= BP_FLAG_SDNV_INCOMPLETE;
        fixedwidth = size - sdnv.index;
    }

    /* Write SDNV */
    endindex = fixedwidth + sdnv.index - 1;
    for(i = endindex; i >= (int)sdnv.index; i--)
    {
        if(i == endindex)   block[i] = sdnv.value & 0x7F;
        else                block[i] = sdnv.value | 0x80;
        sdnv.value >>= 7;
    }

    /* Set Overflow  */
    if(sdnv.value > 0) *flags |= BP_FLAG_SDNV_OVERFLOW;

    /* Return Next Index */
    return fixedwidth + sdnv.index;
}

/*--------------------------------------------------------------------------------------
 * sdnv_mask - truncates value to width
 *
 *  sdnv - pointer to sdnv that will be truncated [input/output]
 *-------------------------------------------------------------------------------------*/
void sdnv_mask(bp_field_t* sdnv)
{
    int num_bits = sdnv->width * 7;
    int max_bits = sizeof(bp_val_t) * 8;
    int shift_bits = max_bits - num_bits;
    bp_val_t val_mask = BP_MAX_ENCODED_VALUE >> shift_bits;
    sdnv->value = sdnv->value & val_mask;
}