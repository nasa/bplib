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

#define BP_DTNTIME_INFINITE UINT64_MAX

/* Endpoint IDs */
#define BP_MAX_EID_STRING 128
#define BP_IPN_NULL       0

/* Class of Service */
#define BP_COS_BULK      0
#define BP_COS_NORMAL    1
#define BP_COS_EXPEDITED 2
#define BP_COS_EXTENDED  3

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int  bplib_init(void);
void bplib_deinit(void);

int bplib_display(const void *bundle, size_t size, uint32_t *flags);
int bplib_eid2ipn(const char *eid, size_t len, bp_ipn_t *node, bp_ipn_t *service);
int bplib_ipn2eid(char *eid, size_t len, bp_ipn_t node, bp_ipn_t service);

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
 * block reflecting this IPN address (storage_addr) when the bundle is passed on.
 *
 * @note This creates a separate storage entity from the data interfaces and as
 * such has a separate IPN node number.  This configuration is appropriate for
 * relay nodes, but an entity like this can also be created in endpoint nodes
 * as well for testing and debug purposes.
 *
 * @param rtbl Routing table instance
 * @param storage_addr IPN address of this entity
 * @return bp_handle_t value referring to this entity
 */
bp_handle_t bplib_create_ram_storage(bplib_routetbl_t *rtbl, const bp_ipn_addr_t *storage_addr);
bp_handle_t bplib_create_file_storage(bplib_routetbl_t *rtbl, const bp_ipn_addr_t *storage_addr);

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
 * @param timeout Timeout
 * @retval BP_SUCCESS if successful
 */
int bplib_send(bp_socket_t *desc, const void *payload, size_t size, uint32_t timeout);

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
 * @param timeout Timeout
 * @retval BP_SUCCESS if successful
 */
int bplib_recv(bp_socket_t *desc, void *payload, size_t *size, uint32_t timeout);

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
 * @param timeout Timeout
 * @retval BP_SUCCESS if successful
 */
int bplib_cla_ingress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, const void *bundle, size_t size, uint32_t timeout);

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
 * @param timeout Timeout
 * @retval BP_SUCCESS if successful
 */
int bplib_cla_egress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, void *bundle, size_t *size, uint32_t timeout);

/**
 * @brief Get an operational value
 *
 * Reads the current value of a single bplib operational variable.  For interface variables (such as bundle/byte counts)
 * the intf_id specifies the scope to read.  For global scope variables, the intf_id should be passed as
 * BP_INVALID_HANDLE.
 *
 * @param rtbl Routing table instance
 * @param intf_id the interface (scope) to read, or BP_INVALID_HANDLE for global scope
 * @param var_id the specific variable value to read
 * @param[out] value the value to set
 *
 * @retval BP_SUCCESS if successfully read
 */
int bplib_query_integer(bplib_routetbl_t *rtbl, bp_handle_t intf_id, bplib_variable_t var_id, bp_sval_t *value);

/**
 * @brief Set an operational value
 *
 * Writes the value of a single bplib operational variable.  For interface variables (such as bundle/byte counts) the
 * intf_id specifies the scope to write.  For global scope variables, the intf_id should be passed as BP_INVALID_HANDLE.
 *
 * @param rtbl Routing table instance
 * @param intf_id the interface (scope) to write, or BP_INVALID_HANDLE for global scope
 * @param var_id the specific variable value to write
 * @param value the value to set
 *
 * @retval BP_SUCCESS if successfully written
 */
int bplib_config_integer(bplib_routetbl_t *rtbl, bp_handle_t intf_id, bplib_variable_t var_id, bp_sval_t value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* BPLIB_H */
