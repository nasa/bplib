/******************************************************************************
 * Filename     : bplib_blk_pay.c
 * Purpose      : Bundle Protocol Payload Block
 * Design Notes :
 * 
 * -------------------------------------------------
 *         DTN Aggregate Custody Signal Bundle
 * -------------------------------------------------
 * |    MSB    |           |           |    LSB    |
 * | (8 bits)  | (8 bits)  | (8 bits)  | (8 bits)  |
 * |-----------|-----------|-----------|-----------|
 * |                                               | 
 * |              Primary Bundle Block             | 
 * |                                               | 
 * |-----------|-----------|-----------------------|
 * .           .           .                       .
 * |-----------|-----------|-----------------------|
 * |    0x1    | Blk Flags |     Block Length      | --> Payload BLock
 * |-----------|-----------|-----------------------|
 * |    0x40   |  Status   |  First Custody ID...  | --> Aggregate Custody Signal   
 * |-----------|-----------|-----------------------|
 * |  ...First Custody ID  |   Number Valid CIDs   |
 * |-----------------------|-----------------------|
 * |  Number Skipped CIDs  |   Number Valid CIDs   |
 * |-----------------------|-----------------------|
 * |  Number Skipped CIDs  |   Number Valid CIDs   |
 * |-----------------------|-----------------------|
 * .                       .                       .
 * |-----------------------|-----------------------|
 * |  Number Skipped CIDs  |   Number Valid CIDs   |
 * |-----------------------------------------------|    Variable
 * 
 ******************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdlib.h>
#include "bplib.h"
#include "bplib_blk.h"
#include "bplib_blk_pay.h"
#include "bplib_sdnv.h"

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
 * bplib_blk_pay_write - 
 * 
 *  block - pointer to memory that holds bundle block [OUTPUT]
 *  size - size of block [INPUT]
 * 
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_blk_pay_write (void* block, int size)
{
    uint8_t* buffer = (uint8_t*)block;
    
    /* Check Size */
    if(size < 4) return BP_PARMERR;

    /* Set Block Flags */
    uint8_t blk_flags = 0;
    blk_flags |= BP_BLK_REPALL_MASK;
    blk_flags |= BP_BLK_LASTBLOCK_MASK;

    /* Write Block */
    buffer[0] = 0x1;   // block id
    bplib_sdnv_write(&buffer[1],   1,  blk_flags); 
    bplib_sdnv_write(&buffer[2],   2,  0);     // block length of rest of block

    /* Return Block Size */
    return 4;
}

/*--------------------------------------------------------------------------------------
 * bplib_blk_pay_update - 
 * 
 *  block - pointer to memory that holds bundle block [OUTPUT]
 *  size - size of block [INPUT]
 *  payload_size - size of the payload to place in this bundle (if fragmented, then fragment size)
 * 
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_blk_pay_update (void* block, int size, int payload_size)
{
    uint8_t* buffer = (uint8_t*)block;
    
    /* Check Size */
    if(size < 2) return BP_PARMERR;

    /* Write Block Length */
    bplib_sdnv_write(&buffer[2], 2, payload_size);     // block length of rest of block

    /* Write Number of Bytes Written */
    return 2;
}

/*--------------------------------------------------------------------------------------
 * bplib_rec_acs_process - 
 * 
 *  bundle - pointer to block holding bundle block [INPUT]
 *  size - size of block [INPUT]
 *  pri - pointer to a primary bundle block structure to be populated by this function [OUTPUT]
 * 
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int bplib_rec_acs_process ( void* rec, int size, uint8_t rec_status,
                            bp_sid_t* active_table, int table_size, 
                            bp_store_relinquish_t relinquish, int store_handle)
{
    uint32_t cid, fill, i;
    uint8_t* buf = (uint8_t*)rec;
    int cidin = BP_TRUE;
    uint8_t flags = 0;
    int index = 0;
    
    /* Read First Custody ID */
    index += bplib_sdnv_read(buf, size - index, &cid, &flags);
    if(flags != BP_SUCCESS) return BP_BUNDLEPARSEERR;

    /* Process Though Fills */
    while(index < size)
    {
        /* Read Fill */
        index += bplib_sdnv_read(buf, size - index, &fill, &flags);
        if(flags != BP_SUCCESS) return BP_BUNDLEPARSEERR;

        /* Process Custody IDs */
        if(cidin == BP_TRUE)
        {
            /* Free Bundles */
            cidin = BP_FALSE;
            for(i = 0; i < fill; i++)
            {
                int ati = (cid + i) % table_size;
                bp_sid_t sid = active_table[ati];
                if(sid != BP_SID_VACANT)
                {
                    relinquish(store_handle, &sid);
                    active_table[ati] = BP_SID_VACANT;
                }                        
            }
        }
        else
        {
            /* Skip Bundles */
            cidin = BP_TRUE;
        }

        /* Set Next Custody ID */
        cid += fill;
    }

    return index;
}

/*--------------------------------------------------------------------------------------
 * bplib_rec_acs_process - 
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
int bplib_rec_acs_write(uint8_t* rec, int size, int delivered, uint32_t first_cid, uint32_t* fills, int num_fills)
{
    int rec_index = 0, fill_index;
    
    /* Write Record */
    rec[rec_index++] = BP_PROTO_ACS_REC_TYPE; // record type
    rec[rec_index++] = BP_REC_RPTRCV_MASK | BP_REC_RPTACT_MASK | (delivered ? BP_REC_RPTDLV_MASK : BP_REC_RPTFRW_MASK);
    rec_index += bplib_sdnv_write(&rec[rec_index], 4, first_cid);
    for(fill_index = 0; fill_index < num_fills; fill_index++)
    {
        if(rec_index + 2 > size) return BP_PARMERR;
        rec_index += bplib_sdnv_write(&rec[rec_index], 2, fills[fill_index]);        
    }

    /* Return Block Size */
    return rec_index;
}
