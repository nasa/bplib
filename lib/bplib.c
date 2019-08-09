/************************************************************************
 * File: bplib.c
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
#include "sdnv.h"
#include "block.h"
#include "pri.h"
#include "cteb.h"
#include "bib.h"
#include "pay.h"
#include "dacs.h"
#include "bplib_os.h"
#include "crc.h"
#include "rb_tree.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#ifndef LIBID
#define LIBID                           "unversioned"
#endif

#define BP_EMPTY                        (-1)
#define BP_BUNDLE_HDR_BUF_SIZE          128
#define BP_NUM_EXCLUDE_REGIONS          8

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* --------------- Storage Types ------------------- */

/* Payload Storage Block */
typedef struct {
    bool                request_custody;    /* boolean whether original bundle requested custody on payload delivery */
    int                 payloadsize;        /* size of the payload */
} bp_payload_store_t;

/* Bundle Storage Block */
typedef struct {
    uint32_t            exprtime;           /* absolute time when bundle expires */
    bp_sdnv_t           cidsdnv;            /* SDNV of custody id field of bundle */
    int                 cteboffset;         /* offset of the CTEB block of bundle */
    int                 biboffset;          /* offset of the BIB block of bundle */
    int                 payoffset;          /* offset of the payload block of bundle */
    int                 headersize;         /* size of the header (portion of buffer below used) */
    int                 bundlesize;         /* total size of the bundle (header and payload) */
    uint8_t             header[BP_BUNDLE_HDR_BUF_SIZE]; /* header portion of bundle */
} bp_bundle_store_t;

/* --------------- Bundle Types ------------------- */

/* Data Bundle */
typedef struct {
    bp_blk_pri_t        primary_block;
    bp_blk_cteb_t       custody_block;
    bp_blk_bib_t        integrity_block;
    bp_blk_pay_t        payload_block;
    int                 maxlength;  /* maximum size of bundle in bytes (includes header blocks) */
    int                 originate;  /* 1: originated bundle, 0: forwarded bundle */
    bp_bundle_store_t   bundle_store;
    bp_payload_store_t  payload_store;
} bp_data_bundle_t;

/* --------------- Application Types ------------------- */

/* Active Table */
typedef struct {
    bp_sid_t*           sid;
    uint32_t*           retx;
    uint32_t            oldest_cid;
    uint32_t            current_cid;
} bp_active_table_t;

/* Channel Control Block */
typedef struct {
    int                 index;
    bp_attr_t           attributes;

    /* Mutex's and Signals */
    int                 data_bundle_lock;
    int                 dacs_bundle_lock;
    int                 active_table_signal;

    /* Local EID */
    uint32_t            local_node;
    uint32_t            local_service;

    /* Storage Service Call-Backs and Handles  */
    bp_store_t          storage;
    int                 data_store_handle;
    int                 payload_store_handle;
    int                 dacs_store_handle;

    bp_data_bundle_t    data_bundle;
    bp_dacs_t           dacs;

    bp_active_table_t   active_table;

    bp_stats_t          stats;

    int                 timeout;                /* seconds, zero for infinite */
    int                 dacs_rate;              /* number of seconds to wait between sending ACS bundles */
    int                 proc_admin_only;        /* process only administrative records (for sender only agents) */
    int                 wrap_response;          /* what to do when active table wraps */
    int                 cid_reuse;              /* favor reusing CID when retransmitting */
} bp_channel_t;

/******************************************************************************
 FILE DATA
 ******************************************************************************/

static bp_channel_t* channels;
static int channels_max;
static int channels_lock;

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * getset_opt - Get/Set utility function
 *
 *  - getset --> false: get, true: set
 *  - assumes parameter checking has already been performed
 *-------------------------------------------------------------------------------------*/
static int getset_opt(int c, int opt, void* val, int len, bool getset)
{
    bp_channel_t* ch = &channels[c];

    /* Select and Process Option */
    switch(opt)
    {
        case BP_OPT_SETSEQUENCE_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            uint32_t* seq = (uint32_t*)val;
            if(getset)  ch->data_bundle.primary_block.createseq.value = *seq;
            else        *seq = ch->data_bundle.primary_block.createseq.value;
            break;
        }
        case BP_OPT_LIFETIME_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* lifetime = (int*)val;
            if(getset)  ch->data_bundle.primary_block.lifetime.value = *lifetime;
            else        *lifetime = ch->data_bundle.primary_block.lifetime.value;
            break;
        }
        case BP_OPT_CSTRQST_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(getset && *enable != true && *enable != false) return BP_PARMERR;
            if(getset)  ch->data_bundle.primary_block.request_custody = *enable;
            else        *enable = ch->data_bundle.primary_block.request_custody;
            break;
        }
        case BP_OPT_ICHECK_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(getset && *enable != true && *enable != false) return BP_PARMERR;
            if(getset)  ch->data_bundle.primary_block.integrity_check = *enable;
            else        *enable = ch->data_bundle.primary_block.integrity_check;
            break;
        }
        case BP_OPT_ALLOWFRAG_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(getset && *enable != true && *enable != false) return BP_PARMERR;
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
            break;
        }
        case BP_OPT_PAYCRC_D:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* type = (int*)val;
            if(getset)  ch->data_bundle.integrity_block.cipher_suite_id.value = *type;
            else        *type = ch->data_bundle.integrity_block.cipher_suite_id.value;
            break;
        }
        case BP_OPT_TIMEOUT:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* timeout = (int*)val;
            if(getset)  ch->timeout = *timeout;
            else        *timeout = ch->timeout;
            break;
        }
        case BP_OPT_BUNDLELEN:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* maxlen = (int*)val;
            if(getset)  ch->data_bundle.maxlength = *maxlen;
            else        *maxlen = ch->data_bundle.maxlength;
            break;
        }
        case BP_OPT_ORIGINATE:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(getset && *enable != true && *enable != false) return BP_PARMERR;
            if(getset)  ch->data_bundle.originate = *enable;
            else        *enable = ch->data_bundle.originate;
            break;
        }
        case BP_OPT_PROCADMINONLY:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(getset && *enable != true && *enable != false) return BP_PARMERR;
            if(getset)  ch->proc_admin_only = *enable;
            else        *enable = ch->proc_admin_only;
            break;
        }
        case BP_OPT_WRAPRSP:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* wrap = (int*)val;
            if(getset && *wrap != BP_WRAP_RESEND && *wrap != BP_WRAP_BLOCK && *wrap != BP_WRAP_DROP) return BP_PARMERR;
            if(getset)  ch->wrap_response = *wrap;
            else        *wrap = ch->wrap_response;
            break;
        }
        case BP_OPT_CIDREUSE:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(getset && *enable != true && *enable != false) return BP_PARMERR;
            if(getset)  ch->cid_reuse = *enable;
            else        *enable = ch->cid_reuse;
            break;
        }
        case BP_OPT_ACSRATE:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* rate = (int*)val;
            if(getset)  ch->dacs_rate = *rate;
            else        *rate = ch->dacs_rate;
            break;
        }
        default:
        {
            /* Option Not Found */
            return bplog(BP_PARMERR, "Config. Option Not Found (%d)\n", opt);
        }
    }

    /* Re-initialize Bundles */
    if(getset) initialize_orig_bundle(&ch->data_bundle);

    /* Option Successfully Processed */
    return BP_SUCCESS;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_init - initializes bp library
 *-------------------------------------------------------------------------------------*/
