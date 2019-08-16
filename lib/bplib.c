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
#include "bplib_os.h"
#include "sdnv.h"
#include "pri.h"
#include "cteb.h"
#include "bib.h"
#include "pay.h"
#include "custody.h"
#include "bundle.h"
#include "crc.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#ifndef LIBID
#define LIBID                           "unversioned"
#endif

#define BP_EMPTY                        (-1)
#define BP_WRAP_TIMEOUT                 1000    /* milliseconds */

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

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
    bp_bundle_t         bundle;
    bp_custody_t        custody;
    int                 active_table_signal;
    bp_active_table_t   active_table;
    bp_stats_t          stats;
} bp_channel_t;

/******************************************************************************
 CONSTANT DATA
 ******************************************************************************/

static const bp_attr_t default_attributes = {
    .active_table_size      = BP_DEFAULT_ACTIVE_TABLE_SIZE,
    .timeout                = BP_DEFAULT_TIMEOUT,
    .dacs_rate              = BP_DEFAULT_DACS_RATE,
    .wrap_response          = BP_DEFAULT_WRAP_RESPONSE,
    .cid_reuse              = BP_DEFAULT_CID_REUSE,
    .lifetime               = BP_DEFAULT_LIFETIME,
    .request_custody        = BP_DEFAULT_REQUEST_CUSTODY,
    .allow_fragmentation    = BP_DEFAULT_ALLOW_FRAGMENTATION,
    .integrity_check        = BP_DEFAULT_INTEGRITY_CHECK,
    .cipher_suite           = BP_DEFAULT_CIPHER_SUITE,
    .maxlength              = BP_DEFAULT_BUNDLE_MAXLENGTH,
    .max_fills_per_dacs     = BP_DEFAULT_MAX_FILLS_PER_DACS,
    .max_gaps_per_dacs      = BP_DEFAULT_MAX_GAPS_PER_DACS,
    .storage_service_parm   = NULL
};

/******************************************************************************
 FILE DATA
 ******************************************************************************/

static bp_channel_t* channels;
static int channels_max;
static int channels_lock;

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
int bplib_open(bp_route_t route, bp_store_t store, bp_attr_t* attributes)
{
    assert(store.create);
    assert(store.destroy);
    assert(store.enqueue);
    assert(store.dequeue);
    assert(store.retrieve);
    assert(store.relinquish);
    assert(store.getcount);

    int i, status;
    uint16_t flags = 0;
    bp_channel_t* ch = NULL;
    
    bplib_os_lock(channels_lock);
    {
        /* Find open channel slot */
        for(i = 0; i < channels_max; i++)
        {
            if(channels[i].index == BP_EMPTY)
            {
                /* Set Channel */
                ch = &channels[i];
                
                /* Clear Channel Memory and Initialize to Defaults */
                memset(ch, 0, sizeof(bp_channel_t));
                
                /* Set Initial Attributes */
                if(attributes)  ch->attributes = *attributes;
                else            ch->attributes = default_attributes;

                /* Initialize Bundle */
                status = bundle_initialize(&ch->bundle, route, store, &ch->attributes, true, &flags);
                if(status != BP_SUCCESS) break;
                
                /* Initialize DACS */
                status = custody_initialize(&ch->custody, route, store, &ch->attributes, &flags);
                if(status != BP_SUCCESS) break;
                
                /* Initialize Active Table */
                ch->active_table_signal  = bplib_os_createlock();
                if(ch->active_table_signal < 0)
                {
                    status = bplog(BP_FAILEDOS, "Failed to create lock for active table \n");
                    break;
                }

                /* Allocate Memory for Active Table */
                ch->active_table.sid = (bp_sid_t*)malloc(sizeof(bp_sid_t) * ch->attributes.active_table_size);
                ch->active_table.retx = (uint32_t*)malloc(sizeof(uint32_t) * ch->attributes.active_table_size);
                if(ch->active_table.sid == NULL || ch->active_table.retx == NULL)
                {
                    status = bplog(BP_FAILEDMEM, "Failed to allocate memory for channel active table\n");
                    break;
                }
                else
                {
                    memset(ch->active_table.sid, 0, sizeof(bp_sid_t*) * ch->attributes.active_table_size);
                    memset(ch->active_table.retx, 0, sizeof(uint32_t) * ch->attributes.active_table_size);
                }
                
                /* Initialize Data */
                ch->active_table.oldest_cid     = 0;
                ch->active_table.current_cid    = 0;
                ch->index                       = i;

                /* Exit Loop - Success */
                break;
            }
        }
    }
    bplib_os_unlock(channels_lock);

    /* Handle Failures */
    if(ch == NULL)
    {
        bplog(BP_CHANNELSFULL, "Cannot open channel, not enough room\n");
    }
    else if(ch->index == BP_EMPTY)
    {
        /* Loop Exited Early */
        bplib_close(i);
    }
        
    /* Return Channel */
    return ch->index;
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
            if(ch->active_table_signal != BP_INVALID_HANDLE) bplib_os_destroylock(ch->active_table_signal);
            
            bundle_uninitialize(&ch->bundle);
            custody_uninitialize(&ch->custody);
                   
            if(ch->active_table.sid) free(ch->active_table.sid);
            if(ch->active_table.retx) free(ch->active_table.retx);
                    
            ch->index = BP_EMPTY;
        }
        bplib_os_unlock(channels_lock);
    }
}

