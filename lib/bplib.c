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
    /* Storage Service */
    bp_store_t              store;
    /* Statistics */
    bp_stats_t              stats;
    /* Data Bundles */
    bp_bundle_t             bundle;
    int                     bundle_handle;
    int                     payload_handle;
    bp_val_t                current_active_cid;
    int                     active_table_signal;
    bp_active_table_t       active_table;
    /* DTN Aggregate Custody Signals */
    bp_bundle_t             dacs;
    int                     dacs_handle;
    uint8_t*                dacs_buffer;
    int                     dacs_size;
    bp_val_t                dacs_last_sent;
    int                     custody_tree_lock;
    rb_tree_t               custody_tree;
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
    .cid_reuse              = BP_DEFAULT_CID_REUSE,
    .cipher_suite           = BP_DEFAULT_CIPHER_SUITE,
    .class_of_service       = BP_DEFAULT_CLASS_OF_SERVICE,
    .timeout                = BP_DEFAULT_TIMEOUT,
    .max_length             = BP_DEFAULT_MAX_LENGTH,
    .dacs_rate              = BP_DEFAULT_DACS_RATE,
    .protocol_version       = BP_DEFAULT_PROTOCOL_VERSION,
    .retransmit_order       = BP_DEFAULT_RETRANSMIT_ORDER,
    .active_table_size      = BP_DEFAULT_ACTIVE_TABLE_SIZE,
    .max_fills_per_dacs     = BP_DEFAULT_MAX_FILLS_PER_DACS,
    .max_gaps_per_dacs      = BP_DEFAULT_MAX_GAPS_PER_DACS,
    .persistent_storage     = BP_DEFAULT_PERSISTENT_STORAGE,
    .storage_service_parm   = BP_DEFAULT_STORAGE_SERVICE_PARM
};

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * create_bundle
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int create_bundle(void* parm, bool is_record, uint8_t* payload, int size, int timeout)
{
    bp_channel_t*       ch      = (bp_channel_t*)parm;
    bp_bundle_data_t*   data    = NULL;
    int                 handle  = BP_INVALID_HANDLE;

    /* Set Type of Bundle */
    if(is_record)
    {
        handle = ch->dacs_handle;
        data = &ch->dacs.data;
    }
    else /* data bundle */
    {
        handle = ch->bundle_handle;
        data = &ch->bundle.data;
    }

    /* Enqueue Bundle */
    int storage_header_size = &data->header[data->headersize] - (uint8_t*)data;
    return ch->store.enqueue(handle, data, storage_header_size, payload, size, timeout);
}

/*--------------------------------------------------------------------------------------
 * delete_bundle
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int delete_bundle(void* parm, bp_val_t cid, uint32_t* flags)
{
    bp_channel_t* ch = (bp_channel_t*)parm;

    bp_active_bundle_t bundle;
    int status = ch->active_table.remove(ch->active_table.table, cid, &bundle);
    if(status == BP_SUCCESS)
    {
        status = ch->store.relinquish(ch->bundle_handle, bundle.sid);
        if(status != BP_SUCCESS)
        {
            *flags |= BP_FLAG_STORE_FAILURE;
        }
    }
    else
    {
        *flags |= BP_FLAG_UNKNOWN_CID;
    }

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * create_dacs -
 *
 *  Notes:
 *-------------------------------------------------------------------------------------*/
