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
 TYPEDEFS
 ******************************************************************************/

/* Active Table */
typedef struct {
    bp_sid_t            sid;    /* storage id */
    bp_val_t            retx;   /* retransmit time */
    bp_val_t            cid;    /* custody id */
} bp_active_bundle_t;

/* Channel Control Block */
typedef struct {
    bp_store_t          store;
    bp_attr_t           attributes;    
    bp_bundle_t         bundle;
    bp_custody_t        custody;
    int                 bundle_handle;
    int                 payload_handle;
    int                 record_handle;
    bp_val_t            oldest_active_cid;
    bp_val_t            current_active_cid;
    int                 active_table_signal;
    bp_active_bundle_t* active_table;
    bp_stats_t          stats;
} bp_channel_t;

/******************************************************************************
 CONSTANT DATA
 ******************************************************************************/

static const bp_attr_t default_attributes = {
    .lifetime               = BP_DEFAULT_LIFETIME,
    .request_custody        = BP_DEFAULT_REQUEST_CUSTODY,
    .admin_record           = BP_DEFAULT_ADMIN_RECORD,
    .integrity_check        = BP_DEFAULT_INTEGRITY_CHECK,
    .allow_fragmentation    = BP_DEFAULT_ALLOW_FRAGMENTATION,
    .cipher_suite           = BP_DEFAULT_CIPHER_SUITE,
    .timeout                = BP_DEFAULT_TIMEOUT,
    .max_length             = BP_DEFAULT_MAX_LENGTH,
    .wrap_response          = BP_DEFAULT_WRAP_RESPONSE,
    .cid_reuse              = BP_DEFAULT_CID_REUSE,
    .dacs_rate              = BP_DEFAULT_DACS_RATE,
    .active_table_size      = BP_DEFAULT_ACTIVE_TABLE_SIZE,
    .max_fills_per_dacs     = BP_DEFAULT_MAX_FILLS_PER_DACS,
    .max_gaps_per_dacs      = BP_DEFAULT_MAX_GAPS_PER_DACS,
    .storage_service_parm   = BP_DEFAULT_STORAGE_SERVICE_PARM
};

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * generate
 *-------------------------------------------------------------------------------------*/
