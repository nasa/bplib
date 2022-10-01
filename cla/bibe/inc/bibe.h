/************************************************************************
 * File: bibe.h
 *
 * This software was created at NASA's Goddard Space Flight Center.
 *
 * Maintainer(s):
 *  Karl Sipfle, for Code 580 NASA GSFC
 *
 * Copyright © 2020-2022 United States Government as represented by 
 * the Administrator of
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
 *************************************************************************/

int setup_cla_bibe(bplib_routetbl_t *rtbl, uint16_t local_port, uint16_t remote_port);
int teardown_cla_bibe();

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
int bplib_bibe_ingress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, const void *bundle, size_t size, uint32_t timeout);

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
int bplib_bibe_egress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, void *bundle, size_t *size, uint32_t timeout);
