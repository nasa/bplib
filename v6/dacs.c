/************************************************************************
 * File: dacs.c
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
#include "rb_tree.h"
#include "v6.h"
#include "sdnv.h"
#include "dacs.h"

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * dacs_write -
 *
 *  rec - buffer containing the ACS record [OUTPUT]
 *  size - size of buffer [INPUT]
 *  max_fills_per_dacs - the maximum number of allowable fills for each dacs
 *  tree - a rb_tree ptr containing the cid ranges for the bundle. The tree nodes will
 *      be deleted as they are written to the dacs. [OUTPUT]
 *  iter - a ptr to a ptr the next rb_node in the tree to extract the fill information
 *      and then delete. [OUTPUT]
 *
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int dacs_write(uint8_t* rec, int size, int max_fills_per_dacs, rb_tree_t* tree, uint32_t* flags)
{
    bp_field_t cid = { 0, 2, 0 };
    bp_field_t fill = { 0, 0, 0 };
    uint32_t sdnvflags = 0;

    /* Write Record Information */
    rec[BP_ACS_REC_TYPE_INDEX] = BP_ACS_REC_TYPE; /* record type */
    rec[BP_ACS_REC_STATUS_INDEX] = BP_ACS_ACK_MASK;

    /* Write First CID and Fills */
    int count_fills = 0; /* The number of fills that have occured so far. */

    /* Store the previous and next range fills. */
    rb_range_t range;
    rb_range_t prev_range;

    /* Get the first available range from the rb tree and fill it. */
    rb_tree_get_next(tree, &range, true, false);
    cid.value = range.value;
    fill.index = sdnv_write(rec, size, cid, &sdnvflags);
    fill.value = range.offset + 1;
    fill.index = sdnv_write(rec, size, fill, &sdnvflags);
    count_fills += 2;

    /* Traverse tree in order and write out fills to dacs. */
    while (count_fills < max_fills_per_dacs && !rb_tree_is_empty(tree))
    {
        prev_range = range;
        rb_tree_get_next(tree, &range, true, false);

        /* Write range of missing cid.
           Calculate the missing values between the current and previous node. */
        fill.value = range.value - (prev_range.value + prev_range.offset + 1);
        fill.index = sdnv_write(rec, size, fill, &sdnvflags);

        /* Write range of received cids. */
        fill.value = range.offset + 1;
        fill.index = sdnv_write(rec, size, fill, &sdnvflags);
        count_fills += 2;
    }

    /* Success Oriented Error Checking */
    if(sdnvflags != 0)
    {
        *flags |= sdnvflags;
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Flags raised during processing of DACS (%08X)\n", sdnvflags);
    }

    /* Return Block Size */
    return fill.index;
}

/*--------------------------------------------------------------------------------------
 * dacs_read -
 *-------------------------------------------------------------------------------------*/
int dacs_read(const uint8_t* rec, int rec_size, int* num_acks, bp_delete_func_t ack, void* ack_parm, uint32_t* flags)
{
    bp_val_t i;
    bp_field_t cid = { 0, 2, 0 };
    bp_field_t fill = { 0, 0, 0 };
    int cidin = true;
    uint8_t acs_status = rec[BP_ACS_REC_STATUS_INDEX];
    bool ack_success = (acs_status & BP_ACS_ACK_MASK) == BP_ACS_ACK_MASK;
    int ack_count = 0;
    uint32_t sdnvflags = 0;
    int ret_status = BP_SUCCESS;

    /* Read First Custody ID */
    fill.index = sdnv_read(rec, rec_size, &cid, &sdnvflags);
    if(sdnvflags != 0)
    {
        *flags |= sdnvflags;
        return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Failed to read first custody ID (%08X)\n", sdnvflags);
    }

    /* Process Fills */
    while((int)fill.index < rec_size)
    {
        /* Read Fill */
        fill.index = sdnv_read(rec, rec_size, &fill, &sdnvflags);
        if(sdnvflags != 0)
        {
            *flags |= sdnvflags;
            return bplog(flags, BP_FLAG_FAILED_TO_PARSE, "Failed to read fill (%08X)\n", sdnvflags);
        }

        /* Process Custody IDs */
        if(cidin == true && ack_success)
        {
            /* Free Bundles */
            cidin = false;
            for(i = 0; i < fill.value; i++)
            {
                /* Acknowledge Bundle CID */
                int status = ack(ack_parm, cid.value + i, flags);
                if(status == BP_SUCCESS) ack_count++;

                /* Set Return Status */
                if(status != BP_SUCCESS && ret_status != BP_SUCCESS)
                {
                    /* Save Off First Failure */
                    ret_status = status;
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

    /* Set Number of Acknowledgments */
    *num_acks = ack_count;

    /* Return Bytes Read or Error Code */
    if(ret_status == BP_SUCCESS)    return fill.index;
    else                            return ret_status;
}
