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

#ifndef BPLIB_H
#define BPLIB_H

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

/* Event Flags */
#define BP_FLAG_DIAGNOSTIC              0x00000000
#define BP_FLAG_NONCOMPLIANT            0x00000001 /* valid bundle but agent not able to comply with standard */
#define BP_FLAG_INCOMPLETE              0x00000002 /* block in bundle was not recognized */
#define BP_FLAG_UNRELIABLE_TIME         0x00000004 /* the os call to get time return a suspicious value */
#define BP_FLAG_DROPPED                 0x00000008 /* bundle dropped because a required extension block could not be processed */
#define BP_FLAG_FAILED_INTEGRITY_CHECK  0x00000010 /* bundle with BIB failed the integrity check on the payload */
#define BP_FLAG_BUNDLE_TOO_LARGE        0x00000020 /* size of bundle exceeds capacity allowed by library */
#define BP_FLAG_ROUTE_NEEDED            0x00000040 /* the bundle returned should be routed before transmission */
#define BP_FLAG_STORE_FAILURE           0x00000080 /* storage service failed to deliver data */
#define BP_FLAG_UNKNOWN_CID             0x00000100 /* received CID in acknowledgment for which no bundle was found */
#define BP_FLAG_SDNV_OVERFLOW           0x00000200 /* insufficient room in variable to read/write value */
#define BP_FLAG_SDNV_INCOMPLETE         0x00000400 /* insufficient room in block to read/write value */
#define BP_FLAG_ACTIVE_TABLE_WRAP       0x00000800 /* the active table wrapped */
#define BP_FLAG_DUPLICATES              0x00001000 /* multiple bundles on the network have the same custody id */
#define BP_FLAG_CUSTODY_FULL            0x00002000 /* the dacs rb_tree was full */
#define BP_FLAG_UNKNOWNREC              0x00004000 /* bundle contained unknown adminstrative record */
#define BP_FLAG_INVALID_CIPHER_SUITEID  0x00008000 /* invalid cipher suite ID found in BIB */
#define BP_FLAG_INVALID_BIB_RESULT_TYPE 0x00010000 /* invalid result type found in BIB */
#define BP_FLAG_INVALID_BIB_TARGET_TYPE 0x00020000 /* invalid target type found in BIB */
#define BP_FLAG_FAILED_TO_PARSE         0x00040000 /* unable to parse bundle due to internal inconsistencies in bundle */
#define BP_FLAG_API_ERROR               0x00080000 /* calling code incorrectly used library */
#define BP_FLAG_OUT_OF_MEMORY           0x00100000 /* memory pool exhausted */
#define BP_FLAG_UNIMPLEMENTED           0x80000000 /* request requires a feature not implemented in BPLib */

/* Timeouts */
#define BP_PEND  (-1)
#define BP_CHECK 0

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

int  bplib_init(void);
void bplib_deinit(void);

/*
 * These calls were from the previous version of BPLib, and are not all implemented
 * yet.  See replacements below
 */

bp_desc_t *bplib_open(bp_route_t route, bp_store_t store, bp_attr_t attributes);
void       bplib_close(bp_desc_t *desc);

int bplib_flush(bp_desc_t *desc);
int bplib_config(bp_desc_t *desc, int mode, int opt, int *val);
int bplib_latchstats(bp_desc_t *desc, bp_stats_t *stats);

int bplib_store(bp_desc_t *desc, const void *payload, size_t size, int timeout, uint32_t *flags);
int bplib_load(bp_desc_t *desc, const void **bundle, size_t *size, int timeout, uint32_t *flags);
int bplib_process(bp_desc_t *desc, const void *bundle, size_t size, int timeout, uint32_t *flags);
int bplib_accept(bp_desc_t *desc, const void **payload, size_t *size, int timeout, uint32_t *flags);

int bplib_ackbundle(bp_desc_t *desc, const void *bundle);
int bplib_ackpayload(bp_desc_t *desc, const void *payload);

int bplib_routeinfo(const void *bundle, size_t size, bp_route_t *route);
int bplib_display(const void *bundle, size_t size, uint32_t *flags);
int bplib_eid2ipn(const char *eid, size_t len, bp_ipn_t *node, bp_ipn_t *service);
int bplib_ipn2eid(char *eid, size_t len, bp_ipn_t node, bp_ipn_t service);
int bplib_attrinit(bp_attr_t *attributes);

/* The following calls are specific to the new (BPv7) implementation */

/**
 * @brief Creates/opens a socket-like entity for application data
 *
 * The returned object can then be further configured.  This allows
 * for exchanging payload data units.
 *
 * @param rtbl Routing table instance
 * @return Pointer to socket-like object
 */
bp_socket_t *bplib_create_socket(bplib_routetbl_t *rtbl);

/**
 * @brief Closes/Deletes the socket-like entity for application data
 *
 * The socket object is unconfigured, and may no longer be used for
 * passing payload data unuts
 *
 * @param desc Socket-like object
 */

void bplib_close_socket(bp_socket_t *desc);

/**
 * @brief Logically binds the socket-like entity to a local IPN node number and service
 *
 * This specifies which local node and service the socket should be bound to,
 * and this in turn sets the "source" EID in the bundles that get generated
 * from this interface.
 *
 * @param desc Socket-like object
 * @param source_ipn Local IPN address to bind to
 * @retval BP_SUCCESS if successful
 */
int bplib_bind_socket(bp_socket_t *desc, const bp_ipn_addr_t *source_ipn);