/*--------------------------------------------------------------------------------------
 * bplib_config -
 *-------------------------------------------------------------------------------------*/
int bplib_config(int channel, int mode, int opt, void* val, int len)
{
    /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(val == NULL)                            return BP_PARMERR;
    
    /* Set Shortcut to Channel */
    bp_channel_t* ch = &channels[channel];

    /* Set Mode */
    bool setopt = mode == BP_OPT_MODE_WRITE ? true : false;
    
    /* Select and Process Option */
    switch(opt)
    {
        case BP_OPT_LIFETIME:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* lifetime = (int*)val;
            if(setopt)  ch->attributes.lifetime = *lifetime;
            else        *lifetime = ch->attributes.lifetime;
            break;
        }
        case BP_OPT_CSTRQST:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(setopt && *enable != true && *enable != false) return BP_PARMERR;
            if(setopt)  ch->attributes.request_custody = *enable;
            else        *enable = ch->attributes.request_custody;
            break;
        }
        case BP_OPT_ICHECK:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(setopt && *enable != true && *enable != false) return BP_PARMERR;
            if(setopt)  ch->attributes.integrity_check = *enable;
            else        *enable = ch->attributes.integrity_check;
            break;
        }
        case BP_OPT_ALLOWFRAG:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(setopt && *enable != true && *enable != false) return BP_PARMERR;
            if(setopt)  ch->attributes.allow_fragmentation = *enable;
            else        *enable = ch->attributes.allow_fragmentation;
            break;
        }
        case BP_OPT_PAYCRC:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* type = (int*)val;
            if(setopt)  ch->attributes.cipher_suite = *type;
            else        *type = ch->attributes.cipher_suite;
            break;
        }
        case BP_OPT_TIMEOUT:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* timeout = (int*)val;
            if(setopt)  ch->attributes.timeout = *timeout;
            else        *timeout = ch->attributes.timeout;
            break;
        }
        case BP_OPT_BUNDLELEN:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* maxlen = (int*)val;
            if(setopt)  ch->attributes.maxlength = *maxlen;
            else        *maxlen = ch->attributes.maxlength;
            break;
        }
        case BP_OPT_WRAPRSP:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* wrap = (int*)val;
            if(setopt && *wrap != BP_WRAP_RESEND && *wrap != BP_WRAP_BLOCK && *wrap != BP_WRAP_DROP) return BP_PARMERR;
            if(setopt)  ch->attributes.wrap_response = *wrap;
            else        *wrap = ch->attributes.wrap_response;
            break;
        }
        case BP_OPT_CIDREUSE:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(setopt && *enable != true && *enable != false) return BP_PARMERR;
            if(setopt)  ch->attributes.cid_reuse = *enable;
            else        *enable = ch->attributes.cid_reuse;
            break;
        }
        case BP_OPT_ACSRATE:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* rate = (int*)val;
            if(setopt)  ch->attributes.dacs_rate = *rate;
            else        *rate = ch->attributes.dacs_rate;
            break;
        }
        default:
        {
            /* Option Not Found */
            return bplog(BP_PARMERR, "Config. Option Not Found (%d)\n", opt);
        }
    }

    /* Re-initialize Bundles */
    if(setopt) ch->bundle.prebuilt = true;

    /* Return Status */
    return BP_SUCCESS;
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
    ch->stats.bundles = ch->bundle.store.getcount(ch->bundle.bundle_handle);
    ch->stats.payloads = ch->bundle.store.getcount(ch->bundle.payload_handle);
    ch->stats.records = ch->custody.bundle.store.getcount(ch->custody.bundle.bundle_handle);

    /* Latch Statistics */
    *stats = ch->stats;

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_store -
 *-------------------------------------------------------------------------------------*/
