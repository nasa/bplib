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
#include "pri.h"
#include "bib.h"
#include "pay.h"
#include "sdnv.h"
#include "bplib_os.h"
#include "dacs.h"
#include "data.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BP_ACS_REC_TYPE_INDEX       0
#define BP_ACS_REC_STATUS_INDEX     1
#define BP_ACS_ACK_MASK             0x80    /* if set, then custody successfully transfered */

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* DTN Aggregate Custody Signal - Bundle */
typedef struct {
    bp_blk_pri_t        primary_block;
    bp_blk_bib_t        integrity_block;
    bp_blk_pay_t        payload_block;
    uint32_t            cstnode;
    uint32_t            cstserv;
    rb_tree_t           tree;       /* balanced tree to store bundle ids */
    uint32_t            last_dacs;  /* time of last dacs generated */
    uint8_t*            paybuf;     /* buffer to hold built DACS record */
    int                 paybuf_size;
    bp_bundle_store_t   dacs_store;
} dacs_bundle_t;

/******************************************************************************
 LOCAL DATA
 ******************************************************************************/

static const bp_blk_pri_t dacs_pri_blk = 
{
    .version            = BP_DEFAULT_BP_VERSION,
                            /*          Value         Index       Width   */
    .pcf                = { 0,                          1,          3 },
    .blklen             = { 0,                          4,          1 },
    .dstnode            = { 0,                          5,          4 },
    .dstserv            = { 0,                          9,          2 },
    .srcnode            = { 0,                          11,         4 },
    .srcserv            = { 0,                          15,         2 },
    .rptnode            = { 0,                          17,         4 },
    .rptserv            = { 0,                          21,         2 },
    .cstnode            = { 0,                          23,         4 },
    .cstserv            = { 0,                          27,         2 },
    .createsec          = { BP_DEFAULT_CREATE_SECS,     29,         6 },
    .createseq          = { 0,                          35,         4 },
    .lifetime           = { BP_DEFAULT_LIFETIME,        39,         4 },
    .dictlen            = { 0,                          43,         1 },
    .fragoffset         = { 0,                          44,         4 },
    .paylen             = { 0,                          48,         4 },
    .is_admin_rec       = true,
    .request_custody    = false,
    .allow_frag         = false,
    .is_frag            = false,
    .integrity_check    = BP_DEFAULT_ICHECK
};

static const bp_blk_bib_t dacs_bib_blk = {
                                /* Value                         Index  Width */
    .block_flags              = {  0,                            1,     1     },
    .block_length             = {  0,                            2,     4     },
    .security_target_count    = {  1,                            6,     1     },
    .security_target_type     = {  1,                            7,     1     },
    .security_target_sequence = {  0,                            8,     1     },
    .cipher_suite_id          = {  BP_DEFAULT_CIPHER_SUITE,      9,     1     },
    .cipher_suite_flags       = {  0,                            10,    1     },
    .security_result_count    = {  1,                            11,    1     },
    .security_result_type     =    0,
    .security_result_length   = {  1,                            13,    1     },
};

static const bp_blk_pay_t dacs_pay_blk = {
    .bf                       = { 0,                              1,    1     },
    .blklen                   = { 0,                              2,    4     },
    .payptr                   = NULL,
    .paysize                  = 0
};

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * dacs_insert - Attempts to insert a value into the dacs tree and sets 
 *
 * value: The value to attempt to insert into the dacs tree. [INPUT]
 * dacs: A ptr to the dacs to try to insert a value into. [OUTPUT]
 * dacsflags: A ptr to a uint16_t used to set and store flags pertaining to the creation
 *      of the current dacs. [OUTPUT]
 * returns: True if value was inserted, or false indicating dacs should be stored and
 *      new dacs started
 *-------------------------------------------------------------------------------------*/