BP_LOCAL_SCOPE int create_dacs(bp_channel_t* ch, unsigned long sysnow, int timeout, uint32_t* flags)
{
    int ret_status = BP_SUCCESS;

    /* If the custody_tree has nodes, initialize the iterator for traversing the custody_tree in order */
    rb_tree_goto_first(&ch->custody_tree);

    /* Continue to delete nodes from the custody_tree and write them to DACS until the custody_tree is empty */
    while (!rb_tree_is_empty(&ch->custody_tree))
    {
        /* Build Acknowledgment - will remove nodes from the custody_tree */
        int size = v6_populate_acknowledgment(ch->dacs_buffer, ch->dacs_size, ch->dacs.attributes.max_fills_per_dacs, &ch->custody_tree, flags);
        if(size > 0)
        {
            int status = BP_SUCCESS;

            /* Check if Re-initialization Needed */
            if(ch->dacs.prebuilt == false)
            {
                status = v6_populate_bundle(&ch->dacs, flags);
            }

            /* Send Bundle */
            if(status == BP_SUCCESS)
            {
                status = v6_send_bundle(&ch->dacs, ch->dacs_buffer, size, create_bundle, ch, timeout, flags);
                if(status == BP_SUCCESS)
                {
                    /* DACS successfully enqueued */
                    ch->dacs_last_sent = sysnow;
                }
                else if(ret_status == BP_SUCCESS)
                {
                    /* Save first failed DACS enqueue to return later */
                    ret_status = status;
                    *flags |= BP_FLAG_STORE_FAILURE;
                    ch->stats.lost++;
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
int bplib_init(void)
{
    int status;

    /* Initialize OS Interface */
    bplib_os_init();

    /* Initialize v6 Module */
    status = v6_initialize();
    if(status != BP_SUCCESS) return status;

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_open -
 *-------------------------------------------------------------------------------------*/
bp_desc_t* bplib_open(bp_route_t route, bp_store_t store, bp_attr_t attributes)
{
    assert(store.create);
    assert(store.destroy);
    assert(store.enqueue);
    assert(store.dequeue);
    assert(store.retrieve);
    assert(store.release);
    assert(store.relinquish);
    assert(store.getcount);

    int status = BP_SUCCESS;

    /* Allocate Channel */
    bp_desc_t* desc = (bp_desc_t*)bplib_os_calloc(sizeof(bp_desc_t));
    bp_channel_t* ch = (bp_channel_t*)bplib_os_calloc(sizeof(bp_channel_t));
    if(desc == NULL || ch == NULL)
    {
        if(desc) bplib_os_free(desc);
        if(ch) bplib_os_free(ch);
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Cannot open channel: not enough memory\n");
        return NULL;
    }
    else
    {
        desc->channel = ch;
    }

    /* Clear Channel Memory and Initialize to Defaults */
    ch->custody_tree_lock   = BP_INVALID_HANDLE;
    ch->active_table_signal = BP_INVALID_HANDLE;
    ch->bundle_handle       = BP_INVALID_HANDLE;
    ch->payload_handle      = BP_INVALID_HANDLE;
    ch->dacs_handle         = BP_INVALID_HANDLE;

    /* Set Store */
    ch->store = store;

    /* Check Protocol Version */
    if(attributes.protocol_version != 6)
    {
        bplog(NULL, BP_FLAG_NONCOMPLIANT, "Unsupported bundle protocol version: %d\n", attributes.protocol_version);
        bplib_close(desc);
        return NULL;
    }

    /* Initialize Bundle Store */
    ch->bundle_handle = ch->store.create(BP_STORE_DATA_TYPE, route.local_node, route.local_service, attributes.persistent_storage, attributes.storage_service_parm);
    if(ch->bundle_handle < 0)
    {
        bplog(NULL, BP_FLAG_STORE_FAILURE, "Failed to create storage handle for bundles\n");
        bplib_close(desc);
        return NULL;
    }

    /* Initialize Payload Store */
    ch->payload_handle = ch->store.create(BP_STORE_PAYLOAD_TYPE, route.local_node, route.local_service, attributes.persistent_storage, attributes.storage_service_parm);
    if(ch->payload_handle < 0)
    {
        bplog(NULL, BP_FLAG_STORE_FAILURE, "Failed to create storage handle for payloads\n");
        bplib_close(desc);
        return NULL;
    }

    /* Initialize DACS Store */
    ch->dacs_handle = ch->store.create(BP_STORE_DACS_TYPE, route.local_node, route.local_service, attributes.persistent_storage, attributes.storage_service_parm);
    if(ch->dacs_handle < 0)
    {
        bplog(NULL, BP_FLAG_STORE_FAILURE, "Failed to create storage handle for dacs\n");
        bplib_close(desc);
        return NULL;
    }

    /* Initialize Bundle */
    status = v6_create(&ch->bundle, route, attributes);
    if(status != BP_SUCCESS)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to initialize bundle\n", status);
        bplib_close(desc);
        return NULL;
    }

    /* Build DACS Attributes */
    bp_attr_t dacs_attributes = attributes;
    dacs_attributes.request_custody = false;
    dacs_attributes.admin_record = true;

    /* Build DACS Route */
    bp_route_t custody_route = route;
    custody_route.destination_node = BP_IPN_NULL;
    custody_route.destination_service = BP_IPN_NULL;

    /* Initialize DACS */
    status = v6_create(&ch->dacs, custody_route, dacs_attributes);
    if(status != BP_SUCCESS)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to initialize dacs\n");
        bplib_close(desc);
        return NULL;
    }

    /* Create DACS Lock */
    ch->custody_tree_lock = bplib_os_createlock();
    if(ch->custody_tree_lock == BP_INVALID_HANDLE)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to create a lock for dacs processing\n");
        bplib_close(desc);
        return NULL;
    }

    /* Allocate Memory for Channel DACS Bundle Fills */
    ch->dacs_size = sizeof(bp_val_t) * attributes.max_fills_per_dacs + 6; // 2 bytes per fill plus payload block header
    ch->dacs_buffer = (uint8_t*)bplib_os_calloc(ch->dacs_size);
    if(ch->dacs_buffer == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to allocate memory for channel DACS\n");
        bplib_close(desc);
        return NULL;
    }

    /* Allocate Memory for DACS Channel Tree to Store Bundle IDs */
    status = rb_tree_create(attributes.max_gaps_per_dacs, &ch->custody_tree);
    if(status != BP_SUCCESS)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to allocate memory for channel DACS tree\n");
        bplib_close(desc);
        return NULL;
    }

    /* Initialize Last Time DACS Sent */
    ch->dacs_last_sent = 0;

    /* Initialize Active Table Signal */
    ch->active_table_signal = bplib_os_createlock();
    if(ch->active_table_signal < 0)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to create custody_tree_lock for active table\n");
        bplib_close(desc);
        return NULL;
    }

    /* Initialize Active Table Functions */
    if(attributes.retransmit_order == BP_RETX_SMALLEST_CID)
    {
        ch->active_table.create     = (bp_table_create_t)cbuf_create;
        ch->active_table.destroy    = (bp_table_destroy_t)cbuf_destroy;
        ch->active_table.add        = (bp_table_add_t)cbuf_add;
        ch->active_table.next       = (bp_table_next_t)cbuf_next;
        ch->active_table.remove     = (bp_table_remove_t)cbuf_remove;
        ch->active_table.available  = (bp_table_available_t)cbuf_available;
        ch->active_table.count      = (bp_table_count_t)cbuf_count;
    }
    else if(attributes.retransmit_order == BP_RETX_OLDEST_BUNDLE)
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
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Unrecognized attribute for creating active table: %d\n", attributes.retransmit_order);
        bplib_close(desc);
        return NULL;
    }


    /* Initialize Active Table */
    status = ch->active_table.create(&ch->active_table.table, attributes.active_table_size);
    if(status != BP_SUCCESS)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "Failed to create active table for channel\n");
        bplib_close(desc);
        return NULL;
    }

    /* Initialize Current Custody ID */
    ch->current_active_cid  = 0;

    /* Return Channel */
    return desc;
}

