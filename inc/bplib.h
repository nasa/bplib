/************************************************************************
 * File: bplib.h
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

#ifndef _bplib_h_
#define _bplib_h_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_os.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

/* Return Codes */
#define BP_SUCCESS                      0
#define BP_ERROR                        (-1)
#define BP_TIMEOUT                      (-2)
#define BP_DUPLICATE                    (-3)
#define BP_FULL                         (-4)
#define BP_PENDING_ACKNOWLEDGMENT       (-5)
#define BP_PENDING_FORWARD              (-6)
#define BP_PENDING_ACCEPTANCE           (-7)
#define BP_PENDING_APPLICATION          (-8)
#define BP_PENDING_EXPIRATION           (-9)

/* Event Flags */
#define BP_FLAG_DIAGNOSTIC              0x0000000
#define BP_FLAG_NONCOMPLIANT            0x0000001   /* valid bundle but agent not able to comply with standard */
#define BP_FLAG_INCOMPLETE              0x0000002   /* block in bundle was not recognized */
#define BP_FLAG_UNRELIABLE_TIME         0x0000004   /* the os call to get time return a suspicious value */
#define BP_FLAG_DROPPED                 0x0000008   /* bundle dropped because a required extension block could not be processed */
#define BP_FLAG_FAILED_INTEGRITY_CHECK  0x0000010   /* bundle with BIB failed the integrity check on the payload */
#define BP_FLAG_BUNDLE_TOO_LARGE        0x0000020   /* size of bundle exceeds capacity allowed by library */
#define BP_FLAG_ROUTE_NEEDED            0x0000040   /* the bundle returned should be routed before transmission */
#define BP_FLAG_STORE_FAILURE           0x0000080   /* storage service failed to deliver data */
#define BP_FLAG_UNKNOWN_CID             0x0000100   /* received CID in acknowledgment for which no bundle was found */
#define BP_FLAG_SDNV_OVERFLOW           0x0000200   /* insufficient room in variable to read/write value */
#define BP_FLAG_SDNV_INCOMPLETE         0x0000400   /* insufficient room in block to read/write value */
#define BP_FLAG_ACTIVE_TABLE_WRAP       0x0000800   /* the active table wrapped */
#define BP_FLAG_DUPLICATES              0x0001000   /* multiple bundles on the network have the same custody id */
#define BP_FLAG_CUSTODY_FULL            0x0002000   /* the dacs rb_tree was full */
#define BP_FLAG_UNKNOWNREC              0x0004000   /* bundle contained unknown adminstrative record */
#define BP_FLAG_INVALID_CIPHER_SUITEID  0x0008000   /* invalid cipher suite ID found in BIB */
#define BP_FLAG_INVALID_BIB_RESULT_TYPE 0x0010000   /* invalid result type found in BIB */
#define BP_FLAG_INVALID_BIB_TARGET_TYPE 0x0020000   /* invalid target type found in BIB */
#define BP_FLAG_FAILED_TO_PARSE         0x0040000   /* unable to parse bundle due to internal inconsistencies in bundle */

/* Handles */
#define BP_INVALID_HANDLE               (-1)    /* used for integers (os locks, storage services) */

/* Timeouts */
#define BP_PEND                         (-1)
#define BP_CHECK                        0

/* Endpoint IDs */
#define BP_MAX_EID_STRING               128
#define BP_IPN_NULL                     0

/* Storage IDs */
#define BP_SID_VACANT                   0

/* Error Correcting Codes */
#define BP_ECC_NO_ERRORS                0
#define BP_ECC_COR_ERRORS               (-1)
#define BP_ECC_UNCOR_ERRORS             (-2)

/* Class of Service */
#define BP_COS_BULK                     0
#define BP_COS_NORMAL                   1
#define BP_COS_EXPEDITED                2
#define BP_COS_EXTENDED                 3

/* Bundle Integrity Types */
#define BP_BIB_NONE                     0
#define BP_BIB_CRC16_X25                10
#define BP_BIB_CRC32_CASTAGNOLI         11

/* Retransmit Order */
#define BP_RETX_OLDEST_BUNDLE           0
#define BP_RETX_SMALLEST_CID            1

