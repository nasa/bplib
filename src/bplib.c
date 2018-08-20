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

/* Application Definitions */
#define BP_EMPTY                        (-1)
#define BP_MAX_FILL                     0x3FFF

/* Data Bundle Header Definitions */
#define BP_DATA_HDR_SIZE                76
#define BP_DATA_HDR_CTEB_BLK_INDEX      52
#define BP_DATA_HDR_BIB_BLK_INDEX       64
#define BP_DATA_HDR_PAY_BLK_INDEX       72
#define BP_DATA_PROLOG_SIZE             8

/* ACS Bundle Header Definitions */
#define BP_ACS_HDR_SIZE                 64
#define BP_ACS_HDR_BIB_BLK_INDEX        52
#define BP_ACS_HDR_PAY_BLK_INDEX        60

/* Wrap Responses */
#define BP_WRAP_RESEND                  0
#define BP_WRAP_BLOCK                   1
#define BP_WRAP_DROP                    2

/* Configurations */
#ifndef BP_ACTIVE_TABLE_SIZE
#define BP_ACTIVE_TABLE_SIZE            16384
#endif

#ifndef BP_ACS_TABLE_SIZE  
#define BP_ACS_TABLE_SIZE               4   // maximum number of custody eids to keep track of per ACS report period
#endif

#ifndef BP_MAX_AGENTS
#define BP_MAX_AGENTS                   2
#endif

#ifndef BP_MAX_CHANNELS
#define BP_MAX_CHANNELS                 4
#endif

#ifndef BP_MAX_FILLS_PER_ACS
#define BP_MAX_FILLS_PER_ACS            64
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

#ifndef BP_DEFAULT_FRAGMENT_MAXLENGTH
#define BP_DEFAULT_FRAGMENT_MAXLENGTH   4096
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

#ifndef BP_DEFAULT_ACS_RATE
#define BP_DEFAULT_ACS_RATE             1000    // milliseconds
#endif

#ifndef BP_DEFAULT_BP_VERSION
#define BP_DEFAULT_BP_VERSION           BP_PRI_VERSION
#endif

/* ACS Payload Size */
#define BP_ACS_PAY_SIZE (8 + (2 * BP_MAX_FILLS_PER_ACS))


/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Payload Storage Block */
typedef struct {
    int             cstrqst;
    uint32_t        cstnode;
    uint32_t        cstserv;
    uint32_t        cid;
} bp_payload_t;

/* Data Bundle Storage Block */
typedef struct {
    union {
        bp_time_t   retxtime;                   // re-transmit time
        uint8_t     bytes[BP_DATA_PROLOG_SIZE];
    } prolog;
    uint8_t         header[BP_DATA_HDR_SIZE];   // memory for bundle header
} bp_data_t;

/* Aggregate Custody Bundle Block */
typedef struct {
    uint32_t        cstnode;
    uint32_t        cstserv;
    uint32_t        first_cid;
    uint32_t        last_cid;
    uint32_t        num_cids;
    uint32_t        fills[BP_MAX_FILLS_PER_ACS];
    int             num_fills;
    uint8_t         header[BP_ACS_HDR_SIZE];
} bp_acs_t;

/* Agent Control Block */
typedef struct {
    int             agent_index;
    bp_store_t      store;
    bp_ipn_t        local_node;
} bp_agent_t;
    
/* Channel Control Block */
typedef struct {
    /* Identifiers */
    int             channel_index;
    int             agent_index;
    int             store_bundle;
    int             store_payload;
    int             store_acs;
    /* Settings */
    bp_blk_pri_t    primary_data_block;
    bp_blk_pri_t    primary_acs_block;
    bp_blk_cteb_t   custody_block;
    bp_blk_bib_t    integrity_block;
    int             timeout;                // seconds, zero for infinite
    int             bundle_maxlength;
    int             fragment_maxlength;    
    int             proc_admin_only;        // process only administrative records
    int             wrap_response;
    int             acs_rate_ms;            // number of milliseconds to wait between sending ACS bundles
    /* Data */
    bp_sid_t        active_table[BP_ACTIVE_TABLE_SIZE];
    uint32_t        current_custody_id;
    uint32_t        oldest_custody_id;
    bp_acs_t        acs_table[BP_ACS_TABLE_SIZE];
    int             num_acs_entries;
    bp_data_t       data_bundle;
} bp_channel_t;

/******************************************************************************
 FILE DATA
 ******************************************************************************/

static bp_agent_t   agents[BP_MAX_AGENTS];
static bp_channel_t channels[BP_MAX_CHANNELS];

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * initialize_data_header - Initialize Data Bundle Header from Channel Struct
 * 
 *  Does not populate following (see update_data_header):
 *      - creation time (when using system time)
 *      - creation sequence
 *      - fragment offset
 *      - total payload length
 *      - custody id
 *      - payload crc
 *      - payload block length
 *-------------------------------------------------------------------------------------*/
static void initialize_data_header(int c)
{
    uint8_t* hdrbuf = channels[c].data_bundle.header;

    /* Primary Block */
    bplib_blk_pri_write(hdrbuf, BP_PRI_BLK_LENGTH, &channels[c].primary_data_block);
    
    /* Custody Transfer Extension Block */
    bplib_blk_cteb_write(&hdrbuf[BP_DATA_HDR_CTEB_BLK_INDEX], BP_CTEB_BLK_LENGTH, &channels[c].custody_block);
 
    /* Bundle Integrity Block */
    bplib_blk_bib_write(&hdrbuf[BP_DATA_HDR_BIB_BLK_INDEX], BP_BIB_BLK_LENGTH, &channels[c].integrity_block);

    /* Payload Block */
    bplib_blk_pay_write(&hdrbuf[BP_DATA_HDR_PAY_BLK_INDEX], BP_DATA_HDR_SIZE - BP_DATA_HDR_PAY_BLK_INDEX);
}

/*--------------------------------------------------------------------------------------
 * update_data_header - Update Data Bundle Header using Payload
 * 
 *  updates fields in the data_bundle header that are specific to the payload
 *-------------------------------------------------------------------------------------*/
