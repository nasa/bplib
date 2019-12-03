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
#include "v6.h"
#include "rb_tree.h"
#include "bundle_types.h"
#include "cbuf.h"
#include "rh_hash.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Table Functions */
typedef int (*bp_table_create_t)    (void** table, int size);
typedef int (*bp_table_destroy_t)   (void* table);
typedef int (*bp_table_add_t)       (void* table, bp_active_bundle_t bundle, bool overwrite);
typedef int (*bp_table_next_t)      (void* table, bp_active_bundle_t* bundle);
typedef int (*bp_table_remove_t)    (void* table, bp_val_t cid, bp_active_bundle_t* bundle);
typedef int (*bp_table_available_t) (void* table, bp_val_t cid);
typedef int (*bp_table_count_t)     (void* table);

/* Active Table */
typedef struct {
    void*                   table; 
    bp_table_create_t       create;
    bp_table_destroy_t      destroy;
    bp_table_add_t          add;
    bp_table_next_t         next;
    bp_table_remove_t       remove;
    bp_table_available_t    available;
    bp_table_count_t        count;
} bp_active_table_t;

/* Channel Control Block */
typedef struct {
    bp_store_t              store;
    bp_attr_t               attributes;  
    bp_attr_t               custody_attributes;
    bp_stats_t              stats;
    bp_bundle_t             bundle;
    bp_bundle_t             custody;
    int                     bundle_handle;
    int                     payload_handle;
    int                     record_handle;
    uint8_t*                dacs_buffer;
    int                     dacs_size;
    bp_val_t                dacs_last_sent;
    int                     custody_tree_lock;
    rb_tree_t               custody_tree;
    bp_val_t                current_active_cid;
    int                     active_table_signal;
    bp_active_table_t       active_table;
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
    .cid_reuse              = BP_DEFAULT_CID_REUSE,
    .dacs_rate              = BP_DEFAULT_DACS_RATE,
    .protocol_version       = BP_DEFAULT_PROTOCOL_VERSION,
    .retransmit_order       = BP_DEFAULT_RETRANSMIT_ORDER,
    .active_table_size      = BP_DEFAULT_ACTIVE_TABLE_SIZE,
    .max_fills_per_dacs     = BP_DEFAULT_MAX_FILLS_PER_DACS,
    .max_gaps_per_dacs      = BP_DEFAULT_MAX_GAPS_PER_DACS,
    .storage_service_parm   = BP_DEFAULT_STORAGE_SERVICE_PARM
};

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * create_bundle
 *-------------------------------------------------------------------------------------*/
