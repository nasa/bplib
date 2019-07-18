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
 CRC DEFINITIONS
 ******************************************************************************/

// Defines the parameters for the CRC-IBM-SDLC implementation.
static struct crc_parameters crc16_x25 =  {.name                            = "CRC-16 X25", 
                                           .length                          = 16,
                                           .should_reflect_input            = true,
                                           .should_reflect_output           = true,
                                           .n_bit_params = {
                                               .crc16 = {
                                                   .generator_polynomial    = 0x1021,
                                                   .initial_value           = 0xFFFF,
                                                   .final_xor               = 0xF0B8,
                                                   .check_value             = 0x906E
                                           }}};

static struct crc_parameters crc32_castagnoli = {.name                         = "CRC-32 Castagnoli", 
                                                 .length                       = 32,
                                                 .should_reflect_input         = true,
                                                 .should_reflect_output        = true,
                                                 .n_bit_params = {
                                                     .crc32 = {
                                                         .generator_polynomial = 0x1EDC6F41,
                                                         .initial_value        = 0xFFFFFFFF,
                                                         .final_xor            = 0xFFFFFFFF,
                                                         .check_value          = 0xE3069283
                                                 }}};

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_blk_bib_init -
 *
 *  bib - pointer to a bundle integrity block structure to be populated by this function [OUTPUT]
 *
 *  Returns: int indicating sucess or failure.
 *-------------------------------------------------------------------------------------*/