void bplib_init(int max_channels)
{
    int i;

    /* Initialize OS Interface */
    bplib_os_init();
    
    /* Initialize the Bundle Integrity Block Module */
    bib_init();

    /* Create Channel Lock */
    channels_lock = bplib_os_createlock();

    /* Allocate Channel Memory */
    if(max_channels <= 0)   channels_max = BP_DEFAULT_MAX_CHANNELS;
    else                    channels_max = max_channels;
    channels = (bp_channel_t*)malloc(channels_max * sizeof(bp_channel_t));

    /* Set All Channel Control Blocks to Empty */
    for(i = 0; i < channels_max; i++)
    {
        channels[i].index = BP_EMPTY;
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_open -
 *-------------------------------------------------------------------------------------*/
int bplib_open(bp_store_t storage, bp_ipn_t local_node, bp_ipn_t local_service, bp_ipn_t destination_node, bp_ipn_t destination_service, bp_attr_t* attributes)
{
    int i, channel, status;

    fflush(stdin);
    assert(storage.create);
    assert(storage.destroy);
    assert(storage.enqueue);
    assert(storage.dequeue);
    assert(storage.retrieve);
    assert(storage.relinquish);
    assert(storage.getcount);

    channel = BP_INVALID_HANDLE;
    bplib_os_lock(channels_lock);
    {
        /* Find open channel slot */
        for(i = 0; i < channels_max; i++)
        {
            if(channels[i].index == BP_EMPTY)
            {
                /* Clear Channel Memory and Initialize to Defaults */
                memset(&channels[i], 0, sizeof(channels[i]));
                channels[i].data_bundle_lock     = BP_INVALID_HANDLE;
                channels[i].dacs_bundle_lock     = BP_INVALID_HANDLE;
                channels[i].active_table_signal  = BP_INVALID_HANDLE;
                channels[i].data_store_handle    = BP_INVALID_HANDLE;
                channels[i].payload_store_handle = BP_INVALID_HANDLE;
                channels[i].dacs_store_handle    = BP_INVALID_HANDLE;
                
                /* Set Initial Attributes */
                if(attributes) channels[i].attributes = *attributes;
 
                /* Default Unset Attributes */
                if(attributes->active_table_size == 0)      channels[i].attributes.active_table_size = BP_DEFAULT_ACTIVE_TABLE_SIZE;                    
                if(attributes->max_concurrent_dacs == 0)    channels[i].attributes.max_concurrent_dacs = BP_DEFAULT_MAX_CONCURRENT_DACS;
                if(attributes->max_fills_per_dacs == 0)     channels[i].attributes.max_fills_per_dacs = BP_DEFAULT_MAX_FILLS_PER_DACS;
                if(attributes->max_gaps_per_dacs == 0)      channels[i].attributes.max_gaps_per_dacs = BP_DEFAULT_MAX_GAPS_PER_DACS;

                /* Initialize Assets */
                channels[i].data_bundle_lock     = bplib_os_createlock();
                channels[i].dacs_bundle_lock     = bplib_os_createlock();
                channels[i].active_table_signal  = bplib_os_createlock();
                channels[i].local_node           = local_node;
                channels[i].local_service        = local_service;
                channels[i].storage              = storage; /* structure copy */
                channels[i].data_store_handle    = channels[i].storage.create(channels[i].attributes.storage_service_parm);
                channels[i].payload_store_handle = channels[i].storage.create(channels[i].attributes.storage_service_parm);
                channels[i].dacs_store_handle    = channels[i].storage.create(channels[i].attributes.storage_service_parm);

                /* Check Assets */
                if( channels[i].data_bundle_lock  < 0 ||
                    channels[i].dacs_bundle_lock  < 0 ||
                    channels[i].active_table_signal < 0 )
                {
                    bplib_close(i);
                    bplib_os_unlock(channels_lock);
                    bplog(BP_FAILEDOS, "Failed to allocate OS locks for channel\n");
                    return BP_INVALID_HANDLE;
                }
                else if( channels[i].data_store_handle    < 0 ||
                         channels[i].payload_store_handle < 0 ||
                         channels[i].dacs_store_handle    < 0 )
                {
                    bplib_close(i);
                    bplib_os_unlock(channels_lock);
                    bplog(BP_FAILEDSTORE, "Failed to create storage handles for channel\n");
                    return BP_INVALID_HANDLE;
                }

                /* Register Channel */
                channels[i].index = i;

                /* Initialize Data Bundle */
                channels[i].data_bundle.primary_block               = native_data_pri_blk;
                channels[i].data_bundle.primary_block.dstnode.value = destination_node;
                channels[i].data_bundle.primary_block.dstserv.value = destination_service;
                channels[i].data_bundle.primary_block.srcnode.value = local_node;
                channels[i].data_bundle.primary_block.srcserv.value = local_service;
                channels[i].data_bundle.primary_block.rptnode.value = 0;
                channels[i].data_bundle.primary_block.rptserv.value = 0;
                channels[i].data_bundle.primary_block.cstnode.value = local_node;
                channels[i].data_bundle.primary_block.cstserv.value = local_service;
                channels[i].data_bundle.custody_block               = native_cteb_blk;
                channels[i].data_bundle.custody_block.cid.value     = 0;
                channels[i].data_bundle.custody_block.cstnode       = local_node;
                channels[i].data_bundle.custody_block.cstserv       = local_service;
                channels[i].data_bundle.integrity_block             = native_bib_blk;
                channels[i].data_bundle.payload_block               = native_pay_blk;
                channels[i].data_bundle.maxlength                   = BP_DEFAULT_BUNDLE_MAXLENGTH;
                channels[i].data_bundle.originate                   = BP_DEFAULT_ORIGINATION;

                /* Write EID */
                bplib_ipn2eid(channels[i].data_bundle.custody_block.csteid, BP_MAX_EID_STRING, local_node, local_service);

                /* Initialize DACS */
                status = dacs_initialize(&channels[i].dacs, channels[i].attributes.max_concurrent_dacs, channels[i].attributes.max_fills_per_dacs, channels[i].attributes.max_gaps_per_dacs, local_node, local_service);
                if(status != BP_SUCCESS)
                {
                    bplib_close(i);
                    bplib_os_unlock(channels_lock);
                    return BP_INVALID_HANDLE;
                }
                
                /* Allocate Memory for Active Table */
                channels[i].active_table.sid = (bp_sid_t*)malloc(sizeof(bp_sid_t) * channels[i].attributes.active_table_size);
                channels[i].active_table.retx = (uint32_t*)malloc(sizeof(uint32_t) * channels[i].attributes.active_table_size);
                if(channels[i].active_table.sid == NULL || channels[i].active_table.retx == NULL)
                {
                    bplib_close(i);
                    bplib_os_unlock(channels_lock);
                    bplog(BP_FAILEDMEM, "Failed to allocate memory for channel active table\n");                    
                    return BP_INVALID_HANDLE;
                }
                else
                {
                    memset(channels[i].active_table.sid, 0, sizeof(bp_sid_t*) * channels[i].attributes.active_table_size);
                    memset(channels[i].active_table.retx, 0, sizeof(uint32_t) * channels[i].attributes.active_table_size);
                }
                
                /* Initialize Data */
                channels[i].active_table.oldest_cid     = 0;
                channels[i].active_table.current_cid    = 0;
                channels[i].dacs_rate                   = BP_DEFAULT_DACS_RATE;
                channels[i].timeout                     = BP_DEFAULT_TIMEOUT;
                channels[i].proc_admin_only             = BP_DEFAULT_PROC_ADMIN_ONLY;
                channels[i].wrap_response               = BP_DEFAULT_WRAP_RESPONSE;
                channels[i].cid_reuse                   = BP_DEFAULT_CID_REUSE;

                /* Populate Initial Data Bundle Storage Header
                 *  only initialize data bundle and not dacs or forwarded bundles
                 *  since for storage the dacs bundles are initialized
                 *  when custody requests arrive, and forwarded bundles are
                 *  initialized each time a bundle is forwarded */
                initialize_orig_bundle(&channels[i].data_bundle);

                /* Set Channel Handle */
                channel = i;
                break;
            }
        }
    }
    bplib_os_unlock(channels_lock);

    /* Check if Channels Table Full */
    if(channel == BP_INVALID_HANDLE)
    {
        bplog(BP_CHANNELSFULL, "Cannot open channel, not enough room\n");
    }
        
    /* Return Channel */
    return channel;
}

/*--------------------------------------------------------------------------------------
 * bplib_close -
 *-------------------------------------------------------------------------------------*/
void bplib_close(int channel)
{
    if(channel >= 0 && channel < channels_max && channels[channel].index != BP_EMPTY)
    {
        bp_channel_t* ch = &channels[channel];
        bplib_os_lock(channels_lock);
        {
            if(ch->data_store_handle    != BP_INVALID_HANDLE) ch->storage.destroy(ch->data_store_handle);
            if(ch->payload_store_handle != BP_INVALID_HANDLE) ch->storage.destroy(ch->payload_store_handle);
            if(ch->dacs_store_handle    != BP_INVALID_HANDLE) ch->storage.destroy(ch->dacs_store_handle);
            
            if(ch->data_bundle_lock     != BP_INVALID_HANDLE) bplib_os_destroylock(ch->data_bundle_lock);
            if(ch->dacs_bundle_lock     != BP_INVALID_HANDLE) bplib_os_destroylock(ch->dacs_bundle_lock);
            if(ch->active_table_signal  != BP_INVALID_HANDLE) bplib_os_destroylock(ch->active_table_signal);
            
            dacs_uninitialize(&ch->dacs);
                   
            if(ch->active_table.sid) free(ch->active_table.sid);
            if(ch->active_table.retx) free(ch->active_table.retx);
                    
            ch->index = BP_EMPTY;
        }
        bplib_os_unlock(channels_lock);
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_getopt -
 *-------------------------------------------------------------------------------------*/
int bplib_getopt(int channel, int opt, void* val, int len)
{
    int status;

    /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(val == NULL)                            return BP_PARMERR;

    /* Call Internal Function */
    status = getset_opt(channel, opt, val, len, false);

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_setopt -
 *-------------------------------------------------------------------------------------*/
int bplib_setopt(int channel, int opt, void* val, int len)
{
    int status;

     /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(val == NULL)                            return BP_PARMERR;

    /* Call Internal Function */
    status = getset_opt(channel, opt, val, len, true);

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_latchstats -
 *-------------------------------------------------------------------------------------*/
int bplib_latchstats(int channel, bp_stats_t* stats)
{
     /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(stats == NULL)                          return BP_PARMERR;

    /* Shortcut to Channel */
    bp_channel_t* ch = &channels[channel];

    /* Update Store Counts */
    ch->stats.bundles = ch->storage.getcount(ch->data_store_handle);
    ch->stats.payloads = ch->storage.getcount(ch->payload_store_handle);
    ch->stats.records = ch->storage.getcount(ch->dacs_store_handle);

    /* Latch Statistics */
    *stats = ch->stats;

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_store -
 *-------------------------------------------------------------------------------------*/
int bplib_store(int channel, void* payload, int size, int timeout, uint16_t* storflags)
{
    int status;

    /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(payload == NULL)                        return BP_PARMERR;

    /* Lock Data Bundle */
    bplib_os_lock(channels[channel].data_bundle_lock);
    {
        bp_channel_t* ch = &channels[channel];

        if(!ch->data_bundle.originate)
        {
            status = bplog(BP_WRONGORIGINATION, "Cannot originate bundle on channel designated for forwarding\n");
        }
        else
        {
            /* Update Payload */
            ch->data_bundle.payload_block.payptr = (uint8_t*)payload;
            ch->data_bundle.payload_block.paysize = size;

            /* Store Bundle */
            status = store_data_bundle(&ch->data_bundle, ch->storage.enqueue, ch->data_store_handle, timeout, storflags);
            if(status == BP_SUCCESS) ch->stats.generated++;
        }
    }
    bplib_os_unlock(channels[channel].data_bundle_lock);

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_load -
 *-------------------------------------------------------------------------------------*/
int bplib_load(int channel, void** bundle, int* size, int timeout, uint16_t* loadflags)
{
    int status = BP_SUCCESS; /* size of bundle returned or error code */

    /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(bundle == NULL || size == NULL)         return BP_PARMERR;

    /* Set Short Cuts */
    bp_channel_t*           ch          = &channels[channel];
    bp_store_dequeue_t      dequeue     = ch->storage.dequeue;
    bp_store_retrieve_t     retrieve    = ch->storage.retrieve;
    bp_store_relinquish_t   relinquish  = ch->storage.relinquish;

    /* Setup State */
    uint32_t                sysnow      = bplib_os_systime();   /* get current system time (used for timeouts, seconds) */
    bp_bundle_store_t*      ds          = NULL;                 /* start out assuming nothing to send */
    int                     store       = -1;                   /* handle for storage of bundle being loaded */
    bp_sid_t                sid         = BP_SID_VACANT;        /* storage id points to nothing */
    int                     ati         = -1;                   /* active table index */
    bool                    newcid      = true;                 /* whether to assign new custody id and active table entry */

    /* Check DACS Period */
    bplib_os_lock(ch->dacs_bundle_lock);
    {
        dacs_check(&ch->dacs, ch->dacs_rate, sysnow, BP_CHECK, ch->storage.enqueue, ch->dacs_store_handle, loadflags);
    }
    bplib_os_unlock(ch->dacs_bundle_lock);

    /* Check if DACS Needs to be Sent */
    if(dequeue(ch->dacs_store_handle, (void**)&ds, NULL, &sid, BP_CHECK) == BP_SUCCESS)
    {
        /* Send DACS - (ds is not null) */
        store = ch->dacs_store_handle;

        /* Set Route Flag */
        *loadflags |= BP_FLAG_ROUTENEEDED;
    }
    else
    {
        /* Send Data - (if ds is set below) */
        store = ch->data_store_handle;
    }

    /* Process Active Table for Timeouts */
    bplib_os_lock(ch->active_table_signal);
    {
        /* Try to Send Timed-out Bundle */
        while((ds == NULL) && (ch->active_table.oldest_cid < ch->active_table.current_cid))
        {
            ati = ch->active_table.oldest_cid % ch->attributes.active_table_size;
            sid = ch->active_table.sid[ati];
            if(sid == BP_SID_VACANT) /* entry vacant */
            {
                ch->active_table.oldest_cid++;
            }
            else if(retrieve(ch->data_store_handle, (void**)&ds, NULL, sid, BP_CHECK) == BP_SUCCESS)
            {
                if(ds->exprtime != 0 && sysnow >= ds->exprtime) /* check lifetime */
                {
                    /* Bundle Expired - Clear Entry */
                    relinquish(ch->data_store_handle, sid);
                    ch->active_table.sid[ati] = BP_SID_VACANT;
                    ch->active_table.oldest_cid++;
                    ch->stats.expired++;
                    ds = NULL;
                }
                else if(ch->timeout != 0 && sysnow >= (ch->active_table.retx[ati] + ch->timeout)) /* check timeout */
                {
                    /* Retransmit Bundle */
                    ch->active_table.oldest_cid++;
                    ch->stats.retransmitted++;

                    /* Handle Active Table and Custody ID */
                    if(ch->cid_reuse)
                    {
                        /* Set flag to reuse custody id and active table entry, 
                         * active table entry is not cleared, since CID is being reused */
                        newcid = false;
                    }
                    else
                    {
                        /* Clear Entry (it will be reinserted below at the current CID) */
                        ch->active_table.sid[ati] = BP_SID_VACANT;
                    }
                }
                else /* oldest active bundle still active */
                {
                    /* Bundle Not Ready to Retransmit */
                    ds = NULL;

                    /* Check Active Table Has Room
                     * Since next step is to dequeue from storage, need to make
                     * sure that there is room in the active table since we don't
                     * want to dequeue a bundle from storage and have no place
                     * to put it.  Note that it is possible that even if the
                     * active table was full, if the bundle dequeued did not
                     * request custody transfer it could still go out, but the
                     * current design requires that at least one slot in the active
                     * table is open at all times. */
                    ati = ch->active_table.current_cid % ch->attributes.active_table_size;
                    sid = ch->active_table.sid[ati];
                    if(sid != BP_SID_VACANT) /* entry vacant */
                    {
                        *loadflags |= BP_FLAG_ACTIVETABLEWRAP;

                        if(ch->wrap_response == BP_WRAP_RESEND)
                        {
                            /* Bump Oldest Custody ID */
                            ch->active_table.oldest_cid++;
                            
                            /* Retrieve Bundle from Storage */
                            if(retrieve(ch->data_store_handle, (void**)&ds, NULL, sid, BP_CHECK) != BP_SUCCESS)
                            {
                                /* Failed to Retrieve - Clear Entry (and loop again) */
                                relinquish(ch->data_store_handle, sid);
                                ch->active_table.sid[ati] = BP_SID_VACANT;
                                *loadflags |= BP_FLAG_STOREFAILURE;
                                ch->stats.lost++;
                            }
                            else
                            {
                                /* Force Retransmit - Do Not Reuse Custody ID */
                                ch->stats.retransmitted++;
                                bplib_os_waiton(ch->active_table_signal, BP_DEFAULT_WRAP_TIMEOUT);
                            }
                        }
                        else if(ch->wrap_response == BP_WRAP_BLOCK)
                        {
                            /* Custody ID Wrapped Around to Occupied Slot */                            
                            status = BP_OVERFLOW;                   
                            bplib_os_waiton(ch->active_table_signal, BP_DEFAULT_WRAP_TIMEOUT);
                        }
                        else /* if(ch->wrap_response == BP_WRAP_DROP) */
                        {
                            /* Bump Oldest Custody ID */
                            ch->active_table.oldest_cid++;

                            /* Clear Entry (and loop again) */
                            relinquish(ch->data_store_handle, sid);
                            ch->active_table.sid[ati] = BP_SID_VACANT;
                            ch->stats.lost++;
                        }
                    }

                    /* Break Out of Loop */
                    break;
                }
            }
            else
            {
                /* Failed to Retrieve Bundle from Storage */
                relinquish(ch->data_store_handle, sid);
                ch->active_table.sid[ati] = BP_SID_VACANT;
                *loadflags |= BP_FLAG_STOREFAILURE;
                ch->stats.lost++;
            }
        }
    }
    bplib_os_unlock(ch->active_table_signal);

    /* Try to Send Stored Bundle (if nothing ready to send yet) */
    while(ds == NULL)
    {
        /* Dequeue Bundle from Storage Service */
        int deq_status = dequeue(ch->data_store_handle, (void**)&ds, NULL, &sid, timeout);
        if(deq_status == BP_SUCCESS)
        {
            if(ds->exprtime != 0 && sysnow >= ds->exprtime)
            {
                /* Bundle Expired Clear Entry (and loop again) */
                relinquish(ch->data_store_handle, sid);
                ch->stats.expired++;
                sid = BP_SID_VACANT;
                ds = NULL;
            }
        }
        else if(deq_status == BP_TIMEOUT)
        {
            /* No Bundles in Storage to Send */
            status = BP_TIMEOUT;
            break;
        }
        else
        {
            /* Failed Storage Service */
            status = BP_FAILEDSTORE;
            *loadflags |= BP_FLAG_STOREFAILURE;
            break;
        }
    }

    /* Process Active Table for Sending Next Bundle */
    bplib_os_lock(ch->active_table_signal);
    {
        /* Check if Bundle Ready to Transmit */
        if(ds != NULL)
        {
            /* Check Buffer Size */
            if(*bundle == NULL || *size >= ds->bundlesize)
            {
                /* Check/Allocate Bundle Memory */
                if(*bundle == NULL) *bundle = malloc(ds->bundlesize);
                    
                /* Successfully Load Bundle to Application and Relinquish Memory */                
                if(*bundle != NULL)
                {
                    /* If Custody Transfer */
                    if(ds->cteboffset != 0)
                    {
                        /* Assign Custody ID and Active Table Entry */
                        if(newcid)
                        {
                            ati = ch->active_table.current_cid % ch->attributes.active_table_size;
                            ch->active_table.sid[ati] = sid;
                            ds->cidsdnv.value = ch->active_table.current_cid++;
                            sdnv_write(&ds->header[ds->cteboffset], ds->bundlesize - ds->cteboffset, ds->cidsdnv, loadflags);
                        }

                        /* Update Retransmit Time */
                        ch->active_table.retx[ati] = sysnow;
                    }

                    /* Load Bundle */
                    memcpy(*bundle, ds->header, ds->bundlesize);
                    *size = ds->bundlesize;
                    ch->stats.transmitted++;
                    status = BP_SUCCESS;

                    /* If No Custody Transfer - Free Bundle Memory */
                    if(ds->cteboffset == 0)
                    {
                        relinquish(store, sid);
                    }
                }
                else
                {
                    status = bplog(BP_FAILEDMEM, "Unable to acquire memory for bundle of size %d\n", ds->bundlesize);
                    relinquish(store, sid);
                    ch->stats.lost++;                    
                }
            }
            else
            {
                status = bplog(BP_BUNDLETOOLARGE, "Bundle too large to fit inside buffer (%d %d)\n", *size, ds->bundlesize);
                relinquish(store, sid);
                ch->stats.lost++;
            }
        }

        /* Update Active Statistic */
        ch->stats.active = ch->active_table.current_cid - ch->active_table.oldest_cid;
    }
    bplib_os_unlock(ch->active_table_signal);

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_process -
 *-------------------------------------------------------------------------------------*/
int bplib_process(int channel, void* bundle, int size, int timeout, uint16_t* procflags)
{
    int                 status;
    int                 enstat;
    bp_channel_t*       ch;

    uint32_t            sysnow;

    uint8_t*            buffer = (uint8_t*)bundle;
    int                 index = 0;

    int                 ei = 0;
    int                 exclude[BP_NUM_EXCLUDE_REGIONS];

    bp_blk_pri_t        pri_blk;

    bool                acknowledge = false;
    bool                cteb_present = false;
    int                 cteb_index = 0;
    bp_blk_cteb_t       cteb_blk;

    bool                bib_present = false;
    int                 bib_index = 0;
    bp_blk_bib_t        bib_blk;

    int                 pay_index = 0;
    bp_blk_pay_t        pay_blk;

    /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(bundle == NULL)                         return BP_PARMERR;

    /* Count Reception */
    ch = &channels[channel];
    ch->stats.received++;

    /* Parse Primary Block */
    exclude[ei++] = index;
    status = pri_read(buffer, size, &pri_blk, true);
    if(status <= 0) return bplog(status, "Failed to parse primary block of size %d\n", size);
    else            index += status;
    exclude[ei++] = index;

    /* Check Unsupported */
    if(pri_blk.dictlen.value != 0)
    {
        *procflags |= BP_FLAG_NONCOMPLIANT;
        return bplog(BP_UNSUPPORTED, "Unsupported bundle attempted to be processed (%d)\n", pri_blk.dictlen.value);
    }

    /* Check Life Time */
    sysnow = bplib_os_systime();
    if((pri_blk.lifetime.value != 0) && (sysnow >= (pri_blk.lifetime.value + pri_blk.createsec.value)))
    {
        ch->stats.expired++;
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
            cteb_present = true;
            cteb_index = index;
            if(pri_blk.request_custody) exclude[ei++] = index;
            status = cteb_read(&buffer[cteb_index], size - cteb_index, &cteb_blk, true);
            if(status <= 0) return bplog(status, "Failed to parse CTEB block at offset %d\n", cteb_index);
            else            index += status;
            if(pri_blk.request_custody) exclude[ei++] = index;
        }
        else if(blk_type == BP_BIB_BLK_TYPE)
        {
            bib_present = true;
            bib_index = index;
            exclude[ei++] = index;
            status = bib_read(&buffer[bib_index], size - bib_index, &bib_blk, true);
            if(status <= 0) return bplog(status, "Failed to parse BIB block at offset %d\n", bib_index);
            else            index += status;
            exclude[ei++] = index;
        }
        else if(blk_type != BP_PAY_BLK_TYPE) /* skip over block */
        {
            bp_sdnv_t blk_flags = { 0, 1, 0 };
            bp_sdnv_t blk_len = { 0, 0, 0 };
            int start_index = index;
            int data_index = 0; /* start of the block after the block length, set below */
            
            blk_len.index = sdnv_read(&buffer[start_index], size - start_index, &blk_flags, procflags);
            data_index = sdnv_read(&buffer[start_index], size - start_index, &blk_len, procflags);

            /* Check Parsing Status */
            if(*procflags & (BP_FLAG_SDNVOVERFLOW | BP_FLAG_SDNVINCOMPLETE))
            {
                return bplog(BP_BUNDLEPARSEERR, "Failed (%0X) to parse block at index %d\n", *procflags, start_index);
            }
            else
            {
                index += data_index + blk_len.value;
            }

            /* Mark Processing as Incomplete */
            *procflags |= BP_FLAG_INCOMPLETE;
            bplog(BP_UNSUPPORTED, "Skipping over unrecognized block of type 0x%02X and size %d\n", blk_type, blk_len.value);

            /* Should transmit status report that block cannot be processed */
            if(blk_flags.value & BP_BLK_NOTIFYNOPROC_MASK) *procflags |= BP_FLAG_NONCOMPLIANT;

            /* Delete bundle since block not recognized */
            if(blk_flags.value & BP_BLK_DELETENOPROC_MASK)
            {
                return bplog(BP_DROPPED, "Dropping bundle with unrecognized block\n");
            }

            /* Drop block since it cannot be processed */
            if(blk_flags.value & BP_BLK_DROPNOPROC_MASK)
            {
                exclude[ei++] = start_index;
                exclude[ei++] = index;
            }

            /* Mark As Forwarded without Processed */
            blk_flags.value |= BP_BLK_FORWARDNOPROC_MASK;
            sdnv_write(&buffer[start_index], size - start_index, blk_flags, procflags);
        }
        else /* payload block */
        {
            pay_index = index;
            exclude[ei++] = index; /* start of payload header */
            status = pay_read(&buffer[pay_index], size - pay_index, &pay_blk, true);
            if(status <= 0) return bplog(status, "Failed (%d) to read payload block\n", status);
            else            index += status;
            exclude[ei++] = index + pay_blk.paysize;

            /* Perform Integrity Check */
            if(bib_present)
            {
                status = bib_verify(pay_blk.payptr, pay_blk.paysize, &bib_blk);
                if(status <= 0) return bplog(status, "Bundle failed integrity check\n");
            }

            /* Check Size of Payload */
            if(pri_blk.is_admin_rec && pay_blk.paysize < 2)
            {
                return bplog(BP_BUNDLEPARSEERR, "Invalid block length: %d\n", pay_blk.paysize);
            }

            /* Process Payload */
            if(pri_blk.dstnode.value != ch->local_node) /* forward bundle (dst node != local node) */
            {
                /* Check Ability to Forward */
                if(ch->data_bundle.originate)
                {
                    status = bplog(BP_WRONGORIGINATION, "Unable to forward bundle on an originating channel\n");
                }
                else if(pay_blk.paysize > ch->data_bundle.maxlength)
                {
                    /* Check Ability to Fragment */
                    if(!pri_blk.allow_frag) status = bplog(BP_BUNDLETOOLARGE, "Unable (%d) to fragment forwarded bundle (%d > %d)\n", BP_UNSUPPORTED, pay_blk.paysize, ch->data_bundle.maxlength);
                    else                    pri_blk.is_frag = true;
                }

                /* Lock Data Bundle */
                bplib_os_lock(ch->data_bundle_lock);
                {
                    /* Initialize Forwarded Bundle */
                    if(status == BP_SUCCESS) status = initialize_forw_bundle(&ch->data_bundle, &pri_blk, &pay_blk, ch->local_node, ch->local_service, cteb_present, buffer, exclude, ei, procflags);

                    /* Store Forwarded Bundle */
                    if(status == BP_SUCCESS) status = store_data_bundle(&ch->data_bundle, ch->storage.enqueue, ch->data_store_handle, timeout, procflags);
                }
                bplib_os_unlock(ch->data_bundle_lock);

                /* Handle Custody Transfer */
                if(status == BP_SUCCESS && pri_blk.request_custody)
                {
                    if(cteb_present)
                    {
                        acknowledge = true;
                    }
                    else
                    {
                        *procflags |= BP_FLAG_NONCOMPLIANT;
                        bplog(BP_UNSUPPORTED, "Only aggregate custody supported\n");
                    }
                }
            }
            else if((ch->local_service != 0) && (pri_blk.dstserv.value != ch->local_service))
            {
                status = bplog(BP_WRONGCHANNEL, "Wrong channel to service bundle (%lu, %lu)\n", (unsigned long)pri_blk.dstserv.value, (unsigned long)ch->local_service);
            }
            else if(pri_blk.is_admin_rec) /* Administrative Record */
            {
                /* Read Record Information */
                uint32_t rec_type = buffer[index];

                /* Lock Active Table */
                bplib_os_lock(ch->active_table_signal);
                {
                    /* Process Record */
                    if(rec_type == BP_ACS_REC_TYPE)
                    {
                        int acknowledgment_count = 0;
                        dacs_process(&buffer[index], size - index, &acknowledgment_count, ch->active_table.sid, ch->attributes.active_table_size, ch->storage.relinquish, ch->data_store_handle, procflags);
                        if(acknowledgment_count > 0)
                        {
                            ch->stats.acknowledged += acknowledgment_count;
                            bplib_os_signal(ch->active_table_signal);
                        }
                    }
                    else if(rec_type == BP_CS_REC_TYPE)     status = bplog(BP_UNSUPPORTED, "Custody signal bundles are not supported\n");
                    else if(rec_type == BP_STAT_REC_TYPE)   status = bplog(BP_UNSUPPORTED, "Status report bundles are not supported\n");
                    else                                    status = bplog(BP_UNKNOWNREC, "Unknown administrative record: %u\n", (unsigned int)rec_type);
                }
                bplib_os_unlock(ch->active_table_signal);
            }
            else if(ch->proc_admin_only)
            {
                status = bplog(BP_IGNORE, "Non-administrative bundle ignored\n");
            }
            else /* deliver bundle payload to application */
            {
                bp_payload_store_t* pay = &ch->data_bundle.payload_store;
                pay->payloadsize = size - index;

                /* Set Custody Transfer Request */
                pay->request_custody = false;
                if(pri_blk.request_custody)
                {
                    if(cteb_present)
                    {
                        pay->request_custody = true;
                    }
                    else
                    {
                        *procflags |= BP_FLAG_NONCOMPLIANT;
                        bplog(BP_UNSUPPORTED, "Only aggregate custody supported\n");
                    }
                }

                /* Enqueue Payload into Storage */
                enstat = ch->storage.enqueue(ch->payload_store_handle, pay, sizeof(bp_payload_store_t), &buffer[index], pay->payloadsize, timeout);
                if(enstat > 0)
                {
                    /* Acknowledge Custody */
                    if(pay->request_custody)
                    {
                        acknowledge = true;
                    }
                }
                else
                {
                    status = bplog(BP_FAILEDSTORE, "Failed (%d) to store payload\n", enstat);
                }
            }

            /* Acknowledge Custody Transfer - Update DACS */
            if(acknowledge)
            {
                bplib_os_lock(ch->dacs_bundle_lock);
                {
                    status = dacs_acknowledge(&ch->dacs, &cteb_blk, sysnow, BP_CHECK, ch->storage.enqueue, ch->dacs_store_handle, procflags);
                }
                bplib_os_unlock(ch->dacs_bundle_lock);
            }

            /* Stop Processing Bundle Once Payload Block Reached */
            break;
        }
    }

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_accept -
 *
 *  Returns success if payload copied, or error code (zero, negative)
 *-------------------------------------------------------------------------------------*/
int bplib_accept(int channel, void** payload, int* size, int timeout, uint16_t* acptflags)
{
    (void)acptflags;
    
    int status, deqstat;;

    /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(payload == NULL || size == NULL)        return BP_PARMERR;

    /* Set Shortcuts */
    bp_channel_t*           ch          = &channels[channel];
    bp_store_dequeue_t      dequeue     = ch->storage.dequeue;
    bp_store_relinquish_t   relinquish  = ch->storage.relinquish;
    uint8_t*                storebuf    = NULL;
    int                     storelen    = 0;
    bp_sid_t                sid         = BP_SID_VACANT;

    /* Dequeue Payload from Storage */
    deqstat = dequeue(ch->payload_store_handle, (void**)&storebuf, &storelen, &sid, timeout);
    if(deqstat > 0)
    {
        /* Access Payload */
        uint8_t*            payptr      = &storebuf[sizeof(bp_payload_store_t)];
        bp_payload_store_t* paystore    = (bp_payload_store_t*)storebuf;
        int                 paylen      = paystore->payloadsize;

        /* Return Payload to Application */
        if(*payload == NULL || *size >= paylen)
        {
            /* Check/Allocate Memory for Payload */
            if(*payload == NULL) *payload = malloc(paylen);
            
            /* Copy Payload and Set Status */
            if(*payload != NULL)
            {
                memcpy(*payload, payptr, paylen);
                *size = paylen;
                ch->stats.delivered++;
                status = BP_SUCCESS;
            }
            else
            {
                status = bplog(BP_FAILEDMEM, "Unable to acquire memory for payload of size %d\n", paylen);
                ch->stats.lost++;
            }
        }
        else
        {
            status = bplog(BP_PAYLOADTOOLARGE, "Payload too large to fit inside buffer (%d %d)\n", *size, paylen);
            ch->stats.lost++;
        }

        /* Relinquish Memory */
        relinquish(ch->payload_store_handle, sid);
    }
    else 
    {
        status = deqstat;
    }

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_routeinfo -
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
    status = pri_read(buffer, size, &pri_blk, true);
    if(status <= 0) return status;

    /* Set Addresses */
    if(destination_node)    *destination_node = (bp_ipn_t)pri_blk.dstnode.value;
    if(destination_service) *destination_service = (bp_ipn_t)pri_blk.dstserv.value;

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_eid2ipn -
 *
 *  eid -                   null-terminated string representation of End Point ID [INPUT]
 *  len -                   size in bytes of above string [INPUT]
 *  node -                  node number as read from eid [OUTPUT]
 *  service -               service number as read from eid [OUTPUT]
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_eid2ipn(const char* eid, int len, bp_ipn_t* node, bp_ipn_t* service)
{
    char eidtmp[BP_MAX_EID_STRING];
    int tmplen;
    char* node_ptr;
    char* service_ptr;
    char* endptr;
    unsigned long node_result;
    unsigned long service_result;

    /* Sanity Check EID Pointer */
    if(eid == NULL)
    {
        return bplog(BP_INVALIDEID, "EID is null\n");
    }

    /* Sanity Check Length of EID */
    if(len < 7)
    {
        return bplog(BP_INVALIDEID, "EID must be at least 7 characters, act: %d\n", len);
    }
    else if(len > BP_MAX_EID_STRING)
    {
        return bplog(BP_INVALIDEID, "EID cannot exceed %d bytes in length, act: %d\n", BP_MAX_EID_STRING, len);
    }

    /* Check IPN Scheme */
    if(eid[0] != 'i' || eid[1] != 'p' || eid[2] != 'n' || eid[3] != ':')
    {
        return bplog(BP_INVALIDEID, "EID (%s) must start with 'ipn:'\n", eid);
    }

    /* Copy EID to Temporary Buffer and Set Pointers */
    tmplen = len - 4;
    memcpy(eidtmp, &eid[4], tmplen);
    eidtmp[tmplen] = '\0';
    node_ptr = eidtmp;
    service_ptr = strchr(node_ptr, '.');
    if(service_ptr != NULL)
    {
        *service_ptr = '\0';
        service_ptr++;
    }
    else
    {
        return bplog(BP_INVALIDEID, "Unable to find dotted notation in EID (%s)\n", eid);
    }

    /* Parse Node Number */
    errno = 0;
    node_result = strtoul(node_ptr, &endptr, 10); /* assume IPN node and service numbers always written in base 10 */
    if( (endptr == node_ptr) ||
        ((node_result == ULONG_MAX || node_result == 0) && errno == ERANGE) )
    {
        return bplog(BP_INVALIDEID, "Unable to parse EID (%s) node number\n", eid);
    }

    /* Parse Service Number */
    errno = 0;
    service_result = strtoul(service_ptr, &endptr, 10); /* assume IPN node and service numbers always written in base 10 */
    if( (endptr == service_ptr) ||
        ((service_result == ULONG_MAX || service_result == 0) && errno == ERANGE) )
    {
        return bplog(BP_INVALIDEID, "Unable to parse EID (%s) service number\n", eid);
    }

    /* Set Outputs */
    *node = (uint32_t)node_result;
    *service = (uint32_t)service_result;
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_ipn2eid -
 *
 *  eid -                   buffer that will hold null-terminated string representation of End Point ID [OUTPUT]
 *  len -                   size in bytes of above buffer [INPUT]
 *  node -                  node number to be written into eid [INPUT]
 *  service -               service number to be written into eid [INPUT]
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_ipn2eid(char* eid, int len, bp_ipn_t node, bp_ipn_t service)
{
    /* Sanity Check EID Buffer Pointer */
    if(eid == NULL)
    {
        return bplog(BP_INVALIDEID, "EID buffer is null\n");
    }

    /* Sanity Check Length of EID Buffer */
    if(len < 7)
    {
        return bplog(BP_INVALIDEID, "EID buffer must be at least 7 characters, act: %d\n", len);
    }
    else if(len > BP_MAX_EID_STRING)
    {
        return bplog(BP_INVALIDEID, "EID buffer cannot exceed %d bytes in length, act: %d\n", BP_MAX_EID_STRING, len);
    }

    /* Write EID */
    bplib_os_format(eid, len, "ipn:%lu.%lu", (unsigned long)node, (unsigned long)service);

    return BP_SUCCESS;
}
