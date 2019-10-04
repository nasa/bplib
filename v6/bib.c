/************************************************************************
 * File: bib.c
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
#include "crc.h"
#include "v6.h"
#include "bib.h"

/******************************************************************************
 CRC DEFINITIONS
 ******************************************************************************/

static crc_parameters_t crc16_x25 = {
    .name                            = "CRC-16 X25", 
    .length                          = 16,
    .should_reflect_input            = true,
    .should_reflect_output           = true,
    .n_bit_params = {
        .crc16 = {
            .generator_polynomial    = 0x1021,
            .initial_value           = 0xFFFF,
            .final_xor               = 0xF0B8,
            .check_value             = 0x906E
        }
    }
};

static crc_parameters_t crc32_castagnoli = {
    .name                         = "CRC-32 Castagnoli", 
    .length                       = 32,
    .should_reflect_input         = true,
    .should_reflect_output        = true,
    .n_bit_params = {
        .crc32 = {
            .generator_polynomial = 0x1EDC6F41,
            .initial_value        = 0xFFFFFFFF,
            .final_xor            = 0xFFFFFFFF,
            .check_value          = 0xE3069283
        }
    }
};

/******************************************************************************
 STATIC FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * to_big_endian16 - Converts a uint16_t to big endian representation and writes it to
 *      buffer.
 *
 * val: The uint16_t to convert to big endian. [INPUT]
 * buffer: The buffer to write the converted value to. [OUTPUT]
 *-------------------------------------------------------------------------------------*/
static inline void to_big_endian16(uint16_t val, uint8_t* buffer)
{
    buffer[0] = (val >> 8) & 0xFF;
    buffer[1] = (val     ) & 0xFF;
}

/*--------------------------------------------------------------------------------------
 * to_big_endian32 - Converts a uint16_t to big endian representation and writes it to
 *      buffer.
 *
 * val: The uint32_t to convert to big endian. [INPUT]
 * buffer: The buffer to write the converted value to. [OUTPUT]
 *-------------------------------------------------------------------------------------*/