static int generate(void* parm, bool is_record, uint8_t* payload, int size, int timeout)
{
    bp_channel_t*       ch      = (bp_channel_t*)parm;
    int                 status  = BP_ERROR;
    bp_bundle_data_t*   data    = NULL;
    int                 handle  = BP_INVALID_HANDLE;
    
    /* Set Type of Bundle */
    if(is_record)
    {
        handle = ch->record_handle;
        data = &ch->custody.bundle.data;        
    }
    else /* data bundle */
    {
        handle = ch->bundle_handle;
        data = &ch->bundle.data;        
    }

    /* Enqueue Bundle */
    int storage_header_size = &data->header[data->headersize] - (uint8_t*)data;
    status = ch->store.enqueue(handle, data, storage_header_size, payload, size, timeout);

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * acknowledge
 *-------------------------------------------------------------------------------------*/
static int acknowledge(void* parm, bp_val_t cid)
{
    bp_channel_t*   ch      = (bp_channel_t*)parm;
    int             status  = BP_FAILEDRESPONSE;
    int             ati     = cid % ch->attributes.active_table_size;
    bp_sid_t        sid     = ch->active_table[ati].sid;

    /* Check Active Table Entry */
    if(sid != BP_SID_VACANT)
    {
        if(ch->active_table[ati].cid == cid)
        {
            /* Free Bundle */
            status = ch->store.relinquish(ch->bundle_handle, sid);
            ch->active_table[ati].sid = BP_SID_VACANT;
        }
    }

    /* Return Status */
    return status;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_init - initializes bp library
 *-------------------------------------------------------------------------------------*/
void bplib_init(void)
{
    /* Initialize OS Interface */
    bplib_os_init();
    
    /* Initialize the Bundle Integrity Block Module */
    bib_init();
}

/*--------------------------------------------------------------------------------------
 * bplib_open -
 *-------------------------------------------------------------------------------------*/
bp_desc_t bplib_open(bp_route_t route, bp_store_t store, bp_attr_t* attributes)
{
    assert(store.create);
    assert(store.destroy);
    assert(store.enqueue);
    assert(store.dequeue);
    assert(store.retrieve);
    assert(store.release);
    assert(store.relinquish);
    assert(store.getcount);
    
    /* Allocate Channel */
    bp_channel_t* ch = (bp_channel_t*)malloc(sizeof(bp_channel_t));
    if(ch == NULL)
    {
        bplog(BP_FAILEDMEM, "Cannot open channel: not enough memory\n");
        return NULL;
    }

    /* Clear Channel Memory and Initialize to Defaults */
    memset(ch, 0, sizeof(bp_channel_t));
    ch->active_table_signal = BP_INVALID_HANDLE;
    ch->bundle_handle       = BP_INVALID_HANDLE;
    ch->payload_handle      = BP_INVALID_HANDLE;
    ch->record_handle       = BP_INVALID_HANDLE;

    /* Set Store */
    ch->store = store;
    
    /* Set Initial Attributes */
    if(attributes)  ch->attributes = *attributes;
    else            ch->attributes = default_attributes;

    /* Initialize Bundle Store */
    ch->bundle_handle = ch->store.create(ch->attributes.storage_service_parm);
    if(ch->bundle_handle < 0)
    {
        bplib_close(ch);
        bplog(BP_FAILEDSTORE, "Failed to create storage handle for bundles\n");
        return NULL;
    }
    
    /* Initialize Payload Store */
    ch->payload_handle = ch->store.create(ch->attributes.storage_service_parm);
    if(ch->payload_handle < 0)
    {
        bplib_close(ch);
        bplog(BP_FAILEDSTORE, "Failed to create storage handle for payloads\n");
        return NULL;
    }

    /* Initialize Record Store */
    ch->record_handle = ch->store.create(ch->attributes.storage_service_parm);
    if(ch->record_handle < 0)
    {
        bplib_close(ch);
        bplog(BP_FAILEDSTORE, "Failed to create storage handle for records\n");
        return NULL;
    }

    /* Initialize Bundle and Custody Modules 
     *  NOTE: this must occur first and together so that future calls to 
     *  un-initialize are safe to make.
     */
    uint16_t flags = 0;
    int bundle_status = bundle_initialize(&ch->bundle, route, &ch->attributes, &flags);
    int custody_status = custody_initialize(&ch->custody, route, &ch->attributes, &flags);
    if(bundle_status != BP_SUCCESS || custody_status != BP_SUCCESS)
    {
        bplog(BP_ERROR, "Failed to initialize channel, flags=%0X\n", flags);
        bplib_close(ch);
        return NULL;
    }

    /* Initialize Active Table */
    ch->active_table_signal = bplib_os_createlock();
    if(ch->active_table_signal < 0)
    {
        bplib_close(ch);
        bplog(BP_FAILEDOS, "Failed to create lock for active table\n");
        return NULL;
    }

    /* Allocate Memory for Active Table */
    ch->active_table = (bp_active_bundle_t*)malloc(sizeof(bp_active_bundle_t) * ch->attributes.active_table_size);
    if(ch->active_table == NULL)
    {
        bplib_close(ch);
        bplog(BP_FAILEDMEM, "Failed to allocate memory for channel active table\n");
        return NULL;
    }
    else
    {
        memset(ch->active_table, 0, sizeof(bp_active_bundle_t) * ch->attributes.active_table_size);
    }

    /* Initialize Data */
    ch->oldest_active_cid   = 1;
    ch->current_active_cid  = 1;
    
    /* Return Channel */
    return ch;
}

/*--------------------------------------------------------------------------------------
 * bplib_close -
 *-------------------------------------------------------------------------------------*/
void bplib_close(bp_desc_t channel)
{
    /* Check Parameters */
    if(channel == NULL) return;
    
    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;
    
    /* Un-initialize Active Table */
    if(ch->active_table_signal != BP_INVALID_HANDLE) bplib_os_destroylock(ch->active_table_signal);
    if(ch->active_table) free(ch->active_table);

    /* Un-initialize Bundle Store */
    if(ch->bundle_handle >= 0)
    {
        ch->store.destroy(ch->bundle_handle);
        ch->bundle_handle = BP_INVALID_HANDLE;
    }
    
    /* Un-initialize Payload Store */
    if(ch->payload_handle >= 0)
    {
        ch->store.destroy(ch->payload_handle);
        ch->payload_handle = BP_INVALID_HANDLE;
    }

    /* Un-initialize Record Store */
    if(ch->record_handle >= 0)
    {
        ch->store.destroy(ch->record_handle);
        ch->record_handle = BP_INVALID_HANDLE;
    }

    /* Un-initialize Bundle and Custody Modules */
    custody_uninitialize(&ch->custody);
    
    /* Free Channel */
    free(ch);
}

/*--------------------------------------------------------------------------------------
 * bplib_flush -
 *-------------------------------------------------------------------------------------*/
int bplib_flush(bp_desc_t channel)
{
    /* Check Parameters */
    if(channel == NULL) return BP_PARMERR;
    
    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;

    /* Flush Data Bundles */
    int handle = ch->bundle_handle;

    /* Lock Active Table */
    bplib_os_lock(ch->active_table_signal);
    {
        /* Relinquish All Active Bundles */
        while(ch->oldest_active_cid != ch->current_active_cid)
        {
            /* Get Storage ID of Oldest Active Bundle */
            int ati = ch->oldest_active_cid % ch->attributes.active_table_size;
            bp_sid_t sid = ch->active_table[ati].sid;

            /* Relinquish Bundle */
            if(sid != BP_SID_VACANT)
            {
                ch->store.relinquish(handle, sid);
                ch->active_table[ati].sid = BP_SID_VACANT;
                ch->stats.lost++;
            }

            /* Go To Next Active Table Entry */
            ch->oldest_active_cid++;
        }
    }
    bplib_os_unlock(ch->active_table_signal);

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_config -
 *-------------------------------------------------------------------------------------*/
int bplib_config(bp_desc_t channel, int mode, int opt, void* val, int len)
{
    /* Check Parameters */
    if(channel == NULL)     return BP_PARMERR;
    else if(val == NULL)    return BP_PARMERR;
    
    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;

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
        case BP_OPT_REQUEST_CUSTODY:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(setopt && *enable != true && *enable != false) return BP_PARMERR;
            if(setopt)  ch->attributes.request_custody = *enable;
            else        *enable = ch->attributes.request_custody;
            break;
        }
        case BP_OPT_ADMIN_RECORD:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(setopt && *enable != true && *enable != false) return BP_PARMERR;
            if(setopt)  ch->attributes.admin_record = *enable;
            else        *enable = ch->attributes.admin_record;
            break;
        }
        case BP_OPT_INTEGRITY_CHECK:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(setopt && *enable != true && *enable != false) return BP_PARMERR;
            if(setopt)  ch->attributes.integrity_check = *enable;
            else        *enable = ch->attributes.integrity_check;
            break;
        }
        case BP_OPT_ALLOW_FRAGMENTATION:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(setopt && *enable != true && *enable != false) return BP_PARMERR;
            if(setopt)  ch->attributes.allow_fragmentation = *enable;
            else        *enable = ch->attributes.allow_fragmentation;
            break;
        }
        case BP_OPT_CIPHER_SUITE:
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
        case BP_OPT_MAX_LENGTH:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* maxlen = (int*)val;
            if(setopt)  ch->attributes.max_length = *maxlen;
            else        *maxlen = ch->attributes.max_length;
            break;
        }
        case BP_OPT_WRAP_RESPONSE:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* wrap = (int*)val;
            if(setopt && *wrap != BP_WRAP_RESEND && *wrap != BP_WRAP_BLOCK && *wrap != BP_WRAP_DROP) return BP_PARMERR;
            if(setopt)  ch->attributes.wrap_response = *wrap;
            else        *wrap = ch->attributes.wrap_response;
            break;
        }
        case BP_OPT_CID_REUSE:
        {
            if(len != sizeof(int)) return BP_PARMERR;
            int* enable = (int*)val;
            if(setopt && *enable != true && *enable != false) return BP_PARMERR;
            if(setopt)  ch->attributes.cid_reuse = *enable;
            else        *enable = ch->attributes.cid_reuse;
            break;
        }
        case BP_OPT_DACS_RATE:
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
int bplib_latchstats(bp_desc_t channel, bp_stats_t* stats)
{
     /* Check Parameters */
    if(channel == NULL)     return BP_PARMERR;
    else if(stats == NULL)  return BP_PARMERR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;

    /* Update Store Counts */
    ch->stats.bundles = ch->store.getcount(ch->bundle_handle);
    ch->stats.payloads = ch->store.getcount(ch->payload_handle);
    ch->stats.records = ch->store.getcount(ch->record_handle);
    
    /* Update Active Statistic */
    ch->stats.active = ch->current_active_cid - ch->oldest_active_cid;

    /* Latch Statistics */
    *stats = ch->stats;

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_store -
 *-------------------------------------------------------------------------------------*/
int bplib_store(bp_desc_t channel, void* payload, int size, int timeout, uint16_t* flags)
{
    int status;

     /* Check Parameters */
    if(channel == NULL)         return BP_PARMERR;
    else if(payload == NULL)    return BP_PARMERR;
    else if(flags == NULL)  return BP_PARMERR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;

    /* Lock Data Bundle */
    status = bundle_send(&ch->bundle, true, payload, size, generate, ch, timeout, flags);
    if(status == BP_SUCCESS) ch->stats.generated++;

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_load -
 *-------------------------------------------------------------------------------------*/
int bplib_load(bp_desc_t channel, void** bundle, int* size, int timeout, uint16_t* flags)
{
    int status = BP_SUCCESS; /* success or error code */

    /* Check Parameters */
    if(channel == NULL)     return BP_PARMERR;
    else if(bundle == NULL) return BP_PARMERR;
    else if(flags == NULL)  return BP_PARMERR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;
    
    /* Setup State */
    unsigned long   sysnow  = 0;                /* current system time used for timeouts (seconds) */
    bp_object_t*    object  = NULL;             /* stare out assuming nothing to send */
    bp_sid_t        sid     = BP_SID_VACANT;    /* store id points to nothing */
    int             ati     = -1;               /* active table index */
    bool            newcid  = true;             /* whether to assign new custody id and active table entry */
    int             handle  = -1;               /* handle for store service being loaded */

    /* Get Current Time */
    if(bplib_os_systime(&sysnow) == BP_OS_ERROR)
    {
        *flags |= BP_FLAG_UNRELIABLETIME;
    }
    
    /*-------------------------*/
    /* Try to Send DACS Bundle */
    /*-------------------------*/
    if(object == NULL)
    {
        /* Store any DACS currently being accumulated */
        custody_send(&ch->custody, ch->attributes.dacs_rate, sysnow, generate, ch, BP_CHECK, flags);

        /* Dequeue any stored DACS */
        handle = ch->record_handle;    
        if(ch->store.dequeue(handle, &object, BP_CHECK) == BP_SUCCESS)
        {
            sid = object->sid;
            *flags |= BP_FLAG_ROUTENEEDED;
        }
    }
    
    /*------------------------------------------------*/
    /* Try to Send Active Bundle (if nothing to send) */
    /*------------------------------------------------*/
    if(object == NULL)
    {
        /* Send Data Bundle */
        handle = ch->bundle_handle;

        /* Process Active Table for Timeouts */
        bplib_os_lock(ch->active_table_signal);
        {
            /* Try to Send Timed-out Bundle */
            while((object == NULL) && (ch->oldest_active_cid != ch->current_active_cid))
            {
                ati = ch->oldest_active_cid % ch->attributes.active_table_size;
                sid = ch->active_table[ati].sid;
                if(sid == BP_SID_VACANT) /* entry vacant */
                {
                    ch->oldest_active_cid++;
                }
                else if(ch->store.retrieve(handle, sid, &object, BP_CHECK) == BP_SUCCESS)
                {
                    bp_bundle_data_t* data = (bp_bundle_data_t*)object->data;
                    if(data->exprtime != 0 && sysnow >= data->exprtime) /* check lifetime */
                    {
                        /* Bundle Expired - Clear Entry */
                        ch->store.relinquish(handle, sid);
                        ch->active_table[ati].sid = BP_SID_VACANT;
                        ch->oldest_active_cid++;
                        ch->stats.expired++;
                        object = NULL;
                    }
                    else if(ch->attributes.timeout != 0 && sysnow >= (ch->active_table[ati].retx + ch->attributes.timeout)) /* check timeout */
                    {
                        /* Retransmit Bundle */
                        ch->oldest_active_cid++;
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
                            ch->active_table[ati].sid = BP_SID_VACANT;
                        }
                    }
                    else /* oldest active bundle still active */
                    {
                        /* Bundle Not Ready to Retransmit */
                        object = NULL;

                        /* Check Active Table Has Room
                         * Since next step is to dequeue from store, need to make
                         * sure that there is room in the active table since we don't
                         * want to dequeue a bundle from store and have no place
                         * to put it.  Note that it is possible that even if the
                         * active table was full, if the bundle dequeued did not
                         * request custody transfer it could still go out, but the
                         * current design requires that at least one slot in the active
                         * table is open at all times. */
                        ati = ch->current_active_cid % ch->attributes.active_table_size;
                        sid = ch->active_table[ati].sid;
                        if(sid != BP_SID_VACANT) /* entry vacant */
                        {
                            *flags |= BP_FLAG_ACTIVETABLEWRAP;

                            if(ch->attributes.wrap_response == BP_WRAP_RESEND)
                            {
                                /* Bump Oldest Custody ID */
                                ch->oldest_active_cid++;

                                /* Retrieve Bundle from Storage */
                                if(ch->store.retrieve(handle, sid, &object, BP_CHECK) != BP_SUCCESS)
                                {
                                    /* Failed to Retrieve - Clear Entry (and loop again) */
                                    ch->store.relinquish(handle, sid);
                                    ch->active_table[ati].sid = BP_SID_VACANT;
                                    *flags |= BP_FLAG_STOREFAILURE;
                                    ch->stats.lost++;
                                }
                                else
                                {
                                    /* Force Retransmit - Do Not Reuse Custody ID */
                                    ch->stats.retransmitted++;
                                    bplib_os_waiton(ch->active_table_signal, timeout);
                                }
                            }
                            else if(ch->attributes.wrap_response == BP_WRAP_BLOCK)
                            {
                                /* Custody ID Wrapped Around to Occupied Slot */                            
                                status = BP_OVERFLOW;                   
                                bplib_os_waiton(ch->active_table_signal, timeout);
                            }
                            else /* if(ch->wrap_response == BP_WRAP_DROP) */
                            {
                                /* Bump Oldest Custody ID */
                                ch->oldest_active_cid++;

                                /* Clear Entry (and loop again) */
                                ch->store.relinquish(handle, sid);
                                ch->active_table[ati].sid = BP_SID_VACANT;
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
                    ch->store.relinquish(handle, sid);
                    ch->active_table[ati].sid = BP_SID_VACANT;
                    *flags |= BP_FLAG_STOREFAILURE;
                    ch->stats.lost++;
                }
            }
        }
        bplib_os_unlock(ch->active_table_signal);
    }

    /*------------------------------------------------*/
    /* Try to Send Stored Bundle (if nothing to send) */
    /*------------------------------------------------*/
    while(object == NULL)
    {
        /* Dequeue Bundle from Storage Service */
        int deq_status = ch->store.dequeue(handle, &object, timeout);
        if(deq_status == BP_SUCCESS)
        {
            bp_bundle_data_t* data = (bp_bundle_data_t*)object->data;
            sid = object->sid;

            if(data->exprtime != 0 && sysnow >= data->exprtime)
            {
                /* Bundle Expired Clear Entry (and loop again) */
                ch->store.relinquish(handle, sid);
                ch->stats.expired++;
                sid = BP_SID_VACANT;
                object = NULL;
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
    
    /*------------------------------*/
    /* Load Bundle if Ready to Send */
    /*------------------------------*/
    if(object != NULL)
    {
        bp_bundle_data_t* data = (bp_bundle_data_t*)object->data;

        /* Check Custody Transfer */
        if(data->cteboffset != 0)
        {
            /* Mark Bundle as Active */
            bplib_os_lock(ch->active_table_signal);
            {
                /* Assign Custody ID and Active Table Entry */
                if(newcid)
                {
                    ati = ch->current_active_cid % ch->attributes.active_table_size;
                    ch->active_table[ati].sid = sid;
                    ch->active_table[ati].cid = ch->current_active_cid;
                    bundle_update(data, ch->current_active_cid++, flags);
                }

                /* Update Retransmit Time */
                ch->active_table[ati].retx = sysnow;
            }
            bplib_os_unlock(ch->active_table_signal);
        }

        /* Load Bundle */
        *bundle = data->header;
        if(size) *size = data->bundlesize;
        ch->stats.transmitted++;
    }

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_process -
 *-------------------------------------------------------------------------------------*/
int bplib_process(bp_desc_t channel, void* bundle, int size, int timeout, uint16_t* flags)
{
    int status;

    /* Check Parameters */
    if(channel == NULL)     return BP_PARMERR;
    else if(bundle == NULL) return BP_PARMERR;
    else if(flags == NULL)  return BP_PARMERR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;

    /* Count Reception */
    ch->stats.received++;
    
    /* Get Time */
    unsigned long sysnow = 0;
    if(bplib_os_systime(&sysnow) == BP_OS_ERROR)
    {
        *flags |= BP_FLAG_UNRELIABLETIME;
    }

    /* Receive Bundle */
    bp_custodian_t custodian;
    bool custody_transfer = false;
    status = bundle_receive(&ch->bundle, bundle, size, sysnow, &custodian, flags);
    if(status == BP_EXPIRED)
    {
        ch->stats.expired++;
    }
    else if(status == BP_PENDINGACKNOWLEDGMENT)
    {
        /* Process Aggregate Custody Signal - Process DACS */
        bplib_os_lock(ch->active_table_signal);
        {
            int num_acks = custody_acknowledge(&ch->custody, &custodian, acknowledge, (void*)ch, flags);
            if(num_acks > 0)
            {
                ch->stats.acknowledged += num_acks;
                bplib_os_signal(ch->active_table_signal);
                status = BP_SUCCESS;
            }
        }
        bplib_os_unlock(ch->active_table_signal);
    }
    else if(status == BP_PENDINGACCEPTANCE)
    {
        /* Store Payload */
        status = ch->store.enqueue(ch->payload_handle, custodian.rec, custodian.rec_size, NULL, 0, timeout);
        if(status == BP_SUCCESS && custodian.node != BP_IPN_NULL)
        {
            custody_transfer = true;
        }
        else if(status != BP_SUCCESS)
        {
            *flags |= BP_FLAG_STOREFAILURE;
        }
    }
    else if(status == BP_PENDINGFORWARD)
    {
        /* Store Forwarded Bundle */
        status = bundle_send(&ch->bundle, false, custodian.rec, custodian.rec_size, generate, ch, timeout, flags);
        if(status == BP_SUCCESS && custodian.node != BP_IPN_NULL)
        {
            custody_transfer = true;
        }
    }

    /* Acknowledge Custody Transfer - Update DACS */
    if(custody_transfer)
    {
        status = custody_receive(&ch->custody, &custodian, sysnow, generate, ch, BP_CHECK, flags);
    }
    
    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_accept -
 *
 *  Returns success if payload copied, or error code (zero, negative)
 *-------------------------------------------------------------------------------------*/
int bplib_accept(bp_desc_t channel, void** payload, int* size, int timeout, uint16_t* flags)
{
    (void)flags;
    
    int status = BP_SUCCESS;
    bp_object_t* object = NULL;

    /* Check Parameters */
    if(channel == NULL)         return BP_PARMERR;
    else if(payload == NULL)    return BP_PARMERR;
    else if(flags == NULL)      return BP_PARMERR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;

    /* Dequeue Payload from Storage */
    status = ch->store.dequeue(ch->payload_handle, &object, timeout);
    if(status > 0)
    {
        /* Return Payload to Application */
        *payload = object->data;
        if(size) *size = object->size;
        ch->stats.delivered++;
    }

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_ackbundle -
 *-------------------------------------------------------------------------------------*/
int bplib_ackbundle(bp_desc_t channel, void* bundle)
{
    int status = BP_SUCCESS;
    bp_channel_t* ch = (bp_channel_t*)channel;
    bp_bundle_data_t* data = (bp_bundle_data_t*)((uint8_t*)bundle - offsetof(bp_bundle_data_t, header));
    bp_object_t* object = (bp_object_t*)((uint8_t*)data - offsetof(bp_object_t, data));

    /* Check Parameters */
    if(channel == NULL)     return BP_PARMERR;
    else if(bundle == NULL) return BP_PARMERR;

    /* Release Memory */
    ch->store.release(object->handle, object->sid);
    
    /* Free Memory - only when no custody transfer is requested */
    if(data->cteboffset == 0) ch->store.relinquish(object->handle, object->sid);

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_ackpayload -
 *-------------------------------------------------------------------------------------*/
int bplib_ackpayload(bp_desc_t channel, void* payload)
{
    int status = BP_SUCCESS;
    bp_channel_t* ch = (bp_channel_t*)channel;
    bp_object_t* object = (bp_object_t*)((uint8_t*)payload - offsetof(bp_object_t, data));

    /* Check Parameters */
    if(channel == NULL)         return BP_PARMERR;
    else if(payload == NULL)    return BP_PARMERR;

    /* Release Memory */
    ch->store.release(object->handle, object->sid);
    
    /* Free Memory */
    ch->store.relinquish(object->handle, object->sid);

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
    *node = (bp_ipn_t)node_result;
    *service = (bp_ipn_t)service_result;
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
