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
int pay_read(const void *block, int size, bp_blk_pay_t *pay, bool update_indices, uint32_t *flags)
{
    uint8_t *buffer     = (uint8_t *)block;
    int      bytes_read = 0;
    uint32_t sdnvflags  = 0;

    /* Check Size */
    if (size < 1)
    {
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Invalid size of payload block: %d\n", size);
    }

    /* Read Block Information */
    if (!update_indices)
    {
        sdnv_read(buffer, size, &pay->bf, &sdnvflags);
        bytes_read = sdnv_read(buffer, size, &pay->blklen, &sdnvflags);
    }
    else
    {
        pay->bf.width     = 0;
        pay->blklen.width = 0;

        pay->bf.index     = 1;
        pay->blklen.index = sdnv_read(buffer, size, &pay->bf, &sdnvflags);
        bytes_read        = sdnv_read(buffer, size, &pay->blklen, &sdnvflags);
    }

    /* Success Oriented Error Checking */
    if (sdnvflags != 0)
    {
        *flags |= sdnvflags;
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Flags raised during processing of payload block (%08X)\n",
                     sdnvflags);
    }
    else
    {
        pay->payptr  = &buffer[bytes_read];
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
int pay_write(void *block, int size, bp_blk_pay_t *pay, bool update_indices, uint32_t *flags)
{
    uint8_t *buffer        = (uint8_t *)block;
    int      bytes_written = 0;
    uint32_t sdnvflags     = 0;

    /* Check Size */
    if (size < 1)
    {
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Invalid size of payload block: %d\n", size);
    }

    /* Set Block Flags */
    pay->bf.value = 0;
    pay->bf.value |= BP_BLK_REPALL_MASK;
    pay->bf.value |= BP_BLK_LASTBLOCK_MASK;

    /* Set Block Length */
    if (pay->payptr)
    {
        pay->blklen.value = pay->paysize;
    }

    /* Write Block */
    buffer[0] = BP_PAY_BLK_TYPE;
    if (!update_indices)
    {
        sdnv_write(buffer, size, pay->bf, &sdnvflags);
        bytes_written = sdnv_write(buffer, size, pay->blklen, &sdnvflags);
    }
    else
    {
        pay->bf.width     = 0;
        pay->blklen.width = 0;

        pay->bf.index     = 1;
        pay->blklen.index = sdnv_write(buffer, size, pay->bf, &sdnvflags);
        bytes_written     = sdnv_write(buffer, size, pay->blklen, &sdnvflags);
    }

    /* Success Oriented Error Checking */
    if (sdnvflags != 0)
    {
        *flags |= sdnvflags;
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Flags raised during processing of payload block (%08X)\n",
                     sdnvflags);
    }
    else
    {
        return bytes_written;
    }
}