/*--------------------------------------------------------------------------------------
 * bplib_close -
 *-------------------------------------------------------------------------------------*/
void bplib_close(bp_desc_t* desc)
{
    /* Check Parameters */
    if(desc == NULL || desc->channel == NULL) return;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)desc->channel;

    /* Un-initialize Bundle Store */
    if(ch->bundle_handle != BP_INVALID_HANDLE)
    {
        ch->store.destroy(ch->bundle_handle);
        ch->bundle_handle = BP_INVALID_HANDLE;
    }

    /* Un-initialize Payload Store */
    if(ch->payload_handle != BP_INVALID_HANDLE)
    {
        ch->store.destroy(ch->payload_handle);
        ch->payload_handle = BP_INVALID_HANDLE;
    }

    /* Un-initialize Record Store */
    if(ch->dacs_handle != BP_INVALID_HANDLE)
    {
        ch->store.destroy(ch->dacs_handle);
        ch->dacs_handle = BP_INVALID_HANDLE;
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
        bplib_os_free(ch->dacs_buffer);
        ch->dacs_buffer = NULL;
    }

    /* Free Custody Tree */
    rb_tree_destroy(&ch->custody_tree);

    /* Un-initialize Bundle and DACS */
    v6_destroy(&ch->bundle);
    v6_destroy(&ch->dacs);

    /* Un-initialize Active Table */
    if(ch->active_table_signal != BP_INVALID_HANDLE) bplib_os_destroylock(ch->active_table_signal);
    ch->active_table.destroy(ch->active_table.table);

    /* Free Channel */
    bplib_os_free(ch);
    bplib_os_free(desc);
}

/*--------------------------------------------------------------------------------------
 * bplib_flush -
 *-------------------------------------------------------------------------------------*/