static int create_bundle(void* parm, bool is_record, uint8_t* payload, int size, int timeout)
{
    bp_channel_t*       ch      = (bp_channel_t*)parm;
    int                 status  = BP_ERROR;
    bp_bundle_data_t*   data    = NULL;
    int                 handle  = BP_INVALID_HANDLE;
    
    /* Set Type of Bundle */
    if(is_record)
    {
        handle = ch->record_handle;
        data = &ch->custody.data;        
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
 * remove_bundle
 *-------------------------------------------------------------------------------------*/
static int remove_bundle(void* parm, bp_val_t cid)
{
    bp_channel_t* ch = (bp_channel_t*)parm;

    bp_active_bundle_t bundle;
    int status = ch->active_table.remove(ch->active_table.table, cid, &bundle); 
    if(status == BP_SUCCESS)
    {
        status = ch->store.relinquish(ch->bundle_handle, bundle.sid);
    }
    
    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * create_custody -
 *
 *  Notes:
 *-------------------------------------------------------------------------------------*/
static int create_custody(bp_channel_t* ch, unsigned long sysnow, int timeout, uint16_t* flags)
{
    int ret_status = BP_SUCCESS;

    /* If the custody_tree has nodes, initialize the iterator for traversing the custody_tree in order */
    rb_tree_goto_first(&ch->custody_tree);

    /* Continue to delete nodes from the custody_tree and write them to DACS until the custody_tree is empty */
    while (!rb_tree_is_empty(&ch->custody_tree))
    {
        /* Build Acknowledgment - will remove nodes from the custody_tree */
        int size = v6_populate_acknowledgment(ch->dacs_buffer, ch->dacs_size, ch->attributes.max_fills_per_dacs, &ch->custody_tree, flags);
        if(size > 0)
        {
            int status = BP_SUCCESS;
            
            /* Check if Re-initialization Needed */
            if(ch->custody.prebuilt == false)
            {
                status = v6_populate_bundle(&ch->custody, flags);
            }

            /* Send Bundle */
            if(status == BP_SUCCESS)
            {
                status = v6_send_bundle(&ch->custody, ch->dacs_buffer, size, create_bundle, ch, timeout, flags);
                if(status == BP_SUCCESS)
                {
                    /* DACS successfully enqueued */
                    ch->dacs_last_sent = sysnow;
                }
                else if(ret_status == BP_SUCCESS) 
                {
                    /* Save first failed DACS enqueue to return later */
                    ret_status = status;
                    *flags |= BP_FLAG_STOREFAILURE; 
                    bplog(status, "Failed to store DACS for transmission, bundle dropped\n");
                }
            }            
        }
    }

    /* Return Status */
    return ret_status;
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
}

/*--------------------------------------------------------------------------------------
 * bplib_open -
 *-------------------------------------------------------------------------------------*/
bp_desc_t bplib_open(bp_route_t route, bp_store_t store, bp_attr_t attributes)
{
    assert(store.create);
    assert(store.destroy);
    assert(store.enqueue);
    assert(store.dequeue);
    assert(store.retrieve);
    assert(store.release);
    assert(store.relinquish);
    assert(store.getcount);

    uint16_t flags = 0;
    int status = BP_SUCCESS;

    /* Allocate Channel */
    bp_channel_t* ch = (bp_channel_t*)malloc(sizeof(bp_channel_t));
    if(ch == NULL)
    {
        bplog(BP_FAILEDMEM, "Cannot open channel: not enough memory\n");
        return NULL;
    }

    /* Clear Channel Memory and Initialize to Defaults */
    memset(ch, 0, sizeof(bp_channel_t));
    ch->custody_tree_lock   = BP_INVALID_HANDLE;
    ch->active_table_signal = BP_INVALID_HANDLE;
    ch->bundle_handle       = BP_INVALID_HANDLE;
    ch->payload_handle      = BP_INVALID_HANDLE;
    ch->record_handle       = BP_INVALID_HANDLE;

    /* Set Store */
    ch->store = store;
    
    /* Set Initial Attributes */
    ch->attributes = attributes;

    /* Check Protocol Version */
    if(ch->attributes.protocol_version != 6)
    {
        bplog(BP_UNSUPPORTED, "Unsupported bundle protocol version: %d\n", ch->attributes.protocol_version);
        bplib_close(ch);
        return NULL;
    }

    /* Initialize Bundle Store */
    ch->bundle_handle = ch->store.create(ch->attributes.storage_service_parm);
    if(ch->bundle_handle < 0)
    {
        bplog(BP_FAILEDSTORE, "Failed to create storage handle for bundles\n");
        bplib_close(ch);
        return NULL;
    }
    
    /* Initialize Payload Store */
    ch->payload_handle = ch->store.create(ch->attributes.storage_service_parm);
    if(ch->payload_handle < 0)
    {
        bplog(BP_FAILEDSTORE, "Failed to create storage handle for payloads\n");
        bplib_close(ch);
        return NULL;
    }

    /* Initialize Record Store */
    ch->record_handle = ch->store.create(ch->attributes.storage_service_parm);
    if(ch->record_handle < 0)
    {
        bplog(BP_FAILEDSTORE, "Failed to create storage handle for records\n");
        bplib_close(ch);
        return NULL;
    }

    /* Initialize Bundle */
    status = v6_initialize(&ch->bundle, route, &ch->attributes, &flags);
    if(status != BP_SUCCESS)
    {
        bplog(status, "Failed to initialize bundle, flags=%0X\n", flags);
        bplib_close(ch);
        return NULL;
    }
    
    /* Build Custody Attributes */
    ch->custody_attributes = ch->attributes;
    ch->custody_attributes.request_custody = false;
    ch->custody_attributes.admin_record = true;

    /* Build Custody Route */
    bp_route_t custody_route = route;
    custody_route.destination_node = BP_IPN_NULL;
    custody_route.destination_service = BP_IPN_NULL;

    /* Initialize Custody */    
    status = v6_initialize(&ch->custody, custody_route, &ch->custody_attributes, &flags);
    if(status != BP_SUCCESS)
    {
        bplog(status, "Failed to initialize custody, flags=%0X\n", flags);
        bplib_close(ch);
        return NULL;
    }
    
    /* Create DACS Lock */
    ch->custody_tree_lock = bplib_os_createlock();
    if(ch->custody_tree_lock == BP_INVALID_HANDLE)
    {
        bplog(BP_FAILEDOS, "Failed to create a lock for custody processing\n");
        bplib_close(ch);
        return NULL;
    }

    /* Allocate Memory for Channel DACS Bundle Fills */
    ch->dacs_size = sizeof(bp_val_t) * ch->attributes.max_fills_per_dacs + 6; // 2 bytes per fill plus payload block header
    ch->dacs_buffer = (uint8_t*)malloc(ch->dacs_size); 
    if(ch->dacs_buffer == NULL)
    {
        bplog(BP_FAILEDMEM, "Failed to allocate memory for channel DACS\n");                    
        bplib_close(ch);
        return NULL;
    }

    /* Initialize DACS Buffer to Zeroes */
    memset(ch->dacs_buffer, 0, ch->dacs_size);

    /* Allocate Memory for DACS Channel Tree to Store Bundle IDs */
    status = rb_tree_create(ch->attributes.max_gaps_per_dacs, &ch->custody_tree);
    if(status != BP_SUCCESS)
    {
        bplog(BP_FAILEDMEM, "Failed to allocate memory for channel DACS tree\n");
        bplib_close(ch);
        return NULL;
    }

    /* Initialize Last Time DACS Sent */
    ch->dacs_last_sent = 0;

    /* Initialize Active Table Signal */
    ch->active_table_signal = bplib_os_createlock();
    if(ch->active_table_signal < 0)
    {
        bplog(BP_FAILEDOS, "Failed to create custody_tree_lock for active table\n");
        bplib_close(ch);
        return NULL;
    }

    /* Initialize Active Table Functions */
    if(ch->attributes.retransmit_order == BP_RETX_SMALLEST_CID)
    {
        ch->active_table.create     = (bp_table_create_t)cbuf_create;
        ch->active_table.destroy    = (bp_table_destroy_t)cbuf_destroy;
        ch->active_table.add        = (bp_table_add_t)cbuf_add;
        ch->active_table.next       = (bp_table_next_t)cbuf_next;
        ch->active_table.remove     = (bp_table_remove_t)cbuf_remove;
        ch->active_table.available  = (bp_table_available_t)cbuf_available;
        ch->active_table.count      = (bp_table_count_t)cbuf_count;
    }
    else if(ch->attributes.retransmit_order == BP_RETX_OLDEST_BUNDLE)
    {
        ch->active_table.create     = (bp_table_create_t)rh_hash_create;
        ch->active_table.destroy    = (bp_table_destroy_t)rh_hash_destroy;
        ch->active_table.add        = (bp_table_add_t)rh_hash_add;
        ch->active_table.next       = (bp_table_next_t)rh_hash_next;
        ch->active_table.remove     = (bp_table_remove_t)rh_hash_remove;
        ch->active_table.available  = (bp_table_available_t)rh_hash_available;
        ch->active_table.count      = (bp_table_count_t)rh_hash_count;
    }
    else
    {
        bplog(BP_UNSUPPORTED, "Unrecognized attribute for creating active table: %d\n", ch->attributes.retransmit_order);
        bplib_close(ch);
        return NULL;        
    }
    

    /* Initialize Active Table */
    status = ch->active_table.create(&ch->active_table.table, ch->attributes.active_table_size);
    if(status != BP_SUCCESS)
    {
        bplog(status, "Failed to create active table for channel\n");
        bplib_close(ch);
        return NULL;
    }

    /* Initialize Current Custody ID */
    ch->current_active_cid  = 0;
    
    /* Return Channel */
    return ch;
}

/*--------------------------------------------------------------------------------------
 * bplib_close -
 *-------------------------------------------------------------------------------------*/
void bplib_close(bp_desc_t channel)
{
    /* Check Parameters */
    if(channel == BP_INVALID_DESCRIPTOR) return;
    
    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;
    
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

    /* Destroy Custody Tree Lock */
    if(ch->custody_tree_lock != BP_INVALID_HANDLE)
    {
        bplib_os_destroylock(ch->custody_tree_lock);
        ch->custody_tree_lock = BP_INVALID_HANDLE;
    }

    /* Free Buffer for DACS */
    if(ch->dacs_buffer)
    {
        free(ch->dacs_buffer);
        ch->dacs_buffer = NULL;
    }

    /* Free Custody Tree */
    rb_tree_destroy(&ch->custody_tree);

    /* Un-initialize Bundle and Custody Modules */
    v6_uninitialize(&ch->bundle);
    v6_uninitialize(&ch->custody);
    
    /* Un-initialize Active Table */
    if(ch->active_table_signal != BP_INVALID_HANDLE) bplib_os_destroylock(ch->active_table_signal);
    ch->active_table.destroy(ch->active_table.table);

    /* Free Channel */
    free(ch);
}

/*--------------------------------------------------------------------------------------
 * bplib_flush -
 *-------------------------------------------------------------------------------------*/
int bplib_flush(bp_desc_t channel)
{
    /* Check Parameters */
    if(channel == BP_INVALID_DESCRIPTOR) return BP_PARMERR;
    
    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;

    /* Flush Data Bundles */
    int handle = ch->bundle_handle;

    /* Lock Active Table */
    bplib_os_lock(ch->active_table_signal);
    {
        /* Relinquish All Active Bundles */
        bp_active_bundle_t active_bundle;
        while(ch->active_table.next(ch->active_table.table, &active_bundle) == BP_SUCCESS)
        {
            ch->active_table.remove(ch->active_table.table, active_bundle.cid, NULL);
            ch->store.relinquish(handle, active_bundle.sid);
            ch->stats.lost++;
        }
    }
    bplib_os_unlock(ch->active_table_signal);

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_config -
 *-------------------------------------------------------------------------------------*/
int bplib_config(bp_desc_t channel, int mode, int opt, int* val)
{
    /* Check Parameters */
    if(channel == BP_INVALID_DESCRIPTOR)    return BP_PARMERR;
    else if(val == NULL)                    return BP_PARMERR;
    
    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;

    /* Set Mode */
    bool setopt = mode == BP_OPT_MODE_WRITE ? true : false;
    
    /* Select and Process Option */
    switch(opt)
    {
        case BP_OPT_LIFETIME:
        {
            if(setopt)  ch->attributes.lifetime = *val;
            else        *val = ch->attributes.lifetime;
            break;
        }
        case BP_OPT_REQUEST_CUSTODY:
        {
            if(setopt && *val != true && *val != false) return BP_PARMERR;
            if(setopt)  ch->attributes.request_custody = *val;
            else        *val = ch->attributes.request_custody;
            break;
        }
        case BP_OPT_ADMIN_RECORD:
        {
            if(setopt && *val != true && *val != false) return BP_PARMERR;
            if(setopt)  ch->attributes.admin_record = *val;
            else        *val = ch->attributes.admin_record;
            break;
        }
        case BP_OPT_INTEGRITY_CHECK:
        {
            if(setopt && *val != true && *val != false) return BP_PARMERR;
            if(setopt)  ch->attributes.integrity_check = *val;
            else        *val = ch->attributes.integrity_check;
            break;
        }
        case BP_OPT_ALLOW_FRAGMENTATION:
        {
            if(setopt && *val != true && *val != false) return BP_PARMERR;
            if(setopt)  ch->attributes.allow_fragmentation = *val;
            else        *val = ch->attributes.allow_fragmentation;
            break;
        }
        case BP_OPT_CIPHER_SUITE:
        {
            if(setopt)  ch->attributes.cipher_suite = *val;
            else        *val = ch->attributes.cipher_suite;
            break;
        }
        case BP_OPT_TIMEOUT:
        {
            if(setopt)  ch->attributes.timeout = *val;
            else        *val = ch->attributes.timeout;
            break;
        }
        case BP_OPT_MAX_LENGTH:
        {
            if(setopt && *val < 0) return BP_PARMERR;
            if(setopt)  ch->attributes.max_length = *val;
            else        *val = ch->attributes.max_length;
            break;
        }
        case BP_OPT_CID_REUSE:
        {
            if(setopt && *val != true && *val != false) return BP_PARMERR;
            if(setopt)  ch->attributes.cid_reuse = *val;
            else        *val = ch->attributes.cid_reuse;
            break;
        }
        case BP_OPT_DACS_RATE:
        {
            if(setopt)  ch->attributes.dacs_rate = *val;
            else        *val = ch->attributes.dacs_rate;
            break;
        }
        default:
        {
            /* Option Not Found */
            return bplog(BP_PARMERR, "Config. Option Not Found (%d)\n", opt);
        }
    }

    /* Re-initialize Bundles */
    if(setopt) ch->bundle.prebuilt = false;

    /* Return Status */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_latchstats -
 *-------------------------------------------------------------------------------------*/
int bplib_latchstats(bp_desc_t channel, bp_stats_t* stats)
{
     /* Check Parameters */
    if(channel == BP_INVALID_DESCRIPTOR)    return BP_PARMERR;
    else if(stats == NULL)                  return BP_PARMERR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;

    /* Update Store Counts */
    ch->stats.bundles = ch->store.getcount(ch->bundle_handle);
    ch->stats.payloads = ch->store.getcount(ch->payload_handle);
    ch->stats.records = ch->store.getcount(ch->record_handle);
    
    /* Update Active Statistic */
    ch->stats.active = ch->active_table.count(ch->active_table.table);

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
    int status = BP_SUCCESS;

     /* Check Parameters */
    if(channel == BP_INVALID_DESCRIPTOR)    return BP_PARMERR;
    else if(payload == NULL)                return BP_PARMERR;
    else if(flags == NULL)                  return BP_PARMERR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;

    /* Check if Re-initialization Needed */
    if(ch->bundle.prebuilt == false)
    {
        status = v6_populate_bundle(&ch->bundle, flags);
    }

    /* Send Bundle */
    if(status == BP_SUCCESS)
    {
        status = v6_send_bundle(&ch->bundle, payload, size, create_bundle, ch, timeout, flags);
        if(status == BP_SUCCESS)
        {
            ch->stats.generated++;
        }
    }
    
    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_load -
 *-------------------------------------------------------------------------------------*/
int bplib_load(bp_desc_t channel, void** bundle, int* size, int timeout, uint16_t* flags)
{
    bp_active_bundle_t active_bundle = { BP_SID_VACANT, 0, 0 };
    int status = BP_SUCCESS; /* success or error code */

    /* Check Parameters */
    if(channel == BP_INVALID_DESCRIPTOR)    return BP_PARMERR;
    else if(bundle == NULL)                 return BP_PARMERR;
    else if(flags == NULL)                  return BP_PARMERR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;
    
    /* Setup State */
    unsigned long   sysnow  = 0;                /* current system time used for timeouts (seconds) */
    bp_object_t*    object  = NULL;             /* stare out assuming nothing to send */
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
        /* Timeout Any Pending Acknowledgments */
        if(ch->attributes.dacs_rate > 0)
        {
            /* Check If Custody Ready to Send */
            bplib_os_lock(ch->custody_tree_lock);
            {
                if( (sysnow >= (ch->dacs_last_sent + ch->attributes.dacs_rate)) && 
                    !rb_tree_is_empty(&ch->custody_tree) )
                {
                    create_custody(ch, sysnow, BP_CHECK, flags);
                }
            }
            bplib_os_unlock(ch->custody_tree_lock);
        }

        /* Dequeue any stored DACS */
        handle = ch->record_handle;    
        if(ch->store.dequeue(handle, &object, BP_CHECK) == BP_SUCCESS)
        {
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
            /* Get Oldest Bundle */
            while(ch->active_table.next(ch->active_table.table, &active_bundle) == BP_SUCCESS)
            {
                /* Retrieve Oldest Bundle */
                if(ch->store.retrieve(handle, active_bundle.sid, &object, BP_CHECK) == BP_SUCCESS)
                {
                    bp_bundle_data_t* data = (bp_bundle_data_t*)object->data;
                    if(data->exprtime != 0 && sysnow >= data->exprtime) /* check lifetime */
                    {
                        /* Bundle Expired */
                        object = NULL;

                        /* Clear Entry in Active Table and Storage */
                        ch->active_table.remove(ch->active_table.table, active_bundle.cid, NULL);
                        ch->store.relinquish(handle, active_bundle.sid);
                        ch->stats.expired++;
                    }
                    else if(ch->attributes.timeout != 0 && sysnow >= (active_bundle.retx + ch->attributes.timeout)) /* check timeout */
                    {
                        /* Retransmit Bundle */
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
                            ch->active_table.remove(ch->active_table.table, active_bundle.cid, NULL);
                            
                            /* Move to Next Oldest */
                            ch->active_table.next(ch->active_table.table, NULL);
                        }
                        
                        /* Break Out of Loop */
                        break;
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
                        status = ch->active_table.available(ch->active_table.table, ch->current_active_cid);
                        if(status != BP_SUCCESS)
                        {
                            *flags |= BP_FLAG_ACTIVETABLEWRAP;
                            bplib_os_waiton(ch->active_table_signal, timeout);
                        }

                        /* Break Out of Loop */
                        break;
                    }
                }
                else
                {
                    /* Failed to Retrieve Bundle from Storage */
                    ch->active_table.remove(ch->active_table.table, active_bundle.cid, NULL);
                    ch->store.relinquish(handle, active_bundle.sid);
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
    while(object == NULL && status == BP_SUCCESS)
    {
        /* Dequeue Bundle from Storage Service */
        int deq_status = ch->store.dequeue(handle, &object, timeout);
        if(deq_status == BP_SUCCESS)
        {
            bp_bundle_data_t* data = (bp_bundle_data_t*)object->data;

            /* Check Expiration Time */
            if(data->exprtime != 0 && sysnow >= data->exprtime)
            {
                /* Bundle Expired Clear Entry (and loop again) */
                ch->store.relinquish(handle, active_bundle.sid);
                ch->stats.expired++;
                object = NULL;
            }
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
            *flags |= BP_FLAG_STOREFAILURE;
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
                /* Save/Update Storage ID */
                active_bundle.sid = object->sid;

                /* Update Retransmit Time */
                active_bundle.retx = sysnow;
                
                /* Assign New Custody ID */
                if(newcid)
                {
                    active_bundle.cid = ch->current_active_cid++;
                    v6_update_bundle(data, active_bundle.cid, flags);
                }

                /* Update Active Table */
                status = ch->active_table.add(ch->active_table.table, active_bundle, !newcid);
                if(status == BP_DUPLICATECID) *flags |= BP_FLAG_DUPLICATES;
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
    if(channel == BP_INVALID_DESCRIPTOR)    return BP_PARMERR;
    else if(bundle == NULL)                 return BP_PARMERR;
    else if(flags == NULL)                  return BP_PARMERR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;

    /* Count Reception */
    ch->stats.received++;
    
    /* Receive Bundle */
    bp_payload_t payload;
    bool custody_transfer = false;
    status = v6_receive_bundle(&ch->bundle, bundle, size, &payload, flags);
    if(status == BP_EXPIRED)
    {
        ch->stats.expired++;
    }
    else if(status == BP_PENDINGACKNOWLEDGMENT)
    {
        /* Process Aggregate Custody Signal - Process DACS */
        bplib_os_lock(ch->active_table_signal);
        {
            int num_acks = 0;
            int bytes_read = v6_receive_acknowledgment(payload.memptr, payload.data.payloadsize, &num_acks, remove_bundle, ch, flags);            
            ch->stats.acknowledged += num_acks;

            /* Set Status */
            if(bytes_read > 0)
            {
                bplib_os_signal(ch->active_table_signal);
                status = BP_SUCCESS;
            }
            else
            {
                /* Error Code */
                status = bytes_read;
            }
        }
        bplib_os_unlock(ch->active_table_signal);
    }
    else if(status == BP_PENDINGACCEPTANCE)
    {
        /* Store Payload */
        int storage_header_size = offsetof(bp_payload_data_t, payload);
        status = ch->store.enqueue(ch->payload_handle, &payload.data, storage_header_size, payload.memptr, payload.data.payloadsize, timeout);
        if(status == BP_SUCCESS && payload.node != BP_IPN_NULL)
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
        status = v6_send_bundle(&ch->bundle, payload.memptr, payload.data.payloadsize, create_bundle, ch, timeout, flags);
        if(status == BP_SUCCESS && payload.node != BP_IPN_NULL)
        {
            custody_transfer = true;
        }
    }

    /* Acknowledge Custody Transfer - Update DACS */
    if(custody_transfer)
    {
        /* Get Time */
        unsigned long sysnow = 0;
        if(bplib_os_systime(&sysnow) == BP_OS_ERROR)
        {
            *flags |= BP_FLAG_UNRELIABLETIME;
        }

        /* Take Custody */
        bplib_os_lock(ch->custody_tree_lock);
        {
            if(ch->custody.route.destination_node == payload.node && ch->custody.route.destination_service == payload.service)
            {
                /* Insert Custody ID directly into current custody custody_tree */
                int insert_status = rb_tree_insert(payload.cid, &ch->custody_tree);
                if(insert_status == BP_CUSTODYTREEFULL) 
                {
                    /* Flag Full Tree - possibly the custody_tree size is configured to be too small */
                    *flags |= BP_FLAG_RBTREEFULL;

                    /* Store Custody Signal */
                    create_custody(ch, sysnow, BP_CHECK, flags);

                    /* Start New DACS */
                    if(rb_tree_insert(payload.cid, &ch->custody_tree) != BP_SUCCESS)
                    {
                        /* There is no valid reason for an insert to fail on an empty custody_tree */
                        status = BP_FAILEDRESPONSE;
                    }
                }
                else if(insert_status == BP_DUPLICATECID)
                {
                    /* Duplicate values are fine and are treated as a success */
                    *flags |= BP_FLAG_DUPLICATES;
                }
                else if(insert_status != BP_SUCCESS)
                {
                    /* Tree error unexpected */
                    status = BP_FAILEDRESPONSE;        
                }
            }
            else
            {
                /* Store Custody Signal */
                if(!rb_tree_is_empty(&ch->custody_tree))
                {
                    create_custody(ch, sysnow, BP_CHECK, flags);
                }

                /* Initial New DACS Bundle */
                ch->custody.route.destination_node = payload.node;
                ch->custody.route.destination_service = payload.service;
                ch->custody.prebuilt = false;

                /* Start New DACS */
                if(rb_tree_insert(payload.cid, &ch->custody_tree) != BP_SUCCESS)
                {
                    /* There is no valid reason for an insert to fail on an empty custody_tree */
                    status = BP_FAILEDRESPONSE;
                }
            }
        }
        bplib_os_unlock(ch->custody_tree_lock);
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
    if(channel == BP_INVALID_DESCRIPTOR)    return BP_PARMERR;
    else if(payload == NULL)                return BP_PARMERR;
    else if(flags == NULL)                  return BP_PARMERR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)channel;

    /* Dequeue Payload from Storage */
    status = ch->store.dequeue(ch->payload_handle, &object, timeout);
    if(status > 0)
    {
        bp_payload_data_t* data = (bp_payload_data_t*)object->data;

        /* Get Current Time */
        unsigned long sysnow = 0;
        if(bplib_os_systime(&sysnow) == BP_OS_ERROR)
        {
            *flags |= BP_FLAG_UNRELIABLETIME;
            sysnow = 0; /* prevents expiration below */
        }

        /* Check Expiration Time */
        if(data->exprtime != 0 && data->exprtime <= sysnow)
        {
            ch->store.relinquish(ch->payload_handle, object->sid);
            ch->stats.expired++;
            status = BP_EXPIRED;
        }
        else
        {
            /* Return Payload to Application */
            *payload = data->payload;
            if(size) *size = data->payloadsize;        

            /* Check for Application Acknowledgement Request */
            if(data->ackapp) status = BP_PENDINGAPPLICATION;
            
            /* Count as Delivered */
            ch->stats.delivered++;
        }
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
    if(channel == BP_INVALID_DESCRIPTOR)    return BP_PARMERR;
    else if(bundle == NULL)                 return BP_PARMERR;

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
    bp_payload_data_t* data = (bp_payload_data_t*)((uint8_t*)payload - offsetof(bp_payload_data_t, payload));
    bp_object_t* object = (bp_object_t*)((uint8_t*)data - offsetof(bp_object_t, data));

    /* Check Parameters */
    if(channel == BP_INVALID_DESCRIPTOR)    return BP_PARMERR;
    else if(payload == NULL)                return BP_PARMERR;

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
    return v6_routeinfo(bundle, size, route);
}

/*--------------------------------------------------------------------------------------
 * bplib_display -
 *
 *  bundle -                pointer to a bundle (byte array) [INPUT]
 *  size -                  size of bundle being pointed to [INPUT]
 *  flags -                 processing flags [OUTPUT]
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_display(uint8_t* buffer, int size, uint16_t* flags)
{
    return v6_display(buffer, size, flags);
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
