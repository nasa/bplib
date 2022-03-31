/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef bplib_storage_service_h
#define bplib_storage_service_h

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_api_types.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

/* Return Codes */
#define BP_SUCCESS 0
#define BP_ERROR   (-1)
#define BP_TIMEOUT (-2)

/* Endpoint IDs */
#define BP_MAX_EID_STRING 128
#define BP_IPN_NULL       0

/* Storage IDs */
#define BP_SID_VACANT 0

/* Storage Service Types */
#define BP_STORE_DATA_TYPE    0xB0
#define BP_STORE_DACS_TYPE    0xB1
#define BP_STORE_PAYLOAD_TYPE 0xB2

/* Error Correcting Codes */
#define BP_ECC_NO_ERRORS    0
#define BP_ECC_COR_ERRORS   (-1)
#define BP_ECC_UNCOR_ERRORS (-2)

/* Class of Service */
#define BP_COS_BULK      0
#define BP_COS_NORMAL    1
#define BP_COS_EXPEDITED 2
#define BP_COS_EXTENDED  3

/* Bundle Integrity Types */
#define BP_BIB_NONE             0
#define BP_BIB_CRC16_X25        10
#define BP_BIB_CRC32_CASTAGNOLI 11

/* Retransmit Order */
#define BP_RETX_OLDEST_BUNDLE 0
#define BP_RETX_SMALLEST_CID  1

/* Set/Get Option Modes */
#define BP_OPT_MODE_READ  0
#define BP_OPT_MODE_WRITE 1

/* Set/Get Option Defines */
#define BP_OPT_LIFETIME            1
#define BP_OPT_REQUEST_CUSTODY     2
#define BP_OPT_ADMIN_RECORD        3
#define BP_OPT_INTEGRITY_CHECK     4
#define BP_OPT_ALLOW_FRAGMENTATION 5
#define BP_OPT_IGNORE_EXPIRATION   6
#define BP_OPT_CID_REUSE           7
#define BP_OPT_CIPHER_SUITE        8
#define BP_OPT_CLASS_OF_SERVICE    9
#define BP_OPT_TIMEOUT             10
#define BP_OPT_MAX_LENGTH          11
#define BP_OPT_DACS_RATE           12

/* Default Dynamic Configuration */
#define BP_DEFAULT_LIFETIME            86400 /* seconds, 1 day */
#define BP_DEFAULT_REQUEST_CUSTODY     true
#define BP_DEFAULT_ADMIN_RECORD        false
#define BP_DEFAULT_INTEGRITY_CHECK     true
#define BP_DEFAULT_ALLOW_FRAGMENTATION false
#define BP_DEFAULT_IGNORE_EXPIRATION   false
#define BP_DEFAULT_CID_REUSE           false
#define BP_DEFAULT_CIPHER_SUITE        BP_BIB_CRC16_X25
#define BP_DEFAULT_CLASS_OF_SERVICE    BP_COS_NORMAL
#define BP_DEFAULT_TIMEOUT             10   /* seconds */
#define BP_DEFAULT_MAX_LENGTH          4096 /* bytes (must be smaller than BP_MAX_INDEX) */
#define BP_DEFAULT_DACS_RATE           5    /* period in seconds */

/* Default Fixed Configuration */
#define BP_DEFAULT_PROTOCOL_VERSION     6
#define BP_DEFAULT_RETRANSMIT_ORDER     BP_RETX_OLDEST_BUNDLE
#define BP_DEFAULT_ACTIVE_TABLE_SIZE    16384 /* bundles (must be smaller than BP_MAX_INDEX) */
#define BP_DEFAULT_MAX_FILLS_PER_DACS   64    /* constrains size of DACS bundle */
#define BP_DEFAULT_MAX_GAPS_PER_DACS    1028  /* sets size of internal memory used to aggregate custody */
#define BP_DEFAULT_PERSISTENT_STORAGE   false
#define BP_DEFAULT_STORAGE_SERVICE_PARM NULL

/* Global Custody ID (Compile-Time Option) */
#ifndef BPLIB_GLOBAL_CUSTODY_ID
#define BPLIB_GLOBAL_CUSTODY_ID true
#endif

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Address Routing */
typedef struct
{
    bp_ipn_t local_node; /* IPN address used for source and custody addresses */
    bp_ipn_t local_service;
    bp_ipn_t destination_node; /* IPN address bundles are sent to */
    bp_ipn_t destination_service;
    bp_ipn_t report_node; /* IPN address to send report bundles (currently not used) */
    bp_ipn_t report_service;
} bp_route_t;

/* Storage Object Header */
typedef struct
{
    bp_handle_t handle;
    size_t      size;
    bp_sid_t    sid;
} bp_object_hdr_t;