int bplib_flush(bp_desc_t* desc)
{
    /* Check Parameters */
    if(desc == NULL)                return BP_ERROR;
    else if(desc->channel == NULL)  return BP_ERROR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)desc->channel;

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
int bplib_config(bp_desc_t* desc, int mode, int opt, int* val)
{
    /* Check Parameters */
    if(desc == NULL)                return BP_ERROR;
    else if(desc->channel == NULL)  return BP_ERROR;
    else if(val == NULL)            return BP_ERROR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)desc->channel;

    /* Set Mode */
    bool setopt = mode == BP_OPT_MODE_WRITE ? true : false;

    /* Select and Process Option */
    switch(opt)
    {
        case BP_OPT_LIFETIME:
        {
            if(setopt)  ch->bundle.attributes.lifetime = *val;
            else        *val = ch->bundle.attributes.lifetime;
            break;
        }
        case BP_OPT_REQUEST_CUSTODY:
        {
            if(setopt && *val != true && *val != false) return BP_ERROR;
            if(setopt)  ch->bundle.attributes.request_custody = *val;
            else        *val = ch->bundle.attributes.request_custody;
            break;
        }
        case BP_OPT_ADMIN_RECORD:
        {
            if(setopt && *val != true && *val != false) return BP_ERROR;
            if(setopt)  ch->bundle.attributes.admin_record = *val;
            else        *val = ch->bundle.attributes.admin_record;
            break;
        }
        case BP_OPT_INTEGRITY_CHECK:
        {
            if(setopt && *val != true && *val != false) return BP_ERROR;
            if(setopt)  ch->bundle.attributes.integrity_check = *val;
            else        *val = ch->bundle.attributes.integrity_check;
            break;
        }
        case BP_OPT_ALLOW_FRAGMENTATION:
        {
            if(setopt && *val != true && *val != false) return BP_ERROR;
            if(setopt)  ch->bundle.attributes.allow_fragmentation = *val;
            else        *val = ch->bundle.attributes.allow_fragmentation;
            break;
        }
        case BP_OPT_CID_REUSE:
        {
            if(setopt && *val != true && *val != false) return BP_ERROR;
            if(setopt)  ch->bundle.attributes.cid_reuse = *val;
            else        *val = ch->bundle.attributes.cid_reuse;
            break;
        }
        case BP_OPT_CIPHER_SUITE:
        {
            if(setopt)  ch->bundle.attributes.cipher_suite = *val;
            else        *val = ch->bundle.attributes.cipher_suite;
            break;
        }
        case BP_OPT_CLASS_OF_SERVICE:
        {
            if(setopt)  ch->bundle.attributes.class_of_service = *val;
            else        *val = ch->bundle.attributes.class_of_service;
            break;
        }
        case BP_OPT_TIMEOUT:
        {
            if(setopt && *val < 0) return BP_ERROR;
            if(setopt)  ch->bundle.attributes.timeout = *val;
            else        *val = ch->bundle.attributes.timeout;
            break;
        }
        case BP_OPT_MAX_LENGTH:
        {
            if(setopt && *val < 0) return BP_ERROR;
            if(setopt)  ch->bundle.attributes.max_length = *val;
            else        *val = ch->bundle.attributes.max_length;
            break;
        }
        case BP_OPT_DACS_RATE:
        {
            if(setopt)  ch->dacs.attributes.dacs_rate = *val;
            else        *val = ch->dacs.attributes.dacs_rate;
            break;
        }
        default:
        {
            /* Option Not Found */
            return bplog(NULL, BP_FLAG_API_ERROR, "Config. Option Not Found (%d)\n", opt);
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
int bplib_latchstats(bp_desc_t* desc, bp_stats_t* stats)
{
     /* Check Parameters */
    if(desc == NULL)                return BP_ERROR;
    else if(desc->channel == NULL)  return BP_ERROR;
    else if(stats == NULL)          return BP_ERROR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)desc->channel;

    /* Update Store Counts */
    ch->stats.stored_bundles = ch->store.getcount(ch->bundle_handle);
    ch->stats.stored_payloads = ch->store.getcount(ch->payload_handle);
    ch->stats.stored_dacs = ch->store.getcount(ch->dacs_handle);

    /* Update Active Statistic */
    ch->stats.active_bundles = ch->active_table.count(ch->active_table.table);

    /* Latch Statistics */
    *stats = ch->stats;

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_store -
 *-------------------------------------------------------------------------------------*/
int bplib_store(bp_desc_t* desc, void* payload, int size, int timeout, uint32_t* flags)
{
    int status = BP_SUCCESS;

     /* Check Parameters */
    if(desc == NULL)                return BP_ERROR;
    else if(desc->channel == NULL)  return BP_ERROR;
    else if(payload == NULL)        return BP_ERROR;
    else if(flags == NULL)          return BP_ERROR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)desc->channel;

    /* Check if Re-initialization Needed */
    if(ch->bundle.prebuilt == false)
    {
        status = v6_populate_bundle(&ch->bundle, flags);
    }

    /* Send Bundle */
    if(status == BP_SUCCESS)
    {
        status = v6_send_bundle(&ch->bundle, payload, size, create_bundle, ch, timeout, flags);
    }

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_load -
 *-------------------------------------------------------------------------------------*/
int bplib_load(bp_desc_t* desc, void** bundle, int* size, int timeout, uint32_t* flags)
{
    bp_active_bundle_t active_bundle = { BP_SID_VACANT, 0, 0 };
    int status = BP_SUCCESS; /* success or error code */

    /* Check Parameters */
    if(desc == NULL)                return BP_ERROR;
    else if(desc->channel == NULL)  return BP_ERROR;
    else if(bundle == NULL)         return BP_ERROR;
    else if(flags == NULL)          return BP_ERROR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)desc->channel;

    /* Setup State */
    unsigned long   sysnow  = 0;                /* current system time used for timeouts (seconds) */
    bp_object_t*    object  = NULL;             /* start out assuming nothing to send */
    bool            newcid  = true;             /* whether to assign new custody id and active table entry */
    bool            resend  = false;            /* is loaded bundle a retransmission */
    bool            isdacs  = false;            /* is loaded bundle a dacs */

    /* Get Current Time */
    if(bplib_os_systime(&sysnow) == BP_ERROR)
    {
        *flags |= BP_FLAG_UNRELIABLE_TIME;
    }

    /*-------------------------*/
    /* Try to Send DACS Bundle */
    /*-------------------------*/
    if(ch->dacs.attributes.dacs_rate > 0)
    {
        /* Check If DACS Ready to Send */
        bplib_os_lock(ch->custody_tree_lock);
        {
            if( (sysnow >= (ch->dacs_last_sent + ch->dacs.attributes.dacs_rate)) &&
                !rb_tree_is_empty(&ch->custody_tree) )
            {
                create_dacs(ch, sysnow, BP_CHECK, flags);
            }
        }
        bplib_os_unlock(ch->custody_tree_lock);
    }

    /* Dequeue any Stored DACS */
    int dacs_status = ch->store.dequeue(ch->dacs_handle, &object, BP_CHECK);
    if(dacs_status == BP_SUCCESS)
    {
        isdacs = true;
    }
    else if(dacs_status != BP_TIMEOUT)
    {
        /* Failed Storage Service */
        bplog(flags, BP_FLAG_STORE_FAILURE, "Failed (%d) to dequeue dacs bundle from storage service\n", dacs_status);
    }

    /*------------------------------------------------*/
    /* Try to Send Active Bundle (if nothing to send) */
    /*------------------------------------------------*/
    bplib_os_lock(ch->active_table_signal);
    {
        /* Get Oldest Bundle */
        while(object == NULL && ch->active_table.next(ch->active_table.table, &active_bundle) == BP_SUCCESS)
        {
            /* Check if Bundle has Timed Out */
            if(ch->bundle.attributes.timeout != 0 && sysnow >= (active_bundle.retx + ch->bundle.attributes.timeout))
            {
                /* Retrieve Timed Out Bundle from Storage */
                if(ch->store.retrieve(ch->bundle_handle, active_bundle.sid, &object, BP_CHECK) == BP_SUCCESS)
                {
                    bp_bundle_data_t* data = (bp_bundle_data_t*)object->data;

                    /* Check Lifetime of Bundle */
                    if(data->exprtime != 0 && sysnow >= data->exprtime)
                    {
                        /* Bundle Expired */
                        object = NULL;
                        ch->stats.expired++;
                    }
                }
                else
                {
                    /* Failed to Retrieve Bundle from Storage */
                    *flags |= BP_FLAG_STORE_FAILURE;
                    ch->stats.lost++;
                }

                /* Check Success of Retrieving Valid Timed Out Bundle */
                if(object)
                {
                    /* Bundle is a Retransmission */
                    resend = true;

                    /* Handle Active Table and Custody ID */
                    if(ch->bundle.attributes.cid_reuse)
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
                }
                else
                {
                    /* Clear Entry in Active Table and Storage */
                    ch->active_table.remove(ch->active_table.table, active_bundle.cid, NULL);
                    ch->store.release(ch->bundle_handle, active_bundle.sid);
                    ch->store.relinquish(ch->bundle_handle, active_bundle.sid);
                }
            }
            else /* oldest active bundle still active */
            {
                /* Check Active Table Has Room
                    * Since next step is to dequeue from store, need to make sure that there is room
                    * in the active table since we don't want to dequeue a bundle from store and have
                    * no place to put it.  Note that it is possible that even if the active table was
                    * full, if the bundle dequeued did not request custody transfer it could still go
                    * out, but the current design requires that at least one slot in the active table
                    * is open at all times. */
                status = ch->active_table.available(ch->active_table.table, ch->current_active_cid);
                if(status != BP_SUCCESS)
                {
                    *flags |= BP_FLAG_ACTIVE_TABLE_WRAP;
                    status = bplib_os_waiton(ch->active_table_signal, timeout);
                }

                /* Break Out of Loop */
                break;
            }
        }
    }
    bplib_os_unlock(ch->active_table_signal);

    /*------------------------------------------------*/
    /* Try to Send Stored Bundle (if nothing to send) */
    /*------------------------------------------------*/
    while(object == NULL && status == BP_SUCCESS)
    {
        /* Dequeue Bundle from Storage Service */
        int deq_status = ch->store.dequeue(ch->bundle_handle, &object, timeout);
        if(deq_status == BP_SUCCESS)
        {
            bp_bundle_data_t* data = (bp_bundle_data_t*)object->data;

            /* Check Expiration Time */
            if(data->exprtime != 0 && sysnow >= data->exprtime)
            {
                /* Bundle Expired Clear Entry (and loop again) */
                ch->store.release(ch->bundle_handle, object->header.sid);
                ch->store.relinquish(ch->bundle_handle, object->header.sid);
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
            status = bplog(flags, BP_FLAG_STORE_FAILURE, "Failed (%d) to dequeue bundle from storage service\n", deq_status);
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
            /* Save/Update Storage ID */
            active_bundle.sid = object->header.sid;

            /* Update Retransmit Time */
            active_bundle.retx = sysnow;

            /* Save Bundle as Active */
            bplib_os_lock(ch->active_table_signal);
            {
                /* Assign New Custody ID */
                if(newcid) active_bundle.cid = ch->current_active_cid++;

                /* Update Active Table */
                status = ch->active_table.add(ch->active_table.table, active_bundle, !newcid);
                if(status == BP_DUPLICATE) *flags |= BP_FLAG_DUPLICATES;
            }
            bplib_os_unlock(ch->active_table_signal);

            /* Jam Custody ID */
            v6_update_bundle(data, active_bundle.cid, flags);
        }

        /* Load Bundle */
        *bundle = data->header;
        if(size) *size = data->bundlesize;

        /* Update Statistics and Flags */
        if(isdacs)
        {
            ch->stats.transmitted_dacs++;
            *flags |= BP_FLAG_ROUTE_NEEDED;
        }
        else if(resend)
        {
            ch->stats.retransmitted_bundles++;
        }
        else /* new data bundle */
        {
            ch->stats.transmitted_bundles++;
        }
    }

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_process -
 *-------------------------------------------------------------------------------------*/
int bplib_process(bp_desc_t* desc, void* bundle, int size, int timeout, uint32_t* flags)
{
    int status;

    /* Check Parameters */
    if(desc == NULL)                return BP_ERROR;
    else if(desc->channel == NULL)  return BP_ERROR;
    else if(bundle == NULL)         return BP_ERROR;
    else if(flags == NULL)          return BP_ERROR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)desc->channel;

    /* Receive Bundle */
    bp_payload_t payload;
    bool custody_transfer = false;
    status = v6_receive_bundle(&ch->bundle, bundle, size, &payload, flags);
    if(status == BP_PENDING_EXPIRATION)
    {
        ch->stats.expired++;
    }
    else if(status == BP_PENDING_ACKNOWLEDGMENT)
    {
        /* Increment Statistics */
        ch->stats.received_dacs++;

        /* Process Aggregate Custody Signal (DACS) */
        bplib_os_lock(ch->active_table_signal);
        {
            int num_acks = 0;
            int bytes_read = v6_receive_acknowledgment(payload.memptr, payload.data.payloadsize, &num_acks, delete_bundle, ch, flags);
            ch->stats.acknowledged_bundles += num_acks;

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
    else if(status == BP_PENDING_ACCEPTANCE)
    {
        /* Increment Statistics */
        ch->stats.received_bundles++;

        /* Store Payload */
        status = ch->store.enqueue(ch->payload_handle, &payload.data, sizeof(bp_payload_data_t), payload.memptr, payload.data.payloadsize, timeout);
        if(status == BP_SUCCESS && payload.node != BP_IPN_NULL)
        {
            custody_transfer = true;
        }
        else if(status != BP_SUCCESS)
        {
            *flags |= BP_FLAG_STORE_FAILURE;
            ch->stats.lost++;
        }
    }
    else if(status == BP_PENDING_FORWARD)
    {
        /* Increment Statistics */
        ch->stats.forwarded_bundles++;

        /* Store Forwarded Bundle */
        status = v6_send_bundle(&ch->bundle, payload.memptr, payload.data.payloadsize, create_bundle, ch, timeout, flags);
        if(status == BP_SUCCESS && payload.node != BP_IPN_NULL)
        {
            custody_transfer = true;
        }
        else if(status != BP_SUCCESS)
        {
            ch->stats.lost++;
        }
    }
    else if(status == BP_SUCCESS) /* Bundle Destined for Local Node w/o Custody Transfer */
    {
        /* Increment Statistics */
        ch->stats.received_bundles++;
    }
    else
    {
        /* Increment Statistics */
        ch->stats.unrecognized++;
    }


    /* Acknowledge Custody Transfer - Update DACS */
    if(custody_transfer)
    {
        /* Get Time */
        unsigned long sysnow = 0;
        if(bplib_os_systime(&sysnow) == BP_ERROR)
        {
            *flags |= BP_FLAG_UNRELIABLE_TIME;
        }

        /* Take Custody */
        bplib_os_lock(ch->custody_tree_lock);
        {
            if(ch->dacs.route.destination_node == payload.node && ch->dacs.route.destination_service == payload.service)
            {
                /* Insert Custody ID directly into current custody_tree */
                int insert_status = rb_tree_insert(payload.cid, &ch->custody_tree);
                if(insert_status == BP_FULL)
                {
                    /* Flag Full Tree - possibly the custody_tree size is configured to be too small */
                    *flags |= BP_FLAG_CUSTODY_FULL;

                    /* Store Custody Signal */
                    create_dacs(ch, sysnow, BP_CHECK, flags);

                    /* Start New DACS */
                    insert_status = rb_tree_insert(payload.cid, &ch->custody_tree);

                    /* There is no valid reason for an insert to fail on an empty custody_tree */
                    assert(insert_status == BP_SUCCESS);
                }
                else if(insert_status == BP_DUPLICATE)
                {
                    /* Duplicate values are fine and are treated as a success */
                    *flags |= BP_FLAG_DUPLICATES;
                }
                else if(insert_status != BP_SUCCESS)
                {
                    /* Tree error unexpected */
                    assert(false);
                }
            }
            else
            {
                /* Store DACS Bundle */
                if(!rb_tree_is_empty(&ch->custody_tree))
                {
                    create_dacs(ch, sysnow, BP_CHECK, flags);
                }

                /* Initial New DACS Bundle */
                ch->dacs.route.destination_node = payload.node;
                ch->dacs.route.destination_service = payload.service;
                ch->dacs.prebuilt = false;

                /* Start New DACS */
                int insert_status = rb_tree_insert(payload.cid, &ch->custody_tree);

                /* There is no valid reason for an insert to fail on an empty custody_tree */
                assert(insert_status == BP_SUCCESS);
                (void)insert_status; /* when asserts compiled out, insert_status becomes unused */
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
int bplib_accept(bp_desc_t* desc, void** payload, int* size, int timeout, uint32_t* flags)
{
    int status = BP_SUCCESS;
    bp_object_t* object = NULL;

    /* Check Parameters */
    if(desc == NULL)                return BP_ERROR;
    else if(desc->channel == NULL)  return BP_ERROR;
    else if(payload == NULL)        return BP_ERROR;
    else if(flags == NULL)          return BP_ERROR;

    /* Get Channel */
    bp_channel_t* ch = (bp_channel_t*)desc->channel;

    while(object == NULL && status == BP_SUCCESS)
    {
        /* Dequeue Payload from Storage */
        status = ch->store.dequeue(ch->payload_handle, &object, timeout);
        if(status == BP_SUCCESS)
        {
            bp_payload_data_t* data = (bp_payload_data_t*)object->data;

            /* Get Current Time */
            unsigned long sysnow = 0;
            if(bplib_os_systime(&sysnow) == BP_ERROR)
            {
                *flags |= BP_FLAG_UNRELIABLE_TIME;
                sysnow = 0; /* prevents expiration below */
            }

            /* Check Expiration Time */
            if(data->exprtime != 0 && data->exprtime <= sysnow)
            {
                ch->store.release(ch->payload_handle, object->header.sid);
                ch->store.relinquish(ch->payload_handle, object->header.sid);
                ch->stats.expired++;
                object = NULL;
            }
            else
            {
                /* Return Payload to Application */
                *payload = (void*)((uint8_t*)data + sizeof(bp_payload_data_t));
                if(size) *size = data->payloadsize;

                /* Check for Application Acknowledgement Request */
                if(data->ackapp) status = BP_PENDING_APPLICATION;

                /* Count as Delivered */
                ch->stats.delivered_payloads++;
            }
        }
        else if(status != BP_TIMEOUT)
        {
            /* Failed Storage Service */
            *flags |= BP_FLAG_STORE_FAILURE;
        }
    }

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_ackbundle -
 *-------------------------------------------------------------------------------------*/
int bplib_ackbundle(bp_desc_t* desc, void* bundle)
{
    int status = BP_SUCCESS;

    /* Check Parameters */
    if(desc == NULL)                return BP_ERROR;
    else if(desc->channel == NULL)  return BP_ERROR;
    else if(bundle == NULL)         return BP_ERROR;

    /* Determine Storage Object Pointer */
    bp_channel_t* ch = (bp_channel_t*)desc->channel;
    bp_bundle_data_t* data = (bp_bundle_data_t*)((uint8_t*)bundle - offsetof(bp_bundle_data_t, header));
    bp_object_t* object = (bp_object_t*)((uint8_t*)data - sizeof(bp_object_hdr_t));

    /* Release Memory */
    ch->store.release(object->header.handle, object->header.sid);

    /* Free Memory - only when no custody transfer is requested */
    if(data->cteboffset == 0) ch->store.relinquish(object->header.handle, object->header.sid);

    /* Return Status */
    return status;
}

/*--------------------------------------------------------------------------------------
 * bplib_ackpayload -
 *-------------------------------------------------------------------------------------*/
int bplib_ackpayload(bp_desc_t* desc, void* payload)
{
    int status = BP_SUCCESS;

    /* Check Parameters */
    if(desc == NULL)                return BP_ERROR;
    else if(desc->channel == NULL)  return BP_ERROR;
    else if(payload == NULL)        return BP_ERROR;

    /* Determine Storage Object Pointer */
    bp_channel_t* ch = (bp_channel_t*)desc->channel;
    bp_payload_data_t* data = (bp_payload_data_t*)((uint8_t*)payload - sizeof(bp_payload_data_t));
    bp_object_t* object = (bp_object_t*)((uint8_t*)data - sizeof(bp_object_hdr_t));

    /* Release Memory */
    ch->store.release(object->header.handle, object->header.sid);

    /* Free Memory */
    ch->store.relinquish(object->header.handle, object->header.sid);

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
int bplib_display(void* bundle, int size, uint32_t* flags)
{
    return v6_display(bundle, size, flags);
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
        return bplog(NULL, BP_FLAG_API_ERROR, "EID is null\n");
    }

    /* Sanity Check Length of EID */
    if(len < 7)
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID must be at least 7 characters, act: %d\n", len);
    }
    else if(len > BP_MAX_EID_STRING)
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID cannot exceed %d bytes in length, act: %d\n", BP_MAX_EID_STRING, len);
    }

    /* Check IPN Scheme */
    if(eid[0] != 'i' || eid[1] != 'p' || eid[2] != 'n' || eid[3] != ':')
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID (%s) must start with 'ipn:'\n", eid);
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
        return bplog(NULL, BP_FLAG_API_ERROR, "Unable to find dotted notation in EID (%s)\n", eid);
    }

    /* Parse Node Number */
    errno = 0;
    node_result = strtoul(node_ptr, &endptr, 10); /* assume IPN node and service numbers always written in base 10 */
    if( (endptr == node_ptr) ||
        ((node_result == ULONG_MAX || node_result == 0) && errno == ERANGE) )
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "Unable to parse EID (%s) node number\n", eid);
    }

    /* Parse Service Number */
    errno = 0;
    service_result = strtoul(service_ptr, &endptr, 10); /* assume IPN node and service numbers always written in base 10 */
    if( (endptr == service_ptr) ||
        ((service_result == ULONG_MAX || service_result == 0) && errno == ERANGE) )
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "Unable to parse EID (%s) service number\n", eid);
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
        return bplog(NULL, BP_FLAG_API_ERROR, "EID buffer is null\n");
    }

    /* Sanity Check Length of EID Buffer */
    if(len < 7)
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID buffer must be at least 7 characters, act: %d\n", len);
    }
    else if(len > BP_MAX_EID_STRING)
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID buffer cannot exceed %d bytes in length, act: %d\n", BP_MAX_EID_STRING, len);
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
int bplib_attrinit(bp_attr_t* attributes)
{
    if(attributes)
    {
        *attributes = default_attributes;
        return BP_SUCCESS;
    }
    else
    {
        return BP_ERROR;
    }
}
