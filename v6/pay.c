/************************************************************************
 * File: bplib_blk_pay.c
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
#include "block.h"
#include "pay.h"
#include "sdnv.h"
#include "os_api.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_ACS_REC_TYPE_INDEX   0
#define BP_ACS_REC_STATUS_INDEX 1
#define BP_ACS_ACK_MASK         0x80    /* if set, then custody successfully transfered */

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
    uint8_t acs_status = buf[BP_ACS_REC_STATUS_INDEX];
    bool ack_success = (acs_status & BP_ACS_ACK_MASK) == BP_ACS_ACK_MASK;
    
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
        if(cidin == true && ack_success)
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
 *  max_fills_per_dacs - the maximum number of allowable fills for each dacs
 *  tree - a rb_tree ptr containing the cid ranges for the bundle. The tree nodes will 
 *      be deleted as they are written to the dacs. [OUTPUT]
 *  iter - a ptr to a ptr the next rb_node in the tree to extract the fill information
 *      and then delete. [OUTPUT]
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_rec_acs_write(uint8_t* rec, int size, int max_fills_per_dacs, rb_tree_t* tree,
                        rb_node_t** iter)
{
    bp_sdnv_t cid = { 0, 2, 4 };
    bp_sdnv_t fill = { 0, 0, 2 };
    uint16_t flags = 0;

    /* Write Record Information */
    rec[BP_ACS_REC_TYPE_INDEX] = BP_ACS_REC_TYPE; /* record type */
    rec[BP_ACS_REC_STATUS_INDEX] = BP_ACS_ACK_MASK;

    /* Write First CID and Fills */
    int count_fills = 0; /* The number of fills that have occured so far. */

    /* Store the previous and next range fills. */
    rb_range_t range;
    rb_range_t prev_range;

    /* Get the first available range from the rb tree and fill it. */
    rb_tree_get_next_rb_node(tree, iter, &range, true, false);
    cid.value = range.value;
    fill.index = bplib_sdnv_write(rec, size, cid, &flags);
    fill.value = range.offset + 1;
    fill.index = bplib_sdnv_write(rec, size, fill, &flags);    
    count_fills += 2;

    /* Traverse tree in order and write out fills to dacs. */
    while (count_fills < max_fills_per_dacs && *iter != NULL)
    {
        prev_range = range;
        rb_tree_get_next_rb_node(tree, iter, &range, true, false);        

        /* Write range of missing cid.
           Calculate the missing values between the current and previous node. */
        fill.value = range.value - (prev_range.value + prev_range.offset + 1);
        fill.index = bplib_sdnv_write(rec, size, fill, &flags);

        /* Write range of received cids. */
        fill.value = range.offset + 1;
        fill.index = bplib_sdnv_write(rec, size, fill, &flags);    
        count_fills += 2;        
    }

    /* Success Oriented Error Checking */
    if(flags != 0)
    {
        return bplog(BP_BUNDLEPARSEERR,
                     "Flags raised during processing of ACS (%08X)\n", flags); 
    } 

    /* Return Block Size */
    return fill.index;
}
