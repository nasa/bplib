/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "bplib_os.h"
#include "lrc.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define LRC_BLOCK_SIZE           7
#define LRC_CODE_BYTES_PER_BLOCK 2

/******************************************************************************
 LOCAL FILE DATA
 ******************************************************************************/

static uint8_t *LRC_XOR_TABLE = NULL;
static int8_t  *LRC_RCI_TABLE = NULL; /* row-column-index */

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * build_xor_table -
 *
 * table[256] = {
 *   0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
 *   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
 *   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
 *   0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
 *   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
 *   0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
 *   0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
 *   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
 *   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
 *   0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
 *   0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
 *   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
 *   0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
 *   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
 *   1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
 *   0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
 *  }
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE void build_xor_table(uint8_t *table)
{
    int i, k;

    for (i = 0; i < 256; i++)
    {
        int bit_count = 0;

        for (k = 0; k < 8; k++)
        {
            if ((1 << k) & i)
            {
                bit_count++;
            }
        }

        table[i] = ((bit_count % 2) == 1);
    }
}

/*--------------------------------------------------------------------------------------
 * build_rci_table -
 *   0:     no errors (BP_ECC_NO_ERRORS)
 *  -1:     multiple errors (BP_ECC_UNCOR_ERRORS)
 *  >0:     row/column of error, counting from 1 (BP_ECC_COR_ERRORS)
 *
 *  table[256] = {
 *    0,  1,  2, -1,  3, -1, -1, -1,  4, -1, -1, -1, -1, -1, -1, -1, // 0x00 - 0x0F
 *    5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x10 - 0x1F
 *    6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x20 - 0x2F
 *   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x30 - 0x3F
 *    7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x40 - 0x4F
 *   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x50 - 0x5F
 *   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x60 - 0x6F
 *   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x70 - 0x7F
 *    8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x80 - 0x8F
 *   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x90 - 0x9F
 *   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0xA0 - 0xAF
 *   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0xB0 - 0xBF
 *   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0xC0 - 0xCF
 *   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0xD0 - 0xDF
 *   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0xE0 - 0xEF
 *   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1  // 0xF0 - 0xFF
 *  }
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE void build_rci_table(int8_t *table)
{
    int i, k;

    for (i = 0; i < 256; i++)
    {
        table[i] = BP_ECC_NO_ERRORS;
        for (k = 0; k < 8; k++)
        {
            if ((1 << k) & i)
            {
                if (table[i] == BP_ECC_NO_ERRORS)
                {
                    table[i] = k + 1;
                }
                else
                {
                    table[i] = BP_ECC_UNCOR_ERRORS;
                }
            }
        }
    }
}

/*--------------------------------------------------------------------------------------
 * lrc_block_encode -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE void lrc_block_encode(uint8_t *src_block, uint8_t *src_code, int bytes_to_ecc, uint8_t *dst_code)
{
    int     index   = 0;
    uint8_t ecc_col = 0;
    uint8_t ecc_row = 0;

    /* Loop Through All Bytes in Block */
    while (index < bytes_to_ecc)
    {
        /* Encode Current ECC Block */
        ecc_col ^= src_block[index]; /* column */
        ecc_row |= LRC_XOR_TABLE[src_block[index]] << index;

        /* Go to Next Index */
        index++;
    }

    /* Encode Row Parity */
    if (src_code)
    {
        ecc_row |= LRC_XOR_TABLE[src_code[0]] << index;
    }
    else
    {
        ecc_row |= LRC_XOR_TABLE[ecc_col] << index;
    }

    /* Write ECC */
    dst_code[0] = ecc_col;
    dst_code[1] = ecc_row;
}

