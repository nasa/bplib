/******************************************************************************
 * Filename     : bplib.c
 * Purpose      : Bundle Protocol Library
 * Design Notes :
 *
 * -------------------------------------------------
 *                    Data Bundle
 * -------------------------------------------------
 * |    MSB    |           |           |    LSB    |
 * | (8 bits)  | (8 bits)  | (8 bits)  | (8 bits)  |
 * |-----------|-----------|-----------|-----------|    0
 * |                                               |
 * |              Primary Bundle Block             |
 * |              (see bplib_blk_pri.c)            |
 * |                                               |
 * |-----------------------------------------------|    52
 * |                                               |
 * |        Custody Transfer Extension Block       |
 * |             (see bplib_blk_cteb.c)            |
 * |                                               |
 * |-----------------------------------------------|    64
 * |                                               |
 * |             Bundle Integrity Block            |
 * |              (see bplib_blk_bib.c)            |
 * |                                               |
 * |-----------|-----------|-----------------------|    72
 * |                                               |    76 --> start of the payload
 * |              Bundle Payload Block             |
 * |              (see bplib_blk_pay.c)            |
 * |                                               |
 * |-----------------------------------------------|    4096 --> variable, not to exceed
 *
 * -------------------------------------------------
 *              Aggregate Custody Bundle
 * -------------------------------------------------
 * |    MSB    |           |           |    LSB    |
 * | (8 bits)  | (8 bits)  | (8 bits)  | (8 bits)  |
 * |-----------|-----------|-----------|-----------|    0
 * |                                               |
 * |              Primary Bundle Block             |
 * |              (see bplib_blk_pri.c)            |
 * |                                               |
 * |-----------------------------------------------|    52
 * |                                               |
 * |             Bundle Integrity Block            |
 * |              (see bplib_blk_bib.c)            |
 * |                                               |
 * |-----------------------------------------------|    60
 * |                                               |    64 --> start of the payload
 * |              Bundle Payload Block             |
 * |              (see bplib_blk_pay.c)            |
 * |                                               |
 * |-----------------------------------------------|
 *
 ******************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>

#include "bplib.h"
#include "bplib_os.h"
#include "bplib_sdnv.h"
#include "bplib_blk.h"
#include "bplib_blk_pri.h"
#include "bplib_blk_cteb.h"
#include "bplib_blk_bib.h"
#include "bplib_blk_pay.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#ifndef BP_ACTIVE_TABLE_SIZE
#define BP_ACTIVE_TABLE_SIZE            16384
#endif

#ifndef BP_DACS_TABLE_SIZE
#define BP_DACS_TABLE_SIZE              4   // maximum number of custody eids to keep track of per ACS report period
#endif

#ifndef BP_MAX_CHANNELS
#define BP_MAX_CHANNELS                 4
#endif

#ifndef BP_MAX_FILLS_PER_DACS
#define BP_MAX_FILLS_PER_DACS           64
#endif

#ifndef BP_DEFAULT_PAY_CRC
#define BP_DEFAULT_PAY_CRC              BP_BIB_CRC16
#endif

#ifndef BP_DEFAULT_TIMEOUT
#define BP_DEFAULT_TIMEOUT              10
#endif

#ifndef BP_DEFAULT_CREATE_TIME_SYS
#define BP_DEFAULT_CREATE_TIME_SYS      BP_TRUE
#endif

#ifndef BP_DEFAULT_CREATE_TIME_VAL_S
#define BP_DEFAULT_CREATE_TIME_VAL_S    0
#endif

#ifndef BP_DEFAULT_CREATE_TIME_VAL_NS
#define BP_DEFAULT_CREATE_TIME_VAL_NS   0
#endif

#ifndef BP_DEFAULT_CSTRQST
#define BP_DEFAULT_CSTRQST              BP_TRUE
#endif

#ifndef BP_DEFAULT_LIFETIME
#define BP_DEFAULT_LIFETIME             0
#endif

#ifndef BP_DEFAULT_BUNDLE_MAXLENGTH
#define BP_DEFAULT_BUNDLE_MAXLENGTH     4096
#endif

#ifndef BP_DEFAULT_SEQ_RESET_PERIOD
#define BP_DEFAULT_SEQ_RESET_PERIOD     0
#endif

#ifndef BP_DEFAULT_PROC_ADMIN_ONLY
#define BP_DEFAULT_PROC_ADMIN_ONLY      BP_TRUE
#endif

#ifndef BP_DEFAULT_WRAP_RESPONSE
#define BP_DEFAULT_WRAP_RESPONSE        BP_WRAP_RESEND
#endif

#ifndef BP_DEFAULT_DACS_RATE
#define BP_DEFAULT_DACS_RATE            1000    // milliseconds
#endif

#ifndef BP_DEFAULT_BP_VERSION
#define BP_DEFAULT_BP_VERSION           BP_PRI_VERSION
#endif

#define BP_EMPTY                        (-1)
#define BP_MAX_FILL                     0x3FFF
#define BP_DATA_HDR_BUF_SIZE            256
#define BP_DACS_HDR_BUF_SIZE            128
#define BP_DACS_PAY_SIZE                (8 + (2 * BP_MAX_FILLS_PER_DACS))
#define BP_NUM_EXCLUDE_REGIONS          8


/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* --------------- Storage Types ------------------- */

/* Payload Storage Block */
typedef struct {
    int                 request_custody;
    bp_blk_cteb_t       cteb;
} bp_payload_store_t;

/* Data Bundle Storage Block */
typedef struct {
    bp_time_t           retxtime;
    bp_sdnv_t           cidsdnv;
    int                 cteboffset;
    int                 biboffset;
    int                 payoffset;
    int                 headersize;
    int                 bundlesize;
    uint8_t             header[BP_DATA_HDR_BUF_SIZE];
} bp_data_store_t;

/* DTN Aggregate Custody Bundle Storage Block */
typedef struct {
    uint32_t            cstnode;
    uint32_t            cstserv;
    uint32_t            first_cid;
    uint32_t            last_cid;
    uint32_t            num_cids;
    uint32_t            fills[BP_MAX_FILLS_PER_DACS];
    int                 num_fills;
    int                 delivered;  // is it forwarded to destination or delivered to application
    int                 biboffset;
    int                 payoffset;
    int                 headersize;
    int                 bundlesize;
    uint8_t             header[BP_DACS_HDR_BUF_SIZE];
} bp_dacs_store_t;

/* --------------- Bundle Types ------------------- */

/* Data Bundle */
typedef struct {
    bp_blk_pri_t        primary_block;
    bp_blk_cteb_t       custody_block;
    bp_blk_bib_t        integrity_block;
    bp_blk_pay_t        payload_block;
    bp_data_store_t     data_storage;
    int                 data_store_handle;
    bp_payload_store_t  payload_storage;
    int                 payload_store_handle;
    int                 creation_time_sys; // 1: use system time, 0: use provided channel value
    bp_sid_t            active_table[BP_ACTIVE_TABLE_SIZE];
    uint32_t            current_custody_id;
    uint32_t            oldest_custody_id;
} bp_data_bundle_t;

/* DTN Aggregate Custody Signal Bundle */
typedef struct {
    bp_blk_pri_t        primary_block;
    bp_blk_cteb_t       custody_block;
    bp_blk_bib_t        integrity_block;
    bp_blk_pay_t        payload_block;
    bp_dacs_store_t     dacs_storage[BP_DACS_TABLE_SIZE];
    int                 dacs_store_handle;
    int                 num_entries;
    int                 rate_ms;           // number of milliseconds to wait between sending ACS bundles
} bp_dacs_bundle_t;

/* Forwarded Bundle */
typedef struct {
    bp_blk_cteb_t       custody_block;
    bp_blk_bib_t        integrity_block;
    bp_blk_pay_t        payload_block;
    bp_data_store_t     data_storage;
    /* uses data bundle for everything else */
} bp_forw_bundle_t;

/* --------------- Application Types ------------------- */

/* Channel Control Block */
typedef struct {
    int                 channel_index;
    bp_store_t          store;
    bp_data_bundle_t    data_bundle;
    bp_dacs_bundle_t    dacs_bundle;
    bp_forw_bundle_t    forw_bundle;
    int                 timeout;                // seconds, zero for infinite
    int                 bundle_maxlength;
    int                 proc_admin_only;        // process only administrative records
    int                 wrap_response;
} bp_channel_t;