int bplib_blk_bib_init (bp_blk_bib_t* bib)
{
    if (bib->cipher_suite_id.value == BP_BIB_CRC16_X25)
    {
        init_crc_table(&crc16_x25);
        bib->security_result_length.value = 2;
        return BP_SUCCESS;
    }
    else if (bib->cipher_suite_id.value == BP_BIB_CRC32_CASTAGNOLI)
    {
        init_crc_table(&crc32_castagnoli);
        bib->security_result_length.value = 4;
        return BP_SUCCESS;
    }
    return BP_CRCINVALIDLENGTHERR;
} 


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
        bplib_sdnv_read(buffer, size, &bib->block_flags, &flags);
        bplib_sdnv_read(buffer, size, &bib->block_length, &flags);
        bplib_sdnv_read(buffer, size, &bib->security_target_count, &flags);
        bplib_sdnv_read(buffer, size, &bib->security_target_type, &flags);
        bplib_sdnv_read(buffer, size, &bib->security_target_sequence, &flags);
        bplib_sdnv_read(buffer, size, &bib->cipher_suite_id, &flags);
        bplib_sdnv_read(buffer, size, &bib->cipher_suite_flags, &flags);
        bytes_read = bplib_sdnv_read(buffer, size, &bib->security_result_count, &flags);
        buffer[bytes_read] = bib->security_result_type;
        bytes_read = bplib_sdnv_read(buffer, size, &bib->security_result_length, &flags);
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
        bib->block_length.index = bplib_sdnv_read(buffer, size, &bib->block_flags, &flags);
        bib->security_target_count.index = bplib_sdnv_read(buffer, size, &bib->block_length, &flags);
        bib->security_target_sequence.index = bplib_sdnv_read(buffer, size, &bib->security_target_count, &flags);
        bib->cipher_suite_id.index = bplib_sdnv_read(buffer, size, &bib->security_target_sequence, &flags);
        bib->cipher_suite_flags.index = bplib_sdnv_read(buffer, size, &bib->cipher_suite_id, &flags);       
        bib->security_result_count.index = bplib_sdnv_read(buffer, size, &bib->cipher_suite_flags, &flags);
        bib->security_result_type = buffer[bib->security_result_count.index];
        // Reads the security_result length.
        bytes_read = bplib_sdnv_read(buffer, size, (bp_sdnv_t*)(buffer + bib->security_result_count.index + 1), &flags);
    }

    if (bib->security_result_length.value == 2)
    {
        bib->security_result_data.crc16 = *((uint16_t*) (buffer + bytes_read));
        bytes_read += 2;
    }
    else if (bib->security_result_length.value == 4)
    {
        bib->security_result_data.crc32 = *((uint32_t*) (buffer + bytes_read));
        bytes_read += 4;
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
    bib->block_flags.value |= BP_BLK_REPALL_MASK;

    /* Write Block */
    buffer[0] = BP_BIB_BLK_TYPE; // block type
    if(!update_indices)
    {
        bplib_sdnv_write(buffer, size, bib->block_flags, &flags);
        bplib_sdnv_write(buffer, size, bib->block_length, &flags); 
        bplib_sdnv_write(buffer, size, bib->security_target_count, &flags);
        bplib_sdnv_write(buffer, size, bib->security_target_type, &flags);
        bplib_sdnv_write(buffer, size, bib->security_target_sequence, &flags);
        bplib_sdnv_write(buffer, size, bib->cipher_suite_id, &flags);
        bplib_sdnv_write(buffer, size, bib->cipher_suite_flags, &flags);
        bytes_written = bplib_sdnv_write(buffer, size, bib->security_result_count, &flags);
        buffer[bytes_written] = bib->security_result_type;
        bytes_written = bplib_sdnv_write(buffer, size, bib->security_result_length, &flags);
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
        bib->block_length.index = bplib_sdnv_write(buffer, size, bib->block_flags, &flags);
        bib->security_target_count.index = bplib_sdnv_write(buffer, size, bib->block_length, &flags);
        bib->security_target_sequence.index = bplib_sdnv_write(buffer, size, bib->security_target_count, &flags);
        bib->cipher_suite_id.index = bplib_sdnv_write(buffer, size, bib->security_target_sequence, &flags);
        bib->cipher_suite_flags.index = bplib_sdnv_write(buffer, size, bib->cipher_suite_id, &flags);       
        bib->security_result_count.index = bplib_sdnv_write(buffer, size, bib->cipher_suite_flags, &flags);
        bytes_written = bplib_sdnv_write(buffer, size, bib->security_result_count, &flags);
        buffer[bytes_written] = bib->security_result_type;
        bytes_written += 1;
        bib->security_result_length.index = bytes_written;
        bytes_written = bplib_sdnv_write(buffer, size, bib->security_result_length, &flags);
    }

    if (bib->security_result_length.value == 2)
    {
        *(uint16_t *)(buffer + bytes_written) = bib->security_result_data.crc16;
        bib->block_length.value = bytes_written - bib->security_result_length.index;
        bplib_sdnv_write(buffer, size, bib->block_length, &flags);
        bytes_written += 2;
    }
    else if (bib->security_result_length.value == 4)
    {
        *(uint32_t *)(buffer + bytes_written) = bib->security_result_data.crc32;
        bib->block_length.value = bytes_written - bib->security_result_length.index;
        bplib_sdnv_write(buffer, size, bib->block_length, &flags);
        bytes_written += 4;
    }

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
    if((uint32_t) size < bib->security_result_length.value) return BP_BUNDLEPARSEERR;

    /* Calculate and Write Fragment Payload CRC */
    if(bib->cipher_suite_id.value == BP_BIB_CRC16_X25)
    {
        bib->security_result_data.crc16 = get_crc16((uint8_t*)payload, payload_size, &crc16_x25);        bib->security_result_length.value = 2; 
        *(uint16_t *)(buffer + bib->security_result_length.index + bib->security_result_length.width) = bib->security_result_data.crc16;
        
    }
    else if(bib->cipher_suite_id.value == BP_BIB_CRC32_CASTAGNOLI)
    {
        bib->security_result_data.crc32 = get_crc32((uint8_t*)payload, payload_size, &crc32_castagnoli);
        bib->security_result_length.value = 4;
        *(uint32_t *)(buffer + bib->security_result_length.index + bib->security_result_length.width) = bib->security_result_data.crc32;
   ;
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
    if(bib->cipher_suite_id.value == BP_BIB_CRC16_X25)
    {
        uint16_t crc = get_crc16((uint8_t*)payload, payload_size, &crc16_x25);
        if(bib->security_result_data.crc16 != crc)
        {
            /* Return Failure */
            return BP_FAILEDINTEGRITYCHECK;
        }
    }
    else if(bib->cipher_suite_id.value == BP_BIB_CRC32_CASTAGNOLI)
    {
        uint32_t crc = get_crc32((uint8_t*)payload, payload_size, &crc32_castagnoli);
        if(bib->security_result_data.crc32 != crc)
        {
            /* Return Failure */
            return BP_FAILEDINTEGRITYCHECK;
        }
    }

    /* Return Success */
    return BP_SUCCESS;
}