static inline void to_big_endian32(uint32_t val, uint8_t* buffer)
{
    buffer[0] = (val >> 24) & 0xFF;
    buffer[1] = (val >> 16) & 0xFF;
    buffer[2] = (val >>  8) & 0xFF;
    buffer[3] = (val      ) & 0xFF;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bib_init - Inits the crc xor tables for all supported crc specifications.
 *-------------------------------------------------------------------------------------*/
int bib_init (void)
{
    int crc16_status = crc_init(&crc16_x25);
    int crc32_status = crc_init(&crc32_castagnoli);
    
    if(crc16_status == BP_SUCCESS && 
       crc32_status == BP_SUCCESS)
    {
        return BP_SUCCESS;
    }
    else
    {
        return BP_ERROR;
    }
} 

/*--------------------------------------------------------------------------------------
 * bib_read -
 *
 *  block - pointer to block holding bundle block [INPUT]
 *  size - size of block [INPUT]
 *  bib - pointer to a bundle integrity block structure to be populated by this function [OUTPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Number of bytes read
 *-------------------------------------------------------------------------------------*/
int bib_read (void* block, int size, bp_blk_bib_t* bib, bool update_indices, uint16_t* flags)
{
    uint8_t* buffer = (uint8_t*)block;
    int bytes_read = 0;

    /* Check Size */
    if(size < 1) return BP_BUNDLEPARSEERR;

    /* Check Block Type */
    if(buffer[0] != BP_BIB_BLK_TYPE) return BP_BUNDLEPARSEERR;

    /* Read Integrity Information */
    if(!update_indices)
    {
        sdnv_read(buffer, size, &bib->block_flags, flags);
        sdnv_read(buffer, size, &bib->block_length, flags);
        sdnv_read(buffer, size, &bib->security_target_count, flags);
        sdnv_read(buffer, size, &bib->security_target_type, flags);
        sdnv_read(buffer, size, &bib->security_target_sequence, flags);
        sdnv_read(buffer, size, &bib->cipher_suite_id, flags);
        sdnv_read(buffer, size, &bib->cipher_suite_flags, flags);
        bytes_read = sdnv_read(buffer, size, &bib->security_result_count, flags);
        if (bytes_read + 1 > size) return BP_BUNDLEPARSEERR;
        bib->security_result_type = buffer[bytes_read];
        bytes_read = sdnv_read(buffer, size, &bib->security_result_length, flags);
    }
    else
    {
        bib->block_flags.width = 0;
        bib->block_length.width = 0;
        bib->security_target_count.width = 0;
        bib->security_target_type.width = 0;
        bib->security_target_sequence.width = 0;
        bib->cipher_suite_id.width = 0;
        bib->cipher_suite_flags.width = 0;
        bib->security_result_count.width = 0;
        bib->security_result_length.width = 0;

        bib->block_flags.index = 1;
        bib->block_length.index = sdnv_read(buffer, size, &bib->block_flags, flags);
        bib->security_target_count.index = sdnv_read(buffer, size, &bib->block_length, flags);
        bib->security_target_type.index = sdnv_read(buffer, size, &bib->security_target_count, flags);
        bib->security_target_sequence.index = sdnv_read(buffer, size, &bib->security_target_type, flags);
        bib->cipher_suite_id.index = sdnv_read(buffer, size, &bib->security_target_sequence, flags);
        bib->cipher_suite_flags.index = sdnv_read(buffer, size, &bib->cipher_suite_id, flags);       
        bib->security_result_count.index = sdnv_read(buffer, size, &bib->cipher_suite_flags, flags);
        bytes_read = sdnv_read(buffer, size, &bib->security_result_count, flags);
        if (bytes_read + 1 > size) return BP_BUNDLEPARSEERR;
        bib->security_result_type = buffer[bytes_read];
        /* Reads the security_result length. */
        bib->security_result_length.index = bytes_read + 1;
        bytes_read = sdnv_read(buffer, size, &bib->security_result_length, flags);
    }

    if (bib->cipher_suite_id.value == BP_BIB_CRC16_X25 && bib->security_result_length.value == 2)
    {
        if (bytes_read + 2 > size) return BP_BUNDLEPARSEERR;
        bib->security_result_data.crc16 = *((uint16_t*) (buffer + bytes_read));
        uint8_t* valptr = buffer + bytes_read; 
        bib->security_result_data.crc16 = 0;
        bib->security_result_data.crc16 |= (((uint16_t) valptr[0]) << 8);
        bib->security_result_data.crc16 |= ((uint16_t) valptr[1]);
        bytes_read += 2;
    }
    else if (bib->cipher_suite_id.value == BP_BIB_CRC32_CASTAGNOLI && bib->security_result_length.value == 4)
    {
        if (bytes_read + 4 > size) return BP_BUNDLEPARSEERR;
        bib->security_result_data.crc32 = *((uint32_t*) (buffer + bytes_read));
        uint8_t* valptr = buffer + bytes_read; 
        bib->security_result_data.crc32 = 0;
        bib->security_result_data.crc32 |= ((uint32_t) valptr[0]) << 24;
        bib->security_result_data.crc32 |= ((uint32_t) valptr[1]) << 16;
        bib->security_result_data.crc32 |= ((uint32_t) valptr[2]) <<  8;
        bib->security_result_data.crc32 |= ((uint32_t) valptr[3]);
        bytes_read += 4;
    }
    else
    {
        return BP_BUNDLEPARSEERR;
    }

    /* Success Oriented Error Checking */
    if(*flags != 0) return BP_BUNDLEPARSEERR;
    else            return bytes_read;
}

/*--------------------------------------------------------------------------------------
 * bib_write -
 *
 *  block - pointer to memory that holds bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  bib - pointer to a bundle integrity block structure used to write the block [INPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Number of bytes written
 *-------------------------------------------------------------------------------------*/
int bib_write (void* block, int size, bp_blk_bib_t* bib, bool update_indices, uint16_t* flags)
{
    uint8_t* buffer = (uint8_t*)block;
    int bytes_written = 0;

    /* Check Size */
    if(size < 1) return BP_BUNDLEPARSEERR;

    /* Set Block Flags */
    bib->block_flags.value |= BP_BLK_REPALL_MASK;

    /* Write Block */
    buffer[0] = BP_BIB_BLK_TYPE; /* block type */
    if(!update_indices)
    {
        sdnv_write(buffer, size, bib->block_flags, flags);
        sdnv_write(buffer, size, bib->block_length, flags); 
        sdnv_write(buffer, size, bib->security_target_count, flags);
        sdnv_write(buffer, size, bib->security_target_type, flags);
        sdnv_write(buffer, size, bib->security_target_sequence, flags);
        sdnv_write(buffer, size, bib->cipher_suite_id, flags);
        sdnv_write(buffer, size, bib->cipher_suite_flags, flags);
        bytes_written = sdnv_write(buffer, size, bib->security_result_count, flags);
        if (bytes_written + 1 > size) return BP_BUNDLEPARSEERR;
        buffer[bytes_written] = bib->security_result_type;
    }
    else
    {
        bib->block_flags.width = 0;
        bib->block_length.width = 0;
        bib->security_target_count.width = 0;
        bib->security_target_type.width = 0;
        bib->security_target_sequence.width = 0;
        bib->cipher_suite_id.width = 0;
        bib->cipher_suite_flags.width = 0;
        bib->security_result_count.width = 0;
        bib->security_result_length.width = 0;

        bib->block_flags.index = 1;
        bib->block_length.index = sdnv_write(buffer, size, bib->block_flags, flags);
        bib->security_target_count.index = sdnv_write(buffer, size, bib->block_length, flags);
        bib->security_target_type.index = sdnv_write(buffer, size, bib->security_target_count, flags);
        bib->security_target_sequence.index = sdnv_write(buffer, size, bib->security_target_type, flags);
        bib->cipher_suite_id.index = sdnv_write(buffer, size, bib->security_target_sequence, flags);
        bib->cipher_suite_flags.index = sdnv_write(buffer, size, bib->cipher_suite_id, flags);       
        bib->security_result_count.index = sdnv_write(buffer, size, bib->cipher_suite_flags, flags);
        bytes_written = sdnv_write(buffer, size, bib->security_result_count, flags);
        if (bytes_written + 1 > size) return BP_BUNDLEPARSEERR;
        buffer[bytes_written] = bib->security_result_type;
        bib->security_result_length.index = bytes_written + 1;
    }
    
    if (bib->cipher_suite_id.value == BP_BIB_CRC16_X25)
    {
        if (bytes_written + 2 > size) return BP_BUNDLEPARSEERR;
        bib->security_result_length.value = 2; 
        bytes_written = sdnv_write(buffer, size, bib->security_result_length, flags);
        to_big_endian16(bib->security_result_data.crc16, buffer + bytes_written);
        bytes_written += 2;
    }
    else if (bib->cipher_suite_id.value == BP_BIB_CRC32_CASTAGNOLI)
    {
        if (bytes_written + 4 > size) return BP_BUNDLEPARSEERR;
        bib->security_result_length.value = 4;
        bytes_written = sdnv_write(buffer, size, bib->security_result_length, flags);
        to_big_endian32(bib->security_result_data.crc32, buffer + bytes_written);
        bytes_written += 4;
    }
    else
    {
        return BP_BUNDLEPARSEERR;
    }

    bib->block_length.value = bytes_written - bib->security_target_count.index;
    sdnv_write(buffer, size, bib->block_length, flags);

    /* Success Oriented Error Checking */
    if(*flags != 0) return BP_BUNDLEPARSEERR;
    else            return bytes_written;
}

/*--------------------------------------------------------------------------------------
 * bib_update -
 *
 *  block - pointer to memory that holds bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  payload - pointer to payload memory buffer [INPUT]
 *  payload_size - number of bytes to crc over [INPUT]
 *  bib - pointer to a bundle integrity block structure used to write the block [INPUT]
 *
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bib_update (void* block, int size, void* payload, int payload_size, bp_blk_bib_t* bib, uint16_t* flags)
{
    assert(bib);
    assert(payload);

    uint8_t* buffer = (uint8_t*)block;

    /* Check Size */
    if(size < (long)bib->security_result_length.value) return BP_BUNDLEPARSEERR;

    /* Calculate and Write Fragment Payload CRC */
    if(bib->cipher_suite_id.value == BP_BIB_CRC16_X25)
    {
        bib->security_result_data.crc16 = (uint16_t) crc_get((uint8_t*)payload, payload_size, &crc16_x25); 
        uint8_t* valptr = buffer + bib->security_result_length.index + bib->security_result_length.width;
        to_big_endian16(bib->security_result_data.crc16, valptr);
    }
    else if(bib->cipher_suite_id.value == BP_BIB_CRC32_CASTAGNOLI)
    {
        bib->security_result_data.crc32 = crc_get((uint8_t*)payload, payload_size, &crc32_castagnoli);
        uint8_t* valptr = buffer + bib->security_result_length.index + bib->security_result_length.width;
        to_big_endian32(bib->security_result_data.crc32, valptr);
    }

    /* Check for Errors */
    if(*flags != 0) return BP_BUNDLEPARSEERR;
    else            return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bib_verify -
 *
 *  payload - pointer to payload memory buffer [INPUT]
 *  payload_size - number of bytes to crc over [INPUT]
 *  bib - pointer to a bundle integrity block structure used to write the block [INPUT]
 *
 *  Returns:    success or error code
 *-------------------------------------------------------------------------------------*/
int bib_verify (void* payload, int payload_size, bp_blk_bib_t* bib, uint16_t* flags)
{
    assert(payload);
    assert(bib);
    
    (void)flags;

    /* Calculate and Verify Payload CRC */
    if(bib->cipher_suite_id.value == BP_BIB_CRC16_X25)
    {
        uint16_t crc = (uint16_t) crc_get((uint8_t*)payload, payload_size, &crc16_x25);
        if(bib->security_result_data.crc16 != crc)
        {
            /* Return Failure */
            return BP_FAILEDINTEGRITYCHECK;
        }
    }
    else if(bib->cipher_suite_id.value == BP_BIB_CRC32_CASTAGNOLI)
    {
        uint32_t crc = crc_get((uint8_t*)payload, payload_size, &crc32_castagnoli);
        if(bib->security_result_data.crc32 != crc)
        {
            /* Return Failure */
            return BP_FAILEDINTEGRITYCHECK;
        }
    }

    /* Return Success */
    return BP_SUCCESS;
}