/******************************************************************************
 FILE DATA
 ******************************************************************************
 * Notes:
 * 1. The block length field for every bundle block MUST be set to a positive
 *    integer.  The option to update the fields of the bundle reserves the width
 *    of the blklen field and goes back and write the value after the entire
 *    block is written.  If the blklen field was variable, the code would have
 *    to make a first pass to calculate the block length and then a second pass
 *    to use that block length - that would be too much processing.
 */

static bp_channel_t channels[BP_MAX_CHANNELS];

static const bp_blk_pri_t native_data_pri_blk = {
    .version            = BP_DEFAULT_BP_VERSION,
                            /*          Value             Index       Width   */
    .pcf                = { 0,                              1,          3 },
    .blklen             = { 0,                              4,          4 },
    .dstnode            = { 0,                              8,          4 },
    .dstserv            = { 0,                              12,         4 },
    .srcnode            = { 0,                              16,         4 },
    .srcserv            = { 0,                              20,         4 },
    .rptnode            = { 0,                              24,         4 },
    .rptserv            = { 0,                              28,         4 },
    .cstnode            = { 0,                              32,         4 },
    .cstserv            = { 0,                              36,         4 },
    .createtms          = { BP_DEFAULT_CREATE_TIME_VAL_S,   40,         4 },
    .createtmns         = { BP_DEFAULT_CREATE_TIME_VAL_NS,  44,         4 },
    .createseq          = { 0,                              48,         4 },
    .lifetime           = { BP_DEFAULT_LIFETIME,            52,         4 },
    .dictlen            = { 0,                              56,         4 },
    .fragoffset         = { 0,                              60,         4 },
    .paylen             = { 0,                              64,         4 },
    .is_admin_rec       = BP_FALSE,
    .request_custody    = BP_DEFAULT_CSTRQST,
    .allow_frag         = BP_FALSE,
    .is_frag            = BP_FALSE,
    .report_deletion    = BP_FALSE
};

static const bp_blk_pri_t native_dacs_pri_blk = {
    .version            = BP_DEFAULT_BP_VERSION,
                            /*          Value             Index       Width   */
    .pcf                = { 0,                              1,          3 },
    .blklen             = { 0,                              4,          4 },
    .dstnode            = { 0,                              8,          4 },
    .dstserv            = { 0,                              12,         4 },
    .srcnode            = { 0,                              16,         4 },
    .srcserv            = { 0,                              20,         4 },
    .rptnode            = { 0,                              24,         4 },
    .rptserv            = { 0,                              28,         4 },
    .cstnode            = { 0,                              32,         4 },
    .cstserv            = { 0,                              36,         4 },
    .createtms          = { BP_DEFAULT_CREATE_TIME_VAL_S,   40,         4 },
    .createtmns         = { BP_DEFAULT_CREATE_TIME_VAL_NS,  44,         4 },
    .createseq          = { 0,                              48,         4 },
    .lifetime           = { BP_DEFAULT_LIFETIME,            52,         4 },
    .dictlen            = { 0,                              56,         4 },
    .fragoffset         = { 0,                              0,          0 },
    .paylen             = { 0,                              0,          0 },
    .is_admin_rec       = BP_TRUE,
    .request_custody    = BP_FALSE,
    .allow_frag         = BP_FALSE,
    .is_frag            = BP_FALSE,
    .report_deletion    = BP_FALSE,
};

static const bp_blk_cteb_t native_cteb_blk = {
                            /*          Value             Index       Width   */
    .bf                 = { 0,                              1,          1 },
    .blklen             = { 0,                              2,          2 },
    .cid                = { 0,                              4,          4 },
    .cstnode            = { 0,                              8,          4 },
    .cstserv            = { 0,                              12,         4 }
};

static const bp_blk_bib_t native_bib_blk = {
                            /*          Value             Index       Width   */
    .bf                 = { 0,                              1,          1 },
    .blklen             = { 0,                              2,          2 },
    .paytype            = { BP_DEFAULT_PAY_CRC,             4,          2 },
    .paycrc             = { 0,                              6,          2 }
};

static const bp_blk_pay_t native_pay_blk = {
                            /*          Value             Index       Width   */
    .bf                 = { 0,                              1,          1 },
    .blklen             = { 0,                              2,          2 },
    .payptr             = NULL,
    .paysize            = 0
};

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * initialize_data_bundle - Initialize Data Bundle Header from Channel Struct
 *
 *  Does not populate following:
 *      - creation time (when using system time)
 *      - creation sequence
 *      - fragment offset
 *      - total payload length
 *      - custody id
 *      - payload crc
 *      - payload block length
 *-------------------------------------------------------------------------------------*/
static int initialize_data_bundle(bp_data_bundle_t* bundle)
{
    bp_data_store_t* ds     = &bundle->data_storage;
    uint8_t*         hdrbuf = ds->header;

    /* Initialize Storage */
    bplib_os_memset(ds, 0, sizeof(bp_data_store_t));
    ds->cidsdnv = native_cteb_blk.cid;

    /* Initialize Header */
    if(bundle->primary_block.request_custody)
    {
        ds->cteboffset = bplib_blk_pri_write (&hdrbuf[0],              BP_DATA_HDR_BUF_SIZE,                  &bundle->primary_block,   BP_FALSE);
        ds->biboffset  = bplib_blk_cteb_write(&hdrbuf[ds->cteboffset], BP_DATA_HDR_BUF_SIZE - ds->cteboffset, &bundle->custody_block,   BP_FALSE) + ds->cteboffset;
        ds->payoffset  = bplib_blk_bib_write (&hdrbuf[ds->biboffset],  BP_DATA_HDR_BUF_SIZE - ds->biboffset,  &bundle->integrity_block, BP_FALSE) + ds->biboffset;
        ds->headersize = bplib_blk_pay_write (&hdrbuf[ds->payoffset],  BP_DATA_HDR_BUF_SIZE - ds->payoffset,  &bundle->payload_block,   BP_FALSE) + ds->payoffset;
    }
    else
    {
        ds->biboffset  = bplib_blk_pri_write (&hdrbuf[0],              BP_DATA_HDR_BUF_SIZE,                  &bundle->primary_block,   BP_FALSE);
        ds->payoffset  = bplib_blk_bib_write (&hdrbuf[ds->biboffset],  BP_DATA_HDR_BUF_SIZE - ds->biboffset,  &bundle->integrity_block, BP_FALSE) + ds->biboffset;
        ds->headersize = bplib_blk_pay_write (&hdrbuf[ds->payoffset],  BP_DATA_HDR_BUF_SIZE - ds->payoffset,  &bundle->payload_block,   BP_FALSE) + ds->payoffset;
    }

    return ds->headersize;
}

/*--------------------------------------------------------------------------------------
 * store_data_bundle -
 *-------------------------------------------------------------------------------------*/
static int store_data_bundle(bp_channel_t* ch, bp_data_store_t* ds, bp_blk_pri_t* pri, bp_blk_bib_t* bib, bp_blk_pay_t* pay, int originate, int timeout)
{
    int status = 0;
    uint8_t flags = 0;

    if(pri->is_frag || pay->paysize <= ch->bundle_maxlength)
    {
        /* Update Originator Specific Primary Block Fields */
        if(originate)
        {
            /* Set Creation Time */
            if(ch->data_bundle.creation_time_sys)
            {
                bp_time_t tm;
                bplib_systime(&tm);
                pri->createtms.value = tm.s;
                pri->createtmns.value = tm.ns;
                bplib_sdnv_write(ds->header, BP_DATA_HDR_BUF_SIZE, pri->createtms, &flags);
                bplib_sdnv_write(ds->header, BP_DATA_HDR_BUF_SIZE, pri->createtmns, &flags);
            }

            /* Set Sequence */
            bplib_sdnv_write(ds->header, BP_DATA_HDR_BUF_SIZE, pri->createseq, &flags);
        }

        /* Enqueue Bundle */
        int payload_offset = 0;
        while(payload_offset < pay->paysize)
        {
            /* Calculate Storage Header Size and Fragment Size */
            ds->headersize = ds->payoffset + status;
            int storage_header_size = sizeof(bp_data_store_t) - (BP_DATA_HDR_BUF_SIZE - ds->headersize);
            int payload_remaining = pay->paysize - payload_offset;
            int fragment_size = ch->bundle_maxlength <  payload_remaining ? ch->bundle_maxlength : payload_remaining;
            ds->bundlesize = ds->headersize + fragment_size;

            /* Update Primary Block */
            pri->fragoffset.value = payload_offset;
            pri->paylen.value = pay->paysize;
            bplib_sdnv_write(ds->header, BP_DATA_HDR_BUF_SIZE, pri->fragoffset, &flags);
            bplib_sdnv_write(ds->header, BP_DATA_HDR_BUF_SIZE, pri->paylen, &flags);

            /* Update Integrity Block */
            bplib_blk_bib_update(&ds->header[ds->biboffset], BP_DATA_HDR_BUF_SIZE - ds->biboffset, &pay->payptr[payload_offset], fragment_size, bib);

            /* Write Payload Block (static portion) */
            pay->blklen.value = fragment_size;
            status = bplib_blk_pay_write(&ds->header[ds->payoffset], BP_DATA_HDR_BUF_SIZE - ds->payoffset, pay, BP_FALSE);
            if(status <= 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%d) to write payload block (static portion) of bundle\n", status);

////////////////////////////////////////////////////////////////////////
uint8_t* ptr = (uint8_t*)&ds->header;
printf("[%ld]: ", sizeof(bp_data_store_t));
for(unsigned int i = 0; i < sizeof(bp_data_store_t); i++) printf("%02X ", ptr[i]);
printf("\n");
////////////////////////////////////////////////////////////////////////

            /* Enqueue Bundle */
            ch->store.enqueue(ch->data_bundle.data_store_handle, ds, storage_header_size, &pay->payptr[payload_offset], fragment_size, timeout);
            payload_offset += fragment_size;
        }

        /* Increment Sequence Count */
        if(originate)
        {
            pri->createseq.value++;
        }

        /* Return Status */
        if(flags != 0)  return BP_BUNDLEPARSEERR;
        else            return payload_offset;
    }
    else
    {
        return bplog(BP_BUNDLETOOLARGE, "Bundle is not being fragmented yet the payload is too large (%d)\n", pay->paysize);
    }
}