/* Storage Object */
typedef struct
{
    bp_object_hdr_t header;
    char            data[];
} bp_object_t;

/* Storage Service */
typedef struct
{
    bp_handle_t (*create)(int type, bp_ipn_t node, bp_ipn_t service, bool recover, void *parm);

    int (*destroy)(bp_handle_t h);
    int (*enqueue)(bp_handle_t h, const void *data1, size_t data1_size, const void *data2, size_t data2_size,
                   int timeout);
    int (*dequeue)(bp_handle_t h, bp_object_t **object, int timeout);
    int (*retrieve)(bp_handle_t h, bp_sid_t sid, bp_object_t **object, int timeout);
    int (*release)(bp_handle_t h, bp_sid_t sid);
    int (*relinquish)(bp_handle_t h, bp_sid_t sid);
    int (*getcount)(bp_handle_t h);
} bp_store_t;

/* Channel Attributes */
typedef struct
{
    /* Dynamic Attributes */
    bp_val_t lifetime;            /* Number of seconds from creation time before bundle expires */
    bool     request_custody;     /* 0: custody not requested, 1: custody requested */
    bool     admin_record;        /* 0: payload data, 1: administrative record */
    bool     integrity_check;     /* 0: do not include an integrity check, 1: include bundle integrity block */
    bool     allow_fragmentation; /* 0: do not allow, 1: allow (for created bundles, if allowed, it will be used) */
    bool     ignore_expiration;   /* 0: delete expired bundles, 1: ignore lifetime and do not delete expired bundles */
    bool     cid_reuse;           /* 0: new CID when retransmitting, 1: reuse CID when retransmitting */
    int      cipher_suite;        /* 0: present but un-populated, all other values identify a cipher suite */
    int      class_of_service;    /* priority of generated bundles */
    int      timeout;             /* seconds, zero for infinite */
    int      max_length;          /* maximum size of bundle in bytes */
    int      dacs_rate;           /* number of seconds to wait between sending ACS bundles (<=0: no periodic dacs) */
    /* Fixed Attributes */
    int   protocol_version;     /* bundle protocol version; currently only version 6 supported */
    int   retransmit_order;     /* determination of which timed-out bundle is retransmitted first */
    int   active_table_size;    /* number of unacknowledged bundles to keep track of */
    int   max_fills_per_dacs;   /* limits the size of the DACS bundle */
    int   max_gaps_per_dacs;    /* number of gaps in custody IDs that can be kept track of */
    bool  persistent_storage;   /* attempt to recover bundles and payloads from storage service */
    void *storage_service_parm; /* pass through of parameters needed by storage service */
} bp_attr_t;

/* Channel Statistics */
typedef struct
{
    /* Errors */
    uint32_t lost;         /* storage failure (load, process, accept) */
    uint32_t expired;      /* bundles, dacs, and payloads with expired lifetime (load, process, accept) */
    uint32_t unrecognized; /* unable to parse input OR bundle type not supported (process) */
    /* Data Flow */
    uint32_t transmitted_bundles;   /* bundles sent for first time, does not includes re-sends (load) */
    uint32_t transmitted_dacs;      /* dacs sent (load) */
    uint32_t retransmitted_bundles; /* bundles timed-out and resent (load) */
    uint32_t delivered_payloads;    /* payloads delivered to application (accept) */
    uint32_t received_bundles;      /* bundles destined for local node (process) */
    uint32_t forwarded_bundles;     /* bundles received by local node but destined for another node (process) */
    uint32_t received_dacs;         /* dacs destined for local node (process) */
    /* Storage */
    uint32_t stored_bundles;  /* number of data bundles currently in storage */
    uint32_t stored_payloads; /* number of payloads currently in storage */
    uint32_t stored_dacs;     /* number of dacs bundles currently in storage */
    /* Active */
    uint32_t acknowledged_bundles; /* freed by custody signal - process */
    uint32_t active_bundles;       /* number of slots in active table in use */
} bp_stats_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

//int  bplib_init(void);
//void bplib_deinit(void);

bp_desc_t *bplib_storage_open(size_t cache_size, bp_store_t store, bp_attr_t attributes);
void       bplib_storage_close(bp_desc_t *desc);

int bplib_storage_enqueue(bp_desc_t *desc, const void *payload, size_t size, bp_ipn_t node, bp_ipn_t service, int timeout, uint32_t *flags);
int bplib_storage_recvfrom(bp_desc_t *desc, void *payload, size_t *size, bp_ipn_t *node, bp_ipn_t *service, int timeout, uint32_t *flags);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* bplib_storage_service_h */