static int update_data_header(int c, void* fragment_payload, int fragment_payload_size, int fragment_payload_offset, int total_payload_size)
{
    uint8_t* hdrbuf = channels[c].data_bundle.header;

    /* Set Fragmentation */
    if(fragment_payload_size != total_payload_size)
    {
        bplib_blk_pri_setfrag(hdrbuf, fragment_payload_offset);
    }

    /* Set Creation Time */
    if(channels[c].primary_data_block.creation_time_sys)
    {
        bp_time_t tm;
        bplib_systime(&tm);
        bplib_blk_pri_settime(hdrbuf, tm);
    }

    /* Set Sequence */
    bplib_blk_pri_setseq(hdrbuf, channels[c].primary_data_block.sequence);

    /* Set Total Payload Length */
    bplib_blk_pri_setpaylen(hdrbuf, total_payload_size);

    /* Update Bundle Integrity Block */
    bplib_blk_bib_update(&hdrbuf[BP_DATA_HDR_BIB_BLK_INDEX], BP_BIB_BLK_LENGTH, fragment_payload, fragment_payload_size, &channels[c].integrity_block);
    
    /* Update Payload Block */
    bplib_blk_pay_update(&hdrbuf[BP_DATA_HDR_PAY_BLK_INDEX], 4, fragment_payload_size);
    
    /* Return Success */
    return BP_SUCCESS;
}    

/*--------------------------------------------------------------------------------------
 * initialize_acs_header - Initialize ACS Bundle Header from Channel Struct
 * 
 *  Does not populate following (see update_acs_header):
 *      - creation time (when using system time)
 *      - creation sequence
 *      - total payload length
 *      - payload crc
 *      - payload block length
 *-------------------------------------------------------------------------------------*/
static void initialize_acs_header(bp_channel_t* ch, bp_acs_t* acs, uint32_t dstnode, uint32_t dstserv)
{
    uint8_t* hdrbuf = acs->header;

    /* Primary Block */
    bplib_blk_pri_write(hdrbuf, BP_PRI_BLK_LENGTH, &ch->primary_acs_block);
 
    /* Bundle Integrity Block */
    bplib_blk_bib_write(&hdrbuf[BP_ACS_HDR_BIB_BLK_INDEX], BP_BIB_BLK_LENGTH, &ch->integrity_block);

    /* Payload Block */
    bplib_blk_pay_write(&hdrbuf[BP_ACS_HDR_PAY_BLK_INDEX], BP_ACS_HDR_SIZE - BP_ACS_HDR_PAY_BLK_INDEX);

    /* Set Destination */
    bplib_blk_pri_setdst(hdrbuf, dstnode, dstserv);
}

/*--------------------------------------------------------------------------------------
 * update_acs_header - Update ACS Bundle Header using Payload
 * 
 *  updates fields in the acs bundle header that are specific to the payload
 *-------------------------------------------------------------------------------------*/
static int update_acs_header(bp_channel_t* ch, bp_acs_t* acs, void* payload, int payload_size)
{
    uint8_t* hdrbuf = acs->header;

    /* Set Creation Time */
    if(ch->primary_acs_block.creation_time_sys)
    {
        bp_time_t tm;
        bplib_systime(&tm);
        bplib_blk_pri_settime(hdrbuf, tm);
    }

    /* Set Sequence */
    bplib_blk_pri_setseq(hdrbuf, ch->primary_acs_block.sequence);

    /* Set Total Payload Length */
    bplib_blk_pri_setpaylen(hdrbuf, payload_size);

    /* Update Bundle Integrity Block */
    bplib_blk_bib_update(&hdrbuf[BP_ACS_HDR_BIB_BLK_INDEX], BP_BIB_BLK_LENGTH, payload, payload_size, &ch->integrity_block);
    
    /* Update Payload Block */
    bplib_blk_pay_update(&hdrbuf[BP_ACS_HDR_PAY_BLK_INDEX], 4, payload_size);
    
    /* Return Success */
    return BP_SUCCESS;
}    

/*--------------------------------------------------------------------------------------
 * initialize_acs_payload
 *-------------------------------------------------------------------------------------*/
static void initialize_acs_payload(bp_acs_t* acs, uint32_t cid)
{
    acs->first_cid = cid;
    acs->last_cid = cid;
    acs->fills[0] = 0;
    acs->num_fills = 1;
    acs->num_cids = 1;        
}

/*--------------------------------------------------------------------------------------
 * update_acs_payload
 *-------------------------------------------------------------------------------------*/
static uint32_t update_acs_payload(bp_channel_t* ch, uint32_t cid, uint32_t cstnode, uint32_t cstserv, int delivered, int timeout)
{
    static uint8_t buffer[BP_ACS_PAY_SIZE];
    int i, acs_entry;
    uint32_t flags = 0;

    /* Find ACS Table Entry */
    acs_entry = BP_ACS_TABLE_SIZE; // no entry found
    for(i = 0; i < ch->num_acs_entries; i++)
    {
        if(ch->acs_table[i].cstnode == cstnode && ch->acs_table[i].cstserv == cstserv)
        {
            acs_entry = i;
            break;
        }
    }

    /* Handle Entry Not Found */
    if(acs_entry == BP_ACS_TABLE_SIZE)
    {
        if(ch->num_acs_entries < BP_ACS_TABLE_SIZE)
        {
            /* Populate ACS Table */
            ch->acs_table[ch->num_acs_entries].cstnode = cstnode;
            ch->acs_table[ch->num_acs_entries].cstserv = cstserv;
            ch->acs_table[ch->num_acs_entries].num_cids = 0;
            acs_entry = ch->num_acs_entries++;

            /* Initial ACS Header */            
            initialize_acs_header(ch, &ch->acs_table[acs_entry], cstnode, cstserv);
        }
        else
        {
            /* No Room in Table for Another Source */
            flags |= BP_ACPT_TOOMANYSOURCES; 
        }
    }

    /* Populate/Send ACS Bundle(s) */
    if(acs_entry != BP_ACS_TABLE_SIZE)
    {
        bp_acs_t* acs = &ch->acs_table[acs_entry];
        if(acs->num_cids == 0)
        {
            /* Start New ACS */
            initialize_acs_payload(acs, cid);
        }
        else if(cid <= acs->last_cid)
        {
            /* Mark CID Going Backwards */
            flags |= BP_ACPT_CIDWENTBACKWARDS;
        }
        else // cid > acs->last_cid
        {                
            /* Update Fill */
            uint32_t cid_delta = cid - acs->last_cid;
            uint32_t hop_val = cid_delta - 1;
            int fill_index = acs->num_fills - 1;

            acs->last_cid = cid; // save last CID

            if((fill_index + 2) < BP_MAX_FILLS_PER_ACS)
            {
                if(hop_val == 0 && acs->fills[fill_index] < BP_MAX_FILL)
                {
                    acs->fills[fill_index]++;
                }
                else if(hop_val < BP_MAX_FILL)
                {
                    acs->fills[fill_index + 1] = hop_val;
                    acs->fills[fill_index + 2]++;
                    acs->num_fills += 2;
                }
                else
                {
                    flags |= BP_ACPT_FILLOVERFLOW;
                }
            }
            else
            {
                flags |= BP_ACPT_TOOMANYFILLS;
            }
        }

        /* Check Flags */
        if(flags != 0)
        {            
            bp_store_enqueue_t enqueue = agents[ch->agent_index].store.enqueue;
            int acs_size, upstat, enstat;
            
            /* Build ACS */
            acs_size = bplib_rec_acs_write(buffer, BP_ACS_PAY_SIZE, delivered, acs->first_cid, acs->fills, acs->num_fills);
            upstat = update_acs_header(ch, acs, buffer, acs_size);            
            if(upstat != BP_SUCCESS) flags |= BP_ACPT_INCOMPLETE;

            /* Send (enqueue) ACS */
            enstat = enqueue(ch->store_acs, acs->header, BP_ACS_HDR_SIZE, buffer, acs_size, timeout);
            if(enstat != BP_SUCCESS) flags |= BP_ACPT_UNABLETOSTORE;

            /* Start New ACS */
            initialize_acs_payload(acs, cid);
        }                
    }
    
    /* Return Flags */
    return flags;
}