/*--------------------------------------------------------------------------------------
 * initialize_dacs_bundle - Initialize ACS Bundle Header from Channel Struct
 *
 *  Does not populate following (see update_dacs_header):
 *      - creation time (when using system time)
 *      - creation sequence
 *      - total payload length
 *      - payload crc
 *      - payload block length
 *-------------------------------------------------------------------------------------*/
static int initialize_dacs_bundle(bp_dacs_bundle_t* bundle, bp_dacs_store_t* ds, uint32_t dstnode, uint32_t dstserv)
{
    uint8_t* hdrbuf = ds->header;
    uint8_t flags = 0;

    ds->biboffset  = bplib_blk_pri_write (&hdrbuf[0],             BP_DATA_HDR_BUF_SIZE,                 &bundle->primary_block,   BP_FALSE);
    ds->payoffset  = bplib_blk_bib_write (&hdrbuf[ds->biboffset], BP_DATA_HDR_BUF_SIZE - ds->biboffset, &bundle->integrity_block, BP_FALSE) + ds->biboffset;
    ds->headersize = bplib_blk_pay_write (&hdrbuf[ds->payoffset], BP_DATA_HDR_BUF_SIZE - ds->payoffset, &bundle->payload_block,   BP_FALSE) + ds->payoffset;

    bundle->primary_block.dstnode.value = dstnode;
    bundle->primary_block.dstserv.value = dstserv;

    bplib_sdnv_write(hdrbuf, BP_DATA_HDR_BUF_SIZE, bundle->primary_block.dstnode, &flags);
    bplib_sdnv_write(hdrbuf, BP_DATA_HDR_BUF_SIZE, bundle->primary_block.dstserv, &flags);

    if(flags != 0)  return BP_BUNDLEPARSEERR;
    else            return ds->headersize;
}

/*--------------------------------------------------------------------------------------
 * store_dacs_bundle -
 *
 *  Noets: may or may not perform enqueue
 *-------------------------------------------------------------------------------------*/