/* Set/Get Option Modes */
#define BP_OPT_MODE_READ                0
#define BP_OPT_MODE_WRITE               1

/* Set/Get Option Defines */
#define BP_OPT_LIFETIME                 1
#define BP_OPT_REQUEST_CUSTODY          2
#define BP_OPT_ADMIN_RECORD             3
#define BP_OPT_INTEGRITY_CHECK          4
#define BP_OPT_ALLOW_FRAGMENTATION      5
#define BP_OPT_CID_REUSE                6
#define BP_OPT_CIPHER_SUITE             7
#define BP_OPT_CLASS_OF_SERVICE         8
#define BP_OPT_TIMEOUT                  9
#define BP_OPT_MAX_LENGTH               10
#define BP_OPT_DACS_RATE                11

/* Default Dynamic Configuration */
#define BP_DEFAULT_LIFETIME             86400 /* seconds, 1 day */
#define BP_DEFAULT_REQUEST_CUSTODY      true
#define BP_DEFAULT_ADMIN_RECORD         false
#define BP_DEFAULT_INTEGRITY_CHECK      true
#define BP_DEFAULT_ALLOW_FRAGMENTATION  false
#define BP_DEFAULT_CID_REUSE            false
#define BP_DEFAULT_CIPHER_SUITE         BP_BIB_CRC16_X25
#define BP_DEFAULT_CLASS_OF_SERVICE     BP_COS_NORMAL
#define BP_DEFAULT_TIMEOUT              10 /* seconds */
#define BP_DEFAULT_MAX_LENGTH           4096 /* bytes (must be smaller than BP_MAX_INDEX) */
#define BP_DEFAULT_DACS_RATE            5 /* period in seconds */

/* Default Fixed Configuration */
#define BP_DEFAULT_PROTOCOL_VERSION     6
#define BP_DEFAULT_RETRANSMIT_ORDER     BP_RETX_OLDEST_BUNDLE
#define BP_DEFAULT_ACTIVE_TABLE_SIZE    16384 /* bundles (must be smaller than BP_MAX_INDEX) */
#define BP_DEFAULT_MAX_FILLS_PER_DACS   64 /* constrains size of DACS bundle */
#define BP_DEFAULT_MAX_GAPS_PER_DACS    1028 /* sets size of internal memory used to aggregate custody */
#define BP_DEFAULT_STORAGE_SERVICE_PARM NULL

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Channel Descriptor */
typedef struct {
    void* channel;
} bp_desc_t;

/* IPN Schema Endpoint ID Integer Definition */
typedef bp_val_t bp_ipn_t;

/* Address Routing */
typedef struct {
    bp_ipn_t    local_node;
    bp_ipn_t    local_service;
    bp_ipn_t    destination_node;
    bp_ipn_t    destination_service;
    bp_ipn_t    report_node;
    bp_ipn_t    report_service;
} bp_route_t;

/* Storage ID */
typedef unsigned long bp_sid_t;

/* Storage Object Header */
typedef struct {
    int         handle;
    int         size;
    bp_sid_t    sid;
} bp_object_hdr_t;

/* Storage Object */
typedef struct {
    bp_object_hdr_t header;
    char            data[];
} bp_object_t;

/* Storage Service */
typedef struct {
    int (*create)       (void* parm);
    int (*destroy)      (int handle);
    int (*enqueue)      (int handle, void* data1, int data1_size, void* data2, int data2_size, int timeout);
    int (*dequeue)      (int handle, bp_object_t** object, int timeout);
    int (*retrieve)     (int handle, bp_sid_t sid, bp_object_t** object, int timeout);
    int (*release)      (int handle, bp_sid_t sid);
    int (*relinquish)   (int handle, bp_sid_t sid);
    int (*getcount)     (int handle);
} bp_store_t;

