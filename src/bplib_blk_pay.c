/************************************************************************
 * File: bplib_blk_pay.c
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
#include "bplib_blk_pay.h"
#include "bplib_sdnv.h"
#include "bplib_os.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_REC_RPTRCV_MASK      0x01    // report reception
#define BP_REC_RPTACT_MASK      0x02    // report acceptance
#define BP_REC_RPTFRW_MASK      0x04    // report forwarding
#define BP_REC_RPTDLV_MASK      0x08    // report delivery
#define BP_REC_RPTDLT_MASK      0x10    // report deletion

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_blk_pay_read -
 *
 *  block - pointer to block holding bundle block [INPUT]
 *  size - size of block [INPUT]
 *  pay - point to a payload block structure [OUTPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Next index
 *-------------------------------------------------------------------------------------*/
int bplib_blk_pay_read (void* block, int size, bp_blk_pay_t* pay, bool update_indices)
{
    uint8_t* buffer = (uint8_t*)block;
    uint16_t flags = 0;
    int bytes_read = 0;

    /* Check Size */
    if(size < 1) return bplog(BP_BUNDLEPARSEERR, "Invalid size of payload block: %d\n", size);

    /* Read Block Information */
    if(!update_indices)
    {
        bplib_sdnv_read(buffer, size, &pay->bf, &flags);
        bytes_read = bplib_sdnv_read(buffer, size, &pay->blklen, &flags);
    }
    else
    {
        pay->bf.width = 0;
        pay->blklen.width = 0;

        pay->bf.index = 1;
        pay->blklen.index = bplib_sdnv_read(buffer, size, &pay->bf, &flags);
        bytes_read = bplib_sdnv_read(buffer, size, &pay->blklen, &flags);
    }

    /* Success Oriented Error Checking */
    if(flags != 0)
    {
        return bplog(BP_BUNDLEPARSEERR, "Error flags raised during processing of payload block (%08X)\n", flags);
    }
    else
    {
        pay->payptr = &buffer[bytes_read];
        pay->paysize = pay->blklen.value;
        return bytes_read;
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_pay_write -
 *
 *  block - pointer to memory that holds bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  pay - pointer to a payload block structure [INPUT]
 *  update_indices - boolean, 0: use <sdnv>.index, 1: update <sdnv>.index as you go [INPUT]
 *
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_blk_pay_write (void* block, int size, bp_blk_pay_t* pay, bool update_indices)
{
    uint8_t* buffer = (uint8_t*)block;
    uint16_t flags = 0;
    int bytes_written = 0;

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
        bplib_sdnv_write(buffer, size, pay->bf, &flags);
        bytes_written = bplib_sdnv_write(buffer, size, pay->blklen, &flags);
    }
    else
    {
        pay->bf.width = 0;
        pay->blklen.width = 0;

        pay->bf.index = 1;
        pay->blklen.index = bplib_sdnv_write(buffer, size, pay->bf, &flags);
        bytes_written = bplib_sdnv_write(buffer, size, pay->blklen, &flags);
    }

    /* Copy Bytes */
//    if(copy)
//    {
//        if(size >= (bytes_written + pay->paysize))
//        {
//            bplib_os_memcpy(&buffer[bytes_written], pay->payptr, pay->paysize);
//            bytes_written += pay->paysize;
//        }
//        else
//        {
//            return bplog(BP_BUNDLEPARSEERR, "Unable to write payload, buffer too small (%d %d)\n", size, bytes_written + pay->paysize);
//        }
//    }

    /* Success Oriented Error Checking */
    if(flags != 0)  return bplog(BP_BUNDLEPARSEERR, "Flags raised during processing of payload block (%08X)\n", flags);
    else            return bytes_written;
}

/*--------------------------------------------------------------------------------------
 * bplib_rec_acs_process -
 *
 *  rec - pointer to ACS administrative record [INPUT]
 *  size - size of record [INPUT]
 *  acks - number of bundles acknowledged
 *
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_rec_acs_process ( void* rec, int size, int* acks,
                            bp_sid_t* sids, int table_size,
                            bp_store_relinquish_t relinquish, int store_handle)
{
    uint32_t i;
    bp_sdnv_t cid = { 0, 2, 0 };
    bp_sdnv_t fill = { 0, 0, 0 };
    uint8_t* buf = (uint8_t*)rec;
    int cidin = true;
    uint16_t flags = 0;

    /* Initialize Acknowledgment Count */
    *acks = 0;

    /* Read First Custody ID */
    fill.index = bplib_sdnv_read(buf, size, &cid, &flags);
    if(flags != 0) return bplog(BP_BUNDLEPARSEERR, "Failed to read first custody ID (%08X)\n", flags);

    /* Process Though Fills */
    while((int)fill.index < size)
    {
        /* Read Fill */
        fill.index = bplib_sdnv_read(buf, size, &fill, &flags);
        if(flags != 0) return bplog(BP_BUNDLEPARSEERR, "Failed to read fill (%08X)\n", flags);

        /* Process Custody IDs */
        if(cidin == true)
        {
            /* Free Bundles */
            cidin = false;
            for(i = 0; i < fill.value; i++)
            {
                int ati = (cid.value + i) % table_size;
                bp_sid_t sid = sids[ati];
                if(sid != BP_SID_VACANT)
                {
                    relinquish(store_handle, sid);
                    sids[ati] = BP_SID_VACANT;
                    (*acks)++;
                }
            }
        }
        else
        {
            /* Skip Bundles */
            cidin = true;
        }

        /* Set Next Custody ID */
        cid.value += fill.value;
    }

    return fill.index;
}

/*--------------------------------------------------------------------------------------
 * bplib_rec_acs_write -
 *
 *  rec - buffer containing the ACS record [OUTPUT]
 *  size - size of buffer [INPUT]
 *  first_cid - first Custody ID [INPUT]
 *  fills - array of fill values [INPUT]
 *  num_fills - size of fills array [INPUT]
 *  delivered - 0: forwarded, 1: delivered
 *
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_rec_acs_write(uint8_t* rec, int size, bool delivered, uint32_t first_cid, uint32_t* fills, int num_fills)
{
    bp_sdnv_t cid = { 0, 2, 4 };
    bp_sdnv_t fill = { 0, 0, 2 };
    int fill_index = 0;
    uint16_t flags = 0;

    /* Write Record Information */
    rec[0] = BP_ACS_REC_TYPE; // record type
    rec[1] = BP_REC_RPTRCV_MASK | BP_REC_RPTACT_MASK | (delivered ? BP_REC_RPTDLV_MASK : BP_REC_RPTFRW_MASK);

    /* Write First CID and Fills */
    cid.value = first_cid;
    fill.index = bplib_sdnv_write(rec, size, cid, &flags);
    for(fill_index = 0; fill_index < num_fills; fill_index++)
    {
        fill.value = fills[fill_index];
        fill.index = bplib_sdnv_write(rec, size, fill, &flags);
    }

    /* Success Oriented Error Checking */
    if(flags != 0) return bplog(BP_BUNDLEPARSEERR, "Flags raised during processing of ACS (%08X)\n", flags);

    /* Return Block Size */
    return fill.index;
}