static int store_dacs_bundle(bp_channel_t* ch, bp_blk_cteb_t* cteb, int delivered, int timeout, uint32_t* dacsflags)
{
    int i;
    bp_dacs_bundle_t* bundle = &ch->dacs_bundle;
    bp_dacs_store_t* ds = NULL;
    bp_blk_pri_t* pri = &bundle->primary_block;

    /* Find ACS Table Entry */
    for(i = 0; i < bundle->num_entries; i++)
    {
        if(bundle->dacs_storage[i].cstnode == cteb->cstnode.value && bundle->dacs_storage[i].cstserv == cteb->cstserv.value)
        {
            ds = &bundle->dacs_storage[i];
            break;
        }
    }

    /* Handle Entry Not Found */
    if(ds == NULL)
    {
        if(bundle->num_entries < BP_DACS_TABLE_SIZE)
        {
            /* Populate DACS Table */
            ds = &bundle->dacs_storage[bundle->num_entries++];
            ds->cstnode = cteb->cstnode.value;
            ds->cstserv = cteb->cstserv.value;
            ds->num_cids = 0;

            /* Initial ACS Header */
            initialize_dacs_bundle(&ch->dacs_bundle, ds, cteb->cstnode.value, cteb->cstserv.value);
        }
        else
        {
            /* No Room in Table for Another Source */
            *dacsflags |= BP_FLAG_TOOMANYSOURCES;
            return bplog(BP_FAILEDRESPONSE, "No room in DACS table for another source %d.%d\n", cteb->cstnode.value, cteb->cstserv.value);
        }
    }

    /* Populate/Send ACS Bundle(s) */
    if(ds != NULL)
    {
        if(ds->num_cids == 0)
        {
            /* Start New ACS */
            ds->delivered = delivered;
            ds->first_cid = cteb->cid.value;
            ds->last_cid = cteb->cid.value;
            ds->fills[0] = 0;
            ds->num_fills = 1;
            ds->num_cids = 1;
        }
        else if(cteb->cid.value <= ds->last_cid)
        {
            /* Mark CID Going Backwards */
            *dacsflags |= BP_FLAG_CIDWENTBACKWARDS;
        }
        else if(ds->delivered != delivered)
        {
            /* Mark Mixed Response */
            *dacsflags |= BP_FLAG_MIXEDRESPONSE;
        }
        else // cid > ds->last_cid
        {
            /* Update Fill */
            uint32_t cid_delta = cteb->cid.value - ds->last_cid;
            uint32_t hop_val = cid_delta - 1;
            int fill_index = ds->num_fills - 1;

            ds->last_cid = cteb->cid.value; // save last CID

            if((fill_index + 2) < BP_MAX_FILLS_PER_DACS)
            {
                if(hop_val == 0 && ds->fills[fill_index] < BP_MAX_FILL)
                {
                    ds->fills[fill_index]++;
                }
                else if(hop_val < BP_MAX_FILL)
                {
                    ds->fills[fill_index + 1] = hop_val;
                    ds->fills[fill_index + 2]++;
                    ds->num_fills += 2;
                }
                else
                {
                    *dacsflags |= BP_FLAG_FILLOVERFLOW;
                }
            }
            else
            {
                *dacsflags |= BP_FLAG_TOOMANYFILLS;
            }
        }

        /* Store DACS */
        if(*dacsflags != 0)
        {
            static uint8_t buffer[BP_DACS_PAY_SIZE];
            int dacs_size, enstat;
            uint8_t sdnv_flags = 0;
            bp_time_t tm;

            /* Build DACS */
            dacs_size = bplib_rec_acs_write(buffer, BP_DACS_PAY_SIZE, ds->delivered, ds->first_cid, ds->fills, ds->num_fills);

            /* Set Creation Time */
            bplib_systime(&tm);
            pri->createtms.value = tm.s;
            pri->createtmns.value = tm.ns;
            bplib_sdnv_write(ds->header, BP_DATA_HDR_BUF_SIZE, pri->createtms, &sdnv_flags);
            bplib_sdnv_write(ds->header, BP_DATA_HDR_BUF_SIZE, pri->createtmns, &sdnv_flags);

            /* Set Sequence */
            bplib_sdnv_write(ds->header, BP_DATA_HDR_BUF_SIZE, pri->createseq, &sdnv_flags);
            pri->createseq.value++;

            /* Update Bundle Integrity Block */
            bplib_blk_bib_update(&ds->header[ds->biboffset], BP_DATA_HDR_BUF_SIZE - ds->biboffset, buffer, dacs_size, &bundle->integrity_block);

            /* Update Payload Block */
            bundle->payload_block.payptr = buffer;
            bundle->payload_block.paysize = dacs_size;
            bundle->payload_block.blklen.value = dacs_size;
            bplib_sdnv_write(&ds->header[ds->payoffset], BP_DATA_HDR_BUF_SIZE - ds->payoffset, bundle->payload_block.blklen, &sdnv_flags);

            /* Check Status */
            if(sdnv_flags != 0) *dacsflags |= BP_FLAG_PARSEFAILURE;

            /* Send (enqueue) ACS */
            enstat = ch->store.enqueue(ch->dacs_bundle.dacs_store_handle, ds->header, BP_DACS_HDR_BUF_SIZE, buffer, dacs_size, timeout);
            if(enstat != BP_SUCCESS) *dacsflags |= BP_FLAG_UNABLETOSTORE;

            /* Start New DTN ACS */
            ds->delivered = delivered;
            ds->first_cid = cteb->cid.value;
            ds->last_cid = cteb->cid.value;
            ds->fills[0] = 0;
            ds->num_fills = 1;
            ds->num_cids = 1;
        }
    }

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * initialize_forw_bundle -
 *-------------------------------------------------------------------------------------*/
static int initialize_forw_bundle(bp_channel_t* ch, bp_blk_pri_t* pri, bp_blk_pay_t* pay, int cteb_present, uint8_t* buffer, int* exclude, int num_excludes, uint32_t* procflags)
{
    int i, status;
    int hdr_index;
    bp_data_store_t* ds = &ch->forw_bundle.data_storage;
    bp_blk_cteb_t* fcteb = &ch->forw_bundle.custody_block;
    bp_blk_bib_t* fbib = &ch->forw_bundle.integrity_block;
    bp_blk_pay_t* fpay = &ch->forw_bundle.payload_block;

    /* Initialize Data Storage Memory */
    hdr_index = 0;
    bplib_os_memset(ds, 0, sizeof(bp_data_store_t));

    /* Check Payload Size */
    if(pay->paysize > ch->bundle_maxlength)
    {
        if(!pri->allow_frag) return bplog(BP_BUNDLETOOLARGE, "Unable (%d) to fragment forwarded bundle (%d > %d)\n", BP_UNSUPPORTED, pay->paysize, ch->bundle_maxlength);
        else                 pri->is_frag = BP_TRUE;
    }

    /* Accept Custody */
    if(pri->request_custody)
    {
        if(cteb_present == BP_FALSE)
        {
            *procflags |= BP_FLAG_NONCOMPLIANT;
            return bplog(BP_UNSUPPORTED, "Only aggregate custody supported\n");
        }

        pri->rptnode.value = ch->data_bundle.primary_block.rptnode.value;
        pri->rptserv.value = ch->data_bundle.primary_block.rptserv.value;
        pri->cstnode.value = ch->data_bundle.primary_block.cstnode.value;
        pri->cstserv.value = ch->data_bundle.primary_block.cstserv.value;
    }

    /* Write Primary Block */
    status = bplib_blk_pri_write(ds->header, BP_DATA_HDR_BUF_SIZE, pri, BP_FALSE);
    if(status <= 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%d) to write primary block of forwarded bundle\n", status);
    hdr_index += status;

    /* Write Custody Block */
    if(pri->request_custody)
    {
        fcteb->cstnode.value = pri->cstnode.value;
        fcteb->cstserv.value = pri->cstserv.value;
        ds->cidsdnv = fcteb->cid;
        ds->cteboffset = hdr_index;
        status = bplib_blk_cteb_write(&ds->header[ds->cteboffset], BP_DATA_HDR_BUF_SIZE - ds->cteboffset, fcteb, BP_FALSE);
        if(status <= 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%d) to write custody block of forwarded bundle\n", status);
        hdr_index += status;
    }

    /* Write Integrity Block */
    {
        ds->biboffset = hdr_index;
        status = bplib_blk_bib_write(&ds->header[ds->biboffset], BP_DATA_HDR_BUF_SIZE - ds->biboffset, fbib, BP_FALSE);
        if(status <= 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%d) to write integrity block of forwarded bundle\n", status);
        hdr_index += status;
    }

    /* Copy Non-excluded Header Regions */
    for(i = 1; (i + 1) < num_excludes; i+=2)
    {
        int start_index = exclude[i];
        int stop_index = exclude[i + 1];
        int bytes_to_copy = stop_index - start_index;
        if((hdr_index + bytes_to_copy) >= BP_DATA_HDR_BUF_SIZE)
        {
            return bplog(BP_BUNDLETOOLARGE, "Non-excluded forwarded blocks exceed maximum header size (%d)\n", hdr_index);
        }
        else
        {
            bplib_os_memcpy(&ds->header[hdr_index], &buffer[start_index], bytes_to_copy);
            hdr_index += bytes_to_copy;
        }
    }

    /* Initialize Payload Block (static portion) */
    {
        fpay->payptr = pay->payptr;
        fpay->paysize = pay->paysize;
        ds->payoffset = hdr_index;
    }

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * getset_opt - Get/Set utility function
 *
 *  - getset --> 0: get, 1: set
 *  - assumes parameter checking has already been performed
 *-------------------------------------------------------------------------------------*/
static int getset_opt(int c, int opt, void* val, int len, int getset)
{
    bp_channel_t* ch = &channels[c];

    /* Select and Process Option */
    switch(opt)
    {
        case BP_OPT_DSTNODE_D:
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* node = (bp_ipn_t*)val;
            if(getset)  ch->data_bundle.primary_block.dstnode.value = *node;
            else        *node = ch->data_bundle.primary_block.dstnode.value;
            bplog(BP_INFO, "Config. Destination Node %s %lu\n", getset ? "<--" : "-->", (unsigned long)*node);
            break;
        }
        case BP_OPT_DSTSERV_D:
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* service = (bp_ipn_t*)val;
            if(getset)  ch->data_bundle.primary_block.dstserv.value = *service;
            else        *service = ch->data_bundle.primary_block.dstserv.value;
            bplog(BP_INFO, "Config. Destination Service %s %lu\n", getset ? "<--" : "-->", (unsigned long)*service);
            break;
        }
        case BP_OPT_SRCNODE_D:
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* node = (bp_ipn_t*)val;
            if(getset)  ch->data_bundle.primary_block.srcnode.value = *node;
            else        *node = ch->data_bundle.primary_block.srcnode.value;
            bplog(BP_INFO, "Config. Source Node %s %lu\n", getset ? "<--" : "-->", (unsigned long)*node);
            break;
        }
        case BP_OPT_SRCSERV_D:
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* service = (bp_ipn_t*)val;
            if(getset)  ch->data_bundle.primary_block.srcserv.value = *service;
            else        *service = ch->data_bundle.primary_block.srcserv.value;
            bplog(BP_INFO, "Config. Source Service %s %lu\n", getset ? "<--" : "-->", (unsigned long)*service);
            break;
        }
        case BP_OPT_RPTNODE_D:
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* node = (bp_ipn_t*)val;
            if(getset)  ch->data_bundle.primary_block.rptnode.value = *node;
            else        *node = ch->data_bundle.primary_block.rptnode.value;
            bplog(BP_INFO, "Config. Report To Node %s %lu\n", getset ? "<--" : "-->", (unsigned long)*node);
            break;
        }
        case BP_OPT_RPTSERV_D:
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* service = (bp_ipn_t*)val;
            if(getset)  ch->data_bundle.primary_block.rptserv.value = *service;
            else        *service = ch->data_bundle.primary_block.rptserv.value;
            bplog(BP_INFO, "Config. Report To Service %s %lu\n", getset ? "<--" : "-->", (unsigned long)*service);
            break;
        }
        case BP_OPT_CSTNODE_D:
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* node = (bp_ipn_t*)val;
            if(getset)  ch->data_bundle.primary_block.cstnode.value = *node;
            else        *node = ch->data_bundle.primary_block.cstnode.value;
            bplog(BP_INFO, "Config. Custodian Node %s %lu\n", getset ? "<--" : "-->", (unsigned long)*node);
            break;
        }
        case BP_OPT_CSTSERV_D:
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* service = (bp_ipn_t*)val;
            if(getset)  ch->data_bundle.primary_block.cstserv.value = *service;
            else        *service = ch->data_bundle.primary_block.cstserv.value;
            bplog(BP_INFO, "Config. Custodian Service %s %lu\n", getset ? "<--" : "-->", (unsigned long)*service);
            break;
        }
        case BP_OPT_CREATETIMESYS_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(*enable != BP_TRUE && *enable != BP_FALSE) return BP_PARMERR;
            if(getset)  ch->data_bundle.creation_time_sys = *enable;
            else        *enable = ch->data_bundle.creation_time_sys;
            bplog(BP_INFO, "Config. Enable Creation Time System %s %d\n", getset ? "<--" : "-->", *enable);
            break;
        }
        case BP_OPT_CREATETIMEVAL_D:
        {
            if(len != sizeof(bp_time_t)) return BP_PARMERR;
            bp_time_t* create_time = (bp_time_t*)val;
            if(getset)
            {
                ch->data_bundle.primary_block.createtms.value = create_time->s;
                ch->data_bundle.primary_block.createtmns.value = create_time->ns;
            }
            else
            {
                create_time->s = ch->data_bundle.primary_block.createtms.value;
                create_time->ns = ch->data_bundle.primary_block.createtmns.value;
            }
            bplog(BP_INFO, "Config. Creation Time %s %lu.%lu\n", getset ? "<--" : "-->", (unsigned long)create_time->s, (unsigned long)create_time->ns);
            break;
        }
        case BP_OPT_SETSEQUENCE_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            uint32_t* seq = (uint32_t*)val;
            if(getset)  ch->data_bundle.primary_block.createseq.value = *seq;
            else        *seq = ch->data_bundle.primary_block.createseq.value;
            bplog(BP_INFO, "Config. Sequence %s %lu\n", getset ? "<--" : "-->", (unsigned long)*seq);
            break;
        }
        case BP_OPT_LIFETIME_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* lifetime = (int*)val;
            if(getset)  ch->data_bundle.primary_block.lifetime.value = *lifetime;
            else        *lifetime = ch->data_bundle.primary_block.lifetime.value;
            bplog(BP_INFO, "Config. Lifetime %s %d\n", getset ? "<--" : "-->", *lifetime);
            break;
        }
        case BP_OPT_CSTRQST_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(*enable != BP_TRUE && *enable != BP_FALSE) return BP_PARMERR;
            if(getset)  ch->data_bundle.primary_block.request_custody = *enable;
            else        *enable = ch->data_bundle.primary_block.request_custody;
            bplog(BP_INFO, "Config. Enable Custody Request %s %d\n", getset ? "<--" : "-->", *enable);
            break;
        }
        case BP_OPT_ALLOWFRAG_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(*enable != BP_TRUE && *enable != BP_FALSE) return BP_PARMERR;
            if(getset)
            {
                ch->data_bundle.primary_block.allow_frag = *enable;
                ch->data_bundle.primary_block.is_frag = *enable;
            }
            else
            {
                *enable = ch->data_bundle.primary_block.allow_frag;
                *enable = ch->data_bundle.primary_block.is_frag;
            }
            bplog(BP_INFO, "Config. Allow Fragmentation %s %d\n", getset ? "<--" : "-->", *enable);
            break;
        }
        case BP_OPT_PAYCRC_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* type = (int*)val;
            if(getset)  ch->data_bundle.integrity_block.paytype.value = *type;
            else        *type = ch->data_bundle.integrity_block.paytype.value;
            bplog(BP_INFO, "Config. Payload CRC Type %s %d\n", getset ? "<--" : "-->", *type);
            break;
        }
        case BP_OPT_TIMEOUT:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* timeout = (int*)val;
            if(getset)  ch->timeout = *timeout;
            else        *timeout = ch->timeout;
            bplog(BP_INFO, "Config. Timeout %s %d\n", getset ? "<--" : "-->", *timeout);
            break;
        }
        case BP_OPT_BUNDLELEN:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* maxlen = (int*)val;
            if(getset)  ch->bundle_maxlength = *maxlen;
            else        *maxlen = ch->bundle_maxlength;
            bplog(BP_INFO, "Config. Maximum Bundle Length %s %d\n", getset ? "<--" : "-->", *maxlen);
            break;
        }
        case BP_OPT_PROCADMINONLY:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(*enable != BP_TRUE && *enable != BP_FALSE) return BP_PARMERR;
            if(getset)  ch->proc_admin_only = *enable;
            else        *enable = ch->proc_admin_only;
            bplog(BP_INFO, "Config. Enable Processing Only Admin Records %s %d\n", getset ? "<--" : "-->", *enable);
            break;
        }
        case BP_OPT_WRAPRSP:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* wrap = (int*)val;
            if(*wrap != BP_WRAP_RESEND && *wrap != BP_WRAP_BLOCK && *wrap != BP_WRAP_DROP) return BP_PARMERR;
            if(getset)  ch->wrap_response = *wrap;
            else        *wrap = ch->wrap_response;
            bplog(BP_INFO, "Config. Wrap Response %s %d\n", getset ? "<--" : "-->", *wrap);
            break;
        }
        case BP_OPT_ACSRATE:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* rate = (int*)val;
            if(getset)  ch->dacs_bundle.rate_ms = *rate;
            else        *rate = ch->dacs_bundle.rate_ms;
            bplog(BP_INFO, "Config. ACS Rate %s %d\n", getset ? "<--" : "-->", *rate);
            break;
        }
        default:
        {
            /* Option Not Found */
            return bplog(BP_PARMERR, "Config. Option Not Found (%d)\n", opt);
        }
    }

    /* Re-initialize Bundle Header */
    if(getset) initialize_data_bundle(&ch->data_bundle);

    /* Option Successfully Processed */
    return BP_SUCCESS;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_init - initializes bp library
 *-------------------------------------------------------------------------------------*/