/* Channel Attributes */
typedef struct {
    /* Dynamic Attributes */
    bp_val_t    lifetime;               /* Number of seconds from creation time before bundle expires */
    bool        request_custody;        /* 0: custody not requested, 1: custody requested */
    bool        admin_record;           /* 0: payload data, 1: administrative record */
    bool        integrity_check;        /* 0: do not include an integrity check, 1: include bundle integrity block */
    bool        allow_fragmentation;    /* 0: do not allow, 1: allow (for created bundles, if allowed, it will be used) */
    int         cid_reuse;              /* 0: new CID when retransmitting, 1: reuse CID when retransmitting */
    int         cipher_suite;           /* 0: present but un-populated, all other values identify a cipher suite */
    int         class_of_service;       /* priority of generated bundles */
    int         timeout;                /* seconds, zero for infinite */
    int         max_length;             /* maximum size of bundle in bytes (includes header blocks) */
    int         dacs_rate;              /* number of seconds to wait between sending ACS bundles */
    /* Fixed Attributes */
    int         protocol_version;       /* bundle protocol version; currently only version 6 supported */
    int         retransmit_order;       /* determination of which timed-out bundle is retransmitted first */
    int         active_table_size;      /* number of unacknowledged bundles to keep track of */
    int         max_fills_per_dacs;     /* limits the size of the DACS bundle */
    int         max_gaps_per_dacs;      /* number of gaps in custody IDs that can be kept track of */
    void*       storage_service_parm;   /* pass through of parameters needed by storage service */
} bp_attr_t;

/* Channel Statistics */
typedef struct {
    /* Errors */
    uint32_t    lost;                   /* storage failure (load, process, accept) */
    uint32_t    expired;                /* bundles, dacs, and payloads with expired lifetime (load, process, accept) */
    uint32_t    unrecognized;           /* unable to parse input OR bundle type not supported (process) */
    /* Data Flow */
    uint32_t    transmitted_bundles;    /* bundles sent for first time, does not includes re-sends (load) */
    uint32_t    transmitted_dacs;       /* dacs sent (load) */
    uint32_t    retransmitted_bundles;  /* bundles timed-out and resent (load) */
    uint32_t    delivered_payloads;     /* payloads delivered to application (accept) */
    uint32_t    received_bundles;       /* bundles destined for local node (process) */
    uint32_t    forwarded_bundles;      /* bundles received by local node but destined for another node (process) */
    uint32_t    received_dacs;          /* dacs destined for local node (process) */
    /* Storage */
    uint32_t    stored_bundles;         /* number of data bundles currently in storage */
    uint32_t    stored_payloads;        /* number of payloads currently in storage */
    uint32_t    stored_dacs;            /* number of dacs bundles currently in storage */
    /* Active */
    uint32_t    acknowledged_bundles;   /* freed by custody signal - process */
    uint32_t    active_bundles;         /* number of slots in active table in use */
} bp_stats_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int         bplib_init          (void);

bp_desc_t*  bplib_open          (bp_route_t route, bp_store_t store, bp_attr_t attributes);
void        bplib_close         (bp_desc_t* desc);

int         bplib_flush         (bp_desc_t* desc);
int         bplib_config        (bp_desc_t* desc, int mode, int opt, int* val);
int         bplib_latchstats    (bp_desc_t* desc, bp_stats_t* stats);

int         bplib_store         (bp_desc_t* desc, void* payload, int size, int timeout, uint32_t* flags);
int         bplib_load          (bp_desc_t* desc, void** bundle, int* size, int timeout, uint32_t* flags);
int         bplib_process       (bp_desc_t* desc, void* bundle, int size, int timeout, uint32_t* flags);
int         bplib_accept        (bp_desc_t* desc, void** payload, int* size, int timeout, uint32_t* flags);

int         bplib_ackbundle     (bp_desc_t* desc, void* bundle);
int         bplib_ackpayload    (bp_desc_t* desc, void* payload);

int         bplib_routeinfo     (void* bundle, int size, bp_route_t* route);
int         bplib_display       (void* bundle, int size, uint32_t* flags);
int         bplib_eid2ipn       (const char* eid, int len, bp_ipn_t* node, bp_ipn_t* service);
int         bplib_ipn2eid       (char* eid, int len, bp_ipn_t node, bp_ipn_t service);
int         bplib_attrinit      (bp_attr_t* attributes);

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /* _bplib_h_ */
