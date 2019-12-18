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

/* Handles and Descriptors */
#define BP_INVALID_HANDLE               (-1)    /* used for integers (os locks, storage services) */
#define BP_INVALID_DESCRIPTOR           NULL    /* used for pointers (channels) */
    
/* Timeouts */
#define BP_PEND                         (-1)
#define BP_CHECK                        0

/* Endpoint IDs */    
#define BP_MAX_EID_STRING               128
#define BP_IPN_NULL                     0
    
/* Storage IDs */
#define BP_SID_VACANT                   0

/* Return Codes */
#define BP_DEBUG                        2
#define BP_SUCCESS                      1
#define BP_TIMEOUT                      0
#define BP_ERROR                        (-1)
#define BP_PARMERR                      (-2)
#define BP_UNSUPPORTED                  (-3)
#define BP_EXPIRED                      (-4)
#define BP_DROPPED                      (-5)
#define BP_INVALIDHANDLE                (-6)
#define BP_WRONGVERSION                 (-9)
#define BP_BUNDLEPARSEERR               (-10)
#define BP_UNKNOWNREC                   (-11)
#define BP_BUNDLETOOLARGE               (-12)
#define BP_WRONGCHANNEL                 (-13)
#define BP_FAILEDINTEGRITYCHECK         (-14)
#define BP_FAILEDSTORE                  (-15)
#define BP_FAILEDOS                     (-16)
#define BP_FAILEDMEM                    (-17)
#define BP_FAILEDRESPONSE               (-18)
#define BP_INVALIDEID                   (-19)
#define BP_INVALIDCIPHERSUITEID         (-20)
#define BP_DUPLICATECID                 (-21)
#define BP_CUSTODYTREEFULL              (-22)
#define BP_ACTIVETABLEFULL              (-23)
#define BP_CIDNOTFOUND                  (-24)
#define BP_PENDINGACKNOWLEDGMENT        (-25)
#define BP_PENDINGFORWARD               (-26)
#define BP_PENDINGACCEPTANCE            (-27)
#define BP_PENDINGAPPLICATION           (-28)

/* Processing, Acceptance, and Load Flags */
#define BP_FLAG_NONCOMPLIANT            0x0001  /* valid bundle but agent not able to comply with standard */
#define BP_FLAG_INCOMPLETE              0x0002  /* block in bundle was not recognized */
#define BP_FLAG_UNRELIABLETIME          0x0004  /* the os call to get time return a suspicious value */
#define BP_FLAG_FILLOVERFLOW            0x0008  /* a gap in the CIDs exceeds the max fill */
#define BP_FLAG_TOOMANYFILLS            0x0010  /* all the fills in the ACS are used */
#define BP_FLAG_CIDWENTBACKWARDS        0x0020  /* the custody ID went backwards */
#define BP_FLAG_ROUTENEEDED             0x0040  /* the bundle returned should be routed before transmission */
#define BP_FLAG_STOREFAILURE            0x0080  /* storage service failed to deliver data */
#define BP_FLAG_UNKNOWNCID              0x0100  /* received CID in acknowledgment for which no bundle was found */
#define BP_FLAG_SDNVOVERFLOW            0x0200  /* insufficient room in variable to read/write value */
#define BP_FLAG_SDNVINCOMPLETE          0x0400  /* insufficient room in block to read/write value */
#define BP_FLAG_ACTIVETABLEWRAP         0x0800  /* the active table wrapped */
#define BP_FLAG_DUPLICATES              0x1000  /* multiple bundles on the network have the same custody id */
#define BP_FLAG_RBTREEFULL              0x2000  /* the dacs rb_tree was full */

/* Class of Service */
#define BP_COS_BULK                     0
#define BP_COS_NORMAL                   1
#define BP_COS_EXPEDITED                2
#define BP_COS_EXTENDED                 3

/* Bundle Integrity Types */
#define BP_BIB_NONE                     0
#define BP_BIB_CRC16_X25                1
#define BP_BIB_CRC32_CASTAGNOLI         2

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
typedef void* bp_desc_t;

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
typedef void* bp_sid_t;

/* Storage Object */
typedef struct {
    int         handle;
    bp_sid_t    sid;
    int         size;
    char        data[];
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

bp_desc_t   bplib_open          (bp_route_t route, bp_store_t store, bp_attr_t attributes);
void        bplib_close         (bp_desc_t channel);

int         bplib_flush         (bp_desc_t channel);
int         bplib_config        (bp_desc_t channel, int mode, int opt, int* val);
int         bplib_latchstats    (bp_desc_t channel, bp_stats_t* stats);

int         bplib_store         (bp_desc_t channel, void* payload, int size, int timeout, uint16_t* flags);
int         bplib_load          (bp_desc_t channel, void** bundle, int* size, int timeout, uint16_t* flags); 
int         bplib_process       (bp_desc_t channel, void* bundle, int size, int timeout, uint16_t* flags);
int         bplib_accept        (bp_desc_t channel, void** payload, int* size, int timeout, uint16_t* flags);

int         bplib_ackbundle     (bp_desc_t channel, void* bundle);
int         bplib_ackpayload    (bp_desc_t channel, void* payload);

int         bplib_routeinfo     (void* bundle, int size, bp_route_t* route);
int         bplib_display       (void* bundle, int size, uint16_t* flags);
int         bplib_eid2ipn       (const char* eid, int len, bp_ipn_t* node, bp_ipn_t* service);
int         bplib_ipn2eid       (char* eid, int len, bp_ipn_t node, bp_ipn_t service);
int         bplib_attrinit      (bp_attr_t* attributes);

#ifdef __cplusplus
} // extern "C"
#endif 

#endif  /* _bplib_h_ */