void bplib_init(void)
{
    int i;

    /* Set all channel control blocks to empty */
    for(i = 0; i < BP_MAX_CHANNELS; i++)
    {
        channels[i].channel_index = BP_EMPTY;
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_open -
 *-------------------------------------------------------------------------------------*/
int bplib_open(bp_store_t store, bp_ipn_t local_node, bp_ipn_t local_service, bp_ipn_t destination_node, bp_ipn_t destination_service)
{
    int i;

    assert(store.create);
    assert(store.destroy);
    assert(store.enqueue);
    assert(store.dequeue);
    assert(store.retrieve);
    assert(store.refresh);
    assert(store.relinquish);

    /* Find open channel slot */
    for(i = 0; i < BP_MAX_CHANNELS; i++)
    {
        if(channels[i].channel_index == BP_EMPTY)
        {
            /* Clear Channel Memory */
            bplib_os_memset(&channels[i], sizeof(channels[i]), 0);

            /* Initialize Assets */
            channels[i].channel_index                           = i;
            channels[i].store                                   = store; // structure copy

            /* Initialize Data Bundle */
            channels[i].data_bundle.data_store_handle           = channels[i].store.create();
            channels[i].data_bundle.payload_store_handle        = channels[i].store.create();
            channels[i].data_bundle.primary_block               = native_data_pri_blk;
            channels[i].data_bundle.primary_block.dstnode.value = destination_node;
            channels[i].data_bundle.primary_block.dstserv.value = destination_service;
            channels[i].data_bundle.primary_block.srcnode.value = local_node;
            channels[i].data_bundle.primary_block.srcserv.value = local_service;
            channels[i].data_bundle.primary_block.rptnode.value = local_node;
            channels[i].data_bundle.primary_block.rptserv.value = local_service;
            channels[i].data_bundle.primary_block.cstnode.value = local_node;
            channels[i].data_bundle.primary_block.cstserv.value = local_service;
            channels[i].data_bundle.custody_block               = native_cteb_blk;
            channels[i].data_bundle.custody_block.cid.value     = 0;
            channels[i].data_bundle.custody_block.cstnode.value = local_node;
            channels[i].data_bundle.custody_block.cstserv.value = local_service;
            channels[i].data_bundle.integrity_block             = native_bib_blk;
            channels[i].data_bundle.payload_block               = native_pay_blk;
            channels[i].data_bundle.creation_time_sys           = BP_DEFAULT_CREATE_TIME_SYS;
            channels[i].data_bundle.current_custody_id          = 0;
            channels[i].data_bundle.oldest_custody_id           = 0;

            /* Initialize DACS Bundle */
            channels[i].dacs_bundle.dacs_store_handle           = channels[i].store.create();
            channels[i].dacs_bundle.primary_block               = native_dacs_pri_blk;
            channels[i].dacs_bundle.primary_block.srcnode.value = local_node;
            channels[i].dacs_bundle.primary_block.srcserv.value = local_service;
            channels[i].dacs_bundle.primary_block.rptnode.value = local_node;
            channels[i].dacs_bundle.primary_block.rptserv.value = local_service;
            channels[i].dacs_bundle.primary_block.cstnode.value = local_node;
            channels[i].dacs_bundle.primary_block.cstserv.value = local_service;
            channels[i].dacs_bundle.integrity_block             = native_bib_blk;
            channels[i].dacs_bundle.payload_block               = native_pay_blk;
            channels[i].dacs_bundle.num_entries                 = 0;
            channels[i].dacs_bundle.rate_ms                     = BP_DEFAULT_DACS_RATE;

            /* Initialize Forwarded Bundle */
            channels[i].forw_bundle.custody_block               = native_cteb_blk;
            channels[i].forw_bundle.integrity_block             = native_bib_blk;
            channels[i].forw_bundle.payload_block               = native_pay_blk;

            /* Initialize Data */
            channels[i].timeout                                 = BP_DEFAULT_TIMEOUT;
            channels[i].bundle_maxlength                        = BP_DEFAULT_BUNDLE_MAXLENGTH;
            channels[i].proc_admin_only                         = BP_DEFAULT_PROC_ADMIN_ONLY;
            channels[i].wrap_response                           = BP_DEFAULT_WRAP_RESPONSE;

            /* Populate Initial Data Bundle Storage Header
             *  note only initialize data bundle and not dacs bundle
             *  since for storage the dacs bundles are initialized
             *  when custody requests arrive */
            initialize_data_bundle(&channels[i].data_bundle);

            /* Return Channel Handle */
            return i;
        }
    }

    /* Failed to find empty slot */
    return bplog(BP_CHANNELSFULL, "Cannot open channel, not enough room\n");
}

/*--------------------------------------------------------------------------------------
 * bplib_close -
 *-------------------------------------------------------------------------------------*/
void bplib_close(int channel)
{
    if(channel >= 0 && channel < BP_MAX_CHANNELS)
    {
        channels[channel].channel_index = BP_EMPTY;
        channels[channel].store.destroy(channels[channel].data_bundle.data_store_handle);
        channels[channel].store.destroy(channels[channel].data_bundle.payload_store_handle);
        channels[channel].store.destroy(channels[channel].dacs_bundle.dacs_store_handle);
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_getopt -
 *-------------------------------------------------------------------------------------*/
int bplib_getopt(int channel, int opt, void* val, int len)
{
    /* Check Parameters */
    if(channel < 0 || channel >= BP_MAX_CHANNELS)       return BP_PARMERR;
    else if(channels[channel].channel_index != channel) return BP_INVALIDCHANNEL;
    else if(val == NULL)                                return BP_PARMERR;

    /* Call Internal Function */
    return getset_opt(channel, opt, val, len, BP_FALSE);
}

/*--------------------------------------------------------------------------------------
 * bplib_setopt -
 *-------------------------------------------------------------------------------------*/
int bplib_setopt(int channel, int opt, void* val, int len)
{
     /* Check Parameters */
    if(channel < 0 || channel >= BP_MAX_CHANNELS)       return BP_PARMERR;
    else if(channels[channel].channel_index != channel) return BP_INVALIDCHANNEL;
    else if(val == NULL)                                return BP_PARMERR;

    /* Call Internal Function */
    return getset_opt(channel, opt, val, len, BP_TRUE);
}

/*--------------------------------------------------------------------------------------
 * bplib_store -
 *-------------------------------------------------------------------------------------*/
int bplib_store(int channel, void* payload, int size, int timeout)
{
    /* Check Parameters */
    if(channel < 0 || channel >= BP_MAX_CHANNELS)       return BP_PARMERR;
    else if(channels[channel].channel_index != channel) return BP_INVALIDCHANNEL;
    else if(payload == NULL)                            return BP_PARMERR;
    else if(size > channels[channel].bundle_maxlength)  return BP_PARMERR;

    /* Store Bundle */
    return store_data_bundle( &channels[channel],
                              &channels[channel].data_bundle.data_storage,
                              &channels[channel].data_bundle.primary_block,
                              &channels[channel].data_bundle.integrity_block,
                              &channels[channel].data_bundle.payload_block,
                              BP_TRUE, timeout );
}

/*--------------------------------------------------------------------------------------
 * bplib_load -
 *-------------------------------------------------------------------------------------*/
int bplib_load(int channel, void* bundle, int size, int timeout, uint32_t* loadflags)
{
    int status;                         // size of bundle returned or error code
    bp_time_t sysnow;                   // current system time
    bp_time_t sysretx;                  // system time for retransmits
    uint8_t* storebuf;                  // pointer to retrieved storage memory
    int storelen;                       // size of retrieved storage memory
    int ati;                            // active table index
    bp_sid_t sid;                       // id returned from storage of bundle to load
    uint8_t* load_bundle;               // pointer to bundle to load
    int load_size;                      // size of bundle to load
    int load_store;                     // handle for storage of bundle to load
    int load_cteboffset;                // offset of CTEB block which indicates custody requested
    bp_sdnv_t load_cidsdnv;             // sdnv of the CID that needs to be written on load
    bp_channel_t* ch;                   // channel pointer
    bp_store_dequeue_t dequeue;         // dequeue storage function
    bp_store_retrieve_t retrieve;       // retrieve storage function
    bp_store_refresh_t refresh;         // refresh storage function
    bp_store_relinquish_t relinquish;   // relinquish storage function

    /* Check Parameters */
    if(channel < 0 || channel >= BP_MAX_CHANNELS)       return BP_PARMERR;
    else if(channels[channel].channel_index != channel) return BP_INVALIDCHANNEL;
    else if(bundle == NULL)                             return BP_PARMERR;

    /* Set Short Cuts */
    ch          = &channels[channel];
    dequeue     = ch->store.dequeue;
    retrieve    = ch->store.retrieve;
    refresh     = ch->store.refresh;
    relinquish  = ch->store.relinquish;

    /* Setup State */
    bplib_systime(&sysnow);     // get current system time (to used for timeouts)
    load_bundle = NULL;         // start out assuming nothing to send
    load_cteboffset = 0;        // start out not requesting custody
    status = BP_SUCCESS;        // start out assuming operation succeeds
    sid = BP_SID_VACANT;        // storage id points to nothing
    *loadflags = 0;             // start with clean slate

    /* Check if ACS Needs to be Sent */
    if(dequeue(ch->dacs_bundle.dacs_store_handle, (void**)&storebuf, &storelen, &sid, timeout) == BP_SUCCESS)
    {
        /* Transmit ACS Bundle */
        load_bundle = storebuf;
        load_size = storelen;
        load_store = ch->dacs_bundle.dacs_store_handle;

        /* ACS Always Need to be Routed */
        *loadflags |= BP_FLAG_ROUTENEEDED;
    }
    else
    {
        // TODO: use ch->dacs_rate_ms to create ACS bundle if one is not ready
        // no need enqueue...dequeue
    }

    /* Try to Send Timed-out Bundle */
    while((load_bundle == NULL) && (ch->data_bundle.oldest_custody_id < ch->data_bundle.current_custody_id))
    {
        ati = ch->data_bundle.oldest_custody_id % BP_ACTIVE_TABLE_SIZE;
        sid = ch->data_bundle.active_table[ati];
        if(sid == BP_SID_VACANT) // entry vacant
        {
            ch->data_bundle.oldest_custody_id++;
        }
        else if(retrieve(ch->data_bundle.data_store_handle, (void**)&storebuf, &storelen, sid, timeout) == BP_SUCCESS)
        {
            /* Check Timeout */
            bp_data_store_t* ds = (bp_data_store_t*)storebuf;
            if(bplib_cmptime(sysnow, ds->retxtime) >= 0)
            {
                /* Retransmit Bundle */
                load_bundle = &ds->header[0];
                load_size = ds->bundlesize;
                load_store = ch->data_bundle.data_store_handle;
                load_cteboffset = ds->cteboffset;
                load_cidsdnv = ds->cidsdnv;
            }

            /* Exit Loop */
            break;
        }
        else // failed to retrieve bundle from storage
        {
            relinquish(ch->data_bundle.data_store_handle, sid);
            ch->data_bundle.active_table[ati] = BP_SID_VACANT;
            *loadflags |= BP_FLAG_STOREFAILURE;
            bplog(BP_FAILEDSTORE, "Failed to retrieve bundle from storage\n");
        }
    }

    /* Try to Send Stored Bundle (if nothing sent yet) */
    while(load_bundle == NULL)
    {
        ati = ch->data_bundle.current_custody_id % BP_ACTIVE_TABLE_SIZE;
        sid = ch->data_bundle.active_table[ati];
        if(sid == BP_SID_VACANT) // entry vacant
        {
            /* Dequeue Bundle from Storage Service */
            int deq_status = dequeue(ch->data_bundle.data_store_handle, (void**)&storebuf, &storelen, &sid, timeout);
            if(deq_status == BP_SUCCESS)
            {
                /* Write Re-Transmit Time */
                bp_data_store_t* ds = (bp_data_store_t*)storebuf;
                bplib_addtime(&sysretx, sysnow, ch->timeout);
                ds->retxtime = sysretx;
                refresh(ch->data_bundle.data_store_handle, storebuf, sizeof(bp_time_t), 0, sid, timeout);

                /* Transmit Dequeued Bundle */
                load_bundle = &ds->header[0];
                load_size = ds->bundlesize;
                load_store = ch->data_bundle.data_store_handle;
                load_cteboffset = ds->cteboffset;
                load_cidsdnv = ds->cidsdnv;
            }
            else if(deq_status == BP_TIMEOUT)
            {
                /* No Bundles in Storage to Send */
                status = BP_TIMEOUT;
            }
            else
            {
                /* Failed Storage Service */
                status = BP_FAILEDSTORE;
                *loadflags |= BP_FLAG_STOREFAILURE;
            }

            /* Exit Loop */
            break;
        }
        else if(ch->wrap_response == BP_WRAP_RESEND)
        {
            /* Retrieve Bundle from Storage */
            if(retrieve(ch->data_bundle.data_store_handle, (void**)&storebuf, &storelen, sid, timeout) == BP_SUCCESS)
            {
                /* Retransmit Bundle */
                bp_data_store_t* ds = (bp_data_store_t*)storebuf;
                load_bundle = &ds->header[0];
                load_size = ds->bundlesize;
                load_store = ch->data_bundle.data_store_handle;
                load_cteboffset = ds->cteboffset;
                load_cidsdnv = ds->cidsdnv;
            }
            else // failed to retrieve bundle from storage
            {
                relinquish(ch->data_bundle.data_store_handle, sid);
                ch->data_bundle.active_table[ati] = BP_SID_VACANT;
                *loadflags |= BP_FLAG_STOREFAILURE;
                bplog(BP_FAILEDSTORE, "Failed to retrieve bundle from storage\n");
            }
        }
        else if(ch->wrap_response == BP_WRAP_BLOCK)
        {
            /* Custody ID Wrapped Around to Occupied Slot */
            status = BP_OVERFLOW;

            /* Exit Loop */
            break;
        }
        else // if(ch->wrap_response == BP_WRAP_DROP)
        {
            relinquish(ch->data_bundle.data_store_handle, sid);
            ch->data_bundle.active_table[ati] = BP_SID_VACANT;
            ch->data_bundle.oldest_custody_id++;
        }
    }

    /* Check if Bundle Ready to Transmit */
    if(load_bundle != NULL)
    {
        /* Check Buffer Size */
        if(size < load_size)
        {
            status = bplog(BP_BUNDLETOOLARGE, "Bundle too large to fit inside buffer (%d %d)\n", size, load_size);;
        }
        else
        {
            /* Assign Custody ID */
            if(load_cteboffset != 0)
            {
                uint8_t flags = 0;
                ati = ch->data_bundle.current_custody_id % BP_ACTIVE_TABLE_SIZE;
                ch->data_bundle.active_table[ati] = sid;
                ch->data_bundle.custody_block.cid.value = ch->data_bundle.current_custody_id++;
                bplib_sdnv_write(&load_bundle[load_cteboffset], load_size, load_cidsdnv, &flags);
                if(flags != 0) status = bplog(BP_BUNDLEPARSEERR, "Fatal error (%0X): failed to write CID to loaded bundle\n", flags);
            }

            /* Successfully Load Bundle to Application and Relinquish Memory */
            bplib_os_memcpy(bundle, load_bundle, load_size);
            status = load_size;
        }

        /* Free Bundle Memory */
        relinquish(load_store, sid);
    }

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_process -
 *-------------------------------------------------------------------------------------*/
int bplib_process(int channel, void* bundle, int size, int timeout, uint32_t* procflags)
{
    bp_channel_t*       ch;
    int                 status;

    uint8_t*            buffer = (uint8_t*)bundle;
    int                 index = 0;

    int                 ei = 0;
    int                 exclude[BP_NUM_EXCLUDE_REGIONS];

    bp_blk_pri_t        pri_blk;

    int                 cteb_present = BP_FALSE;
    int                 cteb_index = 0;
    bp_blk_cteb_t       cteb_blk;

    int                 bib_present = BP_FALSE;
    int                 bib_index = 0;
    bp_blk_bib_t        bib_blk;

    int                 pay_index = 0;
    bp_blk_pay_t        pay_blk;

    bp_time_t           sysnow;
    bp_time_t           blktime;
    bp_time_t           expiretime;

    /* Check Parameters */
    if(channel < 0 || channel >= BP_MAX_CHANNELS)       return BP_PARMERR;
    else if(channels[channel].channel_index != channel) return BP_INVALIDCHANNEL;
    else if(bundle == NULL)                             return BP_PARMERR;

    /* Set Shortcuts */
    ch = &channels[channel];

    /* Setup Bundle Parsing */
    *procflags = 0;

    /* Parse Primary Block */
    exclude[ei++] = index;
    status = bplib_blk_pri_read(buffer, size, &pri_blk, BP_TRUE);
    if(status <= 0) return status;
    else index += status;
    exclude[ei++] = index;

    /* Set Processing Flag for Reporting Deletion */
    if(pri_blk.report_deletion) *procflags |= BP_FLAG_REPORTDELETE;

    /* Check Administrative Record Status */
    if(ch->proc_admin_only && pri_blk.is_admin_rec == BP_FALSE)
    {
        return bplog(BP_IGNORE, "Non-administrative bundle ignored\n");
    }

    /* Check Unsupported */
    if(pri_blk.dictlen.value != 0)
    {
        return bplog(BP_UNSUPPORTED, "Unsupported bundle attempted to be processed (%d)\n", pri_blk.dictlen.value);
    }

    /* Check Life Time */
    bplib_systime(&sysnow);
    blktime.s = pri_blk.createtms.value;
    blktime.ns = pri_blk.createtmns.value;
    bplib_addtime(&expiretime, blktime, pri_blk.lifetime.value);
    if(bplib_cmptime(sysnow, expiretime) >= 0)
    {
        return bplog(BP_EXPIRED, "Expired bundled attempted to be processed \n");
    }

    /* Parse and Process Remaining Blocks */
    while(index < size)
    {
        /* Read Block Information */
        uint8_t blk_type = buffer[index];

        /* Check Block Type */
        if(blk_type == BP_CTEB_BLK_TYPE)
        {
            cteb_present = BP_TRUE;
            cteb_index = index;
            if(pri_blk.request_custody) exclude[ei++] = index;
            status = bplib_blk_cteb_read(&buffer[cteb_index], size - cteb_index, &cteb_blk, BP_TRUE);
            if(status <= 0) return status;
            else            index += status;
            if(pri_blk.request_custody) exclude[ei++] = index;
        }
        else if(blk_type == BP_BIB_BLK_TYPE)
        {
            bib_present = BP_TRUE;
            bib_index = index;
            exclude[ei++] = index;
            status = bplib_blk_bib_read(&buffer[bib_index], size - bib_index, &bib_blk, BP_TRUE);
            if(status <= 0) return status;
            else            index += status;
            exclude[ei++] = index;
        }
        else if(blk_type != BP_PAY_BLK_TYPE) // skip over block
        {
            uint8_t flags = 0;
            bp_sdnv_t blk_flags = { 0, 1, 0 };
            bp_sdnv_t blk_len = { 0, 0, 0 };
            int start_index = index;
            blk_len.index = bplib_sdnv_read(&buffer[start_index], size - start_index, &blk_flags, &flags);
            index = bplib_sdnv_read(&buffer[start_index], size - start_index, &blk_len, &flags);

            /* Check Parsing Status */
            if(flags != 0) return bplog(BP_BUNDLEPARSEERR, "Failed (%0X) to parse block at index %d\n", flags, start_index);

            /* Mark Processing as Incomplete */
            *procflags |= BP_FLAG_INCOMPLETE;
            bplog(BP_UNSUPPORTED, "Skipping over unrecognized block\n");

            /* Should transmit status report that block cannot be processed */
            if(blk_flags.value & BP_BLK_NOTIFYNOPROC_MASK) *procflags |= BP_FLAG_NONCOMPLIANT;

            /* Delete bundle since block not recognized */
            if(blk_flags.value & BP_BLK_DELETENOPROC_MASK) return bplog(BP_DROPPED, "Dropping bundle with unrecognized block\n");

            /* Should drop block since it cannot be processed */
            if(blk_flags.value & BP_BLK_DROPNOPROC_MASK) *procflags |= BP_FLAG_NONCOMPLIANT;

            /* Mark As Forwarded without Processed */
            blk_flags.value |= BP_BLK_FORWARDNOPROC_MASK;
            bplib_sdnv_write(&buffer[start_index], size - start_index, blk_flags, &flags);
        }
        else // payload block
        {
            pay_index = index;
            exclude[ei++] = index; // start of payload header
            status = bplib_blk_pay_read(&buffer[pay_index], size - pay_index, &pay_blk, BP_TRUE);
            if(status <= 0) return bplog(status, "Failed (%d) to read payload block\n", status);
            else            index += status;
            exclude[ei++] = index + pay_blk.paysize;

            /* Perform Integrity Check */
            if(bib_present)
            {
                status = bplib_blk_bib_verify(pay_blk.payptr, pay_blk.paysize, &bib_blk);
                if(status <= 0) return bplog(status, "Bundle failed integrity check\n");
            }

            /* Process Payload */
            if(pri_blk.is_admin_rec) // Administrative Record
            {
                uint32_t rec_type, rec_status;

                /* Check Record Length */
                if(pay_blk.paysize < 2) return bplog(BP_BUNDLEPARSEERR, "Invalid block length: %d\n", pay_blk.paysize);

                /* Read Record Information */
                rec_type = buffer[index];
                rec_status = buffer[index + 1]; (void)rec_status; // currently not used

                /* Process Record */
                if(rec_type == BP_ACS_REC_TYPE)
                {
                    status = bplib_rec_acs_process( &buffer[index], size - index,
                                                    ch->data_bundle.active_table, BP_ACTIVE_TABLE_SIZE,
                                                    ch->store.relinquish, ch->data_bundle.data_store_handle);
                }
                else if(rec_type == BP_CS_REC_TYPE)     return bplog(BP_UNSUPPORTED, "Custody signal bundles are not supported\n");
                else if(rec_type == BP_STAT_REC_TYPE)   return bplog(BP_UNSUPPORTED, "Status report bundles are not supported\n");
                else                                    return bplog(BP_UNKNOWNREC, "Unknown administrative record: %u\n", (unsigned int)rec_type);
            }
            else if(pri_blk.dstnode.value != ch->data_bundle.primary_block.srcnode.value) // forward bundle (dst node != src node)
            {
                /* Initialize Forwarded Bundle */
                status = initialize_forw_bundle(ch, &pri_blk, &pay_blk, cteb_present, buffer, exclude, ei, procflags);
                if(status <= 0) return status;

                /* Store Forwarded Bundle
                 *  note that the primary block is the only block reused from the
                 *  oritinal bundle, all other blocks are generated above in the
                 *  initialize_forw_bundle function */
                status = store_data_bundle(ch, &ch->forw_bundle.data_storage, &pri_blk, &ch->forw_bundle.integrity_block, &ch->forw_bundle.payload_block, BP_FALSE, timeout);
                if(status <= 0) return status;

                /* Update DACS (bundle successfully forwarded) */
                status = store_dacs_bundle(ch, &cteb_blk, BP_FALSE, timeout, procflags);
                if(status <= 0) return status;
            }
            else if(pri_blk.dstserv.value == ch->data_bundle.primary_block.srcserv.value) // deliver bundle payload to application
            {
                bp_payload_store_t* pay = &ch->data_bundle.payload_storage;

                /* Set Custody Transfer Request */
                pay->request_custody = BP_FALSE;
                if(pri_blk.request_custody)
                {
                    if(cteb_present)
                    {
                        pay->request_custody = BP_TRUE;
                        pay->cteb = cteb_blk;
                    }
                    else
                    {
                        *procflags |= BP_FLAG_NONCOMPLIANT;
                        return bplog(BP_UNSUPPORTED, "Only aggregate custody supported\n");
                    }
                }

                /* Enqueue Payload into Storage */
                status = ch->store.enqueue(ch->data_bundle.payload_store_handle, pay, sizeof(bp_payload_store_t), &buffer[index], size - index, timeout);
                if(status <= 0) return bplog(BP_FAILEDSTORE, "Failed (%d) to store payload\n", status);
            }
            else // wrong channel
            {
                return bplog(BP_WRONGCHANNEL, "Wrong channel to service bundle (%lu, %lu)\n", (unsigned long)pri_blk.dstserv.value, (unsigned long)ch->data_bundle.primary_block.srcserv.value);
            }
        }
    }

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_accept -
 *
 *  Returns number of bytes of payload copied (positive), or error code (zero, negative)
 *-------------------------------------------------------------------------------------*/
int bplib_accept(int channel, void* payload, int size, int timeout, uint32_t* acptflags)
{
    bp_channel_t*           ch;
    bp_store_dequeue_t      dequeue;
    bp_store_relinquish_t   relinquish;
    bp_payload_store_t*     payptr;
    int                     paylen;
    uint8_t*                storebuf;
    int                     storelen;
    bp_sid_t                sid;
    int                     status;

    /* Check Parameters */
    if(channel < 0 || channel >= BP_MAX_CHANNELS)       return BP_PARMERR;
    else if(channels[channel].channel_index != channel) return BP_INVALIDCHANNEL;
    else if(payload == NULL)                            return BP_PARMERR;

    /* Set Acceptance Flags */
    *acptflags = 0;

    /* Set Shortcuts */
    ch = &channels[channel];
    dequeue = ch->store.dequeue;
    relinquish = ch->store.relinquish;

    /* Dequeue Payload from Storage */
    status = dequeue(ch->data_bundle.payload_store_handle, (void**)&storebuf, &storelen, &sid, timeout);
    if(status != BP_SUCCESS) return status;
    else if(storelen < (int)sizeof(bp_payload_store_t))
    {
        return bplog(BP_FAILEDSTORE, "Payload retrieved from storage is too small: %d\n", storelen);
    }

    /* Access Payload */
    payptr = (bp_payload_store_t*)storebuf;
    paylen = storelen - sizeof(bp_payload_store_t);

    /* Copy Payload */
    if(size < paylen)
    {
        status = bplog(BP_PAYLOADTOOLARGE, "Payload too large to fit inside buffer (%d %d)\n", size, paylen);
    }
    else
    {
        /* Successfully Return Payload to Application and Relinquish Memory */
        bplib_os_memcpy(payload, &storebuf[sizeof(bp_payload_store_t)], paylen);
        relinquish(ch->data_bundle.payload_store_handle, sid);
        status = BP_SUCCESS;
    }

    /* Acknowledge Custody */
    if(payptr->request_custody && (status > 0))
    {
        status = store_dacs_bundle(ch, &payptr->cteb, BP_TRUE, timeout, acptflags);
    }

    /* Return Status or Payload Size */
    if(status <= 0) return status;
    else            return paylen;
}


/*--------------------------------------------------------------------------------------
 * bplib_addtime -
 *
 *  bundle -                pointer to a bundle (byte array) [INPUT]
 *  size -                  size of bundle being pointed to [INPUT]
 *  destination_node -      read from bundle [OUTPUT]
 *  destination_service -   as read from bundle [OUTPUT]
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_routeinfo(void* bundle, int size, bp_ipn_t* destination_node, bp_ipn_t* destination_service)
{
    int status;
    bp_blk_pri_t pri_blk;
    uint8_t* buffer = (uint8_t*)bundle;

    /* Check Parameters */
    assert(buffer);

    /* Parse Primary Block */
    status = bplib_blk_pri_read(buffer, size, &pri_blk, BP_FALSE);
    if(status <= 0) return status;

    /* Set Addresses */
    if(destination_node)    *destination_node = (bp_ipn_t)pri_blk.dstnode.value;
    if(destination_service) *destination_service = (bp_ipn_t)pri_blk.dstserv.value;

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_addtime -
 *
 *  Returns:    0 indicates success
 *             -1 indicates failure
 *-------------------------------------------------------------------------------------*/
int bplib_addtime (bp_time_t* result, bp_time_t tm, int sec)
{
    if(result == NULL) return -1;
    result->s = tm.s + sec;
    result->ns = tm.ns;
    return 0;
}

/*--------------------------------------------------------------------------------------
 * bplib_cmptime -
 *
 *  Notes:      The format of BP time precludes the nanosecond field from being larger
 *              than one second, therefore the logic below makes that assumption
 *
 *  Returns:    0 indicates equivalence
 *              1 indicates tm1 > tm2
 *             -1 indicates tm1 < tm2
 *-------------------------------------------------------------------------------------*/
int bplib_cmptime (bp_time_t tm1, bp_time_t tm2)
{
    if(tm1.s > tm2.s)
    {
        return 1;
    }
    else if(tm1.s < tm2.s)
    {
        return -1;
    }
    else if(tm1.ns > tm2.ns)
    {
        return 1;
    }
    else if(tm1.ns < tm2.ns)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}