/**
 * @brief Logically connects the socket-like entity to a remote IPN node number and service
 *
 * This specifies which destination node and service that the socket should send to,
 * and this in turn sets the "desination" EID in the bundles that get generated
 * from this interface.
 *
 * @param desc Socket-like object
 * @param destination_ipn Remote IPN address to connect to
 * @retval BP_SUCCESS if successful
 */
int bplib_connect_socket(bp_socket_t *desc, const bp_ipn_addr_t *destination_ipn);

/**
 * @brief Creates a RAM storage (cache) logical entity
 *
 * The RAM storage accepts any bundles routed to it, and will keep them in cache.
 * Bundles can then be (re-)transmitted from the cache based on the retransmit
 * interval, or may expire from here if the lifetime runs out before a
 * successful transfer.
 *
 * Bundles stored using this entity will get a "previous node" extension
 * block reflecting this IPN address (ipn_addr) when the bundle is passed on.
 *
 * @note This creates a separate storage entity from the data interfaces and as
 * such has a separate IPN node number.  This configuration is appropriate for
 * relay nodes, but an entity like this can also be created in endpoint nodes
 * as well for testing and debug purposes.
 *
 * @param rtbl Routing table instance
 * @param ipn_addr IPN address of this entity
 * @return bp_handle_t value referring to this entity
 */
bp_handle_t bplib_create_ram_storage(bplib_routetbl_t *rtbl, const bp_ipn_addr_t *ipn_addr);

/**
 * @brief Creates a CLA (bundle data unit) logical entity
 *
 * A CLA interface allows the host application to pass full bundles into and out of the
 * system.  This differs from an application data entity, in that the data units here
 * are full bundles, as opposed to payloads.
 *
 * This entity does not have a separate IPN address/node number.
 *
 * @param rtbl Routing table instance
 * @return bp_handle_t value referring to this entity
 */
bp_handle_t bplib_create_cla_intf(bplib_routetbl_t *rtbl);

/**
 * @brief Creates a basic data-passing logical entity
 *
 * This entity acts as the base interface for routing of local data.  It should
 * be configured with the local IPN node number, and all bundles with the destination
 * endpoint node number matching this value will be routed to this entity.  Application
 * data sockets can then be created over this base interface for individual data services.
 *
 * @param rtbl Routing table instance
 * @param node_num IPN node number value
 * @return bp_handle_t value referring to this entity
 */
bp_handle_t bplib_create_node_intf(bplib_routetbl_t *rtbl, bp_ipn_t node_num);

/* App I/O (payload data units) */

/**
 * @brief Send a single application PDU/datagram over the socket-like interface
 *
 * Sends a single datagram via the data socket.  The data will be bundled and
 * routed through the system accordingly.
 *
 * @note the socket-like object must be bound and connected before this API
 * can be used.
 *
 * @param desc Socket-like object from bplib_create_socket()
 * @param payload Pointer to buffer containing application PDU/datagram
 * @param size Size of application PDU/datagram
 * @param timeout Timeout (not yet implemented)
 * @param flags Not used (carryover)
 * @retval BP_SUCCESS if successful
 */
int bplib_send(bp_socket_t *desc, const void *payload, size_t size, int timeout, uint32_t *flags);

/**
 * @brief Receive a single application PDU/datagram over the socket-like interface
 *
 * Receives a single datagram via the data socket.
 * Initially, as an input, the caller should set "size" to the maximum size of the buffer.
 * As an output, this function will set the "size" to the actual size of the PDU.
 *
 * @note the socket-like object must be bound and connected before this API
 * can be used.
 *
 * @param desc Socket-like object from bplib_create_socket()
 * @param payload Pointer to buffer to store application PDU/datagram
 * @param[inout] size Size of buffer on input, size of actual PDU/datagram on output
 * @param timeout Timeout (not yet implemented)
 * @param flags Not used (carryover)
 * @retval BP_SUCCESS if successful
 */
int bplib_recv(bp_socket_t *desc, void *payload, size_t *size, int timeout, uint32_t *flags);

/* CLA I/O (bundle data units) */

/**
 * @brief Receive complete bundle from a remote system
 *
 * This implements the "ingress" side of the CLA API.  Complete RFC 9171-compliant, CBOR-encoded bundles
 * are passed in, which will be parsed by BPLib to identify the internal fields.  The bundle will then
 * be forwarded internally through the routing table to either its destination (if local) or via a relay
 * storage (if configured) or to another CLA.
 *
 * @param rtbl Routing table instance
 * @param intf_id bp_handle_t value from bplib_create_cla_intf()
 * @param bundle Pointer to bundle buffer
 * @param size Size of encoded bundle
 * @param timeout Timeout (not yet implemented)
 * @retval BP_SUCCESS if successful
 */
int bplib_cla_ingress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, const void *bundle, size_t size, int timeout);

/**
 * @brief Send complete bundle to remote system
 *
 * Retrieve the next complete RFC 9171-compliant, CBOR-encoded bundle from the local system, which was routed to the
 * specified CLA interface.  This data may then be forwarded to the CLA implementation for actual transmission to
 * a remote node.
 *
 * @param rtbl Routing table instance
 * @param intf_id bp_handle_t value from bplib_create_cla_intf()
 * @param bundle Pointer to bundle buffer
 * @param[inout] size Size of buffer on input, size of actual bundle on output
 * @param timeout Timeout (not yet implemented)
 * @retval BP_SUCCESS if successful
 */
int bplib_cla_egress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, void *bundle, size_t *size, int timeout);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_H */
