/******************************************************************************
 * Filename     : bplib_sdnv.c
 * Purpose      : Bundle Protocol Library - Self-Delimiting Numeric Values
 * Design Notes :
 *   1. The read routines make no assumptions on fixed lengths but there still
 *      needs to be an assumption made as to the size of the variable to hold
 *      the resulting read.  Therefore if the requested variable size is too
 *      small to hold the read value, an OVERFLOW flag is set.
 *   2. The write routines do assume a fixed length and will write an SDNV that
 *      is always the size specified regardless of the value passed to it.
 ******************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <assert.h>

#include "bplib_sdnv.h"
#include "bplib.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_sdnv_read -
 *
 *  block - pointer to memory address to read from [input]
 *  size - maximum number of bytes to read [input]
 *  value - pointer to variable that will hold the value read from block [output]
 *  width - size of value variable in bytes [input]
 *  flags - pointer to variable that will hold the flags set as result of read [output]
 *  returns - next index (number of bytes read + starting index)
 *-------------------------------------------------------------------------------------*/
int bplib_sdnv_read(uint8_t* block, int size, bp_sdnv_t* sdnv, uint16_t* flags)
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
    for(i = sdnv->index; (i < width) && (i < size); i++)
    {
        sdnv->value <<= 7;
        sdnv->value |= (block[i] & 0x7F);
        if((block[i] & 0x80) == 0x00) return (i + 1);
        else if(size < (i + 2)) *flags |= BP_FLAG_SDNVINCOMPLETE;
    }

    /* Set Overflow  */
    *flags |= BP_FLAG_SDNVOVERFLOW;

    /* Return Next Index */
    return i;
}

/*--------------------------------------------------------------------------------------
 * bplib_sdnv_write -
 *
 *  block - pointer to memory address to be written to [input]
 *  size - number of bytes to write for value (as counted in resulting SDNV) [input]
 *  value - value to write to block [input]
 *  width - size of value variable in bytes [input]
 *  flags - pointer to variable that will hold the flags set as result of write [output]
 *  returns - next index (number of bytes read + starting index)
 *-------------------------------------------------------------------------------------*/
int bplib_sdnv_write(uint8_t* block, int size, bp_sdnv_t sdnv, uint16_t* flags)
{
    assert(block);
    assert(flags);

    int i, maxbytes, endindex;

    /* Initialize Bytes to Write */
    if(sdnv.width <= 0)
    {
        /* Calculate Bytes Needed to Hold Value */
        uint32_t tmpval = sdnv.value;
        maxbytes = 0;
        while(tmpval > 0)
        {
            maxbytes++;
            tmpval >>= 7;
        }
    }
    else if(sdnv.width <= (size - sdnv.index))
    {
        /* Set Fixed Width */
        maxbytes = sdnv.width;
    }
    else
    {
        /* Truncate Width */
        *flags |= BP_FLAG_SDNVINCOMPLETE;
        maxbytes = size - sdnv.index;
    }

    /* Write SDNV */
    endindex = maxbytes + sdnv.index - 1;
    for(i = endindex; i >= (int)sdnv.index; i--)
    {
        if(i == endindex)   block[i] = sdnv.value & 0x7F;
        else                block[i] = sdnv.value | 0x80;
        sdnv.value >>= 7;
    }

    /* Set Overflow  */
    if(sdnv.value > 0) *flags |= BP_FLAG_SDNVOVERFLOW;

    /* Return Next Index */
    return maxbytes + sdnv.index;
}