/*--------------------------------------------------------------------------------------
 * getset_opt - Get/Set utility function
 *  
 *  - getset --> 0: get, 1: set
 *  - assumes parameter checking has already been performed
 *-------------------------------------------------------------------------------------*/
static int getset_opt(int c, int opt, void* val, int len, int getset)
{
    /* Select and Process Option */
    switch(opt)
    {
        case BP_OPT_DSTNODE_D:       
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* node = (bp_ipn_t*)val;
            if(getset)  channels[c].primary_data_block.dstnode = *node;
            else        *node = channels[c].primary_data_block.dstnode;
            bplog(BP_INFO, "Config. Destination Node %s %lu\n", getset ? "<--" : "-->", (unsigned long)*node);
            break;
        }
        case BP_OPT_DSTSERV_D:       
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* service = (bp_ipn_t*)val;
            if(getset)  channels[c].primary_data_block.dstserv = *service;
            else        *service = channels[c].primary_data_block.dstserv;
            bplog(BP_INFO, "Config. Destination Service %s %lu\n", getset ? "<--" : "-->", (unsigned long)*service);
            break;
        }
        case BP_OPT_SRCNODE_D:        
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* node = (bp_ipn_t*)val;
            if(getset)  channels[c].primary_data_block.srcnode = *node;
            else        *node = channels[c].primary_data_block.srcnode;
            bplog(BP_INFO, "Config. Source Node %s %lu\n", getset ? "<--" : "-->", (unsigned long)*node);
            break;
        }
        case BP_OPT_SRCSERV_D:        
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* service = (bp_ipn_t*)val;
            if(getset)  channels[c].primary_data_block.srcserv = *service;
            else        *service = channels[c].primary_data_block.srcserv;
            bplog(BP_INFO, "Config. Source Service %s %lu\n", getset ? "<--" : "-->", (unsigned long)*service);
            break;
        }
        case BP_OPT_RPTNODE_D:      
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* node = (bp_ipn_t*)val;
            if(getset)  channels[c].primary_data_block.rptnode = *node;
            else        *node = channels[c].primary_data_block.rptnode;
            bplog(BP_INFO, "Config. Report To Node %s %lu\n", getset ? "<--" : "-->", (unsigned long)*node);
            break;
        }
        case BP_OPT_RPTSERV_D:     
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* service = (bp_ipn_t*)val;
            if(getset)  channels[c].primary_data_block.rptserv = *service;
            else        *service = channels[c].primary_data_block.rptserv;
            bplog(BP_INFO, "Config. Report To Service %s %lu\n", getset ? "<--" : "-->", (unsigned long)*service);
            break;
        }
        case BP_OPT_CSTNODE_D:       
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* node = (bp_ipn_t*)val;
            if(getset)  channels[c].primary_data_block.cstnode = *node;
            else        *node = channels[c].primary_data_block.cstnode;
            bplog(BP_INFO, "Config. Custodian Node %s %lu\n", getset ? "<--" : "-->", (unsigned long)*node);
            break;
        }
        case BP_OPT_CSTSERV_D:        
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* service = (bp_ipn_t*)val;
            if(getset)  channels[c].primary_data_block.cstserv = *service;
            else        *service = channels[c].primary_data_block.cstserv;
            bplog(BP_INFO, "Config. Custodian Service %s %lu\n", getset ? "<--" : "-->", (unsigned long)*service);
            break;
        }
        case BP_OPT_TIMEOUT_D:        
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* timeout = (int*)val;
            if(getset)  channels[c].timeout = *timeout;
            else        *timeout = channels[c].timeout;
            bplog(BP_INFO, "Config. Timeout %s %d\n", getset ? "<--" : "-->", *timeout);
            break;
        }
        case BP_OPT_CREATETIMESYS_D:     
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(*enable != BP_TRUE && *enable != BP_FALSE) return BP_PARMERR;
            if(getset)  channels[c].primary_data_block.creation_time_sys = *enable;
            else        *enable = channels[c].primary_data_block.creation_time_sys;
            bplog(BP_INFO, "Config. Enable Creation Time System %s %d\n", getset ? "<--" : "-->", *enable);
            break;
        }
        case BP_OPT_CREATETIMEVAL_D:     
        {
            if(len != sizeof(bp_time_t)) return BP_PARMERR;
            bp_time_t* create_time = (bp_time_t*)val;
            if(getset)  channels[c].primary_data_block.createtime = *create_time;
            else        *create_time = channels[c].primary_data_block.createtime;
            bplog(BP_INFO, "Config. Creation Time %s %lu.%lu\n", getset ? "<--" : "-->", (unsigned long)create_time->s, (unsigned long)create_time->ns);
            break;
        }
        case BP_OPT_LIFETIME_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* lifetime = (int*)val;
            if(getset)  channels[c].primary_data_block.lifetime = *lifetime;
            else        *lifetime = channels[c].primary_data_block.lifetime;
            bplog(BP_INFO, "Config. Lifetime %s %d\n", getset ? "<--" : "-->", *lifetime);
            break;
        }
        case BP_OPT_BUNDLELEN_D:      
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* maxlen = (int*)val;
            if(getset)  channels[c].bundle_maxlength = *maxlen;
            else        *maxlen = channels[c].bundle_maxlength;
            bplog(BP_INFO, "Config. Maximum Bundle Length %s %d\n", getset ? "<--" : "-->", *maxlen);
            break;
        }
        case BP_OPT_FRAGMENTLEN_D:    
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* maxlen = (int*)val;
            if(getset)  channels[c].fragment_maxlength = *maxlen;
            else        *maxlen = channels[c].fragment_maxlength;
            bplog(BP_INFO, "Config. Maximum Fragment Length %s %d\n", getset ? "<--" : "-->", *maxlen);
            break;
        }
        case BP_OPT_SETSEQUENCE_D:       
        {
            if(len != sizeof(int)) return BP_PARMERR;
            uint32_t* seq = (uint32_t*)val;
            if(getset)  channels[c].primary_data_block.sequence = *seq;
            else        *seq = channels[c].primary_data_block.sequence;
            bplog(BP_INFO, "Config. Sequence %s %lu\n", getset ? "<--" : "-->", (unsigned long)*seq);
            break;
        }
        case BP_OPT_CSTRQST_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(*enable != BP_TRUE && *enable != BP_FALSE) return BP_PARMERR;
            if(getset)  channels[c].primary_data_block.request_custody = *enable;
            else        *enable = channels[c].primary_data_block.request_custody;
            bplog(BP_INFO, "Config. Enable Custody Request %s %d\n", getset ? "<--" : "-->", *enable);
            break;
        }
        case BP_OPT_SRCNODE_A:        
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* node = (bp_ipn_t*)val;
            if(getset)  channels[c].primary_acs_block.srcnode = *node;
            else        *node = channels[c].primary_acs_block.srcnode;
            bplog(BP_INFO, "Config. Source Node %s %lu\n", getset ? "<--" : "-->", (unsigned long)*node);
            break;
        }
        case BP_OPT_SRCSERV_A:        
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* service = (bp_ipn_t*)val;
            if(getset)  channels[c].primary_acs_block.srcserv = *service;
            else        *service = channels[c].primary_acs_block.srcserv;
            bplog(BP_INFO, "Config. Source Service %s %lu\n", getset ? "<--" : "-->", (unsigned long)*service);
            break;
        }
        case BP_OPT_RPTNODE_A:      
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* node = (bp_ipn_t*)val;
            if(getset)  channels[c].primary_acs_block.rptnode = *node;
            else        *node = channels[c].primary_acs_block.rptnode;
            bplog(BP_INFO, "Config. Report To Node %s %lu\n", getset ? "<--" : "-->", (unsigned long)*node);
            break;
        }
        case BP_OPT_RPTSERV_A:     
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* service = (bp_ipn_t*)val;
            if(getset)  channels[c].primary_acs_block.rptserv = *service;
            else        *service = channels[c].primary_acs_block.rptserv;
            bplog(BP_INFO, "Config. Report To Service %s %lu\n", getset ? "<--" : "-->", (unsigned long)*service);
            break;
        }
        case BP_OPT_CSTNODE_A:       
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* node = (bp_ipn_t*)val;
            if(getset)  channels[c].primary_acs_block.cstnode = *node;
            else        *node = channels[c].primary_acs_block.cstnode;
            bplog(BP_INFO, "Config. Custodian Node %s %lu\n", getset ? "<--" : "-->", (unsigned long)*node);
            break;
        }
        case BP_OPT_CSTSERV_A:        
        {
            if(len != sizeof(bp_ipn_t)) return BP_PARMERR;
            bp_ipn_t* service = (bp_ipn_t*)val;
            if(getset)  channels[c].primary_acs_block.cstserv = *service;
            else        *service = channels[c].primary_acs_block.cstserv;
            bplog(BP_INFO, "Config. Custodian Service %s %lu\n", getset ? "<--" : "-->", (unsigned long)*service);
            break;
        }
        case BP_OPT_CREATETIMESYS_A:     
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(*enable != BP_TRUE && *enable != BP_FALSE) return BP_PARMERR;
            if(getset)  channels[c].primary_acs_block.creation_time_sys = *enable;
            else        *enable = channels[c].primary_acs_block.creation_time_sys;
            bplog(BP_INFO, "Config. Enable Creation Time System %s %d\n", getset ? "<--" : "-->", *enable);
            break;
        }
        case BP_OPT_CREATETIMEVAL_A:     
        {
            if(len != sizeof(bp_time_t)) return BP_PARMERR;
            bp_time_t* create_time = (bp_time_t*)val;
            if(getset)  channels[c].primary_acs_block.createtime = *create_time;
            else        *create_time = channels[c].primary_acs_block.createtime;
            bplog(BP_INFO, "Config. Creation Time %s %lu.%lu\n", getset ? "<--" : "-->", (unsigned long)create_time->s, (unsigned long)create_time->ns);
            break;
        }
        case BP_OPT_LIFETIME_A:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* lifetime = (int*)val;
            if(getset)  channels[c].primary_acs_block.lifetime = *lifetime;
            else        *lifetime = channels[c].primary_acs_block.lifetime;
            bplog(BP_INFO, "Config. Lifetime %s %d\n", getset ? "<--" : "-->", *lifetime);
            break;
        }
        case BP_OPT_PAYCRC:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* type = (int*)val;
            if(getset)  channels[c].integrity_block.paytype = *type;
            else        *type = channels[c].integrity_block.paytype;
            bplog(BP_INFO, "Config. Payload CRC Type %s %d\n", getset ? "<--" : "-->", *type);
            break;
        }
        case BP_OPT_PROCADMINONLY:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(*enable != BP_TRUE && *enable != BP_FALSE) return BP_PARMERR;
            if(getset)  channels[c].proc_admin_only = *enable;
            else        *enable = channels[c].proc_admin_only;
            bplog(BP_INFO, "Config. Enable Processing Only Admin Records %s %d\n", getset ? "<--" : "-->", *enable);
            break;
        }
        case BP_OPT_WRAPRSP:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* wrap = (int*)val;
            if(*wrap != BP_WRAP_RESEND && *wrap != BP_WRAP_BLOCK && *wrap != BP_WRAP_DROP) return BP_PARMERR;
            if(getset)  channels[c].wrap_response = *wrap;
            else        *wrap = channels[c].wrap_response;
            bplog(BP_INFO, "Config. Wrap Response %s %d\n", getset ? "<--" : "-->", *wrap);
            break;
        }
        case BP_OPT_ACSRATE:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* rate = (int*)val;
            if(getset)  channels[c].acs_rate_ms = *rate;
            else        *rate = channels[c].acs_rate_ms;
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
    if(getset) initialize_data_header(c);
    
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
    
    /* Set all agent control blocks to empty */
    for(i = 0; i < BP_MAX_AGENTS; i++)  
    {
        agents[i].agent_index = BP_EMPTY;
    }
    
    /* Set all channel control blocks to empty */
    for(i = 0; i < BP_MAX_CHANNELS; i++)  
    {
        channels[i].channel_index = BP_EMPTY;
        channels[i].agent_index = BP_EMPTY;
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_start - registers and initializes an agent
 *-------------------------------------------------------------------------------------*/
int bplib_start(bp_store_t* store, bp_ipn_t local_node)
{    
    int i;
    
    /* Check parameters */
    assert(store);
    assert(store->create);
    assert(store->destroy);
    assert(store->enqueue);
    assert(store->dequeue);
    assert(store->retrieve);
    assert(store->refresh);
    assert(store->relinquish);
    
    /* Find open agent slot */
    for(i = 0; i < BP_MAX_AGENTS; i++)
    {
        if(agents[i].agent_index == BP_EMPTY)
        {
            /* Initialize agent */
            agents[i].agent_index = i;
            agents[i].store = *store; // struct copy
            agents[i].local_node = local_node;
            return i; // agent handle
        }
    }
    
    /* Failed to find empty slot */
    return bplog(BP_AGENTSFULL, "Cannot start agent, not enough room\n");
}

/*--------------------------------------------------------------------------------------
 * bplib_open - 
 *-------------------------------------------------------------------------------------*/
int bplib_open(int agent, bp_ipn_t local_service, bp_ipn_t destination_node, bp_ipn_t destination_service)
{
    int i;
    
    /* Check agent */
    if(agent < 0 || agent >= BP_MAX_AGENTS)
    {
        return bplog(BP_PARMERR, "Invalid agent index: %d\n", agent);
    }
    else if(agents[agent].agent_index != agent) return BP_INVALIDAGENT;

    /* Find open channel slot */
    for(i = 0; i < BP_MAX_CHANNELS; i++)
    {
        if(channels[i].channel_index == BP_EMPTY)
        {
            /* Clear Channel Memory */
            bplib_os_memset(&channels[i], sizeof(channels[i]), 0);

            /* Initialize Assets */
            channels[i].channel_index                           = i;
            channels[i].agent_index                             = agent;
            channels[i].store_bundle                            = agents[agent].store.create();
            channels[i].store_payload                           = agents[agent].store.create();
            channels[i].store_acs                               = agents[agent].store.create();
    
            /* Initialize Primary Data Block */    
            channels[i].primary_data_block.version              = BP_DEFAULT_BP_VERSION;
            channels[i].primary_data_block.dstnode              = destination_node;
            channels[i].primary_data_block.dstserv              = destination_service;
            channels[i].primary_data_block.srcnode              = agents[agent].local_node;
            channels[i].primary_data_block.srcserv              = local_service;
            channels[i].primary_data_block.rptnode              = agents[agent].local_node;
            channels[i].primary_data_block.rptserv              = local_service;
            channels[i].primary_data_block.cstnode              = agents[agent].local_node;
            channels[i].primary_data_block.cstserv              = local_service;
            channels[i].primary_data_block.createtime.s         = BP_DEFAULT_CREATE_TIME_VAL_S;
            channels[i].primary_data_block.createtime.ns        = BP_DEFAULT_CREATE_TIME_VAL_NS;
            channels[i].primary_data_block.lifetime             = BP_DEFAULT_LIFETIME;
            channels[i].primary_data_block.is_admin_rec         = BP_FALSE;
            channels[i].primary_data_block.is_frag              = BP_FALSE;
            channels[i].primary_data_block.request_custody      = BP_DEFAULT_CSTRQST;
            channels[i].primary_data_block.allow_frag           = BP_TRUE;
            channels[i].primary_data_block.report_deletion      = BP_FALSE;
            channels[i].primary_data_block.creation_time_sys    = BP_DEFAULT_CREATE_TIME_SYS;
            channels[i].primary_data_block.sequence             = 0;

            /* Initialize Data Bundle Options */    
            channels[i].timeout                                 = BP_DEFAULT_TIMEOUT;
            channels[i].bundle_maxlength                        = BP_DEFAULT_BUNDLE_MAXLENGTH;
            channels[i].fragment_maxlength                      = BP_DEFAULT_BUNDLE_MAXLENGTH;
            channels[i].proc_admin_only                         = BP_DEFAULT_PROC_ADMIN_ONLY;
            channels[i].wrap_response                           = BP_DEFAULT_WRAP_RESPONSE;
            channels[i].acs_rate_ms                             = BP_DEFAULT_ACS_RATE;
    
            /* Initialize Primary ACS Block */    
            channels[i].primary_acs_block.version               = BP_DEFAULT_BP_VERSION;
            channels[i].primary_acs_block.dstnode               = 0;
            channels[i].primary_acs_block.dstserv               = 0;
            channels[i].primary_acs_block.srcnode               = agents[agent].local_node;
            channels[i].primary_acs_block.srcserv               = local_service;
            channels[i].primary_acs_block.rptnode               = agents[agent].local_node;
            channels[i].primary_acs_block.rptserv               = local_service;
            channels[i].primary_acs_block.cstnode               = agents[agent].local_node;
            channels[i].primary_acs_block.cstserv               = local_service;
            channels[i].primary_acs_block.createtime.s          = BP_DEFAULT_CREATE_TIME_VAL_S;
            channels[i].primary_acs_block.createtime.ns         = BP_DEFAULT_CREATE_TIME_VAL_NS;
            channels[i].primary_acs_block.lifetime              = BP_DEFAULT_LIFETIME;
            channels[i].primary_acs_block.is_admin_rec          = BP_TRUE;
            channels[i].primary_acs_block.is_frag               = BP_FALSE;
            channels[i].primary_acs_block.request_custody       = BP_FALSE;
            channels[i].primary_acs_block.allow_frag            = BP_FALSE;
            channels[i].primary_acs_block.report_deletion       = BP_FALSE;
            channels[i].primary_acs_block.creation_time_sys     = BP_DEFAULT_CREATE_TIME_SYS;
            channels[i].primary_acs_block.sequence              = 0;

            /* Initialize Custody Block */
            channels[i].custody_block.cid                       = 0;
            channels[i].custody_block.cstnode                   = agents[agent].local_node;
            channels[i].custody_block.cstserv                   = local_service;
            
            /* Initialize Integrity Block */    
            channels[i].integrity_block.paytype                 = BP_DEFAULT_PAY_CRC;

            /* Initialize Data */
            channels[i].current_custody_id                      = 0;
            channels[i].oldest_custody_id                       = 0;
            channels[i].num_acs_entries                         = 0;

            /* Populate Initial Data Header */
            initialize_data_header(i);
                        
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
        agents[channels[channel].agent_index].store.destroy(channels[channel].store_bundle);
        agents[channels[channel].agent_index].store.destroy(channels[channel].store_payload);
        agents[channels[channel].agent_index].store.destroy(channels[channel].store_acs);
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
    bp_channel_t* ch;
    bp_store_enqueue_t enqueue;

    /* Check Parameters */
    if(channel < 0 || channel >= BP_MAX_CHANNELS)       return BP_PARMERR;
    else if(channels[channel].channel_index != channel) return BP_INVALIDCHANNEL;
    else if(payload == NULL)                            return BP_PARMERR;
    else if(size > channels[channel].bundle_maxlength)  return BP_PARMERR;
    
    /* Set Shortcuts */
    ch = &channels[channel];
    enqueue = agents[ch->agent_index].store.enqueue;

    /* Check Fragmentation */
    if(size > ch->fragment_maxlength)
    {
        uint8_t* payload_ptr = (uint8_t*)payload;
        int payload_offset = 0;
        while(payload_offset < size)
        {
            /* Enqueue Pre-Built Bundle in Storage Service */
            update_data_header(channel, &payload_ptr[payload_offset], ch->fragment_maxlength, payload_offset, size);
            enqueue(ch->store_bundle, &ch->data_bundle, sizeof(bp_data_t), &payload_ptr[payload_offset], ch->fragment_maxlength, timeout);
            payload_offset += ch->fragment_maxlength;
        }
    }
    else
    {
        /* Enqueue Pre-Built Bundle in Storage Service */
        update_data_header(channel, payload, size, 0, size);
        enqueue(ch->store_bundle, &ch->data_bundle, sizeof(bp_data_t), payload, size, timeout);
    }

    /* Increment Sequence Count */
    ch->primary_data_block.sequence++;

    /* Return Status */
    return BP_SUCCESS;
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
    int load_custody;                   // whether or not bundle has custody (ACS does not)
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
    dequeue     = agents[ch->agent_index].store.dequeue;
    retrieve    = agents[ch->agent_index].store.retrieve;
    refresh     = agents[ch->agent_index].store.refresh;
    relinquish  = agents[ch->agent_index].store.relinquish;
    
    /* Setup State */
    bplib_systime(&sysnow);     // get current system time (to used for timeouts)
    load_bundle = NULL;         // start out assuming nothing to send
    load_custody = BP_FALSE;    // start out not assigning custody
    status = BP_SUCCESS;        // start out assuming operation succeeds
    sid = BP_SID_VACANT;        // storage id points to nothing
    *loadflags = 0;             // start with clean slate
    
    /* Check if ACS Needs to be Sent */
    if(dequeue(ch->store_acs, (void**)&storebuf, &storelen, &sid, timeout) == BP_SUCCESS)
    {
        /* Transmit ACS Bundle */
        load_bundle = storebuf;
        load_size = storelen;
        load_store = ch->store_acs;
        load_custody = BP_FALSE;
        
        /* ACS Always Need to be Routed */
        *loadflags |= BP_LOAD_ROUTENEEDED;
    }
    
    /* Try to Send Timed-out Bundle */
    while((load_bundle == NULL) && (ch->oldest_custody_id < ch->current_custody_id))
    {
        ati = ch->oldest_custody_id % BP_ACTIVE_TABLE_SIZE;
        sid = ch->active_table[ati];
        if(sid == BP_SID_VACANT) // entry vacant
        {
            ch->oldest_custody_id++;
        }
        else if(retrieve(ch->store_bundle, (void**)&storebuf, &storelen, sid, timeout) == BP_SUCCESS)
        {
            /* Check Timeout */
            bp_data_t* bundle_ptr = (bp_data_t*)storebuf;
            if(bplib_cmptime(sysnow, bundle_ptr->prolog.retxtime) >= 0)
            {
                /* Retransmit Bundle */
                load_bundle = bundle_ptr->header;
                load_size = storelen - BP_DATA_PROLOG_SIZE;
                load_store = ch->store_bundle;
                load_custody = BP_TRUE;
            }

            /* Exit Loop */
            break;
        }
        else // failed to retrieve bundle from storage
        {
            relinquish(ch->store_bundle, sid);
            ch->active_table[ati] = BP_SID_VACANT;
            bplog(BP_FAILEDSTORE, "Failed to retrieve bundle from storage\n");
        }            
    }

    /* Try to Send Stored Bundle (if nothing sent yet) */
    while(load_bundle == NULL)
    {
        ati = ch->current_custody_id % BP_ACTIVE_TABLE_SIZE;
        sid = ch->active_table[ati];
        if(sid == BP_SID_VACANT) // entry vacant
        {
            /* Dequeue Bundle from Storage Service */
            if(dequeue(ch->store_bundle, (void**)&storebuf, &storelen, &sid, timeout) == BP_SUCCESS)
            {
                /* Write Re-Transmit Time */
                bp_data_t* bundle_ptr = (bp_data_t*)storebuf;
                bplib_addtime(&sysretx, sysnow, ch->timeout);
                bundle_ptr->prolog.retxtime = sysretx;
                refresh(ch->store_bundle, storebuf, sizeof(bp_time_t), 0, sid, timeout);

                /* Transmit Dequeued Bundle */
                load_bundle = bundle_ptr->header;
                load_size = storelen - BP_DATA_PROLOG_SIZE;
                load_store = ch->store_bundle;
                load_custody = BP_TRUE;
            }
            else
            {
                /* No Bundles in Storage to Send */
                status = BP_UNDERFLOW;
            }

            /* Exit Loop */
            break;
        }
        else if(ch->wrap_response == BP_WRAP_RESEND)
        {
            /* Retrieve Bundle from Storage */
            if(retrieve(ch->store_bundle, (void**)&storebuf, &storelen, sid, timeout) == BP_SUCCESS)
            {
                /* Retransmit Bundle */
                bp_data_t* bundle_ptr = (bp_data_t*)storebuf;
                load_bundle = bundle_ptr->header;
                load_size = storelen - BP_DATA_PROLOG_SIZE;
                load_store = ch->store_bundle;
                load_custody = BP_TRUE;
            }
            else // failed to retrieve bundle from storage
            {
                relinquish(ch->store_bundle, sid);
                ch->active_table[ati] = BP_SID_VACANT;
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
            relinquish(ch->store_bundle, sid);
            ch->active_table[ati] = BP_SID_VACANT;
            ch->oldest_custody_id++;
        }
    }

    /* Check if Bundle Ready to Transmit */
    if(load_bundle != NULL)
    {
        /* Check Buffer Size */
        if(size < load_size)
        {
            status = BP_BUNDLETOOLARGE;
            bplog(status, "Bundle too large to fit inside buffer (%d %d)\n", size, load_size);
        }
        else
        {
            /* Successfully Load Bundle to Application and Relinquish Memory */
            bplib_os_memcpy(bundle, load_bundle, load_size);
            status = load_size;

            /* Assign Custody ID */
            if(load_custody == BP_TRUE)
            {
                ati = ch->current_custody_id % BP_ACTIVE_TABLE_SIZE;
                ch->active_table[ati] = sid;
                bplib_blk_cteb_update(&storebuf[BP_DATA_HDR_CTEB_BLK_INDEX], BP_CTEB_BLK_LENGTH, ch->current_custody_id);
                ch->current_custody_id++;
            }
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
    bp_store_enqueue_t  enqueue;
    uint8_t*            buffer;
    uint8_t             flags;
    int                 index, status, bib_present, cteb_present;
    bp_blk_pri_t        priblk;
    bp_blk_cteb_t       ctebblk;
    bp_blk_bib_t        bibblk;
    bp_time_t           sysnow, expiretime;
    uint8_t             blk_type;
    uint32_t            blk_flags, blk_length;
    int                 start_index, end_index, flag_size;

    /* Check Parameters */
    if(channel < 0 || channel >= BP_MAX_CHANNELS)       return BP_PARMERR;
    else if(channels[channel].channel_index != channel) return BP_INVALIDCHANNEL;
    else if(bundle == NULL)                             return BP_PARMERR;

    /* Set Shortcuts */
    ch = &channels[channel];
    enqueue = agents[ch->agent_index].store.enqueue;

    /* Setup Bundle Parsing */
    *procflags = 0;
    buffer = (uint8_t*)bundle;
    index = 0;
    bib_present = BP_FALSE;
    cteb_present = BP_FALSE;
    
    /* Parse Primary Block */
    status = bplib_blk_pri_read(&buffer[index], size - index, &priblk);
    if(status <= 0) return status;
    else index += status;

    /* Set Processing Flag for Reporting Deletion */
    if(priblk.report_deletion) *procflags |= BP_PROC_REPORTDELETE;

    /* Check Administrative Record Status */
    if(ch->proc_admin_only && priblk.is_admin_rec == BP_FALSE)
    { 
        /* Ignore all non-admin records */
        return bplog(BP_IGNORE, "Non-administrative bundle ignored\n");
    }

    /* Check Unsupported */
    if(priblk.is_frag || (priblk.dictlen != 0))
    {
        return bplog(BP_UNSUPPORTED, "Unsupported bundle attempted to be processed (%d %d)\n", priblk.is_frag, priblk.dictlen);
    }

    /* Check Life Time */
    bplib_systime(&sysnow);
    bplib_addtime(&expiretime, priblk.createtime, priblk.lifetime);
    if(bplib_cmptime(sysnow, expiretime) >= 0)
    {
        return bplog(BP_EXPIRED, "Expired bundled attempted to be processed \n");
    }

    /* Parse and Process Remaining Blocks */
    while(index < size)
    {
        /* Read Block Information */
        start_index = index;
        blk_type = buffer[index++];
        index += (flag_size = bplib_sdnv_read(&buffer[index], size - index, &blk_flags, &flags));
        index += bplib_sdnv_read(&buffer[index], size - index, &blk_length, &flags);
        if(flags != BP_SUCCESS) return BP_BUNDLEPARSEERR;
        end_index = index + blk_length;

        /* Check Block Type */
        if(blk_type == BP_PROTO_PAY_BLK_TYPE) // Payload Block
        {
            /* Verify Block Length */
            if(size - index < (int)blk_length) return BP_BUNDLEPARSEERR;
            
            /* Perform Integrity Check */
            if(bib_present)
            {
                status = bplib_blk_bib_verify(&buffer[index], blk_length, &bibblk);
                if(status <= 0)
                {
                    return bplog(status, "Bundle failed integrity check\n");
                }
            }

            /* Process Payload */
            if(priblk.is_admin_rec) // Administrative Record
            {
                uint32_t rec_type, rec_status;

                /* Check Record Length */
                if(blk_length < 2)
                {
                    return bplog(BP_BUNDLEPARSEERR, "Invalid block length: %d\n", blk_length);
                }

                /* Read Record Information */
                rec_type = buffer[index++];
                rec_status = buffer[index++];

                /* Check Record Type */
                if(rec_type == BP_PROTO_ACS_REC_TYPE) // Aggregate Custody Signal
                {
                    status = bplib_rec_acs_process( &buffer[index], size - index, rec_status,
                                                    ch->active_table, BP_ACTIVE_TABLE_SIZE, 
                                                    agents[ch->agent_index].store.relinquish, ch->store_bundle);
                }
                else if(rec_type == BP_PROTO_CS_REC_TYPE) // Custody Signal
                {
                    status = BP_UNSUPPORTED;
                }
                else if(rec_type == BP_PROTO_STAT_REC_TYPE) // Status Report
                {
                    status = BP_UNSUPPORTED;
                }
                else // Unknown Record
                {
                    status = BP_UNKNOWNREC;
                    bplog(status, "Unknown administrative record: %u\n", (unsigned int)rec_type);
                }
            }
            else // process bundle
            {
                /* Forward Bundle: Bundle's Final Destination Is NOT This Agent */
                if(priblk.dstnode != ch->primary_data_block.srcnode)
                {
                    /* Check Need for Fragmentation */
                    if(size > ch->fragment_maxlength) // fragment bundle
                    {
                        if(priblk.allow_frag)   status = BP_UNSUPPORTED;
                        else                    status = BP_BUNDLETOOLARGE;
                        bplog(status, "Unable to fragment bundle\n");
                    }
                    else // forward bundle
                    {
                        /* Accept Custody */
                        if(priblk.request_custody)
                        {
                            if(cteb_present)
                            {
                                *procflags |= update_acs_payload(ch, ctebblk.cid, ctebblk.cstnode, ctebblk.cstserv, BP_FALSE, timeout);
                            }
                            else // only aggregate custody supported
                            {
                                *procflags |= BP_PROC_NONCOMPLIANT;
                                bplog(BP_UNSUPPORTED, "Only aggregate custody signals supported\n");
                            }

                            /* Update New Bundle Header to Send */
                            update_data_header(channel, &buffer[index], size - index, 0, size - index);
                            bplib_blk_pri_setdst(ch->data_bundle.header, priblk.dstnode, priblk.dstserv);
                            ch->data_bundle.prolog.retxtime.s = 0;
                            ch->data_bundle.prolog.retxtime.ns = 0;
                            status = enqueue(ch->store_bundle, &ch->data_bundle, sizeof(bp_data_t), &buffer[index], size - index, timeout);

                            /* Restore Destination EID */
                            bplib_blk_pri_setdst(ch->data_bundle.header, ch->primary_data_block.dstnode, ch->primary_data_block.dstserv);
                        }
                        else
                        {
                            /* Enqueue Received Bundle As-Is in Storage Service */
                            uint8_t prolog[BP_DATA_PROLOG_SIZE] = {0};
                            status = enqueue(ch->store_bundle, prolog, BP_DATA_PROLOG_SIZE, buffer, size, timeout);
                        }
                    }
                }
                else if(priblk.dstserv != ch->primary_data_block.srcserv) // wrong channel
                {
                    status = BP_WRONGCHANNEL;
                    bplog(status, "Wrong channel to service bundle (%lu, %lu)\n", (unsigned long)priblk.dstserv, (unsigned long)ch->primary_data_block.srcserv);
                }
                else // deliver bundle payload to application
                {
                    bp_payload_t payload;
                    
                    /* Populate Payload */
                    payload.cid = ctebblk.cid;
                    payload.cstnode = ctebblk.cstnode;
                    payload.cstserv = ctebblk.cstserv;
                    payload.cstrqst = BP_FALSE;

                    /* Set Custody Transfer Request */
                    if(priblk.request_custody)
                    {
                        if(cteb_present)
                        {
                            payload.cstrqst = BP_TRUE;
                        }
                        else
                        {
                            *procflags |= BP_PROC_NONCOMPLIANT;
                            bplog(BP_UNSUPPORTED, "Only aggregate custody supported\n");
                        }
                    }

                    /* Enqueue Payload into Storage */
                    status = enqueue(ch->store_payload, &payload, sizeof(payload), &buffer[index], size - index, timeout);
                }
            }

            /* Return Status (Payload Blocks Must Be Last) */
            if(status <= 0) return status;
            else            return BP_SUCCESS;
        }
        else if(blk_type == BP_CTEB_BLK_TYPE)
        {
            cteb_present = BP_TRUE;
            status = bplib_blk_cteb_read(&buffer[index], size - index, &ctebblk);
            if(status <= 0) return status;
            else            index += status;
        }
        else if(blk_type == BP_BIB_BLK_TYPE)
        {
            bib_present = BP_TRUE;
            status = bplib_blk_bib_read(&buffer[index], size - index, &bibblk);
            if(status <= 0) return status;
            else            index += status;
        }
        else // skip over block
        {
            /* Mark Processing as Incomplete */
            *procflags |= BP_PROC_INCOMPLETE;
            bplog(BP_UNSUPPORTED, "Skipping over unrecognized block\n");

            /* Should transmit status report that block cannot be processed */
            if(blk_flags & BP_BLK_NOTIFYNOPROC_MASK) *procflags |= BP_PROC_NONCOMPLIANT;
            
            /* Delete bundle since block not recognized */
            if(blk_flags & BP_BLK_DELETENOPROC_MASK) return BP_DROPPED;

            /* Should drop block since it cannot be processed */
            if(blk_flags & BP_BLK_DROPNOPROC_MASK) *procflags |= BP_PROC_NONCOMPLIANT;

            /* Mark As Forwarded without Processed */
            blk_flags |= BP_BLK_FORWARDNOPROC_MASK;
            bplib_sdnv_write(&buffer[start_index], flag_size, blk_flags);

            /* Goto Next Block */
            index += blk_length;
        }

        /* Check End Index of Block */
        if(index != end_index)
        {
            return bplog(BP_BUNDLEPARSEERR, "Block processing detected mismatched block length (%d %d)\n", index, end_index);
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
    bp_payload_t*           payptr;
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
    dequeue = agents[ch->agent_index].store.dequeue;
    relinquish = agents[ch->agent_index].store.relinquish;

    /* Dequeue Payload from Storage */
    status = dequeue(ch->store_payload, (void**)&storebuf, &storelen, &sid, timeout);    
    if(status != BP_SUCCESS) return status;
    else if(storelen < (int)sizeof(bp_payload_t))
    {
        return bplog(BP_FAILEDSTORE, "Payload retrieved from storage is too small: %d\n", storelen);
    }

    /* Access Payload */
    payptr = (bp_payload_t*)storebuf;
    paylen = storelen - sizeof(bp_payload_t);

    /* Copy Payload */
    if(size < paylen)
    {
        status = BP_PAYLOADTOOLARGE;
        bplog(status, "Payload too large to fit inside buffer (%d %d)\n", size, paylen);
    }
    else
    {
        /* Successfully Return Payload to Application and Relinquish Memory */
        bplib_os_memcpy(payload, &storebuf[sizeof(bp_payload_t)], paylen);
        status = paylen;
        relinquish(ch->store_payload, sid);
    }

    /* Acknowledge Custody */
    if(payptr->cstrqst && (status > 0))
    {
        *acptflags |= update_acs_payload(ch, payptr->cid, payptr->cstnode, payptr->cstserv, BP_TRUE, timeout);
    }

    /* Return Status */
    return status;
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
 *  Notes:      1. The format of BP time precludes the nansecond field from being larger
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