int bplib_store(int channel, void* payload, int size, int timeout, uint16_t* flags)
{
    int status;

    /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(payload == NULL)                        return BP_PARMERR;

    /* Set Short Cuts */
    bp_channel_t* ch = &channels[channel];

    /* Lock Data Bundle */
    status = bundle_send(&ch->bundle, payload, size, timeout, flags);
    if(status == BP_SUCCESS) ch->stats.generated++;

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_load -
 *-------------------------------------------------------------------------------------*/
int bplib_load(int channel, void** bundle, int* size, int timeout, uint16_t* flags)
{
    int status = BP_SUCCESS; /* size of bundle returned or error code */

    /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(bundle == NULL || size == NULL)         return BP_PARMERR;

    /* Set Short Cuts */
    bp_channel_t*       ch              = &channels[channel];
    
    /* Setup State */
    uint32_t            sysnow          = bplib_os_systime();   /* get current system time (used for timeouts, seconds) */
    bp_bundle_data_t*   data            = NULL;                 /* start out assuming nothing to send */
    bp_sid_t            sid             = BP_SID_VACANT;        /* store id points to nothing */
    int                 ati             = -1;                   /* active table index */
    bool                newcid          = true;                 /* whether to assign new custody id and active table entry */
    bp_store_t*         store           = NULL;                 /* which storage service to used */
    int                 handle          = -1;                   /* handle for store service being loaded */

    /* Check if DACS Needs to be Sent First */
    store = &ch->custody.bundle.store;
    handle = ch->custody.bundle.bundle_handle;    
    custody_check(&ch->custody, ch->attributes.dacs_rate, sysnow, BP_CHECK, flags);
    if(store->dequeue(handle, (void**)&data, NULL, &sid, timeout) == BP_SUCCESS)
    {
        /* Set Route Flag */
        *flags |= BP_FLAG_ROUTENEEDED;
    }
    else
    {
        /* Send Data Bundle */
        store = &ch->bundle.store;
        handle = ch->bundle.bundle_handle;

        /* Process Active Table for Timeouts */
        bplib_os_lock(ch->active_table_signal);
        {
            /* Try to Send Timed-out Bundle */
            while((data == NULL) && (ch->active_table.oldest_cid < ch->active_table.current_cid))
            {
                ati = ch->active_table.oldest_cid % ch->attributes.active_table_size;
                sid = ch->active_table.sid[ati];
                if(sid == BP_SID_VACANT) /* entry vacant */
                {
                    ch->active_table.oldest_cid++;
                }
                else if(store->retrieve(handle, (void**)&data, NULL, sid, BP_CHECK) == BP_SUCCESS)
                {
                    if(data->exprtime != 0 && sysnow >= data->exprtime) /* check lifetime */
                    {
                        /* Bundle Expired - Clear Entry */
                        store->relinquish(handle, sid);
                        ch->active_table.sid[ati] = BP_SID_VACANT;
                        ch->active_table.oldest_cid++;
                        ch->stats.expired++;
                        data = NULL;
                    }
                    else if(ch->attributes.timeout != 0 && sysnow >= (ch->active_table.retx[ati] + ch->attributes.timeout)) /* check timeout */
                    {
                        /* Retransmit Bundle */
                        ch->active_table.oldest_cid++;
                        ch->stats.retransmitted++;

                        /* Handle Active Table and Custody ID */
                        if(ch->attributes.cid_reuse)
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
                        data = NULL;

                        /* Check Active Table Has Room
                         * Since next step is to dequeue from store, need to make
                         * sure that there is room in the active table since we don't
                         * want to dequeue a bundle from store and have no place
                         * to put it.  Note that it is possible that even if the
                         * active table was full, if the bundle dequeued did not
                         * request custody transfer it could still go out, but the
                         * current design requires that at least one slot in the active
                         * table is open at all times. */
                        ati = ch->active_table.current_cid % ch->attributes.active_table_size;
                        sid = ch->active_table.sid[ati];
                        if(sid != BP_SID_VACANT) /* entry vacant */
                        {
                            *flags |= BP_FLAG_ACTIVETABLEWRAP;

                            if(ch->attributes.wrap_response == BP_WRAP_RESEND)
                            {
                                /* Bump Oldest Custody ID */
                                ch->active_table.oldest_cid++;

                                /* Retrieve Bundle from Storage */
                                if(store->retrieve(handle, (void**)&data, NULL, sid, BP_CHECK) != BP_SUCCESS)
                                {
                                    /* Failed to Retrieve - Clear Entry (and loop again) */
                                    store->relinquish(handle, sid);
                                    ch->active_table.sid[ati] = BP_SID_VACANT;
                                    *flags |= BP_FLAG_STOREFAILURE;
                                    ch->stats.lost++;
                                }
                                else
                                {
                                    /* Force Retransmit - Do Not Reuse Custody ID */
                                    ch->stats.retransmitted++;
                                    bplib_os_waiton(ch->active_table_signal, BP_WRAP_TIMEOUT );
                                }
                            }
                            else if(ch->attributes.wrap_response == BP_WRAP_BLOCK)
                            {
                                /* Custody ID Wrapped Around to Occupied Slot */                            
                                status = BP_OVERFLOW;                   
                                bplib_os_waiton(ch->active_table_signal, BP_WRAP_TIMEOUT );
                            }
                            else /* if(ch->wrap_response == BP_WRAP_DROP) */
                            {
                                /* Bump Oldest Custody ID */
                                ch->active_table.oldest_cid++;

                                /* Clear Entry (and loop again) */
                                store->relinquish(handle, sid);
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
                    store->relinquish(handle, sid);
                    ch->active_table.sid[ati] = BP_SID_VACANT;
                    *flags |= BP_FLAG_STOREFAILURE;
                    ch->stats.lost++;
                }
            }
        }
        bplib_os_unlock(ch->active_table_signal);

        /* Try to Send Stored Bundle (if nothing ready to send yet) */
        while(data == NULL)
        {
            /* Dequeue Bundle from Storage Service */
            int deq_status = store->dequeue(handle, (void**)&data, NULL, &sid, timeout);
            if(deq_status == BP_SUCCESS)
            {
                if(data->exprtime != 0 && sysnow >= data->exprtime)
                {
                    /* Bundle Expired Clear Entry (and loop again) */
                    store->relinquish(handle, sid);
                    ch->stats.expired++;
                    sid = BP_SID_VACANT;
                    data = NULL;
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
                *flags |= BP_FLAG_STOREFAILURE;
                break;
            }
        }

    }
    
    /* Load Bundle */
    bplib_os_lock(ch->active_table_signal);
    {
        /* Check if Bundle Ready to Transmit */
        if(data != NULL)
        {
            /* Check Buffer Size */
            if(*bundle == NULL || *size >= data->bundlesize)
            {
                /* Check/Allocate Bundle Memory */
                if(*bundle == NULL) *bundle = malloc(data->bundlesize);
                    
                /* Successfully Load Bundle to Application and Relinquish Memory */                
                if(*bundle != NULL)
                {
                    /* If Custody Transfer */
                    if(data->cteboffset != 0)
                    {
                        /* Assign Custody ID and Active Table Entry */
                        if(newcid)
                        {
                            ati = ch->active_table.current_cid % ch->attributes.active_table_size;
                            ch->active_table.sid[ati] = sid;
                            data->cidsdnv.value = ch->active_table.current_cid++;
                            sdnv_write(&data->header[data->cteboffset], data->bundlesize - data->cteboffset, data->cidsdnv, flags);
                        }

                        /* Update Retransmit Time */
                        ch->active_table.retx[ati] = sysnow;
                    }

                    /* Load Bundle */
                    memcpy(*bundle, data->header, data->bundlesize);
                    *size = data->bundlesize;
                    ch->stats.transmitted++;
                    status = BP_SUCCESS;

                    /* If No Custody Transfer - Free Bundle Memory */
                    if(data->cteboffset == 0)
                    {
                        store->relinquish(handle, sid);
                    }
                }
                else
                {
                    status = bplog(BP_FAILEDMEM, "Unable to acquire memory for bundle of size %d\n", data->bundlesize);
                    store->relinquish(handle, sid);
                    ch->stats.lost++;                    
                }
            }
            else
            {
                status = bplog(BP_BUNDLETOOLARGE, "Bundle too large to fit inside buffer (%d %d)\n", *size, data->bundlesize);
                store->relinquish(handle, sid);
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
int bplib_process(int channel, void* bundle, int size, int timeout, uint16_t* flags)
{
    int status;

    /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(bundle == NULL)                         return BP_PARMERR;

    /* Set Short Cuts */
    bp_channel_t* ch = &channels[channel];

    /* Count Reception */
    ch->stats.received++;
    
    /* Get Time */
    uint32_t sysnow = bplib_os_systime();

    /* Receive Bundle */
    bp_custodian_t custodian;
    status = bundle_receive(&ch->bundle, bundle, size, sysnow, &custodian, timeout, flags);
    if(status == BP_EXPIRED)
    {
        ch->stats.expired++;
    }
    else if(status == BP_PENDINGACKNOWLEDGMENT)
    {
        /* Process Aggregate Custody Signal - Process DACS */
        bplib_os_lock(ch->active_table_signal);
        {
            int acknowledgment_count = 0;
            custody_process(&ch->custody, &custodian, &acknowledgment_count, ch->active_table.sid, ch->attributes.active_table_size, flags);
            if(acknowledgment_count > 0)
            {
                ch->stats.acknowledged += acknowledgment_count;
                bplib_os_signal(ch->active_table_signal);
            }
        }
        bplib_os_unlock(ch->active_table_signal);
    }
    else if(status == BP_PENDINGCUSTODYTRANSFER)
    {
        /* Acknowledge Custody Transfer - Update DACS */
        status = custody_acknowledge(&ch->custody, &custodian, sysnow, BP_CHECK, flags);
    }
    
    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_accept -
 *
 *  Returns success if payload copied, or error code (zero, negative)
 *-------------------------------------------------------------------------------------*/
int bplib_accept(int channel, void** payload, int* size, int timeout, uint16_t* flags)
{
    (void)flags;
    
    int status, deqstat;

    /* Check Parameters */
    if(channel < 0 || channel >= channels_max)      return BP_PARMERR;
    else if(channels[channel].index == BP_EMPTY)    return BP_PARMERR;
    else if(payload == NULL || size == NULL)        return BP_PARMERR;

    /* Set Shortcuts */
    bp_channel_t*   ch      = &channels[channel];
    uint8_t*        payptr  = NULL;
    int             paylen  = 0;
    bp_sid_t        sid     = BP_SID_VACANT;

    /* Dequeue Payload from Storage */
    deqstat = ch->bundle.store.dequeue(ch->bundle.payload_handle, (void**)&payptr, &paylen, &sid, timeout);
    if(deqstat > 0)
    {
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
        ch->bundle.store.relinquish(ch->bundle.payload_handle, sid);
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
int bplib_routeinfo(void* bundle, int size, bp_route_t* route)
{
    int status;
    bp_blk_pri_t pri_blk;
    uint8_t* buffer = (uint8_t*)bundle;
    uint16_t* flags = 0;

    /* Check Parameters */
    assert(buffer);

    /* Parse Primary Block */
    status = pri_read(buffer, size, &pri_blk, true, flags);
    if(status <= 0) return status;

    /* Set Addresses */
    if(route)
    {
        route->local_node           = (bp_ipn_t)pri_blk.srcnode.value;
        route->local_service        = (bp_ipn_t)pri_blk.srcserv.value;
        route->destination_node     = (bp_ipn_t)pri_blk.dstnode.value;
        route->destination_service  = (bp_ipn_t)pri_blk.dstserv.value;
        route->report_node          = (bp_ipn_t)pri_blk.rptnode.value;
        route->report_service       = (bp_ipn_t)pri_blk.rptserv.value;
    }

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_eid2ipn -
 *
 *  eid -                   null-terminated string representation of End Point ID [INPUT]
 *  len -                   size in bytes of above string [INPUT]
 *  node -                  node number as read from eid [OUTPUT]
 *  store -               store number as read from eid [OUTPUT]
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_eid2ipn(const char* eid, int len, bp_ipn_t* node, bp_ipn_t* store)
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
    node_result = strtoul(node_ptr, &endptr, 10); /* assume IPN node and store numbers always written in base 10 */
    if( (endptr == node_ptr) ||
        ((node_result == ULONG_MAX || node_result == 0) && errno == ERANGE) )
    {
        return bplog(BP_INVALIDEID, "Unable to parse EID (%s) node number\n", eid);
    }

    /* Parse Service Number */
    errno = 0;
    service_result = strtoul(service_ptr, &endptr, 10); /* assume IPN node and store numbers always written in base 10 */
    if( (endptr == service_ptr) ||
        ((service_result == ULONG_MAX || service_result == 0) && errno == ERANGE) )
    {
        return bplog(BP_INVALIDEID, "Unable to parse EID (%s) store number\n", eid);
    }

    /* Set Outputs */
    *node = (uint32_t)node_result;
    *store = (uint32_t)service_result;
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_ipn2eid -
 *
 *  eid -                   buffer that will hold null-terminated string representation of End Point ID [OUTPUT]
 *  len -                   size in bytes of above buffer [INPUT]
 *  node -                  node number to be written into eid [INPUT]
 *  store -               store number to be written into eid [INPUT]
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_ipn2eid(char* eid, int len, bp_ipn_t node, bp_ipn_t store)
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
    bplib_os_format(eid, len, "ipn:%lu.%lu", (unsigned long)node, (unsigned long)store);

    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_attrinit -         initializes a channel attribute struct with default values
 *
 *  attr -                  pointer to attribute structure that needs to be initialized
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_attrinit(bp_attr_t* attr)
{
    if(attr)
    {
        *attr = default_attributes;
        return BP_SUCCESS;
    }
    else
    {
        return BP_PARMERR;
    }
}
