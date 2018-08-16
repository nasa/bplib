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
 *  flags - pointer to variable that will hold the flags set as result of read [output]
 *  returns - number of bytes read in constructing value
 *-------------------------------------------------------------------------------------*/
int bplib_sdnv_read(uint8_t* block, int size, uint32_t* value, uint8_t* flags)
{
    assert(block);
    assert(value);
    assert(flags);

    *value = 0;
    *flags = BP_SDNV_SUCCESS;

    /* Check if Byte 1 can be Read */
    if(size < 1)
    {
        *flags |= BP_SDNV_UNDERFLOW;
        return 0;
    }

    /* Read Byte 1 */
    *value |= (block[0] & 0x7F) << 21;
    if((block[0] & 0x80) == 0x00)
    {
        return 1; // read one byte
    }

    /* Check if Byte 2 can be Read */
    if(size < 2)
    {
        *flags |= BP_SDNV_UNDERFLOW;
        return 1;
    }

    /* Read Byte 2 */
    *value |= (block[1] & 0x7F) << 14;
    if((block[1] & 0x80) == 0x00)
    {
        return 2; // read two bytes
    }

    /* Check if Byte 2 can be Read */
    if(size < 3)
    {
        *flags |= BP_SDNV_UNDERFLOW;
        return 2;
    }

    /* Read Byte 3 */
    *value |= (block[2] & 0x7F) << 7;
    if((block[2] & 0x80) == 0x00)
    {
        return 3; // read three bytes
    }

    /* Check if Byte 4 can be Read */
    if(size < 4)
    {
        *flags |= BP_SDNV_UNDERFLOW;
        return 3;
    }

    /* Read Byte 4 */
    *value |= (block[3] & 0x7F);
    if((block[3] & 0x80) == 0x80)
    {
        *flags |= BP_SDNV_OVERFLOW;
    }

    return 4; // read four bytes
}

/*--------------------------------------------------------------------------------------
 * bplib_sdnv_write - 
 * 
 *  block - pointer to memory address to be written to [input]
 *  size - number of bytes to write for value (as counted in resulting SDNV) [input]
 *  value - value to write to block [input]
 *  returns - number of bytes written for sdnv
 *-------------------------------------------------------------------------------------*/
int bplib_sdnv_write(uint8_t* block, int size, uint32_t value)
{
    if(size > 0)    block[0] = (uint8_t)(value >> 7) | 0x80;
    if(size > 1)    block[1] = (uint8_t)(value >> 14) | 0x80;
    if(size > 2)    block[2] = (uint8_t)(value >> 21) | 0x80;
    if(size > 3)    block[3] = (uint8_t)value & 0x7F;
    return size;
}