static bool dacs_insert(dacs_bundle_t* bundle, uint32_t value, uint16_t* dacsflags)
{
    rb_tree_status_t status = rb_tree_insert(value, &bundle->tree);
    if (status == RB_FAIL_TREE_FULL) 
    {
        /* We failed the insert because the tree is full therefore we need to
         * store this dacs and start a new one. */
        *dacsflags |= BP_FLAG_RBTREEFULL;
        return false;
    }
    else if (status == RB_FAIL_INSERT_DUPLICATE)
    {
        /* Duplicate values are fine and are treated as a success */
        *dacsflags |= BP_FLAG_DUPLICATES;
        return true;
    }
    else
    {
        /* Insertion successful */
        return true;
    }
}

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
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
static int dacs_write(uint8_t* rec, int size, int max_fills_per_dacs, rb_tree_t* tree, rb_node_t** iter)
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
    fill.index = sdnv_write(rec, size, cid, &flags);
    fill.value = range.offset + 1;
    fill.index = sdnv_write(rec, size, fill, &flags);    
    count_fills += 2;

    /* Traverse tree in order and write out fills to dacs. */
    while (count_fills < max_fills_per_dacs && *iter != NULL)
    {
        prev_range = range;
        rb_tree_get_next_rb_node(tree, iter, &range, true, false);        

        /* Write range of missing cid.
           Calculate the missing values between the current and previous node. */
        fill.value = range.value - (prev_range.value + prev_range.offset + 1);
        fill.index = sdnv_write(rec, size, fill, &flags);

        /* Write range of received cids. */
        fill.value = range.offset + 1;
        fill.index = sdnv_write(rec, size, fill, &flags);    
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

/*--------------------------------------------------------------------------------------
 * dacs_store -
 *
 *  Notes:
 *-------------------------------------------------------------------------------------*/
static int dacs_store(dacs_bundle_t* bundle, uint32_t sysnow, int timeout, int max_fills_per_dacs, bp_store_enqueue_t enqueue, int store_handle, uint16_t* dacsflags)
{
    int dacs_size, enstat, enstat_fail, storage_header_size;
    bool has_enqueue_failure = false;

    /* If the tree has nodes, initialize the iterator for traversing the tree in order */
    rb_node_t* iter;
    rb_tree_get_first_rb_node(&bundle->tree, &iter);

    /* Continue to delete nodes from the tree and write them to DACS until the tree is empty */
    while (!rb_tree_is_empty(&bundle->tree))
    {
        bp_bundle_store_t* ds = &bundle->dacs_store;
        bp_blk_pri_t* pri = &bundle->primary_block;

        /* Build DACS - will remove nodes from the tree */
        dacs_size = dacs_write(bundle->paybuf, bundle->paybuf_size, max_fills_per_dacs, &bundle->tree, &iter);
        ds->bundlesize = ds->headersize + dacs_size;
        storage_header_size = sizeof(bp_bundle_store_t) - (BP_BUNDLE_HDR_BUF_SIZE - ds->headersize);

        /* Set Creation Time */
        pri->createsec.value = sysnow;
        sdnv_write(ds->header, BP_BUNDLE_HDR_BUF_SIZE, pri->createsec, dacsflags);

        /* Set Sequence */
        sdnv_write(ds->header, BP_BUNDLE_HDR_BUF_SIZE, pri->createseq, dacsflags);
        pri->createseq.value++;

        /* Update Bundle Integrity Block */
        if(ds->biboffset != 0)
        {
            bib_update(&ds->header[ds->biboffset], BP_BUNDLE_HDR_BUF_SIZE - ds->biboffset, bundle->paybuf, dacs_size, &bundle->integrity_block);
        }
        
        /* Update Payload Block */
        bundle->payload_block.payptr = bundle->paybuf;
        bundle->payload_block.paysize = dacs_size;
        bundle->payload_block.blklen.value = dacs_size;
        sdnv_write(&ds->header[ds->payoffset], BP_BUNDLE_HDR_BUF_SIZE - ds->payoffset, bundle->payload_block.blklen, dacsflags);

        /* Send (enqueue) DACS */
        enstat = enqueue(store_handle, ds, storage_header_size, bundle->paybuf, dacs_size, timeout);
        bundle->last_dacs = sysnow;
        
        /* Check Storage Status */
        if(enstat <= 0)
        {
            /* Failure enqueing DACS */
            if(!has_enqueue_failure)
            {
                /* Save first failed DACS enqueue to return later */
                enstat_fail = enstat;
                bplog(enstat, "Failed (%d) to store DACS for transmission, bundle dropped\n", enstat);
            }
    
            has_enqueue_failure = true;
        }
        else 
        {
            /* DACS successfully enqueued */
            bundle->last_dacs = sysnow;
        }
    }

    /* Check if one or more of the DACS to be enqueued failed */
    if(has_enqueue_failure)
    {
        *dacsflags |= BP_FLAG_STOREFAILURE;        
        return enstat_fail;
    }

    /* All DACS were successfully enqueued and the CID tree is now empty */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * dacs_new - Populate ACS Bundle Header from Channel Struct
 *
 *  Does not populate following (see update_dacs_header):
 *      - creation time (when using system time)
 *      - creation sequence
 *      - total payload length
 *      - payload crc
 *      - payload block length
 *-------------------------------------------------------------------------------------*/
static int dacs_new(dacs_bundle_t* bundle, uint32_t dstnode, uint32_t dstserv, uint16_t* dacsflags)
{
    bp_bundle_store_t*  ds      = &bundle->dacs_store;
    uint8_t*            hdrbuf  = ds->header;
    uint16_t            flags   = 0;
    int                 offset  = 0;

    /* Initialize Storage */
    memset(ds, 0, sizeof(bp_bundle_store_t));

    /* Write Primary Block */
    offset = pri_write(&hdrbuf[0], BP_BUNDLE_HDR_BUF_SIZE, &bundle->primary_block, false);

    /* Write Integrity Block */
    if(bundle->primary_block.integrity_check)
    {
        ds->biboffset = offset;
        offset = bib_write(&hdrbuf[offset], BP_BUNDLE_HDR_BUF_SIZE - offset, &bundle->integrity_block, false) + offset;
    }
    else
    {
        ds->biboffset = 0;
    }
    
    /* Write Payload Block */
    ds->payoffset = offset;
    ds->headersize = pay_write(&hdrbuf[offset], BP_BUNDLE_HDR_BUF_SIZE - offset, &bundle->payload_block, false) + offset;

    /* Set Destination EID */
    bundle->primary_block.dstnode.value = dstnode;
    bundle->primary_block.dstserv.value = dstserv;
    sdnv_write(hdrbuf, BP_BUNDLE_HDR_BUF_SIZE, bundle->primary_block.dstnode, &flags);
    sdnv_write(hdrbuf, BP_BUNDLE_HDR_BUF_SIZE, bundle->primary_block.dstserv, &flags);

    /* Set Custody EID */
    bundle->cstnode = dstnode;
    bundle->cstserv = dstserv;
    
    /* Return Status */
    *dacsflags |= flags;
    if(flags != 0)  return BP_BUNDLEPARSEERR;
    else            return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * dacs_update -
 *
 *  Notes:  may or may not perform enqueue depending if DACS needs to be sent
 *          based on whether or not dacs record is full
 *-------------------------------------------------------------------------------------*/
static int dacs_update(dacs_bundle_t* bundle, uint32_t cid, uint32_t sysnow, int timeout, int max_fills_per_dacs, bp_store_enqueue_t enqueue, int store_handle, uint16_t* dacsflags)
{
    /* Attempt to insert cid into bundle. */
    bool insert_status = dacs_insert(bundle, cid, dacsflags);
    if(!insert_status)
    {
        /* Store DACS */
        dacs_store(bundle, sysnow, timeout, max_fills_per_dacs, enqueue, store_handle, dacsflags);

        /* Start New DTN ACS */
        insert_status = dacs_insert(bundle, cid, dacsflags);
        if(!insert_status)
        {
            return BP_FAILEDRESPONSE;
        }
    }

    /* Return Success */
    return BP_SUCCESS;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * dacs_initialize - Allocates resources for DACS and initializes control structures
 *-------------------------------------------------------------------------------------*/
int dacs_initialize (bp_dacs_t* dacs, int max_acks, int max_fills, int max_gaps, int local_node, int local_service)
{
    int j;
    
    /* Set Number Attributes */
    dacs->max_acks  = max_acks;
    dacs->max_fills = max_fills;
    dacs->max_gaps  = max_gaps;
    dacs->num_acks  = 0;
    
    /* Allocate Memory for DACS Bundle */
    dacs->acks_list = malloc(sizeof(dacs_bundle_t) * max_acks);
    if(dacs->acks_list == NULL)
    {
        return bplog(BP_FAILEDMEM, "Failed to allocate memory for channel dacs\n");                    
    }
    else
    {
        memset(dacs->acks_list, 0, sizeof(dacs_bundle_t) * max_acks);
    }

    /* Initialize DACS Bundle */
    for(j = 0; j < max_acks; j++)
    {
        dacs_bundle_t* bundle_list = (dacs_bundle_t*)dacs->acks_list;
        
        /* Allocate Memory for Channel DACS Bundle Fills */
        bundle_list[j].paybuf_size = sizeof(uint16_t) * max_fills + 32; // 2 bytes per fill plus payload block header
        bundle_list[j].paybuf = (uint8_t*)malloc(bundle_list[j].paybuf_size); 
        if(bundle_list[j].paybuf == NULL)
        {
            return bplog(BP_FAILEDMEM, "Failed to allocate memory for channel dacs fills and payload\n");                    
        }
        else
        {
            memset(bundle_list[j].paybuf, 0, bundle_list[j].paybuf_size);
        }

        /* Allocate Memory for Channel DACS Tree to Store Bundle IDs */
        int status = rb_tree_create(max_gaps, &bundle_list[j].tree);
        if (status != RB_SUCCESS)
        {
            return bplog(BP_FAILEDMEM, "Failed to allocate memory for channel dacs tree\n");
        }

        /* Initialize DACS Bundle Fields */
        bundle_list[j].primary_block               = dacs_pri_blk;
        bundle_list[j].primary_block.srcnode.value = local_node;
        bundle_list[j].primary_block.srcserv.value = local_service;
        bundle_list[j].primary_block.rptnode.value = 0;
        bundle_list[j].primary_block.rptserv.value = 0;
        bundle_list[j].primary_block.cstnode.value = local_node;
        bundle_list[j].primary_block.cstserv.value = local_service;
        bundle_list[j].integrity_block             = dacs_bib_blk;
        bundle_list[j].payload_block               = dacs_pay_blk;
    }
    
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * dacs_uninitialize - Frees resources for DACS
 *-------------------------------------------------------------------------------------*/
void dacs_uninitialize(bp_dacs_t* dacs)
{
    int j;
    
    if(dacs)
    {
        dacs_bundle_t* bundle_list = (dacs_bundle_t*)dacs->acks_list;

        for(j = 0; j < dacs->max_acks; j++)
        {
            if(bundle_list[j].paybuf) free(bundle_list[j].paybuf);
            if(bundle_list[j].tree.max_size > 0) 
            {
                rb_tree_destroy(&bundle_list[j].tree);
            }
        }

        free(bundle_list);
    }    
}

/*--------------------------------------------------------------------------------------
 * dacs_acknowledge -
 *-------------------------------------------------------------------------------------*/
int dacs_acknowledge(bp_dacs_t* dacs, bp_blk_cteb_t* cteb, uint32_t sysnow, int timeout, bp_store_enqueue_t enqueue, int store_handle, uint16_t* dacsflags)
{
    int j;
    int status = BP_SUCCESS;

    dacs_bundle_t* bundle_list = (dacs_bundle_t*)dacs->acks_list;

    /* Find DACS Entry */                   
    dacs_bundle_t* bundle = NULL;                    
    for(j = 0; j < dacs->num_acks; j++)
    {
        if(bundle_list[j].cstnode == cteb->cstnode && bundle_list[j].cstserv == cteb->cstserv)
        {
            bundle = &bundle_list[j];
            break;
        }
    }

    /* Handle Entry Not Found */
    if(bundle == NULL)
    {
        if(dacs->num_acks < dacs->max_acks)
        {
            /* Initial DACS Bundle */
            bundle = &bundle_list[dacs->num_acks++];
            dacs_new(bundle, cteb->cstnode, cteb->cstserv, dacsflags);
        }
        else
        {
            /* No Room in Table for Another Source */
            *dacsflags |= BP_FLAG_TOOMANYSOURCES;
            status = bplog(BP_FAILEDRESPONSE, "No room in DACS table for another source %d.%d\n", cteb->cstnode, cteb->cstserv);
        }
    }

    /* Update DACS */
    if(bundle != NULL)
    {
        status = dacs_update(bundle, cteb->cid.value, sysnow, timeout, dacs->max_fills, enqueue, store_handle, dacsflags);
    }
    
    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * dacs_check -
 *
 *  Notes:  may or may not perform enqueue depending if DACS needs to be sent
 *          based on whether or not it has been too long a time without sending a DACS
 *-------------------------------------------------------------------------------------*/
int dacs_check(bp_dacs_t* dacs, uint32_t period, uint32_t sysnow, int timeout, bp_store_enqueue_t enqueue, int store_handle, uint16_t* dacsflags)
{   
    int j;
    int ret_status = BP_SUCCESS;

    if(period > 0)
    {
        dacs_bundle_t* bundle_list = (dacs_bundle_t*)dacs->acks_list;
        for(j = 0; j < dacs->num_acks; j++)
        {
            if( (sysnow >= (bundle_list[j].last_dacs + period)) && 
                !rb_tree_is_empty(&bundle_list[j].tree) )
            {
                int status = dacs_store(&bundle_list[j], sysnow, timeout, dacs->max_fills, enqueue, store_handle, dacsflags);
                if(status != BP_SUCCESS && ret_status == BP_SUCCESS)
                {
                    ret_status = status;
                }
            }
        }
    }

    return ret_status;
}

/*--------------------------------------------------------------------------------------
 * dacs_process -
 *
 *  rec - pointer to ACS administrative record [INPUT]
 *  size - size of record [INPUT]
 *  acks - number of bundles acknowledged
 *
 *  Returns:    Number of bytes processed of bundle
 *-------------------------------------------------------------------------------------*/
int dacs_process(void* rec, int size, int* acks, bp_sid_t* sids, int table_size, bp_store_relinquish_t relinquish, int store_handle, uint16_t* dacsflags)
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
    fill.index = sdnv_read(buf, size, &cid, &flags);
    if(flags != 0)
    {
        *dacsflags |= flags;
        return bplog(BP_BUNDLEPARSEERR, "Failed to read first custody ID (%08X)\n", flags);
    }

    /* Process Though Fills */
    while((int)fill.index < size)
    {

        /* Read Fill */
        fill.index = sdnv_read(buf, size, &fill, &flags);
        if(flags != 0)
        {
            *dacsflags |= flags;
            return bplog(BP_BUNDLEPARSEERR, "Failed to read fill (%08X)\n", flags);
        }

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