/*--------------------------------------------------------------------------------------
 * lrc_block_decode -
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int lrc_block_decode(uint8_t *src_block, uint8_t *src_code, uint8_t *calc_code)
{
    uint8_t src_col  = src_code[0];
    uint8_t src_row  = src_code[1];
    uint8_t calc_col = calc_code[0];
    uint8_t calc_row = calc_code[1];

    /* Check ECC */
    if (calc_col != src_col || calc_row != src_row)
    {
        uint8_t delta_col = calc_col ^ src_col;
        uint8_t delta_row = calc_row ^ src_row;

        /* Find Row & Column Error */
        int column = LRC_RCI_TABLE[delta_col];
        int row    = LRC_RCI_TABLE[delta_row];

        /* Attempt to Correct Error */
        if (column > 0 && row > 0)
        {
            /* correct bit at row:column */
            src_block[row - 1] ^= (1 << (column - 1));
            return BP_ECC_COR_ERRORS;
        }
        /* this only checks for errors in the row ecc byte since the column ecc byte
            is parity checked; there is no case where a column has a single bit error,
            and it doesn't show up in the row ecc */
        else if ((column == BP_ECC_NO_ERRORS) && (row > 0))
        {
            /* do nothing - error in row ECC byte */
            return BP_ECC_COR_ERRORS;
        }
        else
        {
            /* uncorrectable error */
            return BP_ECC_UNCOR_ERRORS;
        }
    }

    return BP_ECC_NO_ERRORS;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * lrc_init -
 *
 *  returns number of bytes available for data
 *-------------------------------------------------------------------------------------*/
int lrc_init(int frame_size)
{
    /* Allocate ECC Tables and Buffers */
    LRC_XOR_TABLE = (uint8_t *)bplib_os_calloc(256);
    LRC_RCI_TABLE = (int8_t *)bplib_os_calloc(256);
    if (!LRC_XOR_TABLE || !LRC_RCI_TABLE)
    {
        lrc_uninit();
        return frame_size; /* on error, all bytes available for data, since ecc failed to initialize */
    }

    /* Initialize ECC Tables */
    build_xor_table(LRC_XOR_TABLE);
    build_rci_table(LRC_RCI_TABLE);

    /* Return Bytes Available for Data */
    return (LRC_BLOCK_SIZE * frame_size) / (LRC_BLOCK_SIZE + LRC_CODE_BYTES_PER_BLOCK);
}

/*--------------------------------------------------------------------------------------
 * lrc_uninit -
 *-------------------------------------------------------------------------------------*/
void lrc_uninit(void)
{
    if (LRC_XOR_TABLE)
    {
        bplib_os_free(LRC_XOR_TABLE);
        LRC_XOR_TABLE = NULL;
    }

    if (LRC_RCI_TABLE)
    {
        bplib_os_free(LRC_RCI_TABLE);
        LRC_RCI_TABLE = NULL;
    }
}

/*--------------------------------------------------------------------------------------
 * lrc_encode -
 *-------------------------------------------------------------------------------------*/
void lrc_encode(uint8_t *frame_buffer, int data_size)
{
    int data_index = 0;
    int ecc_index  = data_size;

    /* Loop Through All Data in Page */
    while (data_index < data_size)
    {
        /* Encode ECC for Block */
        int bytes_left   = data_size - data_index;
        int bytes_to_ecc = bytes_left < LRC_BLOCK_SIZE ? bytes_left : LRC_BLOCK_SIZE;
        lrc_block_encode(&frame_buffer[data_index], NULL, bytes_to_ecc, &frame_buffer[ecc_index]);

        /* Goto Next Block */
        data_index += LRC_BLOCK_SIZE;
        ecc_index += 2;
    }
}

/*--------------------------------------------------------------------------------------
 * lrc_decode -
 *-------------------------------------------------------------------------------------*/
int lrc_decode(uint8_t *frame_buffer, int data_size)
{
    int ret_status = BP_ECC_NO_ERRORS;

    int     data_index = 0;
    int     ecc_index  = data_size;
    uint8_t ecc_code[LRC_CODE_BYTES_PER_BLOCK];

    /* Loop Through All Data in Page */
    while (data_index < data_size)
    {
        /* Generate ECC for Block */
        int bytes_left   = data_size - data_index;
        int bytes_to_ecc = bytes_left < LRC_BLOCK_SIZE ? bytes_left : LRC_BLOCK_SIZE;
        lrc_block_encode(&frame_buffer[data_index], &frame_buffer[ecc_index], bytes_to_ecc, ecc_code);

        /* Check (and correct) ECC for Block */
        int status = lrc_block_decode(&frame_buffer[data_index], &frame_buffer[ecc_index], ecc_code);
        if (status == BP_ECC_UNCOR_ERRORS)
        {
            ret_status = status;
            break;
        }
        else if (status == BP_ECC_COR_ERRORS)
        {
            ret_status = status;
        }

        /* Goto Next Block */
        data_index += LRC_BLOCK_SIZE;
        ecc_index += 2;
    }

    return ret_status;
}
